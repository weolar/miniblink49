// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_edit_ctrl.h"

#include <utility>

#include "core/fpdfdoc/cpvt_word.h"
#include "core/fxge/fx_font.h"
#include "fpdfsdk/pwl/cpwl_caret.h"
#include "fpdfsdk/pwl/cpwl_edit_impl.h"
#include "fpdfsdk/pwl/cpwl_scroll_bar.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"
#include "public/fpdf_fwlevent.h"
#include "third_party/base/ptr_util.h"

CPWL_EditCtrl::CPWL_EditCtrl(const CreateParams& cp,
                             std::unique_ptr<PrivateData> pAttachedData)
    : CPWL_Wnd(cp, std::move(pAttachedData)),
      m_pEdit(pdfium::MakeUnique<CPWL_EditImpl>()) {
  GetCreationParams()->eCursorType = FXCT_VBEAM;
}

CPWL_EditCtrl::~CPWL_EditCtrl() = default;


void CPWL_EditCtrl::OnCreated() {
  SetFontSize(GetCreationParams()->fFontSize);
  m_pEdit->SetFontMap(GetFontMap());
  m_pEdit->SetNotify(this);
  m_pEdit->Initialize();
}

bool CPWL_EditCtrl::IsWndHorV() {
  CFX_Matrix mt = GetWindowMatrix();
  return mt.Transform(CFX_PointF(1, 1)).y == mt.Transform(CFX_PointF(0, 1)).y;
}

void CPWL_EditCtrl::SetCursor() {
  if (IsValid()) {
    if (CFX_SystemHandler* pSH = GetSystemHandler()) {
      if (IsWndHorV())
        pSH->SetCursor(FXCT_VBEAM);
      else
        pSH->SetCursor(FXCT_HBEAM);
    }
  }
}

WideString CPWL_EditCtrl::GetSelectedText() {
  return m_pEdit->GetSelectedText();
}

void CPWL_EditCtrl::ReplaceSelection(const WideString& text) {
  m_pEdit->ClearSelection();
  m_pEdit->InsertText(text, FX_CHARSET_Default);
}

bool CPWL_EditCtrl::RePosChildWnd() {
  m_pEdit->SetPlateRect(GetClientRect());
  return true;
}

void CPWL_EditCtrl::SetScrollInfo(const PWL_SCROLL_INFO& info) {
  if (CPWL_Wnd* pChild = GetVScrollBar())
    pChild->SetScrollInfo(info);
}

void CPWL_EditCtrl::SetScrollPosition(float pos) {
  if (CPWL_Wnd* pChild = GetVScrollBar())
    pChild->SetScrollPosition(pos);
}

void CPWL_EditCtrl::ScrollWindowVertically(float pos) {
  m_pEdit->SetScrollPos(CFX_PointF(m_pEdit->GetScrollPos().x, pos));
}

void CPWL_EditCtrl::CreateChildWnd(const CreateParams& cp) {
  if (!IsReadOnly())
    CreateEditCaret(cp);
}

void CPWL_EditCtrl::CreateEditCaret(const CreateParams& cp) {
  if (m_pEditCaret)
    return;

  CreateParams ecp = cp;
  ecp.dwFlags = PWS_CHILD | PWS_NOREFRESHCLIP;
  ecp.dwBorderWidth = 0;
  ecp.nBorderStyle = BorderStyle::SOLID;
  ecp.rcRectWnd = CFX_FloatRect();

  auto pCaret = pdfium::MakeUnique<CPWL_Caret>(ecp, CloneAttachedData());
  m_pEditCaret = pCaret.get();
  m_pEditCaret->SetInvalidRect(GetClientRect());
  AddChild(std::move(pCaret));
  m_pEditCaret->Realize();
}

void CPWL_EditCtrl::SetFontSize(float fFontSize) {
  m_pEdit->SetFontSize(fFontSize);
}

float CPWL_EditCtrl::GetFontSize() const {
  return m_pEdit->GetFontSize();
}

bool CPWL_EditCtrl::OnKeyDown(uint16_t nChar, uint32_t nFlag) {
  if (m_bMouseDown)
    return true;

  bool bRet = CPWL_Wnd::OnKeyDown(nChar, nFlag);

  // FILTER
  switch (nChar) {
    default:
      return false;
    case FWL_VKEY_Delete:
    case FWL_VKEY_Up:
    case FWL_VKEY_Down:
    case FWL_VKEY_Left:
    case FWL_VKEY_Right:
    case FWL_VKEY_Home:
    case FWL_VKEY_End:
    case FWL_VKEY_Insert:
    case 'C':
    case 'V':
    case 'X':
    case 'A':
    case 'Z':
    case 'c':
    case 'v':
    case 'x':
    case 'a':
    case 'z':
      break;
  }

  if (nChar == FWL_VKEY_Delete && m_pEdit->IsSelected())
    nChar = FWL_VKEY_Unknown;

  switch (nChar) {
    case FWL_VKEY_Delete:
      Delete();
      return true;
    case FWL_VKEY_Insert:
      if (IsSHIFTpressed(nFlag))
        PasteText();
      return true;
    case FWL_VKEY_Up:
      m_pEdit->OnVK_UP(IsSHIFTpressed(nFlag), false);
      return true;
    case FWL_VKEY_Down:
      m_pEdit->OnVK_DOWN(IsSHIFTpressed(nFlag), false);
      return true;
    case FWL_VKEY_Left:
      m_pEdit->OnVK_LEFT(IsSHIFTpressed(nFlag), false);
      return true;
    case FWL_VKEY_Right:
      m_pEdit->OnVK_RIGHT(IsSHIFTpressed(nFlag), false);
      return true;
    case FWL_VKEY_Home:
      m_pEdit->OnVK_HOME(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      return true;
    case FWL_VKEY_End:
      m_pEdit->OnVK_END(IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
      return true;
    case FWL_VKEY_Unknown:
      if (!IsSHIFTpressed(nFlag))
        ClearSelection();
      else
        CutText();
      return true;
    default:
      break;
  }

  return bRet;
}

bool CPWL_EditCtrl::OnChar(uint16_t nChar, uint32_t nFlag) {
  if (m_bMouseDown)
    return true;

  CPWL_Wnd::OnChar(nChar, nFlag);

  // FILTER
  switch (nChar) {
    case 0x0A:
    case 0x1B:
      return false;
    default:
      break;
  }

  bool bCtrl = IsCTRLpressed(nFlag);
  bool bAlt = IsALTpressed(nFlag);
  bool bShift = IsSHIFTpressed(nFlag);

  uint16_t word = nChar;

  if (bCtrl && !bAlt) {
    switch (nChar) {
      case 'C' - 'A' + 1:
        CopyText();
        return true;
      case 'V' - 'A' + 1:
        PasteText();
        return true;
      case 'X' - 'A' + 1:
        CutText();
        return true;
      case 'A' - 'A' + 1:
        SelectAll();
        return true;
      case 'Z' - 'A' + 1:
        if (bShift)
          Redo();
        else
          Undo();
        return true;
      default:
        if (nChar < 32)
          return false;
    }
  }

  if (IsReadOnly())
    return true;

  if (m_pEdit->IsSelected() && word == FWL_VKEY_Back)
    word = FWL_VKEY_Unknown;

  ClearSelection();

  switch (word) {
    case FWL_VKEY_Back:
      Backspace();
      break;
    case FWL_VKEY_Return:
      InsertReturn();
      break;
    case FWL_VKEY_Unknown:
      break;
    default:
      InsertWord(word, GetCharSet());
      break;
  }

  return true;
}

bool CPWL_EditCtrl::OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnLButtonDown(point, nFlag);

  if (ClientHitTest(point)) {
    if (m_bMouseDown && !InvalidateRect(nullptr))
      return true;

    m_bMouseDown = true;
    SetCapture();

    m_pEdit->OnMouseDown(point, IsSHIFTpressed(nFlag), IsCTRLpressed(nFlag));
  }

  return true;
}

bool CPWL_EditCtrl::OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnLButtonUp(point, nFlag);

  if (m_bMouseDown) {
    // can receive keybord message
    if (ClientHitTest(point) && !IsFocused())
      SetFocus();

    ReleaseCapture();
    m_bMouseDown = false;
  }

  return true;
}

bool CPWL_EditCtrl::OnMouseMove(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnMouseMove(point, nFlag);

  if (m_bMouseDown)
    m_pEdit->OnMouseMove(point, false, false);

  return true;
}

void CPWL_EditCtrl::SetEditCaret(bool bVisible) {
  CFX_PointF ptHead;
  CFX_PointF ptFoot;
  if (bVisible)
    GetCaretInfo(&ptHead, &ptFoot);

  SetCaret(bVisible, ptHead, ptFoot);
  // Note, |this| may no longer be viable at this point. If more work needs to
  // be done, check the return value of SetCaret().
}

void CPWL_EditCtrl::GetCaretInfo(CFX_PointF* ptHead, CFX_PointF* ptFoot) const {
  CPWL_EditImpl_Iterator* pIterator = m_pEdit->GetIterator();
  pIterator->SetAt(m_pEdit->GetCaret());
  CPVT_Word word;
  CPVT_Line line;
  if (pIterator->GetWord(word)) {
    ptHead->x = word.ptWord.x + word.fWidth;
    ptHead->y = word.ptWord.y + word.fAscent;
    ptFoot->x = word.ptWord.x + word.fWidth;
    ptFoot->y = word.ptWord.y + word.fDescent;
  } else if (pIterator->GetLine(line)) {
    ptHead->x = line.ptLine.x;
    ptHead->y = line.ptLine.y + line.fLineAscent;
    ptFoot->x = line.ptLine.x;
    ptFoot->y = line.ptLine.y + line.fLineDescent;
  }
}

bool CPWL_EditCtrl::SetCaret(bool bVisible,
                             const CFX_PointF& ptHead,
                             const CFX_PointF& ptFoot) {
  if (!m_pEditCaret)
    return true;

  if (!IsFocused() || m_pEdit->IsSelected())
    bVisible = false;

  ObservedPtr thisObserved(this);
  m_pEditCaret->SetCaret(bVisible, ptHead, ptFoot);
  if (!thisObserved)
    return false;

  return true;
}

WideString CPWL_EditCtrl::GetText() {
  return m_pEdit->GetText();
}

void CPWL_EditCtrl::SetSelection(int32_t nStartChar, int32_t nEndChar) {
  m_pEdit->SetSelection(nStartChar, nEndChar);
}

void CPWL_EditCtrl::GetSelection(int32_t& nStartChar, int32_t& nEndChar) const {
  m_pEdit->GetSelection(nStartChar, nEndChar);
}

void CPWL_EditCtrl::ClearSelection() {
  if (!IsReadOnly())
    m_pEdit->ClearSelection();
}

void CPWL_EditCtrl::SelectAll() {
  m_pEdit->SelectAll();
}

void CPWL_EditCtrl::SetScrollPos(const CFX_PointF& point) {
  m_pEdit->SetScrollPos(point);
}

CFX_PointF CPWL_EditCtrl::GetScrollPos() const {
  return m_pEdit->GetScrollPos();
}

void CPWL_EditCtrl::CopyText() {}

void CPWL_EditCtrl::PasteText() {}

void CPWL_EditCtrl::CutText() {}

void CPWL_EditCtrl::InsertWord(uint16_t word, int32_t nCharset) {
  if (!IsReadOnly())
    m_pEdit->InsertWord(word, nCharset);
}

void CPWL_EditCtrl::InsertReturn() {
  if (!IsReadOnly())
    m_pEdit->InsertReturn();
}

void CPWL_EditCtrl::Delete() {
  if (!IsReadOnly())
    m_pEdit->Delete();
}

void CPWL_EditCtrl::Backspace() {
  if (!IsReadOnly())
    m_pEdit->Backspace();
}

bool CPWL_EditCtrl::CanUndo() {
  return !IsReadOnly() && m_pEdit->CanUndo();
}

bool CPWL_EditCtrl::CanRedo() {
  return !IsReadOnly() && m_pEdit->CanRedo();
}

bool CPWL_EditCtrl::Undo() {
  return CanUndo() && m_pEdit->Undo();
}

bool CPWL_EditCtrl::Redo() {
  return CanRedo() && m_pEdit->Redo();
}

int32_t CPWL_EditCtrl::GetCharSet() const {
  return m_nCharSet < 0 ? FX_CHARSET_Default : m_nCharSet;
}

void CPWL_EditCtrl::SetReadyToInput() {
  if (m_bMouseDown) {
    ReleaseCapture();
    m_bMouseDown = false;
  }
}
