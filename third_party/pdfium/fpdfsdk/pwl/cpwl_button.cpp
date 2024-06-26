// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_button.h"

#include <utility>

#include "fpdfsdk/pwl/cpwl_wnd.h"

CPWL_Button::CPWL_Button(const CreateParams& cp,
                         std::unique_ptr<PrivateData> pAttachedData)
    : CPWL_Wnd(cp, std::move(pAttachedData)) {
  GetCreationParams()->eCursorType = FXCT_HAND;
}

CPWL_Button::~CPWL_Button() = default;

bool CPWL_Button::OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnLButtonDown(point, nFlag);
  m_bMouseDown = true;
  SetCapture();
  return true;
}

bool CPWL_Button::OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) {
  CPWL_Wnd::OnLButtonUp(point, nFlag);
  ReleaseCapture();
  m_bMouseDown = false;
  return true;
}
