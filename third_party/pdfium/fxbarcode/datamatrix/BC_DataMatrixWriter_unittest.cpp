// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/datamatrix/BC_DataMatrixWriter.h"

#include <vector>

#include "core/fxcrt/fx_memory.h"
#include "testing/gtest/include/gtest/gtest.h"

class CBC_DataMatrixWriterTest : public testing::Test {
 public:
  CBC_DataMatrixWriterTest() = default;
  ~CBC_DataMatrixWriterTest() override = default;

  // testing::Test:
  void SetUp() override { BC_Library_Init(); }
  void TearDown() override { BC_Library_Destroy(); }
};

TEST_F(CBC_DataMatrixWriterTest, Encode) {
  CBC_DataMatrixWriter writer;
  int32_t width;
  int32_t height;

  {
    static constexpr int kExpectedDimension = 10;
    // clang-format off
    static constexpr uint8_t kExpectedData[] = {
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
        1, 1, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 1, 0, 1, 0, 0, 0, 0, 1, 0,
        1, 1, 1, 1, 0, 0, 0, 1, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        1, 1, 1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 1, 0,
        1, 0, 1, 0, 1, 1, 1, 1, 0, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    // clang-format on
    std::vector<uint8_t> data = writer.Encode(L"", &width, &height);
    ASSERT_EQ(FX_ArraySize(kExpectedData), data.size());
    ASSERT_EQ(kExpectedDimension, width);
    ASSERT_EQ(kExpectedDimension, height);
    for (size_t i = 0; i < FX_ArraySize(kExpectedData); ++i)
      EXPECT_EQ(kExpectedData[i], data[i]) << i;
  }
  {
    static constexpr int kExpectedDimension = 14;
    // clang-format off
    static constexpr uint8_t kExpectedData[] = {
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
        1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1,
        1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0,
        1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1,
        1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0,
        1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1,
        1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1,
        1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0,
        1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1,
        1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    // clang-format on
    std::vector<uint8_t> data = writer.Encode(L"helloworld", &width, &height);
    ASSERT_EQ(FX_ArraySize(kExpectedData), data.size());
    ASSERT_EQ(kExpectedDimension, width);
    ASSERT_EQ(kExpectedDimension, height);
    for (size_t i = 0; i < FX_ArraySize(kExpectedData); ++i)
      EXPECT_EQ(kExpectedData[i], data[i]) << i;
  }
  {
    static constexpr int kExpectedDimension = 10;
    // clang-format off
    static constexpr uint8_t kExpectedData[] = {
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
        1, 1, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 0, 0, 0, 1, 0, 1, 1, 0,
        1, 1, 0, 0, 1, 1, 0, 1, 0, 1,
        1, 1, 0, 0, 1, 1, 1, 0, 0, 0,
        1, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 1, 0, 1, 0, 1, 1, 1, 1, 0,
        1, 1, 1, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    // clang-format on
    std::vector<uint8_t> data = writer.Encode(L"12345", &width, &height);
    ASSERT_EQ(FX_ArraySize(kExpectedData), data.size());
    ASSERT_EQ(kExpectedDimension, width);
    ASSERT_EQ(kExpectedDimension, height);
    for (size_t i = 0; i < FX_ArraySize(kExpectedData); ++i)
      EXPECT_EQ(kExpectedData[i], data[i]) << i;
  }
  {
    static constexpr int kExpectedDimension = 18;
    // clang-format off
    static constexpr uint8_t kExpectedData[] = {
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
        1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1,
        1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0,
        1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1,
        1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0,
        1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1,
        1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0,
        1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1,
        1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0,
        1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1,
        1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0,
        1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0,
        1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1,
        1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0,
        1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    // clang-format on
    std::vector<uint8_t> data =
        writer.Encode(L"abcdefghijklmnopqrst", &width, &height);
    ASSERT_EQ(FX_ArraySize(kExpectedData), data.size());
    ASSERT_EQ(kExpectedDimension, width);
    ASSERT_EQ(kExpectedDimension, height);
    for (size_t i = 0; i < FX_ArraySize(kExpectedData); ++i)
      EXPECT_EQ(kExpectedData[i], data[i]) << i;
  }
  {
    std::vector<uint8_t> data = writer.Encode(L"hello world", &width, &height);
    ASSERT_TRUE(data.empty());
  }
}
