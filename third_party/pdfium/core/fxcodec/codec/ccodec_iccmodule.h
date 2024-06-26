// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_ICCMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_ICCMODULE_H_

#include <memory>

#include "core/fxcodec/fx_codec_def.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

#if defined(USE_SYSTEM_LCMS2)
#include <lcms2.h>
#else
#include "third_party/lcms/include/lcms2.h"
#endif

class CLcmsCmm {
 public:
  CLcmsCmm(cmsHTRANSFORM transform,
           int srcComponents,
           bool bIsLab,
           bool bNormal);
  ~CLcmsCmm();

  cmsHTRANSFORM transform() const { return m_hTransform; }
  int components() const { return m_nSrcComponents; }
  bool IsLab() const { return m_bLab; }
  bool IsNormal() const { return m_bNormal; }

 private:
  const cmsHTRANSFORM m_hTransform;
  const int m_nSrcComponents;
  const bool m_bLab;
  const bool m_bNormal;
};

class CCodec_IccModule {
 public:
  CCodec_IccModule();
  ~CCodec_IccModule();

  std::unique_ptr<CLcmsCmm> CreateTransform_sRGB(
      pdfium::span<const uint8_t> span);
  void Translate(CLcmsCmm* pTransform,
                 const float* pSrcValues,
                 float* pDestValues);
  void TranslateScanline(CLcmsCmm* pTransform,
                         uint8_t* pDest,
                         const uint8_t* pSrc,
                         int pixels);
  void SetComponents(uint32_t nComponents) { m_nComponents = nComponents; }

 protected:
  uint32_t m_nComponents = 0;
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_ICCMODULE_H_
