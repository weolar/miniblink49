// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/config/gpu_control_list.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {

class StringInfoTest : public testing::Test {
 public:
  StringInfoTest() { }
  virtual ~StringInfoTest() { }

  typedef GpuControlList::StringInfo StringInfo;
};

TEST_F(StringInfoTest, ValidStringInfo) {
  const std::string op[] = {
    "contains",
    "beginwith",
    "endwith",
    "="
  };
  for (size_t i = 0; i < arraysize(op); ++i) {
    {
      StringInfo info(op[i], std::string());
      EXPECT_TRUE(info.IsValid());
    }
    {
      StringInfo info(op[i], "hello");
      EXPECT_TRUE(info.IsValid());
    }
  }
}

TEST_F(StringInfoTest, InvalidStringInfo) {
  const std::string op[] = {
    "Contains",
    "BeginWith",
    "EndWith",
    " =",
    "= "
  };
  for (size_t i = 0; i < arraysize(op); ++i) {
    StringInfo info(op[i], "hello");
    EXPECT_FALSE(info.IsValid());
  }
}

TEST_F(StringInfoTest, StringComparison) {
  {
    StringInfo info("contains", "happy");
    EXPECT_TRUE(info.Contains("unhappy"));
    EXPECT_TRUE(info.Contains("happy1"));
    EXPECT_TRUE(info.Contains("happy"));
    EXPECT_TRUE(info.Contains("a happy dog"));
    EXPECT_TRUE(info.Contains("Happy"));
    EXPECT_TRUE(info.Contains("HAPPY"));
    EXPECT_FALSE(info.Contains("ha-ppy"));
  }
  {
    StringInfo info("beginwith", "happy");
    EXPECT_FALSE(info.Contains("unhappy"));
    EXPECT_TRUE(info.Contains("happy1"));
    EXPECT_TRUE(info.Contains("happy"));
    EXPECT_FALSE(info.Contains("a happy dog"));
    EXPECT_TRUE(info.Contains("Happy"));
    EXPECT_TRUE(info.Contains("HAPPY"));
    EXPECT_FALSE(info.Contains("ha-ppy"));
  }
  {
    StringInfo info("endwith", "happy");
    EXPECT_TRUE(info.Contains("unhappy"));
    EXPECT_FALSE(info.Contains("happy1"));
    EXPECT_TRUE(info.Contains("happy"));
    EXPECT_FALSE(info.Contains("a happy dog"));
    EXPECT_TRUE(info.Contains("Happy"));
    EXPECT_TRUE(info.Contains("HAPPY"));
    EXPECT_FALSE(info.Contains("ha-ppy"));
  }
  {
    StringInfo info("=", "happy");
    EXPECT_FALSE(info.Contains("unhappy"));
    EXPECT_FALSE(info.Contains("happy1"));
    EXPECT_TRUE(info.Contains("happy"));
    EXPECT_FALSE(info.Contains("a happy dog"));
    EXPECT_TRUE(info.Contains("Happy"));
    EXPECT_TRUE(info.Contains("HAPPY"));
    EXPECT_FALSE(info.Contains("ha-ppy"));
    EXPECT_FALSE(info.Contains("ha ppy"));
    EXPECT_FALSE(info.Contains(" happy"));
    EXPECT_FALSE(info.Contains("happy "));
  }
}

}  // namespace gpu

