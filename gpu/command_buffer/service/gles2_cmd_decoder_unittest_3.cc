// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gles2_cmd_decoder.h"

#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder_unittest_base.h"
#include "gpu/command_buffer/service/cmd_buffer_engine.h"
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/program_manager.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_mock.h"

using ::gfx::MockGLInterface;
using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::MatcherCast;
using ::testing::Pointee;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::SetArgumentPointee;
using ::testing::StrEq;

namespace gpu {
namespace gles2 {

using namespace cmds;

class GLES2DecoderTest3 : public GLES2DecoderTestBase {
 public:
  GLES2DecoderTest3() { }
};

INSTANTIATE_TEST_CASE_P(Service, GLES2DecoderTest3, ::testing::Bool());

TEST_P(GLES2DecoderTest3, TraceBeginCHROMIUM) {
  const uint32 kBucketId = 123;
  const char kName[] = "test_command";
  SetBucketAsCString(kBucketId, kName);

  TraceBeginCHROMIUM begin_cmd;
  begin_cmd.Init(kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));
}

TEST_P(GLES2DecoderTest3, TraceEndCHROMIUM) {
  // Test end fails if no begin.
  TraceEndCHROMIUM end_cmd;
  end_cmd.Init();
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_INVALID_OPERATION, GetGLError());

  const uint32 kBucketId = 123;
  const char kName[] = "test_command";
  SetBucketAsCString(kBucketId, kName);

  TraceBeginCHROMIUM begin_cmd;
  begin_cmd.Init(kBucketId);
  EXPECT_EQ(error::kNoError, ExecuteCmd(begin_cmd));

  end_cmd.Init();
  EXPECT_EQ(error::kNoError, ExecuteCmd(end_cmd));
  EXPECT_EQ(GL_NO_ERROR, GetGLError());
}

#include "gpu/command_buffer/service/gles2_cmd_decoder_unittest_3_autogen.h"

}  // namespace gles2
}  // namespace gpu
