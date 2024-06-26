// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_CRT_CFGAS_FORMATSTRING_H_
#define XFA_FGAS_CRT_CFGAS_FORMATSTRING_H_

#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fgas/crt/locale_iface.h"

class CXFA_LocaleMgr;

bool FX_DateFromCanonical(const WideString& wsDate, CFX_DateTime* datetime);
bool FX_TimeFromCanonical(WideStringView wsTime,
                          CFX_DateTime* datetime,
                          LocaleIface* pLocale);

class CFGAS_FormatString {
 public:
  explicit CFGAS_FormatString(CXFA_LocaleMgr* pLocaleMgr);
  ~CFGAS_FormatString();

  void SplitFormatString(const WideString& wsFormatString,
                         std::vector<WideString>* wsPatterns) const;
  FX_LOCALECATEGORY GetCategory(const WideString& wsPattern) const;

  bool ParseText(const WideString& wsSrcText,
                 const WideString& wsPattern,
                 WideString* wsValue) const;
  bool ParseNum(const WideString& wsSrcNum,
                const WideString& wsPattern,
                WideString* wsValue) const;
  bool ParseDateTime(const WideString& wsSrcDateTime,
                     const WideString& wsPattern,
                     FX_DATETIMETYPE eDateTimeType,
                     CFX_DateTime* dtValue) const;
  bool ParseZero(const WideString& wsSrcText,
                 const WideString& wsPattern) const;
  bool ParseNull(const WideString& wsSrcText,
                 const WideString& wsPattern) const;

  bool FormatText(const WideString& wsSrcText,
                  const WideString& wsPattern,
                  WideString* wsOutput) const;
  bool FormatNum(const WideString& wsSrcNum,
                 const WideString& wsPattern,
                 WideString* wsOutput) const;
  bool FormatDateTime(const WideString& wsSrcDateTime,
                      const WideString& wsPattern,
                      FX_DATETIMETYPE eDateTimeType,
                      WideString* wsOutput) const;
  bool FormatZero(const WideString& wsPattern, WideString* wsOutput) const;
  bool FormatNull(const WideString& wsPattern, WideString* wsOutput) const;

 private:
  WideString GetTextFormat(const WideString& wsPattern,
                           WideStringView wsCategory) const;
  LocaleIface* GetNumericFormat(const WideString& wsPattern,
                                int32_t* iDotIndex,
                                uint32_t* dwStyle,
                                WideString* wsPurgePattern) const;
  bool FormatStrNum(WideStringView wsInputNum,
                    const WideString& wsPattern,
                    WideString* wsOutput) const;
  FX_DATETIMETYPE GetDateTimeFormat(const WideString& wsPattern,
                                    LocaleIface** pLocale,
                                    WideString* wsDatePattern,
                                    WideString* wsTimePattern) const;

  UnownedPtr<CXFA_LocaleMgr> const m_pLocaleMgr;
};

#endif  // XFA_FGAS_CRT_CFGAS_FORMATSTRING_H_
