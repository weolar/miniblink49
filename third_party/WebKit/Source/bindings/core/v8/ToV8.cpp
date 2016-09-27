// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ToV8.h"

#include "core/events/EventTarget.h"
#include "core/frame/DOMWindow.h"
#include "core/workers/WorkerGlobalScope.h"

namespace blink {

v8::Local<v8::Value> toV8(DOMWindow* window, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    // Notice that we explicitly ignore creationContext because the DOMWindow
    // has its own creationContext.

    if (UNLIKELY(!window))
        return v8::Null(isolate);
    // Initializes environment of a frame, and return the global object
    // of the frame.
    Frame * frame = window->frame();
    if (!frame)
        return v8Undefined();

    v8::Local<v8::Context> context = toV8Context(frame, DOMWrapperWorld::current(isolate));
    if (context.IsEmpty())
        return v8Undefined();

    v8::Local<v8::Object> global = context->Global();
    ASSERT(!global.IsEmpty());
    return global;
}

v8::Local<v8::Value> toV8(EventTarget* impl, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (UNLIKELY(!impl))
        return v8::Null(isolate);

    if (impl->interfaceName() == EventTargetNames::DOMWindow)
        return toV8(static_cast<DOMWindow*>(impl), creationContext, isolate);
    return toV8(static_cast<ScriptWrappable*>(impl), creationContext, isolate);
}

v8::Local<v8::Value> toV8(WorkerGlobalScope* impl, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    // Notice that we explicitly ignore creationContext because the
    // WorkerGlobalScope has its own creationContext.

    if (UNLIKELY(!impl))
        return v8::Null(isolate);

    WorkerScriptController* script = impl->script();
    if (!script)
        return v8::Null(isolate);

    v8::Local<v8::Object> global = script->context()->Global();
    ASSERT(!global.IsEmpty());
    return global;
}

} // namespace blink
