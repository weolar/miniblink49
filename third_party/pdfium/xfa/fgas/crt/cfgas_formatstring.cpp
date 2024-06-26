// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/crt/cfgas_formatstring.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_extension.h"
#include "xfa/fgas/crt/cfgas_decimal.h"
#include "xfa/fxfa/parser/cxfa_localemgr.h"

#define FX_LOCALECATEGORY_DateHash 0xbde9abde
#define FX_LOCALECATEGORY_TimeHash 0x2d71b00f
#define FX_LOCALECATEGORY_DateTimeHash 0x158c72ed
#define FX_LOCALECATEGORY_NumHash 0x0b4ff870
#define FX_LOCALECATEGORY_TextHash 0x2d08af85
#define FX_LOCALECATEGORY_ZeroHash 0x568cb500
#define FX_LOCALECATEGORY_NullHash 0x052931bb

#define FX_NUMSTYLE_Percent 0x01
#define FX_NUMSTYLE_Exponent 0x02
#define FX_NUMSTYLE_DotVorv 0x04

namespace {

struct LocaleDateTimeSubCategoryWithHash {
  uint32_t uHash;  // Hashed as wide string.
  FX_LOCALEDATETIMESUBCATEGORY eSubCategory;
};

struct LocaleNumberSubCategoryWithHash {
  uint32_t uHash;  // Hashed as wide string.
  FX_LOCALENUMSUBCATEGORY eSubCategory;
};

#undef SUBC
#define SUBC(a, b, c) a, c

const LocaleDateTimeSubCategoryWithHash g_FXLocaleDateTimeSubCatData[] = {
    {SUBC(0x14da2125, "default", FX_LOCALEDATETIMESUBCATEGORY_Default)},
    {SUBC(0x9041d4b0, "short", FX_LOCALEDATETIMESUBCATEGORY_Short)},
    {SUBC(0xa084a381, "medium", FX_LOCALEDATETIMESUBCATEGORY_Medium)},
    {SUBC(0xcdce56b3, "full", FX_LOCALEDATETIMESUBCATEGORY_Full)},
    {SUBC(0xf6b4afb0, "long", FX_LOCALEDATETIMESUBCATEGORY_Long)},
};

const LocaleNumberSubCategoryWithHash g_FXLocaleNumSubCatData[] = {
    {SUBC(0x46f95531, "percent", FX_LOCALENUMPATTERN_Percent)},
    {SUBC(0x4c4e8acb, "currency", FX_LOCALENUMPATTERN_Currency)},
    {SUBC(0x54034c2f, "decimal", FX_LOCALENUMPATTERN_Decimal)},
    {SUBC(0x7568e6ae, "integer", FX_LOCALENUMPATTERN_Integer)},
};

#undef SUBC

struct FX_LOCALETIMEZONEINFO {
  const wchar_t* name;
  int16_t iHour;
  int16_t iMinute;
};

const FX_LOCALETIMEZONEINFO g_FXLocaleTimeZoneData[] = {
    {L"CDT", -5, 0}, {L"CST", -6, 0}, {L"EDT", -4, 0}, {L"EST", -5, 0},
    {L"MDT", -6, 0}, {L"MST", -7, 0}, {L"PDT", -7, 0}, {L"PST", -8, 0},
};

const wchar_t gs_wsTimeSymbols[] = L"hHkKMSFAzZ";
const wchar_t gs_wsDateSymbols[] = L"DJMEeGgYwW";
const wchar_t gs_wsConstChars[] = L",-:/. ";

int32_t ParseTimeZone(const wchar_t* pStr, int32_t iLen, FX_TIMEZONE* tz) {
  tz->tzHour = 0;
  tz->tzMinute = 0;
  if (iLen < 0)
    return 0;

  int32_t iStart = 1;
  int32_t iEnd = iStart + 2;
  while (iStart < iLen && iStart < iEnd)
    tz->tzHour = tz->tzHour * 10 + FXSYS_DecimalCharToInt(pStr[iStart++]);

  if (iStart < iLen && pStr[iStart] == ':')
    iStart++;

  iEnd = iStart + 2;
  while (iStart < iLen && iStart < iEnd)
    tz->tzMinute = tz->tzMinute * 10 + FXSYS_DecimalCharToInt(pStr[iStart++]);

  if (pStr[0] == '-')
    tz->tzHour = -tz->tzHour;

  return iStart;
}

int32_t ConvertHex(int32_t iKeyValue, wchar_t ch) {
  if (FXSYS_IsHexDigit(ch))
    return iKeyValue * 16 + FXSYS_HexCharToInt(ch);
  return iKeyValue;
}

WideString GetLiteralText(const wchar_t* pStrPattern,
                          int32_t* iPattern,
                          int32_t iLenPattern) {
  WideString wsOutput;
  if (pStrPattern[*iPattern] != '\'')
    return wsOutput;

  (*iPattern)++;
  int32_t iQuote = 1;
  while (*iPattern < iLenPattern) {
    if (pStrPattern[*iPattern] == '\'') {
      iQuote++;
      if ((*iPattern + 1 >= iLenPattern) ||
          ((pStrPattern[*iPattern + 1] != '\'') && (iQuote % 2 == 0))) {
        break;
      }
      iQuote++;
      (*iPattern)++;
    } else if (pStrPattern[*iPattern] == '\\' &&
               (*iPattern + 1 < iLenPattern) &&
               pStrPattern[*iPattern + 1] == 'u') {
      int32_t iKeyValue = 0;
      *iPattern += 2;
      int32_t i = 0;
      while (*iPattern < iLenPattern && i++ < 4) {
        wchar_t ch = pStrPattern[(*iPattern)++];
        iKeyValue = ConvertHex(iKeyValue, ch);
      }
      if (iKeyValue != 0)
        wsOutput += static_cast<wchar_t>(iKeyValue & 0x0000FFFF);

      continue;
    }
    wsOutput += pStrPattern[(*iPattern)++];
  }
  return wsOutput;
}

WideString GetLiteralTextReverse(const wchar_t* pStrPattern,
                                 int32_t* iPattern) {
  WideString wsOutput;
  if (pStrPattern[*iPattern] != '\'')
    return wsOutput;

  (*iPattern)--;
  int32_t iQuote = 1;
  while (*iPattern >= 0) {
    if (pStrPattern[*iPattern] == '\'') {
      iQuote++;
      if (*iPattern - 1 >= 0 ||
          ((pStrPattern[*iPattern - 1] != '\'') && (iQuote % 2 == 0))) {
        break;
      }
      iQuote++;
      (*iPattern)--;
    } else if (pStrPattern[*iPattern] == '\\' &&
               pStrPattern[*iPattern + 1] == 'u') {
      (*iPattern)--;
      int32_t iKeyValue = 0;
      int32_t iLen = wsOutput.GetLength();
      int32_t i = 1;
      for (; i < iLen && i < 5; i++) {
        wchar_t ch = wsOutput[i];
        iKeyValue = ConvertHex(iKeyValue, ch);
      }
      if (iKeyValue != 0) {
        wsOutput.Delete(0, i);
        wsOutput = (wchar_t)(iKeyValue & 0x0000FFFF) + wsOutput;
      }
      continue;
    }
    wsOutput = pStrPattern[(*iPattern)--] + wsOutput;
  }
  return wsOutput;
}

bool GetNumericDotIndex(const WideString& wsNum,
                        const WideString& wsDotSymbol,
                        int32_t* iDotIndex) {
  int32_t ccf = 0;
  int32_t iLenf = wsNum.GetLength();
  const wchar_t* pStr = wsNum.c_str();
  int32_t iLenDot = wsDotSymbol.GetLength();
  while (ccf < iLenf) {
    if (pStr[ccf] == '\'') {
      GetLiteralText(pStr, &ccf, iLenf);
    } else if (ccf + iLenDot <= iLenf &&
               !wcsncmp(pStr + ccf, wsDotSymbol.c_str(), iLenDot)) {
      *iDotIndex = ccf;
      return true;
    }
    ccf++;
  }
  auto result = wsNum.Find('.');
  *iDotIndex = result.value_or(iLenf);
  return result.has_value();
}

bool ExtractCountDigits(const wchar_t* str,
                        int len,
                        int count,
                        int* cc,
                        uint32_t* value) {
  for (int i = count; i > 0; --i) {
    if (*cc >= len)
      return false;
    if (!FXSYS_IsDecimalDigit(str[*cc]))
      return false;
    *value = *value * 10 + FXSYS_DecimalCharToInt(str[(*cc)++]);
  }
  return true;
}

bool ExtractCountDigitsWithOptional(const wchar_t* str,
                                    int len,
                                    int count,
                                    int* cc,
                                    uint32_t* value) {
  if (!ExtractCountDigits(str, len, count, cc, value))
    return false;
  ExtractCountDigits(str, len, 1, cc, value);
  return true;
}

bool ParseLocaleDate(const WideString& wsDate,
                     const WideString& wsDatePattern,
                     LocaleIface* pLocale,
                     CFX_DateTime* datetime,
                     int32_t* cc) {
  uint32_t year = 1900;
  uint32_t month = 1;
  uint32_t day = 1;
  int32_t ccf = 0;
  const wchar_t* str = wsDate.c_str();
  int32_t len = wsDate.GetLength();
  const wchar_t* strf = wsDatePattern.c_str();
  int32_t lenf = wsDatePattern.GetLength();
  WideStringView wsDateSymbols(gs_wsDateSymbols);
  while (*cc < len && ccf < lenf) {
    if (strf[ccf] == '\'') {
      WideString wsLiteral = GetLiteralText(strf, &ccf, lenf);
      int32_t iLiteralLen = wsLiteral.GetLength();
      if (*cc + iLiteralLen > len ||
          wcsncmp(str + *cc, wsLiteral.c_str(), iLiteralLen)) {
        return false;
      }
      *cc += iLiteralLen;
      ccf++;
      continue;
    }
    if (!wsDateSymbols.Contains(strf[ccf])) {
      if (strf[ccf] != str[*cc])
        return false;
      (*cc)++;
      ccf++;
      continue;
    }

    WideString symbol;
    symbol.Reserve(4);
    symbol += strf[ccf++];
    while (ccf < lenf && strf[ccf] == symbol[0])
      symbol += strf[ccf++];

    if (symbol.EqualsASCII("D") || symbol.EqualsASCII("DD")) {
      day = 0;
      if (!ExtractCountDigitsWithOptional(str, len, 1, cc, &day))
        return false;
    } else if (symbol.EqualsASCII("J")) {
      uint32_t val = 0;
      ExtractCountDigits(str, len, 3, cc, &val);
    } else if (symbol.EqualsASCII("M") || symbol.EqualsASCII("MM")) {
      month = 0;
      if (!ExtractCountDigitsWithOptional(str, len, 1, cc, &month))
        return false;
    } else if (symbol.EqualsASCII("MMM") || symbol.EqualsASCII("MMMM")) {
      for (uint16_t i = 0; i < 12; i++) {
        WideString wsMonthName =
            pLocale->GetMonthName(i, symbol.EqualsASCII("MMM"));
        if (wsMonthName.IsEmpty())
          continue;
        if (!wcsncmp(wsMonthName.c_str(), str + *cc, wsMonthName.GetLength())) {
          *cc += wsMonthName.GetLength();
          month = i + 1;
          break;
        }
      }
    } else if (symbol.EqualsASCII("EEE") || symbol.EqualsASCII("EEEE")) {
      for (uint16_t i = 0; i < 7; i++) {
        WideString wsDayName =
            pLocale->GetDayName(i, symbol.EqualsASCII("EEE"));
        if (wsDayName.IsEmpty())
          continue;
        if (!wcsncmp(wsDayName.c_str(), str + *cc, wsDayName.GetLength())) {
          *cc += wsDayName.GetLength();
          break;
        }
      }
    } else if (symbol.EqualsASCII("YY") || symbol.EqualsASCII("YYYY")) {
      if (*cc + pdfium::base::checked_cast<int32_t>(symbol.GetLength()) > len)
        return false;

      year = 0;
      if (!ExtractCountDigits(str, len, symbol.GetLength(), cc, &year))
        return false;
      if (symbol.EqualsASCII("YY")) {
        if (year <= 29)
          year += 2000;
        else
          year += 1900;
      }
    } else if (symbol.EqualsASCII("G")) {
      *cc += 2;
    } else if (symbol.EqualsASCII("JJJ") || symbol.EqualsASCIINoCase("E") ||
               symbol.EqualsASCII("w") || symbol.EqualsASCII("WW")) {
      *cc += symbol.GetLength();
    }
  }
  if (*cc < len)
    return false;

  datetime->SetDate(year, month, day);
  return !!(*cc);
}

void ResolveZone(FX_TIMEZONE tzDiff,
                 LocaleIface* pLocale,
                 uint32_t* wHour,
                 uint32_t* wMinute) {
  int32_t iMinuteDiff = *wHour * 60 + *wMinute;
  FX_TIMEZONE tzLocale = pLocale->GetTimeZone();
  iMinuteDiff += tzLocale.tzHour * 60 +
                 (tzLocale.tzHour < 0 ? -tzLocale.tzMinute : tzLocale.tzMinute);
  iMinuteDiff -= tzDiff.tzHour * 60 +
                 (tzDiff.tzHour < 0 ? -tzDiff.tzMinute : tzDiff.tzMinute);

  iMinuteDiff %= 1440;
  if (iMinuteDiff < 0)
    iMinuteDiff += 1440;

  *wHour = iMinuteDiff / 60;
  *wMinute = iMinuteDiff % 60;
}

bool ParseLocaleTime(const WideString& wsTime,
                     const WideString& wsTimePattern,
                     LocaleIface* pLocale,
                     CFX_DateTime* datetime,
                     int32_t* cc) {
  uint32_t hour = 0;
  uint32_t minute = 0;
  uint32_t second = 0;
  uint32_t millisecond = 0;
  int32_t ccf = 0;
  const wchar_t* str = wsTime.c_str();
  int len = wsTime.GetLength();
  const wchar_t* strf = wsTimePattern.c_str();
  int lenf = wsTimePattern.GetLength();
  bool bHasA = false;
  bool bPM = false;
  WideStringView wsTimeSymbols(gs_wsTimeSymbols);
  while (*cc < len && ccf < lenf) {
    if (strf[ccf] == '\'') {
      WideString wsLiteral = GetLiteralText(strf, &ccf, lenf);
      int32_t iLiteralLen = wsLiteral.GetLength();
      if (*cc + iLiteralLen > len ||
          wcsncmp(str + *cc, wsLiteral.c_str(), iLiteralLen)) {
        return false;
      }
      *cc += iLiteralLen;
      ccf++;
      continue;
    }
    if (!wsTimeSymbols.Contains(strf[ccf])) {
      if (strf[ccf] != str[*cc])
        return false;
      (*cc)++;
      ccf++;
      continue;
    }

    WideString symbol;
    symbol.Reserve(4);
    symbol += strf[ccf++];
    while (ccf < lenf && strf[ccf] == symbol[0])
      symbol += strf[ccf++];

    if (symbol.EqualsASCIINoCase("k") || symbol.EqualsASCIINoCase("h")) {
      hour = 0;
      if (!ExtractCountDigitsWithOptional(str, len, 1, cc, &hour))
        return false;
      if (symbol.EqualsASCII("K") && hour == 24)
        hour = 0;
    } else if (symbol.EqualsASCIINoCase("kk") ||
               symbol.EqualsASCIINoCase("hh")) {
      hour = 0;
      if (!ExtractCountDigits(str, len, 2, cc, &hour))
        return false;
      if (symbol.EqualsASCII("KK") && hour == 24)
        hour = 0;
    } else if (symbol.EqualsASCII("M")) {
      minute = 0;
      if (!ExtractCountDigitsWithOptional(str, len, 1, cc, &minute))
        return false;
    } else if (symbol.EqualsASCII("MM")) {
      minute = 0;
      if (!ExtractCountDigits(str, len, 2, cc, &minute))
        return false;
    } else if (symbol.EqualsASCII("S")) {
      second = 0;
      if (!ExtractCountDigitsWithOptional(str, len, 1, cc, &second))
        return false;
    } else if (symbol.EqualsASCII("SS")) {
      second = 0;
      if (!ExtractCountDigits(str, len, 2, cc, &second))
        return false;
    } else if (symbol.EqualsASCII("FFF")) {
      millisecond = 0;
      if (!ExtractCountDigits(str, len, 3, cc, &millisecond))
        return false;
    } else if (symbol.EqualsASCII("A")) {
      WideString wsAM = pLocale->GetMeridiemName(true);
      WideString wsPM = pLocale->GetMeridiemName(false);
      if ((*cc + pdfium::base::checked_cast<int32_t>(wsAM.GetLength()) <=
           len) &&
          (WideStringView(str + *cc, wsAM.GetLength()) == wsAM)) {
        *cc += wsAM.GetLength();
        bHasA = true;
      } else if ((*cc + pdfium::base::checked_cast<int32_t>(wsPM.GetLength()) <=
                  len) &&
                 (WideStringView(str + *cc, wsPM.GetLength()) == wsPM)) {
        *cc += wsPM.GetLength();
        bHasA = true;
        bPM = true;
      }
    } else if (symbol.EqualsASCII("Z")) {
      if (*cc + 3 > len)
        continue;

      WideString tz(str[(*cc)++]);
      tz += str[(*cc)++];
      tz += str[(*cc)++];
      if (tz.EqualsASCII("GMT")) {
        FX_TIMEZONE tzDiff;
        tzDiff.tzHour = 0;
        tzDiff.tzMinute = 0;
        if (*cc < len && (str[*cc] == '-' || str[*cc] == '+'))
          *cc += ParseTimeZone(str + *cc, len - *cc, &tzDiff);

        ResolveZone(tzDiff, pLocale, &hour, &minute);
      } else {
        // Search the timezone list. There are only 8 of them, so linear scan.
        for (size_t i = 0; i < FX_ArraySize(g_FXLocaleTimeZoneData); ++i) {
          const FX_LOCALETIMEZONEINFO& info = g_FXLocaleTimeZoneData[i];
          if (tz != info.name)
            continue;

          hour += info.iHour;
          minute += info.iHour > 0 ? info.iMinute : -info.iMinute;
          break;
        }
      }
    } else if (symbol.EqualsASCII("z")) {
      if (str[*cc] != 'Z') {
        FX_TIMEZONE tzDiff;
        *cc += ParseTimeZone(str + *cc, len - *cc, &tzDiff);
        ResolveZone(tzDiff, pLocale, &hour, &minute);
      } else {
        (*cc)++;
      }
    }
  }
  if (bHasA) {
    if (bPM) {
      hour += 12;
      if (hour == 24)
        hour = 12;
    } else {
      if (hour == 12)
        hour = 0;
    }
  }
  datetime->SetTime(hour, minute, second, millisecond);
  return !!(*cc);
}

int32_t GetNumTrailingLimit(const WideString& wsFormat,
                            int iDotPos,
                            bool* bTrimTailZeros) {
  if (iDotPos < 0)
    return 0;

  int32_t iCount = wsFormat.GetLength();
  int32_t iTreading = 0;
  for (iDotPos++; iDotPos < iCount; iDotPos++) {
    wchar_t wc = wsFormat[iDotPos];
    if (wc == L'z' || wc == L'9' || wc == 'Z') {
      iTreading++;
      *bTrimTailZeros = wc != L'9';
    }
  }
  return iTreading;
}

bool IsLeapYear(uint32_t year) {
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

bool MonthHas30Days(uint32_t month) {
  return month == 4 || month == 6 || month == 9 || month == 11;
}

bool MonthHas31Days(uint32_t month) {
  return month != 2 && !MonthHas30Days(month);
}

// |month| is 1-based. e.g. 1 means January.
uint16_t GetSolarMonthDays(uint16_t year, uint16_t month) {
  if (month == 2)
    return FX_IsLeapYear(year) ? 29 : 28;

  return MonthHas30Days(month) ? 30 : 31;
}

uint16_t GetWeekDay(uint16_t year, uint16_t month, uint16_t day) {
  static const uint16_t month_day[] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
  uint16_t nDays =
      (year - 1) % 7 + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400;
  nDays += month_day[month - 1] + day;
  if (FX_IsLeapYear(year) && month > 2)
    nDays++;
  return nDays % 7;
}

uint16_t GetWeekOfMonth(uint16_t year, uint16_t month, uint16_t day) {
  uint16_t week_day = GetWeekDay(year, month, 1);
  uint16_t week_index = 0;
  week_index += day / 7;
  day = day % 7;
  if (week_day + day > 7)
    week_index++;
  return week_index;
}

uint16_t GetWeekOfYear(uint16_t year, uint16_t month, uint16_t day) {
  uint16_t nDays = 0;
  for (uint16_t i = 1; i < month; i++)
    nDays += GetSolarMonthDays(year, i);

  nDays += day;
  uint16_t week_day = GetWeekDay(year, 1, 1);
  uint16_t week_index = 1;
  week_index += nDays / 7;
  nDays = nDays % 7;
  if (week_day + nDays > 7)
    week_index++;
  return week_index;
}

WideString NumToString(size_t fmt_size, int32_t value) {
  return WideString::Format(
      fmt_size == 1 ? L"%d" : fmt_size == 2 ? L"%02d" : L"%03d", value);
}

WideString DateFormat(const WideString& wsDatePattern,
                      LocaleIface* pLocale,
                      const CFX_DateTime& datetime) {
  WideString wsResult;
  int32_t year = datetime.GetYear();
  uint8_t month = datetime.GetMonth();
  uint8_t day = datetime.GetDay();
  int32_t ccf = 0;
  const wchar_t* strf = wsDatePattern.c_str();
  int32_t lenf = wsDatePattern.GetLength();
  WideStringView wsDateSymbols(gs_wsDateSymbols);
  while (ccf < lenf) {
    if (strf[ccf] == '\'') {
      wsResult += GetLiteralText(strf, &ccf, lenf);
      ccf++;
      continue;
    }
    if (!wsDateSymbols.Contains(strf[ccf])) {
      wsResult += strf[ccf++];
      continue;
    }

    WideString symbol;
    symbol.Reserve(4);
    symbol += strf[ccf++];
    while (ccf < lenf && strf[ccf] == symbol[0])
      symbol += strf[ccf++];

    if (symbol.EqualsASCII("D") || symbol.EqualsASCII("DD")) {
      wsResult += NumToString(symbol.GetLength(), day);
    } else if (symbol.EqualsASCII("J") || symbol.EqualsASCII("JJJ")) {
      uint16_t nDays = 0;
      for (int i = 1; i < month; i++)
        nDays += GetSolarMonthDays(year, i);
      nDays += day;
      wsResult += NumToString(symbol.GetLength(), nDays);
    } else if (symbol.EqualsASCII("M") || symbol.EqualsASCII("MM")) {
      wsResult += NumToString(symbol.GetLength(), month);
    } else if (symbol.EqualsASCII("MMM") || symbol.EqualsASCII("MMMM")) {
      wsResult += pLocale->GetMonthName(month - 1, symbol.EqualsASCII("MMM"));
    } else if (symbol.EqualsASCIINoCase("e")) {
      uint16_t wWeekDay = GetWeekDay(year, month, day);
      wsResult +=
          NumToString(1, symbol.EqualsASCII("E") ? wWeekDay + 1
                                                 : (wWeekDay ? wWeekDay : 7));
    } else if (symbol.EqualsASCII("EEE") || symbol.EqualsASCII("EEEE")) {
      wsResult += pLocale->GetDayName(GetWeekDay(year, month, day),
                                      symbol.EqualsASCII("EEE"));
    } else if (symbol.EqualsASCII("G")) {
      wsResult += pLocale->GetEraName(year > 0);
    } else if (symbol.EqualsASCII("YY")) {
      wsResult += NumToString(2, year % 100);
    } else if (symbol.EqualsASCII("YYYY")) {
      wsResult += NumToString(1, year);
    } else if (symbol.EqualsASCII("w")) {
      wsResult += NumToString(1, GetWeekOfMonth(year, month, day));
    } else if (symbol.EqualsASCII("WW")) {
      wsResult += NumToString(2, GetWeekOfYear(year, month, day));
    }
  }
  return wsResult;
}

WideString TimeFormat(const WideString& wsTimePattern,
                      LocaleIface* pLocale,
                      const CFX_DateTime& datetime) {
  WideString wsResult;
  uint8_t hour = datetime.GetHour();
  uint8_t minute = datetime.GetMinute();
  uint8_t second = datetime.GetSecond();
  uint16_t millisecond = datetime.GetMillisecond();
  int32_t ccf = 0;
  const wchar_t* strf = wsTimePattern.c_str();
  int32_t lenf = wsTimePattern.GetLength();
  uint16_t wHour = hour;
  bool bPM = false;
  if (wsTimePattern.Contains('A')) {
    if (wHour >= 12)
      bPM = true;
  }

  WideStringView wsTimeSymbols(gs_wsTimeSymbols);
  while (ccf < lenf) {
    if (strf[ccf] == '\'') {
      wsResult += GetLiteralText(strf, &ccf, lenf);
      ccf++;
      continue;
    }
    if (!wsTimeSymbols.Contains(strf[ccf])) {
      wsResult += strf[ccf++];
      continue;
    }

    WideString symbol;
    symbol.Reserve(4);
    symbol += strf[ccf++];
    while (ccf < lenf && strf[ccf] == symbol[0])
      symbol += strf[ccf++];

    if (symbol.EqualsASCII("h") || symbol.EqualsASCII("hh")) {
      if (wHour > 12)
        wHour -= 12;
      wsResult += NumToString(symbol.GetLength(), wHour == 0 ? 12 : wHour);
    } else if (symbol.EqualsASCII("K") || symbol.EqualsASCII("KK")) {
      wsResult += NumToString(symbol.GetLength(), wHour == 0 ? 24 : wHour);
    } else if (symbol.EqualsASCII("k") || symbol.EqualsASCII("kk")) {
      if (wHour > 12)
        wHour -= 12;
      wsResult += NumToString(symbol.GetLength(), wHour);
    } else if (symbol.EqualsASCII("H") || symbol.EqualsASCII("HH")) {
      wsResult += NumToString(symbol.GetLength(), wHour);
    } else if (symbol.EqualsASCII("M") || symbol.EqualsASCII("MM")) {
      wsResult += NumToString(symbol.GetLength(), minute);
    } else if (symbol.EqualsASCII("S") || symbol.EqualsASCII("SS")) {
      wsResult += NumToString(symbol.GetLength(), second);
    } else if (symbol.EqualsASCII("FFF")) {
      wsResult += NumToString(3, millisecond);
    } else if (symbol.EqualsASCII("A")) {
      wsResult += pLocale->GetMeridiemName(!bPM);
    } else if (symbol.EqualsASCIINoCase("z")) {
      if (symbol.EqualsASCII("Z"))
        wsResult += L"GMT";
      FX_TIMEZONE tz = pLocale->GetTimeZone();
      if (tz.tzHour != 0 || tz.tzMinute != 0) {
        wsResult += tz.tzHour < 0 ? L"-" : L"+";
        wsResult +=
            WideString::Format(L"%02d:%02d", abs(tz.tzHour), tz.tzMinute);
      }
    }
  }
  return wsResult;
}

WideString FormatDateTimeInternal(const CFX_DateTime& dt,
                                  const WideString& wsDatePattern,
                                  const WideString& wsTimePattern,
                                  bool bDateFirst,
                                  LocaleIface* pLocale) {
  WideString wsDateOut;
  if (!wsDatePattern.IsEmpty())
    wsDateOut = DateFormat(wsDatePattern, pLocale, dt);

  WideString wsTimeOut;
  if (!wsTimePattern.IsEmpty())
    wsTimeOut = TimeFormat(wsTimePattern, pLocale, dt);

  return bDateFirst ? wsDateOut + wsTimeOut : wsTimeOut + wsDateOut;
}

}  // namespace

bool FX_DateFromCanonical(const WideString& wsDate, CFX_DateTime* datetime) {
  const wchar_t* str = wsDate.c_str();
  int len = wsDate.GetLength();
  if (len > 10)
    return false;

  int cc = 0;
  uint32_t year = 0;
  if (!ExtractCountDigits(str, len, 4, &cc, &year))
    return false;
  if (year < 1900)
    return false;
  if (cc >= len) {
    datetime->SetDate(year, 1, 1);
    return true;
  }

  if (str[cc] == '-')
    cc++;

  uint32_t month = 0;
  if (!ExtractCountDigits(str, len, 2, &cc, &month))
    return false;
  if (month > 12 || month < 1)
    return false;
  if (cc >= len) {
    datetime->SetDate(year, month, 1);
    return true;
  }

  if (str[cc] == '-')
    cc++;

  uint32_t day = 0;
  if (!ExtractCountDigits(str, len, 2, &cc, &day))
    return false;
  if (day < 1)
    return false;
  if ((MonthHas31Days(month) && day > 31) ||
      (MonthHas30Days(month) && day > 30)) {
    return false;
  }
  if (month == 2 && day > (IsLeapYear(year) ? 29U : 28U))
    return false;

  datetime->SetDate(year, month, day);
  return true;
}

bool FX_TimeFromCanonical(WideStringView wsTime,
                          CFX_DateTime* datetime,
                          LocaleIface* pLocale) {
  if (wsTime.GetLength() == 0)
    return false;

  const wchar_t* str = wsTime.unterminated_c_str();
  int len = wsTime.GetLength();

  int cc = 0;
  uint32_t hour = 0;
  if (!ExtractCountDigits(str, len, 2, &cc, &hour))
    return false;
  if (hour >= 24)
    return false;
  if (cc >= len) {
    datetime->SetTime(hour, 0, 0, 0);
    return true;
  }

  if (str[cc] == ':')
    cc++;

  uint32_t minute = 0;
  if (!ExtractCountDigits(str, len, 2, &cc, &minute))
    return false;
  if (minute >= 60)
    return false;

  if (cc >= len) {
    datetime->SetTime(hour, minute, 0, 0);
    return true;
  }

  if (str[cc] == ':')
    cc++;

  uint32_t second = 0;
  uint32_t millisecond = 0;
  if (str[cc] != 'Z') {
    if (!ExtractCountDigits(str, len, 2, &cc, &second))
      return false;
    if (second >= 60)
      return false;
    if (cc < len && str[cc] == '.') {
      cc++;
      if (!ExtractCountDigits(str, len, 3, &cc, &millisecond))
        return false;
    }
  }

  // Skip until we find a + or - for the time zone.
  while (cc < len) {
    if (str[cc] == '+' || str[cc] == '-')
      break;
    ++cc;
  }

  if (cc < len) {
    FX_TIMEZONE tzDiff;
    tzDiff.tzHour = 0;
    tzDiff.tzMinute = 0;
    if (str[cc] != 'Z')
      cc += ParseTimeZone(str + cc, len - cc, &tzDiff);

    ResolveZone(tzDiff, pLocale, &hour, &minute);
  }

  datetime->SetTime(hour, minute, second, millisecond);
  return true;
}

CFGAS_FormatString::CFGAS_FormatString(CXFA_LocaleMgr* pLocaleMgr)
    : m_pLocaleMgr(pLocaleMgr) {}

CFGAS_FormatString::~CFGAS_FormatString() {}

void CFGAS_FormatString::SplitFormatString(
    const WideString& wsFormatString,
    std::vector<WideString>* wsPatterns) const {
  int32_t iStrLen = wsFormatString.GetLength();
  const wchar_t* pStr = wsFormatString.c_str();
  const wchar_t* pToken = pStr;
  const wchar_t* pEnd = pStr + iStrLen;
  bool iQuote = false;
  while (true) {
    if (pStr >= pEnd) {
      wsPatterns->push_back(WideString(pToken, pStr - pToken));
      return;
    }
    if (*pStr == '\'') {
      iQuote = !iQuote;
    } else if (*pStr == L'|' && !iQuote) {
      wsPatterns->push_back(WideString(pToken, pStr - pToken));
      pToken = pStr + 1;
    }
    pStr++;
  }
}

FX_LOCALECATEGORY CFGAS_FormatString::GetCategory(
    const WideString& wsPattern) const {
  FX_LOCALECATEGORY eCategory = FX_LOCALECATEGORY_Unknown;
  int32_t ccf = 0;
  int32_t iLenf = wsPattern.GetLength();
  const wchar_t* pStr = wsPattern.c_str();
  bool bBraceOpen = false;
  WideStringView wsConstChars(gs_wsConstChars);
  while (ccf < iLenf) {
    if (pStr[ccf] == '\'') {
      GetLiteralText(pStr, &ccf, iLenf);
    } else if (!bBraceOpen && !wsConstChars.Contains(pStr[ccf])) {
      WideString wsCategory(pStr[ccf]);
      ccf++;
      while (true) {
        if (ccf == iLenf)
          return eCategory;
        if (pStr[ccf] == '.' || pStr[ccf] == '(')
          break;
        if (pStr[ccf] == '{') {
          bBraceOpen = true;
          break;
        }
        wsCategory += pStr[ccf];
        ccf++;
      }

      uint32_t dwHash = FX_HashCode_GetW(wsCategory.AsStringView(), false);
      if (dwHash == FX_LOCALECATEGORY_DateTimeHash)
        return FX_LOCALECATEGORY_DateTime;
      if (dwHash == FX_LOCALECATEGORY_TextHash)
        return FX_LOCALECATEGORY_Text;
      if (dwHash == FX_LOCALECATEGORY_NumHash)
        return FX_LOCALECATEGORY_Num;
      if (dwHash == FX_LOCALECATEGORY_ZeroHash)
        return FX_LOCALECATEGORY_Zero;
      if (dwHash == FX_LOCALECATEGORY_NullHash)
        return FX_LOCALECATEGORY_Null;
      if (dwHash == FX_LOCALECATEGORY_DateHash) {
        if (eCategory == FX_LOCALECATEGORY_Time)
          return FX_LOCALECATEGORY_DateTime;
        eCategory = FX_LOCALECATEGORY_Date;
      } else if (dwHash == FX_LOCALECATEGORY_TimeHash) {
        if (eCategory == FX_LOCALECATEGORY_Date)
          return FX_LOCALECATEGORY_DateTime;
        eCategory = FX_LOCALECATEGORY_Time;
      }
    } else if (pStr[ccf] == '}') {
      bBraceOpen = false;
    }
    ccf++;
  }
  return eCategory;
}

WideString CFGAS_FormatString::GetTextFormat(const WideString& wsPattern,
                                             WideStringView wsCategory) const {
  int32_t ccf = 0;
  int32_t iLenf = wsPattern.GetLength();
  const wchar_t* pStr = wsPattern.c_str();
  bool bBrackOpen = false;
  WideStringView wsConstChars(gs_wsConstChars);
  WideString wsPurgePattern;
  while (ccf < iLenf) {
    if (pStr[ccf] == '\'') {
      int32_t iCurChar = ccf;
      GetLiteralText(pStr, &ccf, iLenf);
      wsPurgePattern += WideStringView(pStr + iCurChar, ccf - iCurChar + 1);
    } else if (!bBrackOpen && !wsConstChars.Contains(pStr[ccf])) {
      WideString wsSearchCategory(pStr[ccf]);
      ccf++;
      while (ccf < iLenf && pStr[ccf] != '{' && pStr[ccf] != '.' &&
             pStr[ccf] != '(') {
        wsSearchCategory += pStr[ccf];
        ccf++;
      }
      if (wsSearchCategory != wsCategory)
        continue;

      while (ccf < iLenf) {
        if (pStr[ccf] == '(') {
          ccf++;
          // Skip over the encoding name.
          while (ccf < iLenf && pStr[ccf] != ')')
            ccf++;
        } else if (pStr[ccf] == '{') {
          bBrackOpen = true;
          break;
        }
        ccf++;
      }
    } else if (pStr[ccf] != '}') {
      wsPurgePattern += pStr[ccf];
    }
    ccf++;
  }
  if (!bBrackOpen)
    wsPurgePattern = wsPattern;

  return wsPurgePattern;
}

LocaleIface* CFGAS_FormatString::GetNumericFormat(
    const WideString& wsPattern,
    int32_t* iDotIndex,
    uint32_t* dwStyle,
    WideString* wsPurgePattern) const {
  *dwStyle = 0;
  LocaleIface* pLocale = nullptr;
  int32_t ccf = 0;
  int32_t iLenf = wsPattern.GetLength();
  const wchar_t* pStr = wsPattern.c_str();
  bool bFindDot = false;
  bool bBrackOpen = false;
  WideStringView wsConstChars(gs_wsConstChars);
  while (ccf < iLenf) {
    if (pStr[ccf] == '\'') {
      int32_t iCurChar = ccf;
      GetLiteralText(pStr, &ccf, iLenf);
      *wsPurgePattern += WideStringView(pStr + iCurChar, ccf - iCurChar + 1);
    } else if (!bBrackOpen && !wsConstChars.Contains(pStr[ccf])) {
      WideString wsCategory(pStr[ccf]);
      ccf++;
      while (ccf < iLenf && pStr[ccf] != '{' && pStr[ccf] != '.' &&
             pStr[ccf] != '(') {
        wsCategory += pStr[ccf];
        ccf++;
      }
      if (!wsCategory.EqualsASCII("num")) {
        bBrackOpen = true;
        ccf = 0;
        continue;
      }
      while (ccf < iLenf) {
        if (pStr[ccf] == '{') {
          bBrackOpen = true;
          break;
        }
        if (pStr[ccf] == '(') {
          ccf++;
          WideString wsLCID;
          while (ccf < iLenf && pStr[ccf] != ')')
            wsLCID += pStr[ccf++];

          //pLocale = m_pLocaleMgr->GetLocaleByName(wsLCID);
          DebugBreak();
        } else if (pStr[ccf] == '.') {
          WideString wsSubCategory;
          ccf++;
          while (ccf < iLenf && pStr[ccf] != '(' && pStr[ccf] != '{')
            wsSubCategory += pStr[ccf++];

          uint32_t dwSubHash =
              FX_HashCode_GetW(wsSubCategory.AsStringView(), false);
          FX_LOCALENUMSUBCATEGORY eSubCategory = FX_LOCALENUMPATTERN_Decimal;
          for (const auto& data : g_FXLocaleNumSubCatData) {
            if (data.uHash == dwSubHash) {
              eSubCategory = data.eSubCategory;
              break;
            }
          }
//           if (!pLocale)
//             pLocale = m_pLocaleMgr->GetDefLocale();
          DebugBreak();

          ASSERT(pLocale);

          wsSubCategory = pLocale->GetNumPattern(eSubCategory);
          auto result = wsSubCategory.Find('.');
          if (result.has_value() && result.value() != 0) {
            *iDotIndex += wsPurgePattern->GetLength();
            bFindDot = true;
            *dwStyle |= FX_NUMSTYLE_DotVorv;
          }
          *wsPurgePattern += wsSubCategory;
          if (eSubCategory == FX_LOCALENUMPATTERN_Percent)
            *dwStyle |= FX_NUMSTYLE_Percent;

          continue;
        }
        ccf++;
      }
    } else if (pStr[ccf] == 'E') {
      *dwStyle |= FX_NUMSTYLE_Exponent;
      *wsPurgePattern += pStr[ccf];
    } else if (pStr[ccf] == '%') {
      *dwStyle |= FX_NUMSTYLE_Percent;
      *wsPurgePattern += pStr[ccf];
    } else if (pStr[ccf] != '}') {
      *wsPurgePattern += pStr[ccf];
    }
    if (!bFindDot) {
      if (pStr[ccf] == '.' || pStr[ccf] == 'V' || pStr[ccf] == 'v') {
        bFindDot = true;
        *iDotIndex = wsPurgePattern->GetLength() - 1;
        *dwStyle |= FX_NUMSTYLE_DotVorv;
      }
    }
    ccf++;
  }
  if (!bFindDot)
    *iDotIndex = wsPurgePattern->GetLength();
//   if (!pLocale)
//     pLocale = m_pLocaleMgr->GetDefLocale();
  DebugBreak();
  return pLocale;
}

bool CFGAS_FormatString::ParseText(const WideString& wsSrcText,
                                   const WideString& wsPattern,
                                   WideString* wsValue) const {
  wsValue->clear();
  if (wsSrcText.IsEmpty() || wsPattern.IsEmpty())
    return false;

  WideString wsTextFormat = GetTextFormat(wsPattern, L"text");
  if (wsTextFormat.IsEmpty())
    return false;

  int32_t iText = 0;
  int32_t iPattern = 0;
  const wchar_t* pStrText = wsSrcText.c_str();
  int32_t iLenText = wsSrcText.GetLength();
  const wchar_t* pStrPattern = wsTextFormat.c_str();
  int32_t iLenPattern = wsTextFormat.GetLength();
  while (iPattern < iLenPattern && iText < iLenText) {
    switch (pStrPattern[iPattern]) {
      case '\'': {
        WideString wsLiteral =
            GetLiteralText(pStrPattern, &iPattern, iLenPattern);
        int32_t iLiteralLen = wsLiteral.GetLength();
        if (iText + iLiteralLen > iLenText ||
            wcsncmp(pStrText + iText, wsLiteral.c_str(), iLiteralLen)) {
          *wsValue = wsSrcText;
          return false;
        }
        iText += iLiteralLen;
        iPattern++;
        break;
      }
      case 'A':
        if (FXSYS_iswalpha(pStrText[iText])) {
          *wsValue += pStrText[iText];
          iText++;
        }
        iPattern++;
        break;
      case 'X':
        *wsValue += pStrText[iText];
        iText++;
        iPattern++;
        break;
      case 'O':
      case '0':
        if (FXSYS_IsDecimalDigit(pStrText[iText]) ||
            FXSYS_iswalpha(pStrText[iText])) {
          *wsValue += pStrText[iText];
          iText++;
        }
        iPattern++;
        break;
      case '9':
        if (FXSYS_IsDecimalDigit(pStrText[iText])) {
          *wsValue += pStrText[iText];
          iText++;
        }
        iPattern++;
        break;
      default:
        if (pStrPattern[iPattern] != pStrText[iText]) {
          *wsValue = wsSrcText;
          return false;
        }
        iPattern++;
        iText++;
        break;
    }
  }
  return iPattern == iLenPattern && iText == iLenText;
}

bool CFGAS_FormatString::ParseNum(const WideString& wsSrcNum,
                                  const WideString& wsPattern,
                                  WideString* wsValue) const {
  wsValue->clear();
  if (wsSrcNum.IsEmpty() || wsPattern.IsEmpty())
    return false;

  int32_t dot_index_f = -1;
  uint32_t dwFormatStyle = 0;
  WideString wsNumFormat;
  LocaleIface* pLocale =
      GetNumericFormat(wsPattern, &dot_index_f, &dwFormatStyle, &wsNumFormat);
  if (!pLocale || wsNumFormat.IsEmpty())
    return false;

  int32_t iExponent = 0;
  WideString wsDotSymbol = pLocale->GetDecimalSymbol();
  WideString wsGroupSymbol = pLocale->GetGroupingSymbol();
  int32_t iGroupLen = wsGroupSymbol.GetLength();
  WideString wsMinus = pLocale->GetMinusSymbol();
  int32_t iMinusLen = wsMinus.GetLength();
  const wchar_t* str = wsSrcNum.c_str();
  int len = wsSrcNum.GetLength();
  const wchar_t* strf = wsNumFormat.c_str();
  int lenf = wsNumFormat.GetLength();
  bool bHavePercentSymbol = false;
  bool bNeg = false;
  bool bReverseParse = false;
  int32_t dot_index = 0;

  // If we're looking for a '.', 'V' or 'v' and the input string does not
  // have a dot index for one of those, then we disable parsing the decimal.
  if (!GetNumericDotIndex(wsSrcNum, wsDotSymbol, &dot_index) &&
      (dwFormatStyle & FX_NUMSTYLE_DotVorv))
    bReverseParse = true;

  // This parse is broken into two parts based on the '.' in the number
  // (or 'V' or 'v'). |dot_index_f| is the location of the dot in the format and
  // |dot_index| is the location of the dot in the number.
  //
  // This first while() starts at the '.' and walks backwards to the start of
  // the number. The second while() walks from the dot forwards to the end of
  // the decimal.

  int ccf = dot_index_f - 1;
  int cc = dot_index - 1;
  while (ccf >= 0 && cc >= 0) {
    switch (strf[ccf]) {
      case '\'': {
        WideString wsLiteral = GetLiteralTextReverse(strf, &ccf);
        int32_t iLiteralLen = wsLiteral.GetLength();
        cc -= iLiteralLen - 1;
        if (cc < 0 || wcsncmp(str + cc, wsLiteral.c_str(), iLiteralLen))
          return false;

        cc--;
        ccf--;
        break;
      }
      case '9':
        if (!FXSYS_IsDecimalDigit(str[cc]))
          return false;

        wsValue->InsertAtFront(str[cc]);
        cc--;
        ccf--;
        break;
      case 'z':
      case 'Z':
        if (strf[ccf] == 'z' || str[cc] != ' ') {
          if (FXSYS_IsDecimalDigit(str[cc])) {
            wsValue->InsertAtFront(str[cc]);
            cc--;
          }
        } else {
          cc--;
        }
        ccf--;
        break;
      case 'S':
      case 's':
        if (str[cc] == '+' || (strf[ccf] == 'S' && str[cc] == ' ')) {
          cc--;
        } else {
          cc -= iMinusLen - 1;
          if (cc < 0 || wcsncmp(str + cc, wsMinus.c_str(), iMinusLen))
            return false;

          cc--;
          bNeg = true;
        }
        ccf--;
        break;
      case 'E': {
        bool bExpSign = false;
        while (cc >= 0) {
          if (str[cc] == 'E' || str[cc] == 'e')
            break;
          if (FXSYS_IsDecimalDigit(str[cc])) {
            iExponent = iExponent + FXSYS_DecimalCharToInt(str[cc]) * 10;
            cc--;
            continue;
          }
          if (str[cc] == '+') {
            cc--;
            continue;
          }
          if (cc - iMinusLen + 1 > 0 && !wcsncmp(str + (cc - iMinusLen + 1),
                                                 wsMinus.c_str(), iMinusLen)) {
            bExpSign = true;
            cc -= iMinusLen;
            continue;
          }

          return false;
        }
        cc--;
        iExponent = bExpSign ? -iExponent : iExponent;
        ccf--;
        break;
      }
      case '$': {
        WideString wsSymbol = pLocale->GetCurrencySymbol();
        int32_t iSymbolLen = wsSymbol.GetLength();
        cc -= iSymbolLen - 1;
        if (cc < 0 || wcsncmp(str + cc, wsSymbol.c_str(), iSymbolLen))
          return false;

        cc--;
        ccf--;
        break;
      }
      case 'r':
      case 'R':
        if (ccf - 1 >= 0 && ((strf[ccf] == 'R' && strf[ccf - 1] == 'C') ||
                             (strf[ccf] == 'r' && strf[ccf - 1] == 'c'))) {
          if (strf[ccf] == 'R' && str[cc] == ' ') {
            cc -= 2;
          } else if (str[cc] == 'R' && cc - 1 >= 0 && str[cc - 1] == 'C') {
            bNeg = true;
            cc -= 2;
          }
          ccf -= 2;
        } else {
          ccf--;
        }
        break;
      case 'b':
      case 'B':
        if (ccf - 1 >= 0 && ((strf[ccf] == 'B' && strf[ccf - 1] == 'D') ||
                             (strf[ccf] == 'b' && strf[ccf - 1] == 'd'))) {
          if (strf[ccf] == 'B' && str[cc] == ' ') {
            cc -= 2;
          } else if (str[cc] == 'B' && cc - 1 >= 0 && str[cc - 1] == 'D') {
            bNeg = true;
            cc -= 2;
          }
          ccf -= 2;
        } else {
          ccf--;
        }
        break;
      case '%': {
        WideString wsSymbol = pLocale->GetPercentSymbol();
        int32_t iSysmbolLen = wsSymbol.GetLength();
        cc -= iSysmbolLen - 1;
        if (cc < 0 || wcsncmp(str + cc, wsSymbol.c_str(), iSysmbolLen))
          return false;

        cc--;
        ccf--;
        bHavePercentSymbol = true;
        break;
      }
      case '.':
      case 'V':
      case 'v':
      case '8':
        return false;
      case ',': {
        if (cc >= 0) {
          cc -= iGroupLen - 1;
          if (cc >= 0 &&
              wcsncmp(str + cc, wsGroupSymbol.c_str(), iGroupLen) == 0) {
            cc--;
          } else {
            cc += iGroupLen - 1;
          }
        }
        ccf--;
        break;
      }
      case '(':
      case ')':
        if (str[cc] == strf[ccf])
          bNeg = true;
        else if (str[cc] != L' ')
          return false;

        cc--;
        ccf--;
        break;
      default:
        if (strf[ccf] != str[cc])
          return false;

        cc--;
        ccf--;
    }
  }
  if (cc >= 0) {
    if (str[cc] == '-') {
      bNeg = true;
      cc--;
    }
    if (cc >= 0)
      return false;
  }
  if (dot_index < len && (dwFormatStyle & FX_NUMSTYLE_DotVorv))
    *wsValue += '.';
  if (!bReverseParse) {
    ccf = dot_index_f + 1;
    cc = (dot_index == len) ? len : dot_index + 1;
    while (cc < len && ccf < lenf) {
      switch (strf[ccf]) {
        case '\'': {
          WideString wsLiteral = GetLiteralText(strf, &ccf, lenf);
          int32_t iLiteralLen = wsLiteral.GetLength();
          if (cc + iLiteralLen > len ||
              wcsncmp(str + cc, wsLiteral.c_str(), iLiteralLen)) {
            return false;
          }
          cc += iLiteralLen;
          ccf++;
          break;
        }
        case '9':
          if (!FXSYS_IsDecimalDigit(str[cc]))
            return false;

          *wsValue += str[cc];
          cc++;
          ccf++;
          break;
        case 'z':
        case 'Z':
          if (strf[ccf] == 'z' || str[cc] != ' ') {
            if (FXSYS_IsDecimalDigit(str[cc])) {
              *wsValue += str[cc];
              cc++;
            }
          } else {
            cc++;
          }
          ccf++;
          break;
        case 'S':
        case 's':
          if (str[cc] == '+' || (strf[ccf] == 'S' && str[cc] == ' ')) {
            cc++;
          } else {
            if (cc + iMinusLen > len ||
                wcsncmp(str + cc, wsMinus.c_str(), iMinusLen)) {
              return false;
            }
            bNeg = true;
            cc += iMinusLen;
          }
          ccf++;
          break;
        case 'E': {
          if (cc >= len || (str[cc] != 'E' && str[cc] != 'e'))
            return false;

          bool bExpSign = false;
          cc++;
          if (cc < len) {
            if (str[cc] == '+') {
              cc++;
            } else if (str[cc] == '-') {
              bExpSign = true;
              cc++;
            }
          }
          while (cc < len) {
            if (!FXSYS_IsDecimalDigit(str[cc]))
              break;

            iExponent = iExponent * 10 + FXSYS_DecimalCharToInt(str[cc]);
            cc++;
          }
          iExponent = bExpSign ? -iExponent : iExponent;
          ccf++;
          break;
        }
        case '$': {
          WideString wsSymbol = pLocale->GetCurrencySymbol();
          int32_t iSymbolLen = wsSymbol.GetLength();
          if (cc + iSymbolLen > len ||
              wcsncmp(str + cc, wsSymbol.c_str(), iSymbolLen)) {
            return false;
          }
          cc += iSymbolLen;
          ccf++;
          break;
        }
        case 'c':
        case 'C':
          if (ccf + 1 < lenf && ((strf[ccf] == 'C' && strf[ccf + 1] == 'R') ||
                                 (strf[ccf] == 'c' && strf[ccf + 1] == 'r'))) {
            if (strf[ccf] == 'C' && str[cc] == ' ') {
              cc++;
            } else if (str[cc] == 'C' && cc + 1 < len && str[cc + 1] == 'R') {
              bNeg = true;
              cc += 2;
            }
            ccf += 2;
          }
          break;
        case 'd':
        case 'D':
          if (ccf + 1 < lenf && ((strf[ccf] == 'D' && strf[ccf + 1] == 'B') ||
                                 (strf[ccf] == 'd' && strf[ccf + 1] == 'b'))) {
            if (strf[ccf] == 'D' && str[cc] == ' ') {
              cc++;
            } else if (str[cc] == 'D' && cc + 1 < len && str[cc + 1] == 'B') {
              bNeg = true;
              cc += 2;
            }
            ccf += 2;
          }
          break;
        case '.':
        case 'V':
        case 'v':
          return false;
        case '%': {
          WideString wsSymbol = pLocale->GetPercentSymbol();
          int32_t iSysmbolLen = wsSymbol.GetLength();
          if (cc + iSysmbolLen <= len &&
              !wcsncmp(str + cc, wsSymbol.c_str(), iSysmbolLen)) {
            cc += iSysmbolLen;
          }
          ccf++;
          bHavePercentSymbol = true;
        } break;
        case '8': {
          while (ccf < lenf && strf[ccf] == '8')
            ccf++;

          while (cc < len && FXSYS_IsDecimalDigit(str[cc])) {
            *wsValue += str[cc];
            cc++;
          }
        } break;
        case ',': {
          if (cc + iGroupLen <= len &&
              wcsncmp(str + cc, wsGroupSymbol.c_str(), iGroupLen) == 0) {
            cc += iGroupLen;
          }
          ccf++;
          break;
        }
        case '(':
        case ')':
          if (str[cc] == strf[ccf])
            bNeg = true;
          else if (str[cc] != L' ')
            return false;

          cc++;
          ccf++;
          break;
        default:
          if (strf[ccf] != str[cc])
            return false;

          cc++;
          ccf++;
      }
    }
    if (cc != len)
      return false;
  }
  if (iExponent || bHavePercentSymbol) {
    CFGAS_Decimal decimal = CFGAS_Decimal(wsValue->AsStringView());
    if (iExponent) {
      decimal = decimal *
                CFGAS_Decimal(FXSYS_pow(10, static_cast<float>(iExponent)), 3);
    }
    if (bHavePercentSymbol)
      decimal = decimal / CFGAS_Decimal(100);

    *wsValue = decimal;
  }
  if (bNeg)
    wsValue->InsertAtFront(L'-');

  return true;
}

FX_DATETIMETYPE CFGAS_FormatString::GetDateTimeFormat(
    const WideString& wsPattern,
    LocaleIface** pLocale,
    WideString* wsDatePattern,
    WideString* wsTimePattern) const {
  *pLocale = nullptr;
  WideString wsTempPattern;
  FX_LOCALECATEGORY eCategory = FX_LOCALECATEGORY_Unknown;
  int32_t ccf = 0;
  int32_t iLenf = wsPattern.GetLength();
  const wchar_t* pStr = wsPattern.c_str();
  int32_t iFindCategory = 0;
  bool bBraceOpen = false;
  WideStringView wsConstChars(gs_wsConstChars);
  while (ccf < iLenf) {
    if (pStr[ccf] == '\'') {
      int32_t iCurChar = ccf;
      GetLiteralText(pStr, &ccf, iLenf);
      wsTempPattern += WideStringView(pStr + iCurChar, ccf - iCurChar + 1);
    } else if (!bBraceOpen && iFindCategory != 3 &&
               !wsConstChars.Contains(pStr[ccf])) {
      WideString wsCategory(pStr[ccf]);
      ccf++;
      while (ccf < iLenf && pStr[ccf] != '{' && pStr[ccf] != '.' &&
             pStr[ccf] != '(') {
        if (pStr[ccf] == 'T') {
          *wsDatePattern = wsPattern.Left(ccf);
          *wsTimePattern = wsPattern.Right(wsPattern.GetLength() - ccf);
          wsTimePattern->SetAt(0, ' ');
//           if (!*pLocale)
//             *pLocale = m_pLocaleMgr->GetDefLocale();
          DebugBreak();

          return FX_DATETIMETYPE_DateTime;
        }
        wsCategory += pStr[ccf];
        ccf++;
      }
      if (!(iFindCategory & 1) && wsCategory.EqualsASCII("date")) {
        iFindCategory |= 1;
        eCategory = FX_LOCALECATEGORY_Date;
        if (iFindCategory & 2)
          iFindCategory = 4;
      } else if (!(iFindCategory & 2) && wsCategory.EqualsASCII("time")) {
        iFindCategory |= 2;
        eCategory = FX_LOCALECATEGORY_Time;
      } else if (wsCategory.EqualsASCII("datetime")) {
        iFindCategory = 3;
        eCategory = FX_LOCALECATEGORY_DateTime;
      } else {
        continue;
      }
      while (ccf < iLenf) {
        if (pStr[ccf] == '{') {
          bBraceOpen = true;
          break;
        }
        if (pStr[ccf] == '(') {
          ccf++;
          WideString wsLCID;
          while (ccf < iLenf && pStr[ccf] != ')')
            wsLCID += pStr[ccf++];

          //*pLocale = m_pLocaleMgr->GetLocaleByName(wsLCID);
          DebugBreak();
        } else if (pStr[ccf] == '.') {
          WideString wsSubCategory;
          ccf++;
          while (ccf < iLenf && pStr[ccf] != '(' && pStr[ccf] != '{')
            wsSubCategory += pStr[ccf++];

          uint32_t dwSubHash =
              FX_HashCode_GetW(wsSubCategory.AsStringView(), false);
          FX_LOCALEDATETIMESUBCATEGORY eSubCategory =
              FX_LOCALEDATETIMESUBCATEGORY_Medium;
          for (const auto& data : g_FXLocaleDateTimeSubCatData) {
            if (data.uHash == dwSubHash) {
              eSubCategory = data.eSubCategory;
              break;
            }
          }
//           if (!*pLocale)
//             *pLocale = m_pLocaleMgr->GetDefLocale();
//           ASSERT(*pLocale);
          DebugBreak();

          switch (eCategory) {
            case FX_LOCALECATEGORY_Date:
              *wsDatePattern =
                  wsTempPattern + (*pLocale)->GetDatePattern(eSubCategory);
              break;
            case FX_LOCALECATEGORY_Time:
              *wsTimePattern =
                  wsTempPattern + (*pLocale)->GetTimePattern(eSubCategory);
              break;
            case FX_LOCALECATEGORY_DateTime:
              *wsDatePattern =
                  wsTempPattern + (*pLocale)->GetDatePattern(eSubCategory);
              *wsTimePattern = (*pLocale)->GetTimePattern(eSubCategory);
              break;
            default:
              break;
          }
          wsTempPattern.clear();
          continue;
        }
        ccf++;
      }
    } else if (pStr[ccf] == '}') {
      bBraceOpen = false;
      if (!wsTempPattern.IsEmpty()) {
        if (eCategory == FX_LOCALECATEGORY_Time)
          *wsTimePattern = std::move(wsTempPattern);
        else if (eCategory == FX_LOCALECATEGORY_Date)
          *wsDatePattern = std::move(wsTempPattern);
        else
          wsTempPattern.clear();
      }
    } else {
      wsTempPattern += pStr[ccf];
    }
    ccf++;
  }

  if (!wsTempPattern.IsEmpty()) {
    if (eCategory == FX_LOCALECATEGORY_Date)
      *wsDatePattern += wsTempPattern;
    else
      *wsTimePattern += wsTempPattern;
  }
//   if (!*pLocale)
//     *pLocale = m_pLocaleMgr->GetDefLocale();
  DebugBreak();
  if (!iFindCategory) {
    wsTimePattern->clear();
    *wsDatePattern = wsPattern;
  }
  return (FX_DATETIMETYPE)iFindCategory;
}

bool CFGAS_FormatString::ParseDateTime(const WideString& wsSrcDateTime,
                                       const WideString& wsPattern,
                                       FX_DATETIMETYPE eDateTimeType,
                                       CFX_DateTime* dtValue) const {
  dtValue->Reset();
  if (wsSrcDateTime.IsEmpty() || wsPattern.IsEmpty())
    return false;

  WideString wsDatePattern;
  WideString wsTimePattern;
  LocaleIface* pLocale = nullptr;
  FX_DATETIMETYPE eCategory =
      GetDateTimeFormat(wsPattern, &pLocale, &wsDatePattern, &wsTimePattern);
  if (!pLocale)
    return false;
  if (eCategory == FX_DATETIMETYPE_Unknown)
    eCategory = eDateTimeType;
  if (eCategory == FX_DATETIMETYPE_Unknown)
    return false;
  if (eCategory == FX_DATETIMETYPE_TimeDate) {
    int32_t iStart = 0;
    if (!ParseLocaleTime(wsSrcDateTime, wsTimePattern, pLocale, dtValue,
                         &iStart)) {
      return false;
    }
    if (!ParseLocaleDate(wsSrcDateTime, wsDatePattern, pLocale, dtValue,
                         &iStart)) {
      return false;
    }
  } else {
    int32_t iStart = 0;
    if ((eCategory & FX_DATETIMETYPE_Date) &&
        !ParseLocaleDate(wsSrcDateTime, wsDatePattern, pLocale, dtValue,
                         &iStart)) {
      return false;
    }
    if ((eCategory & FX_DATETIMETYPE_Time) &&
        !ParseLocaleTime(wsSrcDateTime, wsTimePattern, pLocale, dtValue,
                         &iStart)) {
      return false;
    }
  }
  return true;
}

bool CFGAS_FormatString::ParseZero(const WideString& wsSrcText,
                                   const WideString& wsPattern) const {
  WideString wsTextFormat = GetTextFormat(wsPattern, L"zero");

  int32_t iText = 0;
  int32_t iPattern = 0;
  const wchar_t* pStrText = wsSrcText.c_str();
  int32_t iLenText = wsSrcText.GetLength();
  const wchar_t* pStrPattern = wsTextFormat.c_str();
  int32_t iLenPattern = wsTextFormat.GetLength();
  while (iPattern < iLenPattern && iText < iLenText) {
    if (pStrPattern[iPattern] == '\'') {
      WideString wsLiteral =
          GetLiteralText(pStrPattern, &iPattern, iLenPattern);
      int32_t iLiteralLen = wsLiteral.GetLength();
      if (iText + iLiteralLen > iLenText ||
          wcsncmp(pStrText + iText, wsLiteral.c_str(), iLiteralLen)) {
        return false;
      }
      iText += iLiteralLen;
      iPattern++;
      continue;
    }
    if (pStrPattern[iPattern] != pStrText[iText])
      return false;

    iText++;
    iPattern++;
  }
  return iPattern == iLenPattern && iText == iLenText;
}

bool CFGAS_FormatString::ParseNull(const WideString& wsSrcText,
                                   const WideString& wsPattern) const {
  WideString wsTextFormat = GetTextFormat(wsPattern, L"null");

  int32_t iText = 0;
  int32_t iPattern = 0;
  const wchar_t* pStrText = wsSrcText.c_str();
  int32_t iLenText = wsSrcText.GetLength();
  const wchar_t* pStrPattern = wsTextFormat.c_str();
  int32_t iLenPattern = wsTextFormat.GetLength();
  while (iPattern < iLenPattern && iText < iLenText) {
    if (pStrPattern[iPattern] == '\'') {
      WideString wsLiteral =
          GetLiteralText(pStrPattern, &iPattern, iLenPattern);
      int32_t iLiteralLen = wsLiteral.GetLength();
      if (iText + iLiteralLen > iLenText ||
          wcsncmp(pStrText + iText, wsLiteral.c_str(), iLiteralLen)) {
        return false;
      }
      iText += iLiteralLen;
      iPattern++;
      continue;
    }
    if (pStrPattern[iPattern] != pStrText[iText])
      return false;

    iText++;
    iPattern++;
  }
  return iPattern == iLenPattern && iText == iLenText;
}

bool CFGAS_FormatString::FormatText(const WideString& wsSrcText,
                                    const WideString& wsPattern,
                                    WideString* wsOutput) const {
  if (wsPattern.IsEmpty())
    return false;

  int32_t iLenText = wsSrcText.GetLength();
  if (iLenText == 0)
    return false;

  WideString wsTextFormat = GetTextFormat(wsPattern, L"text");

  int32_t iText = 0;
  int32_t iPattern = 0;
  const wchar_t* pStrText = wsSrcText.c_str();
  const wchar_t* pStrPattern = wsTextFormat.c_str();
  int32_t iLenPattern = wsTextFormat.GetLength();
  while (iPattern < iLenPattern) {
    switch (pStrPattern[iPattern]) {
      case '\'': {
        *wsOutput += GetLiteralText(pStrPattern, &iPattern, iLenPattern);
        iPattern++;
        break;
      }
      case 'A':
        if (iText >= iLenText || !FXSYS_iswalpha(pStrText[iText]))
          return false;

        *wsOutput += pStrText[iText++];
        iPattern++;
        break;
      case 'X':
        if (iText >= iLenText)
          return false;

        *wsOutput += pStrText[iText++];
        iPattern++;
        break;
      case 'O':
      case '0':
        if (iText >= iLenText || (!FXSYS_IsDecimalDigit(pStrText[iText]) &&
                                  !FXSYS_iswalpha(pStrText[iText]))) {
          return false;
        }
        *wsOutput += pStrText[iText++];
        iPattern++;
        break;
      case '9':
        if (iText >= iLenText || !FXSYS_IsDecimalDigit(pStrText[iText]))
          return false;

        *wsOutput += pStrText[iText++];
        iPattern++;
        break;
      default:
        *wsOutput += pStrPattern[iPattern++];
        break;
    }
  }
  return iText == iLenText;
}

bool CFGAS_FormatString::FormatStrNum(WideStringView wsInputNum,
                                      const WideString& wsPattern,
                                      WideString* wsOutput) const {
  if (wsInputNum.IsEmpty() || wsPattern.IsEmpty())
    return false;

  int32_t dot_index_f = -1;
  uint32_t dwNumStyle = 0;
  WideString wsNumFormat;
  LocaleIface* pLocale =
      GetNumericFormat(wsPattern, &dot_index_f, &dwNumStyle, &wsNumFormat);
  if (!pLocale || wsNumFormat.IsEmpty())
    return false;

  int32_t cc = 0, ccf = 0;
  const wchar_t* strf = wsNumFormat.c_str();
  int lenf = wsNumFormat.GetLength();
  WideString wsSrcNum(wsInputNum);
  wsSrcNum.TrimLeft('0');
  if (wsSrcNum.IsEmpty() || wsSrcNum[0] == '.')
    wsSrcNum.InsertAtFront('0');

  CFGAS_Decimal decimal = CFGAS_Decimal(wsSrcNum.AsStringView());
  if (dwNumStyle & FX_NUMSTYLE_Percent) {
    decimal = decimal * CFGAS_Decimal(100);
    wsSrcNum = decimal;
  }

  int32_t exponent = 0;
  if (dwNumStyle & FX_NUMSTYLE_Exponent) {
    int fixed_count = 0;
    while (ccf < dot_index_f) {
      switch (strf[ccf]) {
        case '\'':
          GetLiteralText(strf, &ccf, dot_index_f);
          break;
        case '9':
        case 'z':
        case 'Z':
          fixed_count++;
          break;
      }
      ccf++;
    }

    int threshold = 1;
    while (fixed_count > 1) {
      threshold *= 10;
      fixed_count--;
    }
    if (decimal != CFGAS_Decimal(0)) {
      if (decimal < CFGAS_Decimal(threshold)) {
        decimal = decimal * CFGAS_Decimal(10);
        exponent = -1;
        while (decimal < CFGAS_Decimal(threshold)) {
          decimal = decimal * CFGAS_Decimal(10);
          exponent -= 1;
        }
      } else if (decimal > CFGAS_Decimal(threshold)) {
        threshold *= 10;
        while (decimal > CFGAS_Decimal(threshold)) {
          decimal = decimal / CFGAS_Decimal(10);
          exponent += 1;
        }
      }
    }
  }

  bool bTrimTailZeros = false;
  int32_t iTreading =
      GetNumTrailingLimit(wsNumFormat, dot_index_f, &bTrimTailZeros);
  int32_t scale = decimal.GetScale();
  if (iTreading < scale) {
    decimal.SetScale(iTreading);
    wsSrcNum = decimal;
  }
  if (bTrimTailZeros && scale > 0 && iTreading > 0) {
    wsSrcNum.TrimRight(L"0");
    wsSrcNum.TrimRight(L".");
  }

  WideString wsGroupSymbol = pLocale->GetGroupingSymbol();
  bool bNeg = false;
  if (wsSrcNum[0] == '-') {
    bNeg = true;
    wsSrcNum.Delete(0, 1);
  }

  bool bAddNeg = false;
  const wchar_t* str = wsSrcNum.c_str();
  int len = wsSrcNum.GetLength();
  auto dot_index = wsSrcNum.Find('.');
  if (!dot_index.has_value())
    dot_index = len;

  ccf = dot_index_f - 1;
  cc = dot_index.value() - 1;
  while (ccf >= 0) {
    switch (strf[ccf]) {
      case '9':
        if (cc >= 0) {
          if (!FXSYS_IsDecimalDigit(str[cc]))
            return false;

          wsOutput->InsertAtFront(str[cc]);
          cc--;
        } else {
          wsOutput->InsertAtFront(L'0');
        }
        ccf--;
        break;
      case 'z':
        if (cc >= 0) {
          if (!FXSYS_IsDecimalDigit(str[cc]))
            return false;
          if (str[0] != '0')
            wsOutput->InsertAtFront(str[cc]);

          cc--;
        }
        ccf--;
        break;
      case 'Z':
        if (cc >= 0) {
          if (!FXSYS_IsDecimalDigit(str[cc]))
            return false;

          wsOutput->InsertAtFront(str[0] == '0' ? L' ' : str[cc]);
          cc--;
        } else {
          wsOutput->InsertAtFront(L' ');
        }
        ccf--;
        break;
      case 'S':
        if (bNeg) {
          *wsOutput = pLocale->GetMinusSymbol() + *wsOutput;
          bAddNeg = true;
        } else {
          wsOutput->InsertAtFront(L' ');
        }
        ccf--;
        break;
      case 's':
        if (bNeg) {
          *wsOutput = pLocale->GetMinusSymbol() + *wsOutput;
          bAddNeg = true;
        }
        ccf--;
        break;
      case 'E': {
        *wsOutput = WideString::Format(L"E%+d", exponent) + *wsOutput;
        ccf--;
        break;
      }
      case '$': {
        *wsOutput = pLocale->GetCurrencySymbol() + *wsOutput;
        ccf--;
        break;
      }
      case 'r':
        if (ccf - 1 >= 0 && strf[ccf - 1] == 'c') {
          if (bNeg)
            *wsOutput = L"CR" + *wsOutput;

          ccf -= 2;
          bAddNeg = true;
        }
        break;
      case 'R':
        if (ccf - 1 >= 0 && strf[ccf - 1] == 'C') {
          *wsOutput = bNeg ? L"CR" : L"  " + *wsOutput;
          ccf -= 2;
          bAddNeg = true;
        }
        break;
      case 'b':
        if (ccf - 1 >= 0 && strf[ccf - 1] == 'd') {
          if (bNeg)
            *wsOutput = L"db" + *wsOutput;

          ccf -= 2;
          bAddNeg = true;
        }
        break;
      case 'B':
        if (ccf - 1 >= 0 && strf[ccf - 1] == 'D') {
          *wsOutput = bNeg ? L"DB" : L"  " + *wsOutput;
          ccf -= 2;
          bAddNeg = true;
        }
        break;
      case '%': {
        *wsOutput = pLocale->GetPercentSymbol() + *wsOutput;
        ccf--;
        break;
      }
      case ',':
        if (cc >= 0)
          *wsOutput = wsGroupSymbol + *wsOutput;

        ccf--;
        break;
      case '(':
        wsOutput->InsertAtFront(bNeg ? L'(' : L' ');
        bAddNeg = true;
        ccf--;
        break;
      case ')':
        wsOutput->InsertAtFront(bNeg ? L')' : L' ');
        ccf--;
        break;
      case '\'':
        *wsOutput = GetLiteralTextReverse(strf, &ccf) + *wsOutput;
        ccf--;
        break;
      default:
        wsOutput->InsertAtFront(strf[ccf]);
        ccf--;
    }
  }

  if (cc >= 0) {
    int nPos = dot_index.value() % 3;
    wsOutput->clear();
    for (int32_t i = 0;
         i < pdfium::base::checked_cast<int32_t>(dot_index.value()); i++) {
      if (i % 3 == nPos && i != 0)
        *wsOutput += wsGroupSymbol;
      *wsOutput += wsSrcNum[i];
    }
    if (pdfium::base::checked_cast<int32_t>(dot_index.value()) < len) {
      *wsOutput += pLocale->GetDecimalSymbol();
      *wsOutput += wsSrcNum.Right(len - dot_index.value() - 1);
    }
    if (bNeg)
      *wsOutput = pLocale->GetMinusSymbol() + *wsOutput;

    return false;
  }
  if (dot_index_f ==
      pdfium::base::checked_cast<int32_t>(wsNumFormat.GetLength())) {
    if (!bAddNeg && bNeg)
      *wsOutput = pLocale->GetMinusSymbol() + *wsOutput;

    return true;
  }

  WideString wsDotSymbol = pLocale->GetDecimalSymbol();
  if (strf[dot_index_f] == 'V') {
    *wsOutput += wsDotSymbol;
  } else if (strf[dot_index_f] == '.') {
    if (pdfium::base::checked_cast<int32_t>(dot_index.value()) < len)
      *wsOutput += wsDotSymbol;
    else if (strf[dot_index_f + 1] == '9' || strf[dot_index_f + 1] == 'Z')
      *wsOutput += wsDotSymbol;
  }

  ccf = dot_index_f + 1;
  cc = dot_index.value() + 1;
  while (ccf < lenf) {
    switch (strf[ccf]) {
      case '\'':
        *wsOutput += GetLiteralText(strf, &ccf, lenf);
        ccf++;
        break;
      case '9':
        if (cc < len) {
          if (!FXSYS_IsDecimalDigit(str[cc]))
            return false;

          *wsOutput += str[cc];
          cc++;
        } else {
          *wsOutput += L'0';
        }
        ccf++;
        break;
      case 'z':
        if (cc < len) {
          if (!FXSYS_IsDecimalDigit(str[cc]))
            return false;

          *wsOutput += str[cc];
          cc++;
        }
        ccf++;
        break;
      case 'Z':
        if (cc < len) {
          if (!FXSYS_IsDecimalDigit(str[cc]))
            return false;

          *wsOutput += str[cc];
          cc++;
        } else {
          *wsOutput += L'0';
        }
        ccf++;
        break;
      case 'E': {
        *wsOutput += WideString::Format(L"E%+d", exponent);
        ccf++;
        break;
      }
      case '$':
        *wsOutput += pLocale->GetCurrencySymbol();
        ccf++;
        break;
      case 'c':
        if (ccf + 1 < lenf && strf[ccf + 1] == 'r') {
          if (bNeg)
            *wsOutput += L"CR";

          ccf += 2;
          bAddNeg = true;
        }
        break;
      case 'C':
        if (ccf + 1 < lenf && strf[ccf + 1] == 'R') {
          *wsOutput += bNeg ? L"CR" : L"  ";
          ccf += 2;
          bAddNeg = true;
        }
        break;
      case 'd':
        if (ccf + 1 < lenf && strf[ccf + 1] == 'b') {
          if (bNeg)
            *wsOutput += L"db";

          ccf += 2;
          bAddNeg = true;
        }
        break;
      case 'D':
        if (ccf + 1 < lenf && strf[ccf + 1] == 'B') {
          *wsOutput += bNeg ? L"DB" : L"  ";
          ccf += 2;
          bAddNeg = true;
        }
        break;
      case '%':
        *wsOutput += pLocale->GetPercentSymbol();
        ccf++;
        break;
      case '8':
        while (ccf < lenf && strf[ccf] == '8')
          ccf++;
        while (cc < len && FXSYS_IsDecimalDigit(str[cc])) {
          *wsOutput += str[cc];
          cc++;
        }
        break;
      case ',':
        *wsOutput += wsGroupSymbol;
        ccf++;
        break;
      case '(':
        *wsOutput += bNeg ? '(' : ' ';
        bAddNeg = true;
        ccf++;
        break;
      case ')':
        *wsOutput += bNeg ? ')' : ' ';
        ccf++;
        break;
      default:
        ccf++;
    }
  }
  if (!bAddNeg && bNeg) {
    *wsOutput = pLocale->GetMinusSymbol() + (*wsOutput)[0] +
                wsOutput->Right(wsOutput->GetLength() - 1);
  }
  return true;
}

bool CFGAS_FormatString::FormatNum(const WideString& wsSrcNum,
                                   const WideString& wsPattern,
                                   WideString* wsOutput) const {
  if (wsSrcNum.IsEmpty() || wsPattern.IsEmpty())
    return false;
  return FormatStrNum(wsSrcNum.AsStringView(), wsPattern, wsOutput);
}

bool CFGAS_FormatString::FormatDateTime(const WideString& wsSrcDateTime,
                                        const WideString& wsPattern,
                                        FX_DATETIMETYPE eDateTimeType,
                                        WideString* wsOutput) const {
  if (wsSrcDateTime.IsEmpty() || wsPattern.IsEmpty())
    return false;

  WideString wsDatePattern;
  WideString wsTimePattern;
  LocaleIface* pLocale = nullptr;
  FX_DATETIMETYPE eCategory =
      GetDateTimeFormat(wsPattern, &pLocale, &wsDatePattern, &wsTimePattern);
  if (!pLocale)
    return false;

  if (eCategory == FX_DATETIMETYPE_Unknown) {
    if (eDateTimeType == FX_DATETIMETYPE_Time)
      wsTimePattern = std::move(wsDatePattern);

    eCategory = eDateTimeType;
    if (eCategory == FX_DATETIMETYPE_Unknown)
      return false;
  }

  CFX_DateTime dt;
  auto iT = wsSrcDateTime.Find(L"T");
  if (!iT.has_value()) {
    if (eCategory == FX_DATETIMETYPE_Date &&
        FX_DateFromCanonical(wsSrcDateTime, &dt)) {
      *wsOutput = FormatDateTimeInternal(dt, wsDatePattern, wsTimePattern, true,
                                         pLocale);
      return true;
    }
    if (eCategory == FX_DATETIMETYPE_Time &&
        FX_TimeFromCanonical(wsSrcDateTime.AsStringView(), &dt, pLocale)) {
      *wsOutput = FormatDateTimeInternal(dt, wsDatePattern, wsTimePattern, true,
                                         pLocale);
      return true;
    }
  } else {
    WideString wsSrcDate(wsSrcDateTime.c_str(), iT.value());
    WideStringView wsSrcTime(wsSrcDateTime.c_str() + iT.value() + 1,
                             wsSrcDateTime.GetLength() - iT.value() - 1);
    if (wsSrcDate.IsEmpty() || wsSrcTime.IsEmpty())
      return false;
    if (FX_DateFromCanonical(wsSrcDate, &dt) &&
        FX_TimeFromCanonical(wsSrcTime, &dt, pLocale)) {
      *wsOutput = FormatDateTimeInternal(dt, wsDatePattern, wsTimePattern,
                                         eCategory != FX_DATETIMETYPE_TimeDate,
                                         pLocale);
      return true;
    }
  }
  return false;
}

bool CFGAS_FormatString::FormatZero(const WideString& wsPattern,
                                    WideString* wsOutput) const {
  if (wsPattern.IsEmpty())
    return false;

  WideString wsTextFormat = GetTextFormat(wsPattern, L"zero");
  int32_t iPattern = 0;
  const wchar_t* pStrPattern = wsTextFormat.c_str();
  int32_t iLenPattern = wsTextFormat.GetLength();
  while (iPattern < iLenPattern) {
    if (pStrPattern[iPattern] == '\'') {
      *wsOutput += GetLiteralText(pStrPattern, &iPattern, iLenPattern);
      iPattern++;
    } else {
      *wsOutput += pStrPattern[iPattern++];
    }
  }
  return true;
}

bool CFGAS_FormatString::FormatNull(const WideString& wsPattern,
                                    WideString* wsOutput) const {
  if (wsPattern.IsEmpty())
    return false;

  WideString wsTextFormat = GetTextFormat(wsPattern, L"null");
  int32_t iPattern = 0;
  const wchar_t* pStrPattern = wsTextFormat.c_str();
  int32_t iLenPattern = wsTextFormat.GetLength();
  while (iPattern < iLenPattern) {
    if (pStrPattern[iPattern] == '\'') {
      *wsOutput += GetLiteralText(pStrPattern, &iPattern, iLenPattern);
      iPattern++;
      continue;
    }
    *wsOutput += pStrPattern[iPattern++];
  }
  return true;
}
