// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FPDFXFA_CPDFXFA_PAGE_H_
#define FPDFSDK_FPDFXFA_CPDFXFA_PAGE_H_

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/ipdf_page.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/optional.h"

class CPDF_Dictionary;
class CPDFXFA_Context;
class CXFA_FFPageView;

class CPDFXFA_Page final : public IPDF_Page {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  // IPDF_Page:
  CPDF_Page* AsPDFPage() override;
  CPDFXFA_Page* AsXFAPage() override;
  CPDF_Document* GetDocument() const override;
  float GetPageWidth() const override;
  float GetPageHeight() const override;
  CFX_Matrix GetDisplayMatrix(const FX_RECT& rect, int iRotate) const override;
  Optional<CFX_PointF> DeviceToPage(
      const FX_RECT& rect,
      int rotate,
      const CFX_PointF& device_point) const override;
  Optional<CFX_PointF> PageToDevice(
      const FX_RECT& rect,
      int rotate,
      const CFX_PointF& page_point) const override;

  bool LoadPage();
  void LoadPDFPageFromDict(CPDF_Dictionary* pPageDict);
  CPDF_Document::Extension* GetDocumentExtension() const;
  int GetPageIndex() const { return m_iPageIndex; }
  void SetXFAPageViewIndex(int index) { m_iPageIndex = index; }
  CXFA_FFPageView* GetXFAPageView() const;

 private:
  // Refcounted class.
  CPDFXFA_Page(CPDFXFA_Context* pContext, int page_index);
  ~CPDFXFA_Page() override;

  bool LoadPDFPage();

  RetainPtr<CPDF_Page> m_pPDFPage;
  UnownedPtr<CPDFXFA_Context> const m_pContext;
  int m_iPageIndex;
};

#endif  // FPDFSDK_FPDFXFA_CPDFXFA_PAGE_H_
