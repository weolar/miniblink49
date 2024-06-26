// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFSIGNATURE_H_
#define XFA_FXFA_CXFA_FFSIGNATURE_H_

#include "xfa/fxfa/cxfa_fffield.h"

class CXFA_FFSignature final : public CXFA_FFField {
 public:
  explicit CXFA_FFSignature(CXFA_Node* pNode);
  ~CXFA_FFSignature() override;

  // CXFA_FFField
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;
  bool LoadWidget() override;
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
                    const CFX_PointF& pointy) override;
  void OnRButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnRButtonUp(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnRButtonDblClk(uint32_t dwFlags, const CFX_PointF& point) override;

  bool OnKeyDown(uint32_t dwKeyCode, uint32_t dwFlags) override;
  bool OnKeyUp(uint32_t dwKeyCode, uint32_t dwFlags) override;
  bool OnChar(uint32_t dwChar, uint32_t dwFlags) override;
  FWL_WidgetHit OnHitTest(const CFX_PointF& point) override;
  bool OnSetCursor(const CFX_PointF& point) override;
  FormFieldType GetFormFieldType() override;
};

#endif  // XFA_FXFA_CXFA_FFSIGNATURE_H_
