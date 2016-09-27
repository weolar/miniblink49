// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gles2_cmd_decoder_unittest.h"

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
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
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
using ::testing::AtLeast;
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

void GLES2DecoderRGBBackbufferTest::SetUp() {
  // Test codepath with workaround clear_alpha_in_readpixels because
  // ReadPixelsEmulator emulates the incorrect driver behavior.
  CommandLine command_line(0, NULL);
  command_line.AppendSwitchASCII(
      switches::kGpuDriverBugWorkarounds,
      base::IntToString(gpu::CLEAR_ALPHA_IN_READPIXELS));
  InitState init;
  init.gl_version = "3.0";
  init.bind_generates_resource = true;
  InitDecoderWithCommandLine(init, &command_line);
  SetupDefaultProgram();
}

// Override default setup so nothing gets setup.
void GLES2DecoderManualInitTest::SetUp() {
}

void GLES2DecoderManualInitTest::EnableDisableTest(GLenum cap,
                                                   bool enable,
                                                   bool expect_set) {
  if (expect_set) {
    SetupExpectationsForEnableDisable(cap, enable);
  }
  if (enable) {
    Enable cmd;
    cmd.Init(cap);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
  } else {
    Disable cmd;
    cmd.Init(cap);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
  }
}

TEST_P(GLES2DecoderTest, GetIntegervCached) {
  struct TestInfo {
    GLenum pname;
    GLint expected;
  };
  TestInfo tests[] = {
      {
       GL_MAX_TEXTURE_SIZE, TestHelper::kMaxTextureSize,
      },
      {
       GL_MAX_CUBE_MAP_TEXTURE_SIZE, TestHelper::kMaxCubeMapTextureSize,
      },
      {
       GL_MAX_RENDERBUFFER_SIZE, TestHelper::kMaxRenderbufferSize,
      },
  };
  typedef GetIntegerv::Result Result;
  for (size_t ii = 0; ii < sizeof(tests) / sizeof(tests[0]); ++ii) {
    const TestInfo& test = tests[ii];
    Result* result = static_cast<Result*>(shared_memory_address_);
    EXPECT_CALL(*gl_, GetError())
        .WillOnce(Return(GL_NO_ERROR))
        .WillOnce(Return(GL_NO_ERROR))
        .RetiresOnSaturation();
    EXPECT_CALL(*gl_, GetIntegerv(test.pname, _)).Times(0);
    result->size = 0;
    GetIntegerv cmd2;
    cmd2.Init(test.pname, shared_memory_id_, shared_memory_offset_);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd2));
    EXPECT_EQ(decoder_->GetGLES2Util()->GLGetNumValuesReturned(test.pname),
              result->GetNumResults());
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
    EXPECT_EQ(test.expected, result->GetData()[0]);
  }
}

TEST_P(GLES2DecoderWithShaderTest, GetMaxValueInBufferCHROMIUM) {
  SetupIndexBuffer();
  GetMaxValueInBufferCHROMIUM::Result* result =
      static_cast<GetMaxValueInBufferCHROMIUM::Result*>(shared_memory_address_);
  *result = 0;

  GetMaxValueInBufferCHROMIUM cmd;
  cmd.Init(client_element_buffer_id_,
           kValidIndexRangeCount,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(7u, *result);
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  cmd.Init(client_element_buffer_id_,
           kValidIndexRangeCount + 1,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(100u, *result);
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  cmd.Init(kInvalidClientId,
           kValidIndexRangeCount,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
  cmd.Init(client_element_buffer_id_,
           kOutOfRangeIndexRangeEnd,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
  cmd.Init(client_element_buffer_id_,
           kValidIndexRangeCount + 1,
           GL_UNSIGNED_SHORT,
           kOutOfRangeIndexRangeEnd * 2,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
  cmd.Init(client_element_buffer_id_,
           kValidIndexRangeCount + 1,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_buffer_id_,
           kValidIndexRangeCount + 1,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
  cmd.Init(client_element_buffer_id_,
           kValidIndexRangeCount + 1,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kInvalidSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(client_element_buffer_id_,
           kValidIndexRangeCount + 1,
           GL_UNSIGNED_SHORT,
           kValidIndexRangeStart * 2,
           kSharedMemoryId,
           kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, SharedIds) {
  GenSharedIdsCHROMIUM gen_cmd;
  RegisterSharedIdsCHROMIUM reg_cmd;
  DeleteSharedIdsCHROMIUM del_cmd;

  const GLuint kNamespaceId = id_namespaces::kTextures;
  const GLuint kExpectedId1 = 1;
  const GLuint kExpectedId2 = 2;
  const GLuint kExpectedId3 = 4;
  const GLuint kRegisterId = 3;
  GLuint* ids = GetSharedMemoryAs<GLuint*>();
  gen_cmd.Init(kNamespaceId, 0, 2, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
  IdAllocatorInterface* id_allocator = GetIdAllocator(kNamespaceId);
  ASSERT_TRUE(id_allocator != NULL);
  // This check is implementation dependant but it's kind of hard to check
  // otherwise.
  EXPECT_EQ(kExpectedId1, ids[0]);
  EXPECT_EQ(kExpectedId2, ids[1]);
  EXPECT_TRUE(id_allocator->InUse(kExpectedId1));
  EXPECT_TRUE(id_allocator->InUse(kExpectedId2));
  EXPECT_FALSE(id_allocator->InUse(kRegisterId));
  EXPECT_FALSE(id_allocator->InUse(kExpectedId3));

  ClearSharedMemory();
  ids[0] = kRegisterId;
  reg_cmd.Init(kNamespaceId, 1, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(reg_cmd));
  EXPECT_TRUE(id_allocator->InUse(kExpectedId1));
  EXPECT_TRUE(id_allocator->InUse(kExpectedId2));
  EXPECT_TRUE(id_allocator->InUse(kRegisterId));
  EXPECT_FALSE(id_allocator->InUse(kExpectedId3));

  ClearSharedMemory();
  gen_cmd.Init(kNamespaceId, 0, 1, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
  EXPECT_EQ(kExpectedId3, ids[0]);
  EXPECT_TRUE(id_allocator->InUse(kExpectedId1));
  EXPECT_TRUE(id_allocator->InUse(kExpectedId2));
  EXPECT_TRUE(id_allocator->InUse(kRegisterId));
  EXPECT_TRUE(id_allocator->InUse(kExpectedId3));

  ClearSharedMemory();
  ids[0] = kExpectedId1;
  ids[1] = kRegisterId;
  del_cmd.Init(kNamespaceId, 2, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(del_cmd));
  EXPECT_FALSE(id_allocator->InUse(kExpectedId1));
  EXPECT_TRUE(id_allocator->InUse(kExpectedId2));
  EXPECT_FALSE(id_allocator->InUse(kRegisterId));
  EXPECT_TRUE(id_allocator->InUse(kExpectedId3));

  ClearSharedMemory();
  ids[0] = kExpectedId3;
  ids[1] = kExpectedId2;
  del_cmd.Init(kNamespaceId, 2, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(del_cmd));
  EXPECT_FALSE(id_allocator->InUse(kExpectedId1));
  EXPECT_FALSE(id_allocator->InUse(kExpectedId2));
  EXPECT_FALSE(id_allocator->InUse(kRegisterId));
  EXPECT_FALSE(id_allocator->InUse(kExpectedId3));

  // Check passing in an id_offset.
  ClearSharedMemory();
  const GLuint kOffset = 0xABCDEF;
  gen_cmd.Init(kNamespaceId, kOffset, 2, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(gen_cmd));
  EXPECT_EQ(kOffset, ids[0]);
  EXPECT_EQ(kOffset + 1, ids[1]);
}

TEST_P(GLES2DecoderTest, GenSharedIdsCHROMIUMBadArgs) {
  const GLuint kNamespaceId = id_namespaces::kTextures;
  GenSharedIdsCHROMIUM cmd;
  cmd.Init(kNamespaceId, 0, -1, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(kNamespaceId, 0, 1, kInvalidSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(kNamespaceId, 0, 1, kSharedMemoryId, kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, RegisterSharedIdsCHROMIUMBadArgs) {
  const GLuint kNamespaceId = id_namespaces::kTextures;
  RegisterSharedIdsCHROMIUM cmd;
  cmd.Init(kNamespaceId, -1, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(kNamespaceId, 1, kInvalidSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(kNamespaceId, 1, kSharedMemoryId, kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, RegisterSharedIdsCHROMIUMDuplicateIds) {
  const GLuint kNamespaceId = id_namespaces::kTextures;
  const GLuint kRegisterId = 3;
  RegisterSharedIdsCHROMIUM cmd;
  GLuint* ids = GetSharedMemoryAs<GLuint*>();
  ids[0] = kRegisterId;
  cmd.Init(kNamespaceId, 1, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(kNamespaceId, 1, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
}

TEST_P(GLES2DecoderTest, DeleteSharedIdsCHROMIUMBadArgs) {
  const GLuint kNamespaceId = id_namespaces::kTextures;
  DeleteSharedIdsCHROMIUM cmd;
  cmd.Init(kNamespaceId, -1, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(kNamespaceId, 1, kInvalidSharedMemoryId, kSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
  cmd.Init(kNamespaceId, 1, kSharedMemoryId, kInvalidSharedMemoryOffset);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, IsBuffer) {
  EXPECT_FALSE(DoIsBuffer(client_buffer_id_));
  DoBindBuffer(GL_ARRAY_BUFFER, client_buffer_id_, kServiceBufferId);
  EXPECT_TRUE(DoIsBuffer(client_buffer_id_));
  DoDeleteBuffer(client_buffer_id_, kServiceBufferId);
  EXPECT_FALSE(DoIsBuffer(client_buffer_id_));
}

TEST_P(GLES2DecoderTest, IsFramebuffer) {
  EXPECT_FALSE(DoIsFramebuffer(client_framebuffer_id_));
  DoBindFramebuffer(
      GL_FRAMEBUFFER, client_framebuffer_id_, kServiceFramebufferId);
  EXPECT_TRUE(DoIsFramebuffer(client_framebuffer_id_));
  DoDeleteFramebuffer(client_framebuffer_id_,
                      kServiceFramebufferId,
                      true,
                      GL_FRAMEBUFFER,
                      0,
                      true,
                      GL_FRAMEBUFFER,
                      0);
  EXPECT_FALSE(DoIsFramebuffer(client_framebuffer_id_));
}

TEST_P(GLES2DecoderTest, IsProgram) {
  // IsProgram is true as soon as the program is created.
  EXPECT_TRUE(DoIsProgram(client_program_id_));
  EXPECT_CALL(*gl_, DeleteProgram(kServiceProgramId))
      .Times(1)
      .RetiresOnSaturation();
  DoDeleteProgram(client_program_id_, kServiceProgramId);
  EXPECT_FALSE(DoIsProgram(client_program_id_));
}

TEST_P(GLES2DecoderTest, IsRenderbuffer) {
  EXPECT_FALSE(DoIsRenderbuffer(client_renderbuffer_id_));
  DoBindRenderbuffer(
      GL_RENDERBUFFER, client_renderbuffer_id_, kServiceRenderbufferId);
  EXPECT_TRUE(DoIsRenderbuffer(client_renderbuffer_id_));
  DoDeleteRenderbuffer(client_renderbuffer_id_, kServiceRenderbufferId);
  EXPECT_FALSE(DoIsRenderbuffer(client_renderbuffer_id_));
}

TEST_P(GLES2DecoderTest, IsShader) {
  // IsShader is true as soon as the program is created.
  EXPECT_TRUE(DoIsShader(client_shader_id_));
  DoDeleteShader(client_shader_id_, kServiceShaderId);
  EXPECT_FALSE(DoIsShader(client_shader_id_));
}

TEST_P(GLES2DecoderTest, IsTexture) {
  EXPECT_FALSE(DoIsTexture(client_texture_id_));
  DoBindTexture(GL_TEXTURE_2D, client_texture_id_, kServiceTextureId);
  EXPECT_TRUE(DoIsTexture(client_texture_id_));
  DoDeleteTexture(client_texture_id_, kServiceTextureId);
  EXPECT_FALSE(DoIsTexture(client_texture_id_));
}

TEST_P(GLES2DecoderTest, GetMultipleIntegervCHROMIUMValidArgs) {
  const GLsizei kCount = 3;
  GLenum* pnames = GetSharedMemoryAs<GLenum*>();
  pnames[0] = GL_DEPTH_WRITEMASK;
  pnames[1] = GL_COLOR_WRITEMASK;
  pnames[2] = GL_STENCIL_WRITEMASK;
  GLint* results =
      GetSharedMemoryAsWithOffset<GLint*>(sizeof(*pnames) * kCount);

  GLsizei num_results = 0;
  for (GLsizei ii = 0; ii < kCount; ++ii) {
    num_results += decoder_->GetGLES2Util()->GLGetNumValuesReturned(pnames[ii]);
  }
  const GLsizei result_size = num_results * sizeof(*results);
  memset(results, 0, result_size);

  const GLint kSentinel = 0x12345678;
  results[num_results] = kSentinel;

  GetMultipleIntegervCHROMIUM cmd;
  cmd.Init(kSharedMemoryId,
           kSharedMemoryOffset,
           kCount,
           kSharedMemoryId,
           kSharedMemoryOffset + sizeof(*pnames) * kCount,
           result_size);

  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_EQ(1, results[0]);                    // Depth writemask
  EXPECT_EQ(1, results[1]);                    // color writemask red
  EXPECT_EQ(1, results[2]);                    // color writemask green
  EXPECT_EQ(1, results[3]);                    // color writemask blue
  EXPECT_EQ(1, results[4]);                    // color writemask alpha
  EXPECT_EQ(-1, results[5]);                   // stencil writemask alpha
  EXPECT_EQ(kSentinel, results[num_results]);  // End of results
}

TEST_P(GLES2DecoderTest, GetMultipleIntegervCHROMIUMInvalidArgs) {
  const GLsizei kCount = 3;
  // Offset the pnames because GLGetError will use the first uint32.
  const uint32 kPnameOffset = sizeof(uint32);
  const uint32 kResultsOffset = kPnameOffset + sizeof(GLint) * kCount;
  GLenum* pnames = GetSharedMemoryAsWithOffset<GLenum*>(kPnameOffset);
  pnames[0] = GL_DEPTH_WRITEMASK;
  pnames[1] = GL_COLOR_WRITEMASK;
  pnames[2] = GL_STENCIL_WRITEMASK;
  GLint* results = GetSharedMemoryAsWithOffset<GLint*>(kResultsOffset);

  GLsizei num_results = 0;
  for (GLsizei ii = 0; ii < kCount; ++ii) {
    num_results += decoder_->GetGLES2Util()->GLGetNumValuesReturned(pnames[ii]);
  }
  const GLsizei result_size = num_results * sizeof(*results);
  memset(results, 0, result_size);

  const GLint kSentinel = 0x12345678;
  results[num_results] = kSentinel;

  GetMultipleIntegervCHROMIUM cmd;
  // Check bad pnames pointer.
  cmd.Init(kInvalidSharedMemoryId,
           kSharedMemoryOffset + kPnameOffset,
           kCount,
           kSharedMemoryId,
           kSharedMemoryOffset + kResultsOffset,
           result_size);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // Check bad pnames pointer.
  cmd.Init(kSharedMemoryId,
           kInvalidSharedMemoryOffset,
           kCount,
           kSharedMemoryId,
           kSharedMemoryOffset + kResultsOffset,
           result_size);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // Check bad count.
  cmd.Init(kSharedMemoryId,
           kSharedMemoryOffset + kPnameOffset,
           -1,
           kSharedMemoryId,
           kSharedMemoryOffset + kResultsOffset,
           result_size);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // Check bad results pointer.
  cmd.Init(kSharedMemoryId,
           kSharedMemoryOffset + kPnameOffset,
           kCount,
           kInvalidSharedMemoryId,
           kSharedMemoryOffset + kResultsOffset,
           result_size);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // Check bad results pointer.
  cmd.Init(kSharedMemoryId,
           kSharedMemoryOffset + kPnameOffset,
           kCount,
           kSharedMemoryId,
           kInvalidSharedMemoryOffset,
           result_size);
  EXPECT_EQ(error::kOutOfBounds, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // Check bad size.
  cmd.Init(kSharedMemoryId,
           kSharedMemoryOffset + kPnameOffset,
           kCount,
           kSharedMemoryId,
           kSharedMemoryOffset + kResultsOffset,
           result_size + 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
  // Check bad size.
  cmd.Init(kSharedMemoryId,
           kSharedMemoryOffset + kPnameOffset,
           kCount,
           kSharedMemoryId,
           kSharedMemoryOffset + kResultsOffset,
           result_size - 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_VALUE, GetGLError());
  // Check bad enum.
  cmd.Init(kSharedMemoryId,
           kSharedMemoryOffset + kPnameOffset,
           kCount,
           kSharedMemoryId,
           kSharedMemoryOffset + kResultsOffset,
           result_size);
  GLenum temp = pnames[2];
  pnames[2] = GL_TRUE;
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
  pnames[2] = temp;
  // Check results area has not been cleared by client.
  results[1] = 1;
  EXPECT_EQ(error::kInvalidArguments, ExecuteCmd(cmd));
  // Check buffer is what we expect
  EXPECT_EQ(0, results[0]);
  EXPECT_EQ(1, results[1]);
  EXPECT_EQ(0, results[2]);
  EXPECT_EQ(0, results[3]);
  EXPECT_EQ(0, results[4]);
  EXPECT_EQ(0, results[5]);
  EXPECT_EQ(kSentinel, results[num_results]);  // End of results
}

TEST_P(GLES2DecoderManualInitTest, BindGeneratesResourceFalse) {
  InitState init;
  init.gl_version = "3.0";
  InitDecoder(init);

  BindTexture cmd1;
  cmd1.Init(GL_TEXTURE_2D, kInvalidClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd1));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  BindBuffer cmd2;
  cmd2.Init(GL_ARRAY_BUFFER, kInvalidClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd2));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  BindFramebuffer cmd3;
  cmd3.Init(GL_FRAMEBUFFER, kInvalidClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd3));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  BindRenderbuffer cmd4;
  cmd4.Init(GL_RENDERBUFFER, kInvalidClientId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd4));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
}

TEST_P(GLES2DecoderTest, EnableFeatureCHROMIUMBadBucket) {
  const uint32 kBadBucketId = 123;
  EnableFeatureCHROMIUM cmd;
  cmd.Init(kBadBucketId, shared_memory_id_, shared_memory_offset_);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, RequestExtensionCHROMIUMBadBucket) {
  const uint32 kBadBucketId = 123;
  RequestExtensionCHROMIUM cmd;
  cmd.Init(kBadBucketId);
  EXPECT_NE(error::kNoError, ExecuteCmd(cmd));
}

TEST_P(GLES2DecoderTest, BeginQueryEXTDisabled) {
  // Test something fails if off.
}

TEST_P(GLES2DecoderManualInitTest, BeginEndQueryEXT) {
  InitState init;
  init.extensions = "GL_EXT_occlusion_query_boolean";
  init.gl_version = "opengl es 2.0";
  init.has_alpha = true;
  init.request_alpha = true;
  init.bind_generates_resource = true;
  InitDecoder(init);

  // Test end fails if no begin.
  EndQueryEXT end_cmd;
  end_cmd.Init(GL_ANY_SAMPLES_PASSED_EXT, 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  BeginQueryEXT begin_cmd;

  // Test id = 0 fails.
  begin_cmd.Init(
      GL_ANY_SAMPLES_PASSED_EXT, 0, kSharedMemoryId, kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  GenHelper<GenQueriesEXTImmediate>(kNewClientId);

  // Test valid parameters work.
  EXPECT_CALL(*gl_, GenQueriesARB(1, _))
      .WillOnce(SetArgumentPointee<1>(kNewServiceId))
      .RetiresOnSaturation();
  EXPECT_CALL(*gl_, BeginQueryARB(GL_ANY_SAMPLES_PASSED_EXT, kNewServiceId))
      .Times(1)
      .RetiresOnSaturation();

  // Query object should not be created untill BeginQueriesEXT.
  QueryManager* query_manager = decoder_->GetQueryManager();
  ASSERT_TRUE(query_manager != NULL);
  QueryManager::Query* query = query_manager->GetQuery(kNewClientId);
  EXPECT_TRUE(query == NULL);

  // BeginQueryEXT should fail  if id is not generated from GenQueriesEXT.
  begin_cmd.Init(GL_ANY_SAMPLES_PASSED_EXT,
                 kInvalidClientId,
                 kSharedMemoryId,
                 kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  begin_cmd.Init(GL_ANY_SAMPLES_PASSED_EXT,
                 kNewClientId,
                 kSharedMemoryId,
                 kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  // After BeginQueriesEXT id name should have query object associated with it.
  query = query_manager->GetQuery(kNewClientId);
  ASSERT_TRUE(query != NULL);
  EXPECT_FALSE(query->pending());

  // Test trying begin again fails
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  // Test end fails with different target
  end_cmd.Init(GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT, 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  // Test end succeeds
  EXPECT_CALL(*gl_, EndQueryARB(GL_ANY_SAMPLES_PASSED_EXT))
      .Times(1)
      .RetiresOnSaturation();
  end_cmd.Init(GL_ANY_SAMPLES_PASSED_EXT, 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(query->pending());

  EXPECT_CALL(*gl_, DeleteQueriesARB(1, _)).Times(1).RetiresOnSaturation();
}

struct QueryType {
  GLenum type;
  bool is_gl;
};

const QueryType kQueryTypes[] = {
    {GL_COMMANDS_ISSUED_CHROMIUM, false},
    {GL_LATENCY_QUERY_CHROMIUM, false},
    {GL_ASYNC_PIXEL_UNPACK_COMPLETED_CHROMIUM, false},
    {GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM, false},
    {GL_GET_ERROR_QUERY_CHROMIUM, false},
    {GL_COMMANDS_COMPLETED_CHROMIUM, false},
    {GL_ANY_SAMPLES_PASSED_EXT, true},
};

static void CheckBeginEndQueryBadMemoryFails(GLES2DecoderTestBase* test,
                                             GLuint client_id,
                                             GLuint service_id,
                                             const QueryType& query_type,
                                             int32 shm_id,
                                             uint32 shm_offset) {
  // We need to reset the decoder on each iteration, because we lose the
  // context every time.
  GLES2DecoderTestBase::InitState init;
  init.extensions = "GL_EXT_occlusion_query_boolean GL_ARB_sync";
  init.gl_version = "opengl es 2.0";
  init.has_alpha = true;
  init.request_alpha = true;
  init.bind_generates_resource = true;
  test->InitDecoder(init);
  ::testing::StrictMock< ::gfx::MockGLInterface>* gl = test->GetGLMock();

  BeginQueryEXT begin_cmd;

  test->GenHelper<GenQueriesEXTImmediate>(client_id);

  if (query_type.is_gl) {
    EXPECT_CALL(*gl, GenQueriesARB(1, _))
        .WillOnce(SetArgumentPointee<1>(service_id))
        .RetiresOnSaturation();
    EXPECT_CALL(*gl, BeginQueryARB(query_type.type, service_id))
        .Times(1)
        .RetiresOnSaturation();
  }

  // Test bad shared memory fails
  begin_cmd.Init(query_type.type, client_id, shm_id, shm_offset);
  error::Error error1 = test->ExecuteCmd(begin_cmd);

  if (query_type.is_gl) {
    EXPECT_CALL(*gl, EndQueryARB(query_type.type))
        .Times(1)
        .RetiresOnSaturation();
  }
  if (query_type.type == GL_GET_ERROR_QUERY_CHROMIUM) {
    EXPECT_CALL(*gl, GetError())
        .WillOnce(Return(GL_NO_ERROR))
        .RetiresOnSaturation();
  }
  GLsync kGlSync = reinterpret_cast<GLsync>(0xdeadbeef);
  if (query_type.type == GL_COMMANDS_COMPLETED_CHROMIUM) {
    EXPECT_CALL(*gl, Flush()).RetiresOnSaturation();
    EXPECT_CALL(*gl, FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0))
        .WillOnce(Return(kGlSync))
        .RetiresOnSaturation();
#if DCHECK_IS_ON
    EXPECT_CALL(*gl, IsSync(kGlSync))
        .WillOnce(Return(GL_TRUE))
        .RetiresOnSaturation();
#endif
  }

  EndQueryEXT end_cmd;
  end_cmd.Init(query_type.type, 1);
  error::Error error2 = test->ExecuteCmd(end_cmd);

  if (query_type.is_gl) {
    EXPECT_CALL(
        *gl, GetQueryObjectuivARB(service_id, GL_QUERY_RESULT_AVAILABLE_EXT, _))
        .WillOnce(SetArgumentPointee<2>(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*gl, GetQueryObjectuivARB(service_id, GL_QUERY_RESULT_EXT, _))
        .WillOnce(SetArgumentPointee<2>(1))
        .RetiresOnSaturation();
  }
  if (query_type.type == GL_COMMANDS_COMPLETED_CHROMIUM) {
#if DCHECK_IS_ON
    EXPECT_CALL(*gl, IsSync(kGlSync))
        .WillOnce(Return(GL_TRUE))
        .RetiresOnSaturation();
#endif
    EXPECT_CALL(*gl, ClientWaitSync(kGlSync, _, _))
        .WillOnce(Return(GL_ALREADY_SIGNALED))
        .RetiresOnSaturation();
  }

  QueryManager* query_manager = test->GetDecoder()->GetQueryManager();
  ASSERT_TRUE(query_manager != NULL);
  bool process_success = query_manager->ProcessPendingQueries();

  EXPECT_TRUE(error1 != error::kNoError || error2 != error::kNoError ||
              !process_success);

  if (query_type.is_gl) {
    EXPECT_CALL(*gl, DeleteQueriesARB(1, _)).Times(1).RetiresOnSaturation();
  }
  if (query_type.type == GL_COMMANDS_COMPLETED_CHROMIUM) {
#if DCHECK_IS_ON
    EXPECT_CALL(*gl, IsSync(kGlSync))
        .WillOnce(Return(GL_TRUE))
        .RetiresOnSaturation();
#endif
    EXPECT_CALL(*gl, DeleteSync(kGlSync)).Times(1).RetiresOnSaturation();
  }
  test->ResetDecoder();
}

TEST_P(GLES2DecoderManualInitTest, BeginEndQueryEXTBadMemoryIdFails) {
  for (size_t i = 0; i < arraysize(kQueryTypes); ++i) {
    CheckBeginEndQueryBadMemoryFails(this,
                                     kNewClientId,
                                     kNewServiceId,
                                     kQueryTypes[i],
                                     kInvalidSharedMemoryId,
                                     kSharedMemoryOffset);
  }
}

TEST_P(GLES2DecoderManualInitTest, BeginEndQueryEXTBadMemoryOffsetFails) {
  for (size_t i = 0; i < arraysize(kQueryTypes); ++i) {
    // Out-of-bounds.
    CheckBeginEndQueryBadMemoryFails(this,
                                     kNewClientId,
                                     kNewServiceId,
                                     kQueryTypes[i],
                                     kSharedMemoryId,
                                     kInvalidSharedMemoryOffset);
    // Overflow.
    CheckBeginEndQueryBadMemoryFails(this,
                                     kNewClientId,
                                     kNewServiceId,
                                     kQueryTypes[i],
                                     kSharedMemoryId,
                                     0xfffffffcu);
  }
}

TEST_P(GLES2DecoderTest, BeginEndQueryEXTCommandsIssuedCHROMIUM) {
  BeginQueryEXT begin_cmd;

  GenHelper<GenQueriesEXTImmediate>(kNewClientId);

  // Test valid parameters work.
  begin_cmd.Init(GL_COMMANDS_ISSUED_CHROMIUM,
                 kNewClientId,
                 kSharedMemoryId,
                 kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  QueryManager* query_manager = decoder_->GetQueryManager();
  ASSERT_TRUE(query_manager != NULL);
  QueryManager::Query* query = query_manager->GetQuery(kNewClientId);
  ASSERT_TRUE(query != NULL);
  EXPECT_FALSE(query->pending());

  // Test end succeeds
  EndQueryEXT end_cmd;
  end_cmd.Init(GL_COMMANDS_ISSUED_CHROMIUM, 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_FALSE(query->pending());
}

TEST_P(GLES2DecoderTest, BeginEndQueryEXTGetErrorQueryCHROMIUM) {
  BeginQueryEXT begin_cmd;

  GenHelper<GenQueriesEXTImmediate>(kNewClientId);

  // Test valid parameters work.
  begin_cmd.Init(GL_GET_ERROR_QUERY_CHROMIUM,
                 kNewClientId,
                 kSharedMemoryId,
                 kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  QueryManager* query_manager = decoder_->GetQueryManager();
  ASSERT_TRUE(query_manager != NULL);
  QueryManager::Query* query = query_manager->GetQuery(kNewClientId);
  ASSERT_TRUE(query != NULL);
  EXPECT_FALSE(query->pending());

  // Test end succeeds
  QuerySync* sync = static_cast<QuerySync*>(shared_memory_address_);

  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_INVALID_VALUE))
      .RetiresOnSaturation();

  EndQueryEXT end_cmd;
  end_cmd.Init(GL_GET_ERROR_QUERY_CHROMIUM, 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_FALSE(query->pending());
  EXPECT_EQ(static_cast<GLenum>(GL_INVALID_VALUE),
            static_cast<GLenum>(sync->result));
}

TEST_P(GLES2DecoderManualInitTest, BeginEndQueryEXTCommandsCompletedCHROMIUM) {
  InitState init;
  init.extensions = "GL_EXT_occlusion_query_boolean GL_ARB_sync";
  init.gl_version = "opengl es 2.0";
  init.has_alpha = true;
  init.request_alpha = true;
  init.bind_generates_resource = true;
  InitDecoder(init);

  GenHelper<GenQueriesEXTImmediate>(kNewClientId);

  BeginQueryEXT begin_cmd;
  begin_cmd.Init(GL_COMMANDS_COMPLETED_CHROMIUM,
                 kNewClientId,
                 kSharedMemoryId,
                 kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  QueryManager* query_manager = decoder_->GetQueryManager();
  ASSERT_TRUE(query_manager != NULL);
  QueryManager::Query* query = query_manager->GetQuery(kNewClientId);
  ASSERT_TRUE(query != NULL);
  EXPECT_FALSE(query->pending());

  GLsync kGlSync = reinterpret_cast<GLsync>(0xdeadbeef);
  EXPECT_CALL(*gl_, Flush()).RetiresOnSaturation();
  EXPECT_CALL(*gl_, FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0))
      .WillOnce(Return(kGlSync))
      .RetiresOnSaturation();
#if DCHECK_IS_ON
  EXPECT_CALL(*gl_, IsSync(kGlSync))
      .WillOnce(Return(GL_TRUE))
      .RetiresOnSaturation();
#endif

  EndQueryEXT end_cmd;
  end_cmd.Init(GL_COMMANDS_COMPLETED_CHROMIUM, 1);
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_TRUE(query->pending());

#if DCHECK_IS_ON
  EXPECT_CALL(*gl_, IsSync(kGlSync))
      .WillOnce(Return(GL_TRUE))
      .RetiresOnSaturation();
#endif
  EXPECT_CALL(*gl_, ClientWaitSync(kGlSync, _, _))
      .WillOnce(Return(GL_TIMEOUT_EXPIRED))
      .RetiresOnSaturation();
  bool process_success = query_manager->ProcessPendingQueries();

  EXPECT_TRUE(process_success);
  EXPECT_TRUE(query->pending());

#if DCHECK_IS_ON
  EXPECT_CALL(*gl_, IsSync(kGlSync))
      .WillOnce(Return(GL_TRUE))
      .RetiresOnSaturation();
#endif
  EXPECT_CALL(*gl_, ClientWaitSync(kGlSync, _, _))
      .WillOnce(Return(GL_ALREADY_SIGNALED))
      .RetiresOnSaturation();
  process_success = query_manager->ProcessPendingQueries();

  EXPECT_TRUE(process_success);
  EXPECT_FALSE(query->pending());
  QuerySync* sync = static_cast<QuerySync*>(shared_memory_address_);
  EXPECT_EQ(static_cast<GLenum>(0), static_cast<GLenum>(sync->result));

#if DCHECK_IS_ON
  EXPECT_CALL(*gl_, IsSync(kGlSync))
      .WillOnce(Return(GL_TRUE))
      .RetiresOnSaturation();
#endif
  EXPECT_CALL(*gl_, DeleteSync(kGlSync)).Times(1).RetiresOnSaturation();
  ResetDecoder();
}

TEST_P(GLES2DecoderTest, IsEnabledReturnsCachedValue) {
  // NOTE: There are no expectations because no GL functions should be
  // called for DEPTH_TEST or STENCIL_TEST
  static const GLenum kStates[] = {
      GL_DEPTH_TEST, GL_STENCIL_TEST,
  };
  for (size_t ii = 0; ii < arraysize(kStates); ++ii) {
    Enable enable_cmd;
    GLenum state = kStates[ii];
    enable_cmd.Init(state);
    EXPECT_EQ(error::kNoError, ExecuteCmd(enable_cmd));
    IsEnabled::Result* result =
        static_cast<IsEnabled::Result*>(shared_memory_address_);
    IsEnabled is_enabled_cmd;
    is_enabled_cmd.Init(state, shared_memory_id_, shared_memory_offset_);
    EXPECT_EQ(error::kNoError, ExecuteCmd(is_enabled_cmd));
    EXPECT_NE(0u, *result);
    Disable disable_cmd;
    disable_cmd.Init(state);
    EXPECT_EQ(error::kNoError, ExecuteCmd(disable_cmd));
    EXPECT_EQ(error::kNoError, ExecuteCmd(is_enabled_cmd));
    EXPECT_EQ(0u, *result);
  }
}

TEST_P(GLES2DecoderManualInitTest, GpuMemoryManagerCHROMIUM) {
  InitState init;
  init.extensions = "GL_ARB_texture_rectangle";
  init.gl_version = "3.0";
  init.bind_generates_resource = true;
  InitDecoder(init);

  Texture* texture = GetTexture(client_texture_id_)->texture();
  EXPECT_TRUE(texture != NULL);
  EXPECT_TRUE(texture->pool() == GL_TEXTURE_POOL_UNMANAGED_CHROMIUM);

  DoBindTexture(GL_TEXTURE_2D, client_texture_id_, kServiceTextureId);

  TexParameteri cmd;
  cmd.Init(GL_TEXTURE_2D,
           GL_TEXTURE_POOL_CHROMIUM,
           GL_TEXTURE_POOL_UNMANAGED_CHROMIUM);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  cmd.Init(GL_TEXTURE_2D,
           GL_TEXTURE_POOL_CHROMIUM,
           GL_TEXTURE_POOL_MANAGED_CHROMIUM);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  EXPECT_TRUE(texture->pool() == GL_TEXTURE_POOL_MANAGED_CHROMIUM);

  cmd.Init(GL_TEXTURE_2D, GL_TEXTURE_POOL_CHROMIUM, GL_NONE);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

namespace {

class SizeOnlyMemoryTracker : public MemoryTracker {
 public:
  SizeOnlyMemoryTracker() {
    // These are the default textures. 1 for TEXTURE_2D and 6 faces for
    // TEXTURE_CUBE_MAP.
    const size_t kInitialUnmanagedPoolSize = 7 * 4;
    const size_t kInitialManagedPoolSize = 0;
    pool_infos_[MemoryTracker::kUnmanaged].initial_size =
        kInitialUnmanagedPoolSize;
    pool_infos_[MemoryTracker::kManaged].initial_size = kInitialManagedPoolSize;
  }

  // Ensure a certain amount of GPU memory is free. Returns true on success.
  MOCK_METHOD1(EnsureGPUMemoryAvailable, bool(size_t size_needed));

  virtual void TrackMemoryAllocatedChange(size_t old_size,
                                          size_t new_size,
                                          Pool pool) {
    PoolInfo& info = pool_infos_[pool];
    info.size += new_size - old_size;
  }

  size_t GetPoolSize(Pool pool) {
    const PoolInfo& info = pool_infos_[pool];
    return info.size - info.initial_size;
  }

 private:
  virtual ~SizeOnlyMemoryTracker() {}
  struct PoolInfo {
    PoolInfo() : initial_size(0), size(0) {}
    size_t initial_size;
    size_t size;
  };
  std::map<Pool, PoolInfo> pool_infos_;
};

}  // anonymous namespace.

TEST_P(GLES2DecoderManualInitTest, MemoryTrackerInitialSize) {
  scoped_refptr<SizeOnlyMemoryTracker> memory_tracker =
      new SizeOnlyMemoryTracker();
  set_memory_tracker(memory_tracker.get());
  InitState init;
  init.gl_version = "3.0";
  init.bind_generates_resource = true;
  InitDecoder(init);
  // Expect that initial size - size is 0.
  EXPECT_EQ(0u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
  EXPECT_EQ(0u, memory_tracker->GetPoolSize(MemoryTracker::kManaged));
}

TEST_P(GLES2DecoderManualInitTest, MemoryTrackerTexImage2D) {
  scoped_refptr<SizeOnlyMemoryTracker> memory_tracker =
      new SizeOnlyMemoryTracker();
  set_memory_tracker(memory_tracker.get());
  InitState init;
  init.gl_version = "3.0";
  init.bind_generates_resource = true;
  InitDecoder(init);
  DoBindTexture(GL_TEXTURE_2D, client_texture_id_, kServiceTextureId);
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(true))
      .RetiresOnSaturation();
  DoTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               8,
               4,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               kSharedMemoryId,
               kSharedMemoryOffset);
  EXPECT_EQ(128u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(64))
      .WillOnce(Return(true))
      .RetiresOnSaturation();
  DoTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               4,
               4,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               kSharedMemoryId,
               kSharedMemoryOffset);
  EXPECT_EQ(64u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // Check we get out of memory and no call to glTexImage2D if Ensure fails.
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(64))
      .WillOnce(Return(false))
      .RetiresOnSaturation();
  TexImage2D cmd;
  cmd.Init(GL_TEXTURE_2D,
           0,
           GL_RGBA,
           4,
           4,
           GL_RGBA,
           GL_UNSIGNED_BYTE,
           kSharedMemoryId,
           kSharedMemoryOffset);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
  EXPECT_EQ(64u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
}

TEST_P(GLES2DecoderManualInitTest, MemoryTrackerTexStorage2DEXT) {
  scoped_refptr<SizeOnlyMemoryTracker> memory_tracker =
      new SizeOnlyMemoryTracker();
  set_memory_tracker(memory_tracker.get());
  InitState init;
  init.gl_version = "3.0";
  init.bind_generates_resource = true;
  InitDecoder(init);
  DoBindTexture(GL_TEXTURE_2D, client_texture_id_, kServiceTextureId);
  // Check we get out of memory and no call to glTexStorage2DEXT
  // if Ensure fails.
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(false))
      .RetiresOnSaturation();
  TexStorage2DEXT cmd;
  cmd.Init(GL_TEXTURE_2D, 1, GL_RGBA8, 8, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(0u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
  EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
}

TEST_P(GLES2DecoderManualInitTest, MemoryTrackerCopyTexImage2D) {
  GLenum target = GL_TEXTURE_2D;
  GLint level = 0;
  GLenum internal_format = GL_RGBA;
  GLsizei width = 4;
  GLsizei height = 8;
  GLint border = 0;
  scoped_refptr<SizeOnlyMemoryTracker> memory_tracker =
      new SizeOnlyMemoryTracker();
  set_memory_tracker(memory_tracker.get());
  InitState init;
  init.gl_version = "3.0";
  init.has_alpha = true;
  init.request_alpha = true;
  init.bind_generates_resource = true;
  InitDecoder(init);
  DoBindTexture(GL_TEXTURE_2D, client_texture_id_, kServiceTextureId);
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(true))
      .RetiresOnSaturation();
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  EXPECT_CALL(*gl_,
              CopyTexImage2D(
                  target, level, internal_format, 0, 0, width, height, border))
      .Times(1)
      .RetiresOnSaturation();
  CopyTexImage2D cmd;
  cmd.Init(target, level, internal_format, 0, 0, width, height);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(128u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  // Check we get out of memory and no call to glCopyTexImage2D if Ensure fails.
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(false))
      .RetiresOnSaturation();
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
  EXPECT_EQ(128u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
}

TEST_P(GLES2DecoderManualInitTest, MemoryTrackerRenderbufferStorage) {
  scoped_refptr<SizeOnlyMemoryTracker> memory_tracker =
      new SizeOnlyMemoryTracker();
  set_memory_tracker(memory_tracker.get());
  InitState init;
  init.gl_version = "3.0";
  init.bind_generates_resource = true;
  InitDecoder(init);
  DoBindRenderbuffer(
      GL_RENDERBUFFER, client_renderbuffer_id_, kServiceRenderbufferId);
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(true))
      .RetiresOnSaturation();
  EXPECT_CALL(*gl_, RenderbufferStorageEXT(GL_RENDERBUFFER, GL_RGBA, 8, 4))
      .Times(1)
      .RetiresOnSaturation();
  RenderbufferStorage cmd;
  cmd.Init(GL_RENDERBUFFER, GL_RGBA4, 8, 4);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_EQ(128u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
  // Check we get out of memory and no call to glRenderbufferStorage if Ensure
  // fails.
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(false))
      .RetiresOnSaturation();
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
  EXPECT_EQ(128u, memory_tracker->GetPoolSize(MemoryTracker::kUnmanaged));
}

TEST_P(GLES2DecoderManualInitTest, MemoryTrackerBufferData) {
  scoped_refptr<SizeOnlyMemoryTracker> memory_tracker =
      new SizeOnlyMemoryTracker();
  set_memory_tracker(memory_tracker.get());
  InitState init;
  init.gl_version = "3.0";
  init.bind_generates_resource = true;
  InitDecoder(init);
  DoBindBuffer(GL_ARRAY_BUFFER, client_buffer_id_, kServiceBufferId);
  EXPECT_CALL(*gl_, GetError())
      .WillOnce(Return(GL_NO_ERROR))
      .WillOnce(Return(GL_NO_ERROR))
      .RetiresOnSaturation();
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(true))
      .RetiresOnSaturation();
  EXPECT_CALL(*gl_, BufferData(GL_ARRAY_BUFFER, 128, _, GL_STREAM_DRAW))
      .Times(1)
      .RetiresOnSaturation();
  BufferData cmd;
  cmd.Init(GL_ARRAY_BUFFER, 128, 0, 0, GL_STREAM_DRAW);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
  EXPECT_EQ(128u, memory_tracker->GetPoolSize(MemoryTracker::kManaged));
  // Check we get out of memory and no call to glBufferData if Ensure
  // fails.
  EXPECT_CALL(*memory_tracker.get(), EnsureGPUMemoryAvailable(128))
      .WillOnce(Return(false))
      .RetiresOnSaturation();
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
  EXPECT_EQ(128u, memory_tracker->GetPoolSize(MemoryTracker::kManaged));
}

TEST_P(GLES2DecoderManualInitTest, ImmutableCopyTexImage2D) {
  const GLenum kTarget = GL_TEXTURE_2D;
  const GLint kLevel = 0;
  const GLenum kInternalFormat = GL_RGBA;
  const GLenum kSizedInternalFormat = GL_RGBA8;
  const GLsizei kWidth = 4;
  const GLsizei kHeight = 8;
  const GLint kBorder = 0;
  InitState init;
  init.extensions = "GL_EXT_texture_storage";
  init.gl_version = "3.0";
  init.has_alpha = true;
  init.request_alpha = true;
  init.bind_generates_resource = true;
  InitDecoder(init);
  DoBindTexture(GL_TEXTURE_2D, client_texture_id_, kServiceTextureId);

  // CopyTexImage2D will call arbitrary amount of GetErrors.
  EXPECT_CALL(*gl_, GetError())
      .Times(AtLeast(1));

  EXPECT_CALL(*gl_,
              CopyTexImage2D(
                  kTarget, kLevel, kInternalFormat, 0, 0, kWidth, kHeight,
                  kBorder))
      .Times(1);

  EXPECT_CALL(*gl_,
              TexStorage2DEXT(
                  kTarget, kLevel, kSizedInternalFormat, kWidth, kHeight))
      .Times(1);
  CopyTexImage2D copy_cmd;
  copy_cmd.Init(kTarget, kLevel, kInternalFormat, 0, 0, kWidth, kHeight);
  EXPECT_EQ(error::kNoError, ExecuteCmd(copy_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  TexStorage2DEXT storage_cmd;
  storage_cmd.Init(kTarget, kLevel, kSizedInternalFormat, kWidth, kHeight);
  EXPECT_EQ(error::kNoError, ExecuteCmd(storage_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());

  // This should not invoke CopyTexImage2D.
  copy_cmd.Init(kTarget, kLevel, kInternalFormat, 0, 0, kWidth, kHeight);
  EXPECT_EQ(error::kNoError, ExecuteCmd(copy_cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());
}

TEST_P(GLES2DecoderTest, LoseContextCHROMIUMValidArgs) {
  EXPECT_CALL(*mock_decoder_, LoseContext(GL_GUILTY_CONTEXT_RESET_ARB))
      .Times(1);
  cmds::LoseContextCHROMIUM cmd;
  cmd.Init(GL_GUILTY_CONTEXT_RESET_ARB, GL_GUILTY_CONTEXT_RESET_ARB);
  EXPECT_EQ(error::kLostContext, ExecuteCmd(cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

TEST_P(GLES2DecoderTest, LoseContextCHROMIUMInvalidArgs0_0) {
  EXPECT_CALL(*mock_decoder_, LoseContext(_))
      .Times(0);
  cmds::LoseContextCHROMIUM cmd;
  cmd.Init(GL_NONE, GL_GUILTY_CONTEXT_RESET_ARB);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

TEST_P(GLES2DecoderTest, LoseContextCHROMIUMInvalidArgs1_0) {
  EXPECT_CALL(*mock_decoder_, LoseContext(_))
      .Times(0);
  cmds::LoseContextCHROMIUM cmd;
  cmd.Init(GL_GUILTY_CONTEXT_RESET_ARB, GL_NONE);
  EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
  EXPECT_EQ(GL_INVALID_ENUM, GetGLError());
}

INSTANTIATE_TEST_CASE_P(Service, GLES2DecoderTest, ::testing::Bool());

INSTANTIATE_TEST_CASE_P(Service, GLES2DecoderWithShaderTest, ::testing::Bool());

INSTANTIATE_TEST_CASE_P(Service, GLES2DecoderManualInitTest, ::testing::Bool());

INSTANTIATE_TEST_CASE_P(Service,
                        GLES2DecoderRGBBackbufferTest,
                        ::testing::Bool());

}  // namespace gles2
}  // namespace gpu
