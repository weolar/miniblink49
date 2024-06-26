// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_runtimestub.h"

#include "fxjs/cjs_event_context_stub.h"
#include "third_party/base/ptr_util.h"

CJS_RuntimeStub::CJS_RuntimeStub(CPDFSDK_FormFillEnvironment* pFormFillEnv)
    : m_pFormFillEnv(pFormFillEnv) {}

CJS_RuntimeStub::~CJS_RuntimeStub() = default;

IJS_EventContext* CJS_RuntimeStub::NewEventContext() {
  if (!m_pContext)
    m_pContext = pdfium::MakeUnique<CJS_EventContextStub>();
  return m_pContext.get();
}

void CJS_RuntimeStub::ReleaseEventContext(IJS_EventContext* pContext) {}

CPDFSDK_FormFillEnvironment* CJS_RuntimeStub::GetFormFillEnv() const {
  return m_pFormFillEnv.Get();
}

#ifdef PDF_ENABLE_XFA
CJS_Runtime* CJS_RuntimeStub::AsCJSRuntime() {
  return nullptr;
}

bool CJS_RuntimeStub::GetValueByNameFromGlobalObject(ByteStringView,
                                                     CFXJSE_Value*) {
  return false;
}

bool CJS_RuntimeStub::SetValueByNameInGlobalObject(ByteStringView,
                                                   CFXJSE_Value*) {
  return false;
}
#endif  // PDF_ENABLE_XFA

Optional<IJS_Runtime::JS_Error> CJS_RuntimeStub::ExecuteScript(
    const WideString& script) {
  return pdfium::nullopt;
}
