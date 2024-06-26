// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_GLOBAL_TIMER_H_
#define FXJS_GLOBAL_TIMER_H_

#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fxjs/cjs_runtime.h"

class CJS_App;

class GlobalTimer {
 public:
  enum class Type : bool {
    kRepeating = false,
    kOneShot = true,
  };

  GlobalTimer(CJS_App* pObj,
              CPDFSDK_FormFillEnvironment* pFormFillEnv,
              CJS_Runtime* pRuntime,
              Type nType,
              const WideString& script,
              uint32_t dwElapse,
              uint32_t dwTimeOut);
  ~GlobalTimer();

  static void Trigger(int32_t nTimerID);
  static void Cancel(int32_t nTimerID);

  bool IsOneShot() const { return m_nType == Type::kOneShot; }
  uint32_t GetTimeOut() const { return m_dwTimeOut; }
  int32_t GetTimerID() const { return m_nTimerID; }
  CJS_Runtime* GetRuntime() const { return m_pRuntime.Get(); }
  WideString GetJScript() const { return m_swJScript; }

 private:
  bool HasValidID() const;

  const int32_t m_nTimerID;
  CJS_App* const m_pEmbedApp;
  bool m_bProcessing = false;

  // data
  const Type m_nType;
  const uint32_t m_dwTimeOut;
  const WideString m_swJScript;
  CJS_Runtime::ObservedPtr m_pRuntime;
  CPDFSDK_FormFillEnvironment::ObservedPtr m_pFormFillEnv;
};

#endif  // FXJS_GLOBAL_TIMER_H_
