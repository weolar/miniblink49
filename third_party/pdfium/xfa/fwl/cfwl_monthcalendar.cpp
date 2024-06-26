// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_monthcalendar.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fwl/cfwl_datetimepicker.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themetext.h"
#include "xfa/fwl/ifwl_themeprovider.h"

#define MONTHCAL_HSEP_HEIGHT 1
#define MONTHCAL_HMARGIN 3
#define MONTHCAL_VMARGIN 2
#define MONTHCAL_ROWS 9
#define MONTHCAL_COLUMNS 7
#define MONTHCAL_HEADER_BTN_VMARGIN 7
#define MONTHCAL_HEADER_BTN_HMARGIN 5

namespace {

WideString GetAbbreviatedDayOfWeek(int day) {
  switch (day) {
    case 0:
      return L"Sun";
    case 1:
      return L"Mon";
    case 2:
      return L"Tue";
    case 3:
      return L"Wed";
    case 4:
      return L"Thu";
    case 5:
      return L"Fri";
    case 6:
      return L"Sat";
    default:
      NOTREACHED();
      return L"";
  }
}

WideString GetMonth(int month) {
  switch (month) {
    case 0:
      return L"January";
    case 1:
      return L"February";
    case 2:
      return L"March";
    case 3:
      return L"April";
    case 4:
      return L"May";
    case 5:
      return L"June";
    case 6:
      return L"July";
    case 7:
      return L"August";
    case 8:
      return L"September";
    case 9:
      return L"October";
    case 10:
      return L"November";
    case 11:
      return L"December";
    default:
      NOTREACHED();
      return L"";
  }
}

}  // namespace

CFWL_MonthCalendar::CFWL_MonthCalendar(
    const CFWL_App* app,
    std::unique_ptr<CFWL_WidgetProperties> properties,
    CFWL_Widget* pOuter)
    : CFWL_Widget(app, std::move(properties), pOuter) {}

CFWL_MonthCalendar::~CFWL_MonthCalendar() = default;

FWL_Type CFWL_MonthCalendar::GetClassID() const {
  return FWL_Type::MonthCalendar;
}

CFX_RectF CFWL_MonthCalendar::GetAutosizedWidgetRect() {
  CFX_SizeF fs = CalcSize();
  CFX_RectF rect(0, 0, fs.width, fs.height);
  InflateWidgetRect(rect);
  return rect;
}

void CFWL_MonthCalendar::Update() {
  if (IsLocked())
    return;
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();

  GetCapValue();
  if (!m_bInitialized) {
    InitDate();
    m_bInitialized = true;
  }

  ClearDateItem();
  ResetDateItem();
  Layout();
}

void CFWL_MonthCalendar::DrawWidget(CXFA_Graphics* pGraphics,
                                    const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();

  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  if (HasBorder())
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, matrix);

  DrawBackground(pGraphics, pTheme, &matrix);
  DrawHeadBK(pGraphics, pTheme, &matrix);
  DrawLButton(pGraphics, pTheme, &matrix);
  DrawRButton(pGraphics, pTheme, &matrix);
  DrawSeperator(pGraphics, pTheme, &matrix);
  DrawDatesInBK(pGraphics, pTheme, &matrix);
  DrawDatesInCircle(pGraphics, pTheme, &matrix);
  DrawCaption(pGraphics, pTheme, &matrix);
  DrawWeek(pGraphics, pTheme, &matrix);
  DrawDatesIn(pGraphics, pTheme, &matrix);
  DrawDatesOut(pGraphics, pTheme, &matrix);
  DrawToday(pGraphics, pTheme, &matrix);
}

void CFWL_MonthCalendar::SetSelect(int32_t iYear,
                                   int32_t iMonth,
                                   int32_t iDay) {
  ChangeToMonth(iYear, iMonth);
  AddSelDay(iDay);
}

void CFWL_MonthCalendar::DrawBackground(CXFA_Graphics* pGraphics,
                                        IFWL_ThemeProvider* pTheme,
                                        const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::Background;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtClient;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(params);
}

void CFWL_MonthCalendar::DrawHeadBK(CXFA_Graphics* pGraphics,
                                    IFWL_ThemeProvider* pTheme,
                                    const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::Header;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtHead;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(params);
}

void CFWL_MonthCalendar::DrawLButton(CXFA_Graphics* pGraphics,
                                     IFWL_ThemeProvider* pTheme,
                                     const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::LBtn;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = m_iLBtnPartStates;
  params.m_rtPart = m_rtLBtn;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(params);
}

void CFWL_MonthCalendar::DrawRButton(CXFA_Graphics* pGraphics,
                                     IFWL_ThemeProvider* pTheme,
                                     const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::RBtn;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = m_iRBtnPartStates;
  params.m_rtPart = m_rtRBtn;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(params);
}

void CFWL_MonthCalendar::DrawCaption(CXFA_Graphics* pGraphics,
                                     IFWL_ThemeProvider* pTheme,
                                     const CFX_Matrix* pMatrix) {
  CFWL_ThemeText textParam;
  textParam.m_pWidget = this;
  textParam.m_iPart = CFWL_Part::Caption;
  textParam.m_dwStates = CFWL_PartState_Normal;
  textParam.m_pGraphics = pGraphics;
  textParam.m_wsText = GetHeadText(m_iCurYear, m_iCurMonth);
  m_szHead = CalcTextSize(textParam.m_wsText,
                          m_pProperties->m_pThemeProvider.Get(), false);
  CalcHeadSize();
  textParam.m_rtPart = m_rtHeadText;
  textParam.m_dwTTOStyles.single_line_ = true;
  textParam.m_iTTOAlign = FDE_TextAlignment::kCenter;
  if (pMatrix)
    textParam.m_matrix.Concat(*pMatrix);
  pTheme->DrawText(textParam);
}

void CFWL_MonthCalendar::DrawSeperator(CXFA_Graphics* pGraphics,
                                       IFWL_ThemeProvider* pTheme,
                                       const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::HSeparator;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtHSep;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(params);
}

void CFWL_MonthCalendar::DrawDatesInBK(CXFA_Graphics* pGraphics,
                                       IFWL_ThemeProvider* pTheme,
                                       const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::DateInBK;
  params.m_pGraphics = pGraphics;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);

  int32_t iCount = pdfium::CollectionSize<int32_t>(m_arrDates);
  for (int32_t j = 0; j < iCount; j++) {
    DATEINFO* pDataInfo = m_arrDates[j].get();
    if (pDataInfo->dwStates & FWL_ITEMSTATE_MCD_Selected) {
      params.m_dwStates |= CFWL_PartState_Selected;
      if (pDataInfo->dwStates & FWL_ITEMSTATE_MCD_Flag) {
        params.m_dwStates |= CFWL_PartState_Flagged;
      }
    } else if (j == m_iHovered - 1) {
      params.m_dwStates |= CFWL_PartState_Hovered;
    } else if (pDataInfo->dwStates & FWL_ITEMSTATE_MCD_Flag) {
      params.m_dwStates = CFWL_PartState_Flagged;
      pTheme->DrawBackground(params);
    }
    params.m_rtPart = pDataInfo->rect;
    pTheme->DrawBackground(params);
    params.m_dwStates = 0;
  }
}

void CFWL_MonthCalendar::DrawWeek(CXFA_Graphics* pGraphics,
                                  IFWL_ThemeProvider* pTheme,
                                  const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::Week;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TextAlignment::kCenter;
  params.m_dwTTOStyles.single_line_ = true;

  CFX_RectF rtDayOfWeek;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);

  for (int32_t i = 0; i < 7; ++i) {
    rtDayOfWeek =
        CFX_RectF(m_rtWeek.left + i * (m_szCell.width + MONTHCAL_HMARGIN * 2),
                  m_rtWeek.top, m_szCell);

    params.m_rtPart = rtDayOfWeek;
    params.m_wsText = GetAbbreviatedDayOfWeek(i);
    pTheme->DrawText(params);
  }
}

void CFWL_MonthCalendar::DrawToday(CXFA_Graphics* pGraphics,
                                   IFWL_ThemeProvider* pTheme,
                                   const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::Today;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TextAlignment::kCenterLeft;
  params.m_wsText = GetTodayText(m_iYear, m_iMonth, m_iDay);

  m_szToday = CalcTextSize(params.m_wsText,
                           m_pProperties->m_pThemeProvider.Get(), false);
  CalcTodaySize();
  params.m_rtPart = m_rtToday;
  params.m_dwTTOStyles.single_line_ = true;

  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawText(params);
}

void CFWL_MonthCalendar::DrawDatesIn(CXFA_Graphics* pGraphics,
                                     IFWL_ThemeProvider* pTheme,
                                     const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::DatesIn;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TextAlignment::kCenter;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);

  int32_t iCount = pdfium::CollectionSize<int32_t>(m_arrDates);
  for (int32_t j = 0; j < iCount; j++) {
    DATEINFO* pDataInfo = m_arrDates[j].get();
    params.m_wsText = pDataInfo->wsDay;
    params.m_rtPart = pDataInfo->rect;
    params.m_dwStates = pDataInfo->dwStates;
    if (j + 1 == m_iHovered)
      params.m_dwStates |= CFWL_PartState_Hovered;

    params.m_dwTTOStyles.single_line_ = true;
    pTheme->DrawText(params);
  }
}

void CFWL_MonthCalendar::DrawDatesOut(CXFA_Graphics* pGraphics,
                                      IFWL_ThemeProvider* pTheme,
                                      const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::DatesOut;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TextAlignment::kCenter;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawText(params);
}

void CFWL_MonthCalendar::DrawDatesInCircle(CXFA_Graphics* pGraphics,
                                           IFWL_ThemeProvider* pTheme,
                                           const CFX_Matrix* pMatrix) {
  if (m_iMonth != m_iCurMonth || m_iYear != m_iCurYear)
    return;

  if (m_iDay < 1 || m_iDay > pdfium::CollectionSize<int32_t>(m_arrDates))
    return;

  DATEINFO* pDate = m_arrDates[m_iDay - 1].get();
  if (!pDate)
    return;

  CFWL_ThemeBackground params;
  params.m_pWidget = this;
  params.m_iPart = CFWL_Part::DateInCircle;
  params.m_pGraphics = pGraphics;
  params.m_rtPart = pDate->rect;
  params.m_dwStates = CFWL_PartState_Normal;
  if (pMatrix)
    params.m_matrix.Concat(*pMatrix);
  pTheme->DrawBackground(params);
}

CFX_SizeF CFWL_MonthCalendar::CalcSize() {
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider.Get();
  if (!pTheme)
    return CFX_SizeF();

  float fMaxWeekW = 0.0f;
  float fMaxWeekH = 0.0f;
  for (int i = 0; i < 7; ++i) {
    CFX_SizeF sz = CalcTextSize(GetAbbreviatedDayOfWeek(i), pTheme, false);
    fMaxWeekW = (fMaxWeekW >= sz.width) ? fMaxWeekW : sz.width;
    fMaxWeekH = (fMaxWeekH >= sz.height) ? fMaxWeekH : sz.height;
  }

  float fDayMaxW = 0.0f;
  float fDayMaxH = 0.0f;
  for (int day = 10; day <= 31; day++) {
    CFX_SizeF sz = CalcTextSize(WideString::Format(L"%d", day), pTheme, false);
    fDayMaxW = (fDayMaxW >= sz.width) ? fDayMaxW : sz.width;
    fDayMaxH = (fDayMaxH >= sz.height) ? fDayMaxH : sz.height;
  }
  m_szCell.width =
      static_cast<int>(0.5 + (fMaxWeekW >= fDayMaxW ? fMaxWeekW : fDayMaxW));
  m_szCell.height = fMaxWeekH >= fDayMaxH ? fMaxWeekH : fDayMaxH;

  CFX_SizeF fs;
  fs.width = m_szCell.width * MONTHCAL_COLUMNS +
             MONTHCAL_HMARGIN * MONTHCAL_COLUMNS * 2 +
             MONTHCAL_HEADER_BTN_HMARGIN * 2;

  float fMonthMaxW = 0.0f;
  float fMonthMaxH = 0.0f;
  for (int i = 0; i < 12; ++i) {
    CFX_SizeF sz = CalcTextSize(GetMonth(i), pTheme, false);
    fMonthMaxW = (fMonthMaxW >= sz.width) ? fMonthMaxW : sz.width;
    fMonthMaxH = (fMonthMaxH >= sz.height) ? fMonthMaxH : sz.height;
  }

  CFX_SizeF szYear =
      CalcTextSize(GetHeadText(m_iYear, m_iMonth), pTheme, false);
  fMonthMaxH = std::max(fMonthMaxH, szYear.height);
  m_szHead = CFX_SizeF(fMonthMaxW + szYear.width, fMonthMaxH);
  fMonthMaxW =
      m_szHead.width + MONTHCAL_HEADER_BTN_HMARGIN * 2 + m_szCell.width * 2;
  fs.width = std::max(fs.width, fMonthMaxW);

  m_wsToday = GetTodayText(m_iYear, m_iMonth, m_iDay);
  m_szToday = CalcTextSize(m_wsToday, pTheme, false);
  m_szToday.height = (m_szToday.height >= m_szCell.height) ? m_szToday.height
                                                           : m_szCell.height;
  fs.height = m_szCell.width + m_szCell.height * (MONTHCAL_ROWS - 2) +
              m_szToday.height + MONTHCAL_VMARGIN * MONTHCAL_ROWS * 2 +
              MONTHCAL_HEADER_BTN_VMARGIN * 4;
  return fs;
}

void CFWL_MonthCalendar::CalcHeadSize() {
  float fHeadHMargin = (m_rtClient.width - m_szHead.width) / 2;
  float fHeadVMargin = (m_szCell.width - m_szHead.height) / 2;
  m_rtHeadText = CFX_RectF(m_rtClient.left + fHeadHMargin,
                           m_rtClient.top + MONTHCAL_HEADER_BTN_VMARGIN +
                               MONTHCAL_VMARGIN + fHeadVMargin,
                           m_szHead);
}

void CFWL_MonthCalendar::CalcTodaySize() {
  m_rtTodayFlag = CFX_RectF(
      m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN + MONTHCAL_HMARGIN,
      m_rtDates.bottom() + MONTHCAL_HEADER_BTN_VMARGIN + MONTHCAL_VMARGIN,
      m_szCell.width, m_szToday.height);
  m_rtToday = CFX_RectF(
      m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN + m_szCell.width +
          MONTHCAL_HMARGIN * 2,
      m_rtDates.bottom() + MONTHCAL_HEADER_BTN_VMARGIN + MONTHCAL_VMARGIN,
      m_szToday);
}

void CFWL_MonthCalendar::Layout() {
  m_rtClient = GetClientRect();

  m_rtHead = CFX_RectF(
      m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN, m_rtClient.top,
      m_rtClient.width - MONTHCAL_HEADER_BTN_HMARGIN * 2,
      m_szCell.width + (MONTHCAL_HEADER_BTN_VMARGIN + MONTHCAL_VMARGIN) * 2);
  m_rtWeek = CFX_RectF(m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN,
                       m_rtHead.bottom(),
                       m_rtClient.width - MONTHCAL_HEADER_BTN_HMARGIN * 2,
                       m_szCell.height + MONTHCAL_VMARGIN * 2);
  m_rtLBtn = CFX_RectF(m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN,
                       m_rtClient.top + MONTHCAL_HEADER_BTN_VMARGIN,
                       m_szCell.width, m_szCell.width);
  m_rtRBtn = CFX_RectF(m_rtClient.left + m_rtClient.width -
                           MONTHCAL_HEADER_BTN_HMARGIN - m_szCell.width,
                       m_rtClient.top + MONTHCAL_HEADER_BTN_VMARGIN,
                       m_szCell.width, m_szCell.width);
  m_rtHSep = CFX_RectF(
      m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN + MONTHCAL_HMARGIN,
      m_rtWeek.bottom() - MONTHCAL_VMARGIN,
      m_rtClient.width - (MONTHCAL_HEADER_BTN_HMARGIN + MONTHCAL_HMARGIN) * 2,
      MONTHCAL_HSEP_HEIGHT);
  m_rtDates = CFX_RectF(m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN,
                        m_rtWeek.bottom(),
                        m_rtClient.width - MONTHCAL_HEADER_BTN_HMARGIN * 2,
                        m_szCell.height * (MONTHCAL_ROWS - 3) +
                            MONTHCAL_VMARGIN * (MONTHCAL_ROWS - 3) * 2);

  CalDateItem();
}

void CFWL_MonthCalendar::CalDateItem() {
  bool bNewWeek = false;
  int32_t iWeekOfMonth = 0;
  float fLeft = m_rtDates.left;
  float fTop = m_rtDates.top;
  for (const auto& pDateInfo : m_arrDates) {
    if (bNewWeek) {
      iWeekOfMonth++;
      bNewWeek = false;
    }
    pDateInfo->rect = CFX_RectF(
        fLeft +
            pDateInfo->iDayOfWeek * (m_szCell.width + (MONTHCAL_HMARGIN * 2)),
        fTop + iWeekOfMonth * (m_szCell.height + (MONTHCAL_VMARGIN * 2)),
        m_szCell.width + (MONTHCAL_HMARGIN * 2),
        m_szCell.height + (MONTHCAL_VMARGIN * 2));
    if (pDateInfo->iDayOfWeek >= 6)
      bNewWeek = true;
  }
}

void CFWL_MonthCalendar::GetCapValue() {
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
}

void CFWL_MonthCalendar::InitDate() {
  CFX_DateTime now = CFX_DateTime::Now();

  m_iYear = now.GetYear();
  m_iMonth = now.GetMonth();
  m_iDay = now.GetDay();
  m_iCurYear = m_iYear;
  m_iCurMonth = m_iMonth;

  m_wsToday = GetTodayText(m_iYear, m_iMonth, m_iDay);
  m_wsHead = GetHeadText(m_iCurYear, m_iCurMonth);
  m_dtMin = DATE(1500, 12, 1);
  m_dtMax = DATE(2200, 1, 1);
}

void CFWL_MonthCalendar::ClearDateItem() {
  m_arrDates.clear();
}

void CFWL_MonthCalendar::ResetDateItem() {
  int32_t iDays = FX_DaysInMonth(m_iCurYear, m_iCurMonth);
  int32_t iDayOfWeek =
      CFX_DateTime(m_iCurYear, m_iCurMonth, 1, 0, 0, 0, 0).GetDayOfWeek();
  for (int32_t i = 0; i < iDays; i++) {
    if (iDayOfWeek >= 7)
      iDayOfWeek = 0;

    uint32_t dwStates = 0;
    if (m_iYear == m_iCurYear && m_iMonth == m_iCurMonth && m_iDay == (i + 1))
      dwStates |= FWL_ITEMSTATE_MCD_Flag;
    if (pdfium::ContainsValue(m_arrSelDays, i + 1))
      dwStates |= FWL_ITEMSTATE_MCD_Selected;

    CFX_RectF rtDate;
    m_arrDates.push_back(pdfium::MakeUnique<DATEINFO>(
        i + 1, iDayOfWeek, dwStates, rtDate, WideString::Format(L"%d", i + 1)));
    iDayOfWeek++;
  }
}

void CFWL_MonthCalendar::NextMonth() {
  int32_t iYear = m_iCurYear;
  int32_t iMonth = m_iCurMonth;
  if (iMonth >= 12) {
    iMonth = 1;
    iYear++;
  } else {
    iMonth++;
  }
  DATE dt(m_iCurYear, m_iCurMonth, 1);
  if (!(dt < m_dtMax))
    return;

  m_iCurYear = iYear, m_iCurMonth = iMonth;
  ChangeToMonth(m_iCurYear, m_iCurMonth);
}

void CFWL_MonthCalendar::PrevMonth() {
  int32_t iYear = m_iCurYear;
  int32_t iMonth = m_iCurMonth;
  if (iMonth <= 1) {
    iMonth = 12;
    iYear--;
  } else {
    iMonth--;
  }

  DATE dt(m_iCurYear, m_iCurMonth, 1);
  if (!(dt > m_dtMin))
    return;

  m_iCurYear = iYear, m_iCurMonth = iMonth;
  ChangeToMonth(m_iCurYear, m_iCurMonth);
}

void CFWL_MonthCalendar::ChangeToMonth(int32_t iYear, int32_t iMonth) {
  m_iCurYear = iYear;
  m_iCurMonth = iMonth;
  m_iHovered = -1;

  ClearDateItem();
  ResetDateItem();
  CalDateItem();
  m_wsHead = GetHeadText(m_iCurYear, m_iCurMonth);
}

void CFWL_MonthCalendar::RemoveSelDay() {
  int32_t iDatesCount = pdfium::CollectionSize<int32_t>(m_arrDates);
  for (int32_t iSelDay : m_arrSelDays) {
    if (iSelDay <= iDatesCount)
      m_arrDates[iSelDay - 1]->dwStates &= ~FWL_ITEMSTATE_MCD_Selected;
  }
  m_arrSelDays.clear();
}

void CFWL_MonthCalendar::AddSelDay(int32_t iDay) {
  ASSERT(iDay > 0);
  if (!pdfium::ContainsValue(m_arrSelDays, iDay))
    return;

  RemoveSelDay();
  if (iDay <= pdfium::CollectionSize<int32_t>(m_arrDates))
    m_arrDates[iDay - 1]->dwStates |= FWL_ITEMSTATE_MCD_Selected;

  m_arrSelDays.push_back(iDay);
}

void CFWL_MonthCalendar::JumpToToday() {
  if (m_iYear != m_iCurYear || m_iMonth != m_iCurMonth) {
    m_iCurYear = m_iYear;
    m_iCurMonth = m_iMonth;
    ChangeToMonth(m_iYear, m_iMonth);
    AddSelDay(m_iDay);
    return;
  }

  if (!pdfium::ContainsValue(m_arrSelDays, m_iDay))
    AddSelDay(m_iDay);
}

WideString CFWL_MonthCalendar::GetHeadText(int32_t iYear, int32_t iMonth) {
  ASSERT(iMonth > 0);
  ASSERT(iMonth < 13);

  static const wchar_t* const pMonth[] = {L"January", L"February", L"March",
                                          L"April",   L"May",      L"June",
                                          L"July",    L"August",   L"September",
                                          L"October", L"November", L"December"};
  return WideString::Format(L"%ls, %d", pMonth[iMonth - 1], iYear);
}

WideString CFWL_MonthCalendar::GetTodayText(int32_t iYear,
                                            int32_t iMonth,
                                            int32_t iDay) {
  return WideString::Format(L"Today, %d/%d/%d", iDay, iMonth, iYear);
}

int32_t CFWL_MonthCalendar::GetDayAtPoint(const CFX_PointF& point) const {
  int i = 1;  // one-based day values.
  for (const auto& pDateInfo : m_arrDates) {
    if (pDateInfo->rect.Contains(point))
      return i;
    ++i;
  }
  return -1;
}

CFX_RectF CFWL_MonthCalendar::GetDayRect(int32_t iDay) {
  if (iDay <= 0 || iDay > pdfium::CollectionSize<int32_t>(m_arrDates))
    return CFX_RectF();

  DATEINFO* pDateInfo = m_arrDates[iDay - 1].get();
  return pDateInfo ? pDateInfo->rect : CFX_RectF();
}

void CFWL_MonthCalendar::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus:
    case CFWL_Message::Type::KillFocus:
      GetOuter()->GetDelegate()->OnProcessMessage(pMessage);
      break;
    case CFWL_Message::Type::Key:
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
    default:
      break;
  }
  CFWL_Widget::OnProcessMessage(pMessage);
}

void CFWL_MonthCalendar::OnDrawWidget(CXFA_Graphics* pGraphics,
                                      const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}

void CFWL_MonthCalendar::OnLButtonDown(CFWL_MessageMouse* pMsg) {
  if (m_rtLBtn.Contains(pMsg->m_pos)) {
    m_iLBtnPartStates = CFWL_PartState_Pressed;
    PrevMonth();
    RepaintRect(m_rtClient);
  } else if (m_rtRBtn.Contains(pMsg->m_pos)) {
    m_iRBtnPartStates |= CFWL_PartState_Pressed;
    NextMonth();
    RepaintRect(m_rtClient);
  } else if (m_rtToday.Contains(pMsg->m_pos)) {
    JumpToToday();
    RepaintRect(m_rtClient);
  }
}

void CFWL_MonthCalendar::OnLButtonUp(CFWL_MessageMouse* pMsg) {
  if (m_rtLBtn.Contains(pMsg->m_pos)) {
    m_iLBtnPartStates = 0;
    RepaintRect(m_rtLBtn);
    return;
  }
  if (m_rtRBtn.Contains(pMsg->m_pos)) {
    m_iRBtnPartStates = 0;
    RepaintRect(m_rtRBtn);
    return;
  }
  if (m_rtToday.Contains(pMsg->m_pos))
    return;

  int32_t iOldSel = 0;
  if (!m_arrSelDays.empty())
    iOldSel = m_arrSelDays[0];

  int32_t iCurSel = GetDayAtPoint(pMsg->m_pos);
  if (iCurSel > 0) {
    DATEINFO* lpDatesInfo = m_arrDates[iCurSel - 1].get();
    CFX_RectF rtInvalidate(lpDatesInfo->rect);
    if (iOldSel > 0 && iOldSel <= pdfium::CollectionSize<int32_t>(m_arrDates)) {
      lpDatesInfo = m_arrDates[iOldSel - 1].get();
      rtInvalidate.Union(lpDatesInfo->rect);
    }
    AddSelDay(iCurSel);
    CFWL_DateTimePicker* pDateTime =
        static_cast<CFWL_DateTimePicker*>(m_pOuter);
    pDateTime->ProcessSelChanged(m_iCurYear, m_iCurMonth, iCurSel);
    pDateTime->ShowMonthCalendar(false);
  }
}

void CFWL_MonthCalendar::OnMouseMove(CFWL_MessageMouse* pMsg) {
  bool bRepaint = false;
  CFX_RectF rtInvalidate;
  if (m_rtDates.Contains(pMsg->m_pos)) {
    int32_t iHover = GetDayAtPoint(pMsg->m_pos);
    bRepaint = m_iHovered != iHover;
    if (bRepaint) {
      if (m_iHovered > 0)
        rtInvalidate = GetDayRect(m_iHovered);
      if (iHover > 0) {
        CFX_RectF rtDay = GetDayRect(iHover);
        if (rtInvalidate.IsEmpty())
          rtInvalidate = rtDay;
        else
          rtInvalidate.Union(rtDay);
      }
    }
    m_iHovered = iHover;
  } else {
    bRepaint = m_iHovered > 0;
    if (bRepaint)
      rtInvalidate = GetDayRect(m_iHovered);

    m_iHovered = -1;
  }
  if (bRepaint && !rtInvalidate.IsEmpty())
    RepaintRect(rtInvalidate);
}

void CFWL_MonthCalendar::OnMouseLeave(CFWL_MessageMouse* pMsg) {
  if (m_iHovered <= 0)
    return;

  CFX_RectF rtInvalidate = GetDayRect(m_iHovered);
  m_iHovered = -1;
  if (!rtInvalidate.IsEmpty())
    RepaintRect(rtInvalidate);
}

CFWL_MonthCalendar::DATEINFO::DATEINFO(int32_t day,
                                       int32_t dayofweek,
                                       uint32_t dwSt,
                                       CFX_RectF rc,
                                       const WideString& wsday)
    : iDay(day),
      iDayOfWeek(dayofweek),
      dwStates(dwSt),
      rect(rc),
      wsDay(wsday) {}

CFWL_MonthCalendar::DATEINFO::~DATEINFO() {}
