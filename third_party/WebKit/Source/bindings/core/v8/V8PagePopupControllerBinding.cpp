// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/V8PagePopupControllerBinding.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Window.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "core/page/DOMWindowPagePopup.h"
#include "core/page/PagePopupController.h"
#include "platform/TraceEvent.h"

namespace blink {

namespace {

void pagePopupControllerAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> holder = info.Holder();
    DOMWindow* impl = V8Window::toImpl(holder);
    RefPtrWillBeRawPtr<PagePopupController> cppValue(DOMWindowPagePopup::pagePopupController(*impl));
    v8SetReturnValue(info, toV8(cppValue.get(), holder, info.GetIsolate()));
}

void pagePopupControllerAttributeGetterCallback(v8::Local<v8::Name>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("blink", "DOMGetter");
    pagePopupControllerAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("v8", "V8Execution");
}

} // namespace

void V8PagePopupControllerBinding::installPagePopupController(v8::Local<v8::Context> context, v8::Local<v8::Object> windowWrapper)
{
    ExecutionContext* executionContext = toExecutionContext(windowWrapper->CreationContext());
    if (!(executionContext && executionContext->isDocument()
        && ContextFeatures::pagePopupEnabled(toDocument(executionContext))))
        return;

    windowWrapper->SetAccessor(
        context,
        v8AtomicString(context->GetIsolate(), "pagePopupController"),
        pagePopupControllerAttributeGetterCallback);
}

} // namespace blink
