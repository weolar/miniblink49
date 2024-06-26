// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSCOMPUTEDSTYLE_H_
#define CORE_FXCRT_CSS_CFX_CSSCOMPUTEDSTYLE_H_

#include <vector>

#include "core/fxcrt/css/cfx_css.h"
#include "core/fxcrt/css/cfx_csscustomproperty.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxge/fx_dib.h"

class CFX_CSSValueList;

class CFX_CSSComputedStyle final : public Retainable {
 public:
  class InheritedData {
   public:
    InheritedData();
    ~InheritedData();

    CFX_CSSLength m_LetterSpacing;
    CFX_CSSLength m_WordSpacing;
    CFX_CSSLength m_TextIndent;
    RetainPtr<CFX_CSSValueList> m_pFontFamily;
    float m_fFontSize;
    float m_fLineHeight;
    FX_ARGB m_dwFontColor;
    uint16_t m_wFontWeight;
    CFX_CSSFontVariant m_eFontVariant;
    CFX_CSSFontStyle m_eFontStyle;
    CFX_CSSTextAlign m_eTextAlign;
  };

  class NonInheritedData {
   public:
    NonInheritedData();

    CFX_CSSRect m_MarginWidth;
    CFX_CSSRect m_BorderWidth;
    CFX_CSSRect m_PaddingWidth;
    CFX_CSSLength m_Top;
    CFX_CSSLength m_Bottom;
    CFX_CSSLength m_Left;
    CFX_CSSLength m_Right;
    float m_fVerticalAlign;
    CFX_CSSDisplay m_eDisplay;
    CFX_CSSVerticalAlign m_eVerticalAlign;
    uint8_t m_dwTextDecoration;
    bool m_bHasMargin;
    bool m_bHasBorder;
    bool m_bHasPadding;
  };

  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  int32_t CountFontFamilies() const;
  const WideString GetFontFamily(int32_t index) const;
  uint16_t GetFontWeight() const;
  CFX_CSSFontVariant GetFontVariant() const;
  CFX_CSSFontStyle GetFontStyle() const;
  float GetFontSize() const;
  FX_ARGB GetColor() const;
  void SetFontWeight(uint16_t wFontWeight);
  void SetFontVariant(CFX_CSSFontVariant eFontVariant);
  void SetFontStyle(CFX_CSSFontStyle eFontStyle);
  void SetFontSize(float fFontSize);
  void SetColor(FX_ARGB dwFontColor);

  const CFX_CSSRect* GetBorderWidth() const;
  const CFX_CSSRect* GetMarginWidth() const;
  const CFX_CSSRect* GetPaddingWidth() const;
  void SetMarginWidth(const CFX_CSSRect& rect);
  void SetPaddingWidth(const CFX_CSSRect& rect);

  CFX_CSSDisplay GetDisplay() const;

  float GetLineHeight() const;
  const CFX_CSSLength& GetTextIndent() const;
  CFX_CSSTextAlign GetTextAlign() const;
  CFX_CSSVerticalAlign GetVerticalAlign() const;
  float GetNumberVerticalAlign() const;
  uint32_t GetTextDecoration() const;
  const CFX_CSSLength& GetLetterSpacing() const;
  void SetLineHeight(float fLineHeight);
  void SetTextIndent(const CFX_CSSLength& textIndent);
  void SetTextAlign(CFX_CSSTextAlign eTextAlign);
  void SetNumberVerticalAlign(float fAlign);
  void SetTextDecoration(uint32_t dwTextDecoration);
  void SetLetterSpacing(const CFX_CSSLength& letterSpacing);
  void AddCustomStyle(const CFX_CSSCustomProperty& prop);

  bool GetCustomStyle(const WideString& wsName, WideString* pValue) const;

  InheritedData m_InheritedData;
  NonInheritedData m_NonInheritedData;

 private:
  CFX_CSSComputedStyle();
  ~CFX_CSSComputedStyle() override;

  std::vector<CFX_CSSCustomProperty> m_CustomProperties;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSCOMPUTEDSTYLE_H_
