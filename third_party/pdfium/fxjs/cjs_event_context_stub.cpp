// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_event_context_stub.h"

Optional<IJS_Runtime::JS_Error> CJS_EventContextStub::RunScript(
    const WideString& script) {
  return IJS_Runtime::JS_Error(1, 1, L"JavaScript support not present");
}
