// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_CRT_LOCALE_IFACE_H_
#define XFA_FGAS_CRT_LOCALE_IFACE_H_

#include "core/fxcrt/cfx_datetime.h"
#include "core/fxcrt/fx_string.h"

enum FX_LOCALEDATETIMESUBCATEGORY {
  FX_LOCALEDATETIMESUBCATEGORY_Default,
  FX_LOCALEDATETIMESUBCATEGORY_Short,
  FX_LOCALEDATETIMESUBCATEGORY_Medium,
  FX_LOCALEDATETIMESUBCATEGORY_Full,
  FX_LOCALEDATETIMESUBCATEGORY_Long,
};

enum FX_LOCALENUMSUBCATEGORY {
  FX_LOCALENUMPATTERN_Percent,
  FX_LOCALENUMPATTERN_Currency,
  FX_LOCALENUMPATTERN_Decimal,
  FX_LOCALENUMPATTERN_Integer,
};

enum FX_LOCALECATEGORY {
  FX_LOCALECATEGORY_Unknown,
  FX_LOCALECATEGORY_Date,
  FX_LOCALECATEGORY_Time,
  FX_LOCALECATEGORY_DateTime,
  FX_LOCALECATEGORY_Num,
  FX_LOCALECATEGORY_Text,
  FX_LOCALECATEGORY_Zero,
  FX_LOCALECATEGORY_Null,
};

enum FX_DATETIMETYPE {
  FX_DATETIMETYPE_Unknown,
  FX_DATETIMETYPE_Date,
  FX_DATETIMETYPE_Time,
  FX_DATETIMETYPE_DateTime,
  FX_DATETIMETYPE_TimeDate,
};

class LocaleIface {
 public:
  virtual ~LocaleIface() = default;

  virtual WideString GetName() const = 0;
  virtual WideString GetDecimalSymbol() const = 0;
  virtual WideString GetGroupingSymbol() const = 0;
  virtual WideString GetPercentSymbol() const = 0;
  virtual WideString GetMinusSymbol() const = 0;
  virtual WideString GetCurrencySymbol() const = 0;
  virtual WideString GetDateTimeSymbols() const = 0;
  virtual WideString GetMonthName(int32_t nMonth, bool bAbbr) const = 0;
  virtual WideString GetDayName(int32_t nWeek, bool bAbbr) const = 0;
  virtual WideString GetMeridiemName(bool bAM) const = 0;
  virtual FX_TIMEZONE GetTimeZone() const = 0;
  virtual WideString GetEraName(bool bAD) const = 0;
  virtual WideString GetDatePattern(
      FX_LOCALEDATETIMESUBCATEGORY eType) const = 0;
  virtual WideString GetTimePattern(
      FX_LOCALEDATETIMESUBCATEGORY eType) const = 0;
  virtual WideString GetNumPattern(FX_LOCALENUMSUBCATEGORY eType) const = 0;
};

#endif  // XFA_FGAS_CRT_LOCALE_IFACE_H_
