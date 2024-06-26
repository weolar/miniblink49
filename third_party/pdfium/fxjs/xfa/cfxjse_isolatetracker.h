// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CFXJSE_ISOLATETRACKER_H_
#define FXJS_XFA_CFXJSE_ISOLATETRACKER_H_

#include "v8/include/v8.h"

class CFXJSE_ScopeUtil_IsolateHandle {
 public:
  explicit CFXJSE_ScopeUtil_IsolateHandle(v8::Isolate* pIsolate);
  ~CFXJSE_ScopeUtil_IsolateHandle();

 private:
  CFXJSE_ScopeUtil_IsolateHandle(const CFXJSE_ScopeUtil_IsolateHandle&) =
      delete;
  void operator=(const CFXJSE_ScopeUtil_IsolateHandle&) = delete;
  void* operator new(size_t size) = delete;
  void operator delete(void*, size_t) = delete;

  v8::Isolate::Scope m_iscope;
  v8::HandleScope m_hscope;
};

class CFXJSE_ScopeUtil_IsolateHandleRootContext final
    : public CFXJSE_ScopeUtil_IsolateHandle {
 public:
  explicit CFXJSE_ScopeUtil_IsolateHandleRootContext(v8::Isolate* pIsolate);
  ~CFXJSE_ScopeUtil_IsolateHandleRootContext();

 private:
  CFXJSE_ScopeUtil_IsolateHandleRootContext(
      const CFXJSE_ScopeUtil_IsolateHandleRootContext&) = delete;
  void operator=(const CFXJSE_ScopeUtil_IsolateHandleRootContext&) = delete;
  void* operator new(size_t size) = delete;
  void operator delete(void*, size_t) = delete;

  v8::Context::Scope m_cscope;
};

#endif  // FXJS_XFA_CFXJSE_ISOLATETRACKER_H_
