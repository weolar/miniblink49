// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/parser/cxfa_localevalue.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

namespace {

// We don't expect more precision than a float's worth from this code.
float MakeDoubleNumAsFloat(const wchar_t* str) {
  return static_cast<float>(
      CXFA_LocaleValue(XFA_VT_FLOAT, str, nullptr).GetDoubleNum());
}

}  // namespace

TEST(CXFALocaleValueTest, GetDoubleNum) {
  EXPECT_EQ(0.0, MakeDoubleNumAsFloat(L""));
  EXPECT_EQ(0.0, MakeDoubleNumAsFloat(L"0"));
  EXPECT_EQ(0.0, MakeDoubleNumAsFloat(L"0."));
  EXPECT_EQ(0.0, MakeDoubleNumAsFloat(L"0.0"));
  EXPECT_EQ(0.0, MakeDoubleNumAsFloat(L"0.x"));
  EXPECT_EQ(7.0, MakeDoubleNumAsFloat(L"7.x"));
  EXPECT_FLOAT_EQ(0.54321f, MakeDoubleNumAsFloat(L".54321"));
  EXPECT_FLOAT_EQ(0.54321f, MakeDoubleNumAsFloat(L"0.54321"));
  EXPECT_FLOAT_EQ(0.54321f, MakeDoubleNumAsFloat(L"+0.54321"));
  EXPECT_FLOAT_EQ(0.54321f, MakeDoubleNumAsFloat(L"   +0.54321"));
  EXPECT_FLOAT_EQ(-0.54321f, MakeDoubleNumAsFloat(L"-.54321"));
  EXPECT_FLOAT_EQ(-0.54321f, MakeDoubleNumAsFloat(L"-0.54321"));
  EXPECT_FLOAT_EQ(-0.54321f, MakeDoubleNumAsFloat(L"  -0.54321"));
  EXPECT_FLOAT_EQ(-0.054321f, MakeDoubleNumAsFloat(L"-0.54321e-1"));
  EXPECT_FLOAT_EQ(-0.54321f, MakeDoubleNumAsFloat(L"-0.54321e0"));
  EXPECT_FLOAT_EQ(-5.4321f, MakeDoubleNumAsFloat(L"-0.54321e1"));
}
