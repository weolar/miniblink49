// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_XMLLOCALE_H_
#define XFA_FXFA_PARSER_CXFA_XMLLOCALE_H_

#include <memory>

#include "third_party/base/span.h"
#include "xfa/fgas/crt/locale_iface.h"

class CFX_XMLDocument;
class CFX_XMLElement;

class CXFA_XMLLocale final : public LocaleIface {
 public:
  static std::unique_ptr<CXFA_XMLLocale> Create(pdfium::span<uint8_t> data);

  explicit CXFA_XMLLocale(std::unique_ptr<CFX_XMLDocument> root,
                          CFX_XMLElement* locale);
  ~CXFA_XMLLocale() override;

  // LocaleIface
  WideString GetName() const override;
  WideString GetDecimalSymbol() const override;
  WideString GetGroupingSymbol() const override;
  WideString GetPercentSymbol() const override;
  WideString GetMinusSymbol() const override;
  WideString GetCurrencySymbol() const override;
  WideString GetDateTimeSymbols() const override;
  WideString GetMonthName(int32_t nMonth, bool bAbbr) const override;
  WideString GetDayName(int32_t nWeek, bool bAbbr) const override;
  WideString GetMeridiemName(bool bAM) const override;
  FX_TIMEZONE GetTimeZone() const override;
  WideString GetEraName(bool bAD) const override;

  WideString GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY eType) const override;
  WideString GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY eType) const override;
  WideString GetNumPattern(FX_LOCALENUMSUBCATEGORY eType) const override;

 private:
  WideString GetPattern(CFX_XMLElement* pElement,
                        WideStringView bsTag,
                        WideStringView wsName) const;
  WideString GetCalendarSymbol(WideStringView symbol,
                               size_t index,
                               bool bAbbr) const;

  std::unique_ptr<CFX_XMLDocument> xml_doc_;
  UnownedPtr<CFX_XMLElement> locale_;
};

#endif  // XFA_FXFA_PARSER_CXFA_XMLLOCALE_H_
