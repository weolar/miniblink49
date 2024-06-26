// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFIMAGEEDIT_H_
#define XFA_FXFA_CXFA_FFIMAGEEDIT_H_

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/cxfa_fffield.h"

class CXFA_FFImageEdit final : public CXFA_FFField {
 public:
  explicit CXFA_FFImageEdit(CXFA_Node* pNode);
  ~CXFA_FFImageEdit() override;

  // CXFA_FFField
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;
  bool LoadWidget() override;
  bool AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                const CFX_PointF& point,
                                FWL_MouseCommand command) override;
  void OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;
  FormFieldType GetFormFieldType() override;

 private:
  void SetFWLRect() override;
  bool UpdateFWLData() override;
  bool CommitData() override;

  UnownedPtr<IFWL_WidgetDelegate> m_pOldDelegate;
};

#endif  // XFA_FXFA_CXFA_FFIMAGEEDIT_H_
