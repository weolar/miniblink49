// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_COLOR_H_
#define CORE_FPDFAPI_PAGE_CPDF_COLOR_H_

#include <vector>

#include "core/fpdfapi/page/cpdf_colorspace.h"
#include "core/fxcrt/fx_system.h"

class CPDF_Pattern;

class CPDF_Color {
 public:
  CPDF_Color();
  CPDF_Color(const CPDF_Color& that);

  ~CPDF_Color();

  CPDF_Color& operator=(const CPDF_Color& that);

  bool IsNull() const { return !m_pBuffer; }
  bool IsPattern() const;
  void SetColorSpace(CPDF_ColorSpace* pCS);
  void SetValueForNonPattern(const std::vector<float>& values);
  void SetValueForPattern(CPDF_Pattern* pPattern,
                          const std::vector<float>& values);
  uint32_t CountComponents() const;
  bool IsColorSpaceRGB() const;
  bool GetRGB(int* R, int* G, int* B) const;

  // Should only be called if IsPattern() returns true.
  CPDF_Pattern* GetPattern() const;

 protected:
  void ReleaseBuffer();
  void ReleaseColorSpace();
  bool IsPatternInternal() const;

  // TODO(thestig): Convert this to a smart pointer or vector.
  // |m_pBuffer| is created by |m_pCS|, so if it is non-null, then so is
  // |m_pCS|.
  float* m_pBuffer = nullptr;
  CPDF_ColorSpace* m_pCS = nullptr;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_COLOR_H_
