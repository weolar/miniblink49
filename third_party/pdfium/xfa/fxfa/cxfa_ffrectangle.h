// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFRECTANGLE_H_
#define XFA_FXFA_CXFA_FFRECTANGLE_H_

#include "xfa/fxfa/cxfa_ffwidget.h"

class CXFA_FFRectangle final : public CXFA_FFWidget {
 public:
  explicit CXFA_FFRectangle(CXFA_Node* pNode);
  ~CXFA_FFRectangle() override;

  // CXFA_FFWidget
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;
};

#endif  // XFA_FXFA_CXFA_FFRECTANGLE_H_
