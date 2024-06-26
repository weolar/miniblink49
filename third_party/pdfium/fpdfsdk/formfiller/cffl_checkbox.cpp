// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cffl_checkbox.h"

#include <utility>

#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/pwl/cpwl_special_button.h"
#include "public/fpdf_fwlevent.h"
#include "third_party/base/ptr_util.h"

CFFL_CheckBox::CFFL_CheckBox(CPDFSDK_FormFillEnvironment* pApp,
                             CPDFSDK_Widget* pWidget)
    : CFFL_Button(pApp, pWidget) {}

CFFL_CheckBox::~CFFL_CheckBox() {}

std::unique_ptr<CPWL_Wnd> CFFL_CheckBox::NewPWLWindow(
    const CPWL_Wnd::CreateParams& cp,
    std::unique_ptr<CPWL_Wnd::PrivateData> pAttachedData) {
  auto pWnd = pdfium::MakeUnique<CPWL_CheckBox>(cp, std::move(pAttachedData));
  pWnd->Realize();
  pWnd->SetCheck(m_pWidget->IsChecked());
  return std::move(pWnd);
}

bool CFFL_CheckBox::OnKeyDown(CPDFSDK_Annot* pAnnot,
                              uint32_t nKeyCode,
                              uint32_t nFlags) {
  switch (nKeyCode) {
    case FWL_VKEY_Return:
    case FWL_VKEY_Space:
      return true;
    default:
      return CFFL_FormFiller::OnKeyDown(pAnnot, nKeyCode, nFlags);
  }
}
bool CFFL_CheckBox::OnChar(CPDFSDK_Annot* pAnnot,
                           uint32_t nChar,
                           uint32_t nFlags) {
  switch (nChar) {
    case FWL_VKEY_Return:
    case FWL_VKEY_Space: {
      CPDFSDK_PageView* pPageView = pAnnot->GetPageView();
      ASSERT(pPageView);

      CPDFSDK_Annot::ObservedPtr pObserved(m_pWidget.Get());
      if (m_pFormFillEnv->GetInteractiveFormFiller()->OnButtonUp(
              &pObserved, pPageView, nFlags)) {
        if (!pObserved)
          m_pWidget = nullptr;
        return true;
      }
      if (!pObserved) {
        m_pWidget = nullptr;
        return true;
      }

      CFFL_FormFiller::OnChar(pAnnot, nChar, nFlags);

      CPWL_CheckBox* pWnd = GetCheckBox(pPageView, true);
      if (pWnd) {
        CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pAnnot);
        pWnd->SetCheck(!pWidget->IsChecked());
      }

      return CommitData(pPageView, nFlags);
    }
    default:
      return CFFL_FormFiller::OnChar(pAnnot, nChar, nFlags);
  }
}

bool CFFL_CheckBox::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                CPDFSDK_Annot* pAnnot,
                                uint32_t nFlags,
                                const CFX_PointF& point) {
  CFFL_Button::OnLButtonUp(pPageView, pAnnot, nFlags, point);

  if (!IsValid())
    return true;

  CPWL_CheckBox* pWnd = GetCheckBox(pPageView, true);
  if (pWnd) {
    CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pAnnot);
    pWnd->SetCheck(!pWidget->IsChecked());
  }

  return CommitData(pPageView, nFlags);
}

bool CFFL_CheckBox::IsDataChanged(CPDFSDK_PageView* pPageView) {
  CPWL_CheckBox* pWnd = GetCheckBox(pPageView, false);
  return pWnd && pWnd->IsChecked() != m_pWidget->IsChecked();
}

void CFFL_CheckBox::SaveData(CPDFSDK_PageView* pPageView) {
  CPWL_CheckBox* pWnd = GetCheckBox(pPageView, false);
  if (!pWnd)
    return;

  bool bNewChecked = pWnd->IsChecked();
  if (bNewChecked) {
    CPDF_FormField* pField = m_pWidget->GetFormField();
    for (int32_t i = 0, sz = pField->CountControls(); i < sz; i++) {
      if (CPDF_FormControl* pCtrl = pField->GetControl(i)) {
        if (pCtrl->IsChecked()) {
          break;
        }
      }
    }
  }
  CPDFSDK_Widget::ObservedPtr observed_widget(m_pWidget.Get());
  CFFL_CheckBox::ObservedPtr observed_this(this);
  m_pWidget->SetCheck(bNewChecked, NotificationOption::kDoNotNotify);
  if (!observed_widget)
    return;

  m_pWidget->UpdateField();
  if (!observed_widget || !observed_this)
    return;

  SetChangeMark();
}

CPWL_CheckBox* CFFL_CheckBox::GetCheckBox(CPDFSDK_PageView* pPageView,
                                          bool bNew) {
  return static_cast<CPWL_CheckBox*>(GetPDFWindow(pPageView, bNew));
}
