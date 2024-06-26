// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/fx_random.h"

#include <array>
#include <set>

#include "testing/gtest/include/gtest/gtest.h"

TEST(FX_Random, GenerateMT3600times) {
  // Prove this doesn't spin wait for a second each time.
  // Since our global seeds are sequential, they wont't collide once
  // seeded until 2^32 calls, and if the PNRG is any good, we won't
  // get the same sequence from different seeds, esp. with this few
  // iterations.
  std::set<std::array<uint32_t, 16>> seen;
  std::array<uint32_t, 16> current;
  for (int i = 0; i < 3600; ++i) {
    FX_Random_GenerateMT(current.data(), 16);
    EXPECT_TRUE(seen.insert(current).second);
  }
}
