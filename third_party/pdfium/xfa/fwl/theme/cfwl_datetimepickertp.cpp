// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_datetimepickertp.h"

#include "xfa/fwl/cfwl_datetimepicker.h"
#include "xfa/fwl/cfwl_themebackground.h"

CFWL_DateTimePickerTP::CFWL_DateTimePickerTP() {}

CFWL_DateTimePickerTP::~CFWL_DateTimePickerTP() {}

void CFWL_DateTimePickerTP::DrawBackground(
    const CFWL_ThemeBackground& pParams) {
  switch (pParams.m_iPart) {
    case CFWL_Part::Border:
      DrawBorder(pParams.m_pGraphics.Get(), pParams.m_rtPart, pParams.m_matrix);
      break;
    case CFWL_Part::DropDownButton:
      DrawDropDownButton(pParams, pParams.m_matrix);
      break;
    default:
      break;
  }
}

void CFWL_DateTimePickerTP::DrawDropDownButton(
    const CFWL_ThemeBackground& pParams,
    const CFX_Matrix& matrix) {
  uint32_t dwStates = pParams.m_dwStates;
  dwStates &= 0x03;
  FWLTHEME_STATE eState = FWLTHEME_STATE_Normal;
  switch (eState & dwStates) {
    case CFWL_PartState_Normal: {
      eState = FWLTHEME_STATE_Normal;
      break;
    }
    case CFWL_PartState_Hovered: {
      eState = FWLTHEME_STATE_Hover;
      break;
    }
    case CFWL_PartState_Pressed: {
      eState = FWLTHEME_STATE_Pressed;
      break;
    }
    case CFWL_PartState_Disabled: {
      eState = FWLTHEME_STATE_Disable;
      break;
    }
    default:
      break;
  }
  DrawArrowBtn(pParams.m_pGraphics.Get(), pParams.m_rtPart,
               FWLTHEME_DIRECTION_Down, eState, matrix);
}
