// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// It is included by gles2_cmd_decoder_unittest_extensions.cc
#ifndef GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_EXTENSIONS_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_EXTENSIONS_AUTOGEN_H_

// TODO(gman): BlitFramebufferCHROMIUM
// TODO(gman): RenderbufferStorageMultisampleCHROMIUM
// TODO(gman): RenderbufferStorageMultisampleEXT
// TODO(gman): FramebufferTexture2DMultisampleEXT
// TODO(gman): DiscardFramebufferEXTImmediate

TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
    MatrixLoadfCHROMIUMImmediateValidArgs)
{
    cmds::MatrixLoadfCHROMIUMImmediate& cmd = *GetImmediateAs<cmds::MatrixLoadfCHROMIUMImmediate>();
    SpecializedSetup<cmds::MatrixLoadfCHROMIUMImmediate, 0>(true);
    GLfloat temp[16] = {
        0,
    };
    cmd.Init(GL_PATH_PROJECTION_CHROMIUM, &temp[0]);
    EXPECT_CALL(
        *gl_,
        MatrixLoadfEXT(GL_PATH_PROJECTION_CHROMIUM,
            reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
    EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
    MatrixLoadIdentityCHROMIUMValidArgs)
{
    EXPECT_CALL(*gl_, MatrixLoadIdentityEXT(GL_PATH_PROJECTION_CHROMIUM));
    SpecializedSetup<cmds::MatrixLoadIdentityCHROMIUM, 0>(true);
    cmds::MatrixLoadIdentityCHROMIUM cmd;
    cmd.Init(GL_PATH_PROJECTION_CHROMIUM);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): GenPathsCHROMIUM

// TODO(gman): DeletePathsCHROMIUM

TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering, IsPathCHROMIUMValidArgs)
{
    EXPECT_CALL(*gl_, IsPathNV(kServicePathId));
    SpecializedSetup<cmds::IsPathCHROMIUM, 0>(true);
    cmds::IsPathCHROMIUM cmd;
    cmd.Init(client_path_id_, shared_memory_id_, shared_memory_offset_);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
    IsPathCHROMIUMInvalidArgsBadSharedMemoryId)
{
    EXPECT_CALL(*gl_, IsPathNV(kServicePathId)).Times(0);
    SpecializedSetup<cmds::IsPathCHROMIUM, 0>(false);
    cmds::IsPathCHROMIUM cmd;
    cmd.Init(client_path_id_, kInvalidSharedMemoryId, shared_memory_offset_);
    EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
    cmd.Init(client_path_id_, shared_memory_id_, kInvalidSharedMemoryOffset);
    EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}
// TODO(gman): PathCommandsCHROMIUM

// TODO(gman): PathParameterfCHROMIUM

// TODO(gman): PathParameteriCHROMIUM

TEST_P(GLES2DecoderTestWithCHROMIUMPathRendering,
    PathStencilFuncCHROMIUMValidArgs)
{
    EXPECT_CALL(*gl_, PathStencilFuncNV(GL_NEVER, 2, 3));
    SpecializedSetup<cmds::PathStencilFuncCHROMIUM, 0>(true);
    cmds::PathStencilFuncCHROMIUM cmd;
    cmd.Init(GL_NEVER, 2, 3);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): StencilFillPathCHROMIUM

// TODO(gman): StencilStrokePathCHROMIUM

// TODO(gman): CoverFillPathCHROMIUM

// TODO(gman): CoverStrokePathCHROMIUM

// TODO(gman): StencilThenCoverFillPathCHROMIUM

// TODO(gman): StencilThenCoverStrokePathCHROMIUM

TEST_P(GLES2DecoderTestWithBlendEquationAdvanced, BlendBarrierKHRValidArgs)
{
    EXPECT_CALL(*gl_, BlendBarrierKHR());
    SpecializedSetup<cmds::BlendBarrierKHR, 0>(true);
    cmds::BlendBarrierKHR cmd;
    cmd.Init();
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): ApplyScreenSpaceAntialiasingCHROMIUM
#endif // GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_EXTENSIONS_AUTOGEN_H_
