// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_xfawidgethandler.h"

#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "fpdfsdk/cpdfsdk_annot.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/cpdfsdk_xfawidget.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "public/fpdf_fwlevent.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/fwl_widgethit.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#include "xfa/fxfa/fxfa_basic.h"
//#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

CPDFSDK_XFAWidgetHandler::CPDFSDK_XFAWidgetHandler(
    CPDFSDK_FormFillEnvironment* pFormFillEnv)
    : m_pFormFillEnv(pFormFillEnv) {}

CPDFSDK_XFAWidgetHandler::~CPDFSDK_XFAWidgetHandler() {}

bool CPDFSDK_XFAWidgetHandler::CanAnswer(CPDFSDK_Annot* pAnnot) {
  return !!pAnnot->GetXFAWidget();
}

CPDFSDK_Annot* CPDFSDK_XFAWidgetHandler::NewAnnot(CPDF_Annot* pAnnot,
                                                  CPDFSDK_PageView* pPage) {
  return nullptr;
}

CPDFSDK_Annot* CPDFSDK_XFAWidgetHandler::NewAnnot(CXFA_FFWidget* pAnnot,
                                                  CPDFSDK_PageView* pPage) {
  CPDFSDK_InteractiveForm* pForm = m_pFormFillEnv->GetInteractiveForm();
  CPDFSDK_XFAWidget* pWidget = new CPDFSDK_XFAWidget(pAnnot, pPage, pForm);
  pForm->AddXFAMap(pAnnot, pWidget);
  return pWidget;
}

void CPDFSDK_XFAWidgetHandler::OnDraw(CPDFSDK_PageView* pPageView,
                                      CPDFSDK_Annot* pAnnot,
                                      CFX_RenderDevice* pDevice,
                                      const CFX_Matrix& mtUser2Device,
                                      bool bDrawAnnots) {
  ASSERT(pPageView);
  ASSERT(pAnnot);

  CXFA_Graphics gs(pDevice);

  bool bIsHighlight = false;
  if (pPageView->GetFormFillEnv()->GetFocusAnnot() != pAnnot)
    bIsHighlight = true;

  GetXFAWidgetHandler(pAnnot)->RenderWidget(pAnnot->GetXFAWidget(), &gs,
                                            mtUser2Device, bIsHighlight);

  // to do highlight and shadow
}

void CPDFSDK_XFAWidgetHandler::OnLoad(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_XFAWidgetHandler::ReleaseAnnot(CPDFSDK_Annot* pAnnot) {
  CPDFSDK_XFAWidget* pWidget = ToXFAWidget(pAnnot);
  pWidget->GetInteractiveForm()->RemoveXFAMap(pWidget->GetXFAWidget());
  delete pWidget;
}

CFX_FloatRect CPDFSDK_XFAWidgetHandler::GetViewBBox(CPDFSDK_PageView* pPageView,
                                                    CPDFSDK_Annot* pAnnot) {
//   CXFA_Node* node = pAnnot->GetXFAWidget()->GetNode();
//   ASSERT(node->IsWidgetReady());
// 
//   CFX_RectF rcBBox;
//   if (node->GetFFWidgetType() == XFA_FFWidgetType::kSignature) {
//     rcBBox = pAnnot->GetXFAWidget()->GetBBox(XFA_WidgetStatus_Visible,
//                                              CXFA_FFWidget::kDrawFocus);
//   } else {
//     rcBBox = pAnnot->GetXFAWidget()->GetBBox(XFA_WidgetStatus_None,
//                                              CXFA_FFWidget::kDoNotDrawFocus);
//   }
//   CFX_FloatRect rcWidget(rcBBox.left, rcBBox.top, rcBBox.left + rcBBox.width,
//                          rcBBox.top + rcBBox.height);
//   rcWidget.left -= 1.0f;
//   rcWidget.right += 1.0f;
//   rcWidget.bottom -= 1.0f;
//   rcWidget.top += 1.0f;
// 
//   return rcWidget;
  DebugBreak(); // weolar
  return CFX_FloatRect();
}

WideString CPDFSDK_XFAWidgetHandler::GetText(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return WideString();

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->GetText(pAnnot->GetXFAWidget());
}

WideString CPDFSDK_XFAWidgetHandler::GetSelectedText(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return WideString();

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->GetSelectedText(pAnnot->GetXFAWidget());
}

void CPDFSDK_XFAWidgetHandler::ReplaceSelection(CPDFSDK_Annot* pAnnot,
                                                const WideString& text) {
  if (!pAnnot)
    return;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->PasteText(pAnnot->GetXFAWidget(), text);
}

bool CPDFSDK_XFAWidgetHandler::CanUndo(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return false;
  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->CanUndo(pAnnot->GetXFAWidget());
}

bool CPDFSDK_XFAWidgetHandler::CanRedo(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return false;
  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->CanRedo(pAnnot->GetXFAWidget());
}

bool CPDFSDK_XFAWidgetHandler::Undo(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return false;
  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->Undo(pAnnot->GetXFAWidget());
}

bool CPDFSDK_XFAWidgetHandler::Redo(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return false;
  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->Redo(pAnnot->GetXFAWidget());
}

bool CPDFSDK_XFAWidgetHandler::HitTest(CPDFSDK_PageView* pPageView,
                                       CPDFSDK_Annot* pAnnot,
                                       const CFX_PointF& point) {
  if (!pPageView || !pAnnot)
    return false;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = pPageView->GetFormFillEnv();
  if (!pFormFillEnv)
    return false;

  CPDFXFA_Context* pContext = pFormFillEnv->GetXFAContext();
  if (!pContext)
    return false;

  CXFA_FFDocView* pDocView = pContext->GetXFADocView();
  if (!pDocView)
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = pDocView->GetWidgetHandler();
  if (!pWidgetHandler)
    return false;

  FWL_WidgetHit dwHitTest =
      pWidgetHandler->OnHitTest(pAnnot->GetXFAWidget(), point);
  return dwHitTest != FWL_WidgetHit::Unknown;
}

void CPDFSDK_XFAWidgetHandler::OnMouseEnter(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot::ObservedPtr* pAnnot,
                                            uint32_t nFlag) {
  if (!pPageView || !pAnnot->HasObservable())
    return;
  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  pWidgetHandler->OnMouseEnter((*pAnnot)->GetXFAWidget());
}

void CPDFSDK_XFAWidgetHandler::OnMouseExit(CPDFSDK_PageView* pPageView,
                                           CPDFSDK_Annot::ObservedPtr* pAnnot,
                                           uint32_t nFlag) {
  if (!pPageView || !pAnnot->HasObservable())
    return;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  pWidgetHandler->OnMouseExit((*pAnnot)->GetXFAWidget());
}

bool CPDFSDK_XFAWidgetHandler::OnLButtonDown(CPDFSDK_PageView* pPageView,
                                             CPDFSDK_Annot::ObservedPtr* pAnnot,
                                             uint32_t nFlags,
                                             const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnLButtonDown((*pAnnot)->GetXFAWidget(),
                                       GetFWLFlags(nFlags), point);
}

bool CPDFSDK_XFAWidgetHandler::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                           CPDFSDK_Annot::ObservedPtr* pAnnot,
                                           uint32_t nFlags,
                                           const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnLButtonUp((*pAnnot)->GetXFAWidget(),
                                     GetFWLFlags(nFlags), point);
}

bool CPDFSDK_XFAWidgetHandler::OnLButtonDblClk(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnLButtonDblClk((*pAnnot)->GetXFAWidget(),
                                         GetFWLFlags(nFlags), point);
}

bool CPDFSDK_XFAWidgetHandler::OnMouseMove(CPDFSDK_PageView* pPageView,
                                           CPDFSDK_Annot::ObservedPtr* pAnnot,
                                           uint32_t nFlags,
                                           const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnMouseMove((*pAnnot)->GetXFAWidget(),
                                     GetFWLFlags(nFlags), point);
}

bool CPDFSDK_XFAWidgetHandler::OnMouseWheel(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot::ObservedPtr* pAnnot,
                                            uint32_t nFlags,
                                            short zDelta,
                                            const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnMouseWheel((*pAnnot)->GetXFAWidget(),
                                      GetFWLFlags(nFlags), zDelta, point);
}

bool CPDFSDK_XFAWidgetHandler::OnRButtonDown(CPDFSDK_PageView* pPageView,
                                             CPDFSDK_Annot::ObservedPtr* pAnnot,
                                             uint32_t nFlags,
                                             const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnRButtonDown((*pAnnot)->GetXFAWidget(),
                                       GetFWLFlags(nFlags), point);
}

bool CPDFSDK_XFAWidgetHandler::OnRButtonUp(CPDFSDK_PageView* pPageView,
                                           CPDFSDK_Annot::ObservedPtr* pAnnot,
                                           uint32_t nFlags,
                                           const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnRButtonUp((*pAnnot)->GetXFAWidget(),
                                     GetFWLFlags(nFlags), point);
}

bool CPDFSDK_XFAWidgetHandler::OnRButtonDblClk(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot::ObservedPtr* pAnnot,
    uint32_t nFlags,
    const CFX_PointF& point) {
  if (!pPageView || !pAnnot->HasObservable())
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot->Get());
  return pWidgetHandler->OnRButtonDblClk((*pAnnot)->GetXFAWidget(),
                                         GetFWLFlags(nFlags), point);
}

bool CPDFSDK_XFAWidgetHandler::OnChar(CPDFSDK_Annot* pAnnot,
                                      uint32_t nChar,
                                      uint32_t nFlags) {
  if (!pAnnot)
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnChar(pAnnot->GetXFAWidget(), nChar,
                                GetFWLFlags(nFlags));
}

bool CPDFSDK_XFAWidgetHandler::OnKeyDown(CPDFSDK_Annot* pAnnot,
                                         int nKeyCode,
                                         int nFlag) {
  if (!pAnnot)
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnKeyDown(pAnnot->GetXFAWidget(), nKeyCode,
                                   GetFWLFlags(nFlag));
}

bool CPDFSDK_XFAWidgetHandler::OnKeyUp(CPDFSDK_Annot* pAnnot,
                                       int nKeyCode,
                                       int nFlag) {
  if (!pAnnot)
    return false;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnKeyUp(pAnnot->GetXFAWidget(), nKeyCode,
                                 GetFWLFlags(nFlag));
}

bool CPDFSDK_XFAWidgetHandler::OnSetFocus(CPDFSDK_Annot::ObservedPtr* pAnnot,
                                          uint32_t nFlag) {
  return true;
}

bool CPDFSDK_XFAWidgetHandler::OnKillFocus(CPDFSDK_Annot::ObservedPtr* pAnnot,
                                           uint32_t nFlag) {
  CXFA_FFWidget* hWidget = *pAnnot ? (*pAnnot)->GetXFAWidget() : nullptr;
  if (!hWidget)
    return true;

  CXFA_FFPageView* pXFAPageView = hWidget->GetPageView();
  if (!pXFAPageView)
    return true;

  pXFAPageView->GetDocView()->SetFocus(nullptr);
  return true;
}

bool CPDFSDK_XFAWidgetHandler::OnXFAChangedFocus(
    CPDFSDK_Annot::ObservedPtr* pOldAnnot,
    CPDFSDK_Annot::ObservedPtr* pNewAnnot) {
  CXFA_FFWidgetHandler* pWidgetHandler = nullptr;
  if (pOldAnnot->HasObservable())
    pWidgetHandler = GetXFAWidgetHandler(pOldAnnot->Get());
  else if (pNewAnnot->HasObservable())
    pWidgetHandler = GetXFAWidgetHandler(pNewAnnot->Get());

  if (!pWidgetHandler)
    return true;

  CXFA_FFWidget* hWidget = *pNewAnnot ? (*pNewAnnot)->GetXFAWidget() : nullptr;
  if (!hWidget)
    return true;

  CXFA_FFPageView* pXFAPageView = hWidget->GetPageView();
  if (!pXFAPageView)
    return true;

  bool bRet = pXFAPageView->GetDocView()->SetFocus(hWidget);
  if (pXFAPageView->GetDocView()->GetFocusWidget() == hWidget)
    bRet = true;

  return bRet;
}

CXFA_FFWidgetHandler* CPDFSDK_XFAWidgetHandler::GetXFAWidgetHandler(
    CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return nullptr;

  CPDFSDK_PageView* pPageView = pAnnot->GetPageView();
  if (!pPageView)
    return nullptr;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = pPageView->GetFormFillEnv();
  if (!pFormFillEnv)
    return nullptr;

  CPDFXFA_Context* pDoc = pFormFillEnv->GetXFAContext();
  if (!pDoc)
    return nullptr;

  CXFA_FFDocView* pDocView = pDoc->GetXFADocView();
  if (!pDocView)
    return nullptr;

  return pDocView->GetWidgetHandler();
}

uint32_t CPDFSDK_XFAWidgetHandler::GetFWLFlags(uint32_t dwFlag) {
  uint32_t dwFWLFlag = 0;

  if (dwFlag & FWL_EVENTFLAG_ControlKey)
    dwFWLFlag |= FWL_KEYFLAG_Ctrl;
  if (dwFlag & FWL_EVENTFLAG_LeftButtonDown)
    dwFWLFlag |= FWL_KEYFLAG_LButton;
  if (dwFlag & FWL_EVENTFLAG_MiddleButtonDown)
    dwFWLFlag |= FWL_KEYFLAG_MButton;
  if (dwFlag & FWL_EVENTFLAG_RightButtonDown)
    dwFWLFlag |= FWL_KEYFLAG_RButton;
  if (dwFlag & FWL_EVENTFLAG_ShiftKey)
    dwFWLFlag |= FWL_KEYFLAG_Shift;
  if (dwFlag & FWL_EVENTFLAG_AltKey)
    dwFWLFlag |= FWL_KEYFLAG_Alt;

  return dwFWLFlag;
}
