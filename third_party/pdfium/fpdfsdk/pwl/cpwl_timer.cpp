// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_timer.h"

#include <map>

#include "fpdfsdk/pwl/cpwl_timer_handler.h"

namespace {

std::map<int32_t, CPWL_Timer*>& GetPWLTimeMap() {
  // Leak the object at shutdown.
  static auto* timeMap = new std::map<int32_t, CPWL_Timer*>;
  return *timeMap;
}

}  // namespace

CPWL_Timer::CPWL_Timer(CPWL_TimerHandler* pAttached,
                       CFX_SystemHandler* pSystemHandler)
    : m_pAttached(pAttached), m_pSystemHandler(pSystemHandler) {
  ASSERT(m_pAttached);
  ASSERT(m_pSystemHandler);
}

CPWL_Timer::~CPWL_Timer() {
  KillPWLTimer();
}

int32_t CPWL_Timer::SetPWLTimer(int32_t nElapse) {
  KillPWLTimer();

  m_nTimerID = m_pSystemHandler->SetTimer(nElapse, TimerProc);
  if (HasValidID())
    GetPWLTimeMap()[m_nTimerID] = this;
  return m_nTimerID;
}

void CPWL_Timer::KillPWLTimer() {
  if (!HasValidID())
    return;

  m_pSystemHandler->KillTimer(m_nTimerID);
  GetPWLTimeMap().erase(m_nTimerID);
  m_nTimerID = CFX_SystemHandler::kInvalidTimerID;
}

// static
void CPWL_Timer::TimerProc(int32_t idEvent) {
  auto it = GetPWLTimeMap().find(idEvent);
  if (it == GetPWLTimeMap().end())
    return;

  CPWL_Timer* pTimer = it->second;
  if (pTimer->m_pAttached)
    pTimer->m_pAttached->TimerProc();
}
