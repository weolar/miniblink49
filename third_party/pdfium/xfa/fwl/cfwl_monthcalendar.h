// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_MONTHCALENDAR_H_
#define XFA_FWL_CFWL_MONTHCALENDAR_H_

#include <memory>
#include <vector>

#include "core/fxcrt/cfx_datetime.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetproperties.h"

#define FWL_ITEMSTATE_MCD_Flag (1L << 0)
#define FWL_ITEMSTATE_MCD_Selected (1L << 1)

class CFWL_MessageMouse;

class CFWL_MonthCalendar final : public CFWL_Widget {
 public:
  CFWL_MonthCalendar(const CFWL_App* app,
                     std::unique_ptr<CFWL_WidgetProperties> properties,
                     CFWL_Widget* pOuter);
  ~CFWL_MonthCalendar() override;

  // FWL_WidgetImp
  FWL_Type GetClassID() const override;
  CFX_RectF GetAutosizedWidgetRect() override;
  void Update() override;
  void DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

  void SetSelect(int32_t iYear, int32_t iMonth, int32_t iDay);

 private:
  struct DATE {
    DATE() : iYear(0), iMonth(0), iDay(0) {}

    DATE(int32_t year, int32_t month, int32_t day)
        : iYear(year), iMonth(month), iDay(day) {}

    bool operator<(const DATE& right) {
      if (iYear < right.iYear)
        return true;
      if (iYear == right.iYear) {
        if (iMonth < right.iMonth)
          return true;
        if (iMonth == right.iMonth)
          return iDay < right.iDay;
      }
      return false;
    }

    bool operator>(const DATE& right) {
      if (iYear > right.iYear)
        return true;
      if (iYear == right.iYear) {
        if (iMonth > right.iMonth)
          return true;
        if (iMonth == right.iMonth)
          return iDay > right.iDay;
      }
      return false;
    }

    int32_t iYear;
    int32_t iMonth;
    int32_t iDay;
  };
  struct DATEINFO {
    DATEINFO(int32_t day,
             int32_t dayofweek,
             uint32_t dwSt,
             CFX_RectF rc,
             const WideString& wsday);
    ~DATEINFO();

    int32_t iDay;
    int32_t iDayOfWeek;
    uint32_t dwStates;
    CFX_RectF rect;
    WideString wsDay;
  };

  void DrawBackground(CXFA_Graphics* pGraphics,
                      IFWL_ThemeProvider* pTheme,
                      const CFX_Matrix* pMatrix);
  void DrawHeadBK(CXFA_Graphics* pGraphics,
                  IFWL_ThemeProvider* pTheme,
                  const CFX_Matrix* pMatrix);
  void DrawLButton(CXFA_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawRButton(CXFA_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawCaption(CXFA_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawSeperator(CXFA_Graphics* pGraphics,
                     IFWL_ThemeProvider* pTheme,
                     const CFX_Matrix* pMatrix);
  void DrawDatesInBK(CXFA_Graphics* pGraphics,
                     IFWL_ThemeProvider* pTheme,
                     const CFX_Matrix* pMatrix);
  void DrawWeek(CXFA_Graphics* pGraphics,
                IFWL_ThemeProvider* pTheme,
                const CFX_Matrix* pMatrix);
  void DrawToday(CXFA_Graphics* pGraphics,
                 IFWL_ThemeProvider* pTheme,
                 const CFX_Matrix* pMatrix);
  void DrawDatesIn(CXFA_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawDatesOut(CXFA_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    const CFX_Matrix* pMatrix);
  void DrawDatesInCircle(CXFA_Graphics* pGraphics,
                         IFWL_ThemeProvider* pTheme,
                         const CFX_Matrix* pMatrix);
  CFX_SizeF CalcSize();
  void Layout();
  void CalcHeadSize();
  void CalcTodaySize();
  void CalDateItem();
  void GetCapValue();
  void InitDate();
  void ClearDateItem();
  void ResetDateItem();
  void NextMonth();
  void PrevMonth();
  void ChangeToMonth(int32_t iYear, int32_t iMonth);
  void RemoveSelDay();
  void AddSelDay(int32_t iDay);
  void JumpToToday();
  WideString GetHeadText(int32_t iYear, int32_t iMonth);
  WideString GetTodayText(int32_t iYear, int32_t iMonth, int32_t iDay);
  int32_t GetDayAtPoint(const CFX_PointF& point) const;
  CFX_RectF GetDayRect(int32_t iDay);
  void OnLButtonDown(CFWL_MessageMouse* pMsg);
  void OnLButtonUp(CFWL_MessageMouse* pMsg);
  void OnMouseMove(CFWL_MessageMouse* pMsg);
  void OnMouseLeave(CFWL_MessageMouse* pMsg);

  bool m_bInitialized = false;
  CFX_RectF m_rtHead;
  CFX_RectF m_rtWeek;
  CFX_RectF m_rtLBtn;
  CFX_RectF m_rtRBtn;
  CFX_RectF m_rtDates;
  CFX_RectF m_rtHSep;
  CFX_RectF m_rtHeadText;
  CFX_RectF m_rtToday;
  CFX_RectF m_rtTodayFlag;
  WideString m_wsHead;
  WideString m_wsToday;
  std::vector<std::unique_ptr<DATEINFO>> m_arrDates;
  int32_t m_iCurYear = 2011;
  int32_t m_iCurMonth = 1;
  int32_t m_iYear = 2011;
  int32_t m_iMonth = 1;
  int32_t m_iDay = 1;
  int32_t m_iHovered = -1;
  int32_t m_iLBtnPartStates = CFWL_PartState_Normal;
  int32_t m_iRBtnPartStates = CFWL_PartState_Normal;
  DATE m_dtMin;
  DATE m_dtMax;
  CFX_SizeF m_szHead;
  CFX_SizeF m_szCell;
  CFX_SizeF m_szToday;
  std::vector<int32_t> m_arrSelDays;
  CFX_RectF m_rtClient;
};

#endif  // XFA_FWL_CFWL_MONTHCALENDAR_H_
