// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cffl_button.h"

CFFL_Button::CFFL_Button(CPDFSDK_FormFillEnvironment* pApp,
                         CPDFSDK_Widget* pWidget)
    : CFFL_FormFiller(pApp, pWidget), m_bMouseIn(false), m_bMouseDown(false) {}

CFFL_Button::~CFFL_Button() {}

void CFFL_Button::OnMouseEnter(CPDFSDK_PageView* pPageView,
                               CPDFSDK_Annot* pAnnot) {
  m_bMouseIn = true;
  InvalidateRect(GetViewBBox(pPageView, pAnnot));
}

void CFFL_Button::OnMouseExit(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot) {
  m_bMouseIn = false;
  InvalidateRect(GetViewBBox(pPageView, pAnnot));
  EndTimer();
  ASSERT(m_pWidget);
}

bool CFFL_Button::OnLButtonDown(CPDFSDK_PageView* pPageView,
                                CPDFSDK_Annot* pAnnot,
                                uint32_t nFlags,
                                const CFX_PointF& point) {
  if (!pAnnot->GetRect().Contains(point))
    return false;

  m_bMouseDown = true;
  m_bValid = true;
  InvalidateRect(GetViewBBox(pPageView, pAnnot));
  return true;
}

bool CFFL_Button::OnLButtonUp(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              uint32_t nFlags,
                              const CFX_PointF& point) {
  if (!pAnnot->GetRect().Contains(point))
    return false;

  m_bMouseDown = false;
  m_pWidget->GetPDFPage();
  InvalidateRect(GetViewBBox(pPageView, pAnnot));
  return true;
}

bool CFFL_Button::OnMouseMove(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              uint32_t nFlags,
                              const CFX_PointF& point) {
  return true;
}

void CFFL_Button::OnDraw(CPDFSDK_PageView* pPageView,
                         CPDFSDK_Annot* pAnnot,
                         CFX_RenderDevice* pDevice,
                         const CFX_Matrix& mtUser2Device) {
  ASSERT(pPageView);
  CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pAnnot);
  CPDF_FormControl* pCtrl = pWidget->GetFormControl();
  if (pCtrl->GetHighlightingMode() != CPDF_FormControl::Push) {
    pWidget->DrawAppearance(pDevice, mtUser2Device, CPDF_Annot::Normal,
                            nullptr);
    return;
  }
  if (m_bMouseDown) {
    if (pWidget->IsWidgetAppearanceValid(CPDF_Annot::Down)) {
      pWidget->DrawAppearance(pDevice, mtUser2Device, CPDF_Annot::Down,
                              nullptr);
    } else {
      pWidget->DrawAppearance(pDevice, mtUser2Device, CPDF_Annot::Normal,
                              nullptr);
    }
    return;
  }
  if (m_bMouseIn) {
    if (pWidget->IsWidgetAppearanceValid(CPDF_Annot::Rollover)) {
      pWidget->DrawAppearance(pDevice, mtUser2Device, CPDF_Annot::Rollover,
                              nullptr);
    } else {
      pWidget->DrawAppearance(pDevice, mtUser2Device, CPDF_Annot::Normal,
                              nullptr);
    }
    return;
  }

  pWidget->DrawAppearance(pDevice, mtUser2Device, CPDF_Annot::Normal, nullptr);
}

void CFFL_Button::OnDrawDeactive(CPDFSDK_PageView* pPageView,
                                 CPDFSDK_Annot* pAnnot,
                                 CFX_RenderDevice* pDevice,
                                 const CFX_Matrix& mtUser2Device) {
  OnDraw(pPageView, pAnnot, pDevice, mtUser2Device);
}
