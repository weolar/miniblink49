// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// It is included by gles2_cmd_decoder_unittest_2.cc
#ifndef GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_2_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_2_AUTOGEN_H_

TEST_P(GLES2DecoderTest2, ScissorValidArgs) {
  EXPECT_CALL(*gl_, Scissor(1, 2, 3, 4));
  SpecializedSetup<cmds::Scissor, 0>(true);
  cmds::Scissor cmd;
  cmd.Init(1, 2, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, ScissorInvalidArgs2_0) {
  EXPECT_CALL(*gl_, Scissor(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::Scissor, 0>(false);
  cmds::Scissor cmd;
  cmd.Init(1, 2, -1, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest2, ScissorInvalidArgs3_0) {
  EXPECT_CALL(*gl_, Scissor(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::Scissor, 0>(false);
  cmds::Scissor cmd;
  cmd.Init(1, 2, 3, -1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}
// TODO(gman): ShaderBinary

// TODO(gman): ShaderSourceBucket

TEST_P(GLES2DecoderTest2, StencilFuncValidArgs) {
  EXPECT_CALL(*gl_, StencilFunc(GL_NEVER, 2, 3));
  SpecializedSetup<cmds::StencilFunc, 0>(true);
  cmds::StencilFunc cmd;
  cmd.Init(GL_NEVER, 2, 3);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, StencilFuncSeparateValidArgs) {
  EXPECT_CALL(*gl_, StencilFuncSeparate(GL_FRONT, GL_NEVER, 3, 4));
  SpecializedSetup<cmds::StencilFuncSeparate, 0>(true);
  cmds::StencilFuncSeparate cmd;
  cmd.Init(GL_FRONT, GL_NEVER, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, StencilMaskValidArgs) {
  SpecializedSetup<cmds::StencilMask, 0>(true);
  cmds::StencilMask cmd;
  cmd.Init(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, StencilMaskSeparateValidArgs) {
  SpecializedSetup<cmds::StencilMaskSeparate, 0>(true);
  cmds::StencilMaskSeparate cmd;
  cmd.Init(GL_FRONT, 2);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, StencilOpValidArgs) {
  EXPECT_CALL(*gl_, StencilOp(GL_KEEP, GL_INCR, GL_KEEP));
  SpecializedSetup<cmds::StencilOp, 0>(true);
  cmds::StencilOp cmd;
  cmd.Init(GL_KEEP, GL_INCR, GL_KEEP);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, StencilOpSeparateValidArgs) {
  EXPECT_CALL(*gl_, StencilOpSeparate(GL_FRONT, GL_INCR, GL_KEEP, GL_KEEP));
  SpecializedSetup<cmds::StencilOpSeparate, 0>(true);
  cmds::StencilOpSeparate cmd;
  cmd.Init(GL_FRONT, GL_INCR, GL_KEEP, GL_KEEP);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): TexImage2D

TEST_P(GLES2DecoderTest2, TexParameterfValidArgs) {
  EXPECT_CALL(*gl_,
              TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  SpecializedSetup<cmds::TexParameterf, 0>(true);
  cmds::TexParameterf cmd;
  cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterfInvalidArgs0_0) {
  EXPECT_CALL(*gl_, TexParameterf(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterf, 0>(false);
  cmds::TexParameterf cmd;
  cmd.Init(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterfInvalidArgs0_1) {
  EXPECT_CALL(*gl_, TexParameterf(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterf, 0>(false);
  cmds::TexParameterf cmd;
  cmd.Init(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterfInvalidArgs1_0) {
  EXPECT_CALL(*gl_, TexParameterf(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterf, 0>(false);
  cmds::TexParameterf cmd;
  cmd.Init(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterfvImmediateValidArgs) {
  cmds::TexParameterfvImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterfvImmediate>();
  SpecializedSetup<cmds::TexParameterfvImmediate, 0>(true);
  GLfloat temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &temp[0]);
  EXPECT_CALL(
      *gl_,
      TexParameterf(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    *reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterfvImmediateInvalidArgs0_0) {
  cmds::TexParameterfvImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterfvImmediate>();
  EXPECT_CALL(*gl_, TexParameterf(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterfvImmediate, 0>(false);
  GLfloat temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterfvImmediateInvalidArgs0_1) {
  cmds::TexParameterfvImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterfvImmediate>();
  EXPECT_CALL(*gl_, TexParameterf(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterfvImmediate, 0>(false);
  GLfloat temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterfvImmediateInvalidArgs1_0) {
  cmds::TexParameterfvImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterfvImmediate>();
  EXPECT_CALL(*gl_, TexParameterf(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterfvImmediate, 0>(false);
  GLfloat temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameteriValidArgs) {
  EXPECT_CALL(*gl_,
              TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  SpecializedSetup<cmds::TexParameteri, 0>(true);
  cmds::TexParameteri cmd;
  cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameteriInvalidArgs0_0) {
  EXPECT_CALL(*gl_, TexParameteri(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameteri, 0>(false);
  cmds::TexParameteri cmd;
  cmd.Init(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameteriInvalidArgs0_1) {
  EXPECT_CALL(*gl_, TexParameteri(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameteri, 0>(false);
  cmds::TexParameteri cmd;
  cmd.Init(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameteriInvalidArgs1_0) {
  EXPECT_CALL(*gl_, TexParameteri(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameteri, 0>(false);
  cmds::TexParameteri cmd;
  cmd.Init(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_NEAREST);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterivImmediateValidArgs) {
  cmds::TexParameterivImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterivImmediate>();
  SpecializedSetup<cmds::TexParameterivImmediate, 0>(true);
  GLint temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &temp[0]);
  EXPECT_CALL(
      *gl_,
      TexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    *reinterpret_cast<GLint*>(ImmediateDataAddress(&cmd))));
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterivImmediateInvalidArgs0_0) {
  cmds::TexParameterivImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterivImmediate>();
  EXPECT_CALL(*gl_, TexParameteri(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterivImmediate, 0>(false);
  GLint temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterivImmediateInvalidArgs0_1) {
  cmds::TexParameterivImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterivImmediate>();
  EXPECT_CALL(*gl_, TexParameteri(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterivImmediate, 0>(false);
  GLint temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest2, TexParameterivImmediateInvalidArgs1_0) {
  cmds::TexParameterivImmediate& cmd =
      *GetImmediateAs<cmds::TexParameterivImmediate>();
  EXPECT_CALL(*gl_, TexParameteri(_, _, _)).Times(0);
  SpecializedSetup<cmds::TexParameterivImmediate, 0>(false);
  GLint temp[1] = {
      GL_NEAREST,
  };
  cmd.Init(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}
// TODO(gman): TexSubImage2D

TEST_P(GLES2DecoderTest2, Uniform1fValidArgs) {
  EXPECT_CALL(*gl_, Uniform1fv(1, 1, _));
  SpecializedSetup<cmds::Uniform1f, 0>(true);
  cmds::Uniform1f cmd;
  cmd.Init(1, 2);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform1fvImmediateValidArgs) {
  cmds::Uniform1fvImmediate& cmd = *GetImmediateAs<cmds::Uniform1fvImmediate>();
  EXPECT_CALL(
      *gl_,
      Uniform1fv(1, 2, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::Uniform1fvImmediate, 0>(true);
  GLfloat temp[1 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): Uniform1i
// TODO(gman): Uniform1ivImmediate

TEST_P(GLES2DecoderTest2, Uniform2fValidArgs) {
  EXPECT_CALL(*gl_, Uniform2fv(1, 1, _));
  SpecializedSetup<cmds::Uniform2f, 0>(true);
  cmds::Uniform2f cmd;
  cmd.Init(1, 2, 3);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform2fvImmediateValidArgs) {
  cmds::Uniform2fvImmediate& cmd = *GetImmediateAs<cmds::Uniform2fvImmediate>();
  EXPECT_CALL(
      *gl_,
      Uniform2fv(1, 2, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::Uniform2fvImmediate, 0>(true);
  GLfloat temp[2 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform2iValidArgs) {
  EXPECT_CALL(*gl_, Uniform2iv(1, 1, _));
  SpecializedSetup<cmds::Uniform2i, 0>(true);
  cmds::Uniform2i cmd;
  cmd.Init(1, 2, 3);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform2ivImmediateValidArgs) {
  cmds::Uniform2ivImmediate& cmd = *GetImmediateAs<cmds::Uniform2ivImmediate>();
  EXPECT_CALL(
      *gl_,
      Uniform2iv(1, 2, reinterpret_cast<GLint*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::Uniform2ivImmediate, 0>(true);
  GLint temp[2 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform3fValidArgs) {
  EXPECT_CALL(*gl_, Uniform3fv(1, 1, _));
  SpecializedSetup<cmds::Uniform3f, 0>(true);
  cmds::Uniform3f cmd;
  cmd.Init(1, 2, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform3fvImmediateValidArgs) {
  cmds::Uniform3fvImmediate& cmd = *GetImmediateAs<cmds::Uniform3fvImmediate>();
  EXPECT_CALL(
      *gl_,
      Uniform3fv(1, 2, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::Uniform3fvImmediate, 0>(true);
  GLfloat temp[3 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform3iValidArgs) {
  EXPECT_CALL(*gl_, Uniform3iv(1, 1, _));
  SpecializedSetup<cmds::Uniform3i, 0>(true);
  cmds::Uniform3i cmd;
  cmd.Init(1, 2, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform3ivImmediateValidArgs) {
  cmds::Uniform3ivImmediate& cmd = *GetImmediateAs<cmds::Uniform3ivImmediate>();
  EXPECT_CALL(
      *gl_,
      Uniform3iv(1, 2, reinterpret_cast<GLint*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::Uniform3ivImmediate, 0>(true);
  GLint temp[3 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform4fValidArgs) {
  EXPECT_CALL(*gl_, Uniform4fv(1, 1, _));
  SpecializedSetup<cmds::Uniform4f, 0>(true);
  cmds::Uniform4f cmd;
  cmd.Init(1, 2, 3, 4, 5);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform4fvImmediateValidArgs) {
  cmds::Uniform4fvImmediate& cmd = *GetImmediateAs<cmds::Uniform4fvImmediate>();
  EXPECT_CALL(
      *gl_,
      Uniform4fv(1, 2, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::Uniform4fvImmediate, 0>(true);
  GLfloat temp[4 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform4iValidArgs) {
  EXPECT_CALL(*gl_, Uniform4iv(1, 1, _));
  SpecializedSetup<cmds::Uniform4i, 0>(true);
  cmds::Uniform4i cmd;
  cmd.Init(1, 2, 3, 4, 5);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, Uniform4ivImmediateValidArgs) {
  cmds::Uniform4ivImmediate& cmd = *GetImmediateAs<cmds::Uniform4ivImmediate>();
  EXPECT_CALL(
      *gl_,
      Uniform4iv(1, 2, reinterpret_cast<GLint*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::Uniform4ivImmediate, 0>(true);
  GLint temp[4 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, UniformMatrix2fvImmediateValidArgs) {
  cmds::UniformMatrix2fvImmediate& cmd =
      *GetImmediateAs<cmds::UniformMatrix2fvImmediate>();
  EXPECT_CALL(
      *gl_,
      UniformMatrix2fv(
          1, 2, false, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::UniformMatrix2fvImmediate, 0>(true);
  GLfloat temp[4 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, UniformMatrix3fvImmediateValidArgs) {
  cmds::UniformMatrix3fvImmediate& cmd =
      *GetImmediateAs<cmds::UniformMatrix3fvImmediate>();
  EXPECT_CALL(
      *gl_,
      UniformMatrix3fv(
          1, 2, false, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::UniformMatrix3fvImmediate, 0>(true);
  GLfloat temp[9 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, UniformMatrix4fvImmediateValidArgs) {
  cmds::UniformMatrix4fvImmediate& cmd =
      *GetImmediateAs<cmds::UniformMatrix4fvImmediate>();
  EXPECT_CALL(
      *gl_,
      UniformMatrix4fv(
          1, 2, false, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  SpecializedSetup<cmds::UniformMatrix4fvImmediate, 0>(true);
  GLfloat temp[16 * 2] = {
      0,
  };
  cmd.Init(1, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, UseProgramValidArgs) {
  EXPECT_CALL(*gl_, UseProgram(kServiceProgramId));
  SpecializedSetup<cmds::UseProgram, 0>(true);
  cmds::UseProgram cmd;
  cmd.Init(client_program_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, UseProgramInvalidArgs0_0) {
  EXPECT_CALL(*gl_, UseProgram(_)).Times(0);
  SpecializedSetup<cmds::UseProgram, 0>(false);
  cmds::UseProgram cmd;
  cmd.Init(kInvalidClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest2, ValidateProgramValidArgs) {
  EXPECT_CALL(*gl_, ValidateProgram(kServiceProgramId));
  SpecializedSetup<cmds::ValidateProgram, 0>(true);
  cmds::ValidateProgram cmd;
  cmd.Init(client_program_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib1fValidArgs) {
  EXPECT_CALL(*gl_, VertexAttrib1f(1, 2));
  SpecializedSetup<cmds::VertexAttrib1f, 0>(true);
  cmds::VertexAttrib1f cmd;
  cmd.Init(1, 2);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib1fvImmediateValidArgs) {
  cmds::VertexAttrib1fvImmediate& cmd =
      *GetImmediateAs<cmds::VertexAttrib1fvImmediate>();
  SpecializedSetup<cmds::VertexAttrib1fvImmediate, 0>(true);
  GLfloat temp[1] = {
      0,
  };
  cmd.Init(1, &temp[0]);
  EXPECT_CALL(*gl_,
              VertexAttrib1fv(
                  1, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib2fValidArgs) {
  EXPECT_CALL(*gl_, VertexAttrib2f(1, 2, 3));
  SpecializedSetup<cmds::VertexAttrib2f, 0>(true);
  cmds::VertexAttrib2f cmd;
  cmd.Init(1, 2, 3);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib2fvImmediateValidArgs) {
  cmds::VertexAttrib2fvImmediate& cmd =
      *GetImmediateAs<cmds::VertexAttrib2fvImmediate>();
  SpecializedSetup<cmds::VertexAttrib2fvImmediate, 0>(true);
  GLfloat temp[2] = {
      0,
  };
  cmd.Init(1, &temp[0]);
  EXPECT_CALL(*gl_,
              VertexAttrib2fv(
                  1, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib3fValidArgs) {
  EXPECT_CALL(*gl_, VertexAttrib3f(1, 2, 3, 4));
  SpecializedSetup<cmds::VertexAttrib3f, 0>(true);
  cmds::VertexAttrib3f cmd;
  cmd.Init(1, 2, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib3fvImmediateValidArgs) {
  cmds::VertexAttrib3fvImmediate& cmd =
      *GetImmediateAs<cmds::VertexAttrib3fvImmediate>();
  SpecializedSetup<cmds::VertexAttrib3fvImmediate, 0>(true);
  GLfloat temp[3] = {
      0,
  };
  cmd.Init(1, &temp[0]);
  EXPECT_CALL(*gl_,
              VertexAttrib3fv(
                  1, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib4fValidArgs) {
  EXPECT_CALL(*gl_, VertexAttrib4f(1, 2, 3, 4, 5));
  SpecializedSetup<cmds::VertexAttrib4f, 0>(true);
  cmds::VertexAttrib4f cmd;
  cmd.Init(1, 2, 3, 4, 5);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, VertexAttrib4fvImmediateValidArgs) {
  cmds::VertexAttrib4fvImmediate& cmd =
      *GetImmediateAs<cmds::VertexAttrib4fvImmediate>();
  SpecializedSetup<cmds::VertexAttrib4fvImmediate, 0>(true);
  GLfloat temp[4] = {
      0,
  };
  cmd.Init(1, &temp[0]);
  EXPECT_CALL(*gl_,
              VertexAttrib4fv(
                  1, reinterpret_cast<GLfloat*>(ImmediateDataAddress(&cmd))));
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): VertexAttribPointer

TEST_P(GLES2DecoderTest2, ViewportValidArgs) {
  EXPECT_CALL(*gl_, Viewport(1, 2, 3, 4));
  SpecializedSetup<cmds::Viewport, 0>(true);
  cmds::Viewport cmd;
  cmd.Init(1, 2, 3, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest2, ViewportInvalidArgs2_0) {
  EXPECT_CALL(*gl_, Viewport(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::Viewport, 0>(false);
  cmds::Viewport cmd;
  cmd.Init(1, 2, -1, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest2, ViewportInvalidArgs3_0) {
  EXPECT_CALL(*gl_, Viewport(_, _, _, _)).Times(0);
  SpecializedSetup<cmds::Viewport, 0>(false);
  cmds::Viewport cmd;
  cmd.Init(1, 2, 3, -1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}
// TODO(gman): BlitFramebufferCHROMIUM
// TODO(gman): RenderbufferStorageMultisampleCHROMIUM
// TODO(gman): RenderbufferStorageMultisampleEXT
// TODO(gman): FramebufferTexture2DMultisampleEXT
// TODO(gman): TexStorage2DEXT
// TODO(gman): GenQueriesEXTImmediate
// TODO(gman): DeleteQueriesEXTImmediate
// TODO(gman): BeginQueryEXT

// TODO(gman): EndQueryEXT

// TODO(gman): InsertEventMarkerEXT

// TODO(gman): PushGroupMarkerEXT

TEST_P(GLES2DecoderTest2, PopGroupMarkerEXTValidArgs) {
  SpecializedSetup<cmds::PopGroupMarkerEXT, 0>(true);
  cmds::PopGroupMarkerEXT cmd;
  cmd.Init();
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}
// TODO(gman): GenVertexArraysOESImmediate
// TODO(gman): DeleteVertexArraysOESImmediate
// TODO(gman): IsVertexArrayOES
// TODO(gman): BindVertexArrayOES
// TODO(gman): SwapBuffers
// TODO(gman): GetMaxValueInBufferCHROMIUM
// TODO(gman): GenSharedIdsCHROMIUM

// TODO(gman): DeleteSharedIdsCHROMIUM

// TODO(gman): RegisterSharedIdsCHROMIUM

// TODO(gman): EnableFeatureCHROMIUM

// TODO(gman): ResizeCHROMIUM
// TODO(gman): GetRequestableExtensionsCHROMIUM

// TODO(gman): RequestExtensionCHROMIUM

// TODO(gman): GetMultipleIntegervCHROMIUM

// TODO(gman): GetProgramInfoCHROMIUM

// TODO(gman): GetTranslatedShaderSourceANGLE
// TODO(gman): PostSubBufferCHROMIUM
// TODO(gman): TexImageIOSurface2DCHROMIUM
// TODO(gman): CopyTextureCHROMIUM
// TODO(gman): DrawArraysInstancedANGLE
// TODO(gman): DrawElementsInstancedANGLE
// TODO(gman): VertexAttribDivisorANGLE
// TODO(gman): GenMailboxCHROMIUM

// TODO(gman): ProduceTextureCHROMIUMImmediate
// TODO(gman): ProduceTextureDirectCHROMIUMImmediate
// TODO(gman): ConsumeTextureCHROMIUMImmediate
// TODO(gman): CreateAndConsumeTextureCHROMIUMImmediate
// TODO(gman): BindUniformLocationCHROMIUMBucket
// TODO(gman): BindTexImage2DCHROMIUM
// TODO(gman): ReleaseTexImage2DCHROMIUM
// TODO(gman): TraceBeginCHROMIUM

// TODO(gman): TraceEndCHROMIUM
// TODO(gman): AsyncTexSubImage2DCHROMIUM

// TODO(gman): AsyncTexImage2DCHROMIUM

// TODO(gman): WaitAsyncTexImage2DCHROMIUM

// TODO(gman): WaitAllAsyncTexImage2DCHROMIUM

// TODO(gman): DiscardFramebufferEXTImmediate
// TODO(gman): LoseContextCHROMIUM
// TODO(gman): InsertSyncPointCHROMIUM

// TODO(gman): WaitSyncPointCHROMIUM

#endif  // GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_2_AUTOGEN_H_
