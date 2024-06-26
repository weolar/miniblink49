// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_FX_DATE_HELPERS_H_
#define FXJS_FX_DATE_HELPERS_H_

#include <stddef.h>

#include "core/fxcrt/fx_string.h"

namespace fxjs {

enum class ConversionStatus { kSuccess = 0, kBadFormat, kBadDate };

extern const wchar_t* const kMonths[12];
extern const wchar_t* const kFullMonths[12];

double FX_GetDateTime();
int FX_GetYearFromTime(double dt);
int FX_GetMonthFromTime(double dt);
int FX_GetDayFromTime(double dt);
int FX_GetHourFromTime(double dt);
int FX_GetMinFromTime(double dt);
int FX_GetSecFromTime(double dt);
bool FX_IsValidMonth(int m);
bool FX_IsValidDay(int d);
bool FX_IsValid24Hour(int h);
bool FX_IsValidMinute(int m);
bool FX_IsValidSecond(int s);
double FX_LocalTime(double d);
double FX_MakeDay(int nYear, int nMonth, int nDay);
double FX_MakeTime(int nHour, int nMin, int nSec, int nMs);
double FX_MakeDate(double day, double time);

int FX_ParseStringInteger(const WideString& str,
                          size_t nStart,
                          size_t* pSkip,
                          size_t nMaxStep);

WideString FX_ParseStringString(const WideString& str,
                                size_t nStart,
                                size_t* pSkip);

ConversionStatus FX_ParseDateUsingFormat(const WideString& value,
                                         const WideString& format,
                                         double* result);

}  // namespace fxjs

using fxjs::FX_GetDateTime;
using fxjs::FX_GetYearFromTime;
using fxjs::FX_GetMonthFromTime;
using fxjs::FX_GetDayFromTime;
using fxjs::FX_GetHourFromTime;
using fxjs::FX_GetMinFromTime;
using fxjs::FX_GetSecFromTime;
using fxjs::FX_IsValidMonth;
using fxjs::FX_IsValidDay;
using fxjs::FX_IsValid24Hour;
using fxjs::FX_IsValidMinute;
using fxjs::FX_IsValidSecond;
using fxjs::FX_LocalTime;
using fxjs::FX_MakeDay;
using fxjs::FX_MakeTime;
using fxjs::FX_MakeDate;
using fxjs::FX_ParseStringInteger;
using fxjs::FX_ParseStringString;
using fxjs::FX_ParseDateUsingFormat;

#endif  // FXJS_FX_DATE_HELPERS_H_
