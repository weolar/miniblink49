// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_timerobj.h"

#include "fxjs/global_timer.h"
#include "fxjs/js_define.h"

int CJS_TimerObj::ObjDefnID = -1;

// static
int CJS_TimerObj::GetObjDefnID() {
  return ObjDefnID;
}

// static
void CJS_TimerObj::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj("TimerObj", FXJSOBJTYPE_DYNAMIC,
                                 JSConstructor<CJS_TimerObj>, JSDestructor);
}

CJS_TimerObj::CJS_TimerObj(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime)
    : CJS_Object(pObject, pRuntime) {}

CJS_TimerObj::~CJS_TimerObj() = default;

void CJS_TimerObj::SetTimer(GlobalTimer* pTimer) {
  m_nTimerID = pTimer->GetTimerID();
}
