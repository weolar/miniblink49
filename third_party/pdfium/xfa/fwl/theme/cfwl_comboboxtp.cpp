// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_comboboxtp.h"

#include "xfa/fwl/cfwl_combobox.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/ifwl_themeprovider.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

CFWL_ComboBoxTP::CFWL_ComboBoxTP() {}

CFWL_ComboBoxTP::~CFWL_ComboBoxTP() {}

void CFWL_ComboBoxTP::DrawBackground(const CFWL_ThemeBackground& pParams) {
  switch (pParams.m_iPart) {
    case CFWL_Part::Border: {
      DrawBorder(pParams.m_pGraphics.Get(), pParams.m_rtPart, pParams.m_matrix);
      break;
    }
    case CFWL_Part::Background: {
      CXFA_GEPath path;
      const CFX_RectF& rect = pParams.m_rtPart;
      path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
      FX_ARGB argb_color;
      switch (pParams.m_dwStates) {
        case CFWL_PartState_Selected:
          argb_color = FWLTHEME_COLOR_BKSelected;
          break;
        case CFWL_PartState_Disabled:
          argb_color = FWLTHEME_COLOR_EDGERB1;
          break;
        default:
          argb_color = 0xFFFFFFFF;
      }
      pParams.m_pGraphics->SaveGraphState();
      pParams.m_pGraphics->SetFillColor(CXFA_GEColor(argb_color));
      pParams.m_pGraphics->FillPath(&path, FXFILL_WINDING, &pParams.m_matrix);
      pParams.m_pGraphics->RestoreGraphState();
      break;
    }
    case CFWL_Part::DropDownButton: {
      DrawDropDownButton(pParams, pParams.m_dwStates, pParams.m_matrix);
      break;
    }
    case CFWL_Part::StretchHandler: {
      DrawStretchHandler(pParams, 0, pParams.m_matrix);
      break;
    }
    default:
      break;
  }
}

void CFWL_ComboBoxTP::DrawStretchHandler(const CFWL_ThemeBackground& pParams,
                                         uint32_t dwStates,
                                         const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  path.AddRectangle(pParams.m_rtPart.left, pParams.m_rtPart.top,
                    pParams.m_rtPart.width - 1, pParams.m_rtPart.height);
  pParams.m_pGraphics->SetFillColor(CXFA_GEColor(ArgbEncode(0xff, 0xff, 0, 0)));
  pParams.m_pGraphics->FillPath(&path, FXFILL_WINDING, &pParams.m_matrix);
}

void CFWL_ComboBoxTP::DrawDropDownButton(const CFWL_ThemeBackground& pParams,
                                         uint32_t dwStates,
                                         const CFX_Matrix& matrix) {
  FWLTHEME_STATE eState = FWLTHEME_STATE_Normal;
  switch (dwStates) {
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
               FWLTHEME_DIRECTION_Down, eState, pParams.m_matrix);
}
