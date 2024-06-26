// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_SCROLLBAR_H_
#define XFA_FWL_CFWL_SCROLLBAR_H_

#include <memory>

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fwl/cfwl_eventscroll.h"
#include "xfa/fwl/cfwl_timer.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetproperties.h"

class CFWL_Widget;

#define FWL_STYLEEXT_SCB_Horz (0L << 0)
#define FWL_STYLEEXT_SCB_Vert (1L << 0)

class CFWL_ScrollBar final : public CFWL_Widget {
 public:
  CFWL_ScrollBar(const CFWL_App* app,
                 std::unique_ptr<CFWL_WidgetProperties> properties,
                 CFWL_Widget* pOuter);
  ~CFWL_ScrollBar() override;

  // CFWL_Widget
  FWL_Type GetClassID() const override;
  void Update() override;
  void DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

  void GetRange(float* fMin, float* fMax) const {
    ASSERT(fMin);
    ASSERT(fMax);
    *fMin = m_fRangeMin;
    *fMax = m_fRangeMax;
  }
  void SetRange(float fMin, float fMax) {
    m_fRangeMin = fMin;
    m_fRangeMax = fMax;
  }
  float GetPageSize() const { return m_fPageSize; }
  void SetPageSize(float fPageSize) { m_fPageSize = fPageSize; }
  float GetStepSize() const { return m_fStepSize; }
  void SetStepSize(float fStepSize) { m_fStepSize = fStepSize; }
  float GetPos() const { return m_fPos; }
  void SetPos(float fPos) { m_fPos = fPos; }
  void SetTrackPos(float fTrackPos);

 private:
  class Timer final : public CFWL_Timer {
   public:
    explicit Timer(CFWL_ScrollBar* pToolTip);
    ~Timer() override {}

    void Run(CFWL_TimerInfo* pTimerInfo) override;
  };
  friend class CFWL_ScrollBar::Timer;

  bool IsVertical() const {
    return !!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_SCB_Vert);
  }
  void DrawTrack(CXFA_Graphics* pGraphics,
                 IFWL_ThemeProvider* pTheme,
                 bool bLower,
                 const CFX_Matrix* pMatrix);
  void DrawArrowBtn(CXFA_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    bool bMinBtn,
                    const CFX_Matrix* pMatrix);
  void DrawThumb(CXFA_Graphics* pGraphics,
                 IFWL_ThemeProvider* pTheme,
                 const CFX_Matrix* pMatrix);
  void Layout();
  void CalcButtonLen();
  CFX_RectF CalcMinButtonRect();
  CFX_RectF CalcMaxButtonRect();
  CFX_RectF CalcThumbButtonRect(const CFX_RectF& rtThumbRect);
  CFX_RectF CalcMinTrackRect(const CFX_RectF& rtMinRect);
  CFX_RectF CalcMaxTrackRect(const CFX_RectF& rtMaxRect);
  float GetTrackPointPos(const CFX_PointF& point);

  bool SendEvent();
  bool OnScroll(CFWL_EventScroll::Code dwCode, float fPos);
  void OnLButtonDown(const CFX_PointF& point);
  void OnLButtonUp(const CFX_PointF& point);
  void OnMouseMove(const CFX_PointF& point);
  void OnMouseLeave();
  void OnMouseWheel(const CFX_PointF& delta);
  bool DoScroll(CFWL_EventScroll::Code dwCode, float fPos);
  void DoMouseDown(int32_t iItem,
                   const CFX_RectF& rtItem,
                   int32_t& iState,
                   const CFX_PointF& point);
  void DoMouseUp(int32_t iItem,
                 const CFX_RectF& rtItem,
                 int32_t& iState,
                 const CFX_PointF& point);
  void DoMouseMove(int32_t iItem,
                   const CFX_RectF& rtItem,
                   int32_t& iState,
                   const CFX_PointF& point);
  void DoMouseLeave(int32_t iItem, const CFX_RectF& rtItem, int32_t& iState);
  void DoMouseHover(int32_t iItem, const CFX_RectF& rtItem, int32_t& iState);

  UnownedPtr<CFWL_TimerInfo> m_pTimerInfo;
  float m_fRangeMin = 0.0f;
  float m_fRangeMax = -1.0f;
  float m_fPageSize = 0.0f;
  float m_fStepSize = 0.0f;
  float m_fPos = 0.0f;
  float m_fTrackPos = 0.0f;
  int32_t m_iMinButtonState = CFWL_PartState_Normal;
  int32_t m_iMaxButtonState = CFWL_PartState_Normal;
  int32_t m_iThumbButtonState = CFWL_PartState_Normal;
  int32_t m_iMinTrackState = CFWL_PartState_Normal;
  int32_t m_iMaxTrackState = CFWL_PartState_Normal;
  float m_fLastTrackPos = 0.0f;
  CFX_PointF m_cpTrackPoint;
  int32_t m_iMouseWheel = 0;
  float m_fButtonLen = 0.0f;
  bool m_bMouseDown = false;
  bool m_bMinSize = false;
  CFX_RectF m_rtClient;
  CFX_RectF m_rtThumb;
  CFX_RectF m_rtMinBtn;
  CFX_RectF m_rtMaxBtn;
  CFX_RectF m_rtMinTrack;
  CFX_RectF m_rtMaxTrack;
  CFWL_ScrollBar::Timer m_Timer;
};

#endif  // XFA_FWL_CFWL_SCROLLBAR_H_
