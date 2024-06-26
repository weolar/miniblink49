// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFCHECKBUTTON_H_
#define XFA_FXFA_CXFA_FFCHECKBUTTON_H_

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/cxfa_fffield.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_CheckButton;

class CXFA_FFCheckButton final : public CXFA_FFField {
 public:
  CXFA_FFCheckButton(CXFA_Node* pNode, CXFA_CheckButton* button);
  ~CXFA_FFCheckButton() override;

  // CXFA_FFField
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;

  bool LoadWidget() override;
  bool PerformLayout() override;
  bool UpdateFWLData() override;
  void UpdateWidgetProperty() override;
  bool OnLButtonUp(uint32_t dwFlags, const CFX_PointF& point) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;
  FormFieldType GetFormFieldType() override;

  void SetFWLCheckState(XFA_CHECKSTATE eCheckState);

 private:
  bool CommitData() override;
  bool IsDataChanged() override;
  void CapLeftRightPlacement(const CXFA_Margin* captionMargin);
  void AddUIMargin(XFA_AttributeValue iCapPlacement);
  XFA_CHECKSTATE FWLState2XFAState();

  UnownedPtr<IFWL_WidgetDelegate> m_pOldDelegate;
  CFX_RectF m_rtCheckBox;
  UnownedPtr<CXFA_CheckButton> button_;
};

#endif  // XFA_FXFA_CXFA_FFCHECKBUTTON_H_
