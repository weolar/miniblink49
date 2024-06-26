// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_comboedit.h"

#include <memory>
#include <utility>

#include "xfa/fde/cfde_texteditengine.h"
#include "xfa/fwl/cfwl_combobox.h"
#include "xfa/fwl/cfwl_messagemouse.h"

CFWL_ComboEdit::CFWL_ComboEdit(
    const CFWL_App* app,
    std::unique_ptr<CFWL_WidgetProperties> properties,
    CFWL_Widget* pOuter)
    : CFWL_Edit(app, std::move(properties), pOuter) {
  m_pOuter = static_cast<CFWL_ComboBox*>(pOuter);
}

void CFWL_ComboEdit::ClearSelected() {
  ClearSelection();
  RepaintRect(GetRTClient());
}

void CFWL_ComboEdit::SetSelected() {
  FlagFocus(true);
  SelectAll();
}

void CFWL_ComboEdit::FlagFocus(bool bSet) {
  if (bSet) {
    m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
    return;
  }

  m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
  HideCaret(nullptr);
}

void CFWL_ComboEdit::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  bool backDefault = true;
  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus: {
      m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
      backDefault = false;
      break;
    }
    case CFWL_Message::Type::KillFocus: {
      m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
      backDefault = false;
      break;
    }
    case CFWL_Message::Type::Mouse: {
      CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
      if ((pMsg->m_dwCmd == FWL_MouseCommand::LeftButtonDown) &&
          ((m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0)) {
        SetSelected();
      }
      break;
    }
    default:
      break;
  }
  if (backDefault)
    CFWL_Edit::OnProcessMessage(pMessage);
}
