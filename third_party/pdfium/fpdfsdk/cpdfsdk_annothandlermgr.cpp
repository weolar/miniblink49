// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_annothandlermgr.h"

#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfdoc/cpdf_annot.h"
#include "fpdfsdk/cpdfsdk_annot.h"
#include "fpdfsdk/cpdfsdk_annotiterator.h"
#include "fpdfsdk/cpdfsdk_baannot.h"
#include "fpdfsdk/cpdfsdk_baannothandler.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "fpdfsdk/cpdfsdk_widgethandler.h"
#include "public/fpdf_fwlevent.h"
#include "third_party/base/ptr_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/cpdfsdk_xfawidgethandler.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#endif  // PDF_ENABLE_XFA

CPDFSDK_AnnotHandlerMgr::CPDFSDK_AnnotHandlerMgr(
  CPDFSDK_FormFillEnvironment* pFormFillEnv)
  : m_pBAAnnotHandler(pdfium::MakeUnique<CPDFSDK_BAAnnotHandler>())
  //, m_pWidgetHandler(pdfium::MakeUnique<CPDFSDK_WidgetHandler>(pFormFillEnv))
#ifdef PDF_ENABLE_XFA
  //, m_pXFAWidgetHandler(pdfium::MakeUnique<CPDFSDK_XFAWidgetHandler>(pFormFillEnv))
#endif  // PDF_ENABLE_XFA
{
  DebugBreak();
}

CPDFSDK_AnnotHandlerMgr::~CPDFSDK_AnnotHandlerMgr() {}

CPDFSDK_Annot* CPDFSDK_AnnotHandlerMgr::NewAnnot(CPDF_Annot* pAnnot,
                                                 CPDFSDK_PageView* pPageView) {
  ASSERT(pPageView);
  return GetAnnotHandler(pAnnot->GetSubtype())->NewAnnot(pAnnot, pPageView);
}

#ifdef PDF_ENABLE_XFA
CPDFSDK_Annot* CPDFSDK_AnnotHandlerMgr::NewAnnot(CXFA_FFWidget* pAnnot,
                                                 CPDFSDK_PageView* pPageView) {
  ASSERT(pAnnot);
  ASSERT(pPageView);

  return GetAnnotHandler(CPDF_Annot::Subtype::XFAWIDGET)
      ->NewAnnot(pAnnot, pPageView);
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_AnnotHandlerMgr::ReleaseAnnot(CPDFSDK_Annot* pAnnot) {
  IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot);
  pAnnotHandler->ReleaseAnnot(pAnnot);
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnLoad(CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot);
  GetAnnotHandler(pAnnot)->OnLoad(pAnnot);
}

WideString CPDFSDK_AnnotHandlerMgr::Annot_GetText(CPDFSDK_Annot* pAnnot) {
  return GetAnnotHandler(pAnnot)->GetText(pAnnot);
}

WideString CPDFSDK_AnnotHandlerMgr::Annot_GetSelectedText(
    CPDFSDK_Annot* pAnnot) {
  return GetAnnotHandler(pAnnot)->GetSelectedText(pAnnot);
}

void CPDFSDK_AnnotHandlerMgr::Annot_ReplaceSelection(CPDFSDK_Annot* pAnnot,
                                                     const WideString& text) {
  GetAnnotHandler(pAnnot)->ReplaceSelection(pAnnot, text);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_CanUndo(CPDFSDK_Annot* pAnnot) {
  return GetAnnotHandler(pAnnot)->CanUndo(pAnnot);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_CanRedo(CPDFSDK_Annot* pAnnot) {
  return GetAnnotHandler(pAnnot)->CanRedo(pAnnot);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_Undo(CPDFSDK_Annot* pAnnot) {
  return GetAnnotHandler(pAnnot)->Undo(pAnnot);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_Redo(CPDFSDK_Annot* pAnnot) {
  return GetAnnotHandler(pAnnot)->Redo(pAnnot);
}

IPDFSDK_AnnotHandler* CPDFSDK_AnnotHandlerMgr::GetAnnotHandler(
    CPDFSDK_Annot* pAnnot) const {
  return GetAnnotHandler(pAnnot->GetAnnotSubtype());
}

IPDFSDK_AnnotHandler* CPDFSDK_AnnotHandlerMgr::GetAnnotHandler(
    CPDF_Annot::Subtype nAnnotSubtype) const {
  if (nAnnotSubtype == CPDF_Annot::Subtype::WIDGET) {
    //return m_pWidgetHandler.get();
    DebugBreak();
    return nullptr;
  }

#ifdef PDF_ENABLE_XFA
  if (nAnnotSubtype == CPDF_Annot::Subtype::XFAWIDGET) {
    // return m_pXFAWidgetHandler.get();
    DebugBreak();
    return nullptr;
  }
#endif  // PDF_ENABLE_XFA

  return m_pBAAnnotHandler.get();
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnDraw(CPDFSDK_PageView* pPageView,
                                           CPDFSDK_Annot* pAnnot,
                                           CFX_RenderDevice* pDevice,
                                           const CFX_Matrix& mtUser2Device,
                                           bool bDrawAnnots) {
  ASSERT(pAnnot);
  GetAnnotHandler(pAnnot)->OnDraw(pPageView, pAnnot, pDevice, mtUser2Device,
                                  bDrawAnnots);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnLButtonDown(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())
      ->OnLButtonDown(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnLButtonUp(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())
      ->OnLButtonUp(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnLButtonDblClk(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())
      ->OnLButtonDblClk(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnMouseMove(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())
      ->OnMouseMove(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnMouseWheel(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    short zDelta,
    const CFX_PointF& point) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())
      ->OnMouseWheel(pPageView, pAnnot, nFlags, zDelta, point);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnRButtonDown(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())
      ->OnRButtonDown(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnRButtonUp(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())
      ->OnRButtonUp(pPageView, pAnnot, nFlags, point);
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnMouseEnter(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlag) {
  ASSERT(pAnnot->HasObservable());
  GetAnnotHandler(pAnnot->Get())->OnMouseEnter(pPageView, pAnnot, nFlag);
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnMouseExit(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlag) {
  ASSERT(pAnnot->HasObservable());
  GetAnnotHandler(pAnnot->Get())->OnMouseExit(pPageView, pAnnot, nFlag);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnChar(CPDFSDK_Annot* pAnnot,
                                           uint32_t nChar,
                                           uint32_t nFlags) {
  return GetAnnotHandler(pAnnot)->OnChar(pAnnot, nChar, nFlags);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnKeyDown(CPDFSDK_Annot* pAnnot,
                                              int nKeyCode,
                                              int nFlag) {
  if (CPDFSDK_FormFillEnvironment::IsCTRLKeyDown(nFlag) ||
      CPDFSDK_FormFillEnvironment::IsALTKeyDown(nFlag)) {
    return GetAnnotHandler(pAnnot)->OnKeyDown(pAnnot, nKeyCode, nFlag);
  }

  CPDFSDK_PageView* pPage = pAnnot->GetPageView();
  CPDFSDK_Annot* pFocusAnnot = pPage->GetFocusAnnot();
  if (pFocusAnnot && (nKeyCode == FWL_VKEY_Tab)) {
    CPDFSDK_Annot::ObservedPtr pNext(GetNextAnnot(
        pFocusAnnot, !CPDFSDK_FormFillEnvironment::IsSHIFTKeyDown(nFlag)));
    if (pNext && pNext.Get() != pFocusAnnot) {
      pPage->GetFormFillEnv()->SetFocusAnnot(&pNext);
      return true;
    }
  }

  return GetAnnotHandler(pAnnot)->OnKeyDown(pAnnot, nKeyCode, nFlag);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnSetFocus(
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlag) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())->OnSetFocus(pAnnot, nFlag);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnKillFocus(
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlag) {
  ASSERT(pAnnot->HasObservable());
  return GetAnnotHandler(pAnnot->Get())->OnKillFocus(pAnnot, nFlag);
}

#ifdef PDF_ENABLE_XFA
bool CPDFSDK_AnnotHandlerMgr::Annot_OnChangeFocus(
    CPDFSDK_Annot::ObservedPtr* pSetAnnot,
    CPDFSDK_Annot::ObservedPtr* pKillAnnot) {
  bool bXFA = (*pSetAnnot && (*pSetAnnot)->GetXFAWidget()) ||
              (*pKillAnnot && (*pKillAnnot)->GetXFAWidget());

  if (bXFA) {
    if (IPDFSDK_AnnotHandler* pXFAAnnotHandler =
            GetAnnotHandler(CPDF_Annot::Subtype::XFAWIDGET))
      return pXFAAnnotHandler->OnXFAChangedFocus(pKillAnnot, pSetAnnot);
  }

  return true;
}
#endif  // PDF_ENABLE_XFA

CFX_FloatRect CPDFSDK_AnnotHandlerMgr::Annot_OnGetViewBBox(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot);
  return GetAnnotHandler(pAnnot)->GetViewBBox(pPageView, pAnnot);
}

bool CPDFSDK_AnnotHandlerMgr::Annot_OnHitTest(CPDFSDK_PageView* pPageView,
                                              CPDFSDK_Annot* pAnnot,
                                              const CFX_PointF& point) {
  ASSERT(pAnnot);
  IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot);
  if (pAnnotHandler->CanAnswer(pAnnot))
    return pAnnotHandler->HitTest(pPageView, pAnnot, point);

  return false;
}

CPDFSDK_Annot* CPDFSDK_AnnotHandlerMgr::GetNextAnnot(CPDFSDK_Annot* pSDKAnnot,
                                                     bool bNext) {
#ifdef PDF_ENABLE_XFA
  CPDFSDK_PageView* pPageView = pSDKAnnot->GetPageView();
  CPDFXFA_Page* pPage = pPageView->GetPDFXFAPage();
  if (pPage && !pPage->AsPDFPage()) {
    // For xfa annots in XFA pages not backed by PDF pages.
//     std::unique_ptr<IXFA_WidgetIterator> pWidgetIterator(
//         pPage->GetXFAPageView()->CreateWidgetIterator(
//             XFA_TRAVERSEWAY_Tranvalse, XFA_WidgetStatus_Visible |
//                                            XFA_WidgetStatus_Viewable |
//                                            XFA_WidgetStatus_Focused));
//     if (!pWidgetIterator)
//       return nullptr;
//     if (pWidgetIterator->GetCurrentWidget() != pSDKAnnot->GetXFAWidget())
//       pWidgetIterator->SetCurrentWidget(pSDKAnnot->GetXFAWidget());
//     CXFA_FFWidget* hNextFocus = bNext ? pWidgetIterator->MoveToNext()
//                                       : pWidgetIterator->MoveToPrevious();
//     if (!hNextFocus && pSDKAnnot)
//       hNextFocus = pWidgetIterator->MoveToFirst();
// 
//     return pPageView->GetAnnotByXFAWidget(hNextFocus);
    DebugBreak();
    return nullptr;
  }
#endif  // PDF_ENABLE_XFA

  // For PDF annots.
  CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pSDKAnnot);
  CPDFSDK_AnnotIterator ai(pWidget->GetPageView(), pWidget->GetAnnotSubtype());
  return bNext ? ai.GetNextAnnot(pWidget) : ai.GetPrevAnnot(pWidget);
}
