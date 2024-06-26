// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <GLES2/gl2.h>

#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"
#include "gpu/command_buffer/tests/gl_manager.h"
#include "gpu/command_buffer/tests/gl_test_utils.h"
#include "gpu/config/gpu_switches.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {

namespace {
    const GLenum kCubeMapTextureTargets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };
} // namespace

// A collection of tests that exercise the cube map texture.
class GLCubeMapTextureTest : public testing::TestWithParam<GLenum> {
protected:
    void SetUp() override
    {
        base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
        // ANGLE and NVidia fails ReadPixelsFromIncompleteCubeTexture without this
        // workaround.
        command_line.AppendSwitchASCII(switches::kGpuDriverBugWorkarounds,
            base::IntToString(gpu::FORCE_CUBE_COMPLETE));
        gl_.InitializeWithCommandLine(GLManager::Options(), &command_line);
        DCHECK(gl_.workarounds().force_cube_complete);
        for (int i = 0; i < 256; i++) {
            pixels_[i * 4] = 255u;
            pixels_[(i * 4) + 1] = 0;
            pixels_[(i * 4) + 2] = 0;
            pixels_[(i * 4) + 3] = 255u;
        }

        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
        EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

        glGenFramebuffers(1, &framebuffer_id_);
    }

    void TearDown() override
    {
        glDeleteTextures(1, &texture_);
        glDeleteFramebuffers(1, &framebuffer_id_);
        gl_.Destroy();
    }

    GLManager gl_;
    uint8 pixels_[256 * 4];
    const int width_ = 16;
    GLuint texture_;
    GLuint framebuffer_id_;
};

INSTANTIATE_TEST_CASE_P(GLCubeMapTextureTests,
    GLCubeMapTextureTest,
    ::testing::ValuesIn(kCubeMapTextureTargets));

TEST_P(GLCubeMapTextureTest, TexImage2DAfterFBOBinding)
{
    GLenum cube_map_target = GetParam();

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cube_map_target,
        texture_, 0);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
    // force_gl_finish_after_compositing workaround prevents Nexus 5 crash.
    // TODO(dshwang): remove the workaround when it's fixed. crbug.com/518889
    glTexImage2D(cube_map_target, 0, GL_RGBA, width_, width_, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, pixels_);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
}

TEST_P(GLCubeMapTextureTest, ReadPixels)
{
    GLenum cube_map_target = GetParam();

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Make a cube texture complete
    for (unsigned i = 0; i < arraysize(kCubeMapTextureTargets); i++) {
        glTexImage2D(kCubeMapTextureTargets[i], 0, GL_RGBA, width_, width_, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, pixels_);
        EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cube_map_target,
        texture_, 0);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Check that FB is complete.
    EXPECT_EQ(static_cast<GLenum>(GL_FRAMEBUFFER_COMPLETE),
        glCheckFramebufferStatus(GL_FRAMEBUFFER));

    GLTestHelper::CheckPixels(0, 0, width_, width_, 0, pixels_);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
}

TEST_P(GLCubeMapTextureTest, ReadPixelsFromIncompleteCubeTexture)
{
    GLenum cube_map_target = GetParam();

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    glTexImage2D(cube_map_target, 0, GL_RGBA, width_, width_, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, pixels_);
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cube_map_target,
        texture_, 0);

    // force_gl_finish_after_compositing workaround prevents Nexus 5 crash.
    // TODO(dshwang): remove the workaround when it's fixed. crbug.com/518889
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());

    // Check that FB is complete.
    EXPECT_EQ(static_cast<GLenum>(GL_FRAMEBUFFER_COMPLETE),
        glCheckFramebufferStatus(GL_FRAMEBUFFER));
#if defined(OS_ANDROID)
    // No way to workaround on Android NVIDIA drivers. Users have to texImage2D
    // by (POSITIVE_X, NEGATIVE_Z) order only once. If users call texImage2D again
    // after defining all faces, glReadPixels fails.
    GLsizei size = width_ * width_ * 4;
    scoped_ptr<uint8[]> pixels(new uint8[size]);
    glReadPixels(0, 0, width_, width_, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
#else
    // Without force_cube_complete workaround,
    // 1. ANGLE crashes on glReadPixels() from incomplete cube texture.
    // 2. NVidia fails on glReadPixels() from incomplete cube texture.
    GLTestHelper::CheckPixels(0, 0, width_, width_, 0, pixels_);
#endif
    EXPECT_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
}

} // namespace gpu
