// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_pageview.h"

#include <memory>
#include <vector>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfdoc/cpdf_annotlist.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "core/fxcrt/autorestorer.h"
#include "fpdfsdk/cpdfsdk_annot.h"
#include "fpdfsdk/cpdfsdk_annotiteration.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#include "xfa/fxfa/cxfa_rendercontext.h"
#include "xfa/fxgraphics/cxfa_graphics.h"
#endif  // PDF_ENABLE_XFA

CPDFSDK_PageView::CPDFSDK_PageView(CPDFSDK_FormFillEnvironment* pFormFillEnv,
                                   IPDF_Page* page)
    : m_page(page), m_pFormFillEnv(pFormFillEnv) {
  ASSERT(m_page);
  CPDF_Page* pPDFPage = ToPDFPage(page);
  if (pPDFPage) {
    CPDFSDK_InteractiveForm* pForm = pFormFillEnv->GetInteractiveForm();
    CPDF_InteractiveForm* pPDFForm = pForm->GetInteractiveForm();
    pPDFForm->FixPageFields(pPDFPage);
    if (!page->AsXFAPage())
      pPDFPage->SetView(this);
  }
}

CPDFSDK_PageView::~CPDFSDK_PageView() {
  if (!m_page->AsXFAPage()) {
    // The call to |ReleaseAnnot| can cause the page pointed to by |m_page| to
    // be freed, which will cause issues if we try to cleanup the pageview
    // pointer in |m_page|. So, reset the pageview pointer before doing anything
    // else.
    m_page->AsPDFPage()->SetView(nullptr);
  }

  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  for (CPDFSDK_Annot* pAnnot : m_SDKAnnotArray)
    pAnnotHandlerMgr->ReleaseAnnot(pAnnot);

  m_SDKAnnotArray.clear();
  m_pAnnotList.reset();
}

void CPDFSDK_PageView::PageView_OnDraw(CFX_RenderDevice* pDevice,
                                       const CFX_Matrix& mtUser2Device,
                                       CPDF_RenderOptions* pOptions,
                                       const FX_RECT& pClip) {
  m_curMatrix = mtUser2Device;

#ifdef PDF_ENABLE_XFA
  CPDFXFA_Page* pPage = GetPDFXFAPage();
  if (!pPage)
    return;

  auto* pContext = static_cast<CPDFXFA_Context*>(pPage->GetDocumentExtension());
  if (pContext->GetFormType() == FormType::kXFAFull) {
//     CFX_RectF rectClip(
//         static_cast<float>(pClip.left), static_cast<float>(pClip.top),
//         static_cast<float>(pClip.Width()), static_cast<float>(pClip.Height()));
// 
//     CXFA_Graphics gs(pDevice);
//     gs.SetClipRect(rectClip);
// 
//     CXFA_FFPageView* xfaView = pPage->GetXFAPageView();
//     CXFA_RenderContext renderContext(xfaView, rectClip, mtUser2Device);
//     renderContext.DoRender(&gs);
//
//     CXFA_FFDocView* docView = xfaView->GetDocView();
//     if (!docView)
//       return;
//     CPDFSDK_Annot* annot = GetFocusAnnot();
//     if (!annot)
//       return;
//     // Render the focus widget
//     docView->GetWidgetHandler()->RenderWidget(annot->GetXFAWidget(), &gs, mtUser2Device, false);
    DebugBreak();
    return;
  }
#endif  // PDF_ENABLE_XFA

  // for pdf/static xfa.
  CPDFSDK_AnnotIteration annotIteration(this, true);
  for (const auto& pSDKAnnot : annotIteration) {
    m_pFormFillEnv->GetAnnotHandlerMgr()->Annot_OnDraw(
        this, pSDKAnnot.Get(), pDevice, mtUser2Device,
        pOptions->GetDrawAnnots());
  }
}

CPDFSDK_Annot* CPDFSDK_PageView::GetFXAnnotAtPoint(const CFX_PointF& point) {
  CPDFSDK_AnnotHandlerMgr* pAnnotMgr = m_pFormFillEnv->GetAnnotHandlerMgr();
  CPDFSDK_AnnotIteration annotIteration(this, false);
  for (const auto& pSDKAnnot : annotIteration) {
    CFX_FloatRect rc = pAnnotMgr->Annot_OnGetViewBBox(this, pSDKAnnot.Get());
    if (pSDKAnnot->GetAnnotSubtype() == CPDF_Annot::Subtype::POPUP)
      continue;
    if (rc.Contains(point))
      return pSDKAnnot.Get();
  }
  return nullptr;
}

CPDFSDK_Annot* CPDFSDK_PageView::GetFXWidgetAtPoint(const CFX_PointF& point) {
  CPDFSDK_AnnotHandlerMgr* pAnnotMgr = m_pFormFillEnv->GetAnnotHandlerMgr();
  CPDFSDK_AnnotIteration annotIteration(this, false);
  for (const auto& pSDKAnnot : annotIteration) {
    bool bHitTest = pSDKAnnot->GetAnnotSubtype() == CPDF_Annot::Subtype::WIDGET;
#ifdef PDF_ENABLE_XFA
    bHitTest = bHitTest ||
               pSDKAnnot->GetAnnotSubtype() == CPDF_Annot::Subtype::XFAWIDGET;
#endif  // PDF_ENABLE_XFA
    if (bHitTest) {
      pAnnotMgr->Annot_OnGetViewBBox(this, pSDKAnnot.Get());
      if (pAnnotMgr->Annot_OnHitTest(this, pSDKAnnot.Get(), point))
        return pSDKAnnot.Get();
    }
  }
  return nullptr;
}

#ifdef PDF_ENABLE_XFA
CPDFSDK_Annot* CPDFSDK_PageView::AddAnnot(CXFA_FFWidget* pPDFAnnot) {
  if (!pPDFAnnot)
    return nullptr;

  CPDFSDK_Annot* pSDKAnnot = GetAnnotByXFAWidget(pPDFAnnot);
  if (pSDKAnnot)
    return pSDKAnnot;

  CPDFSDK_AnnotHandlerMgr* pAnnotHandler = m_pFormFillEnv->GetAnnotHandlerMgr();
  pSDKAnnot = pAnnotHandler->NewAnnot(pPDFAnnot, this);
  if (!pSDKAnnot)
    return nullptr;

  m_SDKAnnotArray.push_back(pSDKAnnot);
  return pSDKAnnot;
}

bool CPDFSDK_PageView::DeleteAnnot(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return false;

  CPDFXFA_Page* pPage = pAnnot->GetPDFXFAPage();
  if (!pPage)
    return false;

  auto* pContext = static_cast<CPDFXFA_Context*>(pPage->GetDocumentExtension());
  if (!pContext->ContainsXFAForm())
    return false;

  CPDFSDK_Annot::ObservedPtr pObserved(pAnnot);
  if (GetFocusAnnot() == pAnnot)
    m_pFormFillEnv->KillFocusAnnot(0);  // May invoke JS, invalidating pAnnot.

  if (pObserved) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandler =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    if (pAnnotHandler)
      pAnnotHandler->ReleaseAnnot(pObserved.Get());
  }

  auto it = std::find(m_SDKAnnotArray.begin(), m_SDKAnnotArray.end(), pAnnot);
  if (it != m_SDKAnnotArray.end())
    m_SDKAnnotArray.erase(it);
  if (m_pCaptureWidget.Get() == pAnnot)
    m_pCaptureWidget.Reset();

  return true;
}
#endif  // PDF_ENABLE_XFA

CPDF_Document* CPDFSDK_PageView::GetPDFDocument() {
  return m_page->GetDocument();
}

CPDF_Page* CPDFSDK_PageView::GetPDFPage() const {
  return ToPDFPage(m_page);
}

CPDFSDK_Annot* CPDFSDK_PageView::GetAnnotByDict(CPDF_Dictionary* pDict) {
  for (CPDFSDK_Annot* pAnnot : m_SDKAnnotArray) {
    if (pAnnot->GetPDFAnnot()->GetAnnotDict() == pDict)
      return pAnnot;
  }
  return nullptr;
}

#ifdef PDF_ENABLE_XFA
CPDFSDK_Annot* CPDFSDK_PageView::GetAnnotByXFAWidget(CXFA_FFWidget* hWidget) {
  if (!hWidget)
    return nullptr;

  for (CPDFSDK_Annot* pAnnot : m_SDKAnnotArray) {
    if (pAnnot->GetXFAWidget() == hWidget)
      return pAnnot;
  }
  return nullptr;
}
#endif  // PDF_ENABLE_XFA

WideString CPDFSDK_PageView::GetFocusedFormText() {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_GetText(pAnnot);
  }

  return WideString();
}

WideString CPDFSDK_PageView::GetSelectedText() {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_GetSelectedText(pAnnot);
  }

  return WideString();
}

void CPDFSDK_PageView::ReplaceSelection(const WideString& text) {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    pAnnotHandlerMgr->Annot_ReplaceSelection(pAnnot, text);
  }
}

bool CPDFSDK_PageView::CanUndo() {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_CanUndo(pAnnot);
  }
  return false;
}

bool CPDFSDK_PageView::CanRedo() {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_CanRedo(pAnnot);
  }
  return false;
}

bool CPDFSDK_PageView::Undo() {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_Undo(pAnnot);
  }
  return false;
}

bool CPDFSDK_PageView::Redo() {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_Redo(pAnnot);
  }
  return false;
}

bool CPDFSDK_PageView::OnFocus(const CFX_PointF& point, uint32_t nFlag) {
  CPDFSDK_Annot::ObservedPtr pAnnot(GetFXWidgetAtPoint(point));
  if (!pAnnot) {
    m_pFormFillEnv->KillFocusAnnot(nFlag);
    return false;
  }

  m_pFormFillEnv->SetFocusAnnot(&pAnnot);
  return true;
}

bool CPDFSDK_PageView::OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) {
  CPDFSDK_Annot::ObservedPtr pAnnot(GetFXWidgetAtPoint(point));
  if (!pAnnot) {
    m_pFormFillEnv->KillFocusAnnot(nFlag);
    return false;
  }

  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  if (!pAnnotHandlerMgr->Annot_OnLButtonDown(this, &pAnnot, nFlag, point))
    return false;

  if (!pAnnot)
    return false;

  m_pFormFillEnv->SetFocusAnnot(&pAnnot);
  return true;
}

bool CPDFSDK_PageView::OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) {
  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  CPDFSDK_Annot::ObservedPtr pFXAnnot(GetFXWidgetAtPoint(point));
  CPDFSDK_Annot::ObservedPtr pFocusAnnot(GetFocusAnnot());
  if (pFocusAnnot && pFocusAnnot != pFXAnnot) {
    // Last focus Annot gets a chance to handle the event.
    if (pAnnotHandlerMgr->Annot_OnLButtonUp(this, &pFocusAnnot, nFlag, point))
      return true;
  }
  return pFXAnnot &&
         pAnnotHandlerMgr->Annot_OnLButtonUp(this, &pFXAnnot, nFlag, point);
}

bool CPDFSDK_PageView::OnLButtonDblClk(const CFX_PointF& point,
                                       uint32_t nFlag) {
  CPDFSDK_Annot::ObservedPtr pAnnot(GetFXWidgetAtPoint(point));
  if (!pAnnot) {
    m_pFormFillEnv->KillFocusAnnot(nFlag);
    return false;
  }

  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  if (!pAnnotHandlerMgr->Annot_OnLButtonDblClk(this, &pAnnot, nFlag, point))
    return false;

  if (!pAnnot)
    return false;

  m_pFormFillEnv->SetFocusAnnot(&pAnnot);
  return true;
}

#ifdef PDF_ENABLE_XFA
bool CPDFSDK_PageView::OnRButtonDown(const CFX_PointF& point, uint32_t nFlag) {
  CPDFSDK_Annot::ObservedPtr pAnnot(GetFXWidgetAtPoint(point));
  if (!pAnnot)
    return false;

  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  bool ok = pAnnotHandlerMgr->Annot_OnRButtonDown(this, &pAnnot, nFlag, point);
  if (!pAnnot)
    return false;

  if (ok)
    m_pFormFillEnv->SetFocusAnnot(&pAnnot);

  return true;
}

bool CPDFSDK_PageView::OnRButtonUp(const CFX_PointF& point, uint32_t nFlag) {
  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  CPDFSDK_Annot::ObservedPtr pFXAnnot(GetFXWidgetAtPoint(point));
  if (!pFXAnnot)
    return false;

  if (pAnnotHandlerMgr->Annot_OnRButtonUp(this, &pFXAnnot, nFlag, point))
    m_pFormFillEnv->SetFocusAnnot(&pFXAnnot);

  return true;
}
#endif  // PDF_ENABLE_XFA

bool CPDFSDK_PageView::OnMouseMove(const CFX_PointF& point, int nFlag) {
  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  CPDFSDK_Annot::ObservedPtr pFXAnnot(GetFXAnnotAtPoint(point));

  if (m_bOnWidget && m_pCaptureWidget != pFXAnnot)
    ExitWidget(pAnnotHandlerMgr, true, nFlag);

  if (pFXAnnot) {
    if (!m_bOnWidget) {
      EnterWidget(pAnnotHandlerMgr, &pFXAnnot, nFlag);

      // Annot_OnMouseEnter may have invalidated pFXAnnot.
      if (!pFXAnnot) {
        ExitWidget(pAnnotHandlerMgr, false, nFlag);
        return true;
      }
    }

    pAnnotHandlerMgr->Annot_OnMouseMove(this, &pFXAnnot, nFlag, point);
    return true;
  }

  return false;
}

void CPDFSDK_PageView::EnterWidget(CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr,
                                   CPDFSDK_Annot::ObservedPtr* pAnnot,
                                   uint32_t nFlag) {
  m_bOnWidget = true;
  m_pCaptureWidget.Reset(pAnnot->Get());
  pAnnotHandlerMgr->Annot_OnMouseEnter(this, pAnnot, nFlag);
}

void CPDFSDK_PageView::ExitWidget(CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr,
                                  bool callExitCallback,
                                  uint32_t nFlag) {
  m_bOnWidget = false;
  if (m_pCaptureWidget) {
    if (callExitCallback)
      pAnnotHandlerMgr->Annot_OnMouseExit(this, &m_pCaptureWidget, nFlag);

    m_pCaptureWidget.Reset();
  }
}

bool CPDFSDK_PageView::OnMouseWheel(double deltaX,
                                    double deltaY,
                                    const CFX_PointF& point,
                                    int nFlag) {
  CPDFSDK_Annot::ObservedPtr pAnnot(GetFXWidgetAtPoint(point));
  if (!pAnnot)
    return false;

  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();
  return pAnnotHandlerMgr->Annot_OnMouseWheel(this, &pAnnot, nFlag,
                                              static_cast<int>(deltaY), point);
}

bool CPDFSDK_PageView::OnChar(int nChar, uint32_t nFlag) {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_OnChar(pAnnot, nChar, nFlag);
  }

  return false;
}

bool CPDFSDK_PageView::OnKeyDown(int nKeyCode, int nFlag) {
  if (CPDFSDK_Annot* pAnnot = GetFocusAnnot()) {
    CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
        m_pFormFillEnv->GetAnnotHandlerMgr();
    return pAnnotHandlerMgr->Annot_OnKeyDown(pAnnot, nKeyCode, nFlag);
  }
  return false;
}

bool CPDFSDK_PageView::OnKeyUp(int nKeyCode, int nFlag) {
  return false;
}

void CPDFSDK_PageView::LoadFXAnnots() {
  CPDFSDK_AnnotHandlerMgr* pAnnotHandlerMgr =
      m_pFormFillEnv->GetAnnotHandlerMgr();

  AutoRestorer<bool> lock(&m_bLocked);
  m_bLocked = true;

// #ifdef PDF_ENABLE_XFA
//   RetainPtr<CPDFXFA_Page> protector(ToXFAPage(m_page));
//   if (m_pFormFillEnv->GetXFAContext()->GetFormType() == FormType::kXFAFull) {
//     CXFA_FFPageView* pageView = protector->GetXFAPageView();
//     std::unique_ptr<IXFA_WidgetIterator> pWidgetHandler(
//         pageView->CreateWidgetIterator(
//             XFA_TRAVERSEWAY_Form,
//             XFA_WidgetStatus_Visible | XFA_WidgetStatus_Viewable));
//     if (!pWidgetHandler) {
//       return;
//     }
// 
//     while (CXFA_FFWidget* pXFAAnnot = pWidgetHandler->MoveToNext()) {
//       CPDFSDK_Annot* pAnnot = pAnnotHandlerMgr->NewAnnot(pXFAAnnot, this);
//       if (!pAnnot)
//         continue;
//       m_SDKAnnotArray.push_back(pAnnot);
//       pAnnotHandlerMgr->Annot_OnLoad(pAnnot);
//     }
// 
//     return;
//   }
// #endif  // PDF_ENABLE_XFA
  DebugBreak();

  CPDF_Page* pPage = GetPDFPage();
  ASSERT(pPage);
  bool bUpdateAP = CPDF_InteractiveForm::IsUpdateAPEnabled();
  // Disable the default AP construction.
  CPDF_InteractiveForm::SetUpdateAP(false);
  m_pAnnotList = pdfium::MakeUnique<CPDF_AnnotList>(pPage);
  CPDF_InteractiveForm::SetUpdateAP(bUpdateAP);

  const size_t nCount = m_pAnnotList->Count();
  for (size_t i = 0; i < nCount; ++i) {
    CPDF_Annot* pPDFAnnot = m_pAnnotList->GetAt(i);
    CheckUnSupportAnnot(GetPDFDocument(), pPDFAnnot);
    CPDFSDK_Annot* pAnnot = pAnnotHandlerMgr->NewAnnot(pPDFAnnot, this);
    if (!pAnnot)
      continue;
    m_SDKAnnotArray.push_back(pAnnot);
    pAnnotHandlerMgr->Annot_OnLoad(pAnnot);
  }
}

void CPDFSDK_PageView::UpdateRects(const std::vector<CFX_FloatRect>& rects) {
  for (const auto& rc : rects)
    m_pFormFillEnv->Invalidate(m_page, rc.GetOuterRect());
}

void CPDFSDK_PageView::UpdateView(CPDFSDK_Annot* pAnnot) {
  CFX_FloatRect rcWindow = pAnnot->GetRect();
  m_pFormFillEnv->Invalidate(m_page, rcWindow.GetOuterRect());
}

int CPDFSDK_PageView::GetPageIndex() const {
#ifdef PDF_ENABLE_XFA
//   auto* pContext = static_cast<CPDFXFA_Context*>(
//       m_page->AsXFAPage()->GetDocumentExtension());
//   switch (pContext->GetFormType()) {
//     case FormType::kXFAFull: {
//       CXFA_FFPageView* pPageView = m_page->AsXFAPage()->GetXFAPageView();
//       return pPageView ? pPageView->GetPageIndex() : -1;
//     }
//     case FormType::kNone:
//     case FormType::kAcroForm:
//     case FormType::kXFAForeground:
//       break;
//   }
  DebugBreak();
#endif  // PDF_ENABLE_XFA
  return GetPageIndexForStaticPDF();
}

bool CPDFSDK_PageView::IsValidAnnot(const CPDF_Annot* p) const {
  if (!p)
    return false;

  const auto& annots = m_pAnnotList->All();
  auto it = std::find_if(annots.begin(), annots.end(),
                         [p](const std::unique_ptr<CPDF_Annot>& annot) {
                           return annot.get() == p;
                         });
  return it != annots.end();
}

bool CPDFSDK_PageView::IsValidSDKAnnot(const CPDFSDK_Annot* p) const {
  if (!p)
    return false;
  return pdfium::ContainsValue(m_SDKAnnotArray, p);
}

CPDFSDK_Annot* CPDFSDK_PageView::GetFocusAnnot() {
  CPDFSDK_Annot* pFocusAnnot = m_pFormFillEnv->GetFocusAnnot();
  return IsValidSDKAnnot(pFocusAnnot) ? pFocusAnnot : nullptr;
}

int CPDFSDK_PageView::GetPageIndexForStaticPDF() const {
  const CPDF_Dictionary* pDict = GetPDFPage()->GetDict();
  CPDF_Document* pDoc = m_pFormFillEnv->GetPDFDocument();
  return (pDoc && pDict) ? pDoc->GetPageIndex(pDict->GetObjNum()) : -1;
}
