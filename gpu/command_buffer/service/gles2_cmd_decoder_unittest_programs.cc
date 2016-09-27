// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gles2_cmd_decoder.h"

#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"
#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/common/id_allocator.h"
#include "gpu/command_buffer/service/async_pixel_transfer_delegate_mock.h"
#include "gpu/command_buffer/service/async_pixel_transfer_manager.h"
#include "gpu/command_buffer/service/async_pixel_transfer_manager_mock.h"
#include "gpu/command_buffer/service/cmd_buffer_engine.h"
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/context_state.h"
#include "gpu/command_buffer/service/gl_surface_mock.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder_unittest.h"

#include "gpu/command_buffer/service/gpu_switches.h"
#include "gpu/command_buffer/service/image_manager.h"
#include "gpu/command_buffer/service/mailbox_manager.h"
#include "gpu/command_buffer/service/mocks.h"
#include "gpu/command_buffer/service/program_manager.h"
#include "gpu/command_buffer/service/test_helper.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_mock.h"
#include "ui/gl/gl_surface_stub.h"

#if !defined(GL_DEPTH24_STENCIL8)
#define GL_DEPTH24_STENCIL8 0x88F0
#endif

using ::gfx::MockGLInterface;
using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::MatcherCast;
using ::testing::Mock;
using ::testing::Pointee;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::SetArrayArgument;
using ::testing::SetArgumentPointee;
using ::testing::SetArgPointee;
using ::testing::StrEq;
using ::testing::StrictMock;

namespace gpu {
namespace gles2 {

using namespace cmds;

TEST_P(GLES2DecoderWithShaderTest, GetProgramInfoCHROMIUMValidArgs) {
  const uint32 kBucketId = 123;
  GetProgramInfoCHROMIUM cmd;
  cmd.Init(client_program_id_, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  CommonDecoder::Bucket* bucket = decoder_->GetBucket(kBucketId);
  EXPECT_GT(bucket->size(), 0u);
}

TEST_P(GLES2DecoderWithShaderTest, GetProgramInfoCHROMIUMInvalidArgs) {
  const uint32 kBucketId = 123;
  CommonDecoder::Bucket* bucket = decoder_->GetBucket(kBucketId);
  EXPECT_TRUE(bucket == NULL);
  GetProgramInfoCHROMIUM cmd;
  cmd.Init(kInvalidClientId, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  bucket = decoder_->GetBucket(kBucketId);
  ASSERT_TRUE(bucket != NULL);
  EXPECT_EQ(sizeof(ProgramInfoHeader), bucket->size());
  ProgramInfoHeader* info =
      bucket->GetDataAs<ProgramInfoHeader*>(0, sizeof(ProgramInfoHeader));
  ASSERT_TRUE(info != 0);
  EXPECT_EQ(0u, info->link_status);
  EXPECT_EQ(0u, info->num_attribs);
  EXPECT_EQ(0u, info->num_uniforms);
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformivSucceeds) {
  GetUniformiv::Result* result =
      static_cast<GetUniformiv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformiv cmd;
  cmd.Init(client_program_id_,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformiv(kServiceProgramId, kUniform2RealLocation, _))
      .Times(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GLES2Util::GetGLDataTypeSizeForUniforms(kUniform2Type),
            result->size);
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformivArrayElementSucceeds) {
  GetUniformiv::Result* result =
      static_cast<GetUniformiv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformiv cmd;
  cmd.Init(client_program_id_,
           kUniform2ElementFakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_,
              GetUniformiv(kServiceProgramId, kUniform2ElementRealLocation, _))
      .Times(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GLES2Util::GetGLDataTypeSizeForUniforms(kUniform2Type),
            result->size);
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformivBadProgramFails) {
  GetUniformiv::Result* result =
      static_cast<GetUniformiv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformiv cmd;
  // non-existant program
  cmd.Init(kInvalidClientId,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformiv(_, _, _)).Times(0);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
// Valid id that is not a program. The GL spec requires a different error for
// this case.
#if GLES2_TEST_SHADER_VS_PROGRAM_IDS
  result->size = kInitialResult;
  cmd.Init(client_shader_id_,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
#endif  // GLES2_TEST_SHADER_VS_PROGRAM_IDS
  // Unlinked program
  EXPECT_CALL(*gl_, CreateProgram())
      .Times(1)
      .WillOnce(Return(kNewServiceId))
      .RetiresOnSaturation();
  CreateProgram cmd2;
  cmd2.Init(kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd2));
  result->size = kInitialResult;
  cmd.Init(kNewClientId,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformivBadLocationFails) {
  GetUniformiv::Result* result =
      static_cast<GetUniformiv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformiv cmd;
  // invalid location
  cmd.Init(client_program_id_,
           kInvalidUniformLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformiv(_, _, _)).Times(0);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformivBadSharedMemoryFails) {
  GetUniformiv cmd;
  cmd.Init(client_program_id_,
           kUniform2FakeLocation,
           kInvalidSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformiv(_, _, _)).Times(0);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_program_id_,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
};

TEST_P(GLES2DecoderWithShaderTest, GetUniformfvSucceeds) {
  GetUniformfv::Result* result =
      static_cast<GetUniformfv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformfv cmd;
  cmd.Init(client_program_id_,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformfv(kServiceProgramId, kUniform2RealLocation, _))
      .Times(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GLES2Util::GetGLDataTypeSizeForUniforms(kUniform2Type),
            result->size);
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformfvArrayElementSucceeds) {
  GetUniformfv::Result* result =
      static_cast<GetUniformfv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformfv cmd;
  cmd.Init(client_program_id_,
           kUniform2ElementFakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_,
              GetUniformfv(kServiceProgramId, kUniform2ElementRealLocation, _))
      .Times(1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GLES2Util::GetGLDataTypeSizeForUniforms(kUniform2Type),
            result->size);
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformfvBadProgramFails) {
  GetUniformfv::Result* result =
      static_cast<GetUniformfv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformfv cmd;
  // non-existant program
  cmd.Init(kInvalidClientId,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformfv(_, _, _)).Times(0);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
// Valid id that is not a program. The GL spec requires a different error for
// this case.
#if GLES2_TEST_SHADER_VS_PROGRAM_IDS
  result->size = kInitialResult;
  cmd.Init(client_shader_id_,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
#endif  // GLES2_TEST_SHADER_VS_PROGRAM_IDS
  // Unlinked program
  EXPECT_CALL(*gl_, CreateProgram())
      .Times(1)
      .WillOnce(Return(kNewServiceId))
      .RetiresOnSaturation();
  CreateProgram cmd2;
  cmd2.Init(kNewClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd2));
  result->size = kInitialResult;
  cmd.Init(kNewClientId,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformfvBadLocationFails) {
  GetUniformfv::Result* result =
      static_cast<GetUniformfv::Result*>(shared_memory_address_);
  result->size = 0;
  GetUniformfv cmd;
  // invalid location
  cmd.Init(client_program_id_,
           kInvalidUniformLocation,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformfv(_, _, _)).Times(0);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformfvBadSharedMemoryFails) {
  GetUniformfv cmd;
  cmd.Init(client_program_id_,
           kUniform2FakeLocation,
           kInvalidSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_CALL(*gl_, GetUniformfv(_, _, _)).Times(0);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_program_id_,
           kUniform2FakeLocation,
           kSharedMemoryId,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
};

TEST_P(GLES2DecoderWithShaderTest, GetAttachedShadersSucceeds) {
  GetAttachedShaders cmd;
  typedef GetAttachedShaders::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->size = 0;
  EXPECT_CALL(*gl_, GetAttachedShaders(kServiceProgramId, 1, _, _)).WillOnce(
      DoAll(SetArgumentPointee<2>(1), SetArgumentPointee<3>(kServiceShaderId)));
  cmd.Init(client_program_id_,
           shared_memory_id_,
           shared_memory_offset_,
           Result::ComputeSize(1));
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(1, result->GetNumResults());
  EXPECT_EQ(client_shader_id_, result->GetData()[0]);
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetAttachedShadersResultNotInitFail) {
  GetAttachedShaders cmd;
  typedef GetAttachedShaders::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->size = 1;
  EXPECT_CALL(*gl_, GetAttachedShaders(_, _, _, _)).Times(0);
  cmd.Init(client_program_id_,
           shared_memory_id_,
           shared_memory_offset_,
           Result::ComputeSize(1));
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetAttachedShadersBadProgramFails) {
  GetAttachedShaders cmd;
  typedef GetAttachedShaders::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->size = 0;
  EXPECT_CALL(*gl_, GetAttachedShaders(_, _, _, _)).Times(0);
  cmd.Init(kInvalidClientId,
           shared_memory_id_,
           shared_memory_offset_,
           Result::ComputeSize(1));
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0U, result->size);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetAttachedShadersBadSharedMemoryFails) {
  GetAttachedShaders cmd;
  typedef GetAttachedShaders::Result Result;
  cmd.Init(client_program_id_,
           kInvalidSharedMemoryId,
           shared_memory_offset_,
           Result::ComputeSize(1));
  EXPECT_CALL(*gl_, GetAttachedShaders(_, _, _, _)).Times(0);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_program_id_,
           shared_memory_id_,
           kInvalidSharedMemoryOffset,
           Result::ComputeSize(1));
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetShaderPrecisionFormatSucceeds) {
  ScopedGLImplementationSetter gl_impl(::gfx::kGLImplementationEGLGLES2);
  GetShaderPrecisionFormat cmd;
  typedef GetShaderPrecisionFormat::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  const GLint range[2] = {62, 62};
  const GLint precision = 16;
  EXPECT_CALL(*gl_, GetShaderPrecisionFormat(_, _, _, _))
      .WillOnce(DoAll(SetArrayArgument<2>(range, range + 2),
                      SetArgumentPointee<3>(precision)))
      .RetiresOnSaturation();
  cmd.Init(GL_VERTEX_SHADER,
           GL_HIGH_FLOAT,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_NE(0, result->success);
  EXPECT_EQ(range[0], result->min_range);
  EXPECT_EQ(range[1], result->max_range);
  EXPECT_EQ(precision, result->precision);
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetShaderPrecisionFormatResultNotInitFails) {
  GetShaderPrecisionFormat cmd;
  typedef GetShaderPrecisionFormat::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 1;
  // NOTE: GL might not be called. There is no Desktop OpenGL equivalent
  cmd.Init(GL_VERTEX_SHADER,
           GL_HIGH_FLOAT,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetShaderPrecisionFormatBadArgsFails) {
  typedef GetShaderPrecisionFormat::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  GetShaderPrecisionFormat cmd;
  cmd.Init(
      GL_TEXTURE_2D, GL_HIGH_FLOAT, shared_memory_id_, shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
  result->success = 0;
  cmd.Init(GL_VERTEX_SHADER,
           GL_TEXTURE_2D,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest,
       GetShaderPrecisionFormatBadSharedMemoryFails) {
  GetShaderPrecisionFormat cmd;
  cmd.Init(GL_VERTEX_SHADER,
           GL_HIGH_FLOAT,
           kInvalidSharedMemoryId,
           shared_memory_offset_);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(GL_VERTEX_SHADER,
           GL_TEXTURE_2D,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveUniformSucceeds) {
  const GLuint kUniformIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveUniform cmd;
  typedef GetActiveUniform::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  cmd.Init(client_program_id_,
           kUniformIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_NE(0, result->success);
  EXPECT_EQ(kUniform2Size, result->size);
  EXPECT_EQ(kUniform2Type, result->type);
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  CommonDecoder::Bucket* bucket = decoder_->GetBucket(kBucketId);
  ASSERT_TRUE(bucket != NULL);
  EXPECT_EQ(
      0,
      memcmp(
          bucket->GetData(0, bucket->size()), kUniform2Name, bucket->size()));
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveUniformResultNotInitFails) {
  const GLuint kUniformIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveUniform cmd;
  typedef GetActiveUniform::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 1;
  cmd.Init(client_program_id_,
           kUniformIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveUniformBadProgramFails) {
  const GLuint kUniformIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveUniform cmd;
  typedef GetActiveUniform::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  cmd.Init(kInvalidClientId,
           kUniformIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0, result->success);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
#if GLES2_TEST_SHADER_VS_PROGRAM_IDS
  result->success = 0;
  cmd.Init(client_shader_id_,
           kUniformIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0, result->success);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
#endif  // GLES2_TEST_SHADER_VS_PROGRAM_IDS
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveUniformBadIndexFails) {
  const uint32 kBucketId = 123;
  GetActiveUniform cmd;
  typedef GetActiveUniform::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  cmd.Init(client_program_id_,
           kBadUniformIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0, result->success);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveUniformBadSharedMemoryFails) {
  const GLuint kUniformIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveUniform cmd;
  cmd.Init(client_program_id_,
           kUniformIndex,
           kBucketId,
           kInvalidSharedMemoryId,
           shared_memory_offset_);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_program_id_,
           kUniformIndex,
           kBucketId,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveAttribSucceeds) {
  const GLuint kAttribIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveAttrib cmd;
  typedef GetActiveAttrib::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  cmd.Init(client_program_id_,
           kAttribIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_NE(0, result->success);
  EXPECT_EQ(kAttrib2Size, result->size);
  EXPECT_EQ(kAttrib2Type, result->type);
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  CommonDecoder::Bucket* bucket = decoder_->GetBucket(kBucketId);
  ASSERT_TRUE(bucket != NULL);
  EXPECT_EQ(
      0,
      memcmp(bucket->GetData(0, bucket->size()), kAttrib2Name, bucket->size()));
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveAttribResultNotInitFails) {
  const GLuint kAttribIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveAttrib cmd;
  typedef GetActiveAttrib::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 1;
  cmd.Init(client_program_id_,
           kAttribIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveAttribBadProgramFails) {
  const GLuint kAttribIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveAttrib cmd;
  typedef GetActiveAttrib::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  cmd.Init(kInvalidClientId,
           kAttribIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0, result->success);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
#if GLES2_TEST_SHADER_VS_PROGRAM_IDS
  result->success = 0;
  cmd.Init(client_shader_id_,
           kAttribIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0, result->success);
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
#endif  // GLES2_TEST_SHADER_VS_PROGRAM_IDS
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveAttribBadIndexFails) {
  const uint32 kBucketId = 123;
  GetActiveAttrib cmd;
  typedef GetActiveAttrib::Result Result;
  Result* result = static_cast<Result*>(shared_memory_address_);
  result->success = 0;
  cmd.Init(client_program_id_,
           kBadAttribIndex,
           kBucketId,
           shared_memory_id_,
           shared_memory_offset_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0, result->success);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetActiveAttribBadSharedMemoryFails) {
  const GLuint kAttribIndex = 1;
  const uint32 kBucketId = 123;
  GetActiveAttrib cmd;
  cmd.Init(client_program_id_,
           kAttribIndex,
           kBucketId,
           kInvalidSharedMemoryId,
           shared_memory_offset_);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_program_id_,
           kAttribIndex,
           kBucketId,
           shared_memory_id_,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetShaderInfoLogValidArgs) {
  const char* kInfo = "hello";
  const uint32 kBucketId = 123;
  CompileShader compile_cmd;
  GetShaderInfoLog cmd;
  EXPECT_CALL(*gl_, ShaderSource(kServiceShaderId, 1, _, _));
  EXPECT_CALL(*gl_, CompileShader(kServiceShaderId));
  EXPECT_CALL(*gl_, GetShaderiv(kServiceShaderId, GL_COMPILE_STATUS, _))
      .WillOnce(SetArgumentPointee<2>(GL_FALSE))
      .RetiresOnSaturation();
  EXPECT_CALL(*gl_, GetShaderiv(kServiceShaderId, GL_INFO_LOG_LENGTH, _))
      .WillOnce(SetArgumentPointee<2>(strlen(kInfo) + 1))
      .RetiresOnSaturation();
  EXPECT_CALL(*gl_, GetShaderInfoLog(kServiceShaderId, strlen(kInfo) + 1, _, _))
      .WillOnce(DoAll(SetArgumentPointee<2>(strlen(kInfo)),
                      SetArrayArgument<3>(kInfo, kInfo + strlen(kInfo) + 1)));
  compile_cmd.Init(client_shader_id_);
  cmd.Init(client_shader_id_, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(compile_cmd));
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  CommonDecoder::Bucket* bucket = decoder_->GetBucket(kBucketId);
  ASSERT_TRUE(bucket != NULL);
  EXPECT_EQ(strlen(kInfo) + 1, bucket->size());
  EXPECT_EQ(0,
            memcmp(bucket->GetData(0, bucket->size()), kInfo, bucket->size()));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetShaderInfoLogInvalidArgs) {
  const uint32 kBucketId = 123;
  GetShaderInfoLog cmd;
  cmd.Init(kInvalidClientId, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest, CompileShaderValidArgs) {
  EXPECT_CALL(*gl_, ShaderSource(kServiceShaderId, 1, _, _));
  EXPECT_CALL(*gl_, CompileShader(kServiceShaderId));
  EXPECT_CALL(*gl_, GetShaderiv(kServiceShaderId, GL_COMPILE_STATUS, _))
      .WillOnce(SetArgumentPointee<2>(GL_TRUE))
      .RetiresOnSaturation();
  CompileShader cmd;
  cmd.Init(client_shader_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, CompileShaderInvalidArgs) {
  CompileShader cmd;
  cmd.Init(kInvalidClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
#if GLES2_TEST_SHADER_VS_PROGRAM_IDS
  cmd.Init(client_program_id_);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
#endif  // GLES2_TEST_SHADER_VS_PROGRAM_IDS
}

TEST_P(GLES2DecoderTest, ShaderSourceBucketAndGetShaderSourceValidArgs) {
  const uint32 kInBucketId = 123;
  const uint32 kOutBucketId = 125;
  const char kSource[] = "hello";
  const uint32 kSourceSize = sizeof(kSource) - 1;
  SetBucketAsCString(kInBucketId, kSource);
  ShaderSourceBucket cmd;
  cmd.Init(client_shader_id_, kInBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  ClearSharedMemory();
  GetShaderSource get_cmd;
  get_cmd.Init(client_shader_id_, kOutBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(get_cmd));
  CommonDecoder::Bucket* bucket = decoder_->GetBucket(kOutBucketId);
  ASSERT_TRUE(bucket != NULL);
  EXPECT_EQ(kSourceSize + 1, bucket->size());
  EXPECT_EQ(
      0, memcmp(bucket->GetData(0, bucket->size()), kSource, bucket->size()));
}

TEST_P(GLES2DecoderTest, ShaderSourceBucketInvalidArgs) {
  const uint32 kBucketId = 123;
  const char kSource[] = "hello";
  const uint32 kSourceSize = sizeof(kSource) - 1;
  memcpy(shared_memory_address_, kSource, kSourceSize);
  ShaderSourceBucket cmd;
  // Test no bucket.
  cmd.Init(client_texture_id_, kBucketId);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  // Test invalid client.
  SetBucketAsCString(kBucketId, kSource);
  cmd.Init(kInvalidClientId, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
#if GLES2_TEST_SHADER_VS_PROGRAM_IDS
  SetBucketAsCString(kBucketId, kSource);
  cmd.Init(
      client_program_id_, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
#endif  // GLES2_TEST_SHADER_VS_PROGRAM_IDS
}

TEST_P(GLES2DecoderTest, ShaderSourceStripComments) {
  const uint32 kInBucketId = 123;
  const char kSource[] = "hello/*te\ast*/world//a\ab";
  SetBucketAsCString(kInBucketId, kSource);
  ShaderSourceBucket cmd;
  cmd.Init(client_shader_id_, kInBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, Uniform1iValidArgs) {
  EXPECT_CALL(*gl_, Uniform1i(kUniform1RealLocation, 2));
  Uniform1i cmd;
  cmd.Init(kUniform1FakeLocation, 2);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, Uniform1ivImmediateValidArgs) {
  Uniform1ivImmediate& cmd = *GetImmediateAs<Uniform1ivImmediate>();
  EXPECT_CALL(*gl_,
              Uniform1iv(kUniform1RealLocation,
                         1,
                         reinterpret_cast<GLint*>(ImmediateDataAddress(&cmd))));
  GLint temp[1 * 2] = {
      0,
  };
  cmd.Init(kUniform1FakeLocation, 1, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
}

TEST_P(GLES2DecoderWithShaderTest, Uniform1ivImmediateInvalidValidArgs) {
  EXPECT_CALL(*gl_, Uniform1iv(_, _, _)).Times(0);
  Uniform1ivImmediate& cmd = *GetImmediateAs<Uniform1ivImmediate>();
  GLint temp[1 * 2] = {
      0,
  };
  cmd.Init(kUniform1FakeLocation, 2, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, Uniform1ivZeroCount) {
  EXPECT_CALL(*gl_, Uniform1iv(_, _, _)).Times(0);
  Uniform1ivImmediate& cmd = *GetImmediateAs<Uniform1ivImmediate>();
  GLint temp = 0;
  cmd.Init(kUniform1FakeLocation, 0, &temp);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, Uniform1iSamplerIsLmited) {
  EXPECT_CALL(*gl_, Uniform1i(_, _)).Times(0);
  Uniform1i cmd;
  cmd.Init(kUniform1FakeLocation, kNumTextureUnits);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, Uniform1ivSamplerIsLimited) {
  EXPECT_CALL(*gl_, Uniform1iv(_, _, _)).Times(0);
  Uniform1ivImmediate& cmd = *GetImmediateAs<Uniform1ivImmediate>();
  GLint temp[] = {kNumTextureUnits};
  cmd.Init(kUniform1FakeLocation, 1, &temp[0]);
  EXPECT_EQ(error::kNoError, ExecuteImmediateCmd(cmd, sizeof(temp)));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest, BindAttribLocationBucket) {
  const uint32 kBucketId = 123;
  const GLint kLocation = 2;
  const char* kName = "testing";
  EXPECT_CALL(*gl_,
              BindAttribLocation(kServiceProgramId, kLocation, StrEq(kName)))
      .Times(1);
  SetBucketAsCString(kBucketId, kName);
  BindAttribLocationBucket cmd;
  cmd.Init(client_program_id_, kLocation, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, BindAttribLocationBucketInvalidArgs) {
  const uint32 kBucketId = 123;
  const GLint kLocation = 2;
  const char* kName = "testing";
  EXPECT_CALL(*gl_, BindAttribLocation(_, _, _)).Times(0);
  BindAttribLocationBucket cmd;
  // check bucket does not exist.
  cmd.Init(client_program_id_, kLocation, kBucketId);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  // check bucket is empty.
  SetBucketAsCString(kBucketId, NULL);
  cmd.Init(client_program_id_, kLocation, kBucketId);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  // Check bad program id
  SetBucketAsCString(kBucketId, kName);
  cmd.Init(kInvalidClientId, kLocation, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderWithShaderTest, GetAttribLocation) {
  const uint32 kBucketId = 123;
  const char* kNonExistentName = "foobar";
  typedef GetAttribLocation::Result Result;
  Result* result = GetSharedMemoryAs<Result*>();
  SetBucketAsCString(kBucketId, kAttrib2Name);
  *result = -1;
  GetAttribLocation cmd;
  cmd.Init(client_program_id_, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(kAttrib2Location, *result);
  SetBucketAsCString(kBucketId, kNonExistentName);
  *result = -1;
  cmd.Init(client_program_id_, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(-1, *result);
}

TEST_P(GLES2DecoderWithShaderTest, GetAttribLocationInvalidArgs) {
  const uint32 kBucketId = 123;
  typedef GetAttribLocation::Result Result;
  Result* result = GetSharedMemoryAs<Result*>();
  *result = -1;
  GetAttribLocation cmd;
  // Check no bucket
  cmd.Init(client_program_id_, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(-1, *result);
  // Check bad program id.
  SetBucketAsCString(kBucketId, kAttrib2Name);
  cmd.Init(kInvalidClientId, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  *result = -1;
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(-1, *result);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
  // Check bad memory
  cmd.Init(client_program_id_,
           kBucketId,
           kInvalidSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_program_id_,
           kBucketId,
           kSharedMemoryId,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformLocation) {
  const uint32 kBucketId = 123;
  const char* kNonExistentName = "foobar";
  typedef GetUniformLocation::Result Result;
  Result* result = GetSharedMemoryAs<Result*>();
  SetBucketAsCString(kBucketId, kUniform2Name);
  *result = -1;
  GetUniformLocation cmd;
  cmd.Init(client_program_id_, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(kUniform2FakeLocation, *result);
  SetBucketAsCString(kBucketId, kNonExistentName);
  *result = -1;
  cmd.Init(client_program_id_, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(-1, *result);
}

TEST_P(GLES2DecoderWithShaderTest, GetUniformLocationInvalidArgs) {
  const uint32 kBucketId = 123;
  typedef GetUniformLocation::Result Result;
  Result* result = GetSharedMemoryAs<Result*>();
  *result = -1;
  GetUniformLocation cmd;
  // Check no bucket
  cmd.Init(client_program_id_, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(-1, *result);
  // Check bad program id.
  SetBucketAsCString(kBucketId, kUniform2Name);
  cmd.Init(kInvalidClientId, kBucketId, kSharedMemoryId, kSharedMemoryOffset);
  *result = -1;
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(-1, *result);
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
  // Check bad memory
  cmd.Init(client_program_id_,
           kBucketId,
           kInvalidSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_program_id_,
           kBucketId,
           kSharedMemoryId,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderWithShaderTest, BindUniformLocationCHROMIUMBucket) {
  const uint32 kBucketId = 123;
  const GLint kLocation = 2;
  const char* kName = "testing";
  const char* kBadName1 = "gl_testing";
  const char* kBadName2 = "testing[1]";

  SetBucketAsCString(kBucketId, kName);
  BindUniformLocationCHROMIUMBucket cmd;
  cmd.Init(client_program_id_,
           kLocation,
           kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // check negative location
  SetBucketAsCString(kBucketId, kName);
  cmd.Init(client_program_id_, -1, kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
  // check highest location
  SetBucketAsCString(kBucketId, kName);
  GLint kMaxLocation =
      (kMaxFragmentUniformVectors + kMaxVertexUniformVectors) * 4 - 1;
  cmd.Init(client_program_id_,
           kMaxLocation,
           kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // check too high location
  SetBucketAsCString(kBucketId, kName);
  cmd.Init(client_program_id_,
           kMaxLocation + 1,
           kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
  // check bad name "gl_..."
  SetBucketAsCString(kBucketId, kBadName1);
  cmd.Init(client_program_id_,
           kLocation,
           kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
  // check bad name "name[1]" non zero
  SetBucketAsCString(kBucketId, kBadName2);
  cmd.Init(client_program_id_,
           kLocation,
           kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderManualInitTest, ClearUniformsBeforeFirstProgramUse) {
  CommandLine command_line(0, NULL);
  command_line.AppendSwitchASCII(
      switches::kGpuDriverBugWorkarounds,
      base::IntToString(gpu::CLEAR_UNIFORMS_BEFORE_FIRST_PROGRAM_USE));
  InitState init;
  init.gl_version = "3.0";
  init.has_alpha = true;
  init.request_alpha = true;
  init.bind_generates_resource = true;
  InitDecoderWithCommandLine(init, &command_line);
  {
    static AttribInfo attribs[] = {
        {
         kAttrib1Name, kAttrib1Size, kAttrib1Type, kAttrib1Location,
        },
        {
         kAttrib2Name, kAttrib2Size, kAttrib2Type, kAttrib2Location,
        },
        {
         kAttrib3Name, kAttrib3Size, kAttrib3Type, kAttrib3Location,
        },
    };
    static UniformInfo uniforms[] = {
        {kUniform1Name, kUniform1Size, kUniform1Type, kUniform1FakeLocation,
         kUniform1RealLocation, kUniform1DesiredLocation},
        {kUniform2Name, kUniform2Size, kUniform2Type, kUniform2FakeLocation,
         kUniform2RealLocation, kUniform2DesiredLocation},
        {kUniform3Name, kUniform3Size, kUniform3Type, kUniform3FakeLocation,
         kUniform3RealLocation, kUniform3DesiredLocation},
    };
    SetupShader(attribs,
                arraysize(attribs),
                uniforms,
                arraysize(uniforms),
                client_program_id_,
                kServiceProgramId,
                client_vertex_shader_id_,
                kServiceVertexShaderId,
                client_fragment_shader_id_,
                kServiceFragmentShaderId);
    TestHelper::SetupExpectationsForClearingUniforms(
        gl_.get(), uniforms, arraysize(uniforms));
  }

  {
    EXPECT_CALL(*gl_, UseProgram(kServiceProgramId))
        .Times(1)
        .RetiresOnSaturation();
    cmds::UseProgram cmd;
    cmd.Init(client_program_id_);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  }
}

// TODO(gman): DeleteProgram

// TODO(gman): UseProgram

// TODO(gman): DeleteShader

}  // namespace gles2
}  // namespace gpu
