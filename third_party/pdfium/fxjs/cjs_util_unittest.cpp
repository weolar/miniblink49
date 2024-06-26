// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxjs/cjs_util.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

TEST(CJS_Util, ParseDataType) {
  struct ParseDataTypeCase {
    const wchar_t* const input_string;
    const int expected;
  };

  // Commented out tests follow the spec but are not passing.
  const ParseDataTypeCase cases[] = {
      // Not conversions
      {L"", -1},
      {L"d", -1},

      // Simple cases
      {L"%d", UTIL_INT},
      {L"%x", UTIL_INT},
      {L"%f", UTIL_DOUBLE},
      {L"%s", UTIL_STRING},

      // nDecSep Not implemented
      // {L"%,0d", UTIL_INT},
      // {L"%,1d", UTIL_INT},
      // {L"%,2d", UTIL_INT},
      // {L"%,3d", UTIL_INT},
      // {L"%,4d", -1},
      // {L"%,d", -1},

      // cFlags("+ 0#"") are only valid for numeric conversions.
      {L"%+d", UTIL_INT},
      {L"%+x", UTIL_INT},
      {L"%+f", UTIL_DOUBLE},
      // {L"%+s", -1},
      {L"% d", UTIL_INT},
      {L"% x", UTIL_INT},
      {L"% f", UTIL_DOUBLE},
      // {L"% s", -1},
      {L"%0d", UTIL_INT},
      {L"%0x", UTIL_INT},
      {L"%0f", UTIL_DOUBLE},
      // {L"%0s", -1},
      {L"%#d", UTIL_INT},
      {L"%#x", UTIL_INT},
      {L"%#f", UTIL_DOUBLE},
      // {L"%#s", -1},

      // nWidth should work. for all conversions, can be combined with cFlags=0
      // for numbers.
      {L"%5d", UTIL_INT},
      {L"%05d", UTIL_INT},
      {L"%5x", UTIL_INT},
      {L"%05x", UTIL_INT},
      {L"%5f", UTIL_DOUBLE},
      {L"%05f", UTIL_DOUBLE},
      {L"%5s", UTIL_STRING},
      // {L"%05s", -1},

      // nPrecision should only work for float
      // {L"%.5d", -1},
      // {L"%.5x", -1},
      {L"%.5f", UTIL_DOUBLE},
      // {L"%.5s", -1},
      // {L"%.14d", -1},
      // {L"%.14x", -1},
      {L"%.14f", UTIL_DOUBLE},
      // {L"%.14s", -1},
      // {L"%.f", -1},

      // See https://crbug.com/740166
      // nPrecision too large (> 260) causes crashes in Windows.
      // Avoid this by limiting to two digits
      {L"%.1d", UTIL_INT},
      {L"%.10d", UTIL_INT},
      {L"%.100d", -1},

      // Unexpected characters
      {L"%ad", -1},
      {L"%bx", -1},
      // {L"%cf", -1},
      // {L"%es", -1},
      // {L"%gd", -1},
      {L"%hx", -1},
      // {L"%if", -1},
      {L"%js", -1},
      {L"%@d", -1},
      {L"%~x", -1},
      {L"%[f", -1},
      {L"%\0s", -1},
      {L"%\nd", -1},
      {L"%\rx", -1},
      // {L"%%f", -1},
      // {L"%  s", -1},

      // Combine multiple valid components
      {L"%+6d", UTIL_INT},
      {L"% 7x", UTIL_INT},
      {L"%#9.3f", UTIL_DOUBLE},
      {L"%10s", UTIL_STRING},
  };

  for (size_t i = 0; i < FX_ArraySize(cases); i++) {
    std::wstring input(cases[i].input_string);
    EXPECT_EQ(cases[i].expected, CJS_Util::ParseDataType(&input))
        << cases[i].input_string;
  }
}
