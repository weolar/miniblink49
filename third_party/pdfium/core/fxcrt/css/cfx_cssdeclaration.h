// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSDECLARATION_H_
#define CORE_FXCRT_CSS_CFX_CSSDECLARATION_H_

#include <memory>
#include <vector>

#include "core/fxcrt/css/cfx_cssdata.h"

class CFX_CSSPropertyHolder;
class CFX_CSSCustomProperty;

class CFX_CSSDeclaration {
 public:
  using const_prop_iterator =
      std::vector<std::unique_ptr<CFX_CSSPropertyHolder>>::const_iterator;
  using const_custom_iterator =
      std::vector<std::unique_ptr<CFX_CSSCustomProperty>>::const_iterator;

  static bool ParseCSSString(const wchar_t* pszValue,
                             int32_t iValueLen,
                             int32_t* iOffset,
                             int32_t* iLength);
  static bool ParseCSSColor(const wchar_t* pszValue,
                            int32_t iValueLen,
                            FX_ARGB* dwColor);

  CFX_CSSDeclaration();
  ~CFX_CSSDeclaration();

  RetainPtr<CFX_CSSValue> GetProperty(CFX_CSSProperty eProperty,
                                      bool* bImportant) const;

  const_prop_iterator begin() const { return properties_.begin(); }
  const_prop_iterator end() const { return properties_.end(); }

  const_custom_iterator custom_begin() const {
    return custom_properties_.begin();
  }
  const_custom_iterator custom_end() const { return custom_properties_.end(); }

  bool empty() const { return properties_.empty(); }

  void AddProperty(const CFX_CSSData::Property* property, WideStringView value);
  void AddProperty(const WideString& prop, const WideString& value);

  size_t PropertyCountForTesting() const;

  FX_ARGB ParseColorForTest(const wchar_t* pszValue,
                            int32_t iValueLen,
                            FX_ARGB* dwColor) const;

 private:
  void ParseFontProperty(const wchar_t* pszValue,
                         int32_t iValueLen,
                         bool bImportant);
  bool ParseBorderProperty(const wchar_t* pszValue,
                           int32_t iValueLen,
                           RetainPtr<CFX_CSSValue>& pWidth) const;
  void ParseValueListProperty(const CFX_CSSData::Property* pProperty,
                              const wchar_t* pszValue,
                              int32_t iValueLen,
                              bool bImportant);
  void Add4ValuesProperty(const std::vector<RetainPtr<CFX_CSSValue>>& list,
                          bool bImportant,
                          CFX_CSSProperty eLeft,
                          CFX_CSSProperty eTop,
                          CFX_CSSProperty eRight,
                          CFX_CSSProperty eBottom);
  RetainPtr<CFX_CSSValue> ParseNumber(const wchar_t* pszValue,
                                      int32_t iValueLen);
  RetainPtr<CFX_CSSValue> ParseEnum(const wchar_t* pszValue, int32_t iValueLen);
  RetainPtr<CFX_CSSValue> ParseColor(const wchar_t* pszValue,
                                     int32_t iValueLen);
  RetainPtr<CFX_CSSValue> ParseString(const wchar_t* pszValue,
                                      int32_t iValueLen);
  void AddPropertyHolder(CFX_CSSProperty eProperty,
                         RetainPtr<CFX_CSSValue> pValue,
                         bool bImportant);

  std::vector<std::unique_ptr<CFX_CSSPropertyHolder>> properties_;
  std::vector<std::unique_ptr<CFX_CSSCustomProperty>> custom_properties_;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSDECLARATION_H_
