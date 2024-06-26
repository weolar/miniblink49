// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_datetimeedit.h"

#include <memory>
#include <utility>

#include "xfa/fwl/cfwl_datetimepicker.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_widgetmgr.h"

CFWL_DateTimeEdit::CFWL_DateTimeEdit(
    const CFWL_App* app,
    std::unique_ptr<CFWL_WidgetProperties> properties,
    CFWL_Widget* pOuter)
    : CFWL_Edit(app, std::move(properties), pOuter) {}

void CFWL_DateTimeEdit::OnProcessMessage(CFWL_Message* pMessage) {
  if (pMessage->GetType() != CFWL_Message::Type::Mouse) {
    CFWL_Edit::OnProcessMessage(pMessage);
    return;
  }

  CFWL_MessageMouse* pMouse = static_cast<CFWL_MessageMouse*>(pMessage);
  if (pMouse->m_dwCmd == FWL_MouseCommand::LeftButtonDown ||
      pMouse->m_dwCmd == FWL_MouseCommand::RightButtonDown) {
    if ((m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0)
      m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;

    CFWL_DateTimePicker* pDateTime =
        static_cast<CFWL_DateTimePicker*>(m_pOuter);
    if (pDateTime->IsMonthCalendarVisible()) {
      CFX_RectF rtInvalidate = pDateTime->GetWidgetRect();
      pDateTime->ShowMonthCalendar(false);
      rtInvalidate.Offset(-rtInvalidate.left, -rtInvalidate.top);
      pDateTime->RepaintRect(rtInvalidate);
    }
  }
  CFWL_Edit::OnProcessMessage(pMessage);
}
