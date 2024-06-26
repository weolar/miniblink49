// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CFX_FILTEREDDIB_H_
#define CORE_FXGE_DIB_CFX_FILTEREDDIB_H_

#include <vector>

#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/dib/cfx_dibbase.h"

class CFX_FilteredDIB : public CFX_DIBBase {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  virtual FXDIB_Format GetDestFormat() = 0;
  virtual uint32_t* GetDestPalette() = 0;
  virtual void TranslateScanline(const uint8_t* src_buf,
                                 std::vector<uint8_t>* dest_buf) const = 0;
  virtual void TranslateDownSamples(uint8_t* dest_buf,
                                    const uint8_t* src_buf,
                                    int pixels,
                                    int Bpp) const = 0;

  void LoadSrc(const RetainPtr<CFX_DIBBase>& pSrc);

 protected:
  CFX_FilteredDIB();
  ~CFX_FilteredDIB() override;

  // CFX_DIBBase
  const uint8_t* GetScanline(int line) const override;
  void DownSampleScanline(int line,
                          uint8_t* dest_scan,
                          int dest_bpp,
                          int dest_width,
                          bool bFlipX,
                          int clip_left,
                          int clip_width) const override;

  RetainPtr<CFX_DIBBase> m_pSrc;
  mutable std::vector<uint8_t> m_Scanline;
};

#endif  // CORE_FXGE_DIB_CFX_FILTEREDDIB_H_
