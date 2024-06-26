// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_edit.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fde/cfde_texteditengine.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_caret.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_eventtextwillchange.h"
#include "xfa/fwl/cfwl_eventvalidate.h"
#include "xfa/fwl/cfwl_messagekey.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themepart.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fwl/ifwl_themeprovider.h"
#include "xfa/fwl/theme/cfwl_utils.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

namespace {

const int kEditMargin = 3;

#if (_FX_OS_ == _FX_OS_MACOSX_)
constexpr int kEditingModifier = FWL_KEYFLAG_Command;
#else
constexpr int kEditingModifier = FWL_KEYFLAG_Ctrl;
#endif

}  // namespace

CFWL_Edit::CFWL_Edit(const CFWL_App* app,
                     std::unique_ptr<CFWL_WidgetProperties> properties,
                     CFWL_Widget* pOuter)
    : CFWL_Widget(app, std::move(properties), pOuter) {
  m_EdtEngine.SetDelegate(this);
}

CFWL_Edit::~CFWL_Edit() {
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused)
    HideCaret(nullptr);
}

FWL_Type CFWL_Edit::GetClassID() const {
  return FWL_Type::Edit;
}

CFX_RectF CFWL_Edit::GetWidgetRect() {
  CFX_RectF rect = m_pProperties->m_rtWidget;
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
    IFWL_ThemeProvider* theme = GetAvailableTheme();
    float scrollbarWidth = theme ? theme->GetScrollBarWidth() : 0.0f;
    if (IsShowScrollBar(true)) {
      rect.width += scrollbarWidth;
      rect.width += kEditMargin;
    }
    if (IsShowScrollBar(false)) {
      rect.height += scrollbarWidth;
      rect.height += kEditMargin;
    }
  }
  return rect;
}

CFX_RectF CFWL_Edit::GetAutosizedWidgetRect() {
  CFX_RectF rect;

  if (m_EdtEngine.GetLength() > 0) {
    CFX_SizeF size = CalcTextSize(
        m_EdtEngine.GetText(), m_pProperties->m_pThemeProvider.Get(),
        !!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine));
    rect = CFX_RectF(0, 0, size);
  }

  InflateWidgetRect(rect);
  return rect;
}

void CFWL_Edit::SetStates(uint32_t dwStates) {
  if ((m_pProperties->m_dwStates & FWL_WGTSTATE_Invisible) ||
      (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
    HideCaret(nullptr);
  }
  CFWL_Widget::SetStates(dwStates);
}

void CFWL_Edit::Update() {
  if (IsLocked())
    return;
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();

  Layout();
  if (m_rtClient.IsEmpty())
    return;

  UpdateEditEngine();
  UpdateVAlignment();
  UpdateScroll();
  InitCaret();
}

FWL_WidgetHit CFWL_Edit::HitTest(const CFX_PointF& point) {
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
    if (IsShowScrollBar(true)) {
      if (m_pVertScrollBar->GetWidgetRect().Contains(point))
        return FWL_WidgetHit::VScrollBar;
    }
    if (IsShowScrollBar(false)) {
      if (m_pHorzScrollBar->GetWidgetRect().Contains(point))
        return FWL_WidgetHit::HScrollBar;
    }
  }
  if (m_rtClient.Contains(point))
    return FWL_WidgetHit::Edit;
  return FWL_WidgetHit::Unknown;
}

void CFWL_Edit::DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  if (m_rtClient.IsEmpty())
    return;

  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  if (!pTheme)
    return;

  DrawContent(pGraphics, pTheme, &matrix);
  if (HasBorder())
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, matrix);
}

void CFWL_Edit::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  if (!pThemeProvider)
    return;
  if (m_pHorzScrollBar)
    m_pHorzScrollBar->SetThemeProvider(pThemeProvider);
  if (m_pVertScrollBar)
    m_pVertScrollBar->SetThemeProvider(pThemeProvider);
  if (m_pCaret)
    m_pCaret->SetThemeProvider(pThemeProvider);
  m_pProperties->m_pThemeProvider = pThemeProvider;
}

void CFWL_Edit::SetText(const WideString& wsText,
                        CFDE_TextEditEngine::RecordOperation op) {
  m_EdtEngine.Clear();
  m_EdtEngine.Insert(0, wsText, op);
}

int32_t CFWL_Edit::GetTextLength() const {
  return m_EdtEngine.GetLength();
}

WideString CFWL_Edit::GetText() const {
  return m_EdtEngine.GetText();
}

void CFWL_Edit::ClearText() {
  m_EdtEngine.Clear();
}

void CFWL_Edit::SelectAll() {
  m_EdtEngine.SelectAll();
}

bool CFWL_Edit::HasSelection() const {
  return m_EdtEngine.HasSelection();
}

std::pair<size_t, size_t> CFWL_Edit::GetSelection() const {
  return m_EdtEngine.GetSelection();
}

void CFWL_Edit::ClearSelection() {
  return m_EdtEngine.ClearSelection();
}

int32_t CFWL_Edit::GetLimit() const {
  return m_nLimit;
}

void CFWL_Edit::SetLimit(int32_t nLimit) {
  m_nLimit = nLimit;

  if (m_nLimit > 0) {
    m_EdtEngine.SetHasCharacterLimit(true);
    m_EdtEngine.SetCharacterLimit(nLimit);
  } else {
    m_EdtEngine.SetHasCharacterLimit(false);
  }
}

void CFWL_Edit::SetAliasChar(wchar_t wAlias) {
  m_EdtEngine.SetAliasChar(wAlias);
}

Optional<WideString> CFWL_Edit::Copy() {
  if (!m_EdtEngine.HasSelection())
    return {};

  return {m_EdtEngine.GetSelectedText()};
}

Optional<WideString> CFWL_Edit::Cut() {
  if (!m_EdtEngine.HasSelection())
    return {};

  WideString cut_text = m_EdtEngine.DeleteSelectedText();
  UpdateCaret();
  return {cut_text};
}

bool CFWL_Edit::Paste(const WideString& wsPaste) {
  if (m_EdtEngine.HasSelection())
    m_EdtEngine.ReplaceSelectedText(wsPaste);
  else
    m_EdtEngine.Insert(m_CursorPosition, wsPaste);

  return true;
}

bool CFWL_Edit::Undo() {
  return CanUndo() ? m_EdtEngine.Undo() : false;
}

bool CFWL_Edit::Redo() {
  return CanRedo() ? m_EdtEngine.Redo() : false;
}

bool CFWL_Edit::CanUndo() {
  return m_EdtEngine.CanUndo();
}

bool CFWL_Edit::CanRedo() {
  return m_EdtEngine.CanRedo();
}

void CFWL_Edit::SetOuter(CFWL_Widget* pOuter) {
  m_pOuter = pOuter;
}

void CFWL_Edit::NotifyTextFull() {
  CFWL_Event evt(CFWL_Event::Type::TextFull, this);
  DispatchEvent(&evt);
}

void CFWL_Edit::OnCaretChanged() {
  if (m_rtEngine.IsEmpty())
    return;
  if ((m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0)
    return;

  bool bRepaintContent = UpdateOffset();
  UpdateCaret();
  CFX_RectF rtInvalid;
  bool bRepaintScroll = false;
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine) {
    CFWL_ScrollBar* pScroll = UpdateScroll();
    if (pScroll) {
      rtInvalid = pScroll->GetWidgetRect();
      bRepaintScroll = true;
    }
  }
  if (bRepaintContent || bRepaintScroll) {
    if (bRepaintContent)
      rtInvalid.Union(m_rtEngine);
    RepaintRect(rtInvalid);
  }
}

void CFWL_Edit::OnTextWillChange(CFDE_TextEditEngine::TextChange* change) {
  CFWL_EventTextWillChange event(this);
  event.previous_text = change->previous_text;
  event.change_text = change->text;
  event.selection_start = change->selection_start;
  event.selection_end = change->selection_end;
  event.cancelled = false;

  DispatchEvent(&event);

  change->text = event.change_text;
  change->selection_start = event.selection_start;
  change->selection_end = event.selection_end;
  change->cancelled = event.cancelled;
}

void CFWL_Edit::OnTextChanged() {
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VAlignMask)
    UpdateVAlignment();

  LayoutScrollBar();
  RepaintRect(GetClientRect());
}

void CFWL_Edit::OnSelChanged() {
  RepaintRect(GetClientRect());
}

bool CFWL_Edit::OnValidate(const WideString& wsText) {
  CFWL_EventValidate event(this);
  event.wsInsert = wsText;
  event.bValidate = true;
  DispatchEvent(&event);
  return event.bValidate;
}

void CFWL_Edit::SetScrollOffset(float fScrollOffset) {
  m_fScrollOffsetY = fScrollOffset;
}

void CFWL_Edit::DrawTextBk(CXFA_Graphics* pGraphics,
                           IFWL_ThemeProvider* pTheme,
                           const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground param;
  param.m_pWidget = this;
  param.m_iPart = CFWL_Part::Background;
  param.m_bStaticBackground = false;
  param.m_dwStates = m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly
                         ? CFWL_PartState_ReadOnly
                         : CFWL_PartState_Normal;
  uint32_t dwStates = (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled);
  if (dwStates)
    param.m_dwStates = CFWL_PartState_Disabled;
  param.m_pGraphics = pGraphics;
  param.m_matrix = *pMatrix;
  param.m_rtPart = m_rtClient;
  pTheme->DrawBackground(param);

  if (!IsShowScrollBar(true) || !IsShowScrollBar(false))
    return;

  CFX_RectF rtScroll = m_pHorzScrollBar->GetWidgetRect();

  CFX_RectF rtStatic(m_rtClient.right() - rtScroll.height,
                     m_rtClient.bottom() - rtScroll.height, rtScroll.height,
                     rtScroll.height);
  param.m_bStaticBackground = true;
  param.m_bMaximize = true;
  param.m_rtPart = rtStatic;
  pTheme->DrawBackground(param);
}

void CFWL_Edit::DrawContent(CXFA_Graphics* pGraphics,
                            IFWL_ThemeProvider* pTheme,
                            const CFX_Matrix* pMatrix) {
  pGraphics->SaveGraphState();

  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_CombText)
    pGraphics->SaveGraphState();

  CFX_RectF rtClip = m_rtEngine;
  float fOffSetX = m_rtEngine.left - m_fScrollOffsetX;
  float fOffSetY = m_rtEngine.top - m_fScrollOffsetY + m_fVAlignOffset;

  CFX_Matrix mt(1, 0, 0, 1, fOffSetX, fOffSetY);
  if (pMatrix) {
    rtClip = pMatrix->TransformRect(rtClip);
    mt.Concat(*pMatrix);
  }

  bool bShowSel = !!(m_pProperties->m_dwStates & FWL_WGTSTATE_Focused);
  if (bShowSel && m_EdtEngine.HasSelection()) {
    size_t sel_start;
    size_t count;
    std::tie(sel_start, count) = m_EdtEngine.GetSelection();
    std::vector<CFX_RectF> rects =
        m_EdtEngine.GetCharacterRectsInRange(sel_start, count);

    CXFA_GEPath path;
    for (auto& rect : rects) {
      rect.left += fOffSetX;
      rect.top += fOffSetY;
      path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
    }
    pGraphics->SetClipRect(rtClip);

    CFWL_ThemeBackground param;
    param.m_pGraphics = pGraphics;
    param.m_matrix = *pMatrix;
    param.m_pWidget = this;
    param.m_iPart = CFWL_Part::Background;
    param.m_pPath = &path;
    pTheme->DrawBackground(param);
  }

  CFX_RenderDevice* pRenderDev = pGraphics->GetRenderDevice();
  RenderText(pRenderDev, rtClip, mt);

  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_CombText) {
    pGraphics->RestoreGraphState();

    CXFA_GEPath path;
    int32_t iLimit = m_nLimit > 0 ? m_nLimit : 1;
    float fStep = m_rtEngine.width / iLimit;
    float fLeft = m_rtEngine.left + 1;
    for (int32_t i = 1; i < iLimit; i++) {
      fLeft += fStep;
      path.AddLine(CFX_PointF(fLeft, m_rtClient.top),
                   CFX_PointF(fLeft, m_rtClient.bottom()));
    }

    CFWL_ThemeBackground param;
    param.m_pGraphics = pGraphics;
    param.m_matrix = *pMatrix;
    param.m_pWidget = this;
    param.m_iPart = CFWL_Part::CombTextLine;
    param.m_pPath = &path;
    pTheme->DrawBackground(param);
  }
  pGraphics->RestoreGraphState();
}

void CFWL_Edit::RenderText(CFX_RenderDevice* pRenderDev,
                           const CFX_RectF& clipRect,
                           const CFX_Matrix& mt) {
  ASSERT(pRenderDev);

  RetainPtr<CFGAS_GEFont> font = m_EdtEngine.GetFont();
  if (!font)
    return;

  pRenderDev->SetClip_Rect(clipRect);

  CFX_RectF rtDocClip = clipRect;
  if (rtDocClip.IsEmpty()) {
    rtDocClip.left = 0;
    rtDocClip.top = 0;
    rtDocClip.width = static_cast<float>(pRenderDev->GetWidth());
    rtDocClip.height = static_cast<float>(pRenderDev->GetHeight());
  }
  rtDocClip = mt.GetInverse().TransformRect(rtDocClip);

  for (const FDE_TEXTEDITPIECE& info : m_EdtEngine.GetTextPieces()) {
    // If this character is outside the clip, skip it.
    if (!rtDocClip.IntersectWith(info.rtPiece))
      continue;

    std::vector<FXTEXT_CHARPOS> char_pos = m_EdtEngine.GetDisplayPos(info);
    if (char_pos.empty())
      continue;

    CFDE_TextOut::DrawString(pRenderDev, m_EdtEngine.GetFontColor(), font,
                             char_pos, m_EdtEngine.GetFontSize(), &mt);
  }
}

void CFWL_Edit::UpdateEditEngine() {
  UpdateEditParams();
  UpdateEditLayout();
}

void CFWL_Edit::UpdateEditParams() {
  m_EdtEngine.SetAvailableWidth(m_rtEngine.width);
  m_EdtEngine.SetCombText(
      !!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_CombText));

  m_EdtEngine.EnableValidation(
      !!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_Validate));
  m_EdtEngine.EnablePasswordMode(
      !!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_Password));

  uint32_t alignment = 0;
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HAlignMask) {
    case FWL_STYLEEXT_EDT_HNear: {
      alignment |= CFX_TxtLineAlignment_Left;
      break;
    }
    case FWL_STYLEEXT_EDT_HCenter: {
      alignment |= CFX_TxtLineAlignment_Center;
      break;
    }
    case FWL_STYLEEXT_EDT_HFar: {
      alignment |= CFX_TxtLineAlignment_Right;
      break;
    }
    default:
      break;
  }
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HAlignModeMask) {
    case FWL_STYLEEXT_EDT_Justified: {
      alignment |= CFX_TxtLineAlignment_Justified;
      break;
    }
    default:
      break;
  }
  m_EdtEngine.SetAlignment(alignment);

  bool auto_hscroll =
      !!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_AutoHScroll);
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine) {
    m_EdtEngine.EnableMultiLine(true);
    m_EdtEngine.EnableLineWrap(!auto_hscroll);
    m_EdtEngine.LimitVerticalScroll(
        (m_pProperties->m_dwStyles & FWL_WGTSTYLE_VScroll) == 0 &&
        (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_AutoVScroll) == 0);
  } else {
    m_EdtEngine.EnableMultiLine(false);
    m_EdtEngine.EnableLineWrap(false);
    m_EdtEngine.LimitVerticalScroll(false);
  }
  m_EdtEngine.LimitHorizontalScroll(!auto_hscroll);

  IFWL_ThemeProvider* theme = GetAvailableTheme();
  CFWL_ThemePart part;
  part.m_pWidget = this;

  if (!theme) {
    m_fFontSize = FWLTHEME_CAPACITY_FontSize;
    return;
  }
  m_fFontSize = theme->GetFontSize(part);

  RetainPtr<CFGAS_GEFont> pFont = theme->GetFont(part);
  if (!pFont)
    return;

  m_EdtEngine.SetFont(pFont);
  m_EdtEngine.SetFontColor(theme->GetTextColor(part));
  m_EdtEngine.SetFontSize(m_fFontSize);
  m_EdtEngine.SetLineSpace(theme->GetLineHeight(part));
  m_EdtEngine.SetTabWidth(m_fFontSize);
  m_EdtEngine.SetVisibleLineCount(m_rtEngine.height /
                                  theme->GetLineHeight(part));
}

void CFWL_Edit::UpdateEditLayout() {
  m_EdtEngine.Layout();
}

bool CFWL_Edit::UpdateOffset() {
  CFX_RectF rtCaret = m_rtCaret;

  float fOffSetX = m_rtEngine.left - m_fScrollOffsetX;
  float fOffSetY = m_rtEngine.top - m_fScrollOffsetY + m_fVAlignOffset;
  rtCaret.Offset(fOffSetX, fOffSetY);

  const CFX_RectF& edit_bounds = m_rtEngine;
  if (edit_bounds.Contains(rtCaret)) {
    CFX_RectF contents_bounds = m_EdtEngine.GetContentsBoundingBox();
    contents_bounds.Offset(fOffSetX, fOffSetY);
    if (contents_bounds.right() < edit_bounds.right() && m_fScrollOffsetX > 0) {
      m_fScrollOffsetX += contents_bounds.right() - edit_bounds.right();
      m_fScrollOffsetX = std::max(m_fScrollOffsetX, 0.0f);
    }
    if (contents_bounds.bottom() < edit_bounds.bottom() &&
        m_fScrollOffsetY > 0) {
      m_fScrollOffsetY += contents_bounds.bottom() - edit_bounds.bottom();
      m_fScrollOffsetY = std::max(m_fScrollOffsetY, 0.0f);
    }
    return false;
  }

  float offsetX = 0.0;
  float offsetY = 0.0;
  if (rtCaret.left < edit_bounds.left)
    offsetX = rtCaret.left - edit_bounds.left;
  if (rtCaret.right() > edit_bounds.right())
    offsetX = rtCaret.right() - edit_bounds.right();
  if (rtCaret.top < edit_bounds.top)
    offsetY = rtCaret.top - edit_bounds.top;
  if (rtCaret.bottom() > edit_bounds.bottom())
    offsetY = rtCaret.bottom() - edit_bounds.bottom();

  m_fScrollOffsetX += offsetX;
  m_fScrollOffsetY += offsetY;
  if (m_fFontSize > m_rtEngine.height)
    m_fScrollOffsetY = 0;

  return true;
}

bool CFWL_Edit::UpdateOffset(CFWL_ScrollBar* pScrollBar, float fPosChanged) {
  if (pScrollBar == m_pHorzScrollBar.get())
    m_fScrollOffsetX += fPosChanged;
  else
    m_fScrollOffsetY += fPosChanged;
  return true;
}

void CFWL_Edit::UpdateVAlignment() {
  float fSpaceAbove = 0.0f;
  float fSpaceBelow = 0.0f;
  IFWL_ThemeProvider* theme = GetAvailableTheme();
  if (theme) {
    CFWL_ThemePart part;
    part.m_pWidget = this;

    CFX_SizeF pSpace = theme->GetSpaceAboveBelow(part);
    fSpaceAbove = pSpace.width >= 0.1f ? pSpace.width : 0.0f;
    fSpaceBelow = pSpace.height >= 0.1f ? pSpace.height : 0.0f;
  }

  float fOffsetY = 0.0f;
  CFX_RectF contents_bounds = m_EdtEngine.GetContentsBoundingBox();
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VCenter) {
    fOffsetY = (m_rtEngine.height - contents_bounds.height) / 2.0f;
    if (fOffsetY < (fSpaceAbove + fSpaceBelow) / 2.0f &&
        fSpaceAbove < fSpaceBelow) {
      return;
    }
    fOffsetY += (fSpaceAbove - fSpaceBelow) / 2.0f;
  } else if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VFar) {
    fOffsetY = (m_rtEngine.height - contents_bounds.height);
    fOffsetY -= fSpaceBelow;
  } else {
    fOffsetY += fSpaceAbove;
  }
  m_fVAlignOffset = std::max(fOffsetY, 0.0f);
}

void CFWL_Edit::UpdateCaret() {
  CFX_RectF rtCaret = m_rtCaret;
  rtCaret.Offset(m_rtEngine.left - m_fScrollOffsetX,
                 m_rtEngine.top - m_fScrollOffsetY + m_fVAlignOffset);

  CFX_RectF rtClient = GetClientRect();
  rtCaret.Intersect(rtClient);
  if (rtCaret.left > rtClient.right()) {
    float right = rtCaret.right();
    rtCaret.left = rtClient.right() - 1;
    rtCaret.width = right - rtCaret.left;
  }

  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused && !rtCaret.IsEmpty())
    ShowCaret(&rtCaret);
  else
    HideCaret(&rtCaret);
}

CFWL_ScrollBar* CFWL_Edit::UpdateScroll() {
  bool bShowHorz = m_pHorzScrollBar && m_pHorzScrollBar->IsVisible();
  bool bShowVert = m_pVertScrollBar && m_pVertScrollBar->IsVisible();
  if (!bShowHorz && !bShowVert)
    return nullptr;

  CFX_RectF contents_bounds = m_EdtEngine.GetContentsBoundingBox();
  CFWL_ScrollBar* pRepaint = nullptr;
  if (bShowHorz) {
    CFX_RectF rtScroll = m_pHorzScrollBar->GetWidgetRect();
    if (rtScroll.width < contents_bounds.width) {
      m_pHorzScrollBar->LockUpdate();
      float fRange = contents_bounds.width - rtScroll.width;
      m_pHorzScrollBar->SetRange(0.0f, fRange);

      float fPos = pdfium::clamp(m_fScrollOffsetX, 0.0f, fRange);
      m_pHorzScrollBar->SetPos(fPos);
      m_pHorzScrollBar->SetTrackPos(fPos);
      m_pHorzScrollBar->SetPageSize(rtScroll.width);
      m_pHorzScrollBar->SetStepSize(rtScroll.width / 10);
      m_pHorzScrollBar->RemoveStates(FWL_WGTSTATE_Disabled);
      m_pHorzScrollBar->UnlockUpdate();
      m_pHorzScrollBar->Update();
      pRepaint = m_pHorzScrollBar.get();
    } else if ((m_pHorzScrollBar->GetStates() & FWL_WGTSTATE_Disabled) == 0) {
      m_pHorzScrollBar->LockUpdate();
      m_pHorzScrollBar->SetRange(0, -1);
      m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Disabled);
      m_pHorzScrollBar->UnlockUpdate();
      m_pHorzScrollBar->Update();
      pRepaint = m_pHorzScrollBar.get();
    }
  }

  if (bShowVert) {
    CFX_RectF rtScroll = m_pVertScrollBar->GetWidgetRect();
    if (rtScroll.height < contents_bounds.height) {
      m_pVertScrollBar->LockUpdate();
      float fStep = m_EdtEngine.GetLineSpace();
      float fRange =
          std::max(contents_bounds.height - m_rtEngine.height, fStep);

      m_pVertScrollBar->SetRange(0.0f, fRange);
      float fPos = pdfium::clamp(m_fScrollOffsetY, 0.0f, fRange);
      m_pVertScrollBar->SetPos(fPos);
      m_pVertScrollBar->SetTrackPos(fPos);
      m_pVertScrollBar->SetPageSize(rtScroll.height);
      m_pVertScrollBar->SetStepSize(fStep);
      m_pVertScrollBar->RemoveStates(FWL_WGTSTATE_Disabled);
      m_pVertScrollBar->UnlockUpdate();
      m_pVertScrollBar->Update();
      pRepaint = m_pVertScrollBar.get();
    } else if ((m_pVertScrollBar->GetStates() & FWL_WGTSTATE_Disabled) == 0) {
      m_pVertScrollBar->LockUpdate();
      m_pVertScrollBar->SetRange(0, -1);
      m_pVertScrollBar->SetStates(FWL_WGTSTATE_Disabled);
      m_pVertScrollBar->UnlockUpdate();
      m_pVertScrollBar->Update();
      pRepaint = m_pVertScrollBar.get();
    }
  }
  return pRepaint;
}

bool CFWL_Edit::IsShowScrollBar(bool bVert) {
  if (!bVert)
    return false;
  bool bShow =
      (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ShowScrollbarFocus)
          ? (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) ==
                FWL_WGTSTATE_Focused
          : true;
  return bShow && (m_pProperties->m_dwStyles & FWL_WGTSTYLE_VScroll) &&
         (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine) &&
         IsContentHeightOverflow();
}

bool CFWL_Edit::IsContentHeightOverflow() {
  return m_EdtEngine.GetContentsBoundingBox().height > m_rtEngine.height + 1.0f;
}

void CFWL_Edit::Layout() {
  m_rtClient = GetClientRect();
  m_rtEngine = m_rtClient;
  IFWL_ThemeProvider* theme = GetAvailableTheme();
  if (!theme)
    return;

  float fWidth = theme->GetScrollBarWidth();
  CFWL_ThemePart part;
  if (!m_pOuter) {
    part.m_pWidget = this;
    CFX_RectF pUIMargin = theme->GetUIMargin(part);
    m_rtEngine.Deflate(pUIMargin.left, pUIMargin.top, pUIMargin.width,
                       pUIMargin.height);
  } else if (m_pOuter->GetClassID() == FWL_Type::DateTimePicker) {
    part.m_pWidget = m_pOuter;
    CFX_RectF pUIMargin = theme->GetUIMargin(part);
    m_rtEngine.Deflate(pUIMargin.left, pUIMargin.top, pUIMargin.width,
                       pUIMargin.height);
  }

  bool bShowVertScrollbar = IsShowScrollBar(true);
  bool bShowHorzScrollbar = IsShowScrollBar(false);
  if (bShowVertScrollbar) {
    InitVerticalScrollBar();

    CFX_RectF rtVertScr;
    if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
      rtVertScr = CFX_RectF(m_rtClient.right() + kEditMargin, m_rtClient.top,
                            fWidth, m_rtClient.height);
    } else {
      rtVertScr = CFX_RectF(m_rtClient.right() - fWidth, m_rtClient.top, fWidth,
                            m_rtClient.height);
      if (bShowHorzScrollbar)
        rtVertScr.height -= fWidth;
      m_rtEngine.width -= fWidth;
    }

    m_pVertScrollBar->SetWidgetRect(rtVertScr);
    m_pVertScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
    m_pVertScrollBar->Update();
  } else if (m_pVertScrollBar) {
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible);
  }

  if (bShowHorzScrollbar) {
    InitHorizontalScrollBar();

    CFX_RectF rtHoriScr;
    if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
      rtHoriScr = CFX_RectF(m_rtClient.left, m_rtClient.bottom() + kEditMargin,
                            m_rtClient.width, fWidth);
    } else {
      rtHoriScr = CFX_RectF(m_rtClient.left, m_rtClient.bottom() - fWidth,
                            m_rtClient.width, fWidth);
      if (bShowVertScrollbar)
        rtHoriScr.width -= fWidth;
      m_rtEngine.height -= fWidth;
    }
    m_pHorzScrollBar->SetWidgetRect(rtHoriScr);
    m_pHorzScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
    m_pHorzScrollBar->Update();
  } else if (m_pHorzScrollBar) {
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible);
  }
}

void CFWL_Edit::LayoutScrollBar() {
  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ShowScrollbarFocus) ==
      0) {
    return;
  }

  bool bShowVertScrollbar = IsShowScrollBar(true);
  bool bShowHorzScrollbar = IsShowScrollBar(false);

  IFWL_ThemeProvider* theme = GetAvailableTheme();
  float fWidth = theme ? theme->GetScrollBarWidth() : 0;
  if (bShowVertScrollbar) {
    if (!m_pVertScrollBar) {
      InitVerticalScrollBar();
      CFX_RectF rtVertScr;
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
        rtVertScr = CFX_RectF(m_rtClient.right() + kEditMargin, m_rtClient.top,
                              fWidth, m_rtClient.height);
      } else {
        rtVertScr = CFX_RectF(m_rtClient.right() - fWidth, m_rtClient.top,
                              fWidth, m_rtClient.height);
        if (bShowHorzScrollbar)
          rtVertScr.height -= fWidth;
      }
      m_pVertScrollBar->SetWidgetRect(rtVertScr);
      m_pVertScrollBar->Update();
    }
    m_pVertScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
  } else if (m_pVertScrollBar) {
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible);
  }

  if (bShowHorzScrollbar) {
    if (!m_pHorzScrollBar) {
      InitHorizontalScrollBar();
      CFX_RectF rtHoriScr;
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
        rtHoriScr =
            CFX_RectF(m_rtClient.left, m_rtClient.bottom() + kEditMargin,
                      m_rtClient.width, fWidth);
      } else {
        rtHoriScr = CFX_RectF(m_rtClient.left, m_rtClient.bottom() - fWidth,
                              m_rtClient.width, fWidth);
        if (bShowVertScrollbar)
          rtHoriScr.width -= (fWidth);
      }
      m_pHorzScrollBar->SetWidgetRect(rtHoriScr);
      m_pHorzScrollBar->Update();
    }
    m_pHorzScrollBar->RemoveStates(FWL_WGTSTATE_Invisible);
  } else if (m_pHorzScrollBar) {
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible);
  }
  if (bShowVertScrollbar || bShowHorzScrollbar)
    UpdateScroll();
}

CFX_PointF CFWL_Edit::DeviceToEngine(const CFX_PointF& pt) {
  return pt + CFX_PointF(m_fScrollOffsetX - m_rtEngine.left,
                         m_fScrollOffsetY - m_rtEngine.top - m_fVAlignOffset);
}

void CFWL_Edit::InitVerticalScrollBar() {
  if (m_pVertScrollBar)
    return;

  auto prop = pdfium::MakeUnique<CFWL_WidgetProperties>();
  prop->m_dwStyleExes = FWL_STYLEEXT_SCB_Vert;
  prop->m_dwStates = FWL_WGTSTATE_Disabled | FWL_WGTSTATE_Invisible;
  prop->m_pParent = this;
  prop->m_pThemeProvider = m_pProperties->m_pThemeProvider;
  m_pVertScrollBar = pdfium::MakeUnique<CFWL_ScrollBar>(m_pOwnerApp.Get(),
                                                        std::move(prop), this);
}

void CFWL_Edit::InitHorizontalScrollBar() {
  if (m_pHorzScrollBar)
    return;

  auto prop = pdfium::MakeUnique<CFWL_WidgetProperties>();
  prop->m_dwStyleExes = FWL_STYLEEXT_SCB_Horz;
  prop->m_dwStates = FWL_WGTSTATE_Disabled | FWL_WGTSTATE_Invisible;
  prop->m_pParent = this;
  prop->m_pThemeProvider = m_pProperties->m_pThemeProvider;
  m_pHorzScrollBar = pdfium::MakeUnique<CFWL_ScrollBar>(m_pOwnerApp.Get(),
                                                        std::move(prop), this);
}

void CFWL_Edit::ShowCaret(CFX_RectF* pRect) {
//   if (m_pCaret) {
//     m_pCaret->ShowCaret();
//     if (!pRect->IsEmpty())
//       m_pCaret->SetWidgetRect(*pRect);
//     RepaintRect(m_rtEngine);
//     return;
//   }
// 
//   CFWL_Widget* pOuter = this;
//   pRect->Offset(m_pProperties->m_rtWidget.left, m_pProperties->m_rtWidget.top);
//   while (pOuter->GetOuter()) {
//     pOuter = pOuter->GetOuter();
// 
//     CFX_RectF rtOuter = pOuter->GetWidgetRect();
//     pRect->Offset(rtOuter.left, rtOuter.top);
//   }
// 
//   CXFA_FFWidget* pXFAWidget = pOuter->GetLayoutItem();
//   if (!pXFAWidget)
//     return;
// 
//   IXFA_DocEnvironment* pDocEnvironment =
//       pXFAWidget->GetDoc()->GetDocEnvironment();
//   if (!pDocEnvironment)
//     return;
//
//   CFX_RectF rt = pXFAWidget->GetRotateMatrix().TransformRect(*pRect);
//   pDocEnvironment->DisplayCaret(pXFAWidget, true, &rt);
  DebugBreak();
}

void CFWL_Edit::HideCaret(CFX_RectF* pRect) {
//   if (m_pCaret) {
//     m_pCaret->HideCaret();
//     RepaintRect(m_rtEngine);
//     return;
//   }
// 
//   CFWL_Widget* pOuter = this;
//   while (pOuter->GetOuter())
//     pOuter = pOuter->GetOuter();
// 
//   CXFA_FFWidget* pXFAWidget = pOuter->GetLayoutItem();
//   if (!pXFAWidget)
//     return;
// 
//   IXFA_DocEnvironment* pDocEnvironment =
//       pXFAWidget->GetDoc()->GetDocEnvironment();
//   if (!pDocEnvironment)
//     return;
// 
//   pDocEnvironment->DisplayCaret(pXFAWidget, false, pRect);
  DebugBreak();
}

bool CFWL_Edit::ValidateNumberChar(wchar_t cNum) {
  if (!m_bSetRange)
    return true;

  WideString wsText = m_EdtEngine.GetText();
  if (wsText.IsEmpty())
    return cNum != L'0';

  if (HasSelection())
    return wsText.GetInteger() <= m_iMax;
  if (cNum == L'0' && m_CursorPosition == 0)
    return false;

  int32_t nLen = wsText.GetLength();
  WideString l = wsText.Left(m_CursorPosition);
  WideString r = wsText.Right(nLen - m_CursorPosition);
  WideString wsNew = l + cNum + r;
  return wsNew.GetInteger() <= m_iMax;
}

void CFWL_Edit::InitCaret() {
  if (m_pCaret)
    return;

  m_pCaret = pdfium::MakeUnique<CFWL_Caret>(
      m_pOwnerApp.Get(), pdfium::MakeUnique<CFWL_WidgetProperties>(), this);
  m_pCaret->SetParent(this);
  m_pCaret->SetStates(m_pProperties->m_dwStates);
  UpdateCursorRect();
}

void CFWL_Edit::UpdateCursorRect() {
  int32_t bidi_level;
  if (m_EdtEngine.GetLength() > 0) {
    std::tie(bidi_level, m_rtCaret) =
        m_EdtEngine.GetCharacterInfo(m_CursorPosition);
  } else {
    bidi_level = 0;
    m_rtCaret = CFX_RectF();
  }

  // TODO(dsinclair): This should handle bidi level  ...

  m_rtCaret.width = 1.0f;

  // TODO(hnakashima): Handle correctly edits with empty text instead of using
  // these defaults.
  if (m_rtCaret.height == 0)
    m_rtCaret.height = 8.0f;
}

void CFWL_Edit::SetCursorPosition(size_t position) {
  if (m_CursorPosition == position)
    return;

  m_CursorPosition = std::min(position, m_EdtEngine.GetLength());
  UpdateCursorRect();
  OnCaretChanged();
}

void CFWL_Edit::OnProcessMessage(CFWL_Message* pMessage) {
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
      CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
      switch (pMsg->m_dwCmd) {
        case FWL_MouseCommand::LeftButtonDown:
          OnLButtonDown(pMsg);
          break;
        case FWL_MouseCommand::LeftButtonUp:
          OnLButtonUp(pMsg);
          break;
        case FWL_MouseCommand::LeftButtonDblClk:
          OnButtonDoubleClick(pMsg);
          break;
        case FWL_MouseCommand::Move:
          OnMouseMove(pMsg);
          break;
        case FWL_MouseCommand::RightButtonDown:
          DoRButtonDown(pMsg);
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
      else if (pKey->m_dwCmd == FWL_KeyCommand::Char)
        OnChar(pKey);
      break;
    }
    default:
      break;
  }
  CFWL_Widget::OnProcessMessage(pMessage);
}

void CFWL_Edit::OnProcessEvent(CFWL_Event* pEvent) {
  if (!pEvent || pEvent->GetType() != CFWL_Event::Type::Scroll)
    return;

  CFWL_Widget* pSrcTarget = pEvent->GetSrcTarget();
  if ((pSrcTarget == m_pVertScrollBar.get() && m_pVertScrollBar) ||
      (pSrcTarget == m_pHorzScrollBar.get() && m_pHorzScrollBar)) {
    CFWL_EventScroll* pScrollEvent = static_cast<CFWL_EventScroll*>(pEvent);
    OnScroll(static_cast<CFWL_ScrollBar*>(pSrcTarget),
             pScrollEvent->m_iScrollCode, pScrollEvent->m_fPos);
  }
}

void CFWL_Edit::OnDrawWidget(CXFA_Graphics* pGraphics,
                             const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}

void CFWL_Edit::DoRButtonDown(CFWL_MessageMouse* pMsg) {
  SetCursorPosition(m_EdtEngine.GetIndexForPoint(DeviceToEngine(pMsg->m_pos)));
}

void CFWL_Edit::OnFocusChanged(CFWL_Message* pMsg, bool bSet) {
  bool bRepaint = false;
  if (bSet) {
    m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;

    UpdateVAlignment();
    UpdateOffset();
    UpdateCaret();
  } else if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) {
    m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
    HideCaret(nullptr);

    if (HasSelection()) {
      ClearSelection();
      bRepaint = true;
    }
    UpdateOffset();
  }

  LayoutScrollBar();
  if (!bRepaint)
    return;

  CFX_RectF rtInvalidate(0, 0, m_pProperties->m_rtWidget.width,
                         m_pProperties->m_rtWidget.height);
  RepaintRect(rtInvalidate);
}

void CFWL_Edit::OnLButtonDown(CFWL_MessageMouse* pMsg) {
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)
    return;

  m_bLButtonDown = true;
  SetGrab(true);

  bool bRepaint = false;
  if (m_EdtEngine.HasSelection()) {
    m_EdtEngine.ClearSelection();
    bRepaint = true;
  }

  size_t index_at_click =
      m_EdtEngine.GetIndexForPoint(DeviceToEngine(pMsg->m_pos));

  if (index_at_click != m_CursorPosition &&
      !!(pMsg->m_dwFlags & FWL_KEYFLAG_Shift)) {
    size_t start = std::min(m_CursorPosition, index_at_click);
    size_t end = std::max(m_CursorPosition, index_at_click);

    m_EdtEngine.SetSelection(start, end - start);
    bRepaint = true;
  } else {
    SetCursorPosition(index_at_click);
  }

  if (bRepaint)
    RepaintRect(m_rtEngine);
}

void CFWL_Edit::OnLButtonUp(CFWL_MessageMouse* pMsg) {
  m_bLButtonDown = false;
  SetGrab(false);
}

void CFWL_Edit::OnButtonDoubleClick(CFWL_MessageMouse* pMsg) {
  size_t click_idx = m_EdtEngine.GetIndexForPoint(DeviceToEngine(pMsg->m_pos));
  size_t start_idx;
  size_t count;
  std::tie(start_idx, count) = m_EdtEngine.BoundsForWordAt(click_idx);

  m_EdtEngine.SetSelection(start_idx, count);
  m_CursorPosition = start_idx + count;
  RepaintRect(m_rtEngine);
}

void CFWL_Edit::OnMouseMove(CFWL_MessageMouse* pMsg) {
  bool shift = !!(pMsg->m_dwFlags & FWL_KEYFLAG_Shift);
  if (!m_bLButtonDown || !shift)
    return;

  size_t old_cursor_pos = m_CursorPosition;
  SetCursorPosition(m_EdtEngine.GetIndexForPoint(DeviceToEngine(pMsg->m_pos)));
  if (old_cursor_pos == m_CursorPosition)
    return;

  size_t length = m_EdtEngine.GetLength();
  if (m_CursorPosition > length)
    SetCursorPosition(length);

  size_t sel_start = 0;
  size_t count = 0;
  if (m_EdtEngine.HasSelection())
    std::tie(sel_start, count) = m_EdtEngine.GetSelection();
  else
    sel_start = old_cursor_pos;

  size_t start_pos = std::min(sel_start, m_CursorPosition);
  size_t end_pos = std::max(sel_start, m_CursorPosition);
  m_EdtEngine.SetSelection(start_pos, end_pos - start_pos);
}

void CFWL_Edit::OnKeyDown(CFWL_MessageKey* pMsg) {
  bool bShift = !!(pMsg->m_dwFlags & FWL_KEYFLAG_Shift);
  bool bCtrl = !!(pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl);

  size_t sel_start = m_CursorPosition;
  if (m_EdtEngine.HasSelection()) {
    size_t start_idx;
    size_t count;
    std::tie(start_idx, count) = m_EdtEngine.GetSelection();
    sel_start = start_idx;
  }

  switch (pMsg->m_dwKeyCode) {
    case FWL_VKEY_Left:
      SetCursorPosition(m_EdtEngine.GetIndexLeft(m_CursorPosition));
      break;
    case FWL_VKEY_Right:
      SetCursorPosition(m_EdtEngine.GetIndexRight(m_CursorPosition));
      break;
    case FWL_VKEY_Up:
      SetCursorPosition(m_EdtEngine.GetIndexUp(m_CursorPosition));
      break;
    case FWL_VKEY_Down:
      SetCursorPosition(m_EdtEngine.GetIndexDown(m_CursorPosition));
      break;
    case FWL_VKEY_Home:
      SetCursorPosition(
          bCtrl ? 0 : m_EdtEngine.GetIndexAtStartOfLine(m_CursorPosition));
      break;
    case FWL_VKEY_End:
      SetCursorPosition(
          bCtrl ? m_EdtEngine.GetLength()
                : m_EdtEngine.GetIndexAtEndOfLine(m_CursorPosition));
      break;
    case FWL_VKEY_Delete: {
      if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly) ||
          (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
        break;
      }

      m_EdtEngine.Delete(m_CursorPosition, 1);
      UpdateCaret();
      break;
    }
    case FWL_VKEY_Insert:
    case FWL_VKEY_F2:
    case FWL_VKEY_Tab:
    default:
      break;
  }

  // Update the selection.
  if (bShift && sel_start != m_CursorPosition) {
    m_EdtEngine.SetSelection(std::min(sel_start, m_CursorPosition),
                             std::max(sel_start, m_CursorPosition));
    RepaintRect(m_rtEngine);
  }
}

void CFWL_Edit::OnChar(CFWL_MessageKey* pMsg) {
  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly) ||
      (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
    return;
  }

  wchar_t c = static_cast<wchar_t>(pMsg->m_dwKeyCode);
  switch (c) {
    case L'\b':
      if (m_CursorPosition > 0) {
        SetCursorPosition(m_CursorPosition - 1);
        m_EdtEngine.Delete(m_CursorPosition, 1);
        UpdateCaret();
      }
      break;
    case L'\n':
    case 27:   // Esc
    case 127:  // Delete
      break;
    case L'\t':
      m_EdtEngine.Insert(m_CursorPosition, L"\t");
      SetCursorPosition(m_CursorPosition + 1);
      break;
    case L'\r':
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_WantReturn) {
        m_EdtEngine.Insert(m_CursorPosition, L"\n");
        SetCursorPosition(m_CursorPosition + 1);
      }
      break;
    default: {
      if (pMsg->m_dwFlags & kEditingModifier)
        break;

      m_EdtEngine.Insert(m_CursorPosition, WideString(c));
      SetCursorPosition(m_CursorPosition + 1);
      break;
    }
  }
}

bool CFWL_Edit::OnScroll(CFWL_ScrollBar* pScrollBar,
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
      if (fPos < fs.width + fStep / 2) {
        fPos = fs.width;
      }
      break;
    }
    case CFWL_EventScroll::Code::StepForward: {
      fPos += fStep;
      if (fPos > fs.height - fStep / 2) {
        fPos = fs.height;
      }
      break;
    }
    case CFWL_EventScroll::Code::PageBackward: {
      fPos -= pScrollBar->GetPageSize();
      if (fPos < fs.width) {
        fPos = fs.width;
      }
      break;
    }
    case CFWL_EventScroll::Code::PageForward: {
      fPos += pScrollBar->GetPageSize();
      if (fPos > fs.height) {
        fPos = fs.height;
      }
      break;
    }
    case CFWL_EventScroll::Code::Pos:
    case CFWL_EventScroll::Code::TrackPos:
    case CFWL_EventScroll::Code::None:
      break;
    case CFWL_EventScroll::Code::EndScroll:
      return false;
  }
  if (iCurPos == fPos)
    return true;

  pScrollBar->SetPos(fPos);
  pScrollBar->SetTrackPos(fPos);
  UpdateOffset(pScrollBar, fPos - iCurPos);
  UpdateCaret();

  CFX_RectF rect = GetWidgetRect();
  RepaintRect(CFX_RectF(0, 0, rect.width + 2, rect.height + 2));
  return true;
}
