// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_carettp.h"

#include "xfa/fwl/cfwl_caret.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

CFWL_CaretTP::CFWL_CaretTP() {}
CFWL_CaretTP::~CFWL_CaretTP() {}

void CFWL_CaretTP::DrawBackground(const CFWL_ThemeBackground& pParams) {
  switch (pParams.m_iPart) {
    case CFWL_Part::Background: {
      if (!(pParams.m_dwStates & CFWL_PartState_HightLight))
        return;

      DrawCaretBK(pParams.m_pGraphics.Get(), pParams.m_dwStates,
                  pParams.m_rtPart, pParams.m_matrix);
      break;
    }
    default:
      break;
  }
}

void CFWL_CaretTP::DrawCaretBK(CXFA_Graphics* pGraphics,
                               uint32_t dwStates,
                               const CFX_RectF& rect,
                               const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
  pGraphics->SetFillColor(CXFA_GEColor(ArgbEncode(255, 0, 0, 0)));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
}
