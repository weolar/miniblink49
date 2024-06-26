// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CFX_IMAGETRANSFORMER_H_
#define CORE_FXGE_DIB_CFX_IMAGETRANSFORMER_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/dib/cfx_bitmapstorer.h"

class CFX_DIBBase;
class CFX_DIBitmap;
class CFX_ImageStretcher;
class PauseIndicatorIface;

class CFX_ImageTransformer {
 public:
  CFX_ImageTransformer(const RetainPtr<CFX_DIBBase>& pSrc,
                       const CFX_Matrix& matrix,
                       const FXDIB_ResampleOptions& options,
                       const FX_RECT* pClip);
  ~CFX_ImageTransformer();

  bool Continue(PauseIndicatorIface* pPause);

  const FX_RECT& result() const { return m_result; }
  RetainPtr<CFX_DIBitmap> DetachBitmap();

 private:
  struct BilinearData {
    int res_x;
    int res_y;
    int src_col_l;
    int src_row_l;
    int src_col_r;
    int src_row_r;
    int row_offset_l;
    int row_offset_r;
  };

  struct BicubicData {
    int res_x;
    int res_y;
    int src_col_l;
    int src_row_l;
    int src_col_r;
    int src_row_r;
    int pos_pixel[8];
    int u_w[4];
    int v_w[4];
  };

  struct DownSampleData {
    int src_col;
    int src_row;
  };

  struct CalcData {
    CFX_DIBitmap* bitmap;
    const CFX_Matrix& matrix;
    const uint8_t* buf;
    uint32_t pitch;
  };

  void CalcMask(const CalcData& cdata);
  void CalcAlpha(const CalcData& cdata);
  void CalcMono(const CalcData& cdata, FXDIB_Format format);
  void CalcColor(const CalcData& cdata, FXDIB_Format format, int Bpp);

  bool IsBilinear() const;
  bool IsBiCubic() const;

  int stretch_width() const { return m_StretchClip.Width(); }
  int stretch_height() const { return m_StretchClip.Height(); }

  bool InStretchBounds(int col, int row) const {
    return col >= 0 && col <= stretch_width() && row >= 0 &&
           row <= stretch_height();
  }

  void AdjustCoords(int* col, int* row) const;

  void DoBilinearLoop(const CalcData& cdata,
                      int increment,
                      std::function<void(const BilinearData&, uint8_t*)> func);
  void DoBicubicLoop(const CalcData& cdata,
                     int increment,
                     std::function<void(const BicubicData&, uint8_t*)> func);
  void DoDownSampleLoop(
      const CalcData& cdata,
      int increment,
      std::function<void(const DownSampleData&, uint8_t*)> func);

  RetainPtr<CFX_DIBBase> const m_pSrc;
  const CFX_Matrix m_matrix;
  FX_RECT m_StretchClip;
  FX_RECT m_result;
  CFX_Matrix m_dest2stretch;
  std::unique_ptr<CFX_ImageStretcher> m_Stretcher;
  CFX_BitmapStorer m_Storer;
  const FXDIB_ResampleOptions m_ResampleOptions;
  int m_Status = 0;
};

#endif  // CORE_FXGE_DIB_CFX_IMAGETRANSFORMER_H_
