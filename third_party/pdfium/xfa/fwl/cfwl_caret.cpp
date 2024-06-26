// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_caret.h"

#include <utility>

#include "third_party/base/ptr_util.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_timerinfo.h"
#include "xfa/fwl/cfwl_widgetproperties.h"
#include "xfa/fwl/ifwl_themeprovider.h"

namespace {

const uint32_t kBlinkPeriodMs = 600;

constexpr int kStateHighlight = (1 << 0);

}  // namespace

CFWL_Caret::CFWL_Caret(const CFWL_App* app,
                       std::unique_ptr<CFWL_WidgetProperties> properties,
                       CFWL_Widget* pOuter)
    : CFWL_Widget(app, std::move(properties), pOuter),
      m_pTimer(pdfium::MakeUnique<CFWL_Caret::Timer>(this)),
      m_pTimerInfo(nullptr) {
  SetStates(kStateHighlight);
}

CFWL_Caret::~CFWL_Caret() {
  if (m_pTimerInfo) {
    m_pTimerInfo->StopTimer();
    m_pTimerInfo = nullptr;
  }
}

FWL_Type CFWL_Caret::GetClassID() const {
  return FWL_Type::Caret;
}

void CFWL_Caret::Update() {}

void CFWL_Caret::DrawWidget(CXFA_Graphics* pGraphics,
                            const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  if (!m_pProperties->m_pThemeProvider)
    return;

  DrawCaretBK(pGraphics, m_pProperties->m_pThemeProvider.Get(), &matrix);
}

void CFWL_Caret::ShowCaret() {
  if (m_pTimerInfo) {
    CFWL_TimerInfo* pOldTimerInfo = m_pTimerInfo.Release();
    pOldTimerInfo->StopTimer();
  }

  m_pTimerInfo = m_pTimer->StartTimer(kBlinkPeriodMs, true);
  RemoveStates(FWL_WGTSTATE_Invisible);
  SetStates(kStateHighlight);
}

void CFWL_Caret::HideCaret() {
  if (m_pTimerInfo) {
    CFWL_TimerInfo* pOldTimerInfo = m_pTimerInfo.Release();
    pOldTimerInfo->StopTimer();
  }

  SetStates(FWL_WGTSTATE_Invisible);
}

void CFWL_Caret::DrawCaretBK(CXFA_Graphics* pGraphics,
                             IFWL_ThemeProvider* pTheme,
                             const CFX_Matrix* pMatrix) {
  if (!(m_pProperties->m_dwStates & kStateHighlight))
    return;

  CFWL_ThemeBackground param;
  param.m_pWidget = this;
  param.m_pGraphics = pGraphics;
  param.m_rtPart = CFX_RectF(0, 0, GetWidgetRect().Size());
  param.m_iPart = CFWL_Part::Background;
  param.m_dwStates = CFWL_PartState_HightLight;
  if (pMatrix)
    param.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(param);
}

void CFWL_Caret::OnProcessMessage(CFWL_Message* pMessage) {}

void CFWL_Caret::OnDrawWidget(CXFA_Graphics* pGraphics,
                              const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}

CFWL_Caret::Timer::Timer(CFWL_Caret* pCaret) : CFWL_Timer(pCaret) {}

void CFWL_Caret::Timer::Run(CFWL_TimerInfo* pTimerInfo) {
  CFWL_Caret* pCaret = static_cast<CFWL_Caret*>(m_pWidget.Get());
  if (!(pCaret->GetStates() & kStateHighlight))
    pCaret->SetStates(kStateHighlight);
  else
    pCaret->RemoveStates(kStateHighlight);

  CFX_RectF rt = pCaret->GetWidgetRect();
  pCaret->RepaintRect(CFX_RectF(0, 0, rt.width + 1, rt.height));
}
