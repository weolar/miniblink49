// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_generalstate.h"

#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fpdfapi/render/cpdf_docrenderdata.h"
#include "core/fpdfapi/render/cpdf_transferfunc.h"

namespace {

int RI_StringToId(const ByteString& ri) {
  uint32_t id = ri.GetID();
  if (id == FXBSTR_ID('A', 'b', 's', 'o'))
    return 1;

  if (id == FXBSTR_ID('S', 'a', 't', 'u'))
    return 2;

  if (id == FXBSTR_ID('P', 'e', 'r', 'c'))
    return 3;

  return 0;
}

BlendMode GetBlendTypeInternal(const ByteString& mode) {
  switch (mode.GetID()) {
    case FXBSTR_ID('N', 'o', 'r', 'm'):
    case FXBSTR_ID('C', 'o', 'm', 'p'):
      return BlendMode::kNormal;
    case FXBSTR_ID('M', 'u', 'l', 't'):
      return BlendMode::kMultiply;
    case FXBSTR_ID('S', 'c', 'r', 'e'):
      return BlendMode::kScreen;
    case FXBSTR_ID('O', 'v', 'e', 'r'):
      return BlendMode::kOverlay;
    case FXBSTR_ID('D', 'a', 'r', 'k'):
      return BlendMode::kDarken;
    case FXBSTR_ID('L', 'i', 'g', 'h'):
      return BlendMode::kLighten;
    case FXBSTR_ID('C', 'o', 'l', 'o'):
      if (mode.GetLength() == 10)
        return BlendMode::kColorDodge;
      if (mode.GetLength() == 9)
        return BlendMode::kColorBurn;
      return BlendMode::kColor;
    case FXBSTR_ID('H', 'a', 'r', 'd'):
      return BlendMode::kHardLight;
    case FXBSTR_ID('S', 'o', 'f', 't'):
      return BlendMode::kSoftLight;
    case FXBSTR_ID('D', 'i', 'f', 'f'):
      return BlendMode::kDifference;
    case FXBSTR_ID('E', 'x', 'c', 'l'):
      return BlendMode::kExclusion;
    case FXBSTR_ID('H', 'u', 'e', 0):
      return BlendMode::kHue;
    case FXBSTR_ID('S', 'a', 't', 'u'):
      return BlendMode::kSaturation;
    case FXBSTR_ID('L', 'u', 'm', 'i'):
      return BlendMode::kLuminosity;
  }
  return BlendMode::kNormal;
}

}  // namespace

CPDF_GeneralState::CPDF_GeneralState() {}

CPDF_GeneralState::CPDF_GeneralState(const CPDF_GeneralState& that)
    : m_Ref(that.m_Ref) {}

CPDF_GeneralState::~CPDF_GeneralState() {}

void CPDF_GeneralState::SetRenderIntent(const ByteString& ri) {
  m_Ref.GetPrivateCopy()->m_RenderIntent = RI_StringToId(ri);
}

ByteString CPDF_GeneralState::GetBlendMode() const {
  switch (GetBlendType()) {
    case BlendMode::kNormal:
      return ByteString(pdfium::transparency::kNormal);
    case BlendMode::kMultiply:
      return ByteString(pdfium::transparency::kMultiply);
    case BlendMode::kScreen:
      return ByteString(pdfium::transparency::kScreen);
    case BlendMode::kOverlay:
      return ByteString(pdfium::transparency::kOverlay);
    case BlendMode::kDarken:
      return ByteString(pdfium::transparency::kDarken);
    case BlendMode::kLighten:
      return ByteString(pdfium::transparency::kLighten);
    case BlendMode::kColorDodge:
      return ByteString(pdfium::transparency::kColorDodge);
    case BlendMode::kColorBurn:
      return ByteString(pdfium::transparency::kColorBurn);
    case BlendMode::kHardLight:
      return ByteString(pdfium::transparency::kHardLight);
    case BlendMode::kSoftLight:
      return ByteString(pdfium::transparency::kSoftLight);
    case BlendMode::kDifference:
      return ByteString(pdfium::transparency::kDifference);
    case BlendMode::kExclusion:
      return ByteString(pdfium::transparency::kExclusion);
    case BlendMode::kHue:
      return ByteString(pdfium::transparency::kHue);
    case BlendMode::kSaturation:
      return ByteString(pdfium::transparency::kSaturation);
    case BlendMode::kColor:
      return ByteString(pdfium::transparency::kColor);
    case BlendMode::kLuminosity:
      return ByteString(pdfium::transparency::kLuminosity);
  }
  return ByteString(pdfium::transparency::kNormal);
}

BlendMode CPDF_GeneralState::GetBlendType() const {
  const StateData* pData = m_Ref.GetObject();
  return pData ? pData->m_BlendType : BlendMode::kNormal;
}

void CPDF_GeneralState::SetBlendType(BlendMode type) {
  m_Ref.GetPrivateCopy()->m_BlendType = type;
}

float CPDF_GeneralState::GetFillAlpha() const {
  const StateData* pData = m_Ref.GetObject();
  return pData ? pData->m_FillAlpha : 1.0f;
}

void CPDF_GeneralState::SetFillAlpha(float alpha) {
  m_Ref.GetPrivateCopy()->m_FillAlpha = alpha;
}

float CPDF_GeneralState::GetStrokeAlpha() const {
  const StateData* pData = m_Ref.GetObject();
  return pData ? pData->m_StrokeAlpha : 1.0f;
}

void CPDF_GeneralState::SetStrokeAlpha(float alpha) {
  m_Ref.GetPrivateCopy()->m_StrokeAlpha = alpha;
}

CPDF_Object* CPDF_GeneralState::GetSoftMask() const {
  const StateData* pData = m_Ref.GetObject();
  return pData ? pData->m_pSoftMask.Get() : nullptr;
}

void CPDF_GeneralState::SetSoftMask(CPDF_Object* pObject) {
  m_Ref.GetPrivateCopy()->m_pSoftMask = pObject;
}

const CPDF_Object* CPDF_GeneralState::GetTR() const {
  const StateData* pData = m_Ref.GetObject();
  return pData ? pData->m_pTR.Get() : nullptr;
}

void CPDF_GeneralState::SetTR(CPDF_Object* pObject) {
  m_Ref.GetPrivateCopy()->m_pTR = pObject;
}

RetainPtr<CPDF_TransferFunc> CPDF_GeneralState::GetTransferFunc() const {
  const StateData* pData = m_Ref.GetObject();
  return pData ? pData->m_pTransferFunc : nullptr;
}

void CPDF_GeneralState::SetTransferFunc(
    const RetainPtr<CPDF_TransferFunc>& pFunc) {
  m_Ref.GetPrivateCopy()->m_pTransferFunc = pFunc;
}

void CPDF_GeneralState::SetBlendMode(const ByteString& mode) {
  StateData* pData = m_Ref.GetPrivateCopy();
  pData->m_BlendMode = mode;
  pData->m_BlendType = GetBlendTypeInternal(mode);
}

const CFX_Matrix* CPDF_GeneralState::GetSMaskMatrix() const {
  const StateData* pData = m_Ref.GetObject();
  return pData ? &pData->m_SMaskMatrix : nullptr;
}

void CPDF_GeneralState::SetSMaskMatrix(const CFX_Matrix& matrix) {
  m_Ref.GetPrivateCopy()->m_SMaskMatrix = matrix;
}

bool CPDF_GeneralState::GetFillOP() const {
  const StateData* pData = m_Ref.GetObject();
  return pData && pData->m_FillOP;
}

void CPDF_GeneralState::SetFillOP(bool op) {
  m_Ref.GetPrivateCopy()->m_FillOP = op;
}

void CPDF_GeneralState::SetStrokeOP(bool op) {
  m_Ref.GetPrivateCopy()->m_StrokeOP = op;
}

bool CPDF_GeneralState::GetStrokeOP() const {
  const StateData* pData = m_Ref.GetObject();
  return pData && pData->m_StrokeOP;
}

int CPDF_GeneralState::GetOPMode() const {
  return m_Ref.GetObject()->m_OPMode;
}

void CPDF_GeneralState::SetOPMode(int mode) {
  m_Ref.GetPrivateCopy()->m_OPMode = mode;
}

void CPDF_GeneralState::SetBG(CPDF_Object* pObject) {
  m_Ref.GetPrivateCopy()->m_pBG = pObject;
}

void CPDF_GeneralState::SetUCR(CPDF_Object* pObject) {
  m_Ref.GetPrivateCopy()->m_pUCR = pObject;
}

void CPDF_GeneralState::SetHT(CPDF_Object* pObject) {
  m_Ref.GetPrivateCopy()->m_pHT = pObject;
}

void CPDF_GeneralState::SetFlatness(float flatness) {
  m_Ref.GetPrivateCopy()->m_Flatness = flatness;
}

void CPDF_GeneralState::SetSmoothness(float smoothness) {
  m_Ref.GetPrivateCopy()->m_Smoothness = smoothness;
}

bool CPDF_GeneralState::GetStrokeAdjust() const {
  const StateData* pData = m_Ref.GetObject();
  return pData && pData->m_StrokeAdjust;
}

void CPDF_GeneralState::SetStrokeAdjust(bool adjust) {
  m_Ref.GetPrivateCopy()->m_StrokeAdjust = adjust;
}

void CPDF_GeneralState::SetAlphaSource(bool source) {
  m_Ref.GetPrivateCopy()->m_AlphaSource = source;
}

void CPDF_GeneralState::SetTextKnockout(bool knockout) {
  m_Ref.GetPrivateCopy()->m_TextKnockout = knockout;
}

void CPDF_GeneralState::SetMatrix(const CFX_Matrix& matrix) {
  m_Ref.GetPrivateCopy()->m_Matrix = matrix;
}

CFX_Matrix* CPDF_GeneralState::GetMutableMatrix() {
  return &m_Ref.GetPrivateCopy()->m_Matrix;
}

CPDF_GeneralState::StateData::StateData() = default;

CPDF_GeneralState::StateData::StateData(const StateData& that)
    : m_BlendMode(that.m_BlendMode),
      m_BlendType(that.m_BlendType),
      m_pSoftMask(that.m_pSoftMask),
      m_StrokeAlpha(that.m_StrokeAlpha),
      m_FillAlpha(that.m_FillAlpha),
      m_pTR(that.m_pTR),
      m_pTransferFunc(that.m_pTransferFunc),
      m_RenderIntent(that.m_RenderIntent),
      m_StrokeAdjust(that.m_StrokeAdjust),
      m_AlphaSource(that.m_AlphaSource),
      m_TextKnockout(that.m_TextKnockout),
      m_StrokeOP(that.m_StrokeOP),
      m_FillOP(that.m_FillOP),
      m_OPMode(that.m_OPMode),
      m_pBG(that.m_pBG),
      m_pUCR(that.m_pUCR),
      m_pHT(that.m_pHT),
      m_Flatness(that.m_Flatness),
      m_Smoothness(that.m_Smoothness) {
  m_Matrix = that.m_Matrix;
  m_SMaskMatrix = that.m_SMaskMatrix;

  if (that.m_pTransferFunc && that.m_pTransferFunc->GetDocument()) {
    CPDF_DocRenderData* pDocCache =
        that.m_pTransferFunc->GetDocument()->GetRenderData();
    if (pDocCache)
      m_pTransferFunc = pDocCache->GetTransferFunc(m_pTR.Get());
  }
}

CPDF_GeneralState::StateData::~StateData() {
  if (m_pTransferFunc && m_pTransferFunc->GetDocument()) {
    CPDF_DocRenderData* pDocCache =
        m_pTransferFunc->GetDocument()->GetRenderData();
    if (pDocCache) {
      m_pTransferFunc.Reset();  // Give up our reference first.
      pDocCache->MaybePurgeTransferFunc(m_pTR.Get());
    }
  }
}
