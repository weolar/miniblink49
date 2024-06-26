// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_extension.h"

#include <algorithm>
#include <cwctype>
#include <limits>

#include "third_party/base/compiler_specific.h"

namespace {

time_t DefaultTimeFunction() {
  return time(nullptr);
}

struct tm* DefaultLocaltimeFunction(const time_t* tp) {
  return localtime(tp);
}

time_t (*g_time_func)() = DefaultTimeFunction;
struct tm* (*g_localtime_func)(const time_t*) = DefaultLocaltimeFunction;

}  // namespace

float FXSYS_wcstof(const wchar_t* pwsStr, int32_t iLength, int32_t* pUsedLen) {
  ASSERT(pwsStr);

  if (iLength < 0)
    iLength = static_cast<int32_t>(wcslen(pwsStr));
  if (iLength == 0)
    return 0.0f;

  int32_t iUsedLen = 0;
  bool bNegtive = false;
  switch (pwsStr[iUsedLen]) {
    case '-':
      bNegtive = true;
      FALLTHROUGH;
    case '+':
      iUsedLen++;
      break;
  }

  float fValue = 0.0f;
  while (iUsedLen < iLength) {
    wchar_t wch = pwsStr[iUsedLen];
    if (!FXSYS_IsDecimalDigit(wch))
      break;

    fValue = fValue * 10.0f + (wch - L'0');
    iUsedLen++;
  }

  if (iUsedLen < iLength && pwsStr[iUsedLen] == L'.') {
    float fPrecise = 0.1f;
    while (++iUsedLen < iLength) {
      wchar_t wch = pwsStr[iUsedLen];
      if (!FXSYS_IsDecimalDigit(wch))
        break;

      fValue += (wch - L'0') * fPrecise;
      fPrecise *= 0.1f;
    }
  }

  if (iUsedLen < iLength &&
      (pwsStr[iUsedLen] == 'e' || pwsStr[iUsedLen] == 'E')) {
    ++iUsedLen;

    bool negative_exponent = false;
    if (iUsedLen < iLength &&
        (pwsStr[iUsedLen] == '-' || pwsStr[iUsedLen] == '+')) {
      negative_exponent = pwsStr[iUsedLen] == '-';
      ++iUsedLen;
    }

    int32_t exp_value = 0;
    while (iUsedLen < iLength) {
      wchar_t wch = pwsStr[iUsedLen];
      if (!FXSYS_IsDecimalDigit(wch))
        break;

      exp_value = exp_value * 10.0f + (wch - L'0');
      // Exponent is outside the valid range, fail.
      if ((negative_exponent &&
           -exp_value < std::numeric_limits<float>::min_exponent10) ||
          (!negative_exponent &&
           exp_value > std::numeric_limits<float>::max_exponent10)) {
        if (pUsedLen)
          *pUsedLen = 0;
        return 0.0f;
      }

      ++iUsedLen;
    }

    for (size_t i = exp_value; i > 0; --i) {
      if (exp_value > 0) {
        if (negative_exponent)
          fValue /= 10;
        else
          fValue *= 10;
      }
    }
  }

  if (pUsedLen)
    *pUsedLen = iUsedLen;

  return bNegtive ? -fValue : fValue;
}

wchar_t* FXSYS_wcsncpy(wchar_t* dstStr, const wchar_t* srcStr, size_t count) {
  ASSERT(dstStr);
  ASSERT(srcStr);
  ASSERT(count > 0);

  for (size_t i = 0; i < count; ++i)
    if ((dstStr[i] = srcStr[i]) == L'\0')
      break;
  return dstStr;
}

int32_t FXSYS_wcsnicmp(const wchar_t* s1, const wchar_t* s2, size_t count) {
  ASSERT(s1);
  ASSERT(s2);
  ASSERT(count > 0);

  wchar_t wch1 = 0, wch2 = 0;
  while (count-- > 0) {
    wch1 = static_cast<wchar_t>(FXSYS_towlower(*s1++));
    wch2 = static_cast<wchar_t>(FXSYS_towlower(*s2++));
    if (wch1 != wch2)
      break;
  }
  return wch1 - wch2;
}

void FXSYS_IntToTwoHexChars(uint8_t n, char* buf) {
  static const char kHex[] = "0123456789ABCDEF";
  buf[0] = kHex[n / 16];
  buf[1] = kHex[n % 16];
}

void FXSYS_IntToFourHexChars(uint16_t n, char* buf) {
  FXSYS_IntToTwoHexChars(n / 256, buf);
  FXSYS_IntToTwoHexChars(n % 256, buf + 2);
}

size_t FXSYS_ToUTF16BE(uint32_t unicode, char* buf) {
  ASSERT(unicode <= 0xD7FF || (unicode > 0xDFFF && unicode <= 0x10FFFF));
  if (unicode <= 0xFFFF) {
    FXSYS_IntToFourHexChars(unicode, buf);
    return 4;
  }
  unicode -= 0x010000;
  // High ten bits plus 0xD800
  FXSYS_IntToFourHexChars(0xD800 + unicode / 0x400, buf);
  // Low ten bits plus 0xDC00
  FXSYS_IntToFourHexChars(0xDC00 + unicode % 0x400, buf + 4);
  return 8;
}

void FXSYS_SetTimeFunction(time_t (*func)()) {
  g_time_func = func ? func : DefaultTimeFunction;
}

void FXSYS_SetLocaltimeFunction(struct tm* (*func)(const time_t*)) {
  g_localtime_func = func ? func : DefaultLocaltimeFunction;
}

time_t FXSYS_time(time_t* tloc) {
  time_t ret_val = g_time_func();
  if (tloc)
    *tloc = ret_val;
  return ret_val;
}

struct tm* FXSYS_localtime(const time_t* tp) {
  return g_localtime_func(tp);
}
