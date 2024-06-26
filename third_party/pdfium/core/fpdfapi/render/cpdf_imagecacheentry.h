// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_IMAGECACHEENTRY_H_
#define CORE_FPDFAPI_RENDER_CPDF_IMAGECACHEENTRY_H_

#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

class CFX_DIBitmap;
class CPDF_Dictionary;
class CPDF_Document;
class CPDF_Image;
class CPDF_RenderStatus;
class PauseIndicatorIface;

class CPDF_ImageCacheEntry {
 public:
  CPDF_ImageCacheEntry(CPDF_Document* pDoc,
                       const RetainPtr<CPDF_Image>& pImage);
  ~CPDF_ImageCacheEntry();

  void Reset(const RetainPtr<CFX_DIBitmap>& pBitmap);
  uint32_t EstimateSize() const { return m_dwCacheSize; }
  uint32_t GetTimeCount() const { return m_dwTimeCount; }
  CPDF_Image* GetImage() const { return m_pImage.Get(); }

  CPDF_DIBBase::LoadState StartGetCachedBitmap(
      const CPDF_Dictionary* pFormResources,
      CPDF_Dictionary* pPageResources,
      bool bStdCS,
      uint32_t GroupFamily,
      bool bLoadMask,
      CPDF_RenderStatus* pRenderStatus);

  // Returns whether to Continue() or not.
  bool Continue(PauseIndicatorIface* pPause, CPDF_RenderStatus* pRenderStatus);

  RetainPtr<CFX_DIBBase> DetachBitmap();
  RetainPtr<CFX_DIBBase> DetachMask();

  int m_dwTimeCount;
  uint32_t m_MatteColor;

 private:
  void ContinueGetCachedBitmap(CPDF_RenderStatus* pRenderStatus);
  void CalcSize();

  UnownedPtr<CPDF_Document> const m_pDocument;
  RetainPtr<CPDF_Image> const m_pImage;
  RetainPtr<CFX_DIBBase> m_pCurBitmap;
  RetainPtr<CFX_DIBBase> m_pCurMask;
  RetainPtr<CFX_DIBBase> m_pCachedBitmap;
  RetainPtr<CFX_DIBBase> m_pCachedMask;
  uint32_t m_dwCacheSize;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_IMAGECACHEENTRY_H_
