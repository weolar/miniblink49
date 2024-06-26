// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_iccprofile.h"

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fxcodec/codec/ccodec_iccmodule.h"

namespace {

bool DetectSRGB(pdfium::span<const uint8_t> span) {
  static const char kSRGB[] = "sRGB IEC61966-2.1";
  return span.size() == 3144 && memcmp(&span[400], kSRGB, strlen(kSRGB)) == 0;
}

}  // namespace

CPDF_IccProfile::CPDF_IccProfile(const CPDF_Stream* pStream,
                                 pdfium::span<const uint8_t> span)
    : m_bsRGB(DetectSRGB(span)), m_pStream(pStream) {
  if (m_bsRGB) {
    m_nSrcComponents = 3;
    return;
  }

  auto* pIccModule = CPDF_ModuleMgr::Get()->GetIccModule();
  m_Transform = pIccModule->CreateTransform_sRGB(span);
  if (m_Transform)
    m_nSrcComponents = m_Transform->components();
}

CPDF_IccProfile::~CPDF_IccProfile() {}
