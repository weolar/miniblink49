// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/global_timer.h"

#include <map>

#include "fpdfsdk/cfx_systemhandler.h"
#include "fxjs/cjs_app.h"

namespace {

using TimerMap = std::map<int32_t, GlobalTimer*>;
TimerMap* GetGlobalTimerMap() {
  // Leak the timer array at shutdown.
  static auto* s_TimerMap = new TimerMap;
  return s_TimerMap;
}

}  // namespace

GlobalTimer::GlobalTimer(CJS_App* pObj,
                         CPDFSDK_FormFillEnvironment* pFormFillEnv,
                         CJS_Runtime* pRuntime,
                         Type nType,
                         const WideString& script,
                         uint32_t dwElapse,
                         uint32_t dwTimeOut)
    : m_nTimerID(pFormFillEnv->GetSysHandler()->SetTimer(dwElapse, Trigger)),
      m_pEmbedApp(pObj),
      m_nType(nType),
      m_dwTimeOut(dwTimeOut),
      m_swJScript(script),
      m_pRuntime(pRuntime),
      m_pFormFillEnv(pFormFillEnv) {
  if (HasValidID())
    (*GetGlobalTimerMap())[m_nTimerID] = this;
}

GlobalTimer::~GlobalTimer() {
  if (!HasValidID())
    return;

  if (GetRuntime())
    m_pFormFillEnv->GetSysHandler()->KillTimer(m_nTimerID);

  GetGlobalTimerMap()->erase(m_nTimerID);
}

// static
void GlobalTimer::Trigger(int32_t nTimerID) {
  auto it = GetGlobalTimerMap()->find(nTimerID);
  if (it == GetGlobalTimerMap()->end())
    return;

  GlobalTimer* pTimer = it->second;
  if (pTimer->m_bProcessing)
    return;

  pTimer->m_bProcessing = true;
  if (pTimer->m_pEmbedApp)
    pTimer->m_pEmbedApp->TimerProc(pTimer);

  // Timer proc may have destroyed timer, find it again.
  it = GetGlobalTimerMap()->find(nTimerID);
  if (it == GetGlobalTimerMap()->end())
    return;

  pTimer = it->second;
  pTimer->m_bProcessing = false;
  if (pTimer->IsOneShot())
    pTimer->m_pEmbedApp->CancelProc(pTimer);
}

// static
void GlobalTimer::Cancel(int32_t nTimerID) {
  auto it = GetGlobalTimerMap()->find(nTimerID);
  if (it == GetGlobalTimerMap()->end())
    return;

  GlobalTimer* pTimer = it->second;
  pTimer->m_pEmbedApp->CancelProc(pTimer);
}

bool GlobalTimer::HasValidID() const {
  return m_nTimerID != CFX_SystemHandler::kInvalidTimerID;
}
