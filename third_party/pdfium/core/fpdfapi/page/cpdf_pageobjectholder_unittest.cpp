// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/page/cpdf_pageobjectholder.h"

#include <limits>

#include "testing/gtest/include/gtest/gtest.h"

// See https://crbug.com/852273
TEST(CPDFPageObjectHolder, GraphicsDataAsKey) {
  const float fMin = std::numeric_limits<float>::min();
  const float fMax = std::numeric_limits<float>::max();
  const float fInf = std::numeric_limits<float>::infinity();
  const float fNan = std::numeric_limits<float>::quiet_NaN();

  // Verify self-comparisions.
  for (float c1 : {fMin, 1.0f, 2.0f, fMax, fInf, fNan}) {
    for (float c2 : {fMin, 1.0f, 2.0f, fMax, fInf, fNan}) {
      for (BlendMode c3 : {BlendMode::kMultiply, BlendMode::kScreen})
        EXPECT_FALSE(GraphicsData({c1, c2, c3}) < GraphicsData({c1, c2, c3}));
    }
  }

  std::map<GraphicsData, int> graphics_map;

  // Insert in reverse index permuted order.
  size_t x = 0;
  for (BlendMode c3 : {BlendMode::kScreen, BlendMode::kMultiply}) {
    for (float c2 : {fNan, fInf, fMax, 2.0f, 1.0f, fMin}) {
      for (float c1 : {fNan, fInf, fMax, 2.0f, 1.0f, fMin}) {
        graphics_map[{c1, c2, c3}] = x++;
      }
    }
  }
  EXPECT_EQ(72u, x);
  EXPECT_EQ(72u, graphics_map.size());

  // clang-format off
  const int expected[72] = {
      71, 35, 65, 29, 59, 23, 53, 17, 47, 11, 41, 5,
      70, 34, 64, 28, 58, 22, 52, 16, 46, 10, 40, 4,
      69, 33, 63, 27, 57, 21, 51, 15, 45, 9,  39, 3,
      68, 32, 62, 26, 56, 20, 50, 14, 44, 8,  38, 2,
      67, 31, 61, 25, 55, 19, 49, 13, 43, 7,  37, 1,
      66, 30, 60, 24, 54, 18, 48, 12, 42, 6,  36, 0
  };
  // clang-format on

  x = 0;
  for (const auto& item : graphics_map) {
    EXPECT_EQ(expected[x], item.second) << " for position " << x;
    ++x;
  }
  EXPECT_EQ(72u, x);

  // Erase in forward index permuted order.
  for (BlendMode c3 : {BlendMode::kMultiply, BlendMode::kScreen}) {
    for (float c2 : {fMin, 1.0f, 2.0f, fMax, fInf, fNan}) {
      for (float c1 : {fMin, 1.0f, 2.0f, fMax, fInf, fNan})
        graphics_map.erase({c1, c2, c3});
    }
  }
  EXPECT_EQ(0u, graphics_map.size());
}
