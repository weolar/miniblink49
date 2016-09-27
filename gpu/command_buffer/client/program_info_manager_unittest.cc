// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Tests for the Command Buffer Helper.

#include "gpu/command_buffer/client/program_info_manager.h"
#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace gpu {
namespace gles2 {

class ProgramInfoManagerTest : public testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  scoped_ptr<ProgramInfoManager> program_info_manager_;
};

TEST_F(ProgramInfoManagerTest, Basic) {
}

}  // namespace gles2
}  // namespace gpu


