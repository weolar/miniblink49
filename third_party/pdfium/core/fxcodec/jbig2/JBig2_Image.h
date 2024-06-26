// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_IMAGE_H_
#define CORE_FXCODEC_JBIG2_JBIG2_IMAGE_H_

#include <memory>

#include "core/fxcodec/jbig2/JBig2_Define.h"
#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/maybe_owned.h"

struct FX_RECT;

enum JBig2ComposeOp {
  JBIG2_COMPOSE_OR = 0,
  JBIG2_COMPOSE_AND = 1,
  JBIG2_COMPOSE_XOR = 2,
  JBIG2_COMPOSE_XNOR = 3,
  JBIG2_COMPOSE_REPLACE = 4
};

class CJBig2_Image {
 public:
  CJBig2_Image(int32_t w, int32_t h);
  CJBig2_Image(int32_t w, int32_t h, int32_t stride, uint8_t* pBuf);
  CJBig2_Image(const CJBig2_Image& im);
  ~CJBig2_Image();

  static bool IsValidImageSize(int32_t w, int32_t h);

  int32_t width() const { return m_nWidth; }
  int32_t height() const { return m_nHeight; }
  int32_t stride() const { return m_nStride; }

  uint8_t* data() const { return m_pData.Get(); }

  int GetPixel(int32_t x, int32_t y) const;
  void SetPixel(int32_t x, int32_t y, int bVal);

  uint8_t* GetLineUnsafe(int32_t y) const { return data() + y * m_nStride; }
  uint8_t* GetLine(int32_t y) const {
    return (y >= 0 && y < m_nHeight) ? GetLineUnsafe(y) : nullptr;
  }

  void CopyLine(int32_t hTo, int32_t hFrom);
  void Fill(bool v);

  bool ComposeFrom(int32_t x, int32_t y, CJBig2_Image* pSrc, JBig2ComposeOp op);
  bool ComposeFromWithRect(int32_t x,
                           int32_t y,
                           CJBig2_Image* pSrc,
                           const FX_RECT& rtSrc,
                           JBig2ComposeOp op);

  std::unique_ptr<CJBig2_Image> SubImage(int32_t x,
                                         int32_t y,
                                         int32_t w,
                                         int32_t h);
  void Expand(int32_t h, bool v);

  bool ComposeTo(CJBig2_Image* pDst, int32_t x, int32_t y, JBig2ComposeOp op);
  bool ComposeToWithRect(CJBig2_Image* pDst,
                         int32_t x,
                         int32_t y,
                         const FX_RECT& rtSrc,
                         JBig2ComposeOp op);

 private:
  void SubImageFast(int32_t x,
                    int32_t y,
                    int32_t w,
                    int32_t h,
                    CJBig2_Image* pImage);
  void SubImageSlow(int32_t x,
                    int32_t y,
                    int32_t w,
                    int32_t h,
                    CJBig2_Image* pImage);
  bool ComposeToInternal(CJBig2_Image* pDst,
                         int32_t x,
                         int32_t y,
                         JBig2ComposeOp op,
                         const FX_RECT& rtSrc);

  MaybeOwned<uint8_t, FxFreeDeleter> m_pData;
  int32_t m_nWidth = 0;   // 1-bit pixels
  int32_t m_nHeight = 0;  // lines
  int32_t m_nStride = 0;  // bytes, must be multiple of 4.
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_IMAGE_H_
