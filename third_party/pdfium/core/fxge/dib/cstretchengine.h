// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CSTRETCHENGINE_H_
#define CORE_FXGE_DIB_CSTRETCHENGINE_H_

#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_dib.h"

class CFX_DIBBase;
class PauseIndicatorIface;
class ScanlineComposerIface;

class CStretchEngine {
 public:
  CStretchEngine(ScanlineComposerIface* pDestBitmap,
                 FXDIB_Format dest_format,
                 int dest_width,
                 int dest_height,
                 const FX_RECT& clip_rect,
                 const RetainPtr<CFX_DIBBase>& pSrcBitmap,
                 const FXDIB_ResampleOptions& options);
  ~CStretchEngine();

  bool Continue(PauseIndicatorIface* pPause);

  bool StartStretchHorz();
  bool ContinueStretchHorz(PauseIndicatorIface* pPause);
  void StretchVert();

  class CWeightTable {
   public:
    CWeightTable();
    ~CWeightTable();

    bool Calc(int dest_len,
              int dest_min,
              int dest_max,
              int src_len,
              int src_min,
              int src_max,
              const FXDIB_ResampleOptions& options);

    const PixelWeight* GetPixelWeight(int pixel) const;
    PixelWeight* GetPixelWeight(int pixel) {
      return const_cast<PixelWeight*>(
          static_cast<const CWeightTable*>(this)->GetPixelWeight(pixel));
    }

    int* GetValueFromPixelWeight(PixelWeight* pWeight, int index) const;
    size_t GetPixelWeightSize() const;

   private:
    int m_DestMin;
    int m_ItemSize;
    std::vector<uint8_t> m_WeightTables;
    size_t m_dwWeightTablesSize;
  };

  enum class State : uint8_t { kInitial, kHorizontal, kVertical };

  enum class TransformMethod : uint8_t {
    k1BppTo8Bpp,
    k1BppToManyBpp,
    k8BppTo8Bpp,
    k8BppTo8BppWithAlpha,
    k8BppToManyBpp,
    k8BppToManyBppWithAlpha,
    kManyBpptoManyBpp,
    kManyBpptoManyBppWithAlpha
  };

  const FXDIB_Format m_DestFormat;
  const int m_DestBpp;
  const int m_SrcBpp;
  const int m_bHasAlpha;
  RetainPtr<CFX_DIBBase> const m_pSource;
  const uint32_t* m_pSrcPalette;
  const int m_SrcWidth;
  const int m_SrcHeight;
  UnownedPtr<ScanlineComposerIface> const m_pDestBitmap;
  const int m_DestWidth;
  const int m_DestHeight;
  const FX_RECT m_DestClip;
  std::vector<uint8_t> m_DestScanline;
  std::vector<uint8_t> m_DestMaskScanline;
  std::vector<uint8_t> m_InterBuf;
  std::vector<uint8_t> m_ExtraAlphaBuf;
  FX_RECT m_SrcClip;
  int m_InterPitch;
  int m_ExtraMaskPitch;
  FXDIB_ResampleOptions m_ResampleOptions;
  TransformMethod m_TransMethod;
  State m_State = State::kInitial;
  int m_CurRow;
  CWeightTable m_WeightTable;
};

#endif  // CORE_FXGE_DIB_CSTRETCHENGINE_H_
