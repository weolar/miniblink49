// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// This file is included by gles2_implementation.h to declare the
// GL api functions.
#ifndef GPU_COMMAND_BUFFER_CLIENT_GLES2_IMPLEMENTATION_UNITTEST_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_CLIENT_GLES2_IMPLEMENTATION_UNITTEST_AUTOGEN_H_

TEST_F(GLES2ImplementationTest, AttachShader) {
  struct Cmds {
    cmds::AttachShader cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->AttachShader(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}
// TODO: Implement unit test for BindAttribLocation

TEST_F(GLES2ImplementationTest, BindBuffer) {
  struct Cmds {
    cmds::BindBuffer cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_ARRAY_BUFFER, 2);

  gl_->BindBuffer(GL_ARRAY_BUFFER, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  ClearCommands();
  gl_->BindBuffer(GL_ARRAY_BUFFER, 2);
  EXPECT_TRUE(NoCommandsWritten());
}

TEST_F(GLES2ImplementationTest, BindFramebuffer) {
  struct Cmds {
    cmds::BindFramebuffer cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRAMEBUFFER, 2);

  gl_->BindFramebuffer(GL_FRAMEBUFFER, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  ClearCommands();
  gl_->BindFramebuffer(GL_FRAMEBUFFER, 2);
  EXPECT_TRUE(NoCommandsWritten());
}

TEST_F(GLES2ImplementationTest, BindRenderbuffer) {
  struct Cmds {
    cmds::BindRenderbuffer cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_RENDERBUFFER, 2);

  gl_->BindRenderbuffer(GL_RENDERBUFFER, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  ClearCommands();
  gl_->BindRenderbuffer(GL_RENDERBUFFER, 2);
  EXPECT_TRUE(NoCommandsWritten());
}

TEST_F(GLES2ImplementationTest, BlendColor) {
  struct Cmds {
    cmds::BlendColor cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4);

  gl_->BlendColor(1, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, BlendEquation) {
  struct Cmds {
    cmds::BlendEquation cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FUNC_SUBTRACT);

  gl_->BlendEquation(GL_FUNC_SUBTRACT);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, BlendEquationSeparate) {
  struct Cmds {
    cmds::BlendEquationSeparate cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FUNC_SUBTRACT, GL_FUNC_ADD);

  gl_->BlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_ADD);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, BlendFunc) {
  struct Cmds {
    cmds::BlendFunc cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_ZERO, GL_ZERO);

  gl_->BlendFunc(GL_ZERO, GL_ZERO);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, BlendFuncSeparate) {
  struct Cmds {
    cmds::BlendFuncSeparate cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO);

  gl_->BlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, CheckFramebufferStatus) {
  struct Cmds {
    cmds::CheckFramebufferStatus cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::CheckFramebufferStatus::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->CheckFramebufferStatus(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, Clear) {
  struct Cmds {
    cmds::Clear cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->Clear(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, ClearColor) {
  struct Cmds {
    cmds::ClearColor cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4);

  gl_->ClearColor(1, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, ClearDepthf) {
  struct Cmds {
    cmds::ClearDepthf cmd;
  };
  Cmds expected;
  expected.cmd.Init(0.5f);

  gl_->ClearDepthf(0.5f);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, ClearStencil) {
  struct Cmds {
    cmds::ClearStencil cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->ClearStencil(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, ColorMask) {
  struct Cmds {
    cmds::ColorMask cmd;
  };
  Cmds expected;
  expected.cmd.Init(true, true, true, true);

  gl_->ColorMask(true, true, true, true);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, CompileShader) {
  struct Cmds {
    cmds::CompileShader cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->CompileShader(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}
// TODO: Implement unit test for CompressedTexImage2D
// TODO: Implement unit test for CompressedTexSubImage2D

TEST_F(GLES2ImplementationTest, CopyTexImage2D) {
  struct Cmds {
    cmds::CopyTexImage2D cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, 2, GL_ALPHA, 4, 5, 6, 7);

  gl_->CopyTexImage2D(GL_TEXTURE_2D, 2, GL_ALPHA, 4, 5, 6, 7, 0);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, CopyTexImage2DInvalidConstantArg7) {
  gl_->CopyTexImage2D(GL_TEXTURE_2D, 2, GL_ALPHA, 4, 5, 6, 7, 1);
  EXPECT_TRUE(NoCommandsWritten());
  EXPECT_EQ(GL_INVALID_VALUE, CheckError());
}

TEST_F(GLES2ImplementationTest, CopyTexSubImage2D) {
  struct Cmds {
    cmds::CopyTexSubImage2D cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, 2, 3, 4, 5, 6, 7, 8);

  gl_->CopyTexSubImage2D(GL_TEXTURE_2D, 2, 3, 4, 5, 6, 7, 8);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, CullFace) {
  struct Cmds {
    cmds::CullFace cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRONT);

  gl_->CullFace(GL_FRONT);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DeleteBuffers) {
  GLuint ids[2] = {kBuffersStartId, kBuffersStartId + 1};
  struct Cmds {
    cmds::DeleteBuffersImmediate del;
    GLuint data[2];
  };
  Cmds expected;
  expected.del.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kBuffersStartId;
  expected.data[1] = kBuffersStartId + 1;
  gl_->DeleteBuffers(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DeleteFramebuffers) {
  GLuint ids[2] = {kFramebuffersStartId, kFramebuffersStartId + 1};
  struct Cmds {
    cmds::DeleteFramebuffersImmediate del;
    GLuint data[2];
  };
  Cmds expected;
  expected.del.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kFramebuffersStartId;
  expected.data[1] = kFramebuffersStartId + 1;
  gl_->DeleteFramebuffers(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DeleteProgram) {
  struct Cmds {
    cmds::DeleteProgram cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->DeleteProgram(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DeleteRenderbuffers) {
  GLuint ids[2] = {kRenderbuffersStartId, kRenderbuffersStartId + 1};
  struct Cmds {
    cmds::DeleteRenderbuffersImmediate del;
    GLuint data[2];
  };
  Cmds expected;
  expected.del.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kRenderbuffersStartId;
  expected.data[1] = kRenderbuffersStartId + 1;
  gl_->DeleteRenderbuffers(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DeleteShader) {
  struct Cmds {
    cmds::DeleteShader cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->DeleteShader(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DeleteTextures) {
  GLuint ids[2] = {kTexturesStartId, kTexturesStartId + 1};
  struct Cmds {
    cmds::DeleteTexturesImmediate del;
    GLuint data[2];
  };
  Cmds expected;
  expected.del.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kTexturesStartId;
  expected.data[1] = kTexturesStartId + 1;
  gl_->DeleteTextures(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DepthFunc) {
  struct Cmds {
    cmds::DepthFunc cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_NEVER);

  gl_->DepthFunc(GL_NEVER);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DepthMask) {
  struct Cmds {
    cmds::DepthMask cmd;
  };
  Cmds expected;
  expected.cmd.Init(true);

  gl_->DepthMask(true);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DepthRangef) {
  struct Cmds {
    cmds::DepthRangef cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->DepthRangef(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DetachShader) {
  struct Cmds {
    cmds::DetachShader cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->DetachShader(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DisableVertexAttribArray) {
  struct Cmds {
    cmds::DisableVertexAttribArray cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->DisableVertexAttribArray(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DrawArrays) {
  struct Cmds {
    cmds::DrawArrays cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_POINTS, 2, 3);

  gl_->DrawArrays(GL_POINTS, 2, 3);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, EnableVertexAttribArray) {
  struct Cmds {
    cmds::EnableVertexAttribArray cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->EnableVertexAttribArray(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Flush) {
  struct Cmds {
    cmds::Flush cmd;
  };
  Cmds expected;
  expected.cmd.Init();

  gl_->Flush();
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, FramebufferRenderbuffer) {
  struct Cmds {
    cmds::FramebufferRenderbuffer cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 4);

  gl_->FramebufferRenderbuffer(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, FramebufferTexture2D) {
  struct Cmds {
    cmds::FramebufferTexture2D cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 4);

  gl_->FramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 4, 0);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, FramebufferTexture2DInvalidConstantArg4) {
  gl_->FramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 4, 1);
  EXPECT_TRUE(NoCommandsWritten());
  EXPECT_EQ(GL_INVALID_VALUE, CheckError());
}

TEST_F(GLES2ImplementationTest, FrontFace) {
  struct Cmds {
    cmds::FrontFace cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_CW);

  gl_->FrontFace(GL_CW);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, GenBuffers) {
  GLuint ids[2] = {
      0,
  };
  struct Cmds {
    cmds::GenBuffersImmediate gen;
    GLuint data[2];
  };
  Cmds expected;
  expected.gen.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kBuffersStartId;
  expected.data[1] = kBuffersStartId + 1;
  gl_->GenBuffers(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(kBuffersStartId, ids[0]);
  EXPECT_EQ(kBuffersStartId + 1, ids[1]);
}

TEST_F(GLES2ImplementationTest, GenerateMipmap) {
  struct Cmds {
    cmds::GenerateMipmap cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D);

  gl_->GenerateMipmap(GL_TEXTURE_2D);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, GenFramebuffers) {
  GLuint ids[2] = {
      0,
  };
  struct Cmds {
    cmds::GenFramebuffersImmediate gen;
    GLuint data[2];
  };
  Cmds expected;
  expected.gen.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kFramebuffersStartId;
  expected.data[1] = kFramebuffersStartId + 1;
  gl_->GenFramebuffers(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(kFramebuffersStartId, ids[0]);
  EXPECT_EQ(kFramebuffersStartId + 1, ids[1]);
}

TEST_F(GLES2ImplementationTest, GenRenderbuffers) {
  GLuint ids[2] = {
      0,
  };
  struct Cmds {
    cmds::GenRenderbuffersImmediate gen;
    GLuint data[2];
  };
  Cmds expected;
  expected.gen.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kRenderbuffersStartId;
  expected.data[1] = kRenderbuffersStartId + 1;
  gl_->GenRenderbuffers(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(kRenderbuffersStartId, ids[0]);
  EXPECT_EQ(kRenderbuffersStartId + 1, ids[1]);
}

TEST_F(GLES2ImplementationTest, GenTextures) {
  GLuint ids[2] = {
      0,
  };
  struct Cmds {
    cmds::GenTexturesImmediate gen;
    GLuint data[2];
  };
  Cmds expected;
  expected.gen.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kTexturesStartId;
  expected.data[1] = kTexturesStartId + 1;
  gl_->GenTextures(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(kTexturesStartId, ids[0]);
  EXPECT_EQ(kTexturesStartId + 1, ids[1]);
}
// TODO: Implement unit test for GetActiveAttrib
// TODO: Implement unit test for GetActiveUniform
// TODO: Implement unit test for GetAttachedShaders
// TODO: Implement unit test for GetAttribLocation

TEST_F(GLES2ImplementationTest, GetBooleanv) {
  struct Cmds {
    cmds::GetBooleanv cmd;
  };
  typedef cmds::GetBooleanv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetBooleanv(123, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetBufferParameteriv) {
  struct Cmds {
    cmds::GetBufferParameteriv cmd;
  };
  typedef cmds::GetBufferParameteriv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, GL_BUFFER_SIZE, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetBufferParameteriv(123, GL_BUFFER_SIZE, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetFloatv) {
  struct Cmds {
    cmds::GetFloatv cmd;
  };
  typedef cmds::GetFloatv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetFloatv(123, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetFramebufferAttachmentParameteriv) {
  struct Cmds {
    cmds::GetFramebufferAttachmentParameteriv cmd;
  };
  typedef cmds::GetFramebufferAttachmentParameteriv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123,
                    GL_COLOR_ATTACHMENT0,
                    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                    result1.id,
                    result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetFramebufferAttachmentParameteriv(
      123,
      GL_COLOR_ATTACHMENT0,
      GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
      &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetIntegerv) {
  struct Cmds {
    cmds::GetIntegerv cmd;
  };
  typedef cmds::GetIntegerv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetIntegerv(123, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetProgramiv) {
  struct Cmds {
    cmds::GetProgramiv cmd;
  };
  typedef cmds::GetProgramiv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, GL_DELETE_STATUS, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetProgramiv(123, GL_DELETE_STATUS, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}
// TODO: Implement unit test for GetProgramInfoLog

TEST_F(GLES2ImplementationTest, GetRenderbufferParameteriv) {
  struct Cmds {
    cmds::GetRenderbufferParameteriv cmd;
  };
  typedef cmds::GetRenderbufferParameteriv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, GL_RENDERBUFFER_RED_SIZE, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetRenderbufferParameteriv(123, GL_RENDERBUFFER_RED_SIZE, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetShaderiv) {
  struct Cmds {
    cmds::GetShaderiv cmd;
  };
  typedef cmds::GetShaderiv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, GL_SHADER_TYPE, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetShaderiv(123, GL_SHADER_TYPE, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}
// TODO: Implement unit test for GetShaderInfoLog
// TODO: Implement unit test for GetShaderPrecisionFormat

TEST_F(GLES2ImplementationTest, GetTexParameterfv) {
  struct Cmds {
    cmds::GetTexParameterfv cmd;
  };
  typedef cmds::GetTexParameterfv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, GL_TEXTURE_MAG_FILTER, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetTexParameterfv(123, GL_TEXTURE_MAG_FILTER, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetTexParameteriv) {
  struct Cmds {
    cmds::GetTexParameteriv cmd;
  };
  typedef cmds::GetTexParameteriv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(123, GL_TEXTURE_MAG_FILTER, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetTexParameteriv(123, GL_TEXTURE_MAG_FILTER, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}
// TODO: Implement unit test for GetUniformfv
// TODO: Implement unit test for GetUniformiv
// TODO: Implement unit test for GetUniformLocation

TEST_F(GLES2ImplementationTest, GetVertexAttribfv) {
  struct Cmds {
    cmds::GetVertexAttribfv cmd;
  };
  typedef cmds::GetVertexAttribfv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(
      123, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetVertexAttribfv(123, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, GetVertexAttribiv) {
  struct Cmds {
    cmds::GetVertexAttribiv cmd;
  };
  typedef cmds::GetVertexAttribiv::Result Result;
  Result::Type result = 0;
  Cmds expected;
  ExpectedMemoryInfo result1 = GetExpectedResultMemory(4);
  expected.cmd.Init(
      123, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, result1.id, result1.offset);
  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, SizedResultHelper<Result::Type>(1)))
      .RetiresOnSaturation();
  gl_->GetVertexAttribiv(123, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &result);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(static_cast<Result::Type>(1), result);
}

TEST_F(GLES2ImplementationTest, Hint) {
  struct Cmds {
    cmds::Hint cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);

  gl_->Hint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, IsBuffer) {
  struct Cmds {
    cmds::IsBuffer cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsBuffer::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsBuffer(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, IsEnabled) {
  struct Cmds {
    cmds::IsEnabled cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsEnabled::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsEnabled(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, IsFramebuffer) {
  struct Cmds {
    cmds::IsFramebuffer cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsFramebuffer::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsFramebuffer(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, IsProgram) {
  struct Cmds {
    cmds::IsProgram cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsProgram::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsProgram(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, IsRenderbuffer) {
  struct Cmds {
    cmds::IsRenderbuffer cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsRenderbuffer::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsRenderbuffer(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, IsShader) {
  struct Cmds {
    cmds::IsShader cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsShader::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsShader(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, IsTexture) {
  struct Cmds {
    cmds::IsTexture cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsTexture::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsTexture(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}

TEST_F(GLES2ImplementationTest, LineWidth) {
  struct Cmds {
    cmds::LineWidth cmd;
  };
  Cmds expected;
  expected.cmd.Init(0.5f);

  gl_->LineWidth(0.5f);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, LinkProgram) {
  struct Cmds {
    cmds::LinkProgram cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->LinkProgram(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, PixelStorei) {
  struct Cmds {
    cmds::PixelStorei cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_PACK_ALIGNMENT, 1);

  gl_->PixelStorei(GL_PACK_ALIGNMENT, 1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, PolygonOffset) {
  struct Cmds {
    cmds::PolygonOffset cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->PolygonOffset(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, ReleaseShaderCompiler) {
  struct Cmds {
    cmds::ReleaseShaderCompiler cmd;
  };
  Cmds expected;
  expected.cmd.Init();

  gl_->ReleaseShaderCompiler();
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, RenderbufferStorage) {
  struct Cmds {
    cmds::RenderbufferStorage cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_RENDERBUFFER, GL_RGBA4, 3, 4);

  gl_->RenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, SampleCoverage) {
  struct Cmds {
    cmds::SampleCoverage cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, true);

  gl_->SampleCoverage(1, true);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Scissor) {
  struct Cmds {
    cmds::Scissor cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4);

  gl_->Scissor(1, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, StencilFunc) {
  struct Cmds {
    cmds::StencilFunc cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_NEVER, 2, 3);

  gl_->StencilFunc(GL_NEVER, 2, 3);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, StencilFuncSeparate) {
  struct Cmds {
    cmds::StencilFuncSeparate cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRONT, GL_NEVER, 3, 4);

  gl_->StencilFuncSeparate(GL_FRONT, GL_NEVER, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, StencilMask) {
  struct Cmds {
    cmds::StencilMask cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->StencilMask(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, StencilMaskSeparate) {
  struct Cmds {
    cmds::StencilMaskSeparate cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRONT, 2);

  gl_->StencilMaskSeparate(GL_FRONT, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, StencilOp) {
  struct Cmds {
    cmds::StencilOp cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_KEEP, GL_INCR, GL_KEEP);

  gl_->StencilOp(GL_KEEP, GL_INCR, GL_KEEP);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, StencilOpSeparate) {
  struct Cmds {
    cmds::StencilOpSeparate cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRONT, GL_INCR, GL_KEEP, GL_KEEP);

  gl_->StencilOpSeparate(GL_FRONT, GL_INCR, GL_KEEP, GL_KEEP);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, TexParameterf) {
  struct Cmds {
    cmds::TexParameterf cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  gl_->TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, TexParameterfv) {
  GLfloat data[1] = {0};
  struct Cmds {
    cmds::TexParameterfvImmediate cmd;
    GLfloat data[1];
  };

  for (int jj = 0; jj < 1; ++jj) {
    data[jj] = static_cast<GLfloat>(jj);
  }
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &data[0]);
  gl_->TexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &data[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, TexParameteri) {
  struct Cmds {
    cmds::TexParameteri cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  gl_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, TexParameteriv) {
  GLint data[1] = {0};
  struct Cmds {
    cmds::TexParameterivImmediate cmd;
    GLint data[1];
  };

  for (int jj = 0; jj < 1; ++jj) {
    data[jj] = static_cast<GLint>(jj);
  }
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &data[0]);
  gl_->TexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &data[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform1f) {
  struct Cmds {
    cmds::Uniform1f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->Uniform1f(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform1fv) {
  GLfloat data[2][1] = {{0}};
  struct Cmds {
    cmds::Uniform1fvImmediate cmd;
    GLfloat data[2][1];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 1; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 1 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform1fv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform1i) {
  struct Cmds {
    cmds::Uniform1i cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->Uniform1i(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform1iv) {
  GLint data[2][1] = {{0}};
  struct Cmds {
    cmds::Uniform1ivImmediate cmd;
    GLint data[2][1];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 1; ++jj) {
      data[ii][jj] = static_cast<GLint>(ii * 1 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform1iv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform2f) {
  struct Cmds {
    cmds::Uniform2f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3);

  gl_->Uniform2f(1, 2, 3);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform2fv) {
  GLfloat data[2][2] = {{0}};
  struct Cmds {
    cmds::Uniform2fvImmediate cmd;
    GLfloat data[2][2];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 2; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 2 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform2fv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform2i) {
  struct Cmds {
    cmds::Uniform2i cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3);

  gl_->Uniform2i(1, 2, 3);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform2iv) {
  GLint data[2][2] = {{0}};
  struct Cmds {
    cmds::Uniform2ivImmediate cmd;
    GLint data[2][2];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 2; ++jj) {
      data[ii][jj] = static_cast<GLint>(ii * 2 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform2iv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform3f) {
  struct Cmds {
    cmds::Uniform3f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4);

  gl_->Uniform3f(1, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform3fv) {
  GLfloat data[2][3] = {{0}};
  struct Cmds {
    cmds::Uniform3fvImmediate cmd;
    GLfloat data[2][3];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 3; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 3 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform3fv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform3i) {
  struct Cmds {
    cmds::Uniform3i cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4);

  gl_->Uniform3i(1, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform3iv) {
  GLint data[2][3] = {{0}};
  struct Cmds {
    cmds::Uniform3ivImmediate cmd;
    GLint data[2][3];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 3; ++jj) {
      data[ii][jj] = static_cast<GLint>(ii * 3 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform3iv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform4f) {
  struct Cmds {
    cmds::Uniform4f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4, 5);

  gl_->Uniform4f(1, 2, 3, 4, 5);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform4fv) {
  GLfloat data[2][4] = {{0}};
  struct Cmds {
    cmds::Uniform4fvImmediate cmd;
    GLfloat data[2][4];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 4; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 4 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform4fv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform4i) {
  struct Cmds {
    cmds::Uniform4i cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4, 5);

  gl_->Uniform4i(1, 2, 3, 4, 5);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Uniform4iv) {
  GLint data[2][4] = {{0}};
  struct Cmds {
    cmds::Uniform4ivImmediate cmd;
    GLint data[2][4];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 4; ++jj) {
      data[ii][jj] = static_cast<GLint>(ii * 4 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->Uniform4iv(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, UniformMatrix2fv) {
  GLfloat data[2][4] = {{0}};
  struct Cmds {
    cmds::UniformMatrix2fvImmediate cmd;
    GLfloat data[2][4];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 4; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 4 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->UniformMatrix2fv(1, 2, false, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, UniformMatrix2fvInvalidConstantArg2) {
  GLfloat data[2][4] = {{0}};
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 4; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 4 + jj);
    }
  }
  gl_->UniformMatrix2fv(1, 2, true, &data[0][0]);
  EXPECT_TRUE(NoCommandsWritten());
  EXPECT_EQ(GL_INVALID_VALUE, CheckError());
}

TEST_F(GLES2ImplementationTest, UniformMatrix3fv) {
  GLfloat data[2][9] = {{0}};
  struct Cmds {
    cmds::UniformMatrix3fvImmediate cmd;
    GLfloat data[2][9];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 9; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 9 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->UniformMatrix3fv(1, 2, false, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, UniformMatrix3fvInvalidConstantArg2) {
  GLfloat data[2][9] = {{0}};
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 9; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 9 + jj);
    }
  }
  gl_->UniformMatrix3fv(1, 2, true, &data[0][0]);
  EXPECT_TRUE(NoCommandsWritten());
  EXPECT_EQ(GL_INVALID_VALUE, CheckError());
}

TEST_F(GLES2ImplementationTest, UniformMatrix4fv) {
  GLfloat data[2][16] = {{0}};
  struct Cmds {
    cmds::UniformMatrix4fvImmediate cmd;
    GLfloat data[2][16];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 16; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 16 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->UniformMatrix4fv(1, 2, false, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, UniformMatrix4fvInvalidConstantArg2) {
  GLfloat data[2][16] = {{0}};
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 16; ++jj) {
      data[ii][jj] = static_cast<GLfloat>(ii * 16 + jj);
    }
  }
  gl_->UniformMatrix4fv(1, 2, true, &data[0][0]);
  EXPECT_TRUE(NoCommandsWritten());
  EXPECT_EQ(GL_INVALID_VALUE, CheckError());
}

TEST_F(GLES2ImplementationTest, UseProgram) {
  struct Cmds {
    cmds::UseProgram cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->UseProgram(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  ClearCommands();
  gl_->UseProgram(1);
  EXPECT_TRUE(NoCommandsWritten());
}

TEST_F(GLES2ImplementationTest, ValidateProgram) {
  struct Cmds {
    cmds::ValidateProgram cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->ValidateProgram(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib1f) {
  struct Cmds {
    cmds::VertexAttrib1f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->VertexAttrib1f(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib1fv) {
  GLfloat data[1] = {0};
  struct Cmds {
    cmds::VertexAttrib1fvImmediate cmd;
    GLfloat data[1];
  };

  for (int jj = 0; jj < 1; ++jj) {
    data[jj] = static_cast<GLfloat>(jj);
  }
  Cmds expected;
  expected.cmd.Init(1, &data[0]);
  gl_->VertexAttrib1fv(1, &data[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib2f) {
  struct Cmds {
    cmds::VertexAttrib2f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3);

  gl_->VertexAttrib2f(1, 2, 3);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib2fv) {
  GLfloat data[2] = {0};
  struct Cmds {
    cmds::VertexAttrib2fvImmediate cmd;
    GLfloat data[2];
  };

  for (int jj = 0; jj < 2; ++jj) {
    data[jj] = static_cast<GLfloat>(jj);
  }
  Cmds expected;
  expected.cmd.Init(1, &data[0]);
  gl_->VertexAttrib2fv(1, &data[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib3f) {
  struct Cmds {
    cmds::VertexAttrib3f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4);

  gl_->VertexAttrib3f(1, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib3fv) {
  GLfloat data[3] = {0};
  struct Cmds {
    cmds::VertexAttrib3fvImmediate cmd;
    GLfloat data[3];
  };

  for (int jj = 0; jj < 3; ++jj) {
    data[jj] = static_cast<GLfloat>(jj);
  }
  Cmds expected;
  expected.cmd.Init(1, &data[0]);
  gl_->VertexAttrib3fv(1, &data[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib4f) {
  struct Cmds {
    cmds::VertexAttrib4f cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4, 5);

  gl_->VertexAttrib4f(1, 2, 3, 4, 5);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttrib4fv) {
  GLfloat data[4] = {0};
  struct Cmds {
    cmds::VertexAttrib4fvImmediate cmd;
    GLfloat data[4];
  };

  for (int jj = 0; jj < 4; ++jj) {
    data[jj] = static_cast<GLfloat>(jj);
  }
  Cmds expected;
  expected.cmd.Init(1, &data[0]);
  gl_->VertexAttrib4fv(1, &data[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, Viewport) {
  struct Cmds {
    cmds::Viewport cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4);

  gl_->Viewport(1, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, BlitFramebufferCHROMIUM) {
  struct Cmds {
    cmds::BlitFramebufferCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4, 5, 6, 7, 8, 9, GL_NEAREST);

  gl_->BlitFramebufferCHROMIUM(1, 2, 3, 4, 5, 6, 7, 8, 9, GL_NEAREST);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, RenderbufferStorageMultisampleCHROMIUM) {
  struct Cmds {
    cmds::RenderbufferStorageMultisampleCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_RENDERBUFFER, 2, GL_RGBA4, 4, 5);

  gl_->RenderbufferStorageMultisampleCHROMIUM(
      GL_RENDERBUFFER, 2, GL_RGBA4, 4, 5);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, RenderbufferStorageMultisampleEXT) {
  struct Cmds {
    cmds::RenderbufferStorageMultisampleEXT cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_RENDERBUFFER, 2, GL_RGBA4, 4, 5);

  gl_->RenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, 2, GL_RGBA4, 4, 5);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, FramebufferTexture2DMultisampleEXT) {
  struct Cmds {
    cmds::FramebufferTexture2DMultisampleEXT cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 4, 6);

  gl_->FramebufferTexture2DMultisampleEXT(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 4, 0, 6);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest,
       FramebufferTexture2DMultisampleEXTInvalidConstantArg4) {
  gl_->FramebufferTexture2DMultisampleEXT(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 4, 1, 6);
  EXPECT_TRUE(NoCommandsWritten());
  EXPECT_EQ(GL_INVALID_VALUE, CheckError());
}

TEST_F(GLES2ImplementationTest, TexStorage2DEXT) {
  struct Cmds {
    cmds::TexStorage2DEXT cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, 2, GL_RGB565, 4, 5);

  gl_->TexStorage2DEXT(GL_TEXTURE_2D, 2, GL_RGB565, 4, 5);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, GenQueriesEXT) {
  GLuint ids[2] = {
      0,
  };
  struct Cmds {
    cmds::GenQueriesEXTImmediate gen;
    GLuint data[2];
  };
  Cmds expected;
  expected.gen.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kQueriesStartId;
  expected.data[1] = kQueriesStartId + 1;
  gl_->GenQueriesEXT(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(kQueriesStartId, ids[0]);
  EXPECT_EQ(kQueriesStartId + 1, ids[1]);
}

TEST_F(GLES2ImplementationTest, DeleteQueriesEXT) {
  GLuint ids[2] = {kQueriesStartId, kQueriesStartId + 1};
  struct Cmds {
    cmds::DeleteQueriesEXTImmediate del;
    GLuint data[2];
  };
  Cmds expected;
  expected.del.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kQueriesStartId;
  expected.data[1] = kQueriesStartId + 1;
  gl_->DeleteQueriesEXT(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}
// TODO: Implement unit test for BeginQueryEXT
// TODO: Implement unit test for InsertEventMarkerEXT
// TODO: Implement unit test for PushGroupMarkerEXT

TEST_F(GLES2ImplementationTest, PopGroupMarkerEXT) {
  struct Cmds {
    cmds::PopGroupMarkerEXT cmd;
  };
  Cmds expected;
  expected.cmd.Init();

  gl_->PopGroupMarkerEXT();
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, GenVertexArraysOES) {
  GLuint ids[2] = {
      0,
  };
  struct Cmds {
    cmds::GenVertexArraysOESImmediate gen;
    GLuint data[2];
  };
  Cmds expected;
  expected.gen.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kVertexArraysStartId;
  expected.data[1] = kVertexArraysStartId + 1;
  gl_->GenVertexArraysOES(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_EQ(kVertexArraysStartId, ids[0]);
  EXPECT_EQ(kVertexArraysStartId + 1, ids[1]);
}

TEST_F(GLES2ImplementationTest, DeleteVertexArraysOES) {
  GLuint ids[2] = {kVertexArraysStartId, kVertexArraysStartId + 1};
  struct Cmds {
    cmds::DeleteVertexArraysOESImmediate del;
    GLuint data[2];
  };
  Cmds expected;
  expected.del.Init(arraysize(ids), &ids[0]);
  expected.data[0] = kVertexArraysStartId;
  expected.data[1] = kVertexArraysStartId + 1;
  gl_->DeleteVertexArraysOES(arraysize(ids), &ids[0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, IsVertexArrayOES) {
  struct Cmds {
    cmds::IsVertexArrayOES cmd;
  };

  Cmds expected;
  ExpectedMemoryInfo result1 =
      GetExpectedResultMemory(sizeof(cmds::IsVertexArrayOES::Result));
  expected.cmd.Init(1, result1.id, result1.offset);

  EXPECT_CALL(*command_buffer(), OnFlush())
      .WillOnce(SetMemory(result1.ptr, uint32_t(1)))
      .RetiresOnSaturation();

  GLboolean result = gl_->IsVertexArrayOES(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
  EXPECT_TRUE(result);
}
// TODO: Implement unit test for GenSharedIdsCHROMIUM
// TODO: Implement unit test for DeleteSharedIdsCHROMIUM
// TODO: Implement unit test for RegisterSharedIdsCHROMIUM
// TODO: Implement unit test for EnableFeatureCHROMIUM

TEST_F(GLES2ImplementationTest, ResizeCHROMIUM) {
  struct Cmds {
    cmds::ResizeCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3);

  gl_->ResizeCHROMIUM(1, 2, 3);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}
// TODO: Implement unit test for GetRequestableExtensionsCHROMIUM
// TODO: Implement unit test for CreateStreamTextureCHROMIUM
// TODO: Implement unit test for GetTranslatedShaderSourceANGLE

TEST_F(GLES2ImplementationTest, TexImageIOSurface2DCHROMIUM) {
  struct Cmds {
    cmds::TexImageIOSurface2DCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, 2, 3, 4, 5);

  gl_->TexImageIOSurface2DCHROMIUM(GL_TEXTURE_2D, 2, 3, 4, 5);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, CopyTextureCHROMIUM) {
  struct Cmds {
    cmds::CopyTextureCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2, 3, 4, GL_ALPHA, GL_UNSIGNED_BYTE);

  gl_->CopyTextureCHROMIUM(1, 2, 3, 4, GL_ALPHA, GL_UNSIGNED_BYTE);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DrawArraysInstancedANGLE) {
  struct Cmds {
    cmds::DrawArraysInstancedANGLE cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_POINTS, 2, 3, 4);

  gl_->DrawArraysInstancedANGLE(GL_POINTS, 2, 3, 4);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, VertexAttribDivisorANGLE) {
  struct Cmds {
    cmds::VertexAttribDivisorANGLE cmd;
  };
  Cmds expected;
  expected.cmd.Init(1, 2);

  gl_->VertexAttribDivisorANGLE(1, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}
// TODO: Implement unit test for GenMailboxCHROMIUM
// TODO: Implement unit test for BindUniformLocationCHROMIUM

TEST_F(GLES2ImplementationTest, BindTexImage2DCHROMIUM) {
  struct Cmds {
    cmds::BindTexImage2DCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, 2);

  gl_->BindTexImage2DCHROMIUM(GL_TEXTURE_2D, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, ReleaseTexImage2DCHROMIUM) {
  struct Cmds {
    cmds::ReleaseTexImage2DCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_TEXTURE_2D, 2);

  gl_->ReleaseTexImage2DCHROMIUM(GL_TEXTURE_2D, 2);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DiscardFramebufferEXT) {
  GLenum data[2][1] = {{0}};
  struct Cmds {
    cmds::DiscardFramebufferEXTImmediate cmd;
    GLenum data[2][1];
  };

  Cmds expected;
  for (int ii = 0; ii < 2; ++ii) {
    for (int jj = 0; jj < 1; ++jj) {
      data[ii][jj] = static_cast<GLenum>(ii * 1 + jj);
    }
  }
  expected.cmd.Init(1, 2, &data[0][0]);
  gl_->DiscardFramebufferEXT(1, 2, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, LoseContextCHROMIUM) {
  struct Cmds {
    cmds::LoseContextCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(GL_GUILTY_CONTEXT_RESET_ARB, GL_GUILTY_CONTEXT_RESET_ARB);

  gl_->LoseContextCHROMIUM(GL_GUILTY_CONTEXT_RESET_ARB,
                           GL_GUILTY_CONTEXT_RESET_ARB);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}
// TODO: Implement unit test for InsertSyncPointCHROMIUM

TEST_F(GLES2ImplementationTest, WaitSyncPointCHROMIUM) {
  struct Cmds {
    cmds::WaitSyncPointCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init(1);

  gl_->WaitSyncPointCHROMIUM(1);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DrawBuffersEXT) {
  GLenum data[1][1] = {{0}};
  struct Cmds {
    cmds::DrawBuffersEXTImmediate cmd;
    GLenum data[1][1];
  };

  Cmds expected;
  for (int ii = 0; ii < 1; ++ii) {
    for (int jj = 0; jj < 1; ++jj) {
      data[ii][jj] = static_cast<GLenum>(ii * 1 + jj);
    }
  }
  expected.cmd.Init(1, &data[0][0]);
  gl_->DrawBuffersEXT(1, &data[0][0]);
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}

TEST_F(GLES2ImplementationTest, DiscardBackbufferCHROMIUM) {
  struct Cmds {
    cmds::DiscardBackbufferCHROMIUM cmd;
  };
  Cmds expected;
  expected.cmd.Init();

  gl_->DiscardBackbufferCHROMIUM();
  EXPECT_EQ(0, memcmp(&expected, commands_, sizeof(expected)));
}
#endif  // GPU_COMMAND_BUFFER_CLIENT_GLES2_IMPLEMENTATION_UNITTEST_AUTOGEN_H_
