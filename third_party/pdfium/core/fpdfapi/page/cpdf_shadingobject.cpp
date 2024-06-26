// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_shadingobject.h"

#include "core/fpdfapi/page/cpdf_shadingpattern.h"
#include "core/fpdfapi/parser/cpdf_document.h"

CPDF_ShadingObject::CPDF_ShadingObject(int32_t content_stream,
                                       CPDF_ShadingPattern* pattern,
                                       const CFX_Matrix& matrix)
    : CPDF_PageObject(content_stream), m_pShading(pattern), m_Matrix(matrix) {}

CPDF_ShadingObject::~CPDF_ShadingObject() {}

CPDF_PageObject::Type CPDF_ShadingObject::GetType() const {
  return SHADING;
}

void CPDF_ShadingObject::Transform(const CFX_Matrix& matrix) {
  if (m_ClipPath.HasRef())
    m_ClipPath.Transform(matrix);

  m_Matrix.Concat(matrix);
  if (m_ClipPath.HasRef()) {
    CalcBoundingBox();
    return;
  }

  SetRect(matrix.TransformRect(GetRect()));
}

bool CPDF_ShadingObject::IsShading() const {
  return true;
}

CPDF_ShadingObject* CPDF_ShadingObject::AsShading() {
  return this;
}

const CPDF_ShadingObject* CPDF_ShadingObject::AsShading() const {
  return this;
}

void CPDF_ShadingObject::CalcBoundingBox() {
  if (!m_ClipPath.HasRef())
    return;
  SetRect(m_ClipPath.GetClipBox());
}
