// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "fpdfsdk/fpdfxfa/cxfa_fwladaptertimermgr.h"
#include "public/fpdf_formfill.h"
#include "third_party/base/compiler_specific.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffpageview.h"

CPDFXFA_Page::CPDFXFA_Page(CPDFXFA_Context* pContext, int page_index)
    : m_pContext(pContext), m_iPageIndex(page_index) {
  ASSERT(m_pContext);
  ASSERT(m_iPageIndex >= 0);
}

CPDFXFA_Page::~CPDFXFA_Page() = default;

CPDF_Page* CPDFXFA_Page::AsPDFPage() {
  return m_pPDFPage.Get();
}

CPDFXFA_Page* CPDFXFA_Page::AsXFAPage() {
  return this;
}

CPDF_Document* CPDFXFA_Page::GetDocument() const {
  return m_pContext->GetPDFDoc();
}

bool CPDFXFA_Page::LoadPDFPage() {
  CPDF_Document* pPDFDoc = GetDocument();
  CPDF_Dictionary* pDict = pPDFDoc->GetPageDictionary(m_iPageIndex);
  if (!pDict)
    return false;

  if (!m_pPDFPage || m_pPDFPage->GetDict() != pDict) {
    m_pPDFPage = pdfium::MakeRetain<CPDF_Page>(pPDFDoc, pDict, true);
    m_pPDFPage->ParseContent();
  }
  return true;
}

CXFA_FFPageView* CPDFXFA_Page::GetXFAPageView() const {
//   CXFA_FFDocView* pXFADocView = m_pContext->GetXFADocView();
//   return pXFADocView ? pXFADocView->GetPageView(m_iPageIndex) : nullptr;
  //DebugBreak();
  return nullptr;
}

bool CPDFXFA_Page::LoadPage() {
  switch (m_pContext->GetFormType()) {
    case FormType::kNone:
    case FormType::kAcroForm:
    case FormType::kXFAForeground:
      return LoadPDFPage();
    case FormType::kXFAFull:
      return !!GetXFAPageView();
  }
  NOTREACHED();
  return false;
}

void CPDFXFA_Page::LoadPDFPageFromDict(CPDF_Dictionary* pPageDict) {
  ASSERT(pPageDict);
  m_pPDFPage = pdfium::MakeRetain<CPDF_Page>(GetDocument(), pPageDict, true);
  m_pPDFPage->ParseContent();
}

CPDF_Document::Extension* CPDFXFA_Page::GetDocumentExtension() const {
  return m_pContext.Get();
}

float CPDFXFA_Page::GetPageWidth() const {
  CXFA_FFPageView* pPageView = GetXFAPageView();
  if (!m_pPDFPage && !pPageView)
    return 0.0f;

  switch (m_pContext->GetFormType()) {
    case FormType::kNone:
    case FormType::kAcroForm:
    case FormType::kXFAForeground:
      if (m_pPDFPage)
        return m_pPDFPage->GetPageWidth();
      FALLTHROUGH;
    case FormType::kXFAFull:
//       if (pPageView)
//         return pPageView->GetPageViewRect().width;
      DebugBreak();
      break;
  }

  return 0.0f;
}

float CPDFXFA_Page::GetPageHeight() const {
  CXFA_FFPageView* pPageView = GetXFAPageView();
  if (!m_pPDFPage && !pPageView)
    return 0.0f;

  switch (m_pContext->GetFormType()) {
    case FormType::kNone:
    case FormType::kAcroForm:
    case FormType::kXFAForeground:
      if (m_pPDFPage)
        return m_pPDFPage->GetPageHeight();
      FALLTHROUGH;
    case FormType::kXFAFull:
//       if (pPageView)
//         return pPageView->GetPageViewRect().height;
      DebugBreak();
      break;
  }

  return 0.0f;
}

Optional<CFX_PointF> CPDFXFA_Page::DeviceToPage(
    const FX_RECT& rect,
    int rotate,
    const CFX_PointF& device_point) const {
  CXFA_FFPageView* pPageView = GetXFAPageView();
  if (!m_pPDFPage && !pPageView)
    return {};

  CFX_PointF pos =
      GetDisplayMatrix(rect, rotate).GetInverse().Transform(device_point);
  return pos;
}

Optional<CFX_PointF> CPDFXFA_Page::PageToDevice(
    const FX_RECT& rect,
    int rotate,
    const CFX_PointF& page_point) const {
  CXFA_FFPageView* pPageView = GetXFAPageView();
  if (!m_pPDFPage && !pPageView)
    return {};

  CFX_Matrix page2device = GetDisplayMatrix(rect, rotate);
  return page2device.Transform(page_point);
}

CFX_Matrix CPDFXFA_Page::GetDisplayMatrix(const FX_RECT& rect,
                                          int iRotate) const {
  CXFA_FFPageView* pPageView = GetXFAPageView();
  if (!m_pPDFPage && !pPageView)
    return CFX_Matrix();

  switch (m_pContext->GetFormType()) {
    case FormType::kNone:
    case FormType::kAcroForm:
    case FormType::kXFAForeground:
      if (m_pPDFPage)
        return m_pPDFPage->GetDisplayMatrix(rect, iRotate);
      FALLTHROUGH;
    case FormType::kXFAFull:
//       if (pPageView)
//         return pPageView->GetDisplayMatrix(rect, iRotate);
      DebugBreak();
      break;
  }

  return CFX_Matrix();
}
