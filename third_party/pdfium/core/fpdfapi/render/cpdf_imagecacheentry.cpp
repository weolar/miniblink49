// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_imagecacheentry.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fpdfapi/render/cpdf_pagerendercache.h"
#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fpdfapi/render/cpdf_renderstatus.h"
#include "core/fxge/dib/cfx_dibitmap.h"

CPDF_ImageCacheEntry::CPDF_ImageCacheEntry(CPDF_Document* pDoc,
                                           const RetainPtr<CPDF_Image>& pImage)
    : m_dwTimeCount(0),
      m_MatteColor(0),
      m_pDocument(pDoc),
      m_pImage(pImage),
      m_dwCacheSize(0) {}

CPDF_ImageCacheEntry::~CPDF_ImageCacheEntry() {}

void CPDF_ImageCacheEntry::Reset(const RetainPtr<CFX_DIBitmap>& pBitmap) {
  m_pCachedBitmap.Reset();
  if (pBitmap)
    m_pCachedBitmap = pBitmap->Clone(nullptr);
  CalcSize();
}

static uint32_t FPDF_ImageCache_EstimateImageSize(
    const RetainPtr<CFX_DIBBase>& pDIB) {
  return pDIB && pDIB->GetBuffer()
             ? (uint32_t)pDIB->GetHeight() * pDIB->GetPitch() +
                   (uint32_t)pDIB->GetPaletteSize() * 4
             : 0;
}

RetainPtr<CFX_DIBBase> CPDF_ImageCacheEntry::DetachBitmap() {
  return std::move(m_pCurBitmap);
}

RetainPtr<CFX_DIBBase> CPDF_ImageCacheEntry::DetachMask() {
  return std::move(m_pCurMask);
}

CPDF_DIBBase::LoadState CPDF_ImageCacheEntry::StartGetCachedBitmap(
    const CPDF_Dictionary* pFormResources,
    CPDF_Dictionary* pPageResources,
    bool bStdCS,
    uint32_t GroupFamily,
    bool bLoadMask,
    CPDF_RenderStatus* pRenderStatus) {
  ASSERT(pRenderStatus);

  if (m_pCachedBitmap) {
    m_pCurBitmap = m_pCachedBitmap;
    m_pCurMask = m_pCachedMask;
    return CPDF_DIBBase::LoadState::kSuccess;
  }

  m_pCurBitmap = pdfium::MakeRetain<CPDF_DIBBase>();
  CPDF_DIBBase::LoadState ret =
      m_pCurBitmap.As<CPDF_DIBBase>()->StartLoadDIBBase(
          m_pDocument.Get(), m_pImage->GetStream(), true, pFormResources,
          pPageResources, bStdCS, GroupFamily, bLoadMask);
  if (ret == CPDF_DIBBase::LoadState::kContinue)
    return CPDF_DIBBase::LoadState::kContinue;

  if (ret == CPDF_DIBBase::LoadState::kSuccess)
    ContinueGetCachedBitmap(pRenderStatus);
  else
    m_pCurBitmap.Reset();
  return CPDF_DIBBase::LoadState::kFail;
}

bool CPDF_ImageCacheEntry::Continue(PauseIndicatorIface* pPause,
                                    CPDF_RenderStatus* pRenderStatus) {
  CPDF_DIBBase::LoadState ret =
      m_pCurBitmap.As<CPDF_DIBBase>()->ContinueLoadDIBBase(pPause);
  if (ret == CPDF_DIBBase::LoadState::kContinue)
    return true;

  if (ret == CPDF_DIBBase::LoadState::kSuccess)
    ContinueGetCachedBitmap(pRenderStatus);
  else
    m_pCurBitmap.Reset();
  return false;
}

void CPDF_ImageCacheEntry::ContinueGetCachedBitmap(
    CPDF_RenderStatus* pRenderStatus) {
  m_MatteColor = m_pCurBitmap.As<CPDF_DIBBase>()->GetMatteColor();
  m_pCurMask = m_pCurBitmap.As<CPDF_DIBBase>()->DetachMask();
  CPDF_RenderContext* pContext = pRenderStatus->GetContext();
  CPDF_PageRenderCache* pPageRenderCache = pContext->GetPageCache();
  m_dwTimeCount = pPageRenderCache->GetTimeCount();
  if (m_pCurBitmap->GetPitch() * m_pCurBitmap->GetHeight() <
      FPDF_HUGE_IMAGE_SIZE) {
    m_pCachedBitmap = m_pCurBitmap->Clone(nullptr);
    m_pCurBitmap.Reset();
  } else {
    m_pCachedBitmap = m_pCurBitmap;
  }
  if (m_pCurMask) {
    m_pCachedMask = m_pCurMask->Clone(nullptr);
    m_pCurMask.Reset();
  }
  m_pCurBitmap = m_pCachedBitmap;
  m_pCurMask = m_pCachedMask;
  CalcSize();
}

void CPDF_ImageCacheEntry::CalcSize() {
  m_dwCacheSize = FPDF_ImageCache_EstimateImageSize(m_pCachedBitmap) +
                  FPDF_ImageCache_EstimateImageSize(m_pCachedMask);
}
