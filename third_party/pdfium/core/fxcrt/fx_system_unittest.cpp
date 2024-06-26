// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "testing/gtest/include/gtest/gtest.h"

// Unit test covering cases where PDFium replaces well-known library
// functionality on any given platformn.

#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

namespace {

const char kSentinel = 0x7f;

void Check32BitBase16Itoa(int32_t input, const char* expected_output) {
  const size_t kBufLen = 11;  // "-" + 8 digits + NUL + sentinel.
  char buf[kBufLen];
  buf[kBufLen - 1] = kSentinel;
  FXSYS_itoa(input, buf, 16);
  EXPECT_STREQ(expected_output, buf);
  EXPECT_EQ(kSentinel, buf[kBufLen - 1]);
}

void Check32BitBase10Itoa(int32_t input, const char* expected_output) {
  const size_t kBufLen = 13;  // "-" + 10 digits + NUL + sentinel.
  char buf[kBufLen];
  buf[kBufLen - 1] = kSentinel;
  FXSYS_itoa(input, buf, 10);
  EXPECT_STREQ(expected_output, buf);
  EXPECT_EQ(kSentinel, buf[kBufLen - 1]);
}

void Check32BitBase2Itoa(int32_t input, const char* expected_output) {
  const size_t kBufLen = 35;  // "-" + 32 digits + NUL + sentinel.
  char buf[kBufLen];
  buf[kBufLen - 1] = kSentinel;
  FXSYS_itoa(input, buf, 2);
  EXPECT_STREQ(expected_output, buf);
  EXPECT_EQ(kSentinel, buf[kBufLen - 1]);
}

void Check64BitBase16Itoa(int64_t input, const char* expected_output) {
  const size_t kBufLen = 19;  // "-" + 16 digits + NUL + sentinel.
  char buf[kBufLen];
  buf[kBufLen - 1] = kSentinel;
  FXSYS_i64toa(input, buf, 16);
  EXPECT_STREQ(expected_output, buf);
  EXPECT_EQ(kSentinel, buf[kBufLen - 1]);
}

void Check64BitBase10Itoa(int64_t input, const char* expected_output) {
  const size_t kBufLen = 22;  // "-" + 19 digits + NUL + sentinel.
  char buf[kBufLen];
  buf[kBufLen - 1] = kSentinel;
  FXSYS_i64toa(input, buf, 10);
  EXPECT_STREQ(expected_output, buf);
  EXPECT_EQ(kSentinel, buf[kBufLen - 1]);
}

void Check64BitBase2Itoa(int64_t input, const char* expected_output) {
  const size_t kBufLen = 67;  // "-" + 64 digits + NUL + sentinel.
  char buf[kBufLen];
  buf[kBufLen - 1] = kSentinel;
  FXSYS_i64toa(input, buf, 2);
  EXPECT_STREQ(expected_output, buf);
  EXPECT_EQ(kSentinel, buf[kBufLen - 1]);
}

}  // namespace

TEST(fxcrt, FXSYS_itoa_InvalidRadix) {
  char buf[32];

  FXSYS_itoa(42, buf, 17);  // Ours stops at 16.
  EXPECT_STREQ("", buf);

  FXSYS_itoa(42, buf, 1);
  EXPECT_STREQ("", buf);

  FXSYS_itoa(42, buf, 0);
  EXPECT_STREQ("", buf);

  FXSYS_itoa(42, buf, -1);
  EXPECT_STREQ("", buf);
}

TEST(fxcrt, FXSYS_itoa) {
  Check32BitBase16Itoa(std::numeric_limits<int32_t>::min(), "-80000000");
  Check32BitBase10Itoa(std::numeric_limits<int32_t>::min(), "-2147483648");
  Check32BitBase2Itoa(std::numeric_limits<int32_t>::min(),
                      "-10000000000000000000000000000000");

  Check32BitBase16Itoa(-1, "-1");
  Check32BitBase10Itoa(-1, "-1");
  Check32BitBase2Itoa(-1, "-1");

  Check32BitBase16Itoa(0, "0");
  Check32BitBase10Itoa(0, "0");
  Check32BitBase2Itoa(0, "0");

  Check32BitBase16Itoa(42, "2a");
  Check32BitBase10Itoa(42, "42");
  Check32BitBase2Itoa(42, "101010");

  Check32BitBase16Itoa(std::numeric_limits<int32_t>::max(), "7fffffff");
  Check32BitBase10Itoa(std::numeric_limits<int32_t>::max(), "2147483647");
  Check32BitBase2Itoa(std::numeric_limits<int32_t>::max(),
                      "1111111111111111111111111111111");
}

TEST(fxcrt, FXSYS_i64toa_InvalidRadix) {
  char buf[32];

  FXSYS_i64toa(42, buf, 17);  // Ours stops at 16.
  EXPECT_STREQ("", buf);

  FXSYS_i64toa(42, buf, 1);
  EXPECT_STREQ("", buf);

  FXSYS_i64toa(42, buf, 0);
  EXPECT_STREQ("", buf);

  FXSYS_i64toa(42, buf, -1);
  EXPECT_STREQ("", buf);
}

TEST(fxcrt, FXSYS_i64toa) {
  Check64BitBase16Itoa(std::numeric_limits<int64_t>::min(),
                       "-8000000000000000");
  Check64BitBase10Itoa(std::numeric_limits<int64_t>::min(),
                       "-9223372036854775808");
  Check64BitBase2Itoa(
      std::numeric_limits<int64_t>::min(),
      "-1000000000000000000000000000000000000000000000000000000000000000");

  Check64BitBase16Itoa(-1, "-1");
  Check64BitBase10Itoa(-1, "-1");
  Check64BitBase2Itoa(-1, "-1");

  Check64BitBase16Itoa(0, "0");
  Check64BitBase10Itoa(0, "0");
  Check64BitBase2Itoa(0, "0");

  Check64BitBase16Itoa(42, "2a");
  Check64BitBase10Itoa(42, "42");
  Check64BitBase2Itoa(42, "101010");

  Check64BitBase16Itoa(std::numeric_limits<int64_t>::max(), "7fffffffffffffff");
  Check64BitBase10Itoa(std::numeric_limits<int64_t>::max(),
                       "9223372036854775807");
  Check64BitBase2Itoa(
      std::numeric_limits<int64_t>::max(),
      "111111111111111111111111111111111111111111111111111111111111111");
}

#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

TEST(fxcrt, FXSYS_wcsftime) {
  struct tm good_time = {};
  good_time.tm_year = 74;  // 1900-based.
  good_time.tm_mon = 7;    // 0-based.
  good_time.tm_mday = 9;   // 1-based.
  good_time.tm_hour = 11;
  good_time.tm_min = 59;
  good_time.tm_sec = 59;

  wchar_t buf[100] = {};
  EXPECT_EQ(19u, FXSYS_wcsftime(buf, FX_ArraySize(buf), L"%Y-%m-%dT%H:%M:%S",
                                &good_time));
  EXPECT_STREQ(L"1974-08-09T11:59:59", buf);

  // Ensure wcsftime handles a wide range of years without crashing.
  struct tm year_time = {};
  year_time.tm_mon = 7;   // 0-based.
  year_time.tm_mday = 9;  // 1-based.
  year_time.tm_hour = 11;
  year_time.tm_min = 59;
  year_time.tm_sec = 59;

  for (int year = -2500; year <= 8500; ++year) {
    year_time.tm_year = year;
    wchar_t year_buf[100] = {};
    FXSYS_wcsftime(year_buf, FX_ArraySize(year_buf), L"%Y-%m-%dT%H:%M:%S",
                   &year_time);
  }

  // Ensure wcsftime handles bad years, etc. without crashing.
  struct tm bad_time = {};
  bad_time.tm_year = -1;
  bad_time.tm_mon = -1;
  bad_time.tm_mday = -1;
  bad_time.tm_hour = -1;
  bad_time.tm_min = -1;
  bad_time.tm_sec = -1;

  FXSYS_wcsftime(buf, FX_ArraySize(buf), L"%y-%m-%dT%H:%M:%S", &bad_time);

  // Ensure wcsftime handles bad-ish day without crashing (Feb 30).
  struct tm feb_time = {};
  feb_time.tm_year = 115;  // 1900-based.
  feb_time.tm_mon = 1;     // 0-based.
  feb_time.tm_mday = 30;   // 1-based.
  feb_time.tm_hour = 12;
  feb_time.tm_min = 00;
  feb_time.tm_sec = 00;

  FXSYS_wcsftime(buf, FX_ArraySize(buf), L"%y-%m-%dT%H:%M:%S", &feb_time);
}

TEST(fxcrt, FXSYS_atoi) {
  EXPECT_EQ(0, FXSYS_atoi(""));
  EXPECT_EQ(0, FXSYS_atoi("0"));
  EXPECT_EQ(-1, FXSYS_atoi("-1"));
  EXPECT_EQ(2345, FXSYS_atoi("2345"));
  EXPECT_EQ(-2147483647, FXSYS_atoi("-2147483647"));
  // Handle the sign.
  EXPECT_EQ(-2345, FXSYS_atoi("-2345"));
  EXPECT_EQ(2345, FXSYS_atoi("+2345"));
  // The max value.
  EXPECT_EQ(2147483647, FXSYS_atoi("2147483647"));
  // The min value. Written in -1 format to avoid "unary minus operator applied
  // to unsigned type" warning.
  EXPECT_EQ(-2147483647 - 1, FXSYS_atoi("-2147483648"));
  // With invalid char.
  EXPECT_EQ(9, FXSYS_atoi("9x9"));

  // Out of range values.
  EXPECT_EQ(2147483647, FXSYS_atoi("2147483623423412348"));
  EXPECT_EQ(2147483647, FXSYS_atoi("2147483648"));
  EXPECT_EQ(-2147483647 - 1, FXSYS_atoi("-2147483650"));
}

TEST(fxcrt, FXSYS_atoi64) {
  EXPECT_EQ(0, FXSYS_atoi64(""));
  EXPECT_EQ(0, FXSYS_atoi64("0"));
  EXPECT_EQ(-1, FXSYS_atoi64("-1"));
  EXPECT_EQ(2345, FXSYS_atoi64("2345"));
  EXPECT_EQ(-9223372036854775807LL, FXSYS_atoi64("-9223372036854775807"));
  // Handle the sign.
  EXPECT_EQ(-2345, FXSYS_atoi64("-2345"));
  EXPECT_EQ(2345, FXSYS_atoi64("+2345"));
  // The max value.
  EXPECT_EQ(9223372036854775807LL, FXSYS_atoi64("9223372036854775807"));
  // The min value. Written in -1 format to avoid implicit unsigned warning.
  EXPECT_EQ(-9223372036854775807LL - 1LL, FXSYS_atoi64("-9223372036854775808"));
  // With invalid char.
  EXPECT_EQ(9, FXSYS_atoi64("9x9"));

  // Out of range values.
  EXPECT_EQ(9223372036854775807LL, FXSYS_atoi64("922337203685471234123475807"));
  EXPECT_EQ(9223372036854775807LL, FXSYS_atoi64("9223372036854775808"));
  EXPECT_EQ(-9223372036854775807LL - 1LL, FXSYS_atoi64("-9223372036854775810"));
}

TEST(fxcrt, FXSYS_wtoi) {
  EXPECT_EQ(0, FXSYS_wtoi(L""));
  EXPECT_EQ(0, FXSYS_wtoi(L"0"));
  EXPECT_EQ(-1, FXSYS_wtoi(L"-1"));
  EXPECT_EQ(2345, FXSYS_wtoi(L"2345"));
  EXPECT_EQ(-2147483647, FXSYS_wtoi(L"-2147483647"));
  // The max value.
  EXPECT_EQ(2147483647, FXSYS_wtoi(L"2147483647"));
  // The min value.
  EXPECT_EQ(-2147483647 - 1, FXSYS_wtoi(L"-2147483648"));

  // Out of range values.
  EXPECT_EQ(2147483647, FXSYS_wtoi(L"2147483623423412348"));
  EXPECT_EQ(2147483647, FXSYS_wtoi(L"2147483648"));
  EXPECT_EQ(-2147483647 - 1, FXSYS_wtoi(L"-2147483652"));
}

TEST(fxcrt, FXSYS_atoui) {
  EXPECT_EQ(0u, FXSYS_atoui(""));
  EXPECT_EQ(0u, FXSYS_atoui("0"));
  EXPECT_EQ(4294967295, FXSYS_atoui("-1"));
  EXPECT_EQ(2345u, FXSYS_atoui("2345"));
  // Handle the sign.
  EXPECT_EQ(4294964951, FXSYS_atoui("-2345"));
  EXPECT_EQ(2345u, FXSYS_atoui("+2345"));
  // The max value.
  EXPECT_EQ(4294967295, FXSYS_atoui("4294967295"));
  EXPECT_EQ(9u, FXSYS_atoui("9x9"));

  // Out of range values.
  EXPECT_EQ(4294967295, FXSYS_atoui("2147483623423412348"));
  EXPECT_EQ(4294967295, FXSYS_atoui("4294967296"));
  EXPECT_EQ(4294967295, FXSYS_atoui("-4294967345"));
}
