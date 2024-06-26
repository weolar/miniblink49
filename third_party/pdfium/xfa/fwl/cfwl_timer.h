// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_TIMER_H_
#define XFA_FWL_CFWL_TIMER_H_

#include <memory>

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CFWL_TimerInfo;
class CFWL_Widget;
class IFWL_AdapterTimerMgr;

class CFWL_Timer {
 public:
  explicit CFWL_Timer(CFWL_Widget* parent);
  virtual ~CFWL_Timer();

  virtual void Run(CFWL_TimerInfo* hTimer) = 0;
  CFWL_TimerInfo* StartTimer(uint32_t dwElapse, bool bImmediately);

 protected:
  UnownedPtr<CFWL_Widget> m_pWidget;
  std::unique_ptr<IFWL_AdapterTimerMgr> m_pAdapterTimerMgr;
};

#endif  // XFA_FWL_CFWL_TIMER_H_
