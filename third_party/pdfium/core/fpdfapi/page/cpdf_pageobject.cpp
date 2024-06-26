// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_pageobject.h"

constexpr int32_t CPDF_PageObject::kNoContentStream;

CPDF_PageObject::CPDF_PageObject(int32_t content_stream)
    : m_ContentStream(content_stream) {}

CPDF_PageObject::CPDF_PageObject() : CPDF_PageObject(kNoContentStream) {}

CPDF_PageObject::~CPDF_PageObject() = default;

bool CPDF_PageObject::IsText() const {
  return false;
}

bool CPDF_PageObject::IsPath() const {
  return false;
}

bool CPDF_PageObject::IsImage() const {
  return false;
}

bool CPDF_PageObject::IsShading() const {
  return false;
}

bool CPDF_PageObject::IsForm() const {
  return false;
}

CPDF_TextObject* CPDF_PageObject::AsText() {
  return nullptr;
}

const CPDF_TextObject* CPDF_PageObject::AsText() const {
  return nullptr;
}

CPDF_PathObject* CPDF_PageObject::AsPath() {
  return nullptr;
}

const CPDF_PathObject* CPDF_PageObject::AsPath() const {
  return nullptr;
}

CPDF_ImageObject* CPDF_PageObject::AsImage() {
  return nullptr;
}

const CPDF_ImageObject* CPDF_PageObject::AsImage() const {
  return nullptr;
}

CPDF_ShadingObject* CPDF_PageObject::AsShading() {
  return nullptr;
}

const CPDF_ShadingObject* CPDF_PageObject::AsShading() const {
  return nullptr;
}

CPDF_FormObject* CPDF_PageObject::AsForm() {
  return nullptr;
}

const CPDF_FormObject* CPDF_PageObject::AsForm() const {
  return nullptr;
}

void CPDF_PageObject::CopyData(const CPDF_PageObject* pSrc) {
  CopyStates(*pSrc);
  m_Rect = pSrc->m_Rect;
  m_bDirty = true;
}

void CPDF_PageObject::TransformClipPath(const CFX_Matrix& matrix) {
  if (!m_ClipPath.HasRef())
    return;
  m_ClipPath.Transform(matrix);
  SetDirty(true);
}

void CPDF_PageObject::TransformGeneralState(const CFX_Matrix& matrix) {
  if (!m_GeneralState.HasRef())
    return;
  m_GeneralState.GetMutableMatrix()->Concat(matrix);
  SetDirty(true);
}

FX_RECT CPDF_PageObject::GetBBox() const {
  return GetRect().GetOuterRect();
}

FX_RECT CPDF_PageObject::GetTransformedBBox(const CFX_Matrix& matrix) const {
  return matrix.TransformRect(GetRect()).GetOuterRect();
}
