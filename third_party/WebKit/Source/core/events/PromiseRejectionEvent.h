// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PromiseRejectionEvent_h
#define PromiseRejectionEvent_h

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScopedPersistent.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "core/CoreExport.h"
#include "core/events/Event.h"
#include "core/events/PromiseRejectionEventInit.h"

namespace blink {

class CORE_EXPORT PromiseRejectionEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<PromiseRejectionEvent> create()
    {
        return adoptRefWillBeNoop(new PromiseRejectionEvent);
    }
    static PassRefPtrWillBeRawPtr<PromiseRejectionEvent> create(ScriptState* state, const AtomicString& type, const PromiseRejectionEventInit& initializer)
    {
        return adoptRefWillBeNoop(new PromiseRejectionEvent(state, type, initializer));
    }

    ScriptValue reason(ScriptState*) const;
    ScriptPromise promise(ScriptState*) const;

    void setWrapperReference(v8::Isolate*, const v8::Persistent<v8::Object>&);

    virtual const AtomicString& interfaceName() const override;

    // PromiseRejectionEvents are similar to ErrorEvents in that they can't be
    // observed across different worlds.
    virtual bool canBeDispatchedInWorld(const DOMWrapperWorld&) const override;

    DECLARE_VIRTUAL_TRACE();

private:
    PromiseRejectionEvent();
    PromiseRejectionEvent(ScriptState*, const AtomicString&, const PromiseRejectionEventInit&);
    ~PromiseRejectionEvent() override;

    static void didCollectPromise(const v8::WeakCallbackInfo<PromiseRejectionEvent>&);
    static void didCollectReason(const v8::WeakCallbackInfo<PromiseRejectionEvent>&);

    RefPtr<ScriptState> m_scriptState;
    ScopedPersistent<v8::Value> m_promise;
    ScopedPersistent<v8::Value> m_reason;
};

} // namespace blink

#endif // PromiseRejectionEvent_h
