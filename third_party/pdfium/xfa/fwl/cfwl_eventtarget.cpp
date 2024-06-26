// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_eventtarget.h"

#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/ifwl_widgetdelegate.h"

CFWL_EventTarget::CFWL_EventTarget(CFWL_Widget* pListener)
    : m_pListener(pListener), m_bValid(true) {}

CFWL_EventTarget::~CFWL_EventTarget() {}

void CFWL_EventTarget::SetEventSource(CFWL_Widget* pSource) {
  if (pSource)
    m_widgets.insert(pSource);
}

bool CFWL_EventTarget::ProcessEvent(CFWL_Event* pEvent) {
  IFWL_WidgetDelegate* pDelegate = m_pListener->GetDelegate();
  if (!pDelegate)
    return false;
  if (!m_widgets.empty() && m_widgets.count(pEvent->GetSrcTarget()) == 0)
    return false;

  pDelegate->OnProcessEvent(pEvent);
  return true;
}
