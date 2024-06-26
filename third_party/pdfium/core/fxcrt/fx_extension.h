// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_FX_EXTENSION_H_
#define CORE_FXCRT_FX_EXTENSION_H_

#include <time.h>

#include <cctype>
#include <cmath>
#include <cwctype>
#include <memory>

#include "core/fxcrt/fx_string.h"

// #if defined(USE_SYSTEM_ICUUC)
// #include <unicode/uchar.h>
// #else
// #include "third_party/icu/source/common/unicode/uchar.h"
// #endif

inline bool u_islower(int32_t c) {
  DebugBreak();
  return false;
}

inline bool u_isupper(int32_t c) {
  DebugBreak();
  return false;
}

inline int32_t u_tolower(wchar_t c) {
  DebugBreak();
  return 0;
}

inline int32_t u_toupper(wchar_t c) {
  DebugBreak();
  return 0;
}

inline bool u_isalpha(wchar_t c) {
  DebugBreak();
  return false;
}

inline bool u_isalnum(wchar_t c) {
  DebugBreak();
  return false;
}

inline bool u_isspace(wchar_t c) {
  DebugBreak();
  return false;
}

#define FX_INVALID_OFFSET static_cast<uint32_t>(-1)

#if 1 // def PDF_ENABLE_XFA // weolar
#define FX_IsOdd(a) ((a)&1)
#endif  // PDF_ENABLE_XFA

float FXSYS_wcstof(const wchar_t* pwsStr, int32_t iLength, int32_t* pUsedLen);
wchar_t* FXSYS_wcsncpy(wchar_t* dstStr, const wchar_t* srcStr, size_t count);
int32_t FXSYS_wcsnicmp(const wchar_t* s1, const wchar_t* s2, size_t count);

inline bool FXSYS_iswlower(int32_t c) {
  return u_islower(c);
}

inline bool FXSYS_iswupper(int32_t c) {
  return u_isupper(c);
}

inline int32_t FXSYS_towlower(wchar_t c) {
  return u_tolower(c);
}

inline int32_t FXSYS_towupper(wchar_t c) {
  return u_toupper(c);
}

inline char FXSYS_ToUpperASCII(char c) {
  return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
}

inline bool FXSYS_iswalpha(wchar_t c) {
  return u_isalpha(c);
}

inline bool FXSYS_iswalnum(wchar_t c) {
  return u_isalnum(c);
}

inline bool FXSYS_iswspace(wchar_t c) {
  return u_isspace(c);
}

inline bool FXSYS_IsOctalDigit(char c) {
  return c >= '0' && c <= '7';
}

inline bool FXSYS_IsHexDigit(char c) {
  return !((c & 0x80) || !std::isxdigit(c));
}

inline bool FXSYS_IsWideHexDigit(wchar_t c) {
  return !((c & 0xFFFFFF80) || !std::isxdigit(c));
}

inline int FXSYS_HexCharToInt(char c) {
  if (!FXSYS_IsHexDigit(c))
    return 0;
  char upchar = FXSYS_ToUpperASCII(c);
  return upchar > '9' ? upchar - 'A' + 10 : upchar - '0';
}

inline int FXSYS_WideHexCharToInt(wchar_t c) {
  if (!FXSYS_IsWideHexDigit(c))
    return 0;
  char upchar = std::toupper(static_cast<char>(c));
  return upchar > '9' ? upchar - 'A' + 10 : upchar - '0';
}

inline bool FXSYS_IsDecimalDigit(char c) {
  return !((c & 0x80) || !std::isdigit(c));
}

inline bool FXSYS_IsDecimalDigit(wchar_t c) {
  return !((c & 0xFFFFFF80) || !std::iswdigit(c));
}

inline int FXSYS_DecimalCharToInt(char c) {
  return FXSYS_IsDecimalDigit(c) ? c - '0' : 0;
}

inline int FXSYS_DecimalCharToInt(wchar_t c) {
  return FXSYS_IsDecimalDigit(c) ? c - L'0' : 0;
}

void FXSYS_IntToTwoHexChars(uint8_t c, char* buf);
void FXSYS_IntToFourHexChars(uint16_t c, char* buf);

size_t FXSYS_ToUTF16BE(uint32_t unicode, char* buf);

// Strict order over floating types where NaNs may be present.
template <typename T>
bool FXSYS_SafeEQ(const T& lhs, const T& rhs) {
  return (std::isnan(lhs) && std::isnan(rhs)) ||
         (!std::isnan(lhs) && !std::isnan(rhs) && lhs == rhs);
}

template <typename T>
bool FXSYS_SafeLT(const T& lhs, const T& rhs) {
  if (std::isnan(lhs) && std::isnan(rhs))
    return false;
  if (std::isnan(lhs) || std::isnan(rhs))
    return std::isnan(lhs) < std::isnan(rhs);
  return lhs < rhs;
}

// Override time/localtime functions for test consistency.
void FXSYS_SetTimeFunction(time_t (*func)());
void FXSYS_SetLocaltimeFunction(struct tm* (*func)(const time_t*));

// Replacements for time/localtime that respect overrides.
time_t FXSYS_time(time_t* tloc);
struct tm* FXSYS_localtime(const time_t* tp);

#endif  // CORE_FXCRT_FX_EXTENSION_H_
