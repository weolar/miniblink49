// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_special_button.h"

#include <utility>

#include "fpdfsdk/pwl/cpwl_button.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"

CPWL_PushButton::CPWL_PushButton(const CreateParams& cp,
                                 std::unique_ptr<PrivateData> pAttachedData)
    : CPWL_Button(cp, std::move(pAttachedData)) {}

CPWL_PushButton::~CPWL_PushButton() = default;

CFX_FloatRect CPWL_PushButton::GetFocusRect() const {
  return GetWindowRect().GetDeflated(static_cast<float>(GetBorderWidth()),
                                     static_cast<float>(GetBorderWidth()));
}

CPWL_CheckBox::CPWL_CheckBox(const CreateParams& cp,
                             std::unique_ptr<PrivateData> pAttachedData)
    : CPWL_Button(cp, std::move(pAttachedData)) {}

CPWL_CheckBox::~CPWL_CheckBox() = default;

bool CPWL_CheckBox::OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) {
  if (IsReadOnly())
    return false;

  SetCheck(!IsChecked());
  return true;
}

bool CPWL_CheckBox::OnChar(uint16_t nChar, uint32_t nFlag) {
  SetCheck(!IsChecked());
  return true;
}

CPWL_RadioButton::CPWL_RadioButton(const CreateParams& cp,
                                   std::unique_ptr<PrivateData> pAttachedData)
    : CPWL_Button(cp, std::move(pAttachedData)) {}

CPWL_RadioButton::~CPWL_RadioButton() = default;

bool CPWL_RadioButton::OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) {
  if (IsReadOnly())
    return false;

  SetCheck(true);
  return true;
}

bool CPWL_RadioButton::OnChar(uint16_t nChar, uint32_t nFlag) {
  SetCheck(true);
  return true;
}
