// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFIMAGE_H_
#define XFA_FXFA_CXFA_FFIMAGE_H_

#include "xfa/fxfa/cxfa_ffwidget.h"

class CXFA_FFImage final : public CXFA_FFWidget {
 public:
  explicit CXFA_FFImage(CXFA_Node* pNode);
  ~CXFA_FFImage() override;

  // CXFA_FFWidget
  void RenderWidget(CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    uint32_t dwStatus) override;
  bool IsLoaded() override;
  bool LoadWidget() override;
};

#endif  // XFA_FXFA_CXFA_FFIMAGE_H_
