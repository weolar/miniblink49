// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_PUSHBUTTON_H_
#define XFA_FWL_CFWL_PUSHBUTTON_H_

#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetproperties.h"

#define FWL_STATE_PSB_Hovered (1 << FWL_WGTSTATE_MAX)
#define FWL_STATE_PSB_Pressed (1 << (FWL_WGTSTATE_MAX + 1))

class CFWL_MessageMouse;
class CFX_DIBitmap;
class CFWL_Widget;

class CFWL_PushButton final : public CFWL_Widget {
 public:
  explicit CFWL_PushButton(const CFWL_App*);
  ~CFWL_PushButton() override;

  // CFWL_Widget
  FWL_Type GetClassID() const override;
  void SetStates(uint32_t dwStates) override;
  void Update() override;
  void DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

 private:
  void DrawBkground(CXFA_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    const CFX_Matrix* pMatrix);
  uint32_t GetPartStates();
  void UpdateTextOutStyles();
  void OnFocusChanged(CFWL_Message* pMsg, bool bSet);
  void OnLButtonDown(CFWL_MessageMouse* pMsg);
  void OnLButtonUp(CFWL_MessageMouse* pMsg);
  void OnMouseMove(CFWL_MessageMouse* pMsg);
  void OnMouseLeave(CFWL_MessageMouse* pMsg);
  void OnKeyDown(CFWL_MessageKey* pMsg);

  CFX_RectF m_rtClient;
  CFX_RectF m_rtCaption;
  bool m_bBtnDown;
};

#endif  // XFA_FWL_CFWL_PUSHBUTTON_H_
