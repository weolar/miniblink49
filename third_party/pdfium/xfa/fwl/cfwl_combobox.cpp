// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_combobox.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "third_party/base/ptr_util.h"
#include "xfa/fde/cfde_texteditengine.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_eventselectchanged.h"
#include "xfa/fwl/cfwl_listbox.h"
#include "xfa/fwl/cfwl_messagekey.h"
#include "xfa/fwl/cfwl_messagekillfocus.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_messagesetfocus.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themepart.h"
#include "xfa/fwl/cfwl_themetext.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fwl/ifwl_themeprovider.h"

CFWL_ComboBox::CFWL_ComboBox(const CFWL_App* app)
    : CFWL_Widget(app, pdfium::MakeUnique<CFWL_WidgetProperties>(), nullptr),
      m_iCurSel(-1),
      m_iBtnState(CFWL_PartState_Normal) {
  InitComboList();
  InitComboEdit();
}

CFWL_ComboBox::~CFWL_ComboBox() {}

FWL_Type CFWL_ComboBox::GetClassID() const {
  return FWL_Type::ComboBox;
}

void CFWL_ComboBox::AddString(WideStringView wsText) {
  m_pListBox->AddString(wsText);
}

void CFWL_ComboBox::RemoveAt(int32_t iIndex) {
  m_pListBox->RemoveAt(iIndex);
}

void CFWL_ComboBox::RemoveAll() {
  m_pListBox->DeleteAll();
}

void CFWL_ComboBox::ModifyStylesEx(uint32_t dwStylesExAdded,
                                   uint32_t dwStylesExRemoved) {
  if (!m_pEdit)
    InitComboEdit();

  bool bAddDropDown = !!(dwStylesExAdded & FWL_STYLEEXT_CMB_DropDown);
  bool bDelDropDown = !!(dwStylesExRemoved & FWL_STYLEEXT_CMB_DropDown);

  dwStylesExRemoved &= ~FWL_STYLEEXT_CMB_DropDown;
  m_pProperties->m_dwStyleExes |= FWL_STYLEEXT_CMB_DropDown;

  if (bAddDropDown)
    m_pEdit->ModifyStylesEx(0, FWL_STYLEEXT_EDT_ReadOnly);
  else if (bDelDropDown)
    m_pEdit->ModifyStylesEx(FWL_STYLEEXT_EDT_ReadOnly, 0);
  CFWL_Widget::ModifyStylesEx(dwStylesExAdded, dwStylesExRemoved);
}

void CFWL_ComboBox::Update() {
  if (m_iLock)
    return;
  if (m_pEdit)
    ResetEditAlignment();
  ResetTheme();
  Layout();
}

FWL_WidgetHit CFWL_ComboBox::HitTest(const CFX_PointF& point) {
  CFX_RectF rect(0, 0, m_pProperties->m_rtWidget.width - m_rtBtn.width,
                 m_pProperties->m_rtWidget.height);
  if (rect.Contains(point))
    return FWL_WidgetHit::Edit;
  if (m_rtBtn.Contains(point))
    return FWL_WidgetHit::Client;
  if (IsDropListVisible()) {
    rect = m_pListBox->GetWidgetRect();
    if (rect.Contains(point))
      return FWL_WidgetHit::Client;
  }
  return FWL_WidgetHit::Unknown;
}

void CFWL_ComboBox::DrawWidget(CXFA_Graphics* pGraphics,
                               const CFX_Matrix& matrix) {
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  pGraphics->SaveGraphState();
  pGraphics->ConcatMatrix(&matrix);
  if (!m_rtBtn.IsEmpty(0.1f)) {
    CFWL_ThemeBackground param;
    param.m_pWidget = this;
    param.m_iPart = CFWL_Part::DropDownButton;
    param.m_dwStates = m_iBtnState;
    param.m_pGraphics = pGraphics;
    param.m_rtPart = m_rtBtn;
    pTheme->DrawBackground(param);
  }
  pGraphics->RestoreGraphState();

  if (m_pEdit) {
    CFX_RectF rtEdit = m_pEdit->GetWidgetRect();
    CFX_Matrix mt(1, 0, 0, 1, rtEdit.left, rtEdit.top);
    mt.Concat(matrix);
    m_pEdit->DrawWidget(pGraphics, mt);
  }
  if (m_pListBox && IsDropListVisible()) {
    CFX_RectF rtList = m_pListBox->GetWidgetRect();
    CFX_Matrix mt(1, 0, 0, 1, rtList.left, rtList.top);
    mt.Concat(matrix);
    m_pListBox->DrawWidget(pGraphics, mt);
  }
}

void CFWL_ComboBox::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  if (!pThemeProvider)
    return;

  m_pProperties->m_pThemeProvider = pThemeProvider;
  if (m_pListBox)
    m_pListBox->SetThemeProvider(pThemeProvider);
  if (m_pEdit)
    m_pEdit->SetThemeProvider(pThemeProvider);
}

WideString CFWL_ComboBox::GetTextByIndex(int32_t iIndex) const {
  CFWL_ListItem* pItem = static_cast<CFWL_ListItem*>(
      m_pListBox->GetItem(m_pListBox.get(), iIndex));
  return pItem ? pItem->GetText() : WideString();
}

void CFWL_ComboBox::SetCurSel(int32_t iSel) {
  int32_t iCount = m_pListBox->CountItems(nullptr);
  bool bClearSel = iSel < 0 || iSel >= iCount;
  if (IsDropDownStyle() && m_pEdit) {
    if (bClearSel) {
      m_pEdit->SetText(WideString());
    } else {
      CFWL_ListItem* hItem = m_pListBox->GetItem(this, iSel);
      m_pEdit->SetText(hItem ? hItem->GetText() : WideString());
    }
    m_pEdit->Update();
  }
  m_iCurSel = bClearSel ? -1 : iSel;
}

void CFWL_ComboBox::SetStates(uint32_t dwStates) {
  if (IsDropDownStyle() && m_pEdit)
    m_pEdit->SetStates(dwStates);
  if (m_pListBox)
    m_pListBox->SetStates(dwStates);
  CFWL_Widget::SetStates(dwStates);
}

void CFWL_ComboBox::RemoveStates(uint32_t dwStates) {
  if (IsDropDownStyle() && m_pEdit)
    m_pEdit->RemoveStates(dwStates);
  if (m_pListBox)
    m_pListBox->RemoveStates(dwStates);
  CFWL_Widget::RemoveStates(dwStates);
}

void CFWL_ComboBox::SetEditText(const WideString& wsText) {
  if (!m_pEdit)
    return;

  m_pEdit->SetText(wsText);
  m_pEdit->Update();
}

WideString CFWL_ComboBox::GetEditText() const {
  if (m_pEdit)
    return m_pEdit->GetText();
  if (!m_pListBox)
    return WideString();

  CFWL_ListItem* hItem = m_pListBox->GetItem(this, m_iCurSel);
  return hItem ? hItem->GetText() : WideString();
}

void CFWL_ComboBox::OpenDropDownList(bool bActivate) {
  ShowDropList(bActivate);
}

CFX_RectF CFWL_ComboBox::GetBBox() const {
  CFX_RectF rect = m_pProperties->m_rtWidget;
  if (!m_pListBox || !IsDropListVisible())
    return rect;

  CFX_RectF rtList = m_pListBox->GetWidgetRect();
  rtList.Offset(rect.left, rect.top);
  rect.Union(rtList);
  return rect;
}

void CFWL_ComboBox::EditModifyStylesEx(uint32_t dwStylesExAdded,
                                       uint32_t dwStylesExRemoved) {
  if (m_pEdit)
    m_pEdit->ModifyStylesEx(dwStylesExAdded, dwStylesExRemoved);
}

void CFWL_ComboBox::DrawStretchHandler(CXFA_Graphics* pGraphics,
                                       const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground param;
  param.m_pGraphics = pGraphics;
  param.m_iPart = CFWL_Part::StretchHandler;
  param.m_dwStates = CFWL_PartState_Normal;
  param.m_pWidget = this;
  if (pMatrix)
    param.m_matrix.Concat(*pMatrix);
  m_pProperties->m_pThemeProvider->DrawBackground(param);
}

void CFWL_ComboBox::ShowDropList(bool bActivate) {
  if (IsDropListVisible() == bActivate)
    return;

  if (bActivate) {
    CFWL_Event preEvent(CFWL_Event::Type::PreDropDown, this);
    DispatchEvent(&preEvent);

    CFWL_ComboList* pComboList = m_pListBox.get();
    int32_t iItems = pComboList->CountItems(nullptr);
    if (iItems < 1)
      return;

    ResetListItemAlignment();
    pComboList->ChangeSelected(m_iCurSel);

    float fItemHeight = pComboList->CalcItemHeight();
    float fBorder = GetCXBorderSize();
    float fPopupMin = 0.0f;
    if (iItems > 3)
      fPopupMin = fItemHeight * 3 + fBorder * 2;

    float fPopupMax = fItemHeight * iItems + fBorder * 2;
    CFX_RectF rtList(m_rtClient.left, 0, m_pProperties->m_rtWidget.width, 0);
    GetPopupPos(fPopupMin, fPopupMax, m_pProperties->m_rtWidget, &rtList);

    m_pListBox->SetWidgetRect(rtList);
    m_pListBox->Update();
  }

  if (bActivate) {
    m_pListBox->RemoveStates(FWL_WGTSTATE_Invisible);
    CFWL_Event postEvent(CFWL_Event::Type::PostDropDown, this);
    DispatchEvent(&postEvent);
  } else {
    m_pListBox->SetStates(FWL_WGTSTATE_Invisible);
  }

  CFX_RectF rect = m_pListBox->GetWidgetRect();
  rect.Inflate(2, 2);
  RepaintRect(rect);
}

void CFWL_ComboBox::MatchEditText() {
  WideString wsText = m_pEdit->GetText();
  int32_t iMatch = m_pListBox->MatchItem(wsText);
  if (iMatch != m_iCurSel) {
    m_pListBox->ChangeSelected(iMatch);
    if (iMatch >= 0)
      SyncEditText(iMatch);
  } else if (iMatch >= 0) {
    m_pEdit->SetSelected();
  }
  m_iCurSel = iMatch;
}

void CFWL_ComboBox::SyncEditText(int32_t iListItem) {
  CFWL_ListItem* hItem = m_pListBox->GetItem(this, iListItem);
  m_pEdit->SetText(hItem ? hItem->GetText() : WideString());
  m_pEdit->Update();
  m_pEdit->SetSelected();
}

void CFWL_ComboBox::Layout() {
  m_rtClient = GetClientRect();
  m_rtContent = m_rtClient;
  IFWL_ThemeProvider* theme = GetAvailableTheme();
  if (!theme)
    return;

  float borderWidth = 1;
  float fBtn = theme->GetScrollBarWidth();
  if (!(GetStylesEx() & FWL_STYLEEXT_CMB_ReadOnly)) {
    m_rtBtn =
        CFX_RectF(m_rtClient.right() - fBtn, m_rtClient.top + borderWidth,
                  fBtn - borderWidth, m_rtClient.height - 2 * borderWidth);
  }

  CFWL_ThemePart part;
  part.m_pWidget = this;
  CFX_RectF pUIMargin = theme->GetUIMargin(part);
  m_rtContent.Deflate(pUIMargin.left, pUIMargin.top, pUIMargin.width,
                      pUIMargin.height);

  if (!IsDropDownStyle() || !m_pEdit)
    return;

  CFX_RectF rtEdit(m_rtContent.left, m_rtContent.top, m_rtContent.width - fBtn,
                   m_rtContent.height);
  m_pEdit->SetWidgetRect(rtEdit);

  if (m_iCurSel >= 0) {
    CFWL_ListItem* hItem = m_pListBox->GetItem(this, m_iCurSel);
    m_pEdit->LockUpdate();
    m_pEdit->SetText(hItem ? hItem->GetText() : WideString());
    m_pEdit->UnlockUpdate();
  }
  m_pEdit->Update();
}

void CFWL_ComboBox::ResetTheme() {
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();

  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  if (m_pListBox && !m_pListBox->GetThemeProvider())
    m_pListBox->SetThemeProvider(pTheme);
  if (m_pEdit && !m_pEdit->GetThemeProvider())
    m_pEdit->SetThemeProvider(pTheme);
}

void CFWL_ComboBox::ResetEditAlignment() {
  if (!m_pEdit)
    return;

  uint32_t dwAdd = 0;
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CMB_EditHAlignMask) {
    case FWL_STYLEEXT_CMB_EditHCenter: {
      dwAdd |= FWL_STYLEEXT_EDT_HCenter;
      break;
    }
    default: {
      dwAdd |= FWL_STYLEEXT_EDT_HNear;
      break;
    }
  }
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CMB_EditVAlignMask) {
    case FWL_STYLEEXT_CMB_EditVCenter: {
      dwAdd |= FWL_STYLEEXT_EDT_VCenter;
      break;
    }
    case FWL_STYLEEXT_CMB_EditVFar: {
      dwAdd |= FWL_STYLEEXT_EDT_VFar;
      break;
    }
    default: {
      dwAdd |= FWL_STYLEEXT_EDT_VNear;
      break;
    }
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CMB_EditJustified)
    dwAdd |= FWL_STYLEEXT_EDT_Justified;

  m_pEdit->ModifyStylesEx(dwAdd, FWL_STYLEEXT_EDT_HAlignMask |
                                     FWL_STYLEEXT_EDT_HAlignModeMask |
                                     FWL_STYLEEXT_EDT_VAlignMask);
}

void CFWL_ComboBox::ResetListItemAlignment() {
  if (!m_pListBox)
    return;

  uint32_t dwAdd = 0;
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CMB_ListItemAlignMask) {
    case FWL_STYLEEXT_CMB_ListItemCenterAlign: {
      dwAdd |= FWL_STYLEEXT_LTB_CenterAlign;
      break;
    }
    default: {
      dwAdd |= FWL_STYLEEXT_LTB_LeftAlign;
      break;
    }
  }
  m_pListBox->ModifyStylesEx(dwAdd, FWL_STYLEEXT_CMB_ListItemAlignMask);
}

void CFWL_ComboBox::ProcessSelChanged(bool bLButtonUp) {
  m_iCurSel = m_pListBox->GetItemIndex(this, m_pListBox->GetSelItem(0));
  if (!IsDropDownStyle()) {
    RepaintRect(m_rtClient);
    return;
  }

  CFWL_ListItem* hItem = m_pListBox->GetItem(this, m_iCurSel);
  if (!hItem)
    return;
  if (m_pEdit) {
    m_pEdit->SetText(hItem->GetText());
    m_pEdit->Update();
    m_pEdit->SetSelected();
  }

  CFWL_EventSelectChanged ev(this);
  ev.bLButtonUp = bLButtonUp;
  DispatchEvent(&ev);
}

void CFWL_ComboBox::InitComboList() {
  if (m_pListBox)
    return;

  auto prop = pdfium::MakeUnique<CFWL_WidgetProperties>();
  prop->m_pParent = this;
  prop->m_dwStyles = FWL_WGTSTYLE_Border | FWL_WGTSTYLE_VScroll;
  prop->m_dwStates = FWL_WGTSTATE_Invisible;
  prop->m_pThemeProvider = m_pProperties->m_pThemeProvider;
  m_pListBox = pdfium::MakeUnique<CFWL_ComboList>(m_pOwnerApp.Get(),
                                                  std::move(prop), this);
}

void CFWL_ComboBox::InitComboEdit() {
  if (m_pEdit)
    return;

  auto prop = pdfium::MakeUnique<CFWL_WidgetProperties>();
  prop->m_pParent = this;
  prop->m_pThemeProvider = m_pProperties->m_pThemeProvider;

  m_pEdit = pdfium::MakeUnique<CFWL_ComboEdit>(m_pOwnerApp.Get(),
                                               std::move(prop), this);
  m_pEdit->SetOuter(this);
}

void CFWL_ComboBox::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  bool backDefault = true;
  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus: {
      backDefault = false;
      OnFocusChanged(pMessage, true);
      break;
    }
    case CFWL_Message::Type::KillFocus: {
      backDefault = false;
      OnFocusChanged(pMessage, false);
      break;
    }
    case CFWL_Message::Type::Mouse: {
      backDefault = false;
      CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
      switch (pMsg->m_dwCmd) {
        case FWL_MouseCommand::LeftButtonDown:
          OnLButtonDown(pMsg);
          break;
        case FWL_MouseCommand::LeftButtonUp:
          OnLButtonUp(pMsg);
          break;
        default:
          break;
      }
      break;
    }
    case CFWL_Message::Type::Key: {
      backDefault = false;
      CFWL_MessageKey* pKey = static_cast<CFWL_MessageKey*>(pMessage);
      if (pKey->m_dwCmd == FWL_KeyCommand::KeyUp)
        break;
      if (IsDropListVisible() && pKey->m_dwCmd == FWL_KeyCommand::KeyDown) {
        bool bListKey = pKey->m_dwKeyCode == FWL_VKEY_Up ||
                        pKey->m_dwKeyCode == FWL_VKEY_Down ||
                        pKey->m_dwKeyCode == FWL_VKEY_Return ||
                        pKey->m_dwKeyCode == FWL_VKEY_Escape;
        if (bListKey) {
          m_pListBox->GetDelegate()->OnProcessMessage(pMessage);
          break;
        }
      }
      OnKey(pKey);
      break;
    }
    default:
      break;
  }
  if (backDefault)
    CFWL_Widget::OnProcessMessage(pMessage);
}

void CFWL_ComboBox::OnProcessEvent(CFWL_Event* pEvent) {
  CFWL_Event::Type type = pEvent->GetType();
  if (type == CFWL_Event::Type::Scroll) {
    CFWL_EventScroll* pScrollEvent = static_cast<CFWL_EventScroll*>(pEvent);
    CFWL_EventScroll pScrollEv(this);
    pScrollEv.m_iScrollCode = pScrollEvent->m_iScrollCode;
    pScrollEv.m_fPos = pScrollEvent->m_fPos;
    DispatchEvent(&pScrollEv);
  } else if (type == CFWL_Event::Type::TextWillChange) {
    CFWL_Event pTemp(CFWL_Event::Type::EditChanged, this);
    DispatchEvent(&pTemp);
  }
}

void CFWL_ComboBox::OnDrawWidget(CXFA_Graphics* pGraphics,
                                 const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}

void CFWL_ComboBox::OnLButtonUp(CFWL_MessageMouse* pMsg) {
  if (m_rtBtn.Contains(pMsg->m_pos))
    m_iBtnState = CFWL_PartState_Hovered;
  else
    m_iBtnState = CFWL_PartState_Normal;

  RepaintRect(m_rtBtn);
}

void CFWL_ComboBox::OnLButtonDown(CFWL_MessageMouse* pMsg) {
  bool bDropDown = IsDropListVisible();
  CFX_RectF& rtBtn = bDropDown ? m_rtBtn : m_rtClient;
  if (!rtBtn.Contains(pMsg->m_pos))
    return;

  if (IsDropListVisible()) {
    ShowDropList(false);
    return;
  }
  if (m_pEdit)
    MatchEditText();
  ShowDropList(true);
}

void CFWL_ComboBox::OnFocusChanged(CFWL_Message* pMsg, bool bSet) {
  if (bSet) {
    m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
    if ((m_pEdit->GetStates() & FWL_WGTSTATE_Focused) == 0) {
      CFWL_MessageSetFocus msg(nullptr, m_pEdit.get());
      m_pEdit->GetDelegate()->OnProcessMessage(&msg);
    }
  } else {
    m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
    ShowDropList(false);
    CFWL_MessageKillFocus msg(m_pEdit.get());
    m_pEdit->GetDelegate()->OnProcessMessage(&msg);
  }
}

void CFWL_ComboBox::OnKey(CFWL_MessageKey* pMsg) {
  uint32_t dwKeyCode = pMsg->m_dwKeyCode;
  const bool bUp = dwKeyCode == FWL_VKEY_Up;
  const bool bDown = dwKeyCode == FWL_VKEY_Down;
  if (bUp || bDown) {
    CFWL_ComboList* pComboList = m_pListBox.get();
    int32_t iCount = pComboList->CountItems(nullptr);
    if (iCount < 1)
      return;

    bool bMatchEqual = false;
    int32_t iCurSel = m_iCurSel;
    if (m_pEdit) {
      WideString wsText = m_pEdit->GetText();
      iCurSel = pComboList->MatchItem(wsText);
      if (iCurSel >= 0) {
        CFWL_ListItem* item = m_pListBox->GetSelItem(iCurSel);
        bMatchEqual = wsText == (item ? item->GetText() : WideString());
      }
    }
    if (iCurSel < 0) {
      iCurSel = 0;
    } else if (bMatchEqual) {
      if ((bUp && iCurSel == 0) || (bDown && iCurSel == iCount - 1))
        return;
      if (bUp)
        iCurSel--;
      else
        iCurSel++;
    }
    m_iCurSel = iCurSel;
    SyncEditText(m_iCurSel);
    return;
  }
  if (m_pEdit)
    m_pEdit->GetDelegate()->OnProcessMessage(pMsg);
}

void CFWL_ComboBox::GetPopupPos(float fMinHeight,
                                float fMaxHeight,
                                const CFX_RectF& rtAnchor,
                                CFX_RectF* pPopupRect) {
  m_pWidgetMgr->GetAdapterPopupPos(this, fMinHeight, fMaxHeight, rtAnchor,
                                   pPopupRect);
}
