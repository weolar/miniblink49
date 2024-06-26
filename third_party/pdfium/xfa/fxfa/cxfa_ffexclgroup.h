// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFEXCLGROUP_H_
#define XFA_FXFA_CXFA_FFEXCLGROUP_H_

#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"

class CXFA_FFExclGroup final : public CXFA_FFWidget {
 public:
  explicit CXFA_FFExclGroup(CXFA_Node* pNode);
  ~CXFA_FFExclGroup() override;

  // CXFA_FFWidget
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;
};

#endif  // XFA_FXFA_CXFA_FFEXCLGROUP_H_
