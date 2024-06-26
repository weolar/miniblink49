// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_IMAGELOADER_H_
#define CORE_FPDFAPI_RENDER_CPDF_IMAGELOADER_H_

#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

class CFX_DIBBase;
class CPDF_ImageObject;
class CPDF_PageRenderCache;
class CPDF_RenderStatus;
class CPDF_TransferFunc;
class PauseIndicatorIface;

class CPDF_ImageLoader {
 public:
  CPDF_ImageLoader();
  ~CPDF_ImageLoader();

  bool Start(CPDF_ImageObject* pImage,
             CPDF_PageRenderCache* pCache,
             bool bStdCS,
             uint32_t GroupFamily,
             bool bLoadMask,
             CPDF_RenderStatus* pRenderStatus);
  bool Continue(PauseIndicatorIface* pPause, CPDF_RenderStatus* pRenderStatus);

  RetainPtr<CFX_DIBBase> TranslateImage(
      const RetainPtr<CPDF_TransferFunc>& pTransferFunc);

  const RetainPtr<CFX_DIBBase>& GetBitmap() const { return m_pBitmap; }
  const RetainPtr<CFX_DIBBase>& GetMask() const { return m_pMask; }
  uint32_t MatteColor() const { return m_MatteColor; }

 private:
  void HandleFailure();

  uint32_t m_MatteColor = 0;
  bool m_bCached = false;
  RetainPtr<CFX_DIBBase> m_pBitmap;
  RetainPtr<CFX_DIBBase> m_pMask;
  UnownedPtr<CPDF_PageRenderCache> m_pCache;
  UnownedPtr<CPDF_ImageObject> m_pImageObject;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_IMAGELOADER_H_
