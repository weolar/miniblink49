// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_CARET_H_
#define XFA_FWL_CFWL_CARET_H_

#include <memory>

#include "xfa/fwl/cfwl_timer.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"

class CFWL_WidgetProperties;
class CFWL_Widget;

class CFWL_Caret final : public CFWL_Widget {
 public:
  CFWL_Caret(const CFWL_App* app,
             std::unique_ptr<CFWL_WidgetProperties> properties,
             CFWL_Widget* pOuter);
  ~CFWL_Caret() override;

  // CFWL_Widget
  FWL_Type GetClassID() const override;
  void DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;
  void Update() override;

  void ShowCaret();
  void HideCaret();

 private:
  class Timer final : public CFWL_Timer {
   public:
    explicit Timer(CFWL_Caret* pCaret);
    ~Timer() override {}

    void Run(CFWL_TimerInfo* hTimer) override;
  };
  friend class CFWL_Caret::Timer;

  void DrawCaretBK(CXFA_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);

  std::unique_ptr<CFWL_Caret::Timer> m_pTimer;
  UnownedPtr<CFWL_TimerInfo> m_pTimerInfo;
};

#endif  // XFA_FWL_CFWL_CARET_H_
