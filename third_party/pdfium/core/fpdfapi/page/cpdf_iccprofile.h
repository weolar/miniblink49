// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_ICCPROFILE_H_
#define CORE_FPDFAPI_PAGE_CPDF_ICCPROFILE_H_

#include <memory>

#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/span.h"

class CLcmsCmm;
class CPDF_Stream;

class CPDF_IccProfile final : public Retainable {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  const CPDF_Stream* GetStream() const { return m_pStream.Get(); }
  bool IsValid() const { return IsSRGB() || IsSupported(); }
  bool IsSRGB() const { return m_bsRGB; }
  bool IsSupported() const { return !!m_Transform; }
  CLcmsCmm* transform() { return m_Transform.get(); }
  uint32_t GetComponents() const { return m_nSrcComponents; }

 private:
  CPDF_IccProfile(const CPDF_Stream* pStream, pdfium::span<const uint8_t> span);
  ~CPDF_IccProfile() override;

  const bool m_bsRGB;
  UnownedPtr<const CPDF_Stream> const m_pStream;
  std::unique_ptr<CLcmsCmm> m_Transform;
  uint32_t m_nSrcComponents = 0;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_ICCPROFILE_H_
