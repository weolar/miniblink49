// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_tilingpattern.h"

#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "third_party/base/ptr_util.h"

CPDF_TilingPattern::CPDF_TilingPattern(CPDF_Document* pDoc,
                                       CPDF_Object* pPatternObj,
                                       const CFX_Matrix& parentMatrix)
    : CPDF_Pattern(pDoc, pPatternObj, parentMatrix) {
  ASSERT(document());
  m_bColored = pattern_obj()->GetDict()->GetIntegerFor("PaintType") == 1;
  SetPatternToFormMatrix();
}

CPDF_TilingPattern::~CPDF_TilingPattern() {}

CPDF_TilingPattern* CPDF_TilingPattern::AsTilingPattern() {
  return this;
}

CPDF_ShadingPattern* CPDF_TilingPattern::AsShadingPattern() {
  return nullptr;
}

bool CPDF_TilingPattern::Load() {
  if (m_pForm)
    return true;

  const CPDF_Dictionary* pDict = pattern_obj()->GetDict();
  if (!pDict)
    return false;

  m_bColored = pDict->GetIntegerFor("PaintType") == 1;
  m_XStep = static_cast<float>(fabs(pDict->GetNumberFor("XStep")));
  m_YStep = static_cast<float>(fabs(pDict->GetNumberFor("YStep")));

  CPDF_Stream* pStream = pattern_obj()->AsStream();
  if (!pStream)
    return false;

  const CFX_Matrix& matrix = parent_matrix();
  m_pForm = pdfium::MakeUnique<CPDF_Form>(document(), nullptr, pStream);
  m_pForm->ParseContent(nullptr, &matrix, nullptr, nullptr);
  m_BBox = pDict->GetRectFor("BBox");
  return true;
}
