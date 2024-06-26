// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cfxjse_isolatetracker.h"

#include "fxjs/xfa/cfxjse_runtimedata.h"

CFXJSE_ScopeUtil_IsolateHandle::CFXJSE_ScopeUtil_IsolateHandle(
    v8::Isolate* pIsolate)
    : m_iscope(pIsolate), m_hscope(pIsolate) {}

CFXJSE_ScopeUtil_IsolateHandle::~CFXJSE_ScopeUtil_IsolateHandle() = default;

CFXJSE_ScopeUtil_IsolateHandleRootContext::
    CFXJSE_ScopeUtil_IsolateHandleRootContext(v8::Isolate* pIsolate)
    : CFXJSE_ScopeUtil_IsolateHandle(pIsolate),
      m_cscope(v8::Local<v8::Context>::New(
          pIsolate,
          CFXJSE_RuntimeData::Get(pIsolate)->m_hRootContext)) {}

CFXJSE_ScopeUtil_IsolateHandleRootContext::
    ~CFXJSE_ScopeUtil_IsolateHandleRootContext() = default;
