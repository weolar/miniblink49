// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_textstate.h"

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/parser/cpdf_document.h"

CPDF_TextState::CPDF_TextState() {}
CPDF_TextState::~CPDF_TextState() {}

void CPDF_TextState::Emplace() {
  m_Ref.Emplace();
}

CPDF_Font* CPDF_TextState::GetFont() const {
  return m_Ref.GetObject()->m_pFont;
}

void CPDF_TextState::SetFont(CPDF_Font* pFont) {
  m_Ref.GetPrivateCopy()->SetFont(pFont);
}

float CPDF_TextState::GetFontSize() const {
  return m_Ref.GetObject()->m_FontSize;
}

void CPDF_TextState::SetFontSize(float size) {
  m_Ref.GetPrivateCopy()->m_FontSize = size;
}

const float* CPDF_TextState::GetMatrix() const {
  return m_Ref.GetObject()->m_Matrix;
}

float* CPDF_TextState::GetMutableMatrix() {
  return m_Ref.GetPrivateCopy()->m_Matrix;
}

float CPDF_TextState::GetCharSpace() const {
  return m_Ref.GetObject()->m_CharSpace;
}

void CPDF_TextState::SetCharSpace(float sp) {
  m_Ref.GetPrivateCopy()->m_CharSpace = sp;
}

float CPDF_TextState::GetWordSpace() const {
  return m_Ref.GetObject()->m_WordSpace;
}

void CPDF_TextState::SetWordSpace(float sp) {
  m_Ref.GetPrivateCopy()->m_WordSpace = sp;
}

float CPDF_TextState::GetFontSizeV() const {
  return m_Ref.GetObject()->GetFontSizeV();
}

float CPDF_TextState::GetFontSizeH() const {
  return m_Ref.GetObject()->GetFontSizeH();
}

float CPDF_TextState::GetBaselineAngle() const {
  return m_Ref.GetObject()->GetBaselineAngle();
}

float CPDF_TextState::GetShearAngle() const {
  return m_Ref.GetObject()->GetShearAngle();
}

TextRenderingMode CPDF_TextState::GetTextMode() const {
  return m_Ref.GetObject()->m_TextMode;
}

void CPDF_TextState::SetTextMode(TextRenderingMode mode) {
  m_Ref.GetPrivateCopy()->m_TextMode = mode;
}

const float* CPDF_TextState::GetCTM() const {
  return m_Ref.GetObject()->m_CTM;
}

float* CPDF_TextState::GetMutableCTM() {
  return m_Ref.GetPrivateCopy()->m_CTM;
}

CPDF_TextState::TextData::TextData()
    : m_pFont(nullptr),
      m_pDocument(nullptr),
      m_FontSize(1.0f),
      m_CharSpace(0),
      m_WordSpace(0),
      m_TextMode(TextRenderingMode::MODE_FILL) {
  m_Matrix[0] = m_Matrix[3] = 1.0f;
  m_Matrix[1] = m_Matrix[2] = 0;
  m_CTM[0] = m_CTM[3] = 1.0f;
  m_CTM[1] = m_CTM[2] = 0;
}

CPDF_TextState::TextData::TextData(const TextData& that)
    : m_pFont(that.m_pFont),
      m_pDocument(that.m_pDocument),
      m_FontSize(that.m_FontSize),
      m_CharSpace(that.m_CharSpace),
      m_WordSpace(that.m_WordSpace),
      m_TextMode(that.m_TextMode) {
  for (int i = 0; i < 4; ++i)
    m_Matrix[i] = that.m_Matrix[i];

  for (int i = 0; i < 4; ++i)
    m_CTM[i] = that.m_CTM[i];

  if (m_pDocument && m_pFont)
    m_pFont = m_pDocument->GetPageData()->GetFont(m_pFont->GetFontDict());
}

CPDF_TextState::TextData::~TextData() {
  ReleaseFont();
}

void CPDF_TextState::TextData::SetFont(CPDF_Font* pFont) {
  ReleaseFont();
  m_pDocument = pFont ? pFont->GetDocument() : nullptr;
  m_pFont = pFont;
}

float CPDF_TextState::TextData::GetFontSizeV() const {
  return fabs(FXSYS_sqrt2(m_Matrix[1], m_Matrix[3]) * m_FontSize);
}

float CPDF_TextState::TextData::GetFontSizeH() const {
  return fabs(FXSYS_sqrt2(m_Matrix[0], m_Matrix[2]) * m_FontSize);
}

float CPDF_TextState::TextData::GetBaselineAngle() const {
  return atan2(m_Matrix[2], m_Matrix[0]);
}

float CPDF_TextState::TextData::GetShearAngle() const {
  return GetBaselineAngle() + atan2(m_Matrix[1], m_Matrix[3]);
}

void CPDF_TextState::TextData::ReleaseFont() {
  if (!m_pDocument || !m_pFont)
    return;

  CPDF_DocPageData* pPageData = m_pDocument->GetPageData();
  if (pPageData && !pPageData->IsForceClear())
    pPageData->ReleaseFont(m_pFont->GetFontDict());
}

bool SetTextRenderingModeFromInt(int iMode, TextRenderingMode* mode) {
  if (iMode < 0 || iMode > 7)
    return false;
  *mode = static_cast<TextRenderingMode>(iMode);
  return true;
}

bool TextRenderingModeIsClipMode(const TextRenderingMode& mode) {
  switch (mode) {
    case TextRenderingMode::MODE_FILL_CLIP:
    case TextRenderingMode::MODE_STROKE_CLIP:
    case TextRenderingMode::MODE_FILL_STROKE_CLIP:
    case TextRenderingMode::MODE_CLIP:
      return true;
    default:
      return false;
  }
}

bool TextRenderingModeIsStrokeMode(const TextRenderingMode& mode) {
  switch (mode) {
    case TextRenderingMode::MODE_STROKE:
    case TextRenderingMode::MODE_FILL_STROKE:
    case TextRenderingMode::MODE_STROKE_CLIP:
    case TextRenderingMode::MODE_FILL_STROKE_CLIP:
      return true;
    default:
      return false;
  }
}
