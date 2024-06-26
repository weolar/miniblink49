// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gles2_cmd_decoder.h"

#include "base/command_line.h"
#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder_unittest.h"
#include "gpu/command_buffer/service/gpu_switches.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_mock.h"

using ::gfx::MockGLInterface;
using ::testing::_;
using ::testing::Return;

namespace gpu {
namespace gles2 {

    // Class to use to test that functions which need feature flags or
    // extensions always return INVALID_OPERATION if the feature flags is not
    // enabled or extension is not present.
    class GLES2DecoderTestDisabledExtensions : public GLES2DecoderTest {
    public:
        GLES2DecoderTestDisabledExtensions() { }
    };
    INSTANTIATE_TEST_CASE_P(Service,
        GLES2DecoderTestDisabledExtensions,
        ::testing::Bool());

    TEST_P(GLES2DecoderTestDisabledExtensions, CHROMIUMPathRenderingDisabled)
    {
        const GLuint kClientPathId = 0;
        {
            cmds::MatrixLoadfCHROMIUMImmediate& cmd = *GetImmediateAs<cmds::MatrixLoadfCHROMIUMImmediate>();
            GLfloat temp[16] = {
                0,
            };
            cmd.Init(GL_PATH_MODELVIEW_CHROMIUM, temp);
            EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::MatrixLoadIdentityCHROMIUM cmd;
            cmd.Init(GL_PATH_PROJECTION_CHROMIUM);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::GenPathsCHROMIUM cmd;
            cmd.Init(0, 0);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::DeletePathsCHROMIUM cmd;
            cmd.Init(0, 0);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::IsPathCHROMIUM cmd;
            cmd.Init(kClientPathId, shared_memory_id_, shared_memory_offset_);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::PathCommandsCHROMIUM cmd;
            cmd.Init(kClientPathId, 0, 0, 0, 0, GL_FLOAT, 0, 0);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::PathParameterfCHROMIUM cmd;
            cmd.Init(kClientPathId, GL_PATH_STROKE_WIDTH_CHROMIUM, 1.0f);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::PathParameteriCHROMIUM cmd;
            cmd.Init(kClientPathId, GL_PATH_STROKE_WIDTH_CHROMIUM, 1);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::PathStencilFuncCHROMIUM cmd;
            cmd.Init(GL_NEVER, 2, 3);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::StencilFillPathCHROMIUM cmd;
            cmd.Init(kClientPathId, GL_COUNT_UP_CHROMIUM, 1);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::StencilStrokePathCHROMIUM cmd;
            cmd.Init(kClientPathId, 1, 2);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::CoverFillPathCHROMIUM cmd;
            cmd.Init(kClientPathId, GL_BOUNDING_BOX_CHROMIUM);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::CoverStrokePathCHROMIUM cmd;
            cmd.Init(kClientPathId, GL_BOUNDING_BOX_CHROMIUM);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::StencilThenCoverFillPathCHROMIUM cmd;
            cmd.Init(kClientPathId, GL_COUNT_UP_CHROMIUM, 1, GL_BOUNDING_BOX_CHROMIUM);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            cmds::StencilThenCoverStrokePathCHROMIUM cmd;
            cmd.Init(kClientPathId, 1, 2, GL_BOUNDING_BOX_CHROMIUM);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
    }

    class GLES2DecoderTestWithCHROMIUMPathRendering : public GLES2DecoderTest {
    public:
        GLES2DecoderTestWithCHROMIUMPathRendering()
            : client_path_id_(125)
        {
        }

        void SetUp() override
        {
            InitState init;
            init.gl_version = "opengl es 3.1";
            init.has_alpha = true;
            init.has_depth = true;
            init.request_alpha = true;
            init.request_depth = true;
            init.bind_generates_resource = true;
            init.extensions = "GL_NV_path_rendering";
            base::CommandLine command_line(0, NULL);
            command_line.AppendSwitch(switches::kEnableGLPathRendering);
            InitDecoderWithCommandLine(init, &command_line);

            EXPECT_CALL(*gl_, GenPathsNV(1))
                .WillOnce(Return(kServicePathId))
                .RetiresOnSaturation();
            cmds::GenPathsCHROMIUM cmd;
            cmd.Init(client_path_id_, 1);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        }

    protected:
        template <typename TypeParam>
        void TestPathCommandsCHROMIUMCoordTypes();

        GLuint client_path_id_;
        static const GLuint kServicePathId = 311;
    };

    INSTANTIATE_TEST_CASE_P(Service,
        GLES2DecoderTestWithCHROMIUMPathRendering,
        ::testing::Bool());

    class GLES2DecoderTestWithBlendEquationAdvanced : public GLES2DecoderTest {
    public:
        GLES2DecoderTestWithBlendEquationAdvanced() { }
        void SetUp() override
        {
            InitState init;
            init.gl_version = "opengl es 2.0";
            init.has_alpha = true;
            init.has_depth = true;
            init.request_alpha = true;
            init.request_depth = true;
            init.bind_generates_resource = true;
            init.extensions = "GL_KHR_blend_equation_advanced";
            InitDecoder(init);
        }
    };

    INSTANTIATE_TEST_CASE_P(Service,
        GLES2DecoderTestWithBlendEquationAdvanced,
        ::testing::Bool());

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, GenDeletePaths)
    {
        static GLuint kFirstClientID = client_path_id_ + 88;
        static GLsizei kPathCount = 58;
        static GLuint kFirstCreatedServiceID = 8000;

        // GenPaths range 0 causes no calls.
        cmds::GenPathsCHROMIUM gen_cmd;
        gen_cmd.Init(kFirstClientID, 0);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // DeletePaths range 0 causes no calls.
        cmds::DeletePathsCHROMIUM delete_cmd;
        delete_cmd.Init(kFirstClientID, 0);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // DeletePaths client 0 causes no calls and no errors.
        delete_cmd.Init(0, 1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // DeletePaths with a big range should not cause any deletes.
        delete_cmd.Init(client_path_id_ + 1,
            std::numeric_limits<GLsizei>::max() - client_path_id_ - 1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        delete_cmd.Init(std::numeric_limits<GLsizei>::max() + 1,
            std::numeric_limits<GLsizei>::max());
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Normal Gen and Delete should cause the normal calls.
        EXPECT_CALL(*gl_, GenPathsNV(kPathCount))
            .WillOnce(Return(kFirstCreatedServiceID))
            .RetiresOnSaturation();

        gen_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID, kPathCount))
            .RetiresOnSaturation();

        delete_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, GenDeleteRanges)
    {
        static GLuint kFirstClientID = client_path_id_ + 77;
        static GLsizei kPathCount = 5800;
        static GLuint kFirstCreatedServiceID = 8000;

        // Create a range of path names, delete one in middle and then
        // the rest. Expect 3 DeletePath calls.
        EXPECT_CALL(*gl_, GenPathsNV(kPathCount))
            .WillOnce(Return(kFirstCreatedServiceID))
            .RetiresOnSaturation();
        cmds::GenPathsCHROMIUM gen_cmd;
        gen_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID + (kPathCount / 2), 1))
            .RetiresOnSaturation();

        cmds::DeletePathsCHROMIUM delete_cmd;
        delete_cmd.Init(kFirstClientID + (kPathCount / 2), 1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID, (kPathCount / 2)))
            .RetiresOnSaturation();
        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID + (kPathCount / 2) + 1, (kPathCount / 2) - 1)).RetiresOnSaturation();

        delete_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, GenDeleteManyPaths)
    {
        static GLuint kFirstClientID = client_path_id_ + 1;
        static GLsizei kPathCount = std::numeric_limits<GLsizei>::max();
        static GLuint kFirstCreatedServiceID = 8000;

        EXPECT_CALL(*gl_, GenPathsNV(kPathCount))
            .WillOnce(Return(kFirstCreatedServiceID))
            .RetiresOnSaturation();

        // GenPaths with big range.
        cmds::GenPathsCHROMIUM gen_cmd;
        gen_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Path range wraps, so we get connection error.
        gen_cmd.Init(kFirstClientID + kPathCount, kPathCount);
        EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID, kPathCount))
            .RetiresOnSaturation();

        cmds::DeletePathsCHROMIUM delete_cmd;
        delete_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Delete every possible path.
        // We run into the one created for client_path_id_.
        EXPECT_CALL(*gl_, DeletePathsNV(kServicePathId, 1)).RetiresOnSaturation();

        delete_cmd.Init(1u, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        delete_cmd.Init(static_cast<GLuint>(kPathCount) + 1u, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Allocate every possible path, delete few, allocate them back and
        // expect minimum amount of calls.
        EXPECT_CALL(*gl_, GenPathsNV(kPathCount))
            .WillOnce(Return(static_cast<GLuint>(1u)))
            .WillOnce(Return(static_cast<GLuint>(kPathCount) + 1u))
            .RetiresOnSaturation();

        gen_cmd.Init(1u, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        gen_cmd.Init(static_cast<GLuint>(kPathCount) + 1u, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        gen_cmd.Init(static_cast<GLuint>(kPathCount) * 2u + 2u, kPathCount);
        EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, DeletePathsNV(kFirstClientID, 4)).RetiresOnSaturation();

        delete_cmd.Init(kFirstClientID, 4);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, DeletePathsNV(kFirstClientID * 3, 1)).RetiresOnSaturation();

        delete_cmd.Init(kFirstClientID * 3, 1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, GenPathsNV(1))
            .WillOnce(Return(kFirstClientID))
            .WillOnce(Return(kFirstClientID + 1))
            .WillOnce(Return(kFirstClientID + 2))
            .WillOnce(Return(kFirstClientID + 3))
            .RetiresOnSaturation();

        for (int i = 0; i < 4; ++i) {
            gen_cmd.Init(kFirstClientID + i, 1);
            EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
            EXPECT_EQ(GL_NO_ERROR, GetGLError());
        }

        EXPECT_CALL(*gl_, GenPathsNV(1))
            .WillOnce(Return(kFirstClientID * 3))
            .RetiresOnSaturation();
        gen_cmd.Init(kFirstClientID * 3, 1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, DeletePathsNV(1u, kPathCount)).RetiresOnSaturation();
        EXPECT_CALL(*gl_, DeletePathsNV(static_cast<GLuint>(kPathCount) + 1u, kPathCount)).RetiresOnSaturation();

        delete_cmd.Init(1u, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        delete_cmd.Init(static_cast<GLuint>(kPathCount) + 1u, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Cleanup: return the client_path_id_ as a path.
        EXPECT_CALL(*gl_, GenPathsNV(1))
            .WillOnce(Return(static_cast<GLuint>(kServicePathId)))
            .RetiresOnSaturation();

        gen_cmd.Init(client_path_id_, 1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        GenPathsCHROMIUMInvalidCalls)
    {
        static GLuint kFirstClientID = client_path_id_ + 88;
        static GLsizei kPathCount = 5800;
        static GLuint kFirstCreatedServiceID = 8000;

        // Range < 0 is causes gl error.
        cmds::GenPathsCHROMIUM gen_cmd;
        gen_cmd.Init(kFirstClientID, -1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());

        // Path 0 is invalid client id, connection error.
        gen_cmd.Init(0, kPathCount);
        EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Too big range causes client id to wrap, connection error.
        gen_cmd.Init(std::numeric_limits<GLsizei>::max() + 3,
            std::numeric_limits<GLsizei>::max());
        EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Creating duplicate client_ids cause connection error.
        EXPECT_CALL(*gl_, GenPathsNV(kPathCount))
            .WillOnce(Return(kFirstCreatedServiceID))
            .RetiresOnSaturation();

        gen_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Create duplicate by executing the same cmd.
        EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Create duplicate by creating a range that contains
        // an already existing client path id.
        gen_cmd.Init(kFirstClientID - 1, 2);
        EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Cleanup.
        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID, kPathCount))
            .RetiresOnSaturation();
        cmds::DeletePathsCHROMIUM delete_cmd;
        delete_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        DeletePathsCHROMIUMInvalidCalls)
    {
        static GLuint kFirstClientID = client_path_id_ + 88;

        // Range < 0 is causes gl error.
        cmds::DeletePathsCHROMIUM delete_cmd;
        delete_cmd.Init(kFirstClientID, -1);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());

        // Too big range causes client id to wrap, connection error.
        delete_cmd.Init(std::numeric_limits<GLsizei>::max() + 3,
            std::numeric_limits<GLsizei>::max());
        EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        PathCommandsCHROMIUMInvalidCalls)
    {
        static const GLsizei kCorrectCoordCount = 19;
        static const GLsizei kCorrectCommandCount = 6;
        static const GLenum kInvalidCoordType = GL_NONE;

        GLfloat* coords = GetSharedMemoryAs<GLfloat*>();
        unsigned commands_offset = sizeof(GLfloat) * kCorrectCoordCount;
        GLubyte* commands = GetSharedMemoryAsWithOffset<GLubyte*>(commands_offset);
        for (int i = 0; i < kCorrectCoordCount; ++i) {
            coords[i] = 5.0f * i;
        }
        commands[0] = GL_MOVE_TO_CHROMIUM;
        commands[1] = GL_CLOSE_PATH_CHROMIUM;
        commands[2] = GL_LINE_TO_CHROMIUM;
        commands[3] = GL_QUADRATIC_CURVE_TO_CHROMIUM;
        commands[4] = GL_CUBIC_CURVE_TO_CHROMIUM;
        commands[5] = GL_CONIC_CURVE_TO_CHROMIUM;

        EXPECT_CALL(*gl_, PathCommandsNV(kServicePathId, kCorrectCommandCount, commands, kCorrectCoordCount, GL_FLOAT, coords)).RetiresOnSaturation();

        cmds::PathCommandsCHROMIUM cmd;

        // Reference call -- this succeeds.
        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            GL_FLOAT, shared_memory_id_, shared_memory_offset_);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, PathCommandsNV(_, _, _, _, _, _)).Times(0);

        // Invalid client id fails.
        cmd.Init(client_path_id_ - 1, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_, kCorrectCoordCount, GL_FLOAT,
            shared_memory_id_, shared_memory_offset_);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

        // The numCommands < 0.
        cmd.Init(client_path_id_, -1, shared_memory_id_, shared_memory_offset_,
            kCorrectCoordCount, GL_FLOAT, shared_memory_id_,
            shared_memory_offset_);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());

        // The numCoords < 0.
        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_, -1, GL_FLOAT, shared_memory_id_,
            shared_memory_offset_);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());

        // Invalid coordType fails.
        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_, kCorrectCoordCount, kInvalidCoordType,
            shared_memory_id_, shared_memory_offset_);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_ENUM, GetGLError());

        // Big command counts.
        cmd.Init(client_path_id_, std::numeric_limits<GLsizei>::max(),
            shared_memory_id_, shared_memory_offset_ + commands_offset,
            kCorrectCoordCount, GL_FLOAT, shared_memory_id_,
            shared_memory_offset_);
        EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Invalid SHM cases.
        cmd.Init(client_path_id_, kCorrectCommandCount, kInvalidSharedMemoryId,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            GL_FLOAT, shared_memory_id_, shared_memory_offset_);
        EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            kInvalidSharedMemoryOffset, kCorrectCoordCount, GL_FLOAT,
            shared_memory_id_, shared_memory_offset_);
        EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            GL_FLOAT, kInvalidSharedMemoryId, shared_memory_offset_);
        EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            GL_FLOAT, shared_memory_id_, kInvalidSharedMemoryOffset);
        EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // NULL shm command id with non-zero command count.
        cmd.Init(client_path_id_, kCorrectCommandCount, 0, 0, kCorrectCoordCount,
            GL_FLOAT, shared_memory_id_, shared_memory_offset_);
        EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // NULL shm coord id with non-zero coord count.
        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            GL_FLOAT, 0, 0);
        EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // The coordCount not matching what is in commands.
        // Expects kCorrectCoordCount+2 coords.
        commands[1] = GL_MOVE_TO_CHROMIUM;
        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            GL_FLOAT, shared_memory_id_, shared_memory_offset_);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

        // The coordCount not matching what is in commands.
        // Expects kCorrectCoordCount-2 coords.
        commands[0] = GL_CLOSE_PATH_CHROMIUM;
        commands[1] = GL_CLOSE_PATH_CHROMIUM;
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

        // NULL shm coord ids. Currently causes gl error, though client should not let
        // this through.
        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            GL_FLOAT, 0, 0);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        PathCommandsCHROMIUMEmptyCommands)
    {
        EXPECT_CALL(*gl_, PathCommandsNV(kServicePathId, 0, NULL, 0, GL_FLOAT, NULL))
            .RetiresOnSaturation();
        cmds::PathCommandsCHROMIUM cmd;
        cmd.Init(client_path_id_, 0, 0, 0, 0, GL_FLOAT, 0, 0);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        PathCommandsCHROMIUMInvalidCommands)
    {
        EXPECT_CALL(*gl_, PathCommandsNV(_, _, _, _, _, _)).Times(0);

        cmds::PathCommandsCHROMIUM cmd;

        {
            const GLsizei kCoordCount = 2;
            const GLsizei kCommandCount = 2;
            GLfloat* coords = GetSharedMemoryAs<GLfloat*>();
            unsigned commands_offset = sizeof(GLfloat) * kCoordCount;
            GLubyte* commands = GetSharedMemoryAsWithOffset<GLubyte*>(commands_offset);

            coords[0] = 5.0f;
            coords[1] = 5.0f;
            commands[0] = 0x3; // Token MOVE_TO_RELATIVE in NV_path_rendering.
            commands[1] = GL_CLOSE_PATH_CHROMIUM;

            cmd.Init(client_path_id_ - 1, kCommandCount, shared_memory_id_,
                shared_memory_offset_, kCoordCount, GL_FLOAT, shared_memory_id_,
                shared_memory_offset_);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
        {
            const GLsizei kCoordCount = 8;
            const GLsizei kCommandCount = 4;
            GLfloat* coords = GetSharedMemoryAs<GLfloat*>();
            unsigned commands_offset = sizeof(GLfloat) * kCoordCount;
            GLubyte* commands = GetSharedMemoryAsWithOffset<GLubyte*>(commands_offset);

            for (int i = 0; i < kCoordCount; ++i) {
                coords[i] = 5.0f * i;
            }
            commands[0] = GL_MOVE_TO_CHROMIUM;
            commands[1] = GL_MOVE_TO_CHROMIUM;
            commands[2] = 'M'; // Synonym to MOVE_TO in NV_path_rendering.
            commands[3] = GL_MOVE_TO_CHROMIUM;

            cmd.Init(client_path_id_ - 1, kCommandCount, shared_memory_id_,
                shared_memory_offset_, kCoordCount, GL_FLOAT, shared_memory_id_,
                shared_memory_offset_);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
        }
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, PathParameterXCHROMIUM)
    {
        static GLuint kFirstClientID = client_path_id_ + 88;
        static GLsizei kPathCount = 2;
        static GLuint kFirstCreatedServiceID = 8000;

        // Create a paths so that we do not modify client_path_id_
        EXPECT_CALL(*gl_, GenPathsNV(kPathCount))
            .WillOnce(Return(kFirstCreatedServiceID))
            .RetiresOnSaturation();
        cmds::GenPathsCHROMIUM gen_cmd;
        gen_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        cmds::PathParameterfCHROMIUM fcmd;
        cmds::PathParameteriCHROMIUM icmd;
        const struct {
            GLenum pname;
            GLfloat value;
            GLfloat expected_value;
        } kTestcases[] = {
            { GL_PATH_STROKE_WIDTH_CHROMIUM, 1.0f, 1.0f },
            { GL_PATH_STROKE_WIDTH_CHROMIUM, 0.0f, 0.0f },
            { GL_PATH_MITER_LIMIT_CHROMIUM, 500.0f, 500.0f },
            { GL_PATH_STROKE_BOUND_CHROMIUM, .80f, .80f },
            { GL_PATH_STROKE_BOUND_CHROMIUM, 1.80f, 1.0f },
            { GL_PATH_STROKE_BOUND_CHROMIUM, -1.0f, 0.0f },
            { GL_PATH_END_CAPS_CHROMIUM, GL_FLAT_CHROMIUM, GL_FLAT_CHROMIUM },
            { GL_PATH_END_CAPS_CHROMIUM, GL_SQUARE_CHROMIUM, GL_SQUARE_CHROMIUM },
            { GL_PATH_JOIN_STYLE_CHROMIUM,
                GL_MITER_REVERT_CHROMIUM,
                GL_MITER_REVERT_CHROMIUM },
        };

        for (auto& testcase : kTestcases) {
            EXPECT_CALL(*gl_, PathParameterfNV(kFirstCreatedServiceID, testcase.pname, testcase.expected_value))
                .Times(1)
                .RetiresOnSaturation();
            fcmd.Init(kFirstClientID, testcase.pname, testcase.value);
            EXPECT_EQ(error::kNoError, ExecuteCmd(fcmd));
            EXPECT_EQ(GL_NO_ERROR, GetGLError());

            EXPECT_CALL(*gl_,
                PathParameteriNV(kFirstCreatedServiceID + 1, testcase.pname,
                    static_cast<GLint>(testcase.expected_value)))
                .Times(1)
                .RetiresOnSaturation();
            icmd.Init(kFirstClientID + 1, testcase.pname,
                static_cast<GLint>(testcase.value));
            EXPECT_EQ(error::kNoError, ExecuteCmd(icmd));
            EXPECT_EQ(GL_NO_ERROR, GetGLError());
        }

        // Cleanup.
        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID, kPathCount))
            .RetiresOnSaturation();

        cmds::DeletePathsCHROMIUM delete_cmd;
        delete_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        PathParameterXCHROMIUMInvalidArgs)
    {
        static GLuint kFirstClientID = client_path_id_ + 88;
        static GLsizei kPathCount = 2;
        static GLuint kFirstCreatedServiceID = 8000;

        // Create a paths so that we do not modify client_path_id_
        EXPECT_CALL(*gl_, GenPathsNV(kPathCount))
            .WillOnce(Return(kFirstCreatedServiceID))
            .RetiresOnSaturation();
        cmds::GenPathsCHROMIUM gen_cmd;
        gen_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        cmds::PathParameterfCHROMIUM fcmd;
        cmds::PathParameteriCHROMIUM icmd;
        const struct {
            GLenum pname;
            GLfloat value;
            bool try_int_version;
            GLint error;
        } kTestcases[] = {
            { GL_PATH_STROKE_WIDTH_CHROMIUM, -1.0f, true, GL_INVALID_VALUE },
            { GL_PATH_MITER_LIMIT_CHROMIUM,
                std::numeric_limits<float>::infinity(),
                false,
                GL_INVALID_VALUE },
            { GL_PATH_MITER_LIMIT_CHROMIUM,
                std::numeric_limits<float>::quiet_NaN(),
                false,
                GL_INVALID_VALUE },
            { GL_PATH_END_CAPS_CHROMIUM, 0x4, true, GL_INVALID_VALUE },
            { GL_PATH_END_CAPS_CHROMIUM,
                GL_MITER_REVERT_CHROMIUM,
                true,
                GL_INVALID_VALUE },
            { GL_PATH_JOIN_STYLE_CHROMIUM, GL_FLAT_CHROMIUM, true, GL_INVALID_VALUE },
            { GL_PATH_MODELVIEW_CHROMIUM, GL_FLAT_CHROMIUM, true, GL_INVALID_ENUM },
        };

        EXPECT_CALL(*gl_, PathParameterfNV(_, _, _)).Times(0);
        EXPECT_CALL(*gl_, PathParameteriNV(_, _, _)).Times(0);

        for (auto& testcase : kTestcases) {
            fcmd.Init(kFirstClientID, testcase.pname, testcase.value);
            EXPECT_EQ(error::kNoError, ExecuteCmd(fcmd));
            EXPECT_EQ(testcase.error, GetGLError());
            if (!testcase.try_int_version)
                continue;

            icmd.Init(kFirstClientID + 1, testcase.pname,
                static_cast<GLint>(testcase.value));
            EXPECT_EQ(error::kNoError, ExecuteCmd(icmd));
            EXPECT_EQ(testcase.error, GetGLError());
        }

        // Cleanup.
        EXPECT_CALL(*gl_, DeletePathsNV(kFirstCreatedServiceID, kPathCount))
            .RetiresOnSaturation();

        cmds::DeletePathsCHROMIUM delete_cmd;
        delete_cmd.Init(kFirstClientID, kPathCount);
        EXPECT_EQ(error::kNoError, ExecuteCmd(delete_cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, StencilFillPathCHROMIUM)
    {
        SetupExpectationsForApplyingDefaultDirtyState();

        cmds::StencilFillPathCHROMIUM cmd;
        cmds::StencilThenCoverFillPathCHROMIUM tcmd;

        static const GLenum kFillModes[] = {
            GL_INVERT, GL_COUNT_UP_CHROMIUM, GL_COUNT_DOWN_CHROMIUM
        };
        static const GLuint kMask = 0x7F;

        for (auto& fill_mode : kFillModes) {
            EXPECT_CALL(*gl_, StencilFillPathNV(kServicePathId, fill_mode, kMask))
                .RetiresOnSaturation();
            cmd.Init(client_path_id_, fill_mode, kMask);
            EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
            EXPECT_EQ(GL_NO_ERROR, GetGLError());

            EXPECT_CALL(*gl_, StencilThenCoverFillPathNV(kServicePathId, fill_mode, kMask, GL_BOUNDING_BOX_NV))
                .RetiresOnSaturation();
            tcmd.Init(client_path_id_, fill_mode, kMask, GL_BOUNDING_BOX_CHROMIUM);
            EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
            EXPECT_EQ(GL_NO_ERROR, GetGLError());
        }

        // Non-existent path: no error, no call.
        cmd.Init(client_path_id_ - 1, GL_INVERT, 0x80);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        tcmd.Init(client_path_id_ - 1, GL_INVERT, 0x80, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        StencilFillPathCHROMIUMInvalidArgs)
    {
        EXPECT_CALL(*gl_, StencilFillPathNV(_, _, _)).Times(0);
        EXPECT_CALL(*gl_, StencilThenCoverFillPathNV(_, _, _, GL_BOUNDING_BOX_NV))
            .Times(0);

        cmds::StencilFillPathCHROMIUM cmd;
        cmds::StencilThenCoverFillPathCHROMIUM tcmd;

        cmd.Init(client_path_id_, GL_INVERT - 1, 0x80);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_ENUM, GetGLError());

        tcmd.Init(client_path_id_, GL_INVERT - 1, 0x80, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
        EXPECT_EQ(GL_INVALID_ENUM, GetGLError());

        // The /mask/+1 is not power of two -> invalid value.
        cmd.Init(client_path_id_, GL_COUNT_UP_CHROMIUM, 0x80);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());

        tcmd.Init(client_path_id_, GL_COUNT_UP_CHROMIUM, 0x80,
            GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());

        cmd.Init(client_path_id_, GL_COUNT_DOWN_CHROMIUM, 5);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());

        tcmd.Init(client_path_id_, GL_COUNT_DOWN_CHROMIUM, 5,
            GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
        EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, StencilStrokePathCHROMIUM)
    {
        SetupExpectationsForApplyingDefaultDirtyState();

        EXPECT_CALL(*gl_, StencilStrokePathNV(kServicePathId, 1, 0x80))
            .RetiresOnSaturation();
        EXPECT_CALL(*gl_, StencilThenCoverStrokePathNV(kServicePathId, 1, 0x80, GL_BOUNDING_BOX_NV))
            .RetiresOnSaturation();

        cmds::StencilStrokePathCHROMIUM cmd;
        cmds::StencilThenCoverStrokePathCHROMIUM tcmd;

        cmd.Init(client_path_id_, 1, 0x80);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        tcmd.Init(client_path_id_, 1, 0x80, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, StencilThenCoverStrokePathNV(kServicePathId, 1, 0x80, GL_CONVEX_HULL_NV))
            .RetiresOnSaturation();

        tcmd.Init(client_path_id_, 1, 0x80, GL_CONVEX_HULL_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Non-existent path: no error, no call.
        cmd.Init(client_path_id_ - 1, 1, 0x80);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        tcmd.Init(client_path_id_ - 1, 1, 0x80, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(tcmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, CoverFillPathCHROMIUM)
    {
        SetupExpectationsForApplyingDefaultDirtyState();

        EXPECT_CALL(*gl_, CoverFillPathNV(kServicePathId, GL_BOUNDING_BOX_NV))
            .RetiresOnSaturation();
        cmds::CoverFillPathCHROMIUM cmd;
        cmd.Init(client_path_id_, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, CoverFillPathNV(kServicePathId, GL_CONVEX_HULL_NV))
            .RetiresOnSaturation();
        cmd.Init(client_path_id_, GL_CONVEX_HULL_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Non-existent path: no error, no call.
        cmd.Init(client_path_id_ - 1, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, CoverStrokePathCHROMIUM)
    {
        SetupExpectationsForApplyingDefaultDirtyState();
        EXPECT_CALL(*gl_, CoverStrokePathNV(kServicePathId, GL_BOUNDING_BOX_NV))
            .RetiresOnSaturation();
        cmds::CoverStrokePathCHROMIUM cmd;
        cmd.Init(client_path_id_, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        EXPECT_CALL(*gl_, CoverStrokePathNV(kServicePathId, GL_CONVEX_HULL_NV))
            .RetiresOnSaturation();
        cmd.Init(client_path_id_, GL_CONVEX_HULL_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());

        // Non-existent path: no error, no call.
        cmd.Init(client_path_id_ - 1, GL_BOUNDING_BOX_CHROMIUM);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    namespace {
        template <typename T>
        struct gl_type_enum {
        };
        template <>
        struct gl_type_enum<GLbyte> {
            enum { kGLType = GL_BYTE };
        };
        template <>
        struct gl_type_enum<GLubyte> {
            enum { kGLType = GL_UNSIGNED_BYTE };
        };
        template <>
        struct gl_type_enum<GLshort> {
            enum { kGLType = GL_SHORT };
        };
        template <>
        struct gl_type_enum<GLushort> {
            enum { kGLType = GL_UNSIGNED_SHORT };
        };
        template <>
        struct gl_type_enum<GLfloat> {
            enum { kGLType = GL_FLOAT };
        };
    }

    template <typename TypeParam>
    void GLES2DecoderTestWithCHROMIUMPathRendering::
        TestPathCommandsCHROMIUMCoordTypes()
    {
        static const GLsizei kCorrectCoordCount = 19;
        static const GLsizei kCorrectCommandCount = 6;

        TypeParam* coords = GetSharedMemoryAs<TypeParam*>();
        unsigned commands_offset = sizeof(TypeParam) * kCorrectCoordCount;
        GLubyte* commands = GetSharedMemoryAsWithOffset<GLubyte*>(commands_offset);
        for (int i = 0; i < kCorrectCoordCount; ++i) {
            coords[i] = static_cast<TypeParam>(5 * i);
        }
        commands[0] = GL_MOVE_TO_CHROMIUM;
        commands[1] = GL_CLOSE_PATH_CHROMIUM;
        commands[2] = GL_LINE_TO_CHROMIUM;
        commands[3] = GL_QUADRATIC_CURVE_TO_CHROMIUM;
        commands[4] = GL_CUBIC_CURVE_TO_CHROMIUM;
        commands[5] = GL_CONIC_CURVE_TO_CHROMIUM;

        EXPECT_CALL(*gl_, PathCommandsNV(kServicePathId, kCorrectCommandCount, commands, kCorrectCoordCount, gl_type_enum<TypeParam>::kGLType, coords))
            .RetiresOnSaturation();

        cmds::PathCommandsCHROMIUM cmd;

        cmd.Init(client_path_id_, kCorrectCommandCount, shared_memory_id_,
            shared_memory_offset_ + commands_offset, kCorrectCoordCount,
            gl_type_enum<TypeParam>::kGLType, shared_memory_id_,
            shared_memory_offset_);
        EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
        EXPECT_EQ(GL_NO_ERROR, GetGLError());
    }

    TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
        PathCommandsCHROMIUMCoordTypes)
    {
        // Not using a typed test case, because the base class is already parametrized
        // test case and uses GetParam.
        TestPathCommandsCHROMIUMCoordTypes<GLbyte>();
        TestPathCommandsCHROMIUMCoordTypes<GLubyte>();
        TestPathCommandsCHROMIUMCoordTypes<GLshort>();
        TestPathCommandsCHROMIUMCoordTypes<GLushort>();
        TestPathCommandsCHROMIUMCoordTypes<GLfloat>();
    }

#include "gpu/command_buffer/service/gles2_cmd_decoder_unittest_extensions_autogen.h"

} // namespace gles2
} // namespace gpu
