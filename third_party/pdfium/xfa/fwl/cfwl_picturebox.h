// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_PICTUREBOX_H_
#define XFA_FWL_CFWL_PICTUREBOX_H_

#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetproperties.h"

class CFX_DIBitmap;
class CFWL_Widget;

class CFWL_PictureBox final : public CFWL_Widget {
 public:
  explicit CFWL_PictureBox(const CFWL_App* pApp);
  ~CFWL_PictureBox() override;

  // CFWL_Widget
  FWL_Type GetClassID() const override;
  void Update() override;
  void DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

 private:
  CFX_RectF m_rtClient;
  CFX_RectF m_rtImage;
  CFX_Matrix m_matrix;
};

#endif  // XFA_FWL_CFWL_PICTUREBOX_H_
