// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_widgethandler.h"

#include <memory>
#include <vector>

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "fpdfsdk/cpdfsdk_annot.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#endif  // PDF_ENABLE_XFA

CPDFSDK_WidgetHandler::CPDFSDK_WidgetHandler(
    CPDFSDK_FormFillEnvironment* pFormFillEnv)
    : m_pFormFillEnv(pFormFillEnv),
      m_pFormFiller(pFormFillEnv->GetInteractiveFormFiller()) {
  ASSERT(m_pFormFillEnv);
  ASSERT(m_pFormFiller);
}

CPDFSDK_WidgetHandler::~CPDFSDK_WidgetHandler() = default;

bool CPDFSDK_WidgetHandler::CanAnswer(CPDFSDK_Annot* pAnnot) {
  CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pAnnot);
  if (pWidget->IsSignatureWidget())
    return false;

  if (!pWidget->IsVisible())
    return false;

  int nFieldFlags = pWidget->GetFieldFlags();
  if ((nFieldFlags & FIELDFLAG_READONLY) == FIELDFLAG_READONLY)
    return false;

  if (pWidget->GetFieldType() == FormFieldType::kPushButton)
    return true;

  CPDF_Page* pPage = pWidget->GetPDFPage();
  uint32_t dwPermissions = pPage->GetDocument()->GetUserPermissions();
  return (dwPermissions & FPDFPERM_FILL_FORM) ||
         (dwPermissions & FPDFPERM_ANNOT_FORM);
}

CPDFSDK_Annot* CPDFSDK_WidgetHandler::NewAnnot(CPDF_Annot* pAnnot,
                                               CPDFSDK_PageView* pPage) {
  CPDFSDK_InteractiveForm* pForm = m_pFormFillEnv->GetInteractiveForm();
  CPDF_InteractiveForm* pPDFForm = pForm->GetInteractiveForm();
  CPDF_FormControl* pCtrl = pPDFForm->GetControlByDict(pAnnot->GetAnnotDict());
  if (!pCtrl)
    return nullptr;

  CPDFSDK_Widget* pWidget = new CPDFSDK_Widget(pAnnot, pPage, pForm);
  pForm->AddMap(pCtrl, pWidget);
  if (pPDFForm->NeedConstructAP())
    pWidget->ResetAppearance(pdfium::nullopt, false);
  return pWidget;
}

#ifdef PDF_ENABLE_XFA
CPDFSDK_Annot* CPDFSDK_WidgetHandler::NewAnnot(CXFA_FFWidget* hWidget,
                                               CPDFSDK_PageView* pPage) {
  return nullptr;
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_WidgetHandler::ReleaseAnnot(CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot);
  m_pFormFiller->OnDelete(pAnnot);

  std::unique_ptr<CPDFSDK_Widget> pWidget(ToCPDFSDKWidget(pAnnot));
  CPDFSDK_InteractiveForm* pForm = pWidget->GetInteractiveForm();
  CPDF_FormControl* pControl = pWidget->GetFormControl();
  pForm->RemoveMap(pControl);
}

void CPDFSDK_WidgetHandler::OnDraw(CPDFSDK_PageView* pPageView,
                                   CPDFSDK_Annot* pAnnot,
                                   CFX_RenderDevice* pDevice,
                                   const CFX_Matrix& mtUser2Device,
                                   bool bDrawAnnots) {
  if (pAnnot->IsSignatureWidget()) {
    pAnnot->AsBAAnnot()->DrawAppearance(pDevice, mtUser2Device,
                                        CPDF_Annot::Normal, nullptr);
  } else {
    m_pFormFiller->OnDraw(pPageView, pAnnot, pDevice, mtUser2Device);
  }
}

void CPDFSDK_WidgetHandler::OnMouseEnter(CPDFSDK_PageView* pPageView,
                                         CPDFSDK_Annot::ObservedPtr* pAnnot,
                                         uint32_t nFlag) {
  if (!(*pAnnot)->IsSignatureWidget())
    m_pFormFiller->OnMouseEnter(pPageView, pAnnot, nFlag);
}

void CPDFSDK_WidgetHandler::OnMouseExit(CPDFSDK_PageView* pPageView,
                                        CPDFSDK_Annot::ObservedPtr* pAnnot,
                                        uint32_t nFlag) {
  if (!(*pAnnot)->IsSignatureWidget())
    m_pFormFiller->OnMouseExit(pPageView, pAnnot, nFlag);
}

bool CPDFSDK_WidgetHandler::OnLButtonDown(CPDFSDK_PageView* pPageView,
                                          CPDFSDK_Annot::ObservedPtr* pAnnot,
                                          uint32_t nFlags,
                                          const CFX_PointF& point) {
  return !(*pAnnot)->IsSignatureWidget() &&
         m_pFormFiller->OnLButtonDown(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_WidgetHandler::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                        CPDFSDK_Annot::ObservedPtr* pAnnot,
                                        uint32_t nFlags,
                                        const CFX_PointF& point) {
  return !(*pAnnot)->IsSignatureWidget() &&
         m_pFormFiller->OnLButtonUp(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_WidgetHandler::OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot::ObservedPtr* pAnnot,
                                            uint32_t nFlags,
                                            const CFX_PointF& point) {
  return !(*pAnnot)->IsSignatureWidget() &&
         m_pFormFiller->OnLButtonDblClk(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_WidgetHandler::OnMouseMove(CPDFSDK_PageView* pPageView,
                                        CPDFSDK_Annot::ObservedPtr* pAnnot,
                                        uint32_t nFlags,
                                        const CFX_PointF& point) {
  return !(*pAnnot)->IsSignatureWidget() &&
         m_pFormFiller->OnMouseMove(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_WidgetHandler::OnMouseWheel(CPDFSDK_PageView* pPageView,
                                         CPDFSDK_Annot::ObservedPtr* pAnnot,
                                         uint32_t nFlags,
                                         short zDelta,
                                         const CFX_PointF& point) {
  return !(*pAnnot)->IsSignatureWidget() &&
         m_pFormFiller->OnMouseWheel(pPageView, pAnnot, nFlags, zDelta, point);
}

bool CPDFSDK_WidgetHandler::OnRButtonDown(CPDFSDK_PageView* pPageView,
                                          CPDFSDK_Annot::ObservedPtr* pAnnot,
                                          uint32_t nFlags,
                                          const CFX_PointF& point) {
  return !(*pAnnot)->IsSignatureWidget() &&
         m_pFormFiller->OnRButtonDown(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_WidgetHandler::OnRButtonUp(CPDFSDK_PageView* pPageView,
                                        CPDFSDK_Annot::ObservedPtr* pAnnot,
                                        uint32_t nFlags,
                                        const CFX_PointF& point) {
  return !(*pAnnot)->IsSignatureWidget() &&
         m_pFormFiller->OnRButtonUp(pPageView, pAnnot, nFlags, point);
}

bool CPDFSDK_WidgetHandler::OnRButtonDblClk(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot::ObservedPtr* pAnnot,
                                            uint32_t nFlags,
                                            const CFX_PointF& point) {
  return false;
}

bool CPDFSDK_WidgetHandler::OnChar(CPDFSDK_Annot* pAnnot,
                                   uint32_t nChar,
                                   uint32_t nFlags) {
  return !pAnnot->IsSignatureWidget() &&
         m_pFormFiller->OnChar(pAnnot, nChar, nFlags);
}

bool CPDFSDK_WidgetHandler::OnKeyDown(CPDFSDK_Annot* pAnnot,
                                      int nKeyCode,
                                      int nFlag) {
  return !pAnnot->IsSignatureWidget() &&
         m_pFormFiller->OnKeyDown(pAnnot, nKeyCode, nFlag);
}

bool CPDFSDK_WidgetHandler::OnKeyUp(CPDFSDK_Annot* pAnnot,
                                    int nKeyCode,
                                    int nFlag) {
  return false;
}

void CPDFSDK_WidgetHandler::OnLoad(CPDFSDK_Annot* pAnnot) {
  if (pAnnot->IsSignatureWidget())
    return;

  CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pAnnot);
  if (!pWidget->IsAppearanceValid())
    pWidget->ResetAppearance(pdfium::nullopt, false);

  FormFieldType fieldType = pWidget->GetFieldType();
  if (fieldType == FormFieldType::kTextField ||
      fieldType == FormFieldType::kComboBox) {
    CPDFSDK_Annot::ObservedPtr pObserved(pWidget);
    Optional<WideString> sValue = pWidget->OnFormat();
    if (!pObserved)
      return;

    if (sValue.has_value() && fieldType == FormFieldType::kComboBox)
      pWidget->ResetAppearance(sValue, false);
  }

#ifdef PDF_ENABLE_XFA
  CPDFSDK_PageView* pPageView = pAnnot->GetPageView();
  CPDFXFA_Context* pContext = pPageView->GetFormFillEnv()->GetXFAContext();
  if (pContext->GetFormType() == FormType::kXFAForeground) {
    if (!pWidget->IsAppearanceValid() && !pWidget->GetValue().IsEmpty())
      pWidget->ResetAppearance(false);
  }
#endif  // PDF_ENABLE_XFA
}

bool CPDFSDK_WidgetHandler::OnSetFocus(CPDFSDK_Annot::ObservedPtr* pAnnot,
                                       uint32_t nFlag) {
  return (*pAnnot)->IsSignatureWidget() ||
         m_pFormFiller->OnSetFocus(pAnnot, nFlag);
}

bool CPDFSDK_WidgetHandler::OnKillFocus(CPDFSDK_Annot::ObservedPtr* pAnnot,
                                        uint32_t nFlag) {
  return (*pAnnot)->IsSignatureWidget() ||
         m_pFormFiller->OnKillFocus(pAnnot, nFlag);
}

#ifdef PDF_ENABLE_XFA
bool CPDFSDK_WidgetHandler::OnXFAChangedFocus(
    CPDFSDK_Annot::ObservedPtr* pOldAnnot,
    CPDFSDK_Annot::ObservedPtr* pNewAnnot) {
  return true;
}
#endif  // PDF_ENABLE_XFA

CFX_FloatRect CPDFSDK_WidgetHandler::GetViewBBox(CPDFSDK_PageView* pPageView,
                                                 CPDFSDK_Annot* pAnnot) {
  if (!pAnnot->IsSignatureWidget())
    return CFX_FloatRect(m_pFormFiller->GetViewBBox(pPageView, pAnnot));
  return CFX_FloatRect();
}

WideString CPDFSDK_WidgetHandler::GetText(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot->IsSignatureWidget())
    return m_pFormFiller->GetText(pAnnot);
  return WideString();
}

WideString CPDFSDK_WidgetHandler::GetSelectedText(CPDFSDK_Annot* pAnnot) {
  if (!pAnnot->IsSignatureWidget())
    return m_pFormFiller->GetSelectedText(pAnnot);
  return WideString();
}

void CPDFSDK_WidgetHandler::ReplaceSelection(CPDFSDK_Annot* pAnnot,
                                             const WideString& text) {
  if (!pAnnot->IsSignatureWidget())
    m_pFormFiller->ReplaceSelection(pAnnot, text);
}

bool CPDFSDK_WidgetHandler::CanUndo(CPDFSDK_Annot* pAnnot) {
  return !pAnnot->IsSignatureWidget() && m_pFormFiller->CanUndo(pAnnot);
}

bool CPDFSDK_WidgetHandler::CanRedo(CPDFSDK_Annot* pAnnot) {
  return !pAnnot->IsSignatureWidget() && m_pFormFiller->CanRedo(pAnnot);
}

bool CPDFSDK_WidgetHandler::Undo(CPDFSDK_Annot* pAnnot) {
  return !pAnnot->IsSignatureWidget() && m_pFormFiller->Undo(pAnnot);
}

bool CPDFSDK_WidgetHandler::Redo(CPDFSDK_Annot* pAnnot) {
  return !pAnnot->IsSignatureWidget() && m_pFormFiller->Redo(pAnnot);
}

bool CPDFSDK_WidgetHandler::HitTest(CPDFSDK_PageView* pPageView,
                                    CPDFSDK_Annot* pAnnot,
                                    const CFX_PointF& point) {
  ASSERT(pPageView);
  ASSERT(pAnnot);
  return GetViewBBox(pPageView, pAnnot).Contains(point);
}
