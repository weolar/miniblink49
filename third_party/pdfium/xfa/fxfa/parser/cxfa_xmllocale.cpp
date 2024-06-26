// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_xmllocale.h"

#include <utility>

#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlparser.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_localemgr.h"
#include "xfa/fxfa/parser/cxfa_nodelocale.h"
#include "xfa/fxfa/parser/cxfa_timezoneprovider.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

constexpr wchar_t kNumberSymbols[] = L"numberSymbols";
constexpr wchar_t kNumberSymbol[] = L"numberSymbol";
constexpr wchar_t kCurrencySymbols[] = L"currencySymbols";
constexpr wchar_t kCurrencySymbol[] = L"currencySymbol";

}  // namespace

// static
std::unique_ptr<CXFA_XMLLocale> CXFA_XMLLocale::Create(
    pdfium::span<uint8_t> data) {
  auto stream = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(data);
  CFX_XMLParser parser(stream);
  auto doc = parser.Parse();
  if (!doc)
    return nullptr;

  CFX_XMLElement* locale = nullptr;
  for (auto* child = doc->GetRoot()->GetFirstChild(); child;
       child = child->GetNextSibling()) {
    CFX_XMLElement* elem = ToXMLElement(child);
    if (elem && elem->GetName().EqualsASCII("locale")) {
      locale = elem;
      break;
    }
  }
  if (!locale)
    return nullptr;

  return pdfium::MakeUnique<CXFA_XMLLocale>(std::move(doc), locale);
}

CXFA_XMLLocale::CXFA_XMLLocale(std::unique_ptr<CFX_XMLDocument> doc,
                               CFX_XMLElement* locale)
    : xml_doc_(std::move(doc)), locale_(locale) {
  ASSERT(xml_doc_);
  ASSERT(locale_);
}

CXFA_XMLLocale::~CXFA_XMLLocale() {}

WideString CXFA_XMLLocale::GetName() const {
  return locale_->GetAttribute(L"name");
}

WideString CXFA_XMLLocale::GetDecimalSymbol() const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(kNumberSymbols);
  return patterns ? GetPattern(patterns, kNumberSymbol, L"decimal")
                  : WideString();
}

WideString CXFA_XMLLocale::GetGroupingSymbol() const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(kNumberSymbols);
  return patterns ? GetPattern(patterns, kNumberSymbol, L"grouping")
                  : WideString();
}

WideString CXFA_XMLLocale::GetPercentSymbol() const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(kNumberSymbols);
  return patterns ? GetPattern(patterns, kNumberSymbol, L"percent")
                  : WideString();
}

WideString CXFA_XMLLocale::GetMinusSymbol() const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(kNumberSymbols);
  return patterns ? GetPattern(patterns, kNumberSymbol, L"minus")
                  : WideString();
}

WideString CXFA_XMLLocale::GetCurrencySymbol() const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(kCurrencySymbols);
  return patterns ? GetPattern(patterns, kCurrencySymbol, L"symbol")
                  : WideString();
}

WideString CXFA_XMLLocale::GetDateTimeSymbols() const {
  CFX_XMLElement* symbols = locale_->GetFirstChildNamed(L"dateTimeSymbols");
  return symbols ? symbols->GetTextData() : WideString();
}

WideString CXFA_XMLLocale::GetMonthName(int32_t nMonth, bool bAbbr) const {
  return GetCalendarSymbol(L"month", nMonth, bAbbr);
}

WideString CXFA_XMLLocale::GetDayName(int32_t nWeek, bool bAbbr) const {
  return GetCalendarSymbol(L"day", nWeek, bAbbr);
}

WideString CXFA_XMLLocale::GetMeridiemName(bool bAM) const {
  return GetCalendarSymbol(L"meridiem", bAM ? 0 : 1, false);
}

FX_TIMEZONE CXFA_XMLLocale::GetTimeZone() const {
  return CXFA_TimeZoneProvider().GetTimeZone();
}

WideString CXFA_XMLLocale::GetEraName(bool bAD) const {
  return GetCalendarSymbol(L"era", bAD ? 1 : 0, false);
}

WideString CXFA_XMLLocale::GetCalendarSymbol(WideStringView symbol,
                                             size_t index,
                                             bool bAbbr) const {
  CFX_XMLElement* child = locale_->GetFirstChildNamed(L"calendarSymbols");
  if (!child)
    return WideString();

  WideString pstrSymbolNames = symbol + L"Names";
  CFX_XMLElement* name_child = nullptr;
  for (auto* name = child->GetFirstChild(); name;
       name = name->GetNextSibling()) {
    CFX_XMLElement* elem = ToXMLElement(name);
    if (!elem || elem->GetName() != pstrSymbolNames)
      continue;

    WideString abbr = elem->GetAttribute(L"abbr");
    bool abbr_value = false;
    if (!abbr.IsEmpty())
      abbr_value = abbr.EqualsASCII("1");
    if (abbr_value != bAbbr)
      continue;

    name_child = elem;
    break;
  }
  if (!name_child)
    return WideString();

  CFX_XMLElement* sym_element = name_child->GetNthChildNamed(symbol, index);
  return sym_element ? sym_element->GetTextData() : WideString();
}

WideString CXFA_XMLLocale::GetDatePattern(
    FX_LOCALEDATETIMESUBCATEGORY eType) const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(L"datePatterns");
  if (!patterns)
    return WideString();

  WideString wsName;
  switch (eType) {
    case FX_LOCALEDATETIMESUBCATEGORY_Short:
      wsName = L"short";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Default:
    case FX_LOCALEDATETIMESUBCATEGORY_Medium:
      wsName = L"med";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Full:
      wsName = L"full";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Long:
      wsName = L"long";
      break;
  }
  return GetPattern(patterns, L"datePattern", wsName.AsStringView());
}

WideString CXFA_XMLLocale::GetTimePattern(
    FX_LOCALEDATETIMESUBCATEGORY eType) const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(L"timePatterns");
  if (!patterns)
    return WideString();

  WideString wsName;
  switch (eType) {
    case FX_LOCALEDATETIMESUBCATEGORY_Short:
      wsName = L"short";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Default:
    case FX_LOCALEDATETIMESUBCATEGORY_Medium:
      wsName = L"med";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Full:
      wsName = L"full";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Long:
      wsName = L"long";
      break;
  }
  return GetPattern(patterns, L"timePattern", wsName.AsStringView());
}

WideString CXFA_XMLLocale::GetNumPattern(FX_LOCALENUMSUBCATEGORY eType) const {
  CFX_XMLElement* patterns = locale_->GetFirstChildNamed(L"numberPatterns");
  return patterns ? XFA_PatternToString(eType) : WideString();
}

WideString CXFA_XMLLocale::GetPattern(CFX_XMLElement* patterns,
                                      WideStringView bsTag,
                                      WideStringView wsName) const {
  for (auto* child = patterns->GetFirstChild(); child;
       child = child->GetNextSibling()) {
    CFX_XMLElement* pattern = ToXMLElement(child);
    if (pattern && pattern->GetName() == bsTag &&
        pattern->GetAttribute(L"name") == wsName) {
      return pattern->GetTextData();
    }
  }
  return WideString();
}
