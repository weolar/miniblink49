// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_checkbox.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "third_party/base/ptr_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_messagekey.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themetext.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fwl/ifwl_themeprovider.h"

namespace {

const int kCaptionMargin = 5;

}  // namespace

CFWL_CheckBox::CFWL_CheckBox(const CFWL_App* app)
    : CFWL_Widget(app, pdfium::MakeUnique<CFWL_WidgetProperties>(), nullptr) {
  m_TTOStyles.single_line_ = true;
}

CFWL_CheckBox::~CFWL_CheckBox() {}

FWL_Type CFWL_CheckBox::GetClassID() const {
  return FWL_Type::CheckBox;
}

void CFWL_CheckBox::SetBoxSize(float fHeight) {
  m_fBoxHeight = fHeight;
}

void CFWL_CheckBox::Update() {
  if (IsLocked())
    return;
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();

  UpdateTextOutStyles();
  Layout();
}

void CFWL_CheckBox::DrawWidget(CXFA_Graphics* pGraphics,
                               const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  if (!pTheme)
    return;

  if (HasBorder())
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, matrix);

  int32_t dwStates = GetPartStates();

  CFWL_ThemeBackground param;
  param.m_pWidget = this;
  param.m_iPart = CFWL_Part::Background;
  param.m_dwStates = dwStates;
  param.m_pGraphics = pGraphics;
  param.m_matrix.Concat(matrix);
  param.m_rtPart = m_rtClient;
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused)

    param.m_pRtData = &m_rtFocus;
  pTheme->DrawBackground(param);

  param.m_iPart = CFWL_Part::CheckBox;
  param.m_rtPart = m_rtBox;
  pTheme->DrawBackground(param);

  CFWL_ThemeText textParam;
  textParam.m_pWidget = this;
  textParam.m_iPart = CFWL_Part::Caption;
  textParam.m_dwStates = dwStates;
  textParam.m_pGraphics = pGraphics;
  textParam.m_matrix.Concat(matrix);
  textParam.m_rtPart = m_rtCaption;
  textParam.m_wsText = L"Check box";
  textParam.m_dwTTOStyles = m_TTOStyles;
  textParam.m_iTTOAlign = m_iTTOAlign;
  pTheme->DrawText(textParam);
}

void CFWL_CheckBox::SetCheckState(int32_t iCheck) {
  m_pProperties->m_dwStates &= ~FWL_STATE_CKB_CheckMask;
  switch (iCheck) {
    case 1:
      m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
      break;
    case 2:
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_3State)
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Neutral;
      break;
    default:
      break;
  }
  RepaintRect(m_rtClient);
}

void CFWL_CheckBox::Layout() {
  m_pProperties->m_rtWidget.width =
      FXSYS_round(m_pProperties->m_rtWidget.width);
  m_pProperties->m_rtWidget.height =
      FXSYS_round(m_pProperties->m_rtWidget.height);
  m_rtClient = GetClientRect();

  float fTextLeft = m_rtClient.left + m_fBoxHeight;
  m_rtBox = CFX_RectF(m_rtClient.TopLeft(), m_fBoxHeight, m_fBoxHeight);
  m_rtCaption = CFX_RectF(fTextLeft, m_rtClient.top,
                          m_rtClient.right() - fTextLeft, m_rtClient.height);
  m_rtCaption.Inflate(-kCaptionMargin, -kCaptionMargin);

  CFX_RectF rtFocus(m_rtCaption.left, m_rtCaption.top, m_rtCaption.width,
                    m_rtCaption.height);

  CalcTextRect(L"Check box", m_pProperties->m_pThemeProvider.Get(), m_TTOStyles,
               m_iTTOAlign, &rtFocus);

  m_rtFocus = CFX_RectF(m_rtCaption.TopLeft(),
                        std::max(m_rtCaption.width, rtFocus.width),
                        std::min(m_rtCaption.height, rtFocus.height));
  m_rtFocus.Inflate(1, 1);
}

uint32_t CFWL_CheckBox::GetPartStates() const {
  int32_t dwStates = CFWL_PartState_Normal;
  if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
      FWL_STATE_CKB_Neutral) {
    dwStates = CFWL_PartState_Neutral;
  } else if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
             FWL_STATE_CKB_Checked) {
    dwStates = CFWL_PartState_Checked;
  }
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)
    dwStates |= CFWL_PartState_Disabled;
  else if (m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered)
    dwStates |= CFWL_PartState_Hovered;
  else if (m_pProperties->m_dwStates & FWL_STATE_CKB_Pressed)
    dwStates |= CFWL_PartState_Pressed;
  else
    dwStates |= CFWL_PartState_Normal;
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused)
    dwStates |= CFWL_PartState_Focused;
  return dwStates;
}

void CFWL_CheckBox::UpdateTextOutStyles() {
  m_iTTOAlign = FDE_TextAlignment::kTopLeft;
  m_TTOStyles.Reset();
  m_TTOStyles.single_line_ = true;
}

void CFWL_CheckBox::NextStates() {
  uint32_t dwFirststate = m_pProperties->m_dwStates;
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_RadioButton) {
    if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
        FWL_STATE_CKB_Unchecked) {
      m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
    }
  } else {
    if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
        FWL_STATE_CKB_Neutral) {
      m_pProperties->m_dwStates &= ~FWL_STATE_CKB_CheckMask;
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_3State)
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
    } else if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
               FWL_STATE_CKB_Checked) {
      m_pProperties->m_dwStates &= ~FWL_STATE_CKB_CheckMask;
    } else {
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_3State)
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Neutral;
      else
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
    }
  }

  RepaintRect(m_rtClient);
  if (dwFirststate == m_pProperties->m_dwStates)
    return;

  CFWL_Event wmCheckBoxState(CFWL_Event::Type::CheckStateChanged, this);
  DispatchEvent(&wmCheckBoxState);
}

void CFWL_CheckBox::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus:
      OnFocusChanged(true);
      break;
    case CFWL_Message::Type::KillFocus:
      OnFocusChanged(false);
      break;
    case CFWL_Message::Type::Mouse: {
      CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
      switch (pMsg->m_dwCmd) {
        case FWL_MouseCommand::LeftButtonDown:
          OnLButtonDown();
          break;
        case FWL_MouseCommand::LeftButtonUp:
          OnLButtonUp(pMsg);
          break;
        case FWL_MouseCommand::Move:
          OnMouseMove(pMsg);
          break;
        case FWL_MouseCommand::Leave:
          OnMouseLeave();
          break;
        default:
          break;
      }
      break;
    }
    case CFWL_Message::Type::Key: {
      CFWL_MessageKey* pKey = static_cast<CFWL_MessageKey*>(pMessage);
      if (pKey->m_dwCmd == FWL_KeyCommand::KeyDown)
        OnKeyDown(pKey);
      break;
    }
    default:
      break;
  }

  CFWL_Widget::OnProcessMessage(pMessage);
}

void CFWL_CheckBox::OnDrawWidget(CXFA_Graphics* pGraphics,
                                 const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}

void CFWL_CheckBox::OnFocusChanged(bool bSet) {
  if (bSet)
    m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
  else
    m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;

  RepaintRect(m_rtClient);
}

void CFWL_CheckBox::OnLButtonDown() {
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)
    return;

  m_bBtnDown = true;
  m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Hovered;
  m_pProperties->m_dwStates |= FWL_STATE_CKB_Pressed;
  RepaintRect(m_rtClient);
}

void CFWL_CheckBox::OnLButtonUp(CFWL_MessageMouse* pMsg) {
  if (!m_bBtnDown)
    return;

  m_bBtnDown = false;
  if (!m_rtClient.Contains(pMsg->m_pos))
    return;

  m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
  m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Pressed;
  NextStates();
}

void CFWL_CheckBox::OnMouseMove(CFWL_MessageMouse* pMsg) {
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)
    return;

  bool bRepaint = false;
  if (m_bBtnDown) {
    if (m_rtClient.Contains(pMsg->m_pos)) {
      if ((m_pProperties->m_dwStates & FWL_STATE_CKB_Pressed) == 0) {
        bRepaint = true;
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Pressed;
      }
      if ((m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered)) {
        bRepaint = true;
        m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Hovered;
      }
    } else {
      if (m_pProperties->m_dwStates & FWL_STATE_CKB_Pressed) {
        bRepaint = true;
        m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Pressed;
      }
      if ((m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered) == 0) {
        bRepaint = true;
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
      }
    }
  } else {
    if (m_rtClient.Contains(pMsg->m_pos)) {
      if ((m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered) == 0) {
        bRepaint = true;
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
      }
    }
  }
  if (bRepaint)
    RepaintRect(m_rtBox);
}

void CFWL_CheckBox::OnMouseLeave() {
  if (m_bBtnDown)
    m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
  else
    m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Hovered;

  RepaintRect(m_rtBox);
}

void CFWL_CheckBox::OnKeyDown(CFWL_MessageKey* pMsg) {
  if (pMsg->m_dwKeyCode == FWL_VKEY_Tab)
    return;
  if (pMsg->m_dwKeyCode == FWL_VKEY_Return ||
      pMsg->m_dwKeyCode == FWL_VKEY_Space) {
    NextStates();
  }
}
