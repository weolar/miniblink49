// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_transferfunc.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_dibtransferfunc.h"
#include "core/fxge/dib/cfx_dibbase.h"

CPDF_TransferFunc::CPDF_TransferFunc(CPDF_Document* pDoc,
                                     bool bIdentify,
                                     std::vector<uint8_t> samples_r,
                                     std::vector<uint8_t> samples_g,
                                     std::vector<uint8_t> samples_b)
    : m_pPDFDoc(pDoc),
      m_bIdentity(bIdentify),
      m_SamplesR(std::move(samples_r)),
      m_SamplesG(std::move(samples_g)),
      m_SamplesB(std::move(samples_b)) {
  ASSERT(m_SamplesR.size() == kChannelSampleSize);
  ASSERT(m_SamplesG.size() == kChannelSampleSize);
  ASSERT(m_SamplesB.size() == kChannelSampleSize);
}

CPDF_TransferFunc::~CPDF_TransferFunc() = default;

FX_COLORREF CPDF_TransferFunc::TranslateColor(FX_COLORREF colorref) const {
  return FXSYS_BGR(m_SamplesB[FXSYS_GetBValue(colorref)],
                   m_SamplesG[FXSYS_GetGValue(colorref)],
                   m_SamplesR[FXSYS_GetRValue(colorref)]);
}

RetainPtr<CFX_DIBBase> CPDF_TransferFunc::TranslateImage(
    const RetainPtr<CFX_DIBBase>& pSrc) {
  RetainPtr<CPDF_TransferFunc> pHolder(this);
  auto pDest = pdfium::MakeRetain<CPDF_DIBTransferFunc>(pHolder);
  pDest->LoadSrc(pSrc);
  return pDest;
}

pdfium::span<const uint8_t> CPDF_TransferFunc::GetSamplesR() const {
  return m_SamplesR;
}

pdfium::span<const uint8_t> CPDF_TransferFunc::GetSamplesG() const {
  return m_SamplesG;
}

pdfium::span<const uint8_t> CPDF_TransferFunc::GetSamplesB() const {
  return m_SamplesB;
}
