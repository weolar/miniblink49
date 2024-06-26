// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_timerinfo.h"

#include "xfa/fwl/ifwl_adaptertimermgr.h"

CFWL_TimerInfo::CFWL_TimerInfo(IFWL_AdapterTimerMgr* mgr) : m_pMgr(mgr) {
  ASSERT(mgr);
}

CFWL_TimerInfo::~CFWL_TimerInfo() {}

void CFWL_TimerInfo::StopTimer() {
  m_pMgr->Stop(this);
}
