// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFFIELD_H_
#define XFA_FXFA_CXFA_FFFIELD_H_

#include <memory>

#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/ifwl_widgetdelegate.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"

class CXFA_Node;

#define XFA_MINUI_HEIGHT 4.32f
#define XFA_DEFAULTUI_HEIGHT 2.0f

class CXFA_FFField : public CXFA_FFWidget, public IFWL_WidgetDelegate {
 public:
  explicit CXFA_FFField(CXFA_Node* pNode);
  ~CXFA_FFField() override;

  // CXFA_FFWidget
  CFX_RectF GetBBox(uint32_t dwStatus, FocusOption focus) override;
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;
  bool IsLoaded() override;
  bool LoadWidget() override;
  bool PerformLayout() override;
  bool OnMouseEnter() override;
  bool OnMouseExit() override;
  bool AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                const CFX_PointF& point,
                                FWL_MouseCommand command) override;
  void OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnLButtonUp(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnLButtonDblClk(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnMouseMove(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnMouseWheel(uint32_t dwFlags,
                    int16_t zDelta,
                    const CFX_PointF& point) override;
  void OnRButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnRButtonUp(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnRButtonDblClk(uint32_t dwFlags, const CFX_PointF& point) override;

  bool OnSetFocus(CXFA_FFWidget* pOldWidget) override;
  bool OnKillFocus(CXFA_FFWidget* pNewWidget) override;
  bool OnKeyDown(uint32_t dwKeyCode, uint32_t dwFlags) override;
  bool OnKeyUp(uint32_t dwKeyCode, uint32_t dwFlags) override;
  bool OnChar(uint32_t dwChar, uint32_t dwFlags) override;
  FWL_WidgetHit OnHitTest(const CFX_PointF& point) override;
  bool OnSetCursor(const CFX_PointF& point) override;

  // IFWL_WidgetDelegate
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

  void UpdateFWL();
  uint32_t UpdateUIProperty();

 protected:
  bool PtInActiveRect(const CFX_PointF& point) override;

  virtual void SetFWLRect();
  void SetFWLThemeProvider();
  CFWL_Widget* GetNormalWidget() { return m_pNormalWidget.get(); }
  CFX_PointF FWLToClient(const CFX_PointF& point);
  void LayoutCaption();
  void RenderCaption(CXFA_Graphics* pGS, CFX_Matrix* pMatrix);

  int32_t CalculateOverride();
  int32_t CalculateNode(CXFA_Node* pNode);
  bool ProcessCommittedData();
  virtual bool CommitData();
  virtual bool IsDataChanged();
  void DrawHighlight(CXFA_Graphics* pGS,
                     CFX_Matrix* pMatrix,
                     uint32_t dwStatus,
                     bool bEllipse);
  void DrawFocus(CXFA_Graphics* pGS, CFX_Matrix* pMatrix);
  void TranslateFWLMessage(CFWL_Message* pMessage);
  void CapPlacement();
  void CapTopBottomPlacement(const CXFA_Margin* margin,
                             const CFX_RectF& rtWidget,
                             XFA_AttributeValue iCapPlacement);
  void CapLeftRightPlacement(const CXFA_Margin* margin,
                             const CFX_RectF& rtWidget,
                             XFA_AttributeValue iCapPlacement);
  void SetEditScrollOffset();

  std::unique_ptr<CFWL_Widget> m_pNormalWidget;
  CFX_RectF m_rtUI;
  CFX_RectF m_rtCaption;
};

#endif  // XFA_FXFA_CXFA_FFFIELD_H_
