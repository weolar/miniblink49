// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFTEXT_H_
#define XFA_FXFA_CXFA_FFTEXT_H_

#include "xfa/fxfa/cxfa_ffwidget.h"

class CXFA_FFText final : public CXFA_FFWidget {
 public:
  explicit CXFA_FFText(CXFA_Node* pNode);
  ~CXFA_FFText() override;

  // CXFA_FFWidget
  bool AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                const CFX_PointF& point,
                                FWL_MouseCommand command) override;
  void OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnLButtonUp(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnMouseMove(uint32_t dwFlags, const CFX_PointF& point) override;
  FWL_WidgetHit OnHitTest(const CFX_PointF& point) override;
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;
  bool IsLoaded() override;
  bool PerformLayout() override;

 private:
  const wchar_t* GetLinkURLAtPoint(const CFX_PointF& point);
};

#endif  // XFA_FXFA_CXFA_FFTEXT_H_
