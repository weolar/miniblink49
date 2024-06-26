// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_pagerendercache.h"

#include <algorithm>
#include <vector>

#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/render/cpdf_imagecacheentry.h"
#include "core/fpdfapi/render/cpdf_renderstatus.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "third_party/base/ptr_util.h"

namespace {

struct CacheInfo {
  CacheInfo(uint32_t t, CPDF_Stream* stream) : time(t), pStream(stream) {}

  uint32_t time;
  CPDF_Stream* pStream;

  bool operator<(const CacheInfo& other) const { return time < other.time; }
};

}  // namespace

CPDF_PageRenderCache::CPDF_PageRenderCache(CPDF_Page* pPage) : m_pPage(pPage) {}

CPDF_PageRenderCache::~CPDF_PageRenderCache() = default;

void CPDF_PageRenderCache::CacheOptimization(int32_t dwLimitCacheSize) {
  if (m_nCacheSize <= (uint32_t)dwLimitCacheSize)
    return;

  size_t nCount = m_ImageCache.size();
  std::vector<CacheInfo> cache_info;
  cache_info.reserve(nCount);
  for (const auto& it : m_ImageCache) {
    cache_info.emplace_back(it.second->GetTimeCount(),
                            it.second->GetImage()->GetStream());
  }
  std::sort(cache_info.begin(), cache_info.end());

  // Check if time value is about to roll over and reset all entries.
  // The comparision is legal because uint32_t is an unsigned type.
  uint32_t nTimeCount = m_nTimeCount;
  if (nTimeCount + 1 < nTimeCount) {
    for (size_t i = 0; i < nCount; i++)
      m_ImageCache[cache_info[i].pStream]->m_dwTimeCount = i;
    m_nTimeCount = nCount;
  }

  size_t i = 0;
  while (i + 15 < nCount)
    ClearImageCacheEntry(cache_info[i++].pStream);

  while (i < nCount && m_nCacheSize > (uint32_t)dwLimitCacheSize)
    ClearImageCacheEntry(cache_info[i++].pStream);
}

void CPDF_PageRenderCache::ClearImageCacheEntry(CPDF_Stream* pStream) {
  auto it = m_ImageCache.find(pStream);
  if (it == m_ImageCache.end())
    return;

  m_nCacheSize -= it->second->EstimateSize();
  m_ImageCache.erase(it);
}

bool CPDF_PageRenderCache::StartGetCachedBitmap(
    const RetainPtr<CPDF_Image>& pImage,
    bool bStdCS,
    uint32_t GroupFamily,
    bool bLoadMask,
    CPDF_RenderStatus* pRenderStatus) {
  CPDF_Stream* pStream = pImage->GetStream();
  const auto it = m_ImageCache.find(pStream);
  m_bCurFindCache = it != m_ImageCache.end();
  if (m_bCurFindCache) {
    m_pCurImageCacheEntry = it->second.get();
  } else {
    m_pCurImageCacheEntry = pdfium::MakeUnique<CPDF_ImageCacheEntry>(
        m_pPage->GetDocument(), pImage);
  }
  CPDF_DIBBase::LoadState ret = m_pCurImageCacheEntry->StartGetCachedBitmap(
      pRenderStatus->GetFormResource(), m_pPage->m_pPageResources.Get(), bStdCS,
      GroupFamily, bLoadMask, pRenderStatus);
  if (ret == CPDF_DIBBase::LoadState::kContinue)
    return true;

  m_nTimeCount++;
  if (!m_bCurFindCache)
    m_ImageCache[pStream] = m_pCurImageCacheEntry.Release();

  if (ret == CPDF_DIBBase::LoadState::kFail)
    m_nCacheSize += m_pCurImageCacheEntry->EstimateSize();

  return false;
}

bool CPDF_PageRenderCache::Continue(PauseIndicatorIface* pPause,
                                    CPDF_RenderStatus* pRenderStatus) {
  bool ret = m_pCurImageCacheEntry->Continue(pPause, pRenderStatus);
  if (ret)
    return true;

  m_nTimeCount++;
  if (!m_bCurFindCache) {
    m_ImageCache[m_pCurImageCacheEntry->GetImage()->GetStream()] =
        m_pCurImageCacheEntry.Release();
  }
  m_nCacheSize += m_pCurImageCacheEntry->EstimateSize();
  return false;
}

void CPDF_PageRenderCache::ResetBitmap(const RetainPtr<CPDF_Image>& pImage) {
  CPDF_ImageCacheEntry* pEntry;
  CPDF_Stream* pStream = pImage->GetStream();
  const auto it = m_ImageCache.find(pStream);
  if (it == m_ImageCache.end())
    return;

  pEntry = it->second.get();
  m_nCacheSize -= pEntry->EstimateSize();
  pEntry->Reset(nullptr);
  m_nCacheSize += pEntry->EstimateSize();
}
