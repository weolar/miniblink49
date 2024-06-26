// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_fwltheme.h"

#include "core/fxcrt/fx_codepage.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fwl/cfwl_barcode.h"
#include "xfa/fwl/cfwl_caret.h"
#include "xfa/fwl/cfwl_checkbox.h"
#include "xfa/fwl/cfwl_combobox.h"
#include "xfa/fwl/cfwl_datetimepicker.h"
#include "xfa/fwl/cfwl_edit.h"
#include "xfa/fwl/cfwl_listbox.h"
#include "xfa/fwl/cfwl_monthcalendar.h"
#include "xfa/fwl/cfwl_picturebox.h"
#include "xfa/fwl/cfwl_pushbutton.h"
#include "xfa/fwl/cfwl_scrollbar.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themetext.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/parser/cxfa_para.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"

namespace {

const wchar_t* const g_FWLTheme_CalFonts[] = {
    L"Arial", L"Courier New", L"DejaVu Sans",
};

const float kLineHeight = 12.0f;

}  // namespace

CXFA_FFWidget* XFA_ThemeGetOuterWidget(CFWL_Widget* pWidget) {
  CFWL_Widget* pOuter = pWidget;
  while (pOuter && pOuter->GetOuter())
    pOuter = pOuter->GetOuter();
  return pOuter ? pOuter->GetLayoutItem() : nullptr;
}

CXFA_FWLTheme::CXFA_FWLTheme(CXFA_FFApp* pApp)
    : m_pCheckBoxTP(pdfium::MakeUnique<CFWL_CheckBoxTP>()),
      m_pListBoxTP(pdfium::MakeUnique<CFWL_ListBoxTP>()),
      m_pPictureBoxTP(pdfium::MakeUnique<CFWL_PictureBoxTP>()),
      m_pSrollBarTP(pdfium::MakeUnique<CFWL_ScrollBarTP>()),
      m_pEditTP(pdfium::MakeUnique<CFWL_EditTP>()),
      m_pComboBoxTP(pdfium::MakeUnique<CFWL_ComboBoxTP>()),
      m_pMonthCalendarTP(pdfium::MakeUnique<CFWL_MonthCalendarTP>()),
      m_pDateTimePickerTP(pdfium::MakeUnique<CFWL_DateTimePickerTP>()),
      m_pPushButtonTP(pdfium::MakeUnique<CFWL_PushButtonTP>()),
      m_pCaretTP(pdfium::MakeUnique<CFWL_CaretTP>()),
      m_pBarcodeTP(pdfium::MakeUnique<CFWL_BarcodeTP>()),
      m_pTextOut(pdfium::MakeUnique<CFDE_TextOut>()),
      m_pApp(pApp) {
}

bool CXFA_FWLTheme::LoadCalendarFont(CXFA_FFDoc* doc) {
  for (size_t i = 0; !m_pCalendarFont && i < FX_ArraySize(g_FWLTheme_CalFonts);
       ++i) {
    m_pCalendarFont =
        m_pApp->GetXFAFontMgr()->GetFont(doc, g_FWLTheme_CalFonts[i], 0);
  }

  if (!m_pCalendarFont) {
    CFGAS_FontMgr* font_mgr = m_pApp->GetFDEFontMgr();
    if (font_mgr) {
      m_pCalendarFont = font_mgr->GetFontByCodePage(
          FX_CODEPAGE_MSWin_WesternEuropean, 0, nullptr);
    }
  }

  return m_pCalendarFont != nullptr;
}

CXFA_FWLTheme::~CXFA_FWLTheme() {
  m_pTextOut.reset();
  FWLTHEME_Release();
}

void CXFA_FWLTheme::DrawBackground(const CFWL_ThemeBackground& pParams) {
  GetTheme(pParams.m_pWidget)->DrawBackground(pParams);
}

void CXFA_FWLTheme::DrawText(const CFWL_ThemeText& pParams) {
  if (pParams.m_wsText.IsEmpty())
    return;

  if (pParams.m_pWidget->GetClassID() == FWL_Type::MonthCalendar) {
    CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams.m_pWidget);
    if (!pWidget)
      return;

    m_pTextOut->SetStyles(pParams.m_dwTTOStyles);
    m_pTextOut->SetAlignment(pParams.m_iTTOAlign);
    m_pTextOut->SetFont(m_pCalendarFont);
    m_pTextOut->SetFontSize(FWLTHEME_CAPACITY_FontSize);
    m_pTextOut->SetTextColor(FWLTHEME_CAPACITY_TextColor);
    if ((pParams.m_iPart == CFWL_Part::DatesIn) &&
        !(pParams.m_dwStates & FWL_ITEMSTATE_MCD_Flag) &&
        (pParams.m_dwStates &
         (CFWL_PartState_Hovered | CFWL_PartState_Selected))) {
      m_pTextOut->SetTextColor(0xFF888888);
    }
    if (pParams.m_iPart == CFWL_Part::Caption)
      m_pTextOut->SetTextColor(ArgbEncode(0xff, 0, 153, 255));

    CXFA_Graphics* pGraphics = pParams.m_pGraphics;
    CFX_RenderDevice* pRenderDevice = pGraphics->GetRenderDevice();
    CFX_Matrix mtPart = pParams.m_matrix;
    const CFX_Matrix* pMatrix = pGraphics->GetMatrix();
    if (pMatrix)
      mtPart.Concat(*pMatrix);

    m_pTextOut->SetMatrix(mtPart);
    m_pTextOut->DrawLogicText(pRenderDevice, pParams.m_wsText.AsStringView(),
                              pParams.m_rtPart);
    return;
  }
  CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams.m_pWidget);
  if (!pWidget)
    return;

  CXFA_Node* pNode = pWidget->GetNode();
  CXFA_Graphics* pGraphics = pParams.m_pGraphics;
  CFX_RenderDevice* pRenderDevice = pGraphics->GetRenderDevice();
  m_pTextOut->SetStyles(pParams.m_dwTTOStyles);
  m_pTextOut->SetAlignment(pParams.m_iTTOAlign);
  m_pTextOut->SetFont(pNode->GetFDEFont(pWidget->GetDoc()));
  m_pTextOut->SetFontSize(pNode->GetFontSize());
  m_pTextOut->SetTextColor(pNode->GetTextColor());
  CFX_Matrix mtPart = pParams.m_matrix;
  const CFX_Matrix* pMatrix = pGraphics->GetMatrix();
  if (pMatrix)
    mtPart.Concat(*pMatrix);

  m_pTextOut->SetMatrix(mtPart);
  m_pTextOut->DrawLogicText(pRenderDevice, pParams.m_wsText.AsStringView(),
                            pParams.m_rtPart);
}

CFX_RectF CXFA_FWLTheme::GetUIMargin(const CFWL_ThemePart& pThemePart) const {
  CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart.m_pWidget);
  if (!pWidget)
    return CFX_RectF();

  CXFA_ContentLayoutItem* pItem = pWidget;
  CXFA_Node* pNode = pWidget->GetNode();
  CFX_RectF rect = pNode->GetUIMargin();
  CXFA_Para* para = pNode->GetParaIfExists();
  if (para) {
    rect.left += para->GetMarginLeft();
    if (pNode->IsMultiLine())
      rect.width += para->GetMarginRight();
  }
  if (!pItem->GetPrev()) {
    if (pItem->GetNext())
      rect.height = 0;
  } else if (!pItem->GetNext()) {
    rect.top = 0;
  } else {
    rect.top = 0;
    rect.height = 0;
  }
  return rect;
}

float CXFA_FWLTheme::GetCXBorderSize() const {
  return 1.0f;
}

float CXFA_FWLTheme::GetCYBorderSize() const {
  return 1.0f;
}

float CXFA_FWLTheme::GetFontSize(const CFWL_ThemePart& pThemePart) const {
  if (CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart.m_pWidget))
    return pWidget->GetNode()->GetFontSize();
  return FWLTHEME_CAPACITY_FontSize;
}

RetainPtr<CFGAS_GEFont> CXFA_FWLTheme::GetFont(
    const CFWL_ThemePart& pThemePart) const {
  if (CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart.m_pWidget))
    return pWidget->GetNode()->GetFDEFont(pWidget->GetDoc());
  return GetTheme(pThemePart.m_pWidget)->GetFont();
}

float CXFA_FWLTheme::GetLineHeight(const CFWL_ThemePart& pThemePart) const {
  if (CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart.m_pWidget))
    return pWidget->GetNode()->GetLineHeight();
  return kLineHeight;
}

float CXFA_FWLTheme::GetScrollBarWidth() const {
  return 9.0f;
}

FX_COLORREF CXFA_FWLTheme::GetTextColor(
    const CFWL_ThemePart& pThemePart) const {
  if (CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart.m_pWidget))
    return pWidget->GetNode()->GetTextColor();
  return FWLTHEME_CAPACITY_TextColor;
}

CFX_SizeF CXFA_FWLTheme::GetSpaceAboveBelow(
    const CFWL_ThemePart& pThemePart) const {
  CFX_SizeF sizeAboveBelow;
  if (CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart.m_pWidget)) {
    CXFA_Para* para = pWidget->GetNode()->GetParaIfExists();
    if (para) {
      sizeAboveBelow.width = para->GetSpaceAbove();
      sizeAboveBelow.height = para->GetSpaceBelow();
    }
  }
  return sizeAboveBelow;
}

void CXFA_FWLTheme::CalcTextRect(const CFWL_ThemeText& pParams,
                                 CFX_RectF* pRect) {
  if (!m_pTextOut)
    return;

  CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams.m_pWidget);
  if (!pWidget)
    return;

  if (pParams.m_pWidget->GetClassID() == FWL_Type::MonthCalendar) {
    m_pTextOut->SetFont(m_pCalendarFont);
    m_pTextOut->SetFontSize(FWLTHEME_CAPACITY_FontSize);
    m_pTextOut->SetTextColor(FWLTHEME_CAPACITY_TextColor);
    m_pTextOut->SetAlignment(pParams.m_iTTOAlign);
    m_pTextOut->SetStyles(pParams.m_dwTTOStyles);
    m_pTextOut->CalcLogicSize(pParams.m_wsText, pRect);
    return;
  }

  CXFA_Node* pNode = pWidget->GetNode();
  m_pTextOut->SetFont(pNode->GetFDEFont(pWidget->GetDoc()));
  m_pTextOut->SetFontSize(pNode->GetFontSize());
  m_pTextOut->SetTextColor(pNode->GetTextColor());
  m_pTextOut->SetAlignment(pParams.m_iTTOAlign);
  m_pTextOut->SetStyles(pParams.m_dwTTOStyles);
  m_pTextOut->CalcLogicSize(pParams.m_wsText, pRect);
}

CFWL_WidgetTP* CXFA_FWLTheme::GetTheme(CFWL_Widget* pWidget) const {
  switch (pWidget->GetClassID()) {
    case FWL_Type::CheckBox:
      return m_pCheckBoxTP.get();
    case FWL_Type::ListBox:
      return m_pListBoxTP.get();
    case FWL_Type::PictureBox:
      return m_pPictureBoxTP.get();
    case FWL_Type::ScrollBar:
      return m_pSrollBarTP.get();
    case FWL_Type::Edit:
      return m_pEditTP.get();
    case FWL_Type::ComboBox:
      return m_pComboBoxTP.get();
    case FWL_Type::MonthCalendar:
      return m_pMonthCalendarTP.get();
    case FWL_Type::DateTimePicker:
      return m_pDateTimePickerTP.get();
    case FWL_Type::PushButton:
      return m_pPushButtonTP.get();
    case FWL_Type::Caret:
      return m_pCaretTP.get();
    case FWL_Type::Barcode:
      return m_pBarcodeTP.get();
    default:
      return nullptr;
  }
}
