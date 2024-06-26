// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_IFWL_ADAPTERTIMERMGR_H_
#define XFA_FWL_IFWL_ADAPTERTIMERMGR_H_

#include "xfa/fwl/cfwl_timer.h"

class IFWL_AdapterTimerMgr {
 public:
  virtual ~IFWL_AdapterTimerMgr() = default;
  virtual void Start(CFWL_Timer* pTimer,
                     uint32_t dwElapse,
                     bool bImmediately,
                     CFWL_TimerInfo** pTimerInfo) = 0;
  virtual void Stop(CFWL_TimerInfo* pTimerInfo) = 0;
};

#endif  // XFA_FWL_IFWL_ADAPTERTIMERMGR_H_
