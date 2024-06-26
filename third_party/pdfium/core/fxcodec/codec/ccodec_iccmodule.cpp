// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/codec/ccodec_iccmodule.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "core/fxcodec/codec/codec_int.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

// For use with std::unique_ptr<cmsHPROFILE>.
struct CmsProfileDeleter {
  inline void operator()(cmsHPROFILE p) { cmsCloseProfile(p); }
};

using ScopedCmsProfile = std::unique_ptr<void, CmsProfileDeleter>;

bool Check3Components(cmsColorSpaceSignature cs) {
  switch (cs) {
    case cmsSigGrayData:
    case cmsSigCmykData:
      return false;
    default:
      return true;
  }
}

}  // namespace

CLcmsCmm::CLcmsCmm(cmsHTRANSFORM hTransform,
                   int srcComponents,
                   bool bIsLab,
                   bool bNormal)
    : m_hTransform(hTransform),
      m_nSrcComponents(srcComponents),
      m_bLab(bIsLab),
      m_bNormal(bNormal) {}

CLcmsCmm::~CLcmsCmm() {
  cmsDeleteTransform(m_hTransform);
}

CCodec_IccModule::CCodec_IccModule() {}

CCodec_IccModule::~CCodec_IccModule() {}

std::unique_ptr<CLcmsCmm> CCodec_IccModule::CreateTransform_sRGB(
    pdfium::span<const uint8_t> span) {
  ScopedCmsProfile srcProfile(cmsOpenProfileFromMem(span.data(), span.size()));
  if (!srcProfile)
    return nullptr;

  ScopedCmsProfile dstProfile(cmsCreate_sRGBProfile());
  if (!dstProfile)
    return nullptr;

  cmsColorSpaceSignature srcCS = cmsGetColorSpace(srcProfile.get());

  uint32_t nSrcComponents = cmsChannelsOf(srcCS);
  // According to PDF spec, number of components must be 1, 3, or 4.
  if (nSrcComponents != 1 && nSrcComponents != 3 && nSrcComponents != 4)
    return nullptr;

  int srcFormat;
  bool bLab = false;
  bool bNormal = false;
  if (srcCS == cmsSigLabData) {
    srcFormat =
        COLORSPACE_SH(PT_Lab) | CHANNELS_SH(nSrcComponents) | BYTES_SH(0);
    bLab = true;
  } else {
    srcFormat =
        COLORSPACE_SH(PT_ANY) | CHANNELS_SH(nSrcComponents) | BYTES_SH(1);
    // TODO(thestig): Check to see if lcms2 supports more colorspaces that can
    // be considered normal.
    bNormal = srcCS == cmsSigGrayData || srcCS == cmsSigRgbData ||
              srcCS == cmsSigCmykData;
  }
  cmsColorSpaceSignature dstCS = cmsGetColorSpace(dstProfile.get());
  if (!Check3Components(dstCS))
    return nullptr;

  cmsHTRANSFORM hTransform = nullptr;
  const int intent = 0;
  switch (dstCS) {
    case cmsSigRgbData:
      hTransform = cmsCreateTransform(srcProfile.get(), srcFormat,
                                      dstProfile.get(), TYPE_BGR_8, intent, 0);
      break;
    case cmsSigGrayData:
    case cmsSigCmykData:
      // Check3Components() already filtered these types.
      NOTREACHED();
      break;
    default:
      break;
  }
  if (!hTransform)
    return nullptr;

  return pdfium::MakeUnique<CLcmsCmm>(hTransform, nSrcComponents, bLab,
                                      bNormal);
}

void CCodec_IccModule::Translate(CLcmsCmm* pTransform,
                                 const float* pSrcValues,
                                 float* pDestValues) {
  if (!pTransform)
    return;

  uint32_t nSrcComponents = m_nComponents;
  uint8_t output[4];
  // TODO(npm): Currently the CmsDoTransform method is part of LCMS and it will
  // apply some member of m_hTransform to the input. We need to go over all the
  // places which set transform to verify that only nSrcComponents are used.
  if (pTransform->IsLab()) {
    std::vector<double> inputs(std::max(nSrcComponents, 16u));
    for (uint32_t i = 0; i < nSrcComponents; ++i)
      inputs[i] = pSrcValues[i];
    cmsDoTransform(pTransform->transform(), inputs.data(), output, 1);
  } else {
    std::vector<uint8_t> inputs(std::max(nSrcComponents, 16u));
    for (uint32_t i = 0; i < nSrcComponents; ++i) {
      inputs[i] =
          pdfium::clamp(static_cast<int>(pSrcValues[i] * 255.0f), 0, 255);
    }
    cmsDoTransform(pTransform->transform(), inputs.data(), output, 1);
  }
  pDestValues[0] = output[2] / 255.0f;
  pDestValues[1] = output[1] / 255.0f;
  pDestValues[2] = output[0] / 255.0f;
}

void CCodec_IccModule::TranslateScanline(CLcmsCmm* pTransform,
                                         unsigned char* pDest,
                                         const unsigned char* pSrc,
                                         int32_t pixels) {
  if (pTransform)
    cmsDoTransform(pTransform->transform(), pSrc, pDest, pixels);
}
