// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_pathobject.h"

CPDF_PathObject::CPDF_PathObject(int32_t content_stream)
    : CPDF_PageObject(content_stream) {}

CPDF_PathObject::CPDF_PathObject() : CPDF_PathObject(kNoContentStream) {}

CPDF_PathObject::~CPDF_PathObject() = default;

CPDF_PageObject::Type CPDF_PathObject::GetType() const {
  return PATH;
}

void CPDF_PathObject::Transform(const CFX_Matrix& matrix) {
  m_Matrix.Concat(matrix);
  CalcBoundingBox();
  SetDirty(true);
}

bool CPDF_PathObject::IsPath() const {
  return true;
}

CPDF_PathObject* CPDF_PathObject::AsPath() {
  return this;
}

const CPDF_PathObject* CPDF_PathObject::AsPath() const {
  return this;
}

void CPDF_PathObject::CalcBoundingBox() {
  if (!m_Path.HasRef())
    return;
  CFX_FloatRect rect;
  float width = m_GraphState.GetLineWidth();
  if (m_bStroke && width != 0) {
    rect = m_Path.GetBoundingBox(width, m_GraphState.GetMiterLimit());
  } else {
    rect = m_Path.GetBoundingBox();
  }
  rect = m_Matrix.TransformRect(rect);

  if (width == 0 && m_bStroke)
    rect.Inflate(0.5f, 0.5f);
  SetRect(rect);
}
