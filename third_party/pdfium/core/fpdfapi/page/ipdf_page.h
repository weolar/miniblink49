// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_IPDF_PAGE_H_
#define CORE_FPDFAPI_PAGE_IPDF_PAGE_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "third_party/base/optional.h"

class CPDF_Document;
class CPDF_Page;

// Small layering violation, incomplete type and always null if non-XFA.
class CPDFXFA_Page;

// Interface implemented by both page types (CPDF_Page and CPDFXFA_Page).
class IPDF_Page : public Retainable {
 public:
  // There are actually 3 cases: a PDF page, an XFA page backed by a PDF page,
  // and an XFA page not backed by a PDF page. AsPDFPage() will return the
  // PDF page in either of the first two cases. AsXFAPage() is a straight
  // downcast and is null if not either of the last two cases. Hence, both
  // of these may return non-null on a given page.
  virtual CPDF_Page* AsPDFPage() = 0;
  virtual CPDFXFA_Page* AsXFAPage() = 0;

  virtual CPDF_Document* GetDocument() const = 0;

  virtual float GetPageWidth() const = 0;
  virtual float GetPageHeight() const = 0;
  virtual CFX_Matrix GetDisplayMatrix(const FX_RECT& rect,
                                      int iRotate) const = 0;

  virtual Optional<CFX_PointF> DeviceToPage(
      const FX_RECT& rect,
      int rotate,
      const CFX_PointF& device_point) const = 0;

  virtual Optional<CFX_PointF> PageToDevice(
      const FX_RECT& rect,
      int rotate,
      const CFX_PointF& page_point) const = 0;
};

inline CPDF_Page* ToPDFPage(IPDF_Page* pBase) {
  return pBase ? pBase->AsPDFPage() : nullptr;
}

inline CPDFXFA_Page* ToXFAPage(IPDF_Page* pBase) {
  return pBase ? pBase->AsXFAPage() : nullptr;
}

#endif  // CORE_FPDFAPI_PAGE_IPDF_PAGE_H_
