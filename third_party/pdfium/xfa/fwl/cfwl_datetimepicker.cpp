// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_datetimepicker.h"

#include <memory>
#include <utility>

#include "third_party/base/ptr_util.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_eventselectchanged.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_messagesetfocus.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fwl/ifwl_themeprovider.h"

namespace {

const int kDateTimePickerHeight = 20;

}  // namespace
CFWL_DateTimePicker::CFWL_DateTimePicker(const CFWL_App* app)
    : CFWL_Widget(app, pdfium::MakeUnique<CFWL_WidgetProperties>(), nullptr),
      m_iBtnState(1),
      m_iYear(-1),
      m_iMonth(-1),
      m_iDay(-1),
      m_bLBtnDown(false) {
  m_pProperties->m_dwStyleExes = FWL_STYLEEXT_DTP_ShortDateFormat;

  auto monthProp = pdfium::MakeUnique<CFWL_WidgetProperties>();
  monthProp->m_dwStyles = FWL_WGTSTYLE_Popup | FWL_WGTSTYLE_Border;
  monthProp->m_dwStates = FWL_WGTSTATE_Invisible;
  monthProp->m_pParent = this;
  monthProp->m_pThemeProvider = m_pProperties->m_pThemeProvider;
  m_pMonthCal = pdfium::MakeUnique<CFWL_MonthCalendar>(
      m_pOwnerApp.Get(), std::move(monthProp), this);

  m_pMonthCal->SetWidgetRect(
      CFX_RectF(0, 0, m_pMonthCal->GetAutosizedWidgetRect().Size()));

  auto editProp = pdfium::MakeUnique<CFWL_WidgetProperties>();
  editProp->m_pParent = this;
  editProp->m_pThemeProvider = m_pProperties->m_pThemeProvider;

  m_pEdit = pdfium::MakeUnique<CFWL_DateTimeEdit>(m_pOwnerApp.Get(),
                                                  std::move(editProp), this);
  RegisterEventTarget(m_pMonthCal.get());
  RegisterEventTarget(m_pEdit.get());
}

CFWL_DateTimePicker::~CFWL_DateTimePicker() {
  UnregisterEventTarget();
}

FWL_Type CFWL_DateTimePicker::GetClassID() const {
  return FWL_Type::DateTimePicker;
}

void CFWL_DateTimePicker::Update() {
  if (m_iLock)
    return;

  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  m_pEdit->SetThemeProvider(m_pProperties->m_pThemeProvider.Get());
  m_rtClient = GetClientRect();
  m_pEdit->SetWidgetRect(m_rtClient);
  ResetEditAlignment();
  m_pEdit->Update();
  if (!m_pMonthCal->GetThemeProvider())
    m_pMonthCal->SetThemeProvider(m_pProperties->m_pThemeProvider.Get());

  IFWL_ThemeProvider* theme = GetAvailableTheme();
  if (!theme)
    return;

  m_fBtn = theme->GetScrollBarWidth();
  CFX_RectF rtMonthCal = m_pMonthCal->GetAutosizedWidgetRect();
  CFX_RectF rtPopUp(rtMonthCal.left, rtMonthCal.top + kDateTimePickerHeight,
                    rtMonthCal.width, rtMonthCal.height);
  m_pMonthCal->SetWidgetRect(rtPopUp);
  m_pMonthCal->Update();
}

FWL_WidgetHit CFWL_DateTimePicker::HitTest(const CFX_PointF& point) {
  CFX_RectF rect(0, 0, m_pProperties->m_rtWidget.width,
                 m_pProperties->m_rtWidget.height);
  if (rect.Contains(point))
    return FWL_WidgetHit::Edit;
  if (NeedsToShowButton())
    rect.width += m_fBtn;
  if (rect.Contains(point))
    return FWL_WidgetHit::Client;
  if (IsMonthCalendarVisible()) {
    if (m_pMonthCal->GetWidgetRect().Contains(point))
      return FWL_WidgetHit::Client;
  }
  return FWL_WidgetHit::Unknown;
}

void CFWL_DateTimePicker::DrawWidget(CXFA_Graphics* pGraphics,
                                     const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  if (!pTheme)
    return;

  if (HasBorder())
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, matrix);
  if (!m_rtBtn.IsEmpty())
    DrawDropDownButton(pGraphics, pTheme, &matrix);

  if (m_pEdit) {
    CFX_RectF rtEdit = m_pEdit->GetWidgetRect();

    CFX_Matrix mt(1, 0, 0, 1, rtEdit.left, rtEdit.top);
    mt.Concat(matrix);
    m_pEdit->DrawWidget(pGraphics, mt);
  }
  if (!IsMonthCalendarVisible())
    return;

  CFX_RectF rtMonth = m_pMonthCal->GetWidgetRect();
  CFX_Matrix mt(1, 0, 0, 1, rtMonth.left, rtMonth.top);
  mt.Concat(matrix);
  m_pMonthCal->DrawWidget(pGraphics, mt);
}

void CFWL_DateTimePicker::SetThemeProvider(IFWL_ThemeProvider* pTP) {
  m_pProperties->m_pThemeProvider = pTP;
  m_pMonthCal->SetThemeProvider(pTP);
}

void CFWL_DateTimePicker::GetCurSel(int32_t& iYear,
                                    int32_t& iMonth,
                                    int32_t& iDay) {
  iYear = m_iYear;
  iMonth = m_iMonth;
  iDay = m_iDay;
}

void CFWL_DateTimePicker::SetCurSel(int32_t iYear,
                                    int32_t iMonth,
                                    int32_t iDay) {
  if (iYear <= 0 || iYear >= 3000)
    return;
  if (iMonth <= 0 || iMonth >= 13)
    return;
  if (iDay <= 0 || iDay >= 32)
    return;

  m_iYear = iYear;
  m_iMonth = iMonth;
  m_iDay = iDay;
  m_pMonthCal->SetSelect(iYear, iMonth, iDay);
}

void CFWL_DateTimePicker::SetEditText(const WideString& wsText) {
  if (!m_pEdit)
    return;

  m_pEdit->SetText(wsText);
  RepaintRect(m_rtClient);

  CFWL_Event ev(CFWL_Event::Type::EditChanged);
  DispatchEvent(&ev);
}

WideString CFWL_DateTimePicker::GetEditText() const {
  return m_pEdit ? m_pEdit->GetText() : WideString();
}

int32_t CFWL_DateTimePicker::GetEditTextLength() const {
  return m_pEdit ? m_pEdit->GetTextLength() : 0;
}

CFX_RectF CFWL_DateTimePicker::GetBBox() const {
  CFX_RectF rect = m_pProperties->m_rtWidget;
  if (NeedsToShowButton())
    rect.width += m_fBtn;
  if (!IsMonthCalendarVisible())
    return rect;

  CFX_RectF rtMonth = m_pMonthCal->GetWidgetRect();
  rtMonth.Offset(m_pProperties->m_rtWidget.left, m_pProperties->m_rtWidget.top);
  rect.Union(rtMonth);
  return rect;
}

void CFWL_DateTimePicker::ModifyEditStylesEx(uint32_t dwStylesExAdded,
                                             uint32_t dwStylesExRemoved) {
  m_pEdit->ModifyStylesEx(dwStylesExAdded, dwStylesExRemoved);
}

void CFWL_DateTimePicker::DrawDropDownButton(CXFA_Graphics* pGraphics,
                                             IFWL_ThemeProvider* pTheme,
                                             const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground param;
  param.m_pWidget = this;
  param.m_iPart = CFWL_Part::DropDownButton;
  param.m_dwStates = m_iBtnState;
  param.m_pGraphics = pGraphics;
  param.m_rtPart = m_rtBtn;
  if (pMatrix)
    param.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(param);
}

WideString CFWL_DateTimePicker::FormatDateString(int32_t iYear,
                                                 int32_t iMonth,
                                                 int32_t iDay) {
  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_DTP_ShortDateFormat) ==
      FWL_STYLEEXT_DTP_ShortDateFormat) {
    return WideString::Format(L"%d-%d-%d", iYear, iMonth, iDay);
  }

  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_DTP_LongDateFormat) ==
      FWL_STYLEEXT_DTP_LongDateFormat) {
    return WideString::Format(L"%d Year %d Month %d Day", iYear, iMonth, iDay);
  }

  return WideString();
}

void CFWL_DateTimePicker::ShowMonthCalendar(bool bActivate) {
  if (IsMonthCalendarVisible() == bActivate)
    return;

  if (bActivate) {
    CFX_RectF rtMonthCal = m_pMonthCal->GetAutosizedWidgetRect();
    float fPopupMin = rtMonthCal.height;
    float fPopupMax = rtMonthCal.height;
    CFX_RectF rtAnchor(m_pProperties->m_rtWidget);
    rtAnchor.width = rtMonthCal.width;
    rtMonthCal.left = m_rtClient.left;
    rtMonthCal.top = rtAnchor.Height();
    GetPopupPos(fPopupMin, fPopupMax, rtAnchor, &rtMonthCal);
    m_pMonthCal->SetWidgetRect(rtMonthCal);
    if (m_iYear > 0 && m_iMonth > 0 && m_iDay > 0)
      m_pMonthCal->SetSelect(m_iYear, m_iMonth, m_iDay);
    m_pMonthCal->Update();
  }
  if (bActivate)
    m_pMonthCal->RemoveStates(FWL_WGTSTATE_Invisible);
  else
    m_pMonthCal->SetStates(FWL_WGTSTATE_Invisible);

  if (bActivate) {
    CFWL_MessageSetFocus msg(m_pEdit.get(), m_pMonthCal.get());
    m_pEdit->GetDelegate()->OnProcessMessage(&msg);
  }

  CFX_RectF rtInvalidate(0, 0, m_pProperties->m_rtWidget.width,
                         m_pProperties->m_rtWidget.height);

  CFX_RectF rtCal = m_pMonthCal->GetWidgetRect();
  rtInvalidate.Union(rtCal);
  rtInvalidate.Inflate(2, 2);
  RepaintRect(rtInvalidate);
}

bool CFWL_DateTimePicker::IsMonthCalendarVisible() const {
  return m_pMonthCal && m_pMonthCal->IsVisible();
}

void CFWL_DateTimePicker::ResetEditAlignment() {
  if (!m_pEdit)
    return;

  uint32_t dwAdd = 0;
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_DTP_EditHAlignMask) {
    case FWL_STYLEEXT_DTP_EditHCenter: {
      dwAdd |= FWL_STYLEEXT_EDT_HCenter;
      break;
    }
    case FWL_STYLEEXT_DTP_EditHFar: {
      dwAdd |= FWL_STYLEEXT_EDT_HFar;
      break;
    }
    default: {
      dwAdd |= FWL_STYLEEXT_EDT_HNear;
      break;
    }
  }
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_DTP_EditVAlignMask) {
    case FWL_STYLEEXT_DTP_EditVCenter: {
      dwAdd |= FWL_STYLEEXT_EDT_VCenter;
      break;
    }
    case FWL_STYLEEXT_DTP_EditVFar: {
      dwAdd |= FWL_STYLEEXT_EDT_VFar;
      break;
    }
    default: {
      dwAdd |= FWL_STYLEEXT_EDT_VNear;
      break;
    }
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_DTP_EditJustified)
    dwAdd |= FWL_STYLEEXT_EDT_Justified;

  m_pEdit->ModifyStylesEx(dwAdd, FWL_STYLEEXT_EDT_HAlignMask |
                                     FWL_STYLEEXT_EDT_HAlignModeMask |
                                     FWL_STYLEEXT_EDT_VAlignMask);
}

void CFWL_DateTimePicker::ProcessSelChanged(int32_t iYear,
                                            int32_t iMonth,
                                            int32_t iDay) {
  m_iYear = iYear;
  m_iMonth = iMonth;
  m_iDay = iDay;

  WideString wsText = FormatDateString(m_iYear, m_iMonth, m_iDay);
  m_pEdit->SetText(wsText);
  m_pEdit->Update();
  RepaintRect(m_rtClient);

  CFWL_EventSelectChanged ev(this);
  ev.iYear = m_iYear;
  ev.iMonth = m_iMonth;
  ev.iDay = m_iDay;
  DispatchEvent(&ev);
}

bool CFWL_DateTimePicker::NeedsToShowButton() const {
  return m_pProperties->m_dwStates & FWL_WGTSTATE_Focused ||
         m_pMonthCal->GetStates() & FWL_WGTSTATE_Focused ||
         m_pEdit->GetStates() & FWL_WGTSTATE_Focused;
}

void CFWL_DateTimePicker::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus:
      OnFocusChanged(pMessage, true);
      break;
    case CFWL_Message::Type::KillFocus:
      OnFocusChanged(pMessage, false);
      break;
    case CFWL_Message::Type::Mouse: {
      CFWL_MessageMouse* pMouse = static_cast<CFWL_MessageMouse*>(pMessage);
      switch (pMouse->m_dwCmd) {
        case FWL_MouseCommand::LeftButtonDown:
          OnLButtonDown(pMouse);
          break;
        case FWL_MouseCommand::LeftButtonUp:
          OnLButtonUp(pMouse);
          break;
        case FWL_MouseCommand::Move:
          OnMouseMove(pMouse);
          break;
        case FWL_MouseCommand::Leave:
          OnMouseLeave(pMouse);
          break;
        default:
          break;
      }
      break;
    }
    case CFWL_Message::Type::Key: {
      if (m_pEdit->GetStates() & FWL_WGTSTATE_Focused) {
        m_pEdit->GetDelegate()->OnProcessMessage(pMessage);
        return;
      }
      break;
    }
    default:
      break;
  }

  CFWL_Widget::OnProcessMessage(pMessage);
}

void CFWL_DateTimePicker::OnDrawWidget(CXFA_Graphics* pGraphics,
                                       const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}

void CFWL_DateTimePicker::OnFocusChanged(CFWL_Message* pMsg, bool bSet) {
  if (!pMsg)
    return;

  CFX_RectF rtInvalidate(m_rtBtn);
  if (bSet) {
    m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
    if (m_pEdit && !(m_pEdit->GetStylesEx() & FWL_STYLEEXT_EDT_ReadOnly)) {
      m_rtBtn = CFX_RectF(m_pProperties->m_rtWidget.width, 0, m_fBtn,
                          m_pProperties->m_rtWidget.height - 1);
    }
    rtInvalidate = m_rtBtn;
    pMsg->SetDstTarget(m_pEdit.get());
    m_pEdit->GetDelegate()->OnProcessMessage(pMsg);
  } else {
    m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
    m_rtBtn = CFX_RectF();
    if (IsMonthCalendarVisible())
      ShowMonthCalendar(false);
    if (m_pEdit->GetStates() & FWL_WGTSTATE_Focused) {
      pMsg->SetSrcTarget(m_pEdit.get());
      m_pEdit->GetDelegate()->OnProcessMessage(pMsg);
    }
  }
  rtInvalidate.Inflate(2, 2);
  RepaintRect(rtInvalidate);
}

void CFWL_DateTimePicker::OnLButtonDown(CFWL_MessageMouse* pMsg) {
  if (!pMsg)
    return;
  if (!m_rtBtn.Contains(pMsg->m_pos))
    return;

  if (IsMonthCalendarVisible()) {
    ShowMonthCalendar(false);
    return;
  }
  ShowMonthCalendar(true);

  m_bLBtnDown = true;
  RepaintRect(m_rtClient);
}

void CFWL_DateTimePicker::OnLButtonUp(CFWL_MessageMouse* pMsg) {
  if (!pMsg)
    return;

  m_bLBtnDown = false;
  if (m_rtBtn.Contains(pMsg->m_pos))
    m_iBtnState = CFWL_PartState_Hovered;
  else
    m_iBtnState = CFWL_PartState_Normal;
  RepaintRect(m_rtBtn);
}

void CFWL_DateTimePicker::OnMouseMove(CFWL_MessageMouse* pMsg) {
  if (!m_rtBtn.Contains(pMsg->m_pos))
    m_iBtnState = CFWL_PartState_Normal;
  RepaintRect(m_rtBtn);
}

void CFWL_DateTimePicker::OnMouseLeave(CFWL_MessageMouse* pMsg) {
  if (!pMsg)
    return;
  m_iBtnState = CFWL_PartState_Normal;
  RepaintRect(m_rtBtn);
}

void CFWL_DateTimePicker::GetPopupPos(float fMinHeight,
                                      float fMaxHeight,
                                      const CFX_RectF& rtAnchor,
                                      CFX_RectF* pPopupRect) {
  m_pWidgetMgr->GetAdapterPopupPos(this, fMinHeight, fMaxHeight, rtAnchor,
                                   pPopupRect);
}

void CFWL_DateTimePicker::ClearText() {
  m_pEdit->ClearText();
}

void CFWL_DateTimePicker::SelectAll() {
  m_pEdit->SelectAll();
}

void CFWL_DateTimePicker::ClearSelection() {
  m_pEdit->ClearSelection();
}

Optional<WideString> CFWL_DateTimePicker::Copy() {
  return m_pEdit->Copy();
}

Optional<WideString> CFWL_DateTimePicker::Cut() {
  return m_pEdit->Cut();
}

bool CFWL_DateTimePicker::Paste(const WideString& wsPaste) {
  return m_pEdit->Paste(wsPaste);
}

bool CFWL_DateTimePicker::Undo() {
  return m_pEdit->Undo();
}

bool CFWL_DateTimePicker::Redo() {
  return m_pEdit->Redo();
}

bool CFWL_DateTimePicker::CanUndo() {
  return m_pEdit->CanUndo();
}

bool CFWL_DateTimePicker::CanRedo() {
  return m_pEdit->CanRedo();
}
