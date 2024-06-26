// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_listbox.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_messagekey.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_messagemousewheel.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themepart.h"
#include "xfa/fwl/cfwl_themetext.h"
#include "xfa/fwl/ifwl_themeprovider.h"

namespace {

const int kItemTextMargin = 2;

}  // namespace

CFWL_ListBox::CFWL_ListBox(const CFWL_App* app,
                           std::unique_ptr<CFWL_WidgetProperties> properties,
                           CFWL_Widget* pOuter)
    : CFWL_Widget(app, std::move(properties), pOuter) {}

CFWL_ListBox::~CFWL_ListBox() {}

FWL_Type CFWL_ListBox::GetClassID() const {
  return FWL_Type::ListBox;
}

void CFWL_ListBox::Update() {
  if (IsLocked())
    return;
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();

  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_AlignMask) {
    case FWL_STYLEEXT_LTB_LeftAlign: {
      m_iTTOAligns = FDE_TextAlignment::kCenterLeft;
      break;
    }
    case FWL_STYLEEXT_LTB_RightAlign: {
      m_iTTOAligns = FDE_TextAlignment::kCenterRight;
      break;
    }
    case FWL_STYLEEXT_LTB_CenterAlign:
    default: {
      m_iTTOAligns = FDE_TextAlignment::kCenter;
      break;
    }
  }
  m_TTOStyles.single_line_ = true;
  m_fScorllBarWidth = GetScrollWidth();
  CalcSize(false);
}

FWL_WidgetHit CFWL_ListBox::HitTest(const CFX_PointF& point) {
  if (IsShowScrollBar(false)) {
    CFX_RectF rect = m_pHorzScrollBar->GetWidgetRect();
    if (rect.Contains(point))
      return FWL_WidgetHit::HScrollBar;
  }
  if (IsShowScrollBar(true)) {
    CFX_RectF rect = m_pVertScrollBar->GetWidgetRect();
    if (rect.Contains(point))
      return FWL_WidgetHit::VScrollBar;
  }
  if (m_rtClient.Contains(point))
    return FWL_WidgetHit::Client;
  return FWL_WidgetHit::Unknown;
}

void CFWL_ListBox::DrawWidget(CXFA_Graphics* pGraphics,
                              const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  if (!pTheme)
    return;

  pGraphics->SaveGraphState();
  if (HasBorder())
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, matrix);

  CFX_RectF rtClip(m_rtConent);
  if (IsShowScrollBar(false))
    rtClip.height -= m_fScorllBarWidth;
  if (IsShowScrollBar(true))
    rtClip.width -= m_fScorllBarWidth;

  pGraphics->SetClipRect(matrix.TransformRect(rtClip));
  if ((m_pProperties->m_dwStyles & FWL_WGTSTYLE_NoBackground) == 0)
    DrawBkground(pGraphics, pTheme, &matrix);

  DrawItems(pGraphics, pTheme, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_ListBox::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  if (pThemeProvider)
    m_pProperties->m_pThemeProvider = pThemeProvider;
}

int32_t CFWL_ListBox::CountSelItems() {
  int32_t iRet = 0;
  int32_t iCount = CountItems(this);
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_ListItem* pItem = GetItem(this, i);
    if (!pItem)
      continue;
    if (pItem->GetStates() & FWL_ITEMSTATE_LTB_Selected)
      iRet++;
  }
  return iRet;
}

CFWL_ListItem* CFWL_ListBox::GetSelItem(int32_t nIndexSel) {
  int32_t idx = GetSelIndex(nIndexSel);
  if (idx < 0)
    return nullptr;
  return GetItem(this, idx);
}

int32_t CFWL_ListBox::GetSelIndex(int32_t nIndex) {
  int32_t index = 0;
  int32_t iCount = CountItems(this);
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_ListItem* pItem = GetItem(this, i);
    if (!pItem)
      return -1;
    if (pItem->GetStates() & FWL_ITEMSTATE_LTB_Selected) {
      if (index == nIndex)
        return i;
      index++;
    }
  }
  return -1;
}

void CFWL_ListBox::SetSelItem(CFWL_ListItem* pItem, bool bSelect) {
  if (!pItem) {
    if (bSelect) {
      SelectAll();
    } else {
      ClearSelection();
      SetFocusItem(nullptr);
    }
    return;
  }
  if (IsMultiSelection())
    SetSelectionDirect(pItem, bSelect);
  else
    SetSelection(pItem, pItem, bSelect);
}

CFWL_ListItem* CFWL_ListBox::GetListItem(CFWL_ListItem* pItem,
                                         uint32_t dwKeyCode) {
  CFWL_ListItem* hRet = nullptr;
  switch (dwKeyCode) {
    case FWL_VKEY_Up:
    case FWL_VKEY_Down:
    case FWL_VKEY_Home:
    case FWL_VKEY_End: {
      const bool bUp = dwKeyCode == FWL_VKEY_Up;
      const bool bDown = dwKeyCode == FWL_VKEY_Down;
      const bool bHome = dwKeyCode == FWL_VKEY_Home;
      int32_t iDstItem = -1;
      if (bUp || bDown) {
        int32_t index = GetItemIndex(this, pItem);
        iDstItem = dwKeyCode == FWL_VKEY_Up ? index - 1 : index + 1;
      } else if (bHome) {
        iDstItem = 0;
      } else {
        int32_t iCount = CountItems(this);
        iDstItem = iCount - 1;
      }
      hRet = GetItem(this, iDstItem);
      break;
    }
    default:
      break;
  }
  return hRet;
}

void CFWL_ListBox::SetSelection(CFWL_ListItem* hStart,
                                CFWL_ListItem* hEnd,
                                bool bSelected) {
  int32_t iStart = GetItemIndex(this, hStart);
  int32_t iEnd = GetItemIndex(this, hEnd);
  if (iStart > iEnd) {
    int32_t iTemp = iStart;
    iStart = iEnd;
    iEnd = iTemp;
  }
  if (bSelected) {
    int32_t iCount = CountItems(this);
    for (int32_t i = 0; i < iCount; i++) {
      CFWL_ListItem* pItem = GetItem(this, i);
      SetSelectionDirect(pItem, false);
    }
  }
  for (; iStart <= iEnd; iStart++) {
    CFWL_ListItem* pItem = GetItem(this, iStart);
    SetSelectionDirect(pItem, bSelected);
  }
}

void CFWL_ListBox::SetSelectionDirect(CFWL_ListItem* pItem, bool bSelect) {
  if (!pItem)
    return;

  uint32_t dwOldStyle = pItem->GetStates();
  bSelect ? dwOldStyle |= FWL_ITEMSTATE_LTB_Selected
          : dwOldStyle &= ~FWL_ITEMSTATE_LTB_Selected;
  pItem->SetStates(dwOldStyle);
}

bool CFWL_ListBox::IsMultiSelection() const {
  return m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_MultiSelection;
}

bool CFWL_ListBox::IsItemSelected(CFWL_ListItem* pItem) {
  return pItem && (pItem->GetStates() & FWL_ITEMSTATE_LTB_Selected) != 0;
}

void CFWL_ListBox::ClearSelection() {
  bool bMulti = IsMultiSelection();
  int32_t iCount = CountItems(this);
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_ListItem* pItem = GetItem(this, i);
    if (!pItem)
      continue;
    if (!(pItem->GetStates() & FWL_ITEMSTATE_LTB_Selected))
      continue;
    SetSelectionDirect(pItem, false);
    if (!bMulti)
      return;
  }
}

void CFWL_ListBox::SelectAll() {
  if (!IsMultiSelection())
    return;

  int32_t iCount = CountItems(this);
  if (iCount <= 0)
    return;

  CFWL_ListItem* pItemStart = GetItem(this, 0);
  CFWL_ListItem* pItemEnd = GetItem(this, iCount - 1);
  SetSelection(pItemStart, pItemEnd, false);
}

CFWL_ListItem* CFWL_ListBox::GetFocusedItem() {
  int32_t iCount = CountItems(this);
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_ListItem* pItem = GetItem(this, i);
    if (!pItem)
      return nullptr;
    if (pItem->GetStates() & FWL_ITEMSTATE_LTB_Focused)
      return pItem;
  }
  return nullptr;
}

void CFWL_ListBox::SetFocusItem(CFWL_ListItem* pItem) {
  CFWL_ListItem* hFocus = GetFocusedItem();
  if (pItem == hFocus)
    return;

  if (hFocus) {
    uint32_t dwStyle = hFocus->GetStates();
    dwStyle &= ~FWL_ITEMSTATE_LTB_Focused;
    hFocus->SetStates(dwStyle);
  }
  if (pItem) {
    uint32_t dwStyle = pItem->GetStates();
    dwStyle |= FWL_ITEMSTATE_LTB_Focused;
    pItem->SetStates(dwStyle);
  }
}

CFWL_ListItem* CFWL_ListBox::GetItemAtPoint(const CFX_PointF& point) {
  CFX_PointF pos = point - m_rtConent.TopLeft();
  float fPosX = 0.0f;
  if (m_pHorzScrollBar)
    fPosX = m_pHorzScrollBar->GetPos();

  float fPosY = 0.0;
  if (m_pVertScrollBar)
    fPosY = m_pVertScrollBar->GetPos();

  int32_t nCount = CountItems(this);
  for (int32_t i = 0; i < nCount; i++) {
    CFWL_ListItem* pItem = GetItem(this, i);
    if (!pItem)
      continue;

    CFX_RectF rtItem = pItem->GetRect();
    rtItem.Offset(-fPosX, -fPosY);
    if (rtItem.Contains(pos))
      return pItem;
  }
  return nullptr;
}

bool CFWL_ListBox::ScrollToVisible(CFWL_ListItem* pItem) {
  if (!m_pVertScrollBar)
    return false;

  CFX_RectF rtItem = pItem ? pItem->GetRect() : CFX_RectF();
  bool bScroll = false;
  float fPosY = m_pVertScrollBar->GetPos();
  rtItem.Offset(0, -fPosY + m_rtConent.top);
  if (rtItem.top < m_rtConent.top) {
    fPosY += rtItem.top - m_rtConent.top;
    bScroll = true;
  } else if (rtItem.bottom() > m_rtConent.bottom()) {
    fPosY += rtItem.bottom() - m_rtConent.bottom();
    bScroll = true;
  }
  if (!bScroll)
    return false;

  m_pVertScrollBar->SetPos(fPosY);
  m_pVertScrollBar->SetTrackPos(fPosY);
  RepaintRect(m_rtClient);
  return true;
}

void CFWL_ListBox::DrawBkground(CXFA_Graphics* pGraphics,
                                IFWL_ThemeProvider* pTheme,
                                const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return;
  if (!pTheme)
    return;

  CFWL_ThemeBackground param;
  param.m_pWidget = this;
  param.m_iPart = CFWL_Part::Background;
  param.m_dwStates = 0;
  param.m_pGraphics = pGraphics;
  param.m_matrix.Concat(*pMatrix);
  param.m_rtPart = m_rtClient;
  if (IsShowScrollBar(false) && IsShowScrollBar(true))
    param.m_pRtData = &m_rtStatic;
  if (!IsEnabled())
    param.m_dwStates = CFWL_PartState_Disabled;

  pTheme->DrawBackground(param);
}

void CFWL_ListBox::DrawItems(CXFA_Graphics* pGraphics,
                             IFWL_ThemeProvider* pTheme,
                             const CFX_Matrix* pMatrix) {
  float fPosX = 0.0f;
  if (m_pHorzScrollBar)
    fPosX = m_pHorzScrollBar->GetPos();

  float fPosY = 0.0f;
  if (m_pVertScrollBar)
    fPosY = m_pVertScrollBar->GetPos();

  CFX_RectF rtView(m_rtConent);
  if (m_pHorzScrollBar)
    rtView.height -= m_fScorllBarWidth;
  if (m_pVertScrollBar)
    rtView.width -= m_fScorllBarWidth;

  int32_t iCount = CountItems(this);
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_ListItem* pItem = GetItem(this, i);
    if (!pItem)
      continue;

    CFX_RectF rtItem = pItem->GetRect();
    rtItem.Offset(m_rtConent.left - fPosX, m_rtConent.top - fPosY);
    if (rtItem.bottom() < m_rtConent.top)
      continue;
    if (rtItem.top >= m_rtConent.bottom())
      break;
    DrawItem(pGraphics, pTheme, pItem, i, rtItem, pMatrix);
  }
}

void CFWL_ListBox::DrawItem(CXFA_Graphics* pGraphics,
                            IFWL_ThemeProvider* pTheme,
                            CFWL_ListItem* pItem,
                            int32_t Index,
                            const CFX_RectF& rtItem,
                            const CFX_Matrix* pMatrix) {
  uint32_t dwItemStyles = pItem ? pItem->GetStates() : 0;
  uint32_t dwPartStates = CFWL_PartState_Normal;
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)
    dwPartStates = CFWL_PartState_Disabled;
  else if (dwItemStyles & FWL_ITEMSTATE_LTB_Selected)
    dwPartStates = CFWL_PartState_Selected;

  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused &&
      dwItemStyles & FWL_ITEMSTATE_LTB_Focused) {
    dwPartStates |= CFWL_PartState_Focused;
  }

  CFWL_ThemeBackground bg_param;
  bg_param.m_pWidget = this;
  bg_param.m_iPart = CFWL_Part::ListItem;
  bg_param.m_dwStates = dwPartStates;
  bg_param.m_pGraphics = pGraphics;
  bg_param.m_matrix.Concat(*pMatrix);
  bg_param.m_rtPart = rtItem;
  bg_param.m_bMaximize = true;
  CFX_RectF rtFocus(rtItem);
  bg_param.m_pRtData = &rtFocus;
  if (m_pVertScrollBar && !m_pHorzScrollBar &&
      (dwPartStates & CFWL_PartState_Focused)) {
    bg_param.m_rtPart.left += 1;
    bg_param.m_rtPart.width -= (m_fScorllBarWidth + 1);
    rtFocus.Deflate(0.5, 0.5, 1 + m_fScorllBarWidth, 1);
  }
  pTheme->DrawBackground(bg_param);

  if (!pItem)
    return;

  WideString wsText = pItem->GetText();
  if (wsText.GetLength() <= 0)
    return;

  CFX_RectF rtText(rtItem);
  rtText.Deflate(kItemTextMargin, kItemTextMargin);

  CFWL_ThemeText textParam;
  textParam.m_pWidget = this;
  textParam.m_iPart = CFWL_Part::ListItem;
  textParam.m_dwStates = dwPartStates;
  textParam.m_pGraphics = pGraphics;
  textParam.m_matrix.Concat(*pMatrix);
  textParam.m_rtPart = rtText;
  textParam.m_wsText = std::move(wsText);
  textParam.m_dwTTOStyles = m_TTOStyles;
  textParam.m_iTTOAlign = m_iTTOAligns;
  textParam.m_bMaximize = true;
  pTheme->DrawText(textParam);
}

CFX_SizeF CFWL_ListBox::CalcSize(bool bAutoSize) {
  if (!m_pProperties->m_pThemeProvider)
    return CFX_SizeF();

  m_rtClient = GetClientRect();
  m_rtConent = m_rtClient;
  CFX_RectF rtUIMargin;
  if (!m_pOuter) {
    CFWL_ThemePart part;
    part.m_pWidget = this;
    IFWL_ThemeProvider* theme = GetAvailableTheme();
    CFX_RectF pUIMargin = theme ? theme->GetUIMargin(part) : CFX_RectF();
    m_rtConent.Deflate(pUIMargin.left, pUIMargin.top, pUIMargin.width,
                       pUIMargin.height);
  }

  float fWidth = GetMaxTextWidth();
  fWidth += 2 * kItemTextMargin;
  if (!bAutoSize) {
    float fActualWidth = m_rtClient.width - rtUIMargin.left - rtUIMargin.width;
    fWidth = std::max(fWidth, fActualWidth);
  }
  m_fItemHeight = CalcItemHeight();

  int32_t iCount = CountItems(this);
  CFX_SizeF fs;
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_ListItem* htem = GetItem(this, i);
    UpdateItemSize(htem, fs, fWidth, m_fItemHeight, bAutoSize);
  }
  if (bAutoSize)
    return fs;

  float iHeight = m_rtClient.height;
  bool bShowVertScr = false;
  bool bShowHorzScr = false;
  if (!bShowVertScr && (m_pProperties->m_dwStyles & FWL_WGTSTYLE_VScroll))
    bShowVertScr = (fs.height > iHeight);

  CFX_SizeF szRange;
  if (bShowVertScr) {
    if (!m_pVertScrollBar)
      InitVerticalScrollBar();

    CFX_RectF rtScrollBar(m_rtClient.right() - m_fScorllBarWidth,
                          m_rtClient.top, m_fScorllBarWidth,
                          m_rtClient.height - 1);
    if (bShowHorzScr)
      rtScrollBar.height -= m_fScorllBarWidth;

    m_pVertScrollBar->SetWidgetRect(rtScrollBar);
    szRange.width = 0;
    szRange.height = std::max(fs.height - m_rtConent.height, m_fItemHeight);

    m_pVertScrollBar->SetRange(szRange.width, szRange.height);
    m_pVertScrollBar->SetPageSize(rtScrollBar.height * 9 / 10);
    m_pVertScrollBar->SetStepSize(m_fItemHeight);

    float fPos =
        pdfium::clamp(m_pVertScrollBar->GetPos(), 0.0f, szRange.height);
    m_pVertScrollBar->SetPos(fPos);
    m_pVertScrollBar->SetTrackPos(fPos);
    if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_ShowScrollBarFocus) ==
            0 ||
        (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused)) {
      m_pVertScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
    }
    m_pVertScrollBar->Update();
  } else if (m_pVertScrollBar) {
    m_pVertScrollBar->SetPos(0);
    m_pVertScrollBar->SetTrackPos(0);
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible);
  }
  if (bShowHorzScr) {
    if (!m_pHorzScrollBar)
      InitHorizontalScrollBar();

    CFX_RectF rtScrollBar(m_rtClient.left,
                          m_rtClient.bottom() - m_fScorllBarWidth,
                          m_rtClient.width, m_fScorllBarWidth);
    if (bShowVertScr)
      rtScrollBar.width -= m_fScorllBarWidth;

    m_pHorzScrollBar->SetWidgetRect(rtScrollBar);
    szRange.width = 0;
    szRange.height = fs.width - rtScrollBar.width;
    m_pHorzScrollBar->SetRange(szRange.width, szRange.height);
    m_pHorzScrollBar->SetPageSize(fWidth * 9 / 10);
    m_pHorzScrollBar->SetStepSize(fWidth / 10);

    float fPos =
        pdfium::clamp(m_pHorzScrollBar->GetPos(), 0.0f, szRange.height);
    m_pHorzScrollBar->SetPos(fPos);
    m_pHorzScrollBar->SetTrackPos(fPos);
    if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_ShowScrollBarFocus) ==
            0 ||
        (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused)) {
      m_pHorzScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
    }
    m_pHorzScrollBar->Update();
  } else if (m_pHorzScrollBar) {
    m_pHorzScrollBar->SetPos(0);
    m_pHorzScrollBar->SetTrackPos(0);
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible);
  }
  if (bShowVertScr && bShowHorzScr) {
    m_rtStatic = CFX_RectF(m_rtClient.right() - m_fScorllBarWidth,
                           m_rtClient.bottom() - m_fScorllBarWidth,
                           m_fScorllBarWidth, m_fScorllBarWidth);
  }
  return fs;
}

void CFWL_ListBox::UpdateItemSize(CFWL_ListItem* pItem,
                                  CFX_SizeF& size,
                                  float fWidth,
                                  float fItemHeight,
                                  bool bAutoSize) const {
  if (!bAutoSize && pItem) {
    CFX_RectF rtItem(0, size.height, fWidth, fItemHeight);
    pItem->SetRect(rtItem);
  }
  size.width = fWidth;
  size.height += fItemHeight;
}

float CFWL_ListBox::GetMaxTextWidth() {
  float fRet = 0.0f;
  int32_t iCount = CountItems(this);
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_ListItem* pItem = GetItem(this, i);
    if (!pItem)
      continue;

    CFX_SizeF sz = CalcTextSize(pItem->GetText(),
                                m_pProperties->m_pThemeProvider.Get(), false);
    fRet = std::max(fRet, sz.width);
  }
  return fRet;
}

float CFWL_ListBox::GetScrollWidth() {
  IFWL_ThemeProvider* theme = GetAvailableTheme();
  return theme ? theme->GetScrollBarWidth() : 0.0f;
}

float CFWL_ListBox::CalcItemHeight() {
  IFWL_ThemeProvider* theme = GetAvailableTheme();
  CFWL_ThemePart part;
  part.m_pWidget = this;
  return (theme ? theme->GetFontSize(part) : 20.0f) + 2 * kItemTextMargin;
}

void CFWL_ListBox::InitVerticalScrollBar() {
  if (m_pVertScrollBar)
    return;

  auto prop = pdfium::MakeUnique<CFWL_WidgetProperties>();
  prop->m_dwStyleExes = FWL_STYLEEXT_SCB_Vert;
  prop->m_dwStates = FWL_WGTSTATE_Invisible;
  prop->m_pParent = this;
  prop->m_pThemeProvider = m_pScrollBarTP;
  m_pVertScrollBar = pdfium::MakeUnique<CFWL_ScrollBar>(m_pOwnerApp.Get(),
                                                        std::move(prop), this);
}

void CFWL_ListBox::InitHorizontalScrollBar() {
  if (m_pHorzScrollBar)
    return;

  auto prop = pdfium::MakeUnique<CFWL_WidgetProperties>();
  prop->m_dwStyleExes = FWL_STYLEEXT_SCB_Horz;
  prop->m_dwStates = FWL_WGTSTATE_Invisible;
  prop->m_pParent = this;
  prop->m_pThemeProvider = m_pScrollBarTP;
  m_pHorzScrollBar = pdfium::MakeUnique<CFWL_ScrollBar>(m_pOwnerApp.Get(),
                                                        std::move(prop), this);
}

bool CFWL_ListBox::IsShowScrollBar(bool bVert) {
  CFWL_ScrollBar* pScrollbar =
      bVert ? m_pVertScrollBar.get() : m_pHorzScrollBar.get();
  if (!pScrollbar || !pScrollbar->IsVisible())
    return false;

  return !(m_pProperties->m_dwStyleExes &
           FWL_STYLEEXT_LTB_ShowScrollBarFocus) ||
         (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused);
}

void CFWL_ListBox::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;
  if (!IsEnabled())
    return;

  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus:
      OnFocusChanged(pMessage, true);
      break;
    case CFWL_Message::Type::KillFocus:
      OnFocusChanged(pMessage, false);
      break;
    case CFWL_Message::Type::Mouse: {
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
    case CFWL_Message::Type::MouseWheel:
      OnMouseWheel(static_cast<CFWL_MessageMouseWheel*>(pMessage));
      break;
    case CFWL_Message::Type::Key: {
      CFWL_MessageKey* pMsg = static_cast<CFWL_MessageKey*>(pMessage);
      if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown)
        OnKeyDown(pMsg);
      break;
    }
    default:
      break;
  }
  CFWL_Widget::OnProcessMessage(pMessage);
}

void CFWL_ListBox::OnProcessEvent(CFWL_Event* pEvent) {
  if (!pEvent)
    return;
  if (pEvent->GetType() != CFWL_Event::Type::Scroll)
    return;

  CFWL_Widget* pSrcTarget = pEvent->GetSrcTarget();
  if ((pSrcTarget == m_pVertScrollBar.get() && m_pVertScrollBar) ||
      (pSrcTarget == m_pHorzScrollBar.get() && m_pHorzScrollBar)) {
    CFWL_EventScroll* pScrollEvent = static_cast<CFWL_EventScroll*>(pEvent);
    OnScroll(static_cast<CFWL_ScrollBar*>(pSrcTarget),
             pScrollEvent->m_iScrollCode, pScrollEvent->m_fPos);
  }
}

void CFWL_ListBox::OnDrawWidget(CXFA_Graphics* pGraphics,
                                const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}

void CFWL_ListBox::OnFocusChanged(CFWL_Message* pMsg, bool bSet) {
  if (GetStylesEx() & FWL_STYLEEXT_LTB_ShowScrollBarFocus) {
    if (m_pVertScrollBar) {
      if (bSet)
        m_pVertScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
      else
        m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible);
    }
    if (m_pHorzScrollBar) {
      if (bSet)
        m_pHorzScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
      else
        m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible);
    }
  }
  if (bSet)
    m_pProperties->m_dwStates |= (FWL_WGTSTATE_Focused);
  else
    m_pProperties->m_dwStates &= ~(FWL_WGTSTATE_Focused);

  RepaintRect(m_rtClient);
}

void CFWL_ListBox::OnLButtonDown(CFWL_MessageMouse* pMsg) {
  m_bLButtonDown = true;

  CFWL_ListItem* pItem = GetItemAtPoint(pMsg->m_pos);
  if (!pItem)
    return;

  if (IsMultiSelection()) {
    if (pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl) {
      bool bSelected = IsItemSelected(pItem);
      SetSelectionDirect(pItem, !bSelected);
      m_hAnchor = pItem;
    } else if (pMsg->m_dwFlags & FWL_KEYFLAG_Shift) {
      if (m_hAnchor)
        SetSelection(m_hAnchor, pItem, true);
      else
        SetSelectionDirect(pItem, true);
    } else {
      SetSelection(pItem, pItem, true);
      m_hAnchor = pItem;
    }
  } else {
    SetSelection(pItem, pItem, true);
  }

  SetFocusItem(pItem);
  ScrollToVisible(pItem);
  SetGrab(true);
  RepaintRect(m_rtClient);
}

void CFWL_ListBox::OnLButtonUp(CFWL_MessageMouse* pMsg) {
  if (!m_bLButtonDown)
    return;

  m_bLButtonDown = false;
  SetGrab(false);
}

void CFWL_ListBox::OnMouseWheel(CFWL_MessageMouseWheel* pMsg) {
  if (IsShowScrollBar(true))
    m_pVertScrollBar->GetDelegate()->OnProcessMessage(pMsg);
}

void CFWL_ListBox::OnKeyDown(CFWL_MessageKey* pMsg) {
  uint32_t dwKeyCode = pMsg->m_dwKeyCode;
  switch (dwKeyCode) {
    case FWL_VKEY_Tab:
    case FWL_VKEY_Up:
    case FWL_VKEY_Down:
    case FWL_VKEY_Home:
    case FWL_VKEY_End: {
      CFWL_ListItem* pItem = GetFocusedItem();
      pItem = GetListItem(pItem, dwKeyCode);
      bool bShift = !!(pMsg->m_dwFlags & FWL_KEYFLAG_Shift);
      bool bCtrl = !!(pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl);
      OnVK(pItem, bShift, bCtrl);
      break;
    }
    default:
      break;
  }
}

void CFWL_ListBox::OnVK(CFWL_ListItem* pItem, bool bShift, bool bCtrl) {
  if (!pItem)
    return;

  if (IsMultiSelection()) {
    if (bCtrl) {
      // Do nothing.
    } else if (bShift) {
      if (m_hAnchor)
        SetSelection(m_hAnchor, pItem, true);
      else
        SetSelectionDirect(pItem, true);
    } else {
      SetSelection(pItem, pItem, true);
      m_hAnchor = pItem;
    }
  } else {
    SetSelection(pItem, pItem, true);
  }

  SetFocusItem(pItem);
  ScrollToVisible(pItem);

  RepaintRect(CFX_RectF(0, 0, m_pProperties->m_rtWidget.width,
                        m_pProperties->m_rtWidget.height));
}

bool CFWL_ListBox::OnScroll(CFWL_ScrollBar* pScrollBar,
                            CFWL_EventScroll::Code dwCode,
                            float fPos) {
  CFX_SizeF fs;
  pScrollBar->GetRange(&fs.width, &fs.height);
  float iCurPos = pScrollBar->GetPos();
  float fStep = pScrollBar->GetStepSize();
  switch (dwCode) {
    case CFWL_EventScroll::Code::Min: {
      fPos = fs.width;
      break;
    }
    case CFWL_EventScroll::Code::Max: {
      fPos = fs.height;
      break;
    }
    case CFWL_EventScroll::Code::StepBackward: {
      fPos -= fStep;
      if (fPos < fs.width + fStep / 2)
        fPos = fs.width;
      break;
    }
    case CFWL_EventScroll::Code::StepForward: {
      fPos += fStep;
      if (fPos > fs.height - fStep / 2)
        fPos = fs.height;
      break;
    }
    case CFWL_EventScroll::Code::PageBackward: {
      fPos -= pScrollBar->GetPageSize();
      if (fPos < fs.width)
        fPos = fs.width;
      break;
    }
    case CFWL_EventScroll::Code::PageForward: {
      fPos += pScrollBar->GetPageSize();
      if (fPos > fs.height)
        fPos = fs.height;
      break;
    }
    case CFWL_EventScroll::Code::Pos:
    case CFWL_EventScroll::Code::TrackPos:
    case CFWL_EventScroll::Code::None:
      break;
    case CFWL_EventScroll::Code::EndScroll:
      return false;
  }
  if (iCurPos != fPos) {
    pScrollBar->SetPos(fPos);
    pScrollBar->SetTrackPos(fPos);
    RepaintRect(m_rtClient);
  }
  return true;
}

int32_t CFWL_ListBox::CountItems(const CFWL_Widget* pWidget) const {
  return pdfium::CollectionSize<int32_t>(m_ItemArray);
}

CFWL_ListItem* CFWL_ListBox::GetItem(const CFWL_Widget* pWidget,
                                     int32_t nIndex) const {
  if (nIndex < 0 || nIndex >= CountItems(pWidget))
    return nullptr;
  return m_ItemArray[nIndex].get();
}

int32_t CFWL_ListBox::GetItemIndex(CFWL_Widget* pWidget, CFWL_ListItem* pItem) {
  auto it =
      std::find_if(m_ItemArray.begin(), m_ItemArray.end(),
                   [pItem](const std::unique_ptr<CFWL_ListItem>& candidate) {
                     return candidate.get() == pItem;
                   });
  return it != m_ItemArray.end() ? it - m_ItemArray.begin() : -1;
}

CFWL_ListItem* CFWL_ListBox::AddString(WideStringView wsAdd) {
  m_ItemArray.emplace_back(
      pdfium::MakeUnique<CFWL_ListItem>(WideString(wsAdd)));
  return m_ItemArray.back().get();
}

void CFWL_ListBox::RemoveAt(int32_t iIndex) {
  if (iIndex < 0 || static_cast<size_t>(iIndex) >= m_ItemArray.size())
    return;
  m_ItemArray.erase(m_ItemArray.begin() + iIndex);
}

void CFWL_ListBox::DeleteString(CFWL_ListItem* pItem) {
  int32_t nIndex = GetItemIndex(this, pItem);
  if (nIndex < 0 || static_cast<size_t>(nIndex) >= m_ItemArray.size())
    return;

  int32_t iSel = nIndex + 1;
  if (iSel >= CountItems(this))
    iSel = nIndex - 1;
  if (iSel >= 0) {
    if (CFWL_ListItem* item = GetItem(this, iSel))
      item->SetStates(item->GetStates() | FWL_ITEMSTATE_LTB_Selected);
  }

  m_ItemArray.erase(m_ItemArray.begin() + nIndex);
}

void CFWL_ListBox::DeleteAll() {
  m_ItemArray.clear();
}
