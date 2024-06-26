// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_system.h"

#include <limits>

#include "core/fxcrt/fx_extension.h"

namespace {

template <typename IntType, typename CharType>
IntType FXSYS_StrToInt(const CharType* str) {
  if (!str)
    return 0;

  // Process the sign.
  bool neg = *str == '-';
  if (neg || *str == '+')
    str++;

  IntType num = 0;
  while (*str && FXSYS_IsDecimalDigit(*str)) {
    IntType val = FXSYS_DecimalCharToInt(*str);
    if (num > (std::numeric_limits<IntType>::max() - val) / 10) {
      if (neg && std::numeric_limits<IntType>::is_signed) {
        // Return MIN when the represented number is signed type and is smaller
        // than the min value.
        return std::numeric_limits<IntType>::min();
      } else {
        // Return MAX when the represented number is signed type and is larger
        // than the max value, or the number is unsigned type and out of range.
        return std::numeric_limits<IntType>::max();
      }
    }

    num = num * 10 + val;
    str++;
  }
  // When it is a negative value, -num should be returned. Since num may be of
  // unsigned type, use ~num + 1 to avoid the warning of applying unary minus
  // operator to unsigned type.
  return neg ? ~num + 1 : num;
}

template <typename T, typename UT, typename STR_T>
STR_T FXSYS_IntToStr(T value, STR_T str, int radix) {
  if (radix < 2 || radix > 16) {
    str[0] = 0;
    return str;
  }
  if (value == 0) {
    str[0] = '0';
    str[1] = 0;
    return str;
  }
  int i = 0;
  UT uvalue;
  if (value < 0) {
    str[i++] = '-';
    // Standard trick to avoid undefined behaviour when negating INT_MIN.
    uvalue = static_cast<UT>(-(value + 1)) + 1;
  } else {
    uvalue = value;
  }
  int digits = 1;
  T order = uvalue / radix;
  while (order > 0) {
    digits++;
    order = order / radix;
  }
  for (int d = digits - 1; d > -1; d--) {
    str[d + i] = "0123456789abcdef"[uvalue % radix];
    uvalue /= radix;
  }
  str[digits + i] = 0;
  return str;
}

}  // namespace

int FXSYS_round(float d) {
  if (d < static_cast<float>(std::numeric_limits<int>::min()))
    return std::numeric_limits<int>::min();
  if (d > static_cast<float>(std::numeric_limits<int>::max()))
    return std::numeric_limits<int>::max();
  return static_cast<int>(round(d));
}

int32_t FXSYS_atoi(const char* str) {
  return FXSYS_StrToInt<int32_t, char>(str);
}
uint32_t FXSYS_atoui(const char* str) {
  return FXSYS_StrToInt<uint32_t>(str);
}
int32_t FXSYS_wtoi(const wchar_t* str) {
  return FXSYS_StrToInt<int32_t, wchar_t>(str);
}
int64_t FXSYS_atoi64(const char* str) {
  return FXSYS_StrToInt<int64_t, char>(str);
}
const char* FXSYS_i64toa(int64_t value, char* str, int radix) {
  return FXSYS_IntToStr<int64_t, uint64_t, char*>(value, str, radix);
}

#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

int FXSYS_GetACP() {
  return 0;
}

char* FXSYS_strlwr(char* str) {
  if (!str) {
    return nullptr;
  }
  char* s = str;
  while (*str) {
    *str = tolower(*str);
    str++;
  }
  return s;
}
char* FXSYS_strupr(char* str) {
  if (!str) {
    return nullptr;
  }
  char* s = str;
  while (*str) {
    *str = toupper(*str);
    str++;
  }
  return s;
}
wchar_t* FXSYS_wcslwr(wchar_t* str) {
  if (!str) {
    return nullptr;
  }
  wchar_t* s = str;
  while (*str) {
    *str = FXSYS_towlower(*str);
    str++;
  }
  return s;
}
wchar_t* FXSYS_wcsupr(wchar_t* str) {
  if (!str) {
    return nullptr;
  }
  wchar_t* s = str;
  while (*str) {
    *str = FXSYS_towupper(*str);
    str++;
  }
  return s;
}

int FXSYS_stricmp(const char* dst, const char* src) {
  int f;
  int l;
  do {
    f = toupper(*dst);
    l = toupper(*src);
    ++dst;
    ++src;
  } while (f && f == l);
  return f - l;
}

int FXSYS_wcsicmp(const wchar_t* dst, const wchar_t* src) {
  wchar_t f;
  wchar_t l;
  do {
    f = FXSYS_towupper(*dst);
    l = FXSYS_towupper(*src);
    ++dst;
    ++src;
  } while (f && f == l);
  return f - l;
}

char* FXSYS_itoa(int value, char* str, int radix) {
  return FXSYS_IntToStr<int32_t, uint32_t, char*>(value, str, radix);
}

int FXSYS_WideCharToMultiByte(uint32_t codepage,
                              uint32_t dwFlags,
                              const wchar_t* wstr,
                              int wlen,
                              char* buf,
                              int buflen,
                              const char* default_str,
                              int* pUseDefault) {
  int len = 0;
  for (int i = 0; i < wlen; i++) {
    if (wstr[i] < 0x100) {
      if (buf && len < buflen)
        buf[len] = static_cast<char>(wstr[i]);
      len++;
    }
  }
  return len;
}

int FXSYS_MultiByteToWideChar(uint32_t codepage,
                              uint32_t dwFlags,
                              const char* bstr,
                              int blen,
                              wchar_t* buf,
                              int buflen) {
  int wlen = 0;
  for (int i = 0; i < blen; i++) {
    if (buf && wlen < buflen)
      buf[wlen] = reinterpret_cast<const uint8_t*>(bstr)[i];
    wlen++;
  }
  return wlen;
}

#else  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

size_t FXSYS_wcsftime(wchar_t* strDest,
                      size_t maxsize,
                      const wchar_t* format,
                      const struct tm* timeptr) {
  // Avoid tripping an invalid parameter handler and crashing process.
  // Note: leap seconds may cause tm_sec == 60.
  if (timeptr->tm_year < -1900 || timeptr->tm_year > 8099 ||
      timeptr->tm_mon < 0 || timeptr->tm_mon > 11 || timeptr->tm_mday < 1 ||
      timeptr->tm_mday > 31 || timeptr->tm_hour < 0 || timeptr->tm_hour > 23 ||
      timeptr->tm_min < 0 || timeptr->tm_min > 59 || timeptr->tm_sec < 0 ||
      timeptr->tm_sec > 60 || timeptr->tm_wday < 0 || timeptr->tm_wday > 6 ||
      timeptr->tm_yday < 0 || timeptr->tm_yday > 365) {
    strDest[0] = L'\0';
    return 0;
  }
  return wcsftime(strDest, maxsize, format, timeptr);
}

#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
