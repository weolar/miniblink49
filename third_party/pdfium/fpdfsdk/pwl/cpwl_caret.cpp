// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_caret.h"

#include <sstream>
#include <utility>

#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"

CPWL_Caret::CPWL_Caret(const CreateParams& cp,
                       std::unique_ptr<PrivateData> pAttachedData)
    : CPWL_Wnd(cp, std::move(pAttachedData)) {}

CPWL_Caret::~CPWL_Caret() = default;

void CPWL_Caret::DrawThisAppearance(CFX_RenderDevice* pDevice,
                                    const CFX_Matrix& mtUser2Device) {
  if (!IsVisible() || !m_bFlash)
    return;

  CFX_FloatRect rcRect = GetCaretRect();
  CFX_FloatRect rcClip = GetClipRect();
  CFX_PathData path;

  float fCaretX = rcRect.left + m_fWidth * 0.5f;
  float fCaretTop = rcRect.top;
  float fCaretBottom = rcRect.bottom;
  if (!rcClip.IsEmpty()) {
    rcRect.Intersect(rcClip);
    if (rcRect.IsEmpty())
      return;

    fCaretTop = rcRect.top;
    fCaretBottom = rcRect.bottom;
  }

  path.AppendPoint(CFX_PointF(fCaretX, fCaretBottom), FXPT_TYPE::MoveTo, false);
  path.AppendPoint(CFX_PointF(fCaretX, fCaretTop), FXPT_TYPE::LineTo, false);

  CFX_GraphStateData gsd;
  gsd.m_LineWidth = m_fWidth;
  pDevice->DrawPath(&path, &mtUser2Device, &gsd, 0, ArgbEncode(255, 0, 0, 0),
                    FXFILL_ALTERNATE);
}

void CPWL_Caret::TimerProc() {
  m_bFlash = !m_bFlash;
  InvalidateRect(nullptr);
  // Note, |this| may no longer be viable at this point. If more work needs
  // to be done, add an observer.
}

CFX_FloatRect CPWL_Caret::GetCaretRect() const {
  return CFX_FloatRect(m_ptFoot.x, m_ptFoot.y, m_ptHead.x + m_fWidth,
                       m_ptHead.y);
}

void CPWL_Caret::SetCaret(bool bVisible,
                          const CFX_PointF& ptHead,
                          const CFX_PointF& ptFoot) {
  if (!bVisible) {
    m_ptHead = CFX_PointF();
    m_ptFoot = CFX_PointF();
    m_bFlash = false;
    if (!IsVisible())
      return;

    EndTimer();
    CPWL_Wnd::SetVisible(false);
    // Note, |this| may no longer be viable at this point. If more work needs
    // to be done, check the return value of SetVisible().
    return;
  }

  if (!IsVisible()) {
    static constexpr int32_t kCaretFlashIntervalMs = 500;

    m_ptHead = ptHead;
    m_ptFoot = ptFoot;
    EndTimer();
    BeginTimer(kCaretFlashIntervalMs);

    if (!CPWL_Wnd::SetVisible(true))
      return;

    m_bFlash = true;
    Move(m_rcInvalid, false, true);
    // Note, |this| may no longer be viable at this point. If more work needs
    // to be done, check the return value of Move().
    return;
  }

  if (m_ptHead == ptHead && m_ptFoot == ptFoot)
    return;

  m_ptHead = ptHead;
  m_ptFoot = ptFoot;
  m_bFlash = true;
  Move(m_rcInvalid, false, true);
  // Note, |this| may no longer be viable at this point. If more work
  // needs to be done, check the return value of Move().
}

bool CPWL_Caret::InvalidateRect(CFX_FloatRect* pRect) {
  if (!pRect) {
    return CPWL_Wnd::InvalidateRect(nullptr);
  }

  CFX_FloatRect rcRefresh = *pRect;
  if (!rcRefresh.IsEmpty()) {
    rcRefresh.Inflate(0.5f, 0.5f);
    rcRefresh.Normalize();
  }
  rcRefresh.top += 1;
  rcRefresh.bottom -= 1;
  return CPWL_Wnd::InvalidateRect(&rcRefresh);
}

bool CPWL_Caret::SetVisible(bool bVisible) {
  return true;
}
