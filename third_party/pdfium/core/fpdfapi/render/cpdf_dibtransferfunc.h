// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_DIBTRANSFERFUNC_H_
#define CORE_FPDFAPI_RENDER_CPDF_DIBTRANSFERFUNC_H_

#include <vector>

#include "core/fxge/dib/cfx_filtereddib.h"
#include "core/fxge/fx_dib.h"
#include "third_party/base/span.h"

class CPDF_TransferFunc;

class CPDF_DIBTransferFunc final : public CFX_FilteredDIB {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  // CFX_FilteredDIB
  FXDIB_Format GetDestFormat() override;
  FX_ARGB* GetDestPalette() override;
  void TranslateScanline(const uint8_t* src_buf,
                         std::vector<uint8_t>* dest_buf) const override;
  void TranslateDownSamples(uint8_t* dest_buf,
                            const uint8_t* src_buf,
                            int pixels,
                            int Bpp) const override;

 private:
  explicit CPDF_DIBTransferFunc(
      const RetainPtr<CPDF_TransferFunc>& pTransferFunc);
  ~CPDF_DIBTransferFunc() override;

  RetainPtr<CPDF_TransferFunc> m_pTransferFunc;
  const pdfium::span<const uint8_t> m_RampR;
  const pdfium::span<const uint8_t> m_RampG;
  const pdfium::span<const uint8_t> m_RampB;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_DIBTRANSFERFUNC_H_
