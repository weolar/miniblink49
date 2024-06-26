// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cpdf_type3char.h"

#include <utility>

#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_imageobject.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_dib.h"

namespace {

constexpr float kTextUnitInGlyphUnit = 1000.0f;

}  // namespace

CPDF_Type3Char::CPDF_Type3Char(std::unique_ptr<CPDF_Form> pForm)
    : m_pForm(std::move(pForm)) {}

CPDF_Type3Char::~CPDF_Type3Char() {}

// static
float CPDF_Type3Char::TextUnitToGlyphUnit(float fTextUnit) {
  return fTextUnit * kTextUnitInGlyphUnit;
}

// static
void CPDF_Type3Char::TextUnitRectToGlyphUnitRect(CFX_FloatRect* pRect) {
  pRect->Scale(kTextUnitInGlyphUnit);
}

bool CPDF_Type3Char::LoadBitmap(CPDF_RenderContext* pContext) {
  if (m_pBitmap || !m_pForm)
    return true;

  if (m_pForm->GetPageObjectList()->size() != 1 || m_bColored)
    return false;

  auto& pPageObj = m_pForm->GetPageObjectList()->front();
  if (!pPageObj->IsImage())
    return false;

  m_ImageMatrix = pPageObj->AsImage()->matrix();
  {
    // |pSource| actually gets assigned a CPDF_DIBBase, which has pointers
    // into objects owned by |m_pForm|. Make sure it is out of scope before
    // clearing the form.
    RetainPtr<CFX_DIBBase> pSource =
        pPageObj->AsImage()->GetImage()->LoadDIBBase();

    // Clone() is non-virtual, and can't be overloaded by CPDF_DIBBase to
    // return a clone of the subclass as one would typically expect from a
    // such a method. Instead, it only clones the CFX_DIBBase, none of whose
    // members point to objects owned by the form. As a result, |m_pBitmap|
    // may outlive |m_pForm|.
    if (pSource)
      m_pBitmap = pSource->Clone(nullptr);
  }
  m_pForm.reset();
  return true;
}

void CPDF_Type3Char::InitializeFromStreamData(bool bColored,
                                              const float* pData) {
  m_bColored = bColored;
  m_Width = FXSYS_round(TextUnitToGlyphUnit(pData[0]));
  m_BBox.left = FXSYS_round(TextUnitToGlyphUnit(pData[2]));
  m_BBox.bottom = FXSYS_round(TextUnitToGlyphUnit(pData[3]));
  m_BBox.right = FXSYS_round(TextUnitToGlyphUnit(pData[4]));
  m_BBox.top = FXSYS_round(TextUnitToGlyphUnit(pData[5]));
}

void CPDF_Type3Char::Transform(const CFX_Matrix& matrix) {
  m_Width = m_Width * matrix.GetXUnit() + 0.5f;

  CFX_FloatRect char_rect;
  if (m_BBox.right <= m_BBox.left || m_BBox.bottom >= m_BBox.top) {
    char_rect = form()->CalcBoundingBox();
    TextUnitRectToGlyphUnitRect(&char_rect);
  } else {
    char_rect = CFX_FloatRect(m_BBox);
  }

  m_BBox = matrix.TransformRect(char_rect).ToRoundedFxRect();
}

void CPDF_Type3Char::ResetForm() {
  m_pForm.reset();
}

RetainPtr<CFX_DIBitmap> CPDF_Type3Char::GetBitmap() {
  return m_pBitmap;
}

const RetainPtr<CFX_DIBitmap>& CPDF_Type3Char::GetBitmap() const {
  return m_pBitmap;
}
