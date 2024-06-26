// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>
#include <cmath>

#include "base/command_line.h"
#include "gpu/command_buffer/service/gpu_switches.h"
#include "gpu/command_buffer/tests/gl_manager.h"
#include "gpu/command_buffer/tests/gl_test_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define SHADER(Src) #Src

namespace gpu {

class CHROMIUMPathRenderingTest : public testing::Test {
public:
    static const GLsizei kResolution = 100;

protected:
    void SetUp() override
    {
        GLManager::Options options;
        options.size = gfx::Size(kResolution, kResolution);
        base::CommandLine command_line(*base::CommandLine::ForCurrentProcess());
        command_line.AppendSwitch(switches::kEnableGLPathRendering);
        gl_.InitializeWithCommandLine(options, &command_line);
    }

    void TearDown() override { gl_.Destroy(); }

    void ExpectEqualMatrix(const GLfloat* expected, const GLfloat* actual)
    {
        for (size_t i = 0; i < 16; ++i) {
            EXPECT_EQ(expected[i], actual[i]);
        }
    }
    void ExpectEqualMatrix(const GLfloat* expected, const GLint* actual)
    {
        for (size_t i = 0; i < 16; ++i) {
            EXPECT_EQ(static_cast<GLint>(round(expected[i])), actual[i]);
        }
    }

    void SetupStateForTestPattern()
    {
        glViewport(0, 0, kResolution, kResolution);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glStencilMask(0xffffffff);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        static const char* kVertexShaderSource = SHADER(void main() { gl_Position = vec4(1); });
        static const char* kFragmentShaderSource = SHADER(precision mediump float; uniform vec4 color;
                                                          void main() { gl_FragColor = color; });

        GLuint program = GLTestHelper::LoadProgram(kVertexShaderSource, kFragmentShaderSource);
        glUseProgram(program);
        color_loc_ = glGetUniformLocation(program, "color");
        glDeleteProgram(program);

        // Set up orthogonal projection with near/far plane distance of 2.
        static GLfloat matrix[16] = { 2.0f / (kResolution - 1),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            2.0f / (kResolution - 1),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            -1.0f,
            0.0f,
            -1.0f,
            -1.0f,
            0.0f,
            1.0f };
        glMatrixLoadfCHROMIUM(GL_PATH_PROJECTION_CHROMIUM, matrix);
        glMatrixLoadIdentityCHROMIUM(GL_PATH_MODELVIEW_CHROMIUM);

        glEnable(GL_STENCIL_TEST);

        GLTestHelper::CheckGLError("no errors at state setup", __LINE__);
    }

    void SetupPathStateForTestPattern(GLuint path)
    {
        static const GLubyte kCommands[] = { GL_MOVE_TO_CHROMIUM,
            GL_LINE_TO_CHROMIUM,
            GL_QUADRATIC_CURVE_TO_CHROMIUM,
            GL_CUBIC_CURVE_TO_CHROMIUM,
            GL_CLOSE_PATH_CHROMIUM };

        static const GLfloat kCoords[] = { 50.0f,
            50.0f,
            75.0f,
            75.0f,
            100.0f,
            62.5f,
            50.0f,
            25.5f,
            0.0f,
            62.5f,
            50.0f,
            50.0f,
            25.0f,
            75.0f };

        glPathCommandsCHROMIUM(path, arraysize(kCommands), kCommands,
            arraysize(kCoords), GL_FLOAT, kCoords);

        glPathParameterfCHROMIUM(path, GL_PATH_STROKE_WIDTH_CHROMIUM, 5.0f);
        glPathParameterfCHROMIUM(path, GL_PATH_MITER_LIMIT_CHROMIUM, 1.0f);
        glPathParameterfCHROMIUM(path, GL_PATH_STROKE_BOUND_CHROMIUM, .02f);
        glPathParameteriCHROMIUM(path, GL_PATH_JOIN_STYLE_CHROMIUM,
            GL_ROUND_CHROMIUM);
        glPathParameteriCHROMIUM(path, GL_PATH_END_CAPS_CHROMIUM,
            GL_SQUARE_CHROMIUM);
    }

    void VerifyTestPatternFill(float x, float y)
    {
        static const float kFillCoords[] = {
            55.0f, 55.0f, 50.0f, 28.0f, 66.0f, 63.0f
        };
        static const uint8 kBlue[] = { 0, 0, 255, 255 };

        for (size_t i = 0; i < arraysize(kFillCoords); i += 2) {
            float fx = kFillCoords[i];
            float fy = kFillCoords[i + 1];

            EXPECT_TRUE(GLTestHelper::CheckPixels(x + fx, y + fy, 1, 1, 0, kBlue));
        }
    }

    void VerifyTestPatternBg(float x, float y)
    {
        const float kBackgroundCoords[] = { 80.0f, 80.0f, 20.0f, 20.0f, 90.0f, 1.0f };
        const uint8 kExpectedColor[] = { 0, 0, 0, 0 };

        for (size_t i = 0; i < arraysize(kBackgroundCoords); i += 2) {
            float bx = kBackgroundCoords[i];
            float by = kBackgroundCoords[i + 1];

            EXPECT_TRUE(
                GLTestHelper::CheckPixels(x + bx, y + by, 1, 1, 0, kExpectedColor));
        }
    }

    void VerifyTestPatternStroke(float x, float y)
    {
        // Inside the stroke we should have green.
        const uint8 kGreen[] = { 0, 255, 0, 255 };
        EXPECT_TRUE(GLTestHelper::CheckPixels(x + 50, y + 53, 1, 1, 0, kGreen));
        EXPECT_TRUE(GLTestHelper::CheckPixels(x + 26, y + 76, 1, 1, 0, kGreen));

        // Outside the path we should have black.
        const uint8 black[] = { 0, 0, 0, 0 };
        EXPECT_TRUE(GLTestHelper::CheckPixels(x + 10, y + 10, 1, 1, 0, black));
        EXPECT_TRUE(GLTestHelper::CheckPixels(x + 80, y + 80, 1, 1, 0, black));
    }

    void TryAllDrawFunctions(GLuint path, GLenum expected_error)
    {
        glStencilFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F);
        EXPECT_EQ(expected_error, glGetError());

        glStencilFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F);
        EXPECT_EQ(expected_error, glGetError());

        glStencilStrokePathCHROMIUM(path, 0x80, 0x80);
        EXPECT_EQ(expected_error, glGetError());

        glCoverFillPathCHROMIUM(path, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(expected_error, glGetError());

        glCoverStrokePathCHROMIUM(path, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(expected_error, glGetError());

        glStencilThenCoverStrokePathCHROMIUM(path, 0x80, 0x80,
            GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(expected_error, glGetError());

        glStencilThenCoverFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F,
            GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(expected_error, glGetError());
    }

    GLManager gl_;
    GLint color_loc_;
};

TEST_F(CHROMIUMPathRenderingTest, TestMatrix)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    static const GLfloat kIdentityMatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    static const GLfloat kSeqMatrix[16] = {
        0.5f, -0.5f, -0.1f, -0.8f, 4.4f, 5.5f, 6.6f, 7.7f,
        8.8f, 9.9f, 10.11f, 11.22f, 12.33f, 13.44f, 14.55f, 15.66f
    };
    static const GLenum kMatrixModes[] = { GL_PATH_MODELVIEW_CHROMIUM,
        GL_PATH_PROJECTION_CHROMIUM };
    static const GLenum kGetMatrixModes[] = { GL_PATH_MODELVIEW_MATRIX_CHROMIUM,
        GL_PATH_PROJECTION_MATRIX_CHROMIUM };

    for (size_t i = 0; i < arraysize(kMatrixModes); ++i) {
        GLfloat mf[16];
        GLint mi[16];
        memset(mf, 0, sizeof(mf));
        memset(mi, 0, sizeof(mi));
        glGetFloatv(kGetMatrixModes[i], mf);
        glGetIntegerv(kGetMatrixModes[i], mi);
        ExpectEqualMatrix(kIdentityMatrix, mf);
        ExpectEqualMatrix(kIdentityMatrix, mi);

        glMatrixLoadfCHROMIUM(kMatrixModes[i], kSeqMatrix);
        memset(mf, 0, sizeof(mf));
        memset(mi, 0, sizeof(mi));
        glGetFloatv(kGetMatrixModes[i], mf);
        glGetIntegerv(kGetMatrixModes[i], mi);
        ExpectEqualMatrix(kSeqMatrix, mf);
        ExpectEqualMatrix(kSeqMatrix, mi);

        glMatrixLoadIdentityCHROMIUM(kMatrixModes[i]);
        memset(mf, 0, sizeof(mf));
        memset(mi, 0, sizeof(mi));
        glGetFloatv(kGetMatrixModes[i], mf);
        glGetIntegerv(kGetMatrixModes[i], mi);
        ExpectEqualMatrix(kIdentityMatrix, mf);
        ExpectEqualMatrix(kIdentityMatrix, mi);

        EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    }
}

TEST_F(CHROMIUMPathRenderingTest, TestMatrixErrors)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    GLfloat mf[16];
    memset(mf, 0, sizeof(mf));

    glMatrixLoadfCHROMIUM(GL_PATH_MODELVIEW_CHROMIUM, mf);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    glMatrixLoadIdentityCHROMIUM(GL_PATH_PROJECTION_CHROMIUM);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Test that invalid matrix targets fail.
    glMatrixLoadfCHROMIUM(GL_PATH_MODELVIEW_CHROMIUM - 1, mf);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());

    // Test that invalid matrix targets fail.
    glMatrixLoadIdentityCHROMIUM(GL_PATH_PROJECTION_CHROMIUM + 1);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());
}

TEST_F(CHROMIUMPathRenderingTest, TestSimpleCalls)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    // This is unspecified in NV_path_rendering.
    EXPECT_EQ(0u, glGenPathsCHROMIUM(0));

    GLuint path = glGenPathsCHROMIUM(1);
    EXPECT_NE(path, 0u);
    glDeletePathsCHROMIUM(path, 1);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    GLuint first_path = glGenPathsCHROMIUM(5);
    EXPECT_NE(first_path, 0u);
    glDeletePathsCHROMIUM(first_path, 5);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Test deleting paths that are not actually allocated:
    // "unused names in /paths/ are silently ignored".
    first_path = glGenPathsCHROMIUM(5);
    EXPECT_NE(first_path, 0u);
    glDeletePathsCHROMIUM(first_path, 6);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    GLsizei big_range = 0xffff;
    // Setting big_range = std::numeric_limits<GLsizei>::max() should go through
    // too, as far as NV_path_rendering is concerned. Current chromium side id
    // allocator will use too much memory.
    first_path = glGenPathsCHROMIUM(big_range);
    EXPECT_NE(first_path, 0u);
    glDeletePathsCHROMIUM(first_path, big_range);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Test glIsPathCHROMIUM().
    path = glGenPathsCHROMIUM(1);
    EXPECT_FALSE(glIsPathCHROMIUM(path));
    GLubyte commands[] = { GL_MOVE_TO_CHROMIUM, GL_CLOSE_PATH_CHROMIUM };
    GLfloat coords[] = { 50.0f, 50.0f };
    glPathCommandsCHROMIUM(path, arraysize(commands), commands, arraysize(coords),
        GL_FLOAT, coords);
    EXPECT_TRUE(glIsPathCHROMIUM(path));
    glDeletePathsCHROMIUM(path, 1);
    EXPECT_FALSE(glIsPathCHROMIUM(path));
}

TEST_F(CHROMIUMPathRenderingTest, TestGenDeleteErrors)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    // GenPaths / DeletePaths tests.
    // std::numeric_limits<GLuint>::max() is wrong for GLsizei.
    GLuint first_path = glGenPathsCHROMIUM(std::numeric_limits<GLuint>::max());
    EXPECT_EQ(first_path, 0u);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    first_path = glGenPathsCHROMIUM(-1);
    EXPECT_EQ(first_path, 0u);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    glDeletePathsCHROMIUM(1, -5);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    first_path = glGenPathsCHROMIUM(-1);
    EXPECT_EQ(first_path, 0u);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    // Test that delete with first_id and range such that first_id + range
    // overflows the GLuint. Example:
    // Range is 0x7fffffff. First id is X. Last id will be X + 0x7ffffffe.
    // X = 0x80000001 would succeed, where as X = 0x80000002 would fail.
    // To get 0x80000002, we need to allocate first 0x7fffffff and then
    // 3 (0x80000000, 0x80000001 and 0x80000002).
    // While not guaranteed by the API, we expect the implementation
    // hands us deterministic ids.
    first_path = glGenPathsCHROMIUM(std::numeric_limits<GLsizei>::max());
    EXPECT_EQ(first_path, 1u);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    GLuint additional_paths = glGenPathsCHROMIUM(3);
    EXPECT_EQ(additional_paths,
        static_cast<GLuint>(std::numeric_limits<GLsizei>::max()) + 1u);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Test that passing a range so big that it would overflow client_id
    // + range - 1 check causes an error.
    glDeletePathsCHROMIUM(additional_paths + 2u,
        std::numeric_limits<GLsizei>::max());
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_OPERATION), glGetError());

    // Cleanup the above allocations. Also test that passing max value still
    // works.
    glDeletePathsCHROMIUM(1, std::numeric_limits<GLsizei>::max());
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    glDeletePathsCHROMIUM(std::numeric_limits<GLsizei>::max(),
        std::numeric_limits<GLsizei>::max());
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
}

TEST_F(CHROMIUMPathRenderingTest, TestPathParameterErrors)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    GLuint path = glGenPathsCHROMIUM(1);
    // PathParameter*: Wrong value for the pname should fail.
    glPathParameteriCHROMIUM(path, GL_PATH_JOIN_STYLE_CHROMIUM, GL_FLAT_CHROMIUM);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());
    glPathParameterfCHROMIUM(path, GL_PATH_END_CAPS_CHROMIUM,
        GL_MITER_REVERT_CHROMIUM);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    // PathParameter*: Wrong floating-point value should fail.
    glPathParameterfCHROMIUM(path, GL_PATH_STROKE_WIDTH_CHROMIUM, -0.1f);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());
    glPathParameterfCHROMIUM(path, GL_PATH_MITER_LIMIT_CHROMIUM,
        std::numeric_limits<float>::quiet_NaN());
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());
    glPathParameterfCHROMIUM(path, GL_PATH_MITER_LIMIT_CHROMIUM,
        std::numeric_limits<float>::infinity());
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    // PathParameter*: Wrong pname should fail.
    glPathParameteriCHROMIUM(path, GL_PATH_STROKE_WIDTH_CHROMIUM - 1, 5);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());
    glDeletePathsCHROMIUM(path, 1);
}

TEST_F(CHROMIUMPathRenderingTest, TestPathObjectState)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    glViewport(0, 0, kResolution, kResolution);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glStencilMask(0xffffffff);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPathStencilFuncCHROMIUM(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

    // Test that trying to draw non-existing paths does not produce errors or
    // results.
    GLuint non_existing_paths[] = { 0, 55, 74744 };
    for (auto& p : non_existing_paths) {
        EXPECT_FALSE(glIsPathCHROMIUM(p));
        EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
        TryAllDrawFunctions(p, GL_NO_ERROR);
    }

    // Path name marked as used but without path object state causes
    // a GL error upon any draw command.
    GLuint path = glGenPathsCHROMIUM(1);
    EXPECT_FALSE(glIsPathCHROMIUM(path));
    TryAllDrawFunctions(path, GL_INVALID_OPERATION);
    glDeletePathsCHROMIUM(path, 1);

    // Document a bit of an inconsistency: path name marked as used but without
    // path object state causes a GL error upon any draw command (tested above).
    // Path name that had path object state, but then was "cleared", still has a
    // path object state, even though the state is empty.
    path = glGenPathsCHROMIUM(1);
    EXPECT_FALSE(glIsPathCHROMIUM(path));
    GLubyte commands[] = { GL_MOVE_TO_CHROMIUM, GL_CLOSE_PATH_CHROMIUM };
    GLfloat coords[] = { 50.0f, 50.0f };
    glPathCommandsCHROMIUM(path, arraysize(commands), commands, arraysize(coords),
        GL_FLOAT, coords);
    EXPECT_TRUE(glIsPathCHROMIUM(path));
    glPathCommandsCHROMIUM(path, 0, NULL, 0, GL_FLOAT, NULL);
    EXPECT_TRUE(glIsPathCHROMIUM(path)); // The surprise.
    TryAllDrawFunctions(path, GL_NO_ERROR);
    glDeletePathsCHROMIUM(path, 1);

    // Document a bit of an inconsistency: "clearing" a used path name causes
    // path to acquire state.
    path = glGenPathsCHROMIUM(1);
    EXPECT_FALSE(glIsPathCHROMIUM(path));
    glPathCommandsCHROMIUM(path, 0, NULL, 0, GL_FLOAT, NULL);
    EXPECT_TRUE(glIsPathCHROMIUM(path)); // The surprise.
    glDeletePathsCHROMIUM(path, 1);

    // Make sure nothing got drawn by the drawing commands that should not produce
    // anything.
    const uint8 black[] = { 0, 0, 0, 0 };
    EXPECT_TRUE(
        GLTestHelper::CheckPixels(0, 0, kResolution, kResolution, 0, black));
}

TEST_F(CHROMIUMPathRenderingTest, TestUnnamedPathsErrors)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    // Unnamed paths: Trying to create a path object with non-existing path name
    // produces error.  (Not a error in real NV_path_rendering).
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    GLubyte commands[] = { GL_MOVE_TO_CHROMIUM, GL_CLOSE_PATH_CHROMIUM };
    GLfloat coords[] = { 50.0f, 50.0f };
    glPathCommandsCHROMIUM(555, arraysize(commands), commands, arraysize(coords),
        GL_FLOAT, coords);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_OPERATION), glGetError());

    // PathParameter*: Using non-existing path object produces error.
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    glPathParameterfCHROMIUM(555, GL_PATH_STROKE_WIDTH_CHROMIUM, 5.0f);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_OPERATION), glGetError());

    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    glPathParameteriCHROMIUM(555, GL_PATH_JOIN_STYLE_CHROMIUM, GL_ROUND_CHROMIUM);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_OPERATION), glGetError());
}

TEST_F(CHROMIUMPathRenderingTest, TestPathCommandsErrors)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    static const GLenum kInvalidCoordType = GL_NONE;

    GLuint path = glGenPathsCHROMIUM(1);
    GLubyte commands[] = { GL_MOVE_TO_CHROMIUM, GL_CLOSE_PATH_CHROMIUM };
    GLfloat coords[] = { 50.0f, 50.0f };

    glPathCommandsCHROMIUM(path, arraysize(commands), commands, -4, GL_FLOAT,
        coords);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    glPathCommandsCHROMIUM(path, -1, commands, arraysize(coords), GL_FLOAT,
        coords);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    glPathCommandsCHROMIUM(path, arraysize(commands), commands, arraysize(coords),
        kInvalidCoordType, coords);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());

    // These can not distinquish between the check that should fail them.
    // This should fail due to coord count * float size overflow.
    glPathCommandsCHROMIUM(path, arraysize(commands), commands,
        std::numeric_limits<GLsizei>::max(), GL_FLOAT, coords);
    // This should fail due to cmd count + coord count * short size.
    glPathCommandsCHROMIUM(path, arraysize(commands), commands,
        std::numeric_limits<GLsizei>::max(), GL_SHORT, coords);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_OPERATION), glGetError());

    glDeletePathsCHROMIUM(path, 1);
}

TEST_F(CHROMIUMPathRenderingTest, TestPathRenderingInvalidArgs)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    GLuint path = glGenPathsCHROMIUM(1);
    glPathCommandsCHROMIUM(path, 0, NULL, 0, GL_FLOAT, NULL);

    // Verify that normal calls work.
    glStencilFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    glStencilThenCoverFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F,
        GL_BOUNDING_BOX_CHROMIUM);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Using invalid fill mode causes INVALID_ENUM.
    glStencilFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM - 1, 0x7F);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());
    glStencilThenCoverFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM - 1, 0x7F,
        GL_BOUNDING_BOX_CHROMIUM);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());

    // Using invalid cover mode causes INVALID_ENUM.
    glCoverFillPathCHROMIUM(path, GL_CONVEX_HULL_CHROMIUM - 1);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());
    glStencilThenCoverFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F,
        GL_BOUNDING_BOX_CHROMIUM + 1);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_ENUM), glGetError());

    // Using mask+1 not being power of two causes INVALID_VALUE with up/down fill
    // mode.
    glStencilFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x40);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());
    glStencilThenCoverFillPathCHROMIUM(path, GL_COUNT_DOWN_CHROMIUM, 12,
        GL_BOUNDING_BOX_CHROMIUM);
    EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE), glGetError());

    glDeletePathsCHROMIUM(path, 1);
}

// Tests that drawing with CHROMIUM_path_rendering functions work.
TEST_F(CHROMIUMPathRenderingTest, TestPathRendering)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    static const float kBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    static const float kGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };

    SetupStateForTestPattern();

    GLuint path = glGenPathsCHROMIUM(1);
    SetupPathStateForTestPattern(path);

    // Do the stencil fill, cover fill, stencil stroke, cover stroke
    // in unconventional order:
    // 1) stencil the stroke in stencil high bit
    // 2) stencil the fill in low bits
    // 3) cover the fill
    // 4) cover the stroke
    // This is done to check that glPathStencilFunc works, eg the mask
    // goes through. Stencil func is not tested ATM, for simplicity.

    glPathStencilFuncCHROMIUM(GL_ALWAYS, 0, 0xFF);
    glStencilStrokePathCHROMIUM(path, 0x80, 0x80);

    glPathStencilFuncCHROMIUM(GL_ALWAYS, 0, 0x7F);
    glStencilFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F);

    glStencilFunc(GL_LESS, 0, 0x7F);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
    glUniform4fv(color_loc_, 1, kBlue);
    glCoverFillPathCHROMIUM(path, GL_BOUNDING_BOX_CHROMIUM);

    glStencilFunc(GL_EQUAL, 0x80, 0x80);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
    glUniform4fv(color_loc_, 1, kGreen);
    glCoverStrokePathCHROMIUM(path, GL_CONVEX_HULL_CHROMIUM);

    glDeletePathsCHROMIUM(path, 1);

    // Verify the image.
    VerifyTestPatternFill(0.0f, 0.0f);
    VerifyTestPatternBg(0.0f, 0.0f);
    VerifyTestPatternStroke(0.0f, 0.0f);
}

// Tests that drawing with CHROMIUM_path_rendering
// StencilThenCover{Stroke,Fill}Path functions work.
TEST_F(CHROMIUMPathRenderingTest, TestPathRenderingThenFunctions)
{
    if (!GLTestHelper::HasExtension("GL_CHROMIUM_path_rendering"))
        return;

    static float kBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    static float kGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };

    SetupStateForTestPattern();

    GLuint path = glGenPathsCHROMIUM(1);
    SetupPathStateForTestPattern(path);

    glPathStencilFuncCHROMIUM(GL_ALWAYS, 0, 0xFF);
    glStencilFunc(GL_EQUAL, 0x80, 0x80);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
    glUniform4fv(color_loc_, 1, kGreen);
    glStencilThenCoverStrokePathCHROMIUM(path, 0x80, 0x80,
        GL_BOUNDING_BOX_CHROMIUM);

    glPathStencilFuncCHROMIUM(GL_ALWAYS, 0, 0x7F);
    glStencilFunc(GL_LESS, 0, 0x7F);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
    glUniform4fv(color_loc_, 1, kBlue);
    glStencilThenCoverFillPathCHROMIUM(path, GL_COUNT_UP_CHROMIUM, 0x7F,
        GL_CONVEX_HULL_CHROMIUM);

    glDeletePathsCHROMIUM(path, 1);

    // Verify the image.
    VerifyTestPatternFill(0.0f, 0.0f);
    VerifyTestPatternBg(0.0f, 0.0f);
    VerifyTestPatternStroke(0.0f, 0.0f);
}

} // namespace gpu
