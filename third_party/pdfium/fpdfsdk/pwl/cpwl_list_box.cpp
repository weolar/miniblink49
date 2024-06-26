// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_list_box.h"

#include <sstream>
#include <utility>

#include "core/fxge/cfx_renderdevice.h"
#include "fpdfsdk/pwl/cpwl_edit.h"
#include "fpdfsdk/pwl/cpwl_edit_ctrl.h"
#include "fpdfsdk/pwl/cpwl_edit_impl.h"
#include "fpdfsdk/pwl/cpwl_list_impl.h"
#include "fpdfsdk/pwl/cpwl_scroll_bar.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"
#include "public/fpdf_fwlevent.h"
#include "third_party/base/ptr_util.h"

CPWL_List_Notify::CPWL_List_Notify(CPWL_ListBox* pList) : m_pList(pList) {
  ASSERT(m_pList);
}

CPWL_List_Notify::~CPWL_List_Notify() {}

void CPWL_List_Notify::IOnSetScrollInfoY(float fPlateMin,
                                         float fPlateMax,
                                         float fContentMin,
                                         float fContentMax,
                                         float fSmallStep,
                                         float fBigStep) {
  PWL_SCROLL_INFO Info;
  Info.fPlateWidth = fPlateMax - fPlateMin;
  Info.fContentMin = fContentMin;
  Info.fContentMax = fContentMax;
  Info.fSmallStep = fSmallStep;
  Info.fBigStep = fBigStep;
  m_pList->SetScrollInfo(Info);

  CPWL_ScrollBar* pScroll = m_pList->GetVScrollBar();
  if (!pScroll)
    return;

  if (IsFloatBigger(Info.fPlateWidth, Info.fContentMax - Info.fContentMin) ||
      IsFloatEqual(Info.fPlateWidth, Info.fContentMax - Info.fContentMin)) {
    if (pScroll->IsVisible()) {
      pScroll->SetVisible(false);
      m_pList->RePosChildWnd();
    }
  } else {
    if (!pScroll->IsVisible()) {
      pScroll->SetVisible(true);
      m_pList->RePosChildWnd();
    }
  }
}

void CPWL_List_Notify::IOnSetScrollPosY(float fy) {
  m_pList->SetScrollPosition(fy);
}

void CPWL_List_Notify::IOnInvalidateRect(CFX_FloatRect* pRect) {
  m_pList->InvalidateRect(pRect);
}

CPWL_ListBox::CPWL_ListBox(const CreateParams& cp,
                           std::unique_ptr<PrivateData> pAttachedData)
    : CPWL_Wnd(cp, std::move(pAttachedData)),
      m_pList(pdfium::MakeUnique<CPWL_ListCtrl>()) {}

CPWL_ListBox::~CPWL_ListBox() = default;

void CPWL_ListBox::OnCreated() {
  m_pList->SetFontMap(GetFontMap());
  m_pListNotify = pdfium::MakeUnique<CPWL_List_Notify>(this);
  m_pList->SetNotify(m_pListNotify.get());

  SetHoverSel(HasFlag(PLBS_HOVERSEL));
  m_pList->SetMultipleSel(HasFlag(PLBS_MULTIPLESEL));
  m_pList->SetFontSize(GetCreationParams()->fFontSize);

  m_bHoverSel = HasFlag(PLBS_HOVERSEL);
}

void CPWL_ListBox::OnDestroy() {
  // Make sure the notifier is removed from the list as we are about to
  // destroy the notifier and don't want to leave a dangling pointer.
  m_pList->SetNotify(nullptr);
  m_pListNotify.reset();
}

void CPWL_ListBox::DrawThisAppearance(CFX_RenderDevice* pDevice,
                                      const CFX_Matrix& mtUser2Device) {
  CPWL_Wnd::DrawThisAppearance(pDevice, mtUser2Device);

  CFX_FloatRect rcPlate = m_pList->GetPlateRect();
  CFX_FloatRect rcList = GetListRect();
  CFX_FloatRect rcClient = GetClientRect();

  for (int32_t i = 0, sz = m_pList->GetCount(); i < sz; i++) {
    CFX_FloatRect rcItem = m_pList->GetItemRect(i);
    if (rcItem.bottom > rcPlate.top || rcItem.top < rcPlate.bottom)
      continue;

    CFX_PointF ptOffset(rcItem.left, (rcItem.top + rcItem.bottom) * 0.5f);
    if (CPWL_EditImpl* pEdit = m_pList->GetItemEdit(i)) {
      CFX_FloatRect rcContent = pEdit->GetContentRect();
      if (rcContent.Width() > rcClient.Width())
        rcItem.Intersect(rcList);
      else
        rcItem.Intersect(rcClient);
    }

    if (m_pList->IsItemSelected(i)) {
      CFX_SystemHandler* pSysHandler = GetSystemHandler();
      if (pSysHandler && pSysHandler->IsSelectionImplemented()) {
        CPWL_EditImpl::DrawEdit(pDevice, mtUser2Device, m_pList->GetItemEdit(i),
                                GetTextColor().ToFXColor(255), rcList, ptOffset,
                                nullptr, pSysHandler, m_pFormFiller.Get());
        pSysHandler->OutputSelectedRect(m_pFormFiller.Get(), rcItem);
      } else {
        pDevice->DrawFillRect(&mtUser2Device, rcItem,
                              ArgbEncode(255, 0, 51, 113));
        CPWL_EditImpl::DrawEdit(pDevice, mtUser2Device, m_pList->GetItemEdit(i),
                                ArgbEncode(255, 255, 255, 255), rcList,
                                ptOffset, nullptr, pSysHandler,
                                m_pFormFiller.Get());
      }
    } else {
      CFX_SystemHandler* pSysHandler = GetSystemHandler();
      CPWL_EditImpl::DrawEdit(pDevice, mtUser2Device, m_pList->GetItemEdit(i),
                              GetTextColor().ToFXColor(255), rcList, ptOffset,
                              nullptr, pSysHandler, nullptr);
    }
  }
}

bool CPWL_ListBox::OnKeyDown(uint16_t nChar, uint32_t nFlag) {
  CPWL_Wnd::OnKeyDown(nChar, nFlag);

  switch (nChar) {
    default:
      return false;
    case FWL_VKEY_Up:
    case FWL_VKEY_Down:
    case FWL_VKEY_Home:
    case FWL_VKEY_Left:
    case FWL_VKEY_End:
    case FWL_VKEY_Right:
      break;
  }

  switch (nChar) {
    case FWL_VKEY_Up:
      m_pList->OnVK_UP(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      break;
    case FWL_VKEY_Down:
      m_pList->OnVK_DOWN(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      break;
    case FWL_VKEY_Home:
      m_pList->OnVK_HOME(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      break;
    case FWL_VKEY_Left:
      m_pList->OnVK_LEFT(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      break;
    case FWL_VKEY_End:
      m_pList->OnVK_END(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      break;
    case FWL_VKEY_Right:
      m_pList->OnVK_RIGHT(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      break;
    case FWL_VKEY_Delete:
      break;
  }
  OnNotifySelectionChanged(true, nFlag);
  return true;
}

bool CPWL_ListBox::OnChar(uint16_t nChar, uint32_t nFlag) {
  CPWL_Wnd::OnChar(nChar, nFlag);

  if (!m_pList->OnChar(nChar, IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag)))
    return false;

  OnNotifySelectionChanged(true, nFlag);
  return true;
}

bool CPWL_ListBox::OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnLButtonDown(point, nFlag);

  if (ClientHitTest(point)) {
    m_bMouseDown = true;
    SetFocus();
    SetCapture();

    m_pList->OnMouseDown(point, IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
  }

  return true;
}

bool CPWL_ListBox::OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnLButtonUp(point, nFlag);

  if (m_bMouseDown) {
    ReleaseCapture();
    m_bMouseDown = false;
  }
  OnNotifySelectionChanged(false, nFlag);
  return true;
}

void CPWL_ListBox::SetHoverSel(bool bHoverSel) {
  m_bHoverSel = bHoverSel;
}

bool CPWL_ListBox::OnMouseMove(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnMouseMove(point, nFlag);

  if (m_bHoverSel && !IsCaptureMouse() && ClientHitTest(point))
    m_pList->Select(m_pList->GetItemIndex(point));
  if (m_bMouseDown)
    m_pList->OnMouseMove(point, IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));

  return true;
}

void CPWL_ListBox::SetScrollInfo(const PWL_SCROLL_INFO& info) {
  if (CPWL_Wnd* pChild = GetVScrollBar())
    pChild->SetScrollInfo(info);
}

void CPWL_ListBox::SetScrollPosition(float pos) {
  if (CPWL_Wnd* pChild = GetVScrollBar())
    pChild->SetScrollPosition(pos);
}

void CPWL_ListBox::ScrollWindowVertically(float pos) {
  m_pList->SetScrollPos(CFX_PointF(0, pos));
}

bool CPWL_ListBox::RePosChildWnd() {
  if (!CPWL_Wnd::RePosChildWnd())
    return false;

  m_pList->SetPlateRect(GetListRect());
  return true;
}

bool CPWL_ListBox::OnNotifySelectionChanged(bool bKeyDown, uint32_t nFlag) {
  if (!m_pFillerNotify)
    return false;

  CPWL_Wnd::ObservedPtr thisObserved(this);

  WideString swChange = GetText();
  WideString strChangeEx;
  int nSelStart = 0;
  int nSelEnd = swChange.GetLength();
  bool bRC;
  bool bExit;
  std::tie(bRC, bExit) = m_pFillerNotify->OnBeforeKeyStroke(
      GetAttachedData(), swChange, strChangeEx, nSelStart, nSelEnd, bKeyDown,
      nFlag);

  if (!thisObserved)
    return false;

  return bExit;
}

CFX_FloatRect CPWL_ListBox::GetFocusRect() const {
  if (m_pList->IsMultipleSel()) {
    CFX_FloatRect rcCaret = m_pList->GetItemRect(m_pList->GetCaret());
    rcCaret.Intersect(GetClientRect());
    return rcCaret;
  }

  return CPWL_Wnd::GetFocusRect();
}

void CPWL_ListBox::AddString(const WideString& str) {
  m_pList->AddString(str);
}

WideString CPWL_ListBox::GetText() {
  return m_pList->GetText();
}

void CPWL_ListBox::SetFontSize(float fFontSize) {
  m_pList->SetFontSize(fFontSize);
}

float CPWL_ListBox::GetFontSize() const {
  return m_pList->GetFontSize();
}

void CPWL_ListBox::Select(int32_t nItemIndex) {
  m_pList->Select(nItemIndex);
}

void CPWL_ListBox::SetCaret(int32_t nItemIndex) {
  m_pList->SetCaret(nItemIndex);
}

void CPWL_ListBox::SetTopVisibleIndex(int32_t nItemIndex) {
  m_pList->SetTopItem(nItemIndex);
}

void CPWL_ListBox::ScrollToListItem(int32_t nItemIndex) {
  m_pList->ScrollToListItem(nItemIndex);
}

void CPWL_ListBox::ResetContent() {
  m_pList->Empty();
}

void CPWL_ListBox::Reset() {
  m_pList->Cancel();
}

bool CPWL_ListBox::IsMultipleSel() const {
  return m_pList->IsMultipleSel();
}

int32_t CPWL_ListBox::GetCaretIndex() const {
  return m_pList->GetCaret();
}

int32_t CPWL_ListBox::GetCurSel() const {
  return m_pList->GetSelect();
}

bool CPWL_ListBox::IsItemSelected(int32_t nItemIndex) const {
  return m_pList->IsItemSelected(nItemIndex);
}

int32_t CPWL_ListBox::GetTopVisibleIndex() const {
  m_pList->ScrollToListItem(m_pList->GetFirstSelected());
  return m_pList->GetTopItem();
}

int32_t CPWL_ListBox::GetCount() const {
  return m_pList->GetCount();
}

int32_t CPWL_ListBox::FindNext(int32_t nIndex, wchar_t nChar) const {
  return m_pList->FindNext(nIndex, nChar);
}

CFX_FloatRect CPWL_ListBox::GetContentRect() const {
  return m_pList->GetContentRect();
}

float CPWL_ListBox::GetFirstHeight() const {
  return m_pList->GetFirstHeight();
}

CFX_FloatRect CPWL_ListBox::GetListRect() const {
  float width = static_cast<float>(GetBorderWidth() + GetInnerBorderWidth());
  return GetWindowRect().GetDeflated(width, width);
}

bool CPWL_ListBox::OnMouseWheel(short zDelta,
                                const CFX_PointF& point,
                                uint32_t nFlag) {
  if (zDelta < 0)
    m_pList->OnVK_DOWN(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
  else
    m_pList->OnVK_UP(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));

  OnNotifySelectionChanged(false, nFlag);
  return true;
}
