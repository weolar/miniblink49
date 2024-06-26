// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/fx_memory.h"

#include <limits>

#include "testing/gtest/include/gtest/gtest.h"

namespace {

const size_t kMaxByteAlloc = std::numeric_limits<size_t>::max();
const size_t kMaxIntAlloc = kMaxByteAlloc / sizeof(int);
const size_t kOverflowIntAlloc = kMaxIntAlloc + 100;
const size_t kWidth = 640;
const size_t kOverflowIntAlloc2D = kMaxIntAlloc / kWidth + 10;

}  // namespace

// TODO(tsepez): re-enable OOM tests if we can find a way to
// prevent it from hosing the bots.
TEST(fxcrt, DISABLED_FX_AllocOOM) {
  EXPECT_DEATH_IF_SUPPORTED((void)FX_Alloc(int, kMaxIntAlloc), "");

  int* ptr = FX_Alloc(int, 1);
  EXPECT_TRUE(ptr);
  EXPECT_DEATH_IF_SUPPORTED((void)FX_Realloc(int, ptr, kMaxIntAlloc), "");
  FX_Free(ptr);
}

TEST(fxcrt, FX_AllocOverflow) {
  // |ptr| needs to be defined and used to avoid Clang optimizes away the
  // FX_Alloc() statement overzealously for optimized builds.
  int* ptr = nullptr;
  EXPECT_DEATH_IF_SUPPORTED(ptr = FX_Alloc(int, kOverflowIntAlloc), "") << ptr;

  ptr = FX_Alloc(int, 1);
  EXPECT_TRUE(ptr);
  EXPECT_DEATH_IF_SUPPORTED((void)FX_Realloc(int, ptr, kOverflowIntAlloc), "");
  FX_Free(ptr);
}

TEST(fxcrt, FX_AllocOverflow2D) {
  // |ptr| needs to be defined and used to avoid Clang optimizes away the
  // FX_Alloc() statement overzealously for optimized builds.
  int* ptr = nullptr;
  EXPECT_DEATH_IF_SUPPORTED(ptr = FX_Alloc2D(int, kWidth, kOverflowIntAlloc2D),
                            "")
      << ptr;
}

TEST(fxcrt, DISABLED_FX_TryAllocOOM) {
  EXPECT_FALSE(FX_TryAlloc(int, kMaxIntAlloc));

  int* ptr = FX_Alloc(int, 1);
  EXPECT_TRUE(ptr);
  EXPECT_FALSE(FX_TryRealloc(int, ptr, kMaxIntAlloc));
  FX_Free(ptr);
}

#if !defined(__GNUC__)
TEST(fxcrt, FX_TryAllocOverflow) {
  // |ptr| needs to be defined and used to avoid Clang optimizes away the
  // calloc() statement overzealously for optimized builds.
  int* ptr = (int*)calloc(sizeof(int), kOverflowIntAlloc);
  EXPECT_FALSE(ptr) << ptr;

  ptr = FX_Alloc(int, 1);
  EXPECT_TRUE(ptr);
  *ptr = 1492;  // Arbitrary sentinel.
  EXPECT_FALSE(FX_TryRealloc(int, ptr, kOverflowIntAlloc));
  EXPECT_EQ(1492, *ptr);
  FX_Free(ptr);
}
#endif

TEST(fxcrt, DISABLED_FXMEM_DefaultOOM) {
  EXPECT_FALSE(FXMEM_DefaultAlloc(kMaxByteAlloc));

  void* ptr = FXMEM_DefaultAlloc(1);
  EXPECT_TRUE(ptr);
  EXPECT_FALSE(FXMEM_DefaultRealloc(ptr, kMaxByteAlloc));
  FXMEM_DefaultFree(ptr);
}

TEST(fxcrt, FXAlign) {
  static_assert(std::numeric_limits<size_t>::max() % 2 == 1,
                "numeric limit must be odd for this test");

  size_t s0 = 0;
  size_t s1 = 1;
  size_t s2 = 2;
  size_t sbig = std::numeric_limits<size_t>::max() - 2;
  EXPECT_EQ(0u, FxAlignToBoundary<2>(s0));
  EXPECT_EQ(2u, FxAlignToBoundary<2>(s1));
  EXPECT_EQ(2u, FxAlignToBoundary<2>(s2));
  EXPECT_EQ(std::numeric_limits<size_t>::max() - 1, FxAlignToBoundary<2>(sbig));

  int i0 = 0;
  int i511 = 511;
  int i512 = 512;
  int ineg = -513;
  EXPECT_EQ(0, FxAlignToBoundary<512>(i0));
  EXPECT_EQ(512, FxAlignToBoundary<512>(i511));
  EXPECT_EQ(512, FxAlignToBoundary<512>(i512));
  EXPECT_EQ(-512, FxAlignToBoundary<512>(ineg));
}
