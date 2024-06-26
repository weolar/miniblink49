// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_edit_impl.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/page/cpdf_pageobjectholder.h"
#include "core/fpdfapi/page/cpdf_pathobject.h"
#include "core/fpdfapi/page/cpdf_textobject.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfapi/render/cpdf_textrenderer.h"
#include "core/fpdfdoc/cpvt_word.h"
#include "core/fpdfdoc/ipvt_fontmap.h"
#include "core/fxcrt/autorestorer.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "fpdfsdk/cfx_systemhandler.h"
#include "fpdfsdk/pwl/cpwl_edit.h"
#include "fpdfsdk/pwl/cpwl_edit_ctrl.h"
#include "fpdfsdk/pwl/cpwl_scroll_bar.h"
#include "third_party/base/compiler_specific.h"
#include "third_party/base/ptr_util.h"

namespace {

const int kEditUndoMaxItems = 10000;

void DrawTextString(CFX_RenderDevice* pDevice,
                    const CFX_PointF& pt,
                    CPDF_Font* pFont,
                    float fFontSize,
                    const CFX_Matrix& mtUser2Device,
                    const ByteString& str,
                    FX_ARGB crTextFill) {
  if (!pFont)
    return;

  CFX_PointF pos = mtUser2Device.Transform(pt);
  CPDF_RenderOptions ro;
  ASSERT(ro.GetOptions().bClearType);
  ro.SetColorMode(CPDF_RenderOptions::kNormal);
  CPDF_TextRenderer::DrawTextString(pDevice, pos.x, pos.y, pFont, fFontSize,
                                    mtUser2Device, str, crTextFill, nullptr,
                                    &ro);
}

}  // namespace

CPWL_EditImpl_Iterator::CPWL_EditImpl_Iterator(
    CPWL_EditImpl* pEdit,
    CPDF_VariableText::Iterator* pVTIterator)
    : m_pEdit(pEdit), m_pVTIterator(pVTIterator) {}

CPWL_EditImpl_Iterator::~CPWL_EditImpl_Iterator() {}

bool CPWL_EditImpl_Iterator::NextWord() {
  return m_pVTIterator->NextWord();
}

bool CPWL_EditImpl_Iterator::PrevWord() {
  return m_pVTIterator->PrevWord();
}

bool CPWL_EditImpl_Iterator::GetWord(CPVT_Word& word) const {
  ASSERT(m_pEdit);

  if (m_pVTIterator->GetWord(word)) {
    word.ptWord = m_pEdit->VTToEdit(word.ptWord);
    return true;
  }
  return false;
}

bool CPWL_EditImpl_Iterator::GetLine(CPVT_Line& line) const {
  ASSERT(m_pEdit);

  if (m_pVTIterator->GetLine(line)) {
    line.ptLine = m_pEdit->VTToEdit(line.ptLine);
    return true;
  }
  return false;
}

void CPWL_EditImpl_Iterator::SetAt(int32_t nWordIndex) {
  m_pVTIterator->SetAt(nWordIndex);
}

void CPWL_EditImpl_Iterator::SetAt(const CPVT_WordPlace& place) {
  m_pVTIterator->SetAt(place);
}

const CPVT_WordPlace& CPWL_EditImpl_Iterator::GetAt() const {
  return m_pVTIterator->GetWordPlace();
}

CPWL_EditImpl_Provider::CPWL_EditImpl_Provider(IPVT_FontMap* pFontMap)
    : CPDF_VariableText::Provider(pFontMap), m_pFontMap(pFontMap) {
  ASSERT(m_pFontMap);
}

CPWL_EditImpl_Provider::~CPWL_EditImpl_Provider() {}

IPVT_FontMap* CPWL_EditImpl_Provider::GetFontMap() const {
  return m_pFontMap;
}

uint32_t CPWL_EditImpl_Provider::GetCharWidth(int32_t nFontIndex,
                                              uint16_t word) {
  if (CPDF_Font* pPDFFont = m_pFontMap->GetPDFFont(nFontIndex)) {
    uint32_t charcode = word;

    if (pPDFFont->IsUnicodeCompatible())
      charcode = pPDFFont->CharCodeFromUnicode(word);
    else
      charcode = m_pFontMap->CharCodeFromUnicode(nFontIndex, word);

    if (charcode != CPDF_Font::kInvalidCharCode)
      return pPDFFont->GetCharWidthF(charcode);
  }

  return 0;
}

int32_t CPWL_EditImpl_Provider::GetTypeAscent(int32_t nFontIndex) {
  if (CPDF_Font* pPDFFont = m_pFontMap->GetPDFFont(nFontIndex))
    return pPDFFont->GetTypeAscent();

  return 0;
}

int32_t CPWL_EditImpl_Provider::GetTypeDescent(int32_t nFontIndex) {
  if (CPDF_Font* pPDFFont = m_pFontMap->GetPDFFont(nFontIndex))
    return pPDFFont->GetTypeDescent();

  return 0;
}

int32_t CPWL_EditImpl_Provider::GetWordFontIndex(uint16_t word,
                                                 int32_t charset,
                                                 int32_t nFontIndex) {
  return m_pFontMap->GetWordFontIndex(word, charset, nFontIndex);
}

int32_t CPWL_EditImpl_Provider::GetDefaultFontIndex() {
  return 0;
}

bool CPWL_EditImpl_Provider::IsLatinWord(uint16_t word) {
  return FX_EDIT_ISLATINWORD(word);
}

CPWL_EditImpl_Refresh::CPWL_EditImpl_Refresh() {}

CPWL_EditImpl_Refresh::~CPWL_EditImpl_Refresh() {}

void CPWL_EditImpl_Refresh::BeginRefresh() {
  m_RefreshRects.clear();
  m_OldLineRects = std::move(m_NewLineRects);
}

void CPWL_EditImpl_Refresh::Push(const CPVT_WordRange& linerange,
                                 const CFX_FloatRect& rect) {
  m_NewLineRects.emplace_back(CPWL_EditImpl_LineRect(linerange, rect));
}

void CPWL_EditImpl_Refresh::NoAnalyse() {
  for (const auto& lineRect : m_OldLineRects)
    Add(lineRect.m_rcLine);

  for (const auto& lineRect : m_NewLineRects)
    Add(lineRect.m_rcLine);
}

std::vector<CFX_FloatRect>* CPWL_EditImpl_Refresh::GetRefreshRects() {
  return &m_RefreshRects;
}

void CPWL_EditImpl_Refresh::EndRefresh() {
  m_RefreshRects.clear();
}

void CPWL_EditImpl_Refresh::Add(const CFX_FloatRect& new_rect) {
  // Check for overlapped area.
  for (const auto& rect : m_RefreshRects) {
    if (rect.Contains(new_rect))
      return;
  }
  m_RefreshRects.emplace_back(CFX_FloatRect(new_rect));
}

CPWL_EditImpl_Undo::CPWL_EditImpl_Undo()
    : m_nCurUndoPos(0), m_bWorking(false) {}

CPWL_EditImpl_Undo::~CPWL_EditImpl_Undo() {}

bool CPWL_EditImpl_Undo::CanUndo() const {
  return m_nCurUndoPos > 0;
}

void CPWL_EditImpl_Undo::Undo() {
  m_bWorking = true;
  if (CanUndo()) {
    m_UndoItemStack[m_nCurUndoPos - 1]->Undo();
    m_nCurUndoPos--;
  }
  m_bWorking = false;
}

bool CPWL_EditImpl_Undo::CanRedo() const {
  return m_nCurUndoPos < m_UndoItemStack.size();
}

void CPWL_EditImpl_Undo::Redo() {
  m_bWorking = true;
  if (CanRedo()) {
    m_UndoItemStack[m_nCurUndoPos]->Redo();
    m_nCurUndoPos++;
  }
  m_bWorking = false;
}

void CPWL_EditImpl_Undo::AddItem(std::unique_ptr<IFX_Edit_UndoItem> pItem) {
  ASSERT(!m_bWorking);
  ASSERT(pItem);
  if (CanRedo())
    RemoveTails();

  if (m_UndoItemStack.size() >= kEditUndoMaxItems)
    RemoveHeads();

  m_UndoItemStack.push_back(std::move(pItem));
  m_nCurUndoPos = m_UndoItemStack.size();
}

void CPWL_EditImpl_Undo::RemoveHeads() {
  ASSERT(m_UndoItemStack.size() > 1);
  m_UndoItemStack.pop_front();
}

void CPWL_EditImpl_Undo::RemoveTails() {
  while (CanRedo())
    m_UndoItemStack.pop_back();
}

CFXEU_InsertWord::CFXEU_InsertWord(CPWL_EditImpl* pEdit,
                                   const CPVT_WordPlace& wpOldPlace,
                                   const CPVT_WordPlace& wpNewPlace,
                                   uint16_t word,
                                   int32_t charset)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_Word(word),
      m_nCharset(charset) {
  ASSERT(m_pEdit);
}

CFXEU_InsertWord::~CFXEU_InsertWord() {}

void CFXEU_InsertWord::Redo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpOld);
  m_pEdit->InsertWord(m_Word, m_nCharset, false, true);
}

void CFXEU_InsertWord::Undo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpNew);
  m_pEdit->Backspace(false, true);
}

CFXEU_InsertReturn::CFXEU_InsertReturn(CPWL_EditImpl* pEdit,
                                       const CPVT_WordPlace& wpOldPlace,
                                       const CPVT_WordPlace& wpNewPlace)
    : m_pEdit(pEdit), m_wpOld(wpOldPlace), m_wpNew(wpNewPlace) {
  ASSERT(m_pEdit);
}

CFXEU_InsertReturn::~CFXEU_InsertReturn() {}

void CFXEU_InsertReturn::Redo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpOld);
  m_pEdit->InsertReturn(false, true);
}

void CFXEU_InsertReturn::Undo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpNew);
  m_pEdit->Backspace(false, true);
}

CFXEU_Backspace::CFXEU_Backspace(CPWL_EditImpl* pEdit,
                                 const CPVT_WordPlace& wpOldPlace,
                                 const CPVT_WordPlace& wpNewPlace,
                                 uint16_t word,
                                 int32_t charset)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_Word(word),
      m_nCharset(charset) {
  ASSERT(m_pEdit);
}

CFXEU_Backspace::~CFXEU_Backspace() {}

void CFXEU_Backspace::Redo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpOld);
  m_pEdit->Backspace(false, true);
}

void CFXEU_Backspace::Undo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpNew);
  if (m_wpNew.nSecIndex != m_wpOld.nSecIndex)
    m_pEdit->InsertReturn(false, true);
  else
    m_pEdit->InsertWord(m_Word, m_nCharset, false, true);
}

CFXEU_Delete::CFXEU_Delete(CPWL_EditImpl* pEdit,
                           const CPVT_WordPlace& wpOldPlace,
                           const CPVT_WordPlace& wpNewPlace,
                           uint16_t word,
                           int32_t charset,
                           bool bSecEnd)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_Word(word),
      m_nCharset(charset),
      m_bSecEnd(bSecEnd) {
  ASSERT(m_pEdit);
}

CFXEU_Delete::~CFXEU_Delete() {}

void CFXEU_Delete::Redo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpOld);
  m_pEdit->Delete(false, true);
}

void CFXEU_Delete::Undo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpNew);
  if (m_bSecEnd)
    m_pEdit->InsertReturn(false, true);
  else
    m_pEdit->InsertWord(m_Word, m_nCharset, false, true);
}

CFXEU_Clear::CFXEU_Clear(CPWL_EditImpl* pEdit,
                         const CPVT_WordRange& wrSel,
                         const WideString& swText)
    : m_pEdit(pEdit), m_wrSel(wrSel), m_swText(swText) {
  ASSERT(m_pEdit);
}

CFXEU_Clear::~CFXEU_Clear() {}

void CFXEU_Clear::Redo() {
  m_pEdit->SelectNone();
  m_pEdit->SetSelection(m_wrSel.BeginPos, m_wrSel.EndPos);
  m_pEdit->Clear(false, true);
}

void CFXEU_Clear::Undo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wrSel.BeginPos);
  m_pEdit->InsertText(m_swText, FX_CHARSET_Default, false, true);
  m_pEdit->SetSelection(m_wrSel.BeginPos, m_wrSel.EndPos);
}

CFXEU_InsertText::CFXEU_InsertText(CPWL_EditImpl* pEdit,
                                   const CPVT_WordPlace& wpOldPlace,
                                   const CPVT_WordPlace& wpNewPlace,
                                   const WideString& swText,
                                   int32_t charset)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_swText(swText),
      m_nCharset(charset) {
  ASSERT(m_pEdit);
}

CFXEU_InsertText::~CFXEU_InsertText() {}

void CFXEU_InsertText::Redo() {
  m_pEdit->SelectNone();
  m_pEdit->SetCaret(m_wpOld);
  m_pEdit->InsertText(m_swText, m_nCharset, false, true);
}

void CFXEU_InsertText::Undo() {
  m_pEdit->SelectNone();
  m_pEdit->SetSelection(m_wpOld, m_wpNew);
  m_pEdit->Clear(false, true);
}

// static
void CPWL_EditImpl::DrawEdit(CFX_RenderDevice* pDevice,
                             const CFX_Matrix& mtUser2Device,
                             CPWL_EditImpl* pEdit,
                             FX_COLORREF crTextFill,
                             const CFX_FloatRect& rcClip,
                             const CFX_PointF& ptOffset,
                             const CPVT_WordRange* pRange,
                             CFX_SystemHandler* pSystemHandler,
                             CFFL_FormFiller* pFFLData) {
  const bool bContinuous =
      pEdit->GetCharArray() == 0 && pEdit->GetCharSpace() <= 0.0f;
  uint16_t SubWord = pEdit->GetPasswordChar();
  float fFontSize = pEdit->GetFontSize();
  CPVT_WordRange wrSelect = pEdit->GetSelectWordRange();
  FX_COLORREF crCurFill = crTextFill;
  FX_COLORREF crOldFill = crCurFill;
  bool bSelect = false;
  const FX_COLORREF crWhite = ArgbEncode(255, 255, 255, 255);
  const FX_COLORREF crSelBK = ArgbEncode(255, 0, 51, 113);

  std::ostringstream sTextBuf;
  int32_t nFontIndex = -1;
  CFX_PointF ptBT;
  CFX_RenderDevice::StateRestorer restorer(pDevice);
  if (!rcClip.IsEmpty())
    pDevice->SetClip_Rect(mtUser2Device.TransformRect(rcClip).ToFxRect());

  CPWL_EditImpl_Iterator* pIterator = pEdit->GetIterator();
  IPVT_FontMap* pFontMap = pEdit->GetFontMap();
  if (!pFontMap)
    return;

  if (pRange)
    pIterator->SetAt(pRange->BeginPos);
  else
    pIterator->SetAt(0);

  CPVT_WordPlace oldplace;
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetAt();
    if (pRange && place > pRange->EndPos)
      break;

    if (!wrSelect.IsEmpty()) {
      bSelect = place > wrSelect.BeginPos && place <= wrSelect.EndPos;
      crCurFill = bSelect ? crWhite : crTextFill;
    }
    if (pSystemHandler && pSystemHandler->IsSelectionImplemented()) {
      crCurFill = crTextFill;
      crOldFill = crCurFill;
    }
    CPVT_Word word;
    if (pIterator->GetWord(word)) {
      if (bSelect) {
        CPVT_Line line;
        pIterator->GetLine(line);

        if (pSystemHandler && pSystemHandler->IsSelectionImplemented()) {
          CFX_FloatRect rc(word.ptWord.x, line.ptLine.y + line.fLineDescent,
                           word.ptWord.x + word.fWidth,
                           line.ptLine.y + line.fLineAscent);
          rc.Intersect(rcClip);
          pSystemHandler->OutputSelectedRect(pFFLData, rc);
        } else {
          CFX_PathData pathSelBK;
          pathSelBK.AppendRect(word.ptWord.x, line.ptLine.y + line.fLineDescent,
                               word.ptWord.x + word.fWidth,
                               line.ptLine.y + line.fLineAscent);

          pDevice->DrawPath(&pathSelBK, &mtUser2Device, nullptr, crSelBK, 0,
                            FXFILL_WINDING);
        }
      }

      if (bContinuous) {
        if (place.LineCmp(oldplace) != 0 || word.nFontIndex != nFontIndex ||
            crOldFill != crCurFill) {
          if (sTextBuf.tellp() > 0) {
            DrawTextString(pDevice,
                           CFX_PointF(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
                           pFontMap->GetPDFFont(nFontIndex), fFontSize,
                           mtUser2Device, ByteString(sTextBuf), crOldFill);

            sTextBuf.str("");
          }
          nFontIndex = word.nFontIndex;
          ptBT = word.ptWord;
          crOldFill = crCurFill;
        }

        sTextBuf << pEdit->GetPDFWordString(word.nFontIndex, word.Word,
                                            SubWord);
      } else {
        DrawTextString(
            pDevice,
            CFX_PointF(word.ptWord.x + ptOffset.x, word.ptWord.y + ptOffset.y),
            pFontMap->GetPDFFont(word.nFontIndex), fFontSize, mtUser2Device,
            pEdit->GetPDFWordString(word.nFontIndex, word.Word, SubWord),
            crCurFill);
      }
      oldplace = place;
    }
  }

  if (sTextBuf.tellp() > 0) {
    DrawTextString(pDevice,
                   CFX_PointF(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
                   pFontMap->GetPDFFont(nFontIndex), fFontSize, mtUser2Device,
                   ByteString(sTextBuf), crOldFill);
  }
}

CPWL_EditImpl::CPWL_EditImpl()
    : m_pVT(pdfium::MakeUnique<CPDF_VariableText>()),
      m_bEnableScroll(false),
      m_nAlignment(0),
      m_bNotifyFlag(false),
      m_bEnableOverflow(false),
      m_bEnableRefresh(true),
      m_bEnableUndo(true) {}

CPWL_EditImpl::~CPWL_EditImpl() {}

void CPWL_EditImpl::Initialize() {
  m_pVT->Initialize();
  SetCaret(m_pVT->GetBeginWordPlace());
  SetCaretOrigin();
}

void CPWL_EditImpl::SetFontMap(IPVT_FontMap* pFontMap) {
  m_pVTProvider = pdfium::MakeUnique<CPWL_EditImpl_Provider>(pFontMap);
  m_pVT->SetProvider(m_pVTProvider.get());
}

void CPWL_EditImpl::SetNotify(CPWL_EditCtrl* pNotify) {
  m_pNotify = pNotify;
}

void CPWL_EditImpl::SetOperationNotify(CPWL_Edit* pOperationNotify) {
  m_pOperationNotify = pOperationNotify;
}

CPWL_EditImpl_Iterator* CPWL_EditImpl::GetIterator() {
  if (!m_pIterator) {
    m_pIterator =
        pdfium::MakeUnique<CPWL_EditImpl_Iterator>(this, m_pVT->GetIterator());
  }
  return m_pIterator.get();
}

IPVT_FontMap* CPWL_EditImpl::GetFontMap() {
  return m_pVTProvider ? m_pVTProvider->GetFontMap() : nullptr;
}

void CPWL_EditImpl::SetPlateRect(const CFX_FloatRect& rect) {
  m_pVT->SetPlateRect(rect);
  m_ptScrollPos = CFX_PointF(rect.left, rect.top);
  Paint();
}

void CPWL_EditImpl::SetAlignmentH(int32_t nFormat, bool bPaint) {
  m_pVT->SetAlignment(nFormat);
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetAlignmentV(int32_t nFormat, bool bPaint) {
  m_nAlignment = nFormat;
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetPasswordChar(uint16_t wSubWord, bool bPaint) {
  m_pVT->SetPasswordChar(wSubWord);
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetLimitChar(int32_t nLimitChar) {
  m_pVT->SetLimitChar(nLimitChar);
  Paint();
}

void CPWL_EditImpl::SetCharArray(int32_t nCharArray) {
  m_pVT->SetCharArray(nCharArray);
  Paint();
}

void CPWL_EditImpl::SetCharSpace(float fCharSpace) {
  m_pVT->SetCharSpace(fCharSpace);
  Paint();
}

void CPWL_EditImpl::SetMultiLine(bool bMultiLine, bool bPaint) {
  m_pVT->SetMultiLine(bMultiLine);
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetAutoReturn(bool bAuto, bool bPaint) {
  m_pVT->SetAutoReturn(bAuto);
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetAutoFontSize(bool bAuto, bool bPaint) {
  m_pVT->SetAutoFontSize(bAuto);
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetFontSize(float fFontSize) {
  m_pVT->SetFontSize(fFontSize);
  Paint();
}

void CPWL_EditImpl::SetAutoScroll(bool bAuto, bool bPaint) {
  m_bEnableScroll = bAuto;
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetTextOverflow(bool bAllowed, bool bPaint) {
  m_bEnableOverflow = bAllowed;
  if (bPaint)
    Paint();
}

void CPWL_EditImpl::SetSelection(int32_t nStartChar, int32_t nEndChar) {
  if (m_pVT->IsValid()) {
    if (nStartChar == 0 && nEndChar < 0) {
      SelectAll();
    } else if (nStartChar < 0) {
      SelectNone();
    } else {
      if (nStartChar < nEndChar) {
        SetSelection(m_pVT->WordIndexToWordPlace(nStartChar),
                     m_pVT->WordIndexToWordPlace(nEndChar));
      } else {
        SetSelection(m_pVT->WordIndexToWordPlace(nEndChar),
                     m_pVT->WordIndexToWordPlace(nStartChar));
      }
    }
  }
}

void CPWL_EditImpl::SetSelection(const CPVT_WordPlace& begin,
                                 const CPVT_WordPlace& end) {
  if (!m_pVT->IsValid())
    return;

  SelectNone();
  m_SelState.Set(begin, end);
  SetCaret(m_SelState.EndPos);
  ScrollToCaret();
  if (!m_SelState.IsEmpty())
    Refresh();
  SetCaretInfo();
}

void CPWL_EditImpl::GetSelection(int32_t& nStartChar, int32_t& nEndChar) const {
  nStartChar = -1;
  nEndChar = -1;
  if (!m_pVT->IsValid())
    return;

  if (m_SelState.IsEmpty()) {
    nStartChar = m_pVT->WordPlaceToWordIndex(m_wpCaret);
    nEndChar = m_pVT->WordPlaceToWordIndex(m_wpCaret);
    return;
  }
  if (m_SelState.BeginPos < m_SelState.EndPos) {
    nStartChar = m_pVT->WordPlaceToWordIndex(m_SelState.BeginPos);
    nEndChar = m_pVT->WordPlaceToWordIndex(m_SelState.EndPos);
    return;
  }
  nStartChar = m_pVT->WordPlaceToWordIndex(m_SelState.EndPos);
  nEndChar = m_pVT->WordPlaceToWordIndex(m_SelState.BeginPos);
}

int32_t CPWL_EditImpl::GetCaret() const {
  if (m_pVT->IsValid())
    return m_pVT->WordPlaceToWordIndex(m_wpCaret);

  return -1;
}

CPVT_WordPlace CPWL_EditImpl::GetCaretWordPlace() const {
  return m_wpCaret;
}

WideString CPWL_EditImpl::GetText() const {
  WideString swRet;
  if (!m_pVT->IsValid())
    return swRet;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(0);

  CPVT_Word wordinfo;
  CPVT_WordPlace oldplace = pIterator->GetWordPlace();
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetWordPlace();
    if (pIterator->GetWord(wordinfo))
      swRet += wordinfo.Word;
    if (oldplace.nSecIndex != place.nSecIndex)
      swRet += L"\r\n";
    oldplace = place;
  }
  return swRet;
}

WideString CPWL_EditImpl::GetRangeText(const CPVT_WordRange& range) const {
  WideString swRet;
  if (!m_pVT->IsValid())
    return swRet;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordRange wrTemp = range;
  m_pVT->UpdateWordPlace(wrTemp.BeginPos);
  m_pVT->UpdateWordPlace(wrTemp.EndPos);
  pIterator->SetAt(wrTemp.BeginPos);

  CPVT_Word wordinfo;
  CPVT_WordPlace oldplace = wrTemp.BeginPos;
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetWordPlace();
    if (place > wrTemp.EndPos)
      break;
    if (pIterator->GetWord(wordinfo))
      swRet += wordinfo.Word;
    if (oldplace.nSecIndex != place.nSecIndex)
      swRet += L"\r\n";
    oldplace = place;
  }
  return swRet;
}

WideString CPWL_EditImpl::GetSelectedText() const {
  return GetRangeText(m_SelState.ConvertToWordRange());
}

int32_t CPWL_EditImpl::GetTotalLines() const {
  int32_t nLines = 1;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(0);
  while (pIterator->NextLine())
    ++nLines;

  return nLines;
}

CPVT_WordRange CPWL_EditImpl::GetSelectWordRange() const {
  return m_SelState.ConvertToWordRange();
}

void CPWL_EditImpl::SetText(const WideString& sText) {
  Empty();
  DoInsertText(CPVT_WordPlace(0, 0, -1), sText, FX_CHARSET_Default);
  Paint();
}

bool CPWL_EditImpl::InsertWord(uint16_t word, int32_t charset) {
  return InsertWord(word, charset, true, true);
}

bool CPWL_EditImpl::InsertReturn() {
  return InsertReturn(true, true);
}

bool CPWL_EditImpl::Backspace() {
  return Backspace(true, true);
}

bool CPWL_EditImpl::Delete() {
  return Delete(true, true);
}

bool CPWL_EditImpl::ClearSelection() {
  return Clear(true, true);
}

bool CPWL_EditImpl::InsertText(const WideString& sText, int32_t charset) {
  return InsertText(sText, charset, true, true);
}

float CPWL_EditImpl::GetFontSize() const {
  return m_pVT->GetFontSize();
}

uint16_t CPWL_EditImpl::GetPasswordChar() const {
  return m_pVT->GetPasswordChar();
}

int32_t CPWL_EditImpl::GetCharArray() const {
  return m_pVT->GetCharArray();
}

CFX_FloatRect CPWL_EditImpl::GetContentRect() const {
  return VTToEdit(m_pVT->GetContentRect());
}

float CPWL_EditImpl::GetCharSpace() const {
  return m_pVT->GetCharSpace();
}

CPVT_WordRange CPWL_EditImpl::GetWholeWordRange() const {
  if (m_pVT->IsValid())
    return CPVT_WordRange(m_pVT->GetBeginWordPlace(), m_pVT->GetEndWordPlace());

  return CPVT_WordRange();
}

CPVT_WordRange CPWL_EditImpl::GetVisibleWordRange() const {
  if (m_bEnableOverflow)
    return GetWholeWordRange();

  if (m_pVT->IsValid()) {
    CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

    CPVT_WordPlace place1 =
        m_pVT->SearchWordPlace(EditToVT(CFX_PointF(rcPlate.left, rcPlate.top)));
    CPVT_WordPlace place2 = m_pVT->SearchWordPlace(
        EditToVT(CFX_PointF(rcPlate.right, rcPlate.bottom)));

    return CPVT_WordRange(place1, place2);
  }

  return CPVT_WordRange();
}

CPVT_WordPlace CPWL_EditImpl::SearchWordPlace(const CFX_PointF& point) const {
  if (m_pVT->IsValid()) {
    return m_pVT->SearchWordPlace(EditToVT(point));
  }

  return CPVT_WordPlace();
}

void CPWL_EditImpl::Paint() {
  if (m_pVT->IsValid()) {
    RearrangeAll();
    ScrollToCaret();
    Refresh();
    SetCaretOrigin();
    SetCaretInfo();
  }
}

void CPWL_EditImpl::RearrangeAll() {
  if (m_pVT->IsValid()) {
    m_pVT->UpdateWordPlace(m_wpCaret);
    m_pVT->RearrangeAll();
    m_pVT->UpdateWordPlace(m_wpCaret);
    SetScrollInfo();
    SetContentChanged();
  }
}

void CPWL_EditImpl::RearrangePart(const CPVT_WordRange& range) {
  if (m_pVT->IsValid()) {
    m_pVT->UpdateWordPlace(m_wpCaret);
    m_pVT->RearrangePart(range);
    m_pVT->UpdateWordPlace(m_wpCaret);
    SetScrollInfo();
    SetContentChanged();
  }
}

void CPWL_EditImpl::SetContentChanged() {
  if (m_pNotify) {
    CFX_FloatRect rcContent = m_pVT->GetContentRect();
    if (rcContent.Width() != m_rcOldContent.Width() ||
        rcContent.Height() != m_rcOldContent.Height()) {
      m_rcOldContent = rcContent;
    }
  }
}

void CPWL_EditImpl::SelectAll() {
  if (!m_pVT->IsValid())
    return;
  m_SelState = CPWL_EditImpl_Select(GetWholeWordRange());
  SetCaret(m_SelState.EndPos);
  ScrollToCaret();
  Refresh();
  SetCaretInfo();
}

void CPWL_EditImpl::SelectNone() {
  if (!m_pVT->IsValid() || m_SelState.IsEmpty())
    return;

  m_SelState.Reset();
  Refresh();
}

bool CPWL_EditImpl::IsSelected() const {
  return !m_SelState.IsEmpty();
}

CFX_PointF CPWL_EditImpl::VTToEdit(const CFX_PointF& point) const {
  CFX_FloatRect rcContent = m_pVT->GetContentRect();
  CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

  float fPadding = 0.0f;

  switch (m_nAlignment) {
    case 0:
      fPadding = 0.0f;
      break;
    case 1:
      fPadding = (rcPlate.Height() - rcContent.Height()) * 0.5f;
      break;
    case 2:
      fPadding = rcPlate.Height() - rcContent.Height();
      break;
  }

  return CFX_PointF(point.x - (m_ptScrollPos.x - rcPlate.left),
                    point.y - (m_ptScrollPos.y + fPadding - rcPlate.top));
}

CFX_PointF CPWL_EditImpl::EditToVT(const CFX_PointF& point) const {
  CFX_FloatRect rcContent = m_pVT->GetContentRect();
  CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

  float fPadding = 0.0f;

  switch (m_nAlignment) {
    case 0:
      fPadding = 0.0f;
      break;
    case 1:
      fPadding = (rcPlate.Height() - rcContent.Height()) * 0.5f;
      break;
    case 2:
      fPadding = rcPlate.Height() - rcContent.Height();
      break;
  }

  return CFX_PointF(point.x + (m_ptScrollPos.x - rcPlate.left),
                    point.y + (m_ptScrollPos.y + fPadding - rcPlate.top));
}

CFX_FloatRect CPWL_EditImpl::VTToEdit(const CFX_FloatRect& rect) const {
  CFX_PointF ptLeftBottom = VTToEdit(CFX_PointF(rect.left, rect.bottom));
  CFX_PointF ptRightTop = VTToEdit(CFX_PointF(rect.right, rect.top));

  return CFX_FloatRect(ptLeftBottom.x, ptLeftBottom.y, ptRightTop.x,
                       ptRightTop.y);
}

void CPWL_EditImpl::SetScrollInfo() {
  if (!m_pNotify)
    return;

  CFX_FloatRect rcPlate = m_pVT->GetPlateRect();
  CFX_FloatRect rcContent = m_pVT->GetContentRect();
  if (m_bNotifyFlag)
    return;

  AutoRestorer<bool> restorer(&m_bNotifyFlag);
  m_bNotifyFlag = true;

  PWL_SCROLL_INFO Info;
  Info.fPlateWidth = rcPlate.top - rcPlate.bottom;
  Info.fContentMin = rcContent.bottom;
  Info.fContentMax = rcContent.top;
  Info.fSmallStep = rcPlate.Height() / 3;
  Info.fBigStep = rcPlate.Height();
  m_pNotify->SetScrollInfo(Info);
}

void CPWL_EditImpl::SetScrollPosX(float fx) {
  if (!m_bEnableScroll)
    return;

  if (m_pVT->IsValid()) {
    if (!IsFloatEqual(m_ptScrollPos.x, fx)) {
      m_ptScrollPos.x = fx;
      Refresh();
    }
  }
}

void CPWL_EditImpl::SetScrollPosY(float fy) {
  if (!m_bEnableScroll)
    return;

  if (m_pVT->IsValid()) {
    if (!IsFloatEqual(m_ptScrollPos.y, fy)) {
      m_ptScrollPos.y = fy;
      Refresh();

      if (m_pNotify) {
        if (!m_bNotifyFlag) {
          AutoRestorer<bool> restorer(&m_bNotifyFlag);
          m_bNotifyFlag = true;
          m_pNotify->SetScrollPosition(fy);
        }
      }
    }
  }
}

void CPWL_EditImpl::SetScrollPos(const CFX_PointF& point) {
  SetScrollPosX(point.x);
  SetScrollPosY(point.y);
  SetScrollLimit();
  SetCaretInfo();
}

CFX_PointF CPWL_EditImpl::GetScrollPos() const {
  return m_ptScrollPos;
}

void CPWL_EditImpl::SetScrollLimit() {
  if (m_pVT->IsValid()) {
    CFX_FloatRect rcContent = m_pVT->GetContentRect();
    CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

    if (rcPlate.Width() > rcContent.Width()) {
      SetScrollPosX(rcPlate.left);
    } else {
      if (IsFloatSmaller(m_ptScrollPos.x, rcContent.left)) {
        SetScrollPosX(rcContent.left);
      } else if (IsFloatBigger(m_ptScrollPos.x,
                               rcContent.right - rcPlate.Width())) {
        SetScrollPosX(rcContent.right - rcPlate.Width());
      }
    }

    if (rcPlate.Height() > rcContent.Height()) {
      SetScrollPosY(rcPlate.top);
    } else {
      if (IsFloatSmaller(m_ptScrollPos.y,
                         rcContent.bottom + rcPlate.Height())) {
        SetScrollPosY(rcContent.bottom + rcPlate.Height());
      } else if (IsFloatBigger(m_ptScrollPos.y, rcContent.top)) {
        SetScrollPosY(rcContent.top);
      }
    }
  }
}

void CPWL_EditImpl::ScrollToCaret() {
  SetScrollLimit();

  if (!m_pVT->IsValid())
    return;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(m_wpCaret);

  CFX_PointF ptHead;
  CFX_PointF ptFoot;
  CPVT_Word word;
  CPVT_Line line;
  if (pIterator->GetWord(word)) {
    ptHead.x = word.ptWord.x + word.fWidth;
    ptHead.y = word.ptWord.y + word.fAscent;
    ptFoot.x = word.ptWord.x + word.fWidth;
    ptFoot.y = word.ptWord.y + word.fDescent;
  } else if (pIterator->GetLine(line)) {
    ptHead.x = line.ptLine.x;
    ptHead.y = line.ptLine.y + line.fLineAscent;
    ptFoot.x = line.ptLine.x;
    ptFoot.y = line.ptLine.y + line.fLineDescent;
  }

  CFX_PointF ptHeadEdit = VTToEdit(ptHead);
  CFX_PointF ptFootEdit = VTToEdit(ptFoot);
  CFX_FloatRect rcPlate = m_pVT->GetPlateRect();
  if (!IsFloatEqual(rcPlate.left, rcPlate.right)) {
    if (IsFloatSmaller(ptHeadEdit.x, rcPlate.left) ||
        IsFloatEqual(ptHeadEdit.x, rcPlate.left)) {
      SetScrollPosX(ptHead.x);
    } else if (IsFloatBigger(ptHeadEdit.x, rcPlate.right)) {
      SetScrollPosX(ptHead.x - rcPlate.Width());
    }
  }

  if (!IsFloatEqual(rcPlate.top, rcPlate.bottom)) {
    if (IsFloatSmaller(ptFootEdit.y, rcPlate.bottom) ||
        IsFloatEqual(ptFootEdit.y, rcPlate.bottom)) {
      if (IsFloatSmaller(ptHeadEdit.y, rcPlate.top)) {
        SetScrollPosY(ptFoot.y + rcPlate.Height());
      }
    } else if (IsFloatBigger(ptHeadEdit.y, rcPlate.top)) {
      if (IsFloatBigger(ptFootEdit.y, rcPlate.bottom)) {
        SetScrollPosY(ptHead.y);
      }
    }
  }
}

void CPWL_EditImpl::Refresh() {
  if (m_bEnableRefresh && m_pVT->IsValid()) {
    m_Refresh.BeginRefresh();
    RefreshPushLineRects(GetVisibleWordRange());

    m_Refresh.NoAnalyse();
    m_ptRefreshScrollPos = m_ptScrollPos;

    if (m_pNotify) {
      if (!m_bNotifyFlag) {
        AutoRestorer<bool> restorer(&m_bNotifyFlag);
        m_bNotifyFlag = true;
        if (std::vector<CFX_FloatRect>* pRects = m_Refresh.GetRefreshRects()) {
          for (auto& rect : *pRects)
            m_pNotify->InvalidateRect(&rect);
        }
      }
    }

    m_Refresh.EndRefresh();
  }
}

void CPWL_EditImpl::RefreshPushLineRects(const CPVT_WordRange& wr) {
  if (!m_pVT->IsValid())
    return;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordPlace wpBegin = wr.BeginPos;
  m_pVT->UpdateWordPlace(wpBegin);
  CPVT_WordPlace wpEnd = wr.EndPos;
  m_pVT->UpdateWordPlace(wpEnd);
  pIterator->SetAt(wpBegin);

  CPVT_Line lineinfo;
  do {
    if (!pIterator->GetLine(lineinfo))
      break;
    if (lineinfo.lineplace.LineCmp(wpEnd) > 0)
      break;

    CFX_FloatRect rcLine(lineinfo.ptLine.x,
                         lineinfo.ptLine.y + lineinfo.fLineDescent,
                         lineinfo.ptLine.x + lineinfo.fLineWidth,
                         lineinfo.ptLine.y + lineinfo.fLineAscent);

    m_Refresh.Push(CPVT_WordRange(lineinfo.lineplace, lineinfo.lineEnd),
                   VTToEdit(rcLine));
  } while (pIterator->NextLine());
}

void CPWL_EditImpl::RefreshWordRange(const CPVT_WordRange& wr) {
  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordRange wrTemp = wr;

  m_pVT->UpdateWordPlace(wrTemp.BeginPos);
  m_pVT->UpdateWordPlace(wrTemp.EndPos);
  pIterator->SetAt(wrTemp.BeginPos);

  CPVT_Word wordinfo;
  CPVT_Line lineinfo;
  CPVT_WordPlace place;

  while (pIterator->NextWord()) {
    place = pIterator->GetWordPlace();
    if (place > wrTemp.EndPos)
      break;

    pIterator->GetWord(wordinfo);
    pIterator->GetLine(lineinfo);
    if (place.LineCmp(wrTemp.BeginPos) == 0 ||
        place.LineCmp(wrTemp.EndPos) == 0) {
      CFX_FloatRect rcWord(wordinfo.ptWord.x,
                           lineinfo.ptLine.y + lineinfo.fLineDescent,
                           wordinfo.ptWord.x + wordinfo.fWidth,
                           lineinfo.ptLine.y + lineinfo.fLineAscent);

      if (m_pNotify) {
        if (!m_bNotifyFlag) {
          AutoRestorer<bool> restorer(&m_bNotifyFlag);
          m_bNotifyFlag = true;
          CFX_FloatRect rcRefresh = VTToEdit(rcWord);
          m_pNotify->InvalidateRect(&rcRefresh);
        }
      }
    } else {
      CFX_FloatRect rcLine(lineinfo.ptLine.x,
                           lineinfo.ptLine.y + lineinfo.fLineDescent,
                           lineinfo.ptLine.x + lineinfo.fLineWidth,
                           lineinfo.ptLine.y + lineinfo.fLineAscent);

      if (m_pNotify) {
        if (!m_bNotifyFlag) {
          AutoRestorer<bool> restorer(&m_bNotifyFlag);
          m_bNotifyFlag = true;
          CFX_FloatRect rcRefresh = VTToEdit(rcLine);
          m_pNotify->InvalidateRect(&rcRefresh);
        }
      }

      pIterator->NextLine();
    }
  }
}

void CPWL_EditImpl::SetCaret(const CPVT_WordPlace& place) {
  m_wpOldCaret = m_wpCaret;
  m_wpCaret = place;
}

void CPWL_EditImpl::SetCaretInfo() {
  if (m_pNotify) {
    if (!m_bNotifyFlag) {
      CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
      pIterator->SetAt(m_wpCaret);

      CFX_PointF ptHead;
      CFX_PointF ptFoot;
      CPVT_Word word;
      CPVT_Line line;
      if (pIterator->GetWord(word)) {
        ptHead.x = word.ptWord.x + word.fWidth;
        ptHead.y = word.ptWord.y + word.fAscent;
        ptFoot.x = word.ptWord.x + word.fWidth;
        ptFoot.y = word.ptWord.y + word.fDescent;
      } else if (pIterator->GetLine(line)) {
        ptHead.x = line.ptLine.x;
        ptHead.y = line.ptLine.y + line.fLineAscent;
        ptFoot.x = line.ptLine.x;
        ptFoot.y = line.ptLine.y + line.fLineDescent;
      }

      AutoRestorer<bool> restorer(&m_bNotifyFlag);
      m_bNotifyFlag = true;
      m_pNotify->SetCaret(m_SelState.IsEmpty(), VTToEdit(ptHead),
                          VTToEdit(ptFoot));
    }
  }
}

void CPWL_EditImpl::OnMouseDown(const CFX_PointF& point,
                                bool bShift,
                                bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  SelectNone();
  SetCaret(m_pVT->SearchWordPlace(EditToVT(point)));
  m_SelState.Set(m_wpCaret, m_wpCaret);
  ScrollToCaret();
  SetCaretOrigin();
  SetCaretInfo();
}

void CPWL_EditImpl::OnMouseMove(const CFX_PointF& point,
                                bool bShift,
                                bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  SetCaret(m_pVT->SearchWordPlace(EditToVT(point)));
  if (m_wpCaret == m_wpOldCaret)
    return;

  m_SelState.SetEndPos(m_wpCaret);
  ScrollToCaret();
  Refresh();
  SetCaretOrigin();
  SetCaretInfo();
}

void CPWL_EditImpl::OnVK_UP(bool bShift, bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  SetCaret(m_pVT->GetUpWordPlace(m_wpCaret, m_ptCaret));
  if (bShift) {
    if (m_SelState.IsEmpty())
      m_SelState.Set(m_wpOldCaret, m_wpCaret);
    else
      m_SelState.SetEndPos(m_wpCaret);

    if (m_wpOldCaret != m_wpCaret) {
      ScrollToCaret();
      Refresh();
      SetCaretInfo();
    }
  } else {
    SelectNone();
    ScrollToCaret();
    SetCaretInfo();
  }
}

void CPWL_EditImpl::OnVK_DOWN(bool bShift, bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  SetCaret(m_pVT->GetDownWordPlace(m_wpCaret, m_ptCaret));
  if (bShift) {
    if (m_SelState.IsEmpty())
      m_SelState.Set(m_wpOldCaret, m_wpCaret);
    else
      m_SelState.SetEndPos(m_wpCaret);

    if (m_wpOldCaret != m_wpCaret) {
      ScrollToCaret();
      Refresh();
      SetCaretInfo();
    }
  } else {
    SelectNone();
    ScrollToCaret();
    SetCaretInfo();
  }
}

void CPWL_EditImpl::OnVK_LEFT(bool bShift, bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  if (bShift) {
    if (m_wpCaret == m_pVT->GetLineBeginPlace(m_wpCaret) &&
        m_wpCaret != m_pVT->GetSectionBeginPlace(m_wpCaret)) {
      SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));
    }
    SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));
    if (m_SelState.IsEmpty())
      m_SelState.Set(m_wpOldCaret, m_wpCaret);
    else
      m_SelState.SetEndPos(m_wpCaret);

    if (m_wpOldCaret != m_wpCaret) {
      ScrollToCaret();
      Refresh();
      SetCaretInfo();
    }
  } else {
    if (!m_SelState.IsEmpty()) {
      if (m_SelState.BeginPos < m_SelState.EndPos)
        SetCaret(m_SelState.BeginPos);
      else
        SetCaret(m_SelState.EndPos);

      SelectNone();
      ScrollToCaret();
      SetCaretInfo();
    } else {
      if (m_wpCaret == m_pVT->GetLineBeginPlace(m_wpCaret) &&
          m_wpCaret != m_pVT->GetSectionBeginPlace(m_wpCaret)) {
        SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));
      }
      SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));
      ScrollToCaret();
      SetCaretOrigin();
      SetCaretInfo();
    }
  }
}

void CPWL_EditImpl::OnVK_RIGHT(bool bShift, bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  if (bShift) {
    SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));
    if (m_wpCaret == m_pVT->GetLineEndPlace(m_wpCaret) &&
        m_wpCaret != m_pVT->GetSectionEndPlace(m_wpCaret))
      SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));

    if (m_SelState.IsEmpty())
      m_SelState.Set(m_wpOldCaret, m_wpCaret);
    else
      m_SelState.SetEndPos(m_wpCaret);

    if (m_wpOldCaret != m_wpCaret) {
      ScrollToCaret();
      Refresh();
      SetCaretInfo();
    }
  } else {
    if (!m_SelState.IsEmpty()) {
      if (m_SelState.BeginPos > m_SelState.EndPos)
        SetCaret(m_SelState.BeginPos);
      else
        SetCaret(m_SelState.EndPos);

      SelectNone();
      ScrollToCaret();
      SetCaretInfo();
    } else {
      SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));
      if (m_wpCaret == m_pVT->GetLineEndPlace(m_wpCaret) &&
          m_wpCaret != m_pVT->GetSectionEndPlace(m_wpCaret)) {
        SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));
      }
      ScrollToCaret();
      SetCaretOrigin();
      SetCaretInfo();
    }
  }
}

void CPWL_EditImpl::OnVK_HOME(bool bShift, bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  if (bShift) {
    if (bCtrl)
      SetCaret(m_pVT->GetBeginWordPlace());
    else
      SetCaret(m_pVT->GetLineBeginPlace(m_wpCaret));

    if (m_SelState.IsEmpty())
      m_SelState.Set(m_wpOldCaret, m_wpCaret);
    else
      m_SelState.SetEndPos(m_wpCaret);

    ScrollToCaret();
    Refresh();
    SetCaretInfo();
  } else {
    if (!m_SelState.IsEmpty()) {
      SetCaret(std::min(m_SelState.BeginPos, m_SelState.EndPos));
      SelectNone();
      ScrollToCaret();
      SetCaretInfo();
    } else {
      if (bCtrl)
        SetCaret(m_pVT->GetBeginWordPlace());
      else
        SetCaret(m_pVT->GetLineBeginPlace(m_wpCaret));

      ScrollToCaret();
      SetCaretOrigin();
      SetCaretInfo();
    }
  }
}

void CPWL_EditImpl::OnVK_END(bool bShift, bool bCtrl) {
  if (!m_pVT->IsValid())
    return;

  if (bShift) {
    if (bCtrl)
      SetCaret(m_pVT->GetEndWordPlace());
    else
      SetCaret(m_pVT->GetLineEndPlace(m_wpCaret));

    if (m_SelState.IsEmpty())
      m_SelState.Set(m_wpOldCaret, m_wpCaret);
    else
      m_SelState.SetEndPos(m_wpCaret);

    ScrollToCaret();
    Refresh();
    SetCaretInfo();
  } else {
    if (!m_SelState.IsEmpty()) {
      SetCaret(std::max(m_SelState.BeginPos, m_SelState.EndPos));
      SelectNone();
      ScrollToCaret();
      SetCaretInfo();
    } else {
      if (bCtrl)
        SetCaret(m_pVT->GetEndWordPlace());
      else
        SetCaret(m_pVT->GetLineEndPlace(m_wpCaret));

      ScrollToCaret();
      SetCaretOrigin();
      SetCaretInfo();
    }
  }
}

bool CPWL_EditImpl::InsertWord(uint16_t word,
                               int32_t charset,
                               bool bAddUndo,
                               bool bPaint) {
  if (IsTextOverflow() || !m_pVT->IsValid())
    return false;

  m_pVT->UpdateWordPlace(m_wpCaret);
  SetCaret(
      m_pVT->InsertWord(m_wpCaret, word, GetCharSetFromUnicode(word, charset)));
  m_SelState.Set(m_wpCaret, m_wpCaret);
  if (m_wpCaret == m_wpOldCaret)
    return false;

  if (bAddUndo && m_bEnableUndo) {
    AddEditUndoItem(pdfium::MakeUnique<CFXEU_InsertWord>(
        this, m_wpOldCaret, m_wpCaret, word, charset));
  }
  if (bPaint)
    PaintInsertText(m_wpOldCaret, m_wpCaret);

  if (m_pOperationNotify)
    m_pOperationNotify->OnInsertWord(m_wpCaret, m_wpOldCaret);

  return true;
}

bool CPWL_EditImpl::InsertReturn(bool bAddUndo, bool bPaint) {
  if (IsTextOverflow() || !m_pVT->IsValid())
    return false;

  m_pVT->UpdateWordPlace(m_wpCaret);
  SetCaret(m_pVT->InsertSection(m_wpCaret));
  m_SelState.Set(m_wpCaret, m_wpCaret);
  if (m_wpCaret == m_wpOldCaret)
    return false;

  if (bAddUndo && m_bEnableUndo) {
    AddEditUndoItem(
        pdfium::MakeUnique<CFXEU_InsertReturn>(this, m_wpOldCaret, m_wpCaret));
  }
  if (bPaint) {
    RearrangePart(CPVT_WordRange(m_wpOldCaret, m_wpCaret));
    ScrollToCaret();
    Refresh();
    SetCaretOrigin();
    SetCaretInfo();
  }
  if (m_pOperationNotify)
    m_pOperationNotify->OnInsertReturn(m_wpCaret, m_wpOldCaret);

  return true;
}

bool CPWL_EditImpl::Backspace(bool bAddUndo, bool bPaint) {
  if (!m_pVT->IsValid() || m_wpCaret == m_pVT->GetBeginWordPlace())
    return false;

  CPVT_Word word;
  if (bAddUndo) {
    CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
    pIterator->SetAt(m_wpCaret);
    pIterator->GetWord(word);
  }
  m_pVT->UpdateWordPlace(m_wpCaret);
  SetCaret(m_pVT->BackSpaceWord(m_wpCaret));
  m_SelState.Set(m_wpCaret, m_wpCaret);
  if (m_wpCaret == m_wpOldCaret)
    return false;

  if (bAddUndo && m_bEnableUndo) {
    AddEditUndoItem(pdfium::MakeUnique<CFXEU_Backspace>(
        this, m_wpOldCaret, m_wpCaret, word.Word, word.nCharset));
  }
  if (bPaint) {
    RearrangePart(CPVT_WordRange(m_wpCaret, m_wpOldCaret));
    ScrollToCaret();
    Refresh();
    SetCaretOrigin();
    SetCaretInfo();
  }
  if (m_pOperationNotify)
    m_pOperationNotify->OnBackSpace(m_wpCaret, m_wpOldCaret);

  return true;
}

bool CPWL_EditImpl::Delete(bool bAddUndo, bool bPaint) {
  if (!m_pVT->IsValid() || m_wpCaret == m_pVT->GetEndWordPlace())
    return false;

  CPVT_Word word;
  if (bAddUndo) {
    CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
    pIterator->SetAt(m_pVT->GetNextWordPlace(m_wpCaret));
    pIterator->GetWord(word);
  }
  m_pVT->UpdateWordPlace(m_wpCaret);
  bool bSecEnd = (m_wpCaret == m_pVT->GetSectionEndPlace(m_wpCaret));
  SetCaret(m_pVT->DeleteWord(m_wpCaret));
  m_SelState.Set(m_wpCaret, m_wpCaret);
  if (bAddUndo && m_bEnableUndo) {
    if (bSecEnd) {
      AddEditUndoItem(pdfium::MakeUnique<CFXEU_Delete>(
          this, m_wpOldCaret, m_wpCaret, word.Word, word.nCharset, bSecEnd));
    } else {
      AddEditUndoItem(pdfium::MakeUnique<CFXEU_Delete>(
          this, m_wpOldCaret, m_wpCaret, word.Word, word.nCharset, bSecEnd));
    }
  }
  if (bPaint) {
    RearrangePart(CPVT_WordRange(m_wpOldCaret, m_wpCaret));
    ScrollToCaret();
    Refresh();
    SetCaretOrigin();
    SetCaretInfo();
  }
  if (m_pOperationNotify)
    m_pOperationNotify->OnDelete(m_wpCaret, m_wpOldCaret);

  return true;
}

bool CPWL_EditImpl::Empty() {
  if (m_pVT->IsValid()) {
    m_pVT->DeleteWords(GetWholeWordRange());
    SetCaret(m_pVT->GetBeginWordPlace());

    return true;
  }

  return false;
}

bool CPWL_EditImpl::Clear(bool bAddUndo, bool bPaint) {
  if (!m_pVT->IsValid() || m_SelState.IsEmpty())
    return false;

  CPVT_WordRange range = m_SelState.ConvertToWordRange();
  if (bAddUndo && m_bEnableUndo) {
    AddEditUndoItem(
        pdfium::MakeUnique<CFXEU_Clear>(this, range, GetSelectedText()));
  }

  SelectNone();
  SetCaret(m_pVT->DeleteWords(range));
  m_SelState.Set(m_wpCaret, m_wpCaret);
  if (bPaint) {
    RearrangePart(range);
    ScrollToCaret();
    Refresh();
    SetCaretOrigin();
    SetCaretInfo();
  }
  if (m_pOperationNotify)
    m_pOperationNotify->OnClear(m_wpCaret, m_wpOldCaret);

  return true;
}

bool CPWL_EditImpl::InsertText(const WideString& sText,
                               int32_t charset,
                               bool bAddUndo,
                               bool bPaint) {
  if (IsTextOverflow())
    return false;

  m_pVT->UpdateWordPlace(m_wpCaret);
  SetCaret(DoInsertText(m_wpCaret, sText, charset));
  m_SelState.Set(m_wpCaret, m_wpCaret);
  if (m_wpCaret == m_wpOldCaret)
    return false;

  if (bAddUndo && m_bEnableUndo) {
    AddEditUndoItem(pdfium::MakeUnique<CFXEU_InsertText>(
        this, m_wpOldCaret, m_wpCaret, sText, charset));
  }
  if (bPaint)
    PaintInsertText(m_wpOldCaret, m_wpCaret);

  if (m_pOperationNotify)
    m_pOperationNotify->OnInsertText(m_wpCaret, m_wpOldCaret);

  return true;
}

void CPWL_EditImpl::PaintInsertText(const CPVT_WordPlace& wpOld,
                                    const CPVT_WordPlace& wpNew) {
  if (m_pVT->IsValid()) {
    RearrangePart(CPVT_WordRange(wpOld, wpNew));
    ScrollToCaret();
    Refresh();
    SetCaretOrigin();
    SetCaretInfo();
  }
}

bool CPWL_EditImpl::Redo() {
  if (m_bEnableUndo) {
    if (m_Undo.CanRedo()) {
      m_Undo.Redo();
      return true;
    }
  }

  return false;
}

bool CPWL_EditImpl::Undo() {
  if (m_bEnableUndo) {
    if (m_Undo.CanUndo()) {
      m_Undo.Undo();
      return true;
    }
  }

  return false;
}

void CPWL_EditImpl::SetCaretOrigin() {
  if (!m_pVT->IsValid())
    return;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(m_wpCaret);
  CPVT_Word word;
  CPVT_Line line;
  if (pIterator->GetWord(word)) {
    m_ptCaret.x = word.ptWord.x + word.fWidth;
    m_ptCaret.y = word.ptWord.y;
  } else if (pIterator->GetLine(line)) {
    m_ptCaret.x = line.ptLine.x;
    m_ptCaret.y = line.ptLine.y;
  }
}

CPVT_WordPlace CPWL_EditImpl::WordIndexToWordPlace(int32_t index) const {
  if (m_pVT->IsValid())
    return m_pVT->WordIndexToWordPlace(index);

  return CPVT_WordPlace();
}

bool CPWL_EditImpl::IsTextFull() const {
  int32_t nTotalWords = m_pVT->GetTotalWords();
  int32_t nLimitChar = m_pVT->GetLimitChar();
  int32_t nCharArray = m_pVT->GetCharArray();

  return IsTextOverflow() || (nLimitChar > 0 && nTotalWords >= nLimitChar) ||
         (nCharArray > 0 && nTotalWords >= nCharArray);
}

bool CPWL_EditImpl::IsTextOverflow() const {
  if (!m_bEnableScroll && !m_bEnableOverflow) {
    CFX_FloatRect rcPlate = m_pVT->GetPlateRect();
    CFX_FloatRect rcContent = m_pVT->GetContentRect();

    if (m_pVT->IsMultiLine() && GetTotalLines() > 1 &&
        IsFloatBigger(rcContent.Height(), rcPlate.Height())) {
      return true;
    }

    if (IsFloatBigger(rcContent.Width(), rcPlate.Width()))
      return true;
  }

  return false;
}

bool CPWL_EditImpl::CanUndo() const {
  if (m_bEnableUndo) {
    return m_Undo.CanUndo();
  }

  return false;
}

bool CPWL_EditImpl::CanRedo() const {
  if (m_bEnableUndo) {
    return m_Undo.CanRedo();
  }

  return false;
}

void CPWL_EditImpl::EnableRefresh(bool bRefresh) {
  m_bEnableRefresh = bRefresh;
}

void CPWL_EditImpl::EnableUndo(bool bUndo) {
  m_bEnableUndo = bUndo;
}

CPVT_WordPlace CPWL_EditImpl::DoInsertText(const CPVT_WordPlace& place,
                                           const WideString& sText,
                                           int32_t charset) {
  CPVT_WordPlace wp = place;

  if (m_pVT->IsValid()) {
    for (int32_t i = 0, sz = sText.GetLength(); i < sz; i++) {
      uint16_t word = sText[i];
      switch (word) {
        case '\r':
          wp = m_pVT->InsertSection(wp);
          if (i + 1 < sz && sText[i + 1] == '\n')
            i++;
          break;
        case '\n':
          wp = m_pVT->InsertSection(wp);
          break;
        case '\t':
          word = ' ';
          FALLTHROUGH;
        default:
          wp =
              m_pVT->InsertWord(wp, word, GetCharSetFromUnicode(word, charset));
          break;
      }
    }
  }

  return wp;
}

int32_t CPWL_EditImpl::GetCharSetFromUnicode(uint16_t word,
                                             int32_t nOldCharset) {
  if (IPVT_FontMap* pFontMap = GetFontMap())
    return pFontMap->CharSetFromUnicode(word, nOldCharset);
  return nOldCharset;
}

void CPWL_EditImpl::AddEditUndoItem(
    std::unique_ptr<IFX_Edit_UndoItem> pEditUndoItem) {
  m_Undo.AddItem(std::move(pEditUndoItem));
}

ByteString CPWL_EditImpl::GetPDFWordString(int32_t nFontIndex,
                                           uint16_t Word,
                                           uint16_t SubWord) {
  IPVT_FontMap* pFontMap = GetFontMap();
  CPDF_Font* pPDFFont = pFontMap->GetPDFFont(nFontIndex);
  if (!pPDFFont)
    return ByteString();

  ByteString sWord;
  if (SubWord > 0) {
    Word = SubWord;
  } else {
    uint32_t dwCharCode = pPDFFont->IsUnicodeCompatible()
                              ? pPDFFont->CharCodeFromUnicode(Word)
                              : pFontMap->CharCodeFromUnicode(nFontIndex, Word);
    if (dwCharCode > 0) {
      pPDFFont->AppendChar(&sWord, dwCharCode);
      return sWord;
    }
  }
  pPDFFont->AppendChar(&sWord, Word);
  return sWord;
}

CPWL_EditImpl_Select::CPWL_EditImpl_Select() {}

CPWL_EditImpl_Select::CPWL_EditImpl_Select(const CPVT_WordRange& range) {
  Set(range.BeginPos, range.EndPos);
}

CPVT_WordRange CPWL_EditImpl_Select::ConvertToWordRange() const {
  return CPVT_WordRange(BeginPos, EndPos);
}

void CPWL_EditImpl_Select::Reset() {
  BeginPos.Reset();
  EndPos.Reset();
}

void CPWL_EditImpl_Select::Set(const CPVT_WordPlace& begin,
                               const CPVT_WordPlace& end) {
  BeginPos = begin;
  EndPos = end;
}

void CPWL_EditImpl_Select::SetEndPos(const CPVT_WordPlace& end) {
  EndPos = end;
}

bool CPWL_EditImpl_Select::IsEmpty() const {
  return BeginPos == EndPos;
}
