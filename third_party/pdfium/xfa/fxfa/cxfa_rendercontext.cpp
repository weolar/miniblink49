// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_rendercontext.h"

#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

CXFA_RenderContext::CXFA_RenderContext(CXFA_FFPageView* pPageView,
                                       const CFX_RectF& clipRect,
                                       const CFX_Matrix& matrix)
    : m_pWidget(nullptr), m_matrix(matrix), m_rtClipRect(clipRect) {
  matrix.GetInverse().TransformRect(m_rtClipRect);

  m_pWidgetIterator = pPageView->CreateWidgetIterator(
      XFA_TRAVERSEWAY_Form,
      XFA_WidgetStatus_Visible | XFA_WidgetStatus_Viewable);
  m_pWidget = m_pWidgetIterator->MoveToNext();
}

CXFA_RenderContext::~CXFA_RenderContext() {}

void CXFA_RenderContext::DoRender(CXFA_Graphics* gs) {
  while (m_pWidget) {
    CFX_RectF rtWidgetBox = m_pWidget->GetBBox(XFA_WidgetStatus_Visible,
                                               CXFA_FFWidget::kDoNotDrawFocus);
    rtWidgetBox.width += 1;
    rtWidgetBox.height += 1;
    if (rtWidgetBox.IntersectWith(m_rtClipRect))
      m_pWidget->RenderWidget(gs, m_matrix, XFA_WidgetStatus_Highlight);

    m_pWidget = m_pWidgetIterator->MoveToNext();
  }
}
