// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// It is included by gles2_cmd_decoder_unittest_1.cc
#ifndef GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_1_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_1_AUTOGEN_H_

// TODO(gman): ActiveTexture

TEST_P(GLES2DecoderTest1, AttachShaderValidArgs) {
  EXPECT_CALL(*gl_, AttachShader(kServiceProgramId, kServiceShaderId));
  SpecializedSetup<cmds::AttachShader, 0>(true);
  cmds::AttachShader cmd;
  cmd.Init(client_program_id_, client_shader_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): BindAttribLocationBucket

TEST_P(GLES2DecoderTest1, BindBufferValidArgs) {
  EXPECT_CALL(*gl_, BindBuffer(GL_ARRAY_BUFFER, kServiceBufferId));
  SpecializedSetup<cmds::BindBuffer, 0>(true);
  cmds::BindBuffer cmd;
  cmd.Init(GL_ARRAY_BUFFER, client_buffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindBufferValidArgsNewId) {
  EXPECT_CALL(*gl_, BindBuffer(GL_ARRAY_BUFFER, kNewServiceId));
  EXPECT_CALL(*gl_, GenBuffersARB(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  SpecializedSetup<cmds::BindBuffer, 0>(true);
  cmds::BindBuffer cmd;
  cmd.Init(GL_ARRAY_BUFFER, kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetBuffer(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, BindBufferInvalidArgs0_0) {
  EXPECT_CALL(*gl_, BindBuffer(_, _)).Times(0);
  SpecializedSetup<cmds::BindBuffer, 0>(false);
  cmds::BindBuffer cmd;
  cmd.Init(GL_RENDERBUFFER, client_buffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindFramebufferValidArgs) {
  EXPECT_CALL(*gl_, BindFramebufferEXT(GL_FRAMEBUFFER, kServiceFramebufferId));
  SpecializedSetup<cmds::BindFramebuffer, 0>(true);
  cmds::BindFramebuffer cmd;
  cmd.Init(GL_FRAMEBUFFER, client_framebuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindFramebufferValidArgsNewId) {
  EXPECT_CALL(*gl_, BindFramebufferEXT(GL_FRAMEBUFFER, kNewServiceId));
  EXPECT_CALL(*gl_, GenFramebuffersEXT(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  SpecializedSetup<cmds::BindFramebuffer, 0>(true);
  cmds::BindFramebuffer cmd;
  cmd.Init(GL_FRAMEBUFFER, kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetFramebuffer(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, BindFramebufferInvalidArgs0_0) {
  EXPECT_CALL(*gl_, BindFramebufferEXT(_, _)).Times(0);
  SpecializedSetup<cmds::BindFramebuffer, 0>(false);
  cmds::BindFramebuffer cmd;
  cmd.Init(GL_DRAW_FRAMEBUFFER, client_framebuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindFramebufferInvalidArgs0_1) {
  EXPECT_CALL(*gl_, BindFramebufferEXT(_, _)).Times(0);
  SpecializedSetup<cmds::BindFramebuffer, 0>(false);
  cmds::BindFramebuffer cmd;
  cmd.Init(GL_READ_FRAMEBUFFER, client_framebuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindRenderbufferValidArgs) {
  EXPECT_CALL(*gl_,
              BindRenderbufferEXT(GL_RENDERBUFFER, kServiceRenderbufferId));
  SpecializedSetup<cmds::BindRenderbuffer, 0>(true);
  cmds::BindRenderbuffer cmd;
  cmd.Init(GL_RENDERBUFFER, client_renderbuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindRenderbufferValidArgsNewId) {
  EXPECT_CALL(*gl_, BindRenderbufferEXT(GL_RENDERBUFFER, kNewServiceId));
  EXPECT_CALL(*gl_, GenRenderbuffersEXT(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  SpecializedSetup<cmds::BindRenderbuffer, 0>(true);
  cmds::BindRenderbuffer cmd;
  cmd.Init(GL_RENDERBUFFER, kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetRenderbuffer(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, BindRenderbufferInvalidArgs0_0) {
  EXPECT_CALL(*gl_, BindRenderbufferEXT(_, _)).Times(0);
  SpecializedSetup<cmds::BindRenderbuffer, 0>(false);
  cmds::BindRenderbuffer cmd;
  cmd.Init(GL_FRAMEBUFFER, client_renderbuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindTextureValidArgs) {
  EXPECT_CALL(*gl_, BindTexture(GL_TEXTURE_2D, kServiceTextureId));
  SpecializedSetup<cmds::BindTexture, 0>(true);
  cmds::BindTexture cmd;
  cmd.Init(GL_TEXTURE_2D, client_texture_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindTextureValidArgsNewId) {
  EXPECT_CALL(*gl_, BindTexture(GL_TEXTURE_2D, kNewServiceId));
  EXPECT_CALL(*gl_, GenTextures(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  SpecializedSetup<cmds::BindTexture, 0>(true);
  cmds::BindTexture cmd;
  cmd.Init(GL_TEXTURE_2D, kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetTexture(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, BindTextureInvalidArgs0_0) {
  EXPECT_CALL(*gl_, BindTexture(_, _)).Times(0);
  SpecializedSetup<cmds::BindTexture, 0>(false);
  cmds::BindTexture cmd;
  cmd.Init(GL_TEXTURE_1D, client_texture_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BindTextureInvalidArgs0_1) {
  EXPECT_CALL(*gl_, BindTexture(_, _)).Times(0);
  SpecializedSetup<cmds::BindTexture, 0>(false);
  cmds::BindTexture cmd;
  cmd.Init(GL_TEXTURE_3D, client_texture_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendColorValidArgs) {
  EXPECT_CALL(*gl_, BlendColor(1, 2, 3, 4));
  SpecializedSetup<cmds::BlendColor, 0>(true);
  cmds::BlendColor cmd;
  cmd.Init(1, 2, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationValidArgs) {
  EXPECT_CALL(*gl_, BlendEquation(GL_FUNC_SUBTRACT));
  SpecializedSetup<cmds::BlendEquation, 0>(true);
  cmds::BlendEquation cmd;
  cmd.Init(GL_FUNC_SUBTRACT);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationInvalidArgs0_0) {
  EXPECT_CALL(*gl_, BlendEquation(_)).Times(0);
  SpecializedSetup<cmds::BlendEquation, 0>(false);
  cmds::BlendEquation cmd;
  cmd.Init(GL_MIN);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationInvalidArgs0_1) {
  EXPECT_CALL(*gl_, BlendEquation(_)).Times(0);
  SpecializedSetup<cmds::BlendEquation, 0>(false);
  cmds::BlendEquation cmd;
  cmd.Init(GL_MAX);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationSeparateValidArgs) {
  EXPECT_CALL(*gl_, BlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_ADD));
  SpecializedSetup<cmds::BlendEquationSeparate, 0>(true);
  cmds::BlendEquationSeparate cmd;
  cmd.Init(GL_FUNC_SUBTRACT, GL_FUNC_ADD);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationSeparateInvalidArgs0_0) {
  EXPECT_CALL(*gl_, BlendEquationSeparate(_, _)).Times(0);
  SpecializedSetup<cmds::BlendEquationSeparate, 0>(false);
  cmds::BlendEquationSeparate cmd;
  cmd.Init(GL_MIN, GL_FUNC_ADD);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationSeparateInvalidArgs0_1) {
  EXPECT_CALL(*gl_, BlendEquationSeparate(_, _)).Times(0);
  SpecializedSetup<cmds::BlendEquationSeparate, 0>(false);
  cmds::BlendEquationSeparate cmd;
  cmd.Init(GL_MAX, GL_FUNC_ADD);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationSeparateInvalidArgs1_0) {
  EXPECT_CALL(*gl_, BlendEquationSeparate(_, _)).Times(0);
  SpecializedSetup<cmds::BlendEquationSeparate, 0>(false);
  cmds::BlendEquationSeparate cmd;
  cmd.Init(GL_FUNC_SUBTRACT, GL_MIN);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendEquationSeparateInvalidArgs1_1) {
  EXPECT_CALL(*gl_, BlendEquationSeparate(_, _)).Times(0);
  SpecializedSetup<cmds::BlendEquationSeparate, 0>(false);
  cmds::BlendEquationSeparate cmd;
  cmd.Init(GL_FUNC_SUBTRACT, GL_MAX);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendFuncValidArgs) {
  EXPECT_CALL(*gl_, BlendFunc(GL_ZERO, GL_ZERO));
  SpecializedSetup<cmds::BlendFunc, 0>(true);
  cmds::BlendFunc cmd;
  cmd.Init(GL_ZERO, GL_ZERO);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, BlendFuncSeparateValidArgs) {
  EXPECT_CALL(*gl_, BlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO));
  SpecializedSetup<cmds::BlendFuncSeparate, 0>(true);
  cmds::BlendFuncSeparate cmd;
  cmd.Init(GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): BufferData

// TODO(gman): BufferSubData

TEST_P(GLES2DecoderTest1, CheckFramebufferStatusValidArgs) {
  EXPECT_CALL(*gl_, CheckFramebufferStatusEXT(GL_FRAMEBUFFER));
  SpecializedSetup<cmds::CheckFramebufferStatus, 0>(true);
  cmds::CheckFramebufferStatus cmd;
  cmd.Init(GL_FRAMEBUFFER, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, CheckFramebufferStatusInvalidArgs0_0) {
  EXPECT_CALL(*gl_, CheckFramebufferStatusEXT(_)).Times(0);
  SpecializedSetup<cmds::CheckFramebufferStatus, 0>(false);
  cmds::CheckFramebufferStatus cmd;
  cmd.Init(GL_DRAW_FRAMEBUFFER, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, CheckFramebufferStatusInvalidArgs0_1) {
  EXPECT_CALL(*gl_, CheckFramebufferStatusEXT(_)).Times(0);
  SpecializedSetup<cmds::CheckFramebufferStatus, 0>(false);
  cmds::CheckFramebufferStatus cmd;
  cmd.Init(GL_READ_FRAMEBUFFER, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, CheckFramebufferStatusInvalidArgsBadSharedMemoryId) {
  EXPECT_CALL(*gl_, CheckFramebufferStatusEXT(GL_FRAMEBUFFER)).Times(0);
  SpecializedSetup<cmds::CheckFramebufferStatus, 0>(false);
  cmds::CheckFramebufferStatus cmd;
  cmd.Init(GL_FRAMEBUFFER, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(GL_FRAMEBUFFER, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, ClearValidArgs) {
  EXPECT_CALL(*gl_, Clear(1));
  SpecializedSetup<cmds::Clear, 0>(true);
  cmds::Clear cmd;
  cmd.Init(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, ClearColorValidArgs) {
  EXPECT_CALL(*gl_, ClearColor(1, 2, 3, 4));
  SpecializedSetup<cmds::ClearColor, 0>(true);
  cmds::ClearColor cmd;
  cmd.Init(1, 2, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, ClearDepthfValidArgs) {
  EXPECT_CALL(*gl_, ClearDepth(0.5f));
  SpecializedSetup<cmds::ClearDepthf, 0>(true);
  cmds::ClearDepthf cmd;
  cmd.Init(0.5f);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, ClearStencilValidArgs) {
  EXPECT_CALL(*gl_, ClearStencil(1));
  SpecializedSetup<cmds::ClearStencil, 0>(true);
  cmds::ClearStencil cmd;
  cmd.Init(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, ColorMaskValidArgs) {
  SpecializedSetup<cmds::ColorMask, 0>(true);
  cmds::ColorMask cmd;
  cmd.Init(true, true, true, true);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): CompileShader
// TODO(gman): CompressedTexImage2DBucket
// TODO(gman): CompressedTexImage2D

// TODO(gman): CompressedTexSubImage2DBucket
// TODO(gman): CompressedTexSubImage2D

// TODO(gman): CopyTexImage2D

TEST_P(GLES2DecoderTest1, CopyTexSubImage2DValidArgs) {
  EXPECT_CALL(*gl_, CopyTexSubImage2D(GL_TEXTURE_2D, 2, 3, 4, 5, 6, 7, 8));
  SpecializedSetup<cmds::CopyTexSubImage2D, 0>(true);
  cmds::CopyTexSubImage2D cmd;
  cmd.Init(GL_TEXTURE_2D, 2, 3, 4, 5, 6, 7, 8);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, CopyTexSubImage2DInvalidArgs0_0) {
  EXPECT_CALL(*gl_, CopyTexSubImage2D(_, _, _, _, _, _, _, _)).Times(0);
  SpecializedSetup<cmds::CopyTexSubImage2D, 0>(false);
  cmds::CopyTexSubImage2D cmd;
  cmd.Init(GL_PROXY_TEXTURE_CUBE_MAP, 2, 3, 4, 5, 6, 7, 8);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, CopyTexSubImage2DInvalidArgs6_0) {
  EXPECT_CALL(*gl_, CopyTexSubImage2D(_, _, _, _, _, _, _, _)).Times(0);
  SpecializedSetup<cmds::CopyTexSubImage2D, 0>(false);
  cmds::CopyTexSubImage2D cmd;
  cmd.Init(GL_TEXTURE_2D, 2, 3, 4, 5, 6, -1, 8);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest1, CopyTexSubImage2DInvalidArgs7_0) {
  EXPECT_CALL(*gl_, CopyTexSubImage2D(_, _, _, _, _, _, _, _)).Times(0);
  SpecializedSetup<cmds::CopyTexSubImage2D, 0>(false);
  cmds::CopyTexSubImage2D cmd;
  cmd.Init(GL_TEXTURE_2D, 2, 3, 4, 5, 6, 7, -1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest1, CreateProgramValidArgs) {
  EXPECT_CALL(*gl_, CreateProgram()).WillOnce(Return(kNewServiceId));
  SpecializedSetup<cmds::CreateProgram, 0>(true);
  cmds::CreateProgram cmd;
  cmd.Init(kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetProgram(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, CreateShaderValidArgs) {
  EXPECT_CALL(*gl_, CreateShader(GL_VERTEX_SHADER))
      .WillOnce(Return(kNewServiceId));
  SpecializedSetup<cmds::CreateShader, 0>(true);
  cmds::CreateShader cmd;
  cmd.Init(GL_VERTEX_SHADER, kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetShader(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, CreateShaderInvalidArgs0_0) {
  EXPECT_CALL(*gl_, CreateShader(_)).Times(0);
  SpecializedSetup<cmds::CreateShader, 0>(false);
  cmds::CreateShader cmd;
  cmd.Init(GL_GEOMETRY_SHADER, kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, CullFaceValidArgs) {
  EXPECT_CALL(*gl_, CullFace(GL_FRONT));
  SpecializedSetup<cmds::CullFace, 0>(true);
  cmds::CullFace cmd;
  cmd.Init(GL_FRONT);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DeleteBuffersImmediateValidArgs) {
  EXPECT_CALL(*gl_, DeleteBuffersARB(1, Pointee(kServiceBufferId))).Times(1);
  cmds::DeleteBuffersImmediate& cmd =
      *GetImmediateAs<cmds::DeleteBuffersImmediate>();
  SpecializedSetup<cmds::DeleteBuffersImmediate, 0>(true);
  cmd.Init(1, &client_buffer_id_);
  EXPECT_EQ(error::kNoError,
            ExecuteImmediateCmd(cmd, sizeof(client_buffer_id_)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetBuffer(client_buffer_id_) == NULL);
}

TEST_P(GLES2DecoderTest1, DeleteBuffersImmediateInvalidArgs) {
  cmds::DeleteBuffersImmediate& cmd =
      *GetImmediateAs<cmds::DeleteBuffersImmediate>();
  SpecializedSetup<cmds::DeleteBuffersImmediate, 0>(false);
  GLuint temp = kInvalidClientId;
  cmd.Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
}

TEST_P(GLES2DecoderTest1, DeleteFramebuffersImmediateValidArgs) {
  EXPECT_CALL(*gl_, DeleteFramebuffersEXT(1, Pointee(kServiceFramebufferId)))
      .Times(1);
  cmds::DeleteFramebuffersImmediate& cmd =
      *GetImmediateAs<cmds::DeleteFramebuffersImmediate>();
  SpecializedSetup<cmds::DeleteFramebuffersImmediate, 0>(true);
  cmd.Init(1, &client_framebuffer_id_);
  EXPECT_EQ(error::kNoError,
            ExecuteImmediateCmd(cmd, sizeof(client_framebuffer_id_)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetFramebuffer(client_framebuffer_id_) == NULL);
}

TEST_P(GLES2DecoderTest1, DeleteFramebuffersImmediateInvalidArgs) {
  cmds::DeleteFramebuffersImmediate& cmd =
      *GetImmediateAs<cmds::DeleteFramebuffersImmediate>();
  SpecializedSetup<cmds::DeleteFramebuffersImmediate, 0>(false);
  GLuint temp = kInvalidClientId;
  cmd.Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
}

TEST_P(GLES2DecoderTest1, DeleteProgramValidArgs) {
  EXPECT_CALL(*gl_, DeleteProgram(kServiceProgramId));
  SpecializedSetup<cmds::DeleteProgram, 0>(true);
  cmds::DeleteProgram cmd;
  cmd.Init(client_program_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DeleteRenderbuffersImmediateValidArgs) {
  EXPECT_CALL(*gl_, DeleteRenderbuffersEXT(1, Pointee(kServiceRenderbufferId)))
      .Times(1);
  cmds::DeleteRenderbuffersImmediate& cmd =
      *GetImmediateAs<cmds::DeleteRenderbuffersImmediate>();
  SpecializedSetup<cmds::DeleteRenderbuffersImmediate, 0>(true);
  cmd.Init(1, &client_renderbuffer_id_);
  EXPECT_EQ(error::kNoError,
            ExecuteImmediateCmd(cmd, sizeof(client_renderbuffer_id_)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetRenderbuffer(client_renderbuffer_id_) == NULL);
}

TEST_P(GLES2DecoderTest1, DeleteRenderbuffersImmediateInvalidArgs) {
  cmds::DeleteRenderbuffersImmediate& cmd =
      *GetImmediateAs<cmds::DeleteRenderbuffersImmediate>();
  SpecializedSetup<cmds::DeleteRenderbuffersImmediate, 0>(false);
  GLuint temp = kInvalidClientId;
  cmd.Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
}

TEST_P(GLES2DecoderTest1, DeleteShaderValidArgs) {
  EXPECT_CALL(*gl_, DeleteShader(kServiceShaderId));
  SpecializedSetup<cmds::DeleteShader, 0>(true);
  cmds::DeleteShader cmd;
  cmd.Init(client_shader_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DeleteTexturesImmediateValidArgs) {
  EXPECT_CALL(*gl_, DeleteTextures(1, Pointee(kServiceTextureId))).Times(1);
  cmds::DeleteTexturesImmediate& cmd =
      *GetImmediateAs<cmds::DeleteTexturesImmediate>();
  SpecializedSetup<cmds::DeleteTexturesImmediate, 0>(true);
  cmd.Init(1, &client_texture_id_);
  EXPECT_EQ(error::kNoError,
            ExecuteImmediateCmd(cmd, sizeof(client_texture_id_)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetTexture(client_texture_id_) == NULL);
}

TEST_P(GLES2DecoderTest1, DeleteTexturesImmediateInvalidArgs) {
  cmds::DeleteTexturesImmediate& cmd =
      *GetImmediateAs<cmds::DeleteTexturesImmediate>();
  SpecializedSetup<cmds::DeleteTexturesImmediate, 0>(false);
  GLuint temp = kInvalidClientId;
  cmd.Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
}

TEST_P(GLES2DecoderTest1, DepthFuncValidArgs) {
  EXPECT_CALL(*gl_, DepthFunc(GL_NEVER));
  SpecializedSetup<cmds::DepthFunc, 0>(true);
  cmds::DepthFunc cmd;
  cmd.Init(GL_NEVER);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DepthMaskValidArgs) {
  SpecializedSetup<cmds::DepthMask, 0>(true);
  cmds::DepthMask cmd;
  cmd.Init(true);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DepthRangefValidArgs) {
  EXPECT_CALL(*gl_, DepthRange(1, 2));
  SpecializedSetup<cmds::DepthRangef, 0>(true);
  cmds::DepthRangef cmd;
  cmd.Init(1, 2);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DetachShaderValidArgs) {
  EXPECT_CALL(*gl_, DetachShader(kServiceProgramId, kServiceShaderId));
  SpecializedSetup<cmds::DetachShader, 0>(true);
  cmds::DetachShader cmd;
  cmd.Init(client_program_id_, client_shader_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DisableValidArgs) {
  SetupExpectationsForEnableDisable(GL_BLEND, false);
  SpecializedSetup<cmds::Disable, 0>(true);
  cmds::Disable cmd;
  cmd.Init(GL_BLEND);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, DisableInvalidArgs0_0) {
  EXPECT_CALL(*gl_, Disable(_)).Times(0);
  SpecializedSetup<cmds::Disable, 0>(false);
  cmds::Disable cmd;
  cmd.Init(GL_CLIP_PLANE0);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, DisableInvalidArgs0_1) {
  EXPECT_CALL(*gl_, Disable(_)).Times(0);
  SpecializedSetup<cmds::Disable, 0>(false);
  cmds::Disable cmd;
  cmd.Init(GL_POINT_SPRITE);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, DisableVertexAttribArrayValidArgs) {
  EXPECT_CALL(*gl_, DisableVertexAttribArray(1));
  SpecializedSetup<cmds::DisableVertexAttribArray, 0>(true);
  cmds::DisableVertexAttribArray cmd;
  cmd.Init(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): DrawArrays

// TODO(gman): DrawElements

TEST_P(GLES2DecoderTest1, EnableValidArgs) {
  SetupExpectationsForEnableDisable(GL_BLEND, true);
  SpecializedSetup<cmds::Enable, 0>(true);
  cmds::Enable cmd;
  cmd.Init(GL_BLEND);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, EnableInvalidArgs0_0) {
  EXPECT_CALL(*gl_, Enable(_)).Times(0);
  SpecializedSetup<cmds::Enable, 0>(false);
  cmds::Enable cmd;
  cmd.Init(GL_CLIP_PLANE0);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, EnableInvalidArgs0_1) {
  EXPECT_CALL(*gl_, Enable(_)).Times(0);
  SpecializedSetup<cmds::Enable, 0>(false);
  cmds::Enable cmd;
  cmd.Init(GL_POINT_SPRITE);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, EnableVertexAttribArrayValidArgs) {
  EXPECT_CALL(*gl_, EnableVertexAttribArray(1));
  SpecializedSetup<cmds::EnableVertexAttribArray, 0>(true);
  cmds::EnableVertexAttribArray cmd;
  cmd.Init(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, FinishValidArgs) {
  EXPECT_CALL(*gl_, Finish());
  SpecializedSetup<cmds::Finish, 0>(true);
  cmds::Finish cmd;
  cmd.Init();
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, FlushValidArgs) {
  EXPECT_CALL(*gl_, Flush());
  SpecializedSetup<cmds::Flush, 0>(true);
  cmds::Flush cmd;
  cmd.Init();
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferRenderbufferValidArgs) {
  EXPECT_CALL(*gl_,
              FramebufferRenderbufferEXT(GL_FRAMEBUFFER,
                                         GL_COLOR_ATTACHMENT0,
                                         GL_RENDERBUFFER,
                                         kServiceRenderbufferId));
  SpecializedSetup<cmds::FramebufferRenderbuffer, 0>(true);
  cmds::FramebufferRenderbuffer cmd;
  cmd.Init(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_RENDERBUFFER,
           client_renderbuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferRenderbufferInvalidArgs0_0) {
  EXPECT_CALL(*gl_, FramebufferRenderbufferEXT(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::FramebufferRenderbuffer, 0>(false);
  cmds::FramebufferRenderbuffer cmd;
  cmd.Init(GL_DRAW_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_RENDERBUFFER,
           client_renderbuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferRenderbufferInvalidArgs0_1) {
  EXPECT_CALL(*gl_, FramebufferRenderbufferEXT(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::FramebufferRenderbuffer, 0>(false);
  cmds::FramebufferRenderbuffer cmd;
  cmd.Init(GL_READ_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_RENDERBUFFER,
           client_renderbuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferRenderbufferInvalidArgs2_0) {
  EXPECT_CALL(*gl_, FramebufferRenderbufferEXT(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::FramebufferRenderbuffer, 0>(false);
  cmds::FramebufferRenderbuffer cmd;
  cmd.Init(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_FRAMEBUFFER,
           client_renderbuffer_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferTexture2DValidArgs) {
  EXPECT_CALL(*gl_,
              FramebufferTexture2DEXT(GL_FRAMEBUFFER,
                                      GL_COLOR_ATTACHMENT0,
                                      GL_TEXTURE_2D,
                                      kServiceTextureId,
                                      0));
  SpecializedSetup<cmds::FramebufferTexture2D, 0>(true);
  cmds::FramebufferTexture2D cmd;
  cmd.Init(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, client_texture_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferTexture2DInvalidArgs0_0) {
  EXPECT_CALL(*gl_, FramebufferTexture2DEXT(_, _, _, _, _)).Times(0);
  SpecializedSetup<cmds::FramebufferTexture2D, 0>(false);
  cmds::FramebufferTexture2D cmd;
  cmd.Init(GL_DRAW_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_TEXTURE_2D,
           client_texture_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferTexture2DInvalidArgs0_1) {
  EXPECT_CALL(*gl_, FramebufferTexture2DEXT(_, _, _, _, _)).Times(0);
  SpecializedSetup<cmds::FramebufferTexture2D, 0>(false);
  cmds::FramebufferTexture2D cmd;
  cmd.Init(GL_READ_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_TEXTURE_2D,
           client_texture_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, FramebufferTexture2DInvalidArgs2_0) {
  EXPECT_CALL(*gl_, FramebufferTexture2DEXT(_, _, _, _, _)).Times(0);
  SpecializedSetup<cmds::FramebufferTexture2D, 0>(false);
  cmds::FramebufferTexture2D cmd;
  cmd.Init(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_PROXY_TEXTURE_CUBE_MAP,
           client_texture_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, FrontFaceValidArgs) {
  EXPECT_CALL(*gl_, FrontFace(GL_CW));
  SpecializedSetup<cmds::FrontFace, 0>(true);
  cmds::FrontFace cmd;
  cmd.Init(GL_CW);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GenBuffersImmediateValidArgs) {
  EXPECT_CALL(*gl_, GenBuffersARB(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  cmds::GenBuffersImmediate* cmd = GetImmediateAs<cmds::GenBuffersImmediate>();
  GLuint temp = kNewClientId;
  SpecializedSetup<cmds::GenBuffersImmediate, 0>(true);
  cmd->Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(*cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetBuffer(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, GenBuffersImmediateInvalidArgs) {
  EXPECT_CALL(*gl_, GenBuffersARB(_, _)).Times(0);
  cmds::GenBuffersImmediate* cmd = GetImmediateAs<cmds::GenBuffersImmediate>();
  SpecializedSetup<cmds::GenBuffersImmediate, 0>(false);
  cmd->Init(1, &client_buffer_id_);
  EXPECT_EQ(error::kInvalidArguments,
            ExecuteImmediateCmd(*cmd, sizeof(&client_buffer_id_)));
}

TEST_P(GLES2DecoderTest1, GenerateMipmapValidArgs) {
  EXPECT_CALL(*gl_, GenerateMipmapEXT(GL_TEXTURE_2D));
  SpecializedSetup<cmds::GenerateMipmap, 0>(true);
  cmds::GenerateMipmap cmd;
  cmd.Init(GL_TEXTURE_2D);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GenerateMipmapInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GenerateMipmapEXT(_)).Times(0);
  SpecializedSetup<cmds::GenerateMipmap, 0>(false);
  cmds::GenerateMipmap cmd;
  cmd.Init(GL_TEXTURE_1D);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GenerateMipmapInvalidArgs0_1) {
  EXPECT_CALL(*gl_, GenerateMipmapEXT(_)).Times(0);
  SpecializedSetup<cmds::GenerateMipmap, 0>(false);
  cmds::GenerateMipmap cmd;
  cmd.Init(GL_TEXTURE_3D);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GenFramebuffersImmediateValidArgs) {
  EXPECT_CALL(*gl_, GenFramebuffersEXT(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  cmds::GenFramebuffersImmediate* cmd =
      GetImmediateAs<cmds::GenFramebuffersImmediate>();
  GLuint temp = kNewClientId;
  SpecializedSetup<cmds::GenFramebuffersImmediate, 0>(true);
  cmd->Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(*cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetFramebuffer(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, GenFramebuffersImmediateInvalidArgs) {
  EXPECT_CALL(*gl_, GenFramebuffersEXT(_, _)).Times(0);
  cmds::GenFramebuffersImmediate* cmd =
      GetImmediateAs<cmds::GenFramebuffersImmediate>();
  SpecializedSetup<cmds::GenFramebuffersImmediate, 0>(false);
  cmd->Init(1, &client_framebuffer_id_);
  EXPECT_EQ(error::kInvalidArguments,
            ExecuteImmediateCmd(*cmd, sizeof(&client_framebuffer_id_)));
}

TEST_P(GLES2DecoderTest1, GenRenderbuffersImmediateValidArgs) {
  EXPECT_CALL(*gl_, GenRenderbuffersEXT(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  cmds::GenRenderbuffersImmediate* cmd =
      GetImmediateAs<cmds::GenRenderbuffersImmediate>();
  GLuint temp = kNewClientId;
  SpecializedSetup<cmds::GenRenderbuffersImmediate, 0>(true);
  cmd->Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(*cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetRenderbuffer(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, GenRenderbuffersImmediateInvalidArgs) {
  EXPECT_CALL(*gl_, GenRenderbuffersEXT(_, _)).Times(0);
  cmds::GenRenderbuffersImmediate* cmd =
      GetImmediateAs<cmds::GenRenderbuffersImmediate>();
  SpecializedSetup<cmds::GenRenderbuffersImmediate, 0>(false);
  cmd->Init(1, &client_renderbuffer_id_);
  EXPECT_EQ(error::kInvalidArguments,
            ExecuteImmediateCmd(*cmd, sizeof(&client_renderbuffer_id_)));
}

TEST_P(GLES2DecoderTest1, GenTexturesImmediateValidArgs) {
  EXPECT_CALL(*gl_, GenTextures(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId));
  cmds::GenTexturesImmediate* cmd =
      GetImmediateAs<cmds::GenTexturesImmediate>();
  GLuint temp = kNewClientId;
  SpecializedSetup<cmds::GenTexturesImmediate, 0>(true);
  cmd->Init(1, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(*cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(GetTexture(kNewClientId) != NULL);
}

TEST_P(GLES2DecoderTest1, GenTexturesImmediateInvalidArgs) {
  EXPECT_CALL(*gl_, GenTextures(_, _)).Times(0);
  cmds::GenTexturesImmediate* cmd =
      GetImmediateAs<cmds::GenTexturesImmediate>();
  SpecializedSetup<cmds::GenTexturesImmediate, 0>(false);
  cmd->Init(1, &client_texture_id_);
  EXPECT_EQ(error::kInvalidArguments,
            ExecuteImmediateCmd(*cmd, sizeof(&client_texture_id_)));
}
// TODO(gman): GetActiveAttrib

// TODO(gman): GetActiveUniform

// TODO(gman): GetAttachedShaders

// TODO(gman): GetAttribLocation

TEST_P(GLES2DecoderTest1, GetBooleanvValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetBooleanv, 0>(true);
  typedef cmds::GetBooleanv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(*gl_, GetBooleanv(GL_ACTIVE_TEXTURE, result->GetData()));
  result->size = 0;
  cmds::GetBooleanv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_ACTIVE_TEXTURE),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetBooleanvInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetBooleanv(_, _)).Times(0);
  SpecializedSetup<cmds::GetBooleanv, 0>(false);
  cmds::GetBooleanv::Result* result =
      static_cast<cmds::GetBooleanv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBooleanv cmd;
  cmd.Init(GL_FOG_HINT, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetBooleanvInvalidArgs1_0) {
  EXPECT_CALL(*gl_, GetBooleanv(_, _)).Times(0);
  SpecializedSetup<cmds::GetBooleanv, 0>(false);
  cmds::GetBooleanv::Result* result =
      static_cast<cmds::GetBooleanv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBooleanv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetBooleanvInvalidArgs1_1) {
  EXPECT_CALL(*gl_, GetBooleanv(_, _)).Times(0);
  SpecializedSetup<cmds::GetBooleanv, 0>(false);
  cmds::GetBooleanv::Result* result =
      static_cast<cmds::GetBooleanv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBooleanv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetBufferParameterivValidArgs) {
  SpecializedSetup<cmds::GetBufferParameteriv, 0>(true);
  typedef cmds::GetBufferParameteriv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBufferParameteriv cmd;
  cmd.Init(GL_ARRAY_BUFFER,
           GL_BUFFER_SIZE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_BUFFER_SIZE),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetBufferParameterivInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetBufferParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetBufferParameteriv, 0>(false);
  cmds::GetBufferParameteriv::Result* result =
      static_cast<cmds::GetBufferParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBufferParameteriv cmd;
  cmd.Init(GL_RENDERBUFFER,
           GL_BUFFER_SIZE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetBufferParameterivInvalidArgs1_0) {
  EXPECT_CALL(*gl_, GetBufferParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetBufferParameteriv, 0>(false);
  cmds::GetBufferParameteriv::Result* result =
      static_cast<cmds::GetBufferParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBufferParameteriv cmd;
  cmd.Init(GL_ARRAY_BUFFER,
           GL_PIXEL_PACK_BUFFER,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetBufferParameterivInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetBufferParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetBufferParameteriv, 0>(false);
  cmds::GetBufferParameteriv::Result* result =
      static_cast<cmds::GetBufferParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBufferParameteriv cmd;
  cmd.Init(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetBufferParameterivInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetBufferParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetBufferParameteriv, 0>(false);
  cmds::GetBufferParameteriv::Result* result =
      static_cast<cmds::GetBufferParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetBufferParameteriv cmd;
  cmd.Init(GL_ARRAY_BUFFER,
           GL_BUFFER_SIZE,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetErrorValidArgs) {
  EXPECT_CALL(*gl_, GetError());
  SpecializedSetup<cmds::GetError, 0>(true);
  cmds::GetError cmd;
  cmd.Init(shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetErrorInvalidArgsBadSharedMemoryId) {
  EXPECT_CALL(*gl_, GetError()).Times(0);
  SpecializedSetup<cmds::GetError, 0>(false);
  cmds::GetError cmd;
  cmd.Init(kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, GetFloatvValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetFloatv, 0>(true);
  typedef cmds::GetFloatv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(*gl_, GetFloatv(GL_ACTIVE_TEXTURE, result->GetData()));
  result->size = 0;
  cmds::GetFloatv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_ACTIVE_TEXTURE),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetFloatvInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetFloatv(_, _)).Times(0);
  SpecializedSetup<cmds::GetFloatv, 0>(false);
  cmds::GetFloatv::Result* result =
      static_cast<cmds::GetFloatv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetFloatv cmd;
  cmd.Init(GL_FOG_HINT, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetFloatvInvalidArgs1_0) {
  EXPECT_CALL(*gl_, GetFloatv(_, _)).Times(0);
  SpecializedSetup<cmds::GetFloatv, 0>(false);
  cmds::GetFloatv::Result* result =
      static_cast<cmds::GetFloatv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetFloatv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetFloatvInvalidArgs1_1) {
  EXPECT_CALL(*gl_, GetFloatv(_, _)).Times(0);
  SpecializedSetup<cmds::GetFloatv, 0>(false);
  cmds::GetFloatv::Result* result =
      static_cast<cmds::GetFloatv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetFloatv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetFramebufferAttachmentParameterivValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetFramebufferAttachmentParameteriv, 0>(true);
  typedef cmds::GetFramebufferAttachmentParameteriv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(*gl_,
              GetFramebufferAttachmentParameterivEXT(
                  GL_FRAMEBUFFER,
                  GL_COLOR_ATTACHMENT0,
                  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                  result->GetData()));
  result->size = 0;
  cmds::GetFramebufferAttachmentParameteriv cmd;
  cmd.Init(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(
                GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetFramebufferAttachmentParameterivInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetFramebufferAttachmentParameterivEXT(_, _, _, _))
      .Times(0);
  SpecializedSetup<cmds::GetFramebufferAttachmentParameteriv, 0>(false);
  cmds::GetFramebufferAttachmentParameteriv::Result* result =
      static_cast<cmds::GetFramebufferAttachmentParameteriv::Result*>(
          shared_memory_address_);
  result->size = 0;
  cmds::GetFramebufferAttachmentParameteriv cmd;
  cmd.Init(GL_DRAW_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetFramebufferAttachmentParameterivInvalidArgs0_1) {
  EXPECT_CALL(*gl_, GetFramebufferAttachmentParameterivEXT(_, _, _, _))
      .Times(0);
  SpecializedSetup<cmds::GetFramebufferAttachmentParameteriv, 0>(false);
  cmds::GetFramebufferAttachmentParameteriv::Result* result =
      static_cast<cmds::GetFramebufferAttachmentParameteriv::Result*>(
          shared_memory_address_);
  result->size = 0;
  cmds::GetFramebufferAttachmentParameteriv cmd;
  cmd.Init(GL_READ_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetFramebufferAttachmentParameterivInvalidArgs3_0) {
  EXPECT_CALL(*gl_, GetFramebufferAttachmentParameterivEXT(_, _, _, _))
      .Times(0);
  SpecializedSetup<cmds::GetFramebufferAttachmentParameteriv, 0>(false);
  cmds::GetFramebufferAttachmentParameteriv::Result* result =
      static_cast<cmds::GetFramebufferAttachmentParameteriv::Result*>(
          shared_memory_address_);
  result->size = 0;
  cmds::GetFramebufferAttachmentParameteriv cmd;
  cmd.Init(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
           kInvalidSharedMemoryId,
           0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetFramebufferAttachmentParameterivInvalidArgs3_1) {
  EXPECT_CALL(*gl_, GetFramebufferAttachmentParameterivEXT(_, _, _, _))
      .Times(0);
  SpecializedSetup<cmds::GetFramebufferAttachmentParameteriv, 0>(false);
  cmds::GetFramebufferAttachmentParameteriv::Result* result =
      static_cast<cmds::GetFramebufferAttachmentParameteriv::Result*>(
          shared_memory_address_);
  result->size = 0;
  cmds::GetFramebufferAttachmentParameteriv cmd;
  cmd.Init(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT0,
           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetIntegervValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetIntegerv, 0>(true);
  typedef cmds::GetIntegerv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(*gl_, GetIntegerv(GL_ACTIVE_TEXTURE, result->GetData()));
  result->size = 0;
  cmds::GetIntegerv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_ACTIVE_TEXTURE),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetIntegervInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetIntegerv(_, _)).Times(0);
  SpecializedSetup<cmds::GetIntegerv, 0>(false);
  cmds::GetIntegerv::Result* result =
      static_cast<cmds::GetIntegerv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetIntegerv cmd;
  cmd.Init(GL_FOG_HINT, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetIntegervInvalidArgs1_0) {
  EXPECT_CALL(*gl_, GetIntegerv(_, _)).Times(0);
  SpecializedSetup<cmds::GetIntegerv, 0>(false);
  cmds::GetIntegerv::Result* result =
      static_cast<cmds::GetIntegerv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetIntegerv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetIntegervInvalidArgs1_1) {
  EXPECT_CALL(*gl_, GetIntegerv(_, _)).Times(0);
  SpecializedSetup<cmds::GetIntegerv, 0>(false);
  cmds::GetIntegerv::Result* result =
      static_cast<cmds::GetIntegerv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetIntegerv cmd;
  cmd.Init(GL_ACTIVE_TEXTURE, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetProgramivValidArgs) {
  SpecializedSetup<cmds::GetProgramiv, 0>(true);
  typedef cmds::GetProgramiv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetProgramiv cmd;
  cmd.Init(client_program_id_,
           GL_DELETE_STATUS,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_DELETE_STATUS),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetProgramivInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetProgramiv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetProgramiv, 0>(false);
  cmds::GetProgramiv::Result* result =
      static_cast<cmds::GetProgramiv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetProgramiv cmd;
  cmd.Init(client_program_id_, GL_DELETE_STATUS, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetProgramivInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetProgramiv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetProgramiv, 0>(false);
  cmds::GetProgramiv::Result* result =
      static_cast<cmds::GetProgramiv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetProgramiv cmd;
  cmd.Init(client_program_id_,
           GL_DELETE_STATUS,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetProgramInfoLogValidArgs) {
  const char* kInfo = "hello";
  const uint32_t kBucketId = 123;
  SpecializedSetup<cmds::GetProgramInfoLog, 0>(true);

  cmds::GetProgramInfoLog cmd;
  cmd.Init(client_program_id_, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  CommonDecoder::Bucket* bucket = decoder_->GetBucket(kBucketId);
  ASSERT_TRUE(bucket != NULL);
  EXPECT_EQ(strlen(kInfo) + 1, bucket->size());
  EXPECT_EQ(0,
            memcmp(bucket->GetData(0, bucket->size()), kInfo, bucket->size()));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetProgramInfoLogInvalidArgs) {
  const uint32_t kBucketId = 123;
  cmds::GetProgramInfoLog cmd;
  cmd.Init(kInvalidClientId, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetRenderbufferParameterivValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetRenderbufferParameteriv, 0>(true);
  typedef cmds::GetRenderbufferParameteriv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(
      *gl_,
      GetRenderbufferParameterivEXT(
          GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, result->GetData()));
  result->size = 0;
  cmds::GetRenderbufferParameteriv cmd;
  cmd.Init(GL_RENDERBUFFER,
           GL_RENDERBUFFER_RED_SIZE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(
                GL_RENDERBUFFER_RED_SIZE),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetRenderbufferParameterivInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetRenderbufferParameterivEXT(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetRenderbufferParameteriv, 0>(false);
  cmds::GetRenderbufferParameteriv::Result* result =
      static_cast<cmds::GetRenderbufferParameteriv::Result*>(
          shared_memory_address_);
  result->size = 0;
  cmds::GetRenderbufferParameteriv cmd;
  cmd.Init(GL_FRAMEBUFFER,
           GL_RENDERBUFFER_RED_SIZE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetRenderbufferParameterivInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetRenderbufferParameterivEXT(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetRenderbufferParameteriv, 0>(false);
  cmds::GetRenderbufferParameteriv::Result* result =
      static_cast<cmds::GetRenderbufferParameteriv::Result*>(
          shared_memory_address_);
  result->size = 0;
  cmds::GetRenderbufferParameteriv cmd;
  cmd.Init(
      GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetRenderbufferParameterivInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetRenderbufferParameterivEXT(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetRenderbufferParameteriv, 0>(false);
  cmds::GetRenderbufferParameteriv::Result* result =
      static_cast<cmds::GetRenderbufferParameteriv::Result*>(
          shared_memory_address_);
  result->size = 0;
  cmds::GetRenderbufferParameteriv cmd;
  cmd.Init(GL_RENDERBUFFER,
           GL_RENDERBUFFER_RED_SIZE,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetShaderivValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetShaderiv, 0>(true);
  typedef cmds::GetShaderiv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(*gl_,
              GetShaderiv(kServiceShaderId, GL_SHADER_TYPE, result->GetData()));
  result->size = 0;
  cmds::GetShaderiv cmd;
  cmd.Init(client_shader_id_,
           GL_SHADER_TYPE,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_SHADER_TYPE),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetShaderivInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetShaderiv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetShaderiv, 0>(false);
  cmds::GetShaderiv::Result* result =
      static_cast<cmds::GetShaderiv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetShaderiv cmd;
  cmd.Init(client_shader_id_, GL_SHADER_TYPE, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetShaderivInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetShaderiv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetShaderiv, 0>(false);
  cmds::GetShaderiv::Result* result =
      static_cast<cmds::GetShaderiv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetShaderiv cmd;
  cmd.Init(client_shader_id_,
           GL_SHADER_TYPE,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}
// TODO(gman): GetShaderInfoLog
// TODO(gman): GetShaderPrecisionFormat

// TODO(gman): GetShaderSource
// TODO(gman): GetString

TEST_P(GLES2DecoderTest1, GetTexParameterfvValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetTexParameterfv, 0>(true);
  typedef cmds::GetTexParameterfv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(*gl_,
              GetTexParameterfv(
                  GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, result->GetData()));
  result->size = 0;
  cmds::GetTexParameterfv cmd;
  cmd.Init(GL_TEXTURE_2D,
           GL_TEXTURE_MAG_FILTER,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(
      decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_TEXTURE_MAG_FILTER),
      result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetTexParameterfvInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetTexParameterfv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameterfv, 0>(false);
  cmds::GetTexParameterfv::Result* result =
      static_cast<cmds::GetTexParameterfv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameterfv cmd;
  cmd.Init(GL_PROXY_TEXTURE_CUBE_MAP,
           GL_TEXTURE_MAG_FILTER,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetTexParameterfvInvalidArgs1_0) {
  EXPECT_CALL(*gl_, GetTexParameterfv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameterfv, 0>(false);
  cmds::GetTexParameterfv::Result* result =
      static_cast<cmds::GetTexParameterfv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameterfv cmd;
  cmd.Init(GL_TEXTURE_2D,
           GL_GENERATE_MIPMAP,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetTexParameterfvInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetTexParameterfv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameterfv, 0>(false);
  cmds::GetTexParameterfv::Result* result =
      static_cast<cmds::GetTexParameterfv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameterfv cmd;
  cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetTexParameterfvInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetTexParameterfv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameterfv, 0>(false);
  cmds::GetTexParameterfv::Result* result =
      static_cast<cmds::GetTexParameterfv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameterfv cmd;
  cmd.Init(GL_TEXTURE_2D,
           GL_TEXTURE_MAG_FILTER,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetTexParameterivValidArgs) {
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  SpecializedSetup<cmds::GetTexParameteriv, 0>(true);
  typedef cmds::GetTexParameteriv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  EXPECT_CALL(*gl_,
              GetTexParameteriv(
                  GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, result->GetData()));
  result->size = 0;
  cmds::GetTexParameteriv cmd;
  cmd.Init(GL_TEXTURE_2D,
           GL_TEXTURE_MAG_FILTER,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(
      decoder_->GetGLES2Util()->GLGetNumValuesReturned(GL_TEXTURE_MAG_FILTER),
      result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetTexParameterivInvalidArgs0_0) {
  EXPECT_CALL(*gl_, GetTexParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameteriv, 0>(false);
  cmds::GetTexParameteriv::Result* result =
      static_cast<cmds::GetTexParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameteriv cmd;
  cmd.Init(GL_PROXY_TEXTURE_CUBE_MAP,
           GL_TEXTURE_MAG_FILTER,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetTexParameterivInvalidArgs1_0) {
  EXPECT_CALL(*gl_, GetTexParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameteriv, 0>(false);
  cmds::GetTexParameteriv::Result* result =
      static_cast<cmds::GetTexParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameteriv cmd;
  cmd.Init(GL_TEXTURE_2D,
           GL_GENERATE_MIPMAP,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetTexParameterivInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetTexParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameteriv, 0>(false);
  cmds::GetTexParameteriv::Result* result =
      static_cast<cmds::GetTexParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameteriv cmd;
  cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetTexParameterivInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetTexParameteriv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetTexParameteriv, 0>(false);
  cmds::GetTexParameteriv::Result* result =
      static_cast<cmds::GetTexParameteriv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetTexParameteriv cmd;
  cmd.Init(GL_TEXTURE_2D,
           GL_TEXTURE_MAG_FILTER,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}
// TODO(gman): GetUniformfv

// TODO(gman): GetUniformiv

// TODO(gman): GetUniformLocation

TEST_P(GLES2DecoderTest1, GetVertexAttribfvValidArgs) {
  SpecializedSetup<cmds::GetVertexAttribfv, 0>(true);
  typedef cmds::GetVertexAttribfv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetVertexAttribfv cmd;
  cmd.Init(1,
           GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(
                GL_VERTEX_ATTRIB_ARRAY_NORMALIZED),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetVertexAttribfvInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetVertexAttribfv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetVertexAttribfv, 0>(false);
  cmds::GetVertexAttribfv::Result* result =
      static_cast<cmds::GetVertexAttribfv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetVertexAttribfv cmd;
  cmd.Init(1, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetVertexAttribfvInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetVertexAttribfv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetVertexAttribfv, 0>(false);
  cmds::GetVertexAttribfv::Result* result =
      static_cast<cmds::GetVertexAttribfv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetVertexAttribfv cmd;
  cmd.Init(1,
           GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetVertexAttribivValidArgs) {
  SpecializedSetup<cmds::GetVertexAttribiv, 0>(true);
  typedef cmds::GetVertexAttribiv::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetVertexAttribiv cmd;
  cmd.Init(1,
           GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(
                GL_VERTEX_ATTRIB_ARRAY_NORMALIZED),
            result->GetNumResults());
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, GetVertexAttribivInvalidArgs2_0) {
  EXPECT_CALL(*gl_, GetVertexAttribiv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetVertexAttribiv, 0>(false);
  cmds::GetVertexAttribiv::Result* result =
      static_cast<cmds::GetVertexAttribiv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetVertexAttribiv cmd;
  cmd.Init(1, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, kInvalidSharedMemoryId, 0);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}

TEST_P(GLES2DecoderTest1, GetVertexAttribivInvalidArgs2_1) {
  EXPECT_CALL(*gl_, GetVertexAttribiv(_, _, _)).Times(0);
  SpecializedSetup<cmds::GetVertexAttribiv, 0>(false);
  cmds::GetVertexAttribiv::Result* result =
      static_cast<cmds::GetVertexAttribiv::Result*>(shared_memory_address_);
  result->size = 0;
  cmds::GetVertexAttribiv cmd;
  cmd.Init(1,
           GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(0u, result->size);
}
// TODO(gman): GetVertexAttribPointerv

TEST_P(GLES2DecoderTest1, HintValidArgs) {
  EXPECT_CALL(*gl_, Hint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST));
  SpecializedSetup<cmds::Hint, 0>(true);
  cmds::Hint cmd;
  cmd.Init(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, HintInvalidArgs0_0) {
  EXPECT_CALL(*gl_, Hint(_, _)).Times(0);
  SpecializedSetup<cmds::Hint, 0>(false);
  cmds::Hint cmd;
  cmd.Init(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsBufferValidArgs) {
  SpecializedSetup<cmds::IsBuffer, 0>(true);
  cmds::IsBuffer cmd;
  cmd.Init(client_buffer_id_, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsBufferInvalidArgsBadSharedMemoryId) {
  SpecializedSetup<cmds::IsBuffer, 0>(false);
  cmds::IsBuffer cmd;
  cmd.Init(client_buffer_id_, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(client_buffer_id_, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, IsEnabledValidArgs) {
  SpecializedSetup<cmds::IsEnabled, 0>(true);
  cmds::IsEnabled cmd;
  cmd.Init(GL_BLEND, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsEnabledInvalidArgs0_0) {
  EXPECT_CALL(*gl_, IsEnabled(_)).Times(0);
  SpecializedSetup<cmds::IsEnabled, 0>(false);
  cmds::IsEnabled cmd;
  cmd.Init(GL_CLIP_PLANE0, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsEnabledInvalidArgs0_1) {
  EXPECT_CALL(*gl_, IsEnabled(_)).Times(0);
  SpecializedSetup<cmds::IsEnabled, 0>(false);
  cmds::IsEnabled cmd;
  cmd.Init(GL_POINT_SPRITE, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsEnabledInvalidArgsBadSharedMemoryId) {
  SpecializedSetup<cmds::IsEnabled, 0>(false);
  cmds::IsEnabled cmd;
  cmd.Init(GL_BLEND, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(GL_BLEND, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, IsFramebufferValidArgs) {
  SpecializedSetup<cmds::IsFramebuffer, 0>(true);
  cmds::IsFramebuffer cmd;
  cmd.Init(client_framebuffer_id_, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsFramebufferInvalidArgsBadSharedMemoryId) {
  SpecializedSetup<cmds::IsFramebuffer, 0>(false);
  cmds::IsFramebuffer cmd;
  cmd.Init(
      client_framebuffer_id_, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(
      client_framebuffer_id_, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, IsProgramValidArgs) {
  SpecializedSetup<cmds::IsProgram, 0>(true);
  cmds::IsProgram cmd;
  cmd.Init(client_program_id_, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsProgramInvalidArgsBadSharedMemoryId) {
  SpecializedSetup<cmds::IsProgram, 0>(false);
  cmds::IsProgram cmd;
  cmd.Init(client_program_id_, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(client_program_id_, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, IsRenderbufferValidArgs) {
  SpecializedSetup<cmds::IsRenderbuffer, 0>(true);
  cmds::IsRenderbuffer cmd;
  cmd.Init(client_renderbuffer_id_, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsRenderbufferInvalidArgsBadSharedMemoryId) {
  SpecializedSetup<cmds::IsRenderbuffer, 0>(false);
  cmds::IsRenderbuffer cmd;
  cmd.Init(
      client_renderbuffer_id_, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(
      client_renderbuffer_id_, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, IsShaderValidArgs) {
  SpecializedSetup<cmds::IsShader, 0>(true);
  cmds::IsShader cmd;
  cmd.Init(client_shader_id_, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsShaderInvalidArgsBadSharedMemoryId) {
  SpecializedSetup<cmds::IsShader, 0>(false);
  cmds::IsShader cmd;
  cmd.Init(client_shader_id_, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(client_shader_id_, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, IsTextureValidArgs) {
  SpecializedSetup<cmds::IsTexture, 0>(true);
  cmds::IsTexture cmd;
  cmd.Init(client_texture_id_, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, IsTextureInvalidArgsBadSharedMemoryId) {
  SpecializedSetup<cmds::IsTexture, 0>(false);
  cmds::IsTexture cmd;
  cmd.Init(client_texture_id_, kInvalidSharedMemoryId, shared_memory_offset_);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  cmd.Init(client_texture_id_, shared_memory_id_, kInvalidSharedMemoryOffset);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest1, LineWidthValidArgs) {
  EXPECT_CALL(*gl_, LineWidth(0.5f));
  SpecializedSetup<cmds::LineWidth, 0>(true);
  cmds::LineWidth cmd;
  cmd.Init(0.5f);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, LineWidthInvalidValue0_0) {
  SpecializedSetup<cmds::LineWidth, 0>(false);
  cmds::LineWidth cmd;
  cmd.Init(0.0f);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest1, LinkProgramValidArgs) {
  EXPECT_CALL(*gl_, LinkProgram(kServiceProgramId));
  SpecializedSetup<cmds::LinkProgram, 0>(true);
  cmds::LinkProgram cmd;
  cmd.Init(client_program_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): PixelStorei

TEST_P(GLES2DecoderTest1, PolygonOffsetValidArgs) {
  EXPECT_CALL(*gl_, PolygonOffset(1, 2));
  SpecializedSetup<cmds::PolygonOffset, 0>(true);
  cmds::PolygonOffset cmd;
  cmd.Init(1, 2);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): ReadPixels

// TODO(gman): ReleaseShaderCompiler

TEST_P(GLES2DecoderTest1, RenderbufferStorageValidArgs) {
  SpecializedSetup<cmds::RenderbufferStorage, 0>(true);
  cmds::RenderbufferStorage cmd;
  cmd.Init(GL_RENDERBUFFER, GL_RGBA4, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest1, RenderbufferStorageInvalidArgs0_0) {
  EXPECT_CALL(*gl_, RenderbufferStorageEXT(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::RenderbufferStorage, 0>(false);
  cmds::RenderbufferStorage cmd;
  cmd.Init(GL_FRAMEBUFFER, GL_RGBA4, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest1, RenderbufferStorageInvalidArgs2_0) {
  EXPECT_CALL(*gl_, RenderbufferStorageEXT(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::RenderbufferStorage, 0>(false);
  cmds::RenderbufferStorage cmd;
  cmd.Init(GL_RENDERBUFFER, GL_RGBA4, -1, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest1, RenderbufferStorageInvalidArgs3_0) {
  EXPECT_CALL(*gl_, RenderbufferStorageEXT(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::RenderbufferStorage, 0>(false);
  cmds::RenderbufferStorage cmd;
  cmd.Init(GL_RENDERBUFFER, GL_RGBA4, 3, -1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest1, SampleCoverageValidArgs) {
  EXPECT_CALL(*gl_, SampleCoverage(1, true));
  SpecializedSetup<cmds::SampleCoverage, 0>(true);
  cmds::SampleCoverage cmd;
  cmd.Init(1, true);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
#endif  // GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_1_AUTOGEN_H_
