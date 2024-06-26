// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/fxfa_basic.h"

#include "core/fxcrt/bytestring.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

void HashTestCase(uint32_t hash, const char* str, uint32_t* so_far) {
  if (hash != 0xffffffffu) {
    EXPECT_EQ(hash, FX_HashCode_GetAsIfW(str, false)) << str;
    EXPECT_LT(*so_far, hash) << hash;
  } else {
    EXPECT_NE(hash, FX_HashCode_GetAsIfW(str, false)) << str;
  }
  *so_far = hash;
}

}  // namespace

TEST(FXFABasic, PacketHashTest) {
  uint32_t so_far = 0;
#undef PCKT____
#define PCKT____(a, b, c, d, e, f) HashTestCase(a, b, &so_far);
#include "xfa/fxfa/parser/packets.inc"
#undef PCKT____
}

TEST(FXFABasic, AttributeHashTest) {
  uint32_t so_far = 0;
#undef ATTR____
#define ATTR____(a, b, c, d) HashTestCase(a, b, &so_far);
#include "xfa/fxfa/parser/attributes.inc"
#undef ATTR____
}

TEST(FXFABasic, ValueHashTest) {
  uint32_t so_far = 0;
#undef VALUE____
#define VALUE____(a, b, c) HashTestCase(a, b, &so_far);
#include "xfa/fxfa/parser/attribute_values.inc"
#undef VALUE____
}

TEST(FXFABasic, ElementHashTest) {
  uint32_t so_far = 0;
#undef ELEM____
#define ELEM____(a, b, c, d) HashTestCase(a, b, &so_far);
#include "xfa/fxfa/parser/elements.inc"
#undef ELEM____
}
