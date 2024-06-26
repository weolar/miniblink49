// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_csscomputedstyle.h"

#include "core/fxcrt/css/cfx_cssstringvalue.h"
#include "core/fxcrt/css/cfx_cssvaluelist.h"

CFX_CSSComputedStyle::CFX_CSSComputedStyle() = default;

CFX_CSSComputedStyle::~CFX_CSSComputedStyle() = default;

bool CFX_CSSComputedStyle::GetCustomStyle(const WideString& wsName,
                                          WideString* pValue) const {
  for (auto iter = m_CustomProperties.rbegin();
       iter != m_CustomProperties.rend(); ++iter) {
    if (wsName == iter->name()) {
      *pValue = iter->value();
      return true;
    }
  }
  return false;
}

int32_t CFX_CSSComputedStyle::CountFontFamilies() const {
  return m_InheritedData.m_pFontFamily
             ? m_InheritedData.m_pFontFamily->CountValues()
             : 0;
}

const WideString CFX_CSSComputedStyle::GetFontFamily(int32_t index) const {
  return m_InheritedData.m_pFontFamily->GetValue(index)
      .As<CFX_CSSStringValue>()
      ->Value();
}

uint16_t CFX_CSSComputedStyle::GetFontWeight() const {
  return m_InheritedData.m_wFontWeight;
}

CFX_CSSFontVariant CFX_CSSComputedStyle::GetFontVariant() const {
  return m_InheritedData.m_eFontVariant;
}

CFX_CSSFontStyle CFX_CSSComputedStyle::GetFontStyle() const {
  return m_InheritedData.m_eFontStyle;
}

float CFX_CSSComputedStyle::GetFontSize() const {
  return m_InheritedData.m_fFontSize;
}

FX_ARGB CFX_CSSComputedStyle::GetColor() const {
  return m_InheritedData.m_dwFontColor;
}

void CFX_CSSComputedStyle::SetFontWeight(uint16_t wFontWeight) {
  m_InheritedData.m_wFontWeight = wFontWeight;
}

void CFX_CSSComputedStyle::SetFontVariant(CFX_CSSFontVariant eFontVariant) {
  m_InheritedData.m_eFontVariant = eFontVariant;
}

void CFX_CSSComputedStyle::SetFontStyle(CFX_CSSFontStyle eFontStyle) {
  m_InheritedData.m_eFontStyle = eFontStyle;
}

void CFX_CSSComputedStyle::SetFontSize(float fFontSize) {
  m_InheritedData.m_fFontSize = fFontSize;
}

void CFX_CSSComputedStyle::SetColor(FX_ARGB dwFontColor) {
  m_InheritedData.m_dwFontColor = dwFontColor;
}

const CFX_CSSRect* CFX_CSSComputedStyle::GetBorderWidth() const {
  return m_NonInheritedData.m_bHasBorder ? &(m_NonInheritedData.m_BorderWidth)
                                         : nullptr;
}

const CFX_CSSRect* CFX_CSSComputedStyle::GetMarginWidth() const {
  return m_NonInheritedData.m_bHasMargin ? &(m_NonInheritedData.m_MarginWidth)
                                         : nullptr;
}

const CFX_CSSRect* CFX_CSSComputedStyle::GetPaddingWidth() const {
  return m_NonInheritedData.m_bHasPadding ? &(m_NonInheritedData.m_PaddingWidth)
                                          : nullptr;
}

void CFX_CSSComputedStyle::SetMarginWidth(const CFX_CSSRect& rect) {
  m_NonInheritedData.m_MarginWidth = rect;
  m_NonInheritedData.m_bHasMargin = true;
}

void CFX_CSSComputedStyle::SetPaddingWidth(const CFX_CSSRect& rect) {
  m_NonInheritedData.m_PaddingWidth = rect;
  m_NonInheritedData.m_bHasPadding = true;
}

CFX_CSSDisplay CFX_CSSComputedStyle::GetDisplay() const {
  return m_NonInheritedData.m_eDisplay;
}

float CFX_CSSComputedStyle::GetLineHeight() const {
  return m_InheritedData.m_fLineHeight;
}

const CFX_CSSLength& CFX_CSSComputedStyle::GetTextIndent() const {
  return m_InheritedData.m_TextIndent;
}

CFX_CSSTextAlign CFX_CSSComputedStyle::GetTextAlign() const {
  return m_InheritedData.m_eTextAlign;
}

CFX_CSSVerticalAlign CFX_CSSComputedStyle::GetVerticalAlign() const {
  return m_NonInheritedData.m_eVerticalAlign;
}

float CFX_CSSComputedStyle::GetNumberVerticalAlign() const {
  return m_NonInheritedData.m_fVerticalAlign;
}

uint32_t CFX_CSSComputedStyle::GetTextDecoration() const {
  return m_NonInheritedData.m_dwTextDecoration;
}

const CFX_CSSLength& CFX_CSSComputedStyle::GetLetterSpacing() const {
  return m_InheritedData.m_LetterSpacing;
}

void CFX_CSSComputedStyle::SetLineHeight(float fLineHeight) {
  m_InheritedData.m_fLineHeight = fLineHeight;
}

void CFX_CSSComputedStyle::SetTextIndent(const CFX_CSSLength& textIndent) {
  m_InheritedData.m_TextIndent = textIndent;
}

void CFX_CSSComputedStyle::SetTextAlign(CFX_CSSTextAlign eTextAlign) {
  m_InheritedData.m_eTextAlign = eTextAlign;
}

void CFX_CSSComputedStyle::SetNumberVerticalAlign(float fAlign) {
  m_NonInheritedData.m_eVerticalAlign = CFX_CSSVerticalAlign::Number,
  m_NonInheritedData.m_fVerticalAlign = fAlign;
}

void CFX_CSSComputedStyle::SetTextDecoration(uint32_t dwTextDecoration) {
  m_NonInheritedData.m_dwTextDecoration = dwTextDecoration;
}

void CFX_CSSComputedStyle::SetLetterSpacing(
    const CFX_CSSLength& letterSpacing) {
  m_InheritedData.m_LetterSpacing = letterSpacing;
}

void CFX_CSSComputedStyle::AddCustomStyle(const CFX_CSSCustomProperty& prop) {
  // Force the property to be copied so we aren't dependent on the lifetime
  // of whatever currently owns it.
  m_CustomProperties.push_back(prop);
}

CFX_CSSComputedStyle::InheritedData::InheritedData()
    : m_LetterSpacing(CFX_CSSLengthUnit::Normal, 0),
      m_WordSpacing(CFX_CSSLengthUnit::Normal, 0),
      m_TextIndent(CFX_CSSLengthUnit::Point, 0),
      m_pFontFamily(nullptr),
      m_fFontSize(12.0f),
      m_fLineHeight(14.0f),
      m_dwFontColor(0xFF000000),
      m_wFontWeight(400),
      m_eFontVariant(CFX_CSSFontVariant::Normal),
      m_eFontStyle(CFX_CSSFontStyle::Normal),
      m_eTextAlign(CFX_CSSTextAlign::Left) {}

CFX_CSSComputedStyle::InheritedData::~InheritedData() {}

CFX_CSSComputedStyle::NonInheritedData::NonInheritedData()
    : m_MarginWidth(CFX_CSSLengthUnit::Point, 0),
      m_BorderWidth(CFX_CSSLengthUnit::Point, 0),
      m_PaddingWidth(CFX_CSSLengthUnit::Point, 0),
      m_fVerticalAlign(0.0f),
      m_eDisplay(CFX_CSSDisplay::Inline),
      m_eVerticalAlign(CFX_CSSVerticalAlign::Baseline),
      m_dwTextDecoration(0),
      m_bHasMargin(false),
      m_bHasBorder(false),
      m_bHasPadding(false) {}
