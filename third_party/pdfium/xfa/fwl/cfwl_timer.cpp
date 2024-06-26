// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_timer.h"

#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_timerinfo.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/ifwl_adaptertimermgr.h"
#include "xfa/fxfa/cxfa_ffapp.h"

CFWL_Timer::CFWL_Timer(CFWL_Widget* parent) : m_pWidget(parent) {}

CFWL_Timer::~CFWL_Timer() {}

CFWL_TimerInfo* CFWL_Timer::StartTimer(uint32_t dwElapse, bool bImmediately) {
  CXFA_FFApp* pAdapterNative = m_pWidget->GetOwnerApp()->GetAdapterNative();
  if (!pAdapterNative)
    return nullptr;

//   if (!m_pAdapterTimerMgr)
//     m_pAdapterTimerMgr = pAdapterNative->NewTimerMgr();
  DebugBreak();

  if (!m_pAdapterTimerMgr)
    return nullptr;

  CFWL_TimerInfo* pTimerInfo = nullptr;
  m_pAdapterTimerMgr->Start(this, dwElapse, bImmediately, &pTimerInfo);
  return pTimerInfo;
}
