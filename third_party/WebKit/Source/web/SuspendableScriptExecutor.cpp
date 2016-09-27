// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/SuspendableScriptExecutor.h"

#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "platform/UserGestureIndicator.h"
#include "public/platform/WebVector.h"
#include "public/web/WebScriptExecutionCallback.h"

namespace blink {

void SuspendableScriptExecutor::createAndRun(LocalFrame* frame, int worldID, const WillBeHeapVector<ScriptSourceCode>& sources, int extensionGroup, bool userGesture, WebScriptExecutionCallback* callback)
{
    RefPtrWillBeRawPtr<SuspendableScriptExecutor> executor = adoptRefWillBeNoop(new SuspendableScriptExecutor(frame, worldID, sources, extensionGroup, userGesture, callback));
    executor->ref();
    executor->run();
}

void SuspendableScriptExecutor::contextDestroyed()
{
    SuspendableTimer::contextDestroyed();
    m_callback->completed(Vector<v8::Local<v8::Value>>());
    dispose();
}

SuspendableScriptExecutor::SuspendableScriptExecutor(LocalFrame* frame, int worldID, const WillBeHeapVector<ScriptSourceCode>& sources, int extensionGroup, bool userGesture, WebScriptExecutionCallback* callback)
    : SuspendableTimer(frame->document())
    , m_frame(frame)
    , m_worldID(worldID)
    , m_sources(sources)
    , m_extensionGroup(extensionGroup)
    , m_userGesture(userGesture)
    , m_callback(callback)
#if ENABLE(ASSERT)
    , m_disposed(false)
#endif
{
}

SuspendableScriptExecutor::~SuspendableScriptExecutor()
{
#if ENABLE(ASSERT)
    ASSERT(m_disposed);
#endif
}

void SuspendableScriptExecutor::fired()
{
    executeAndDestroySelf();
}

void SuspendableScriptExecutor::run()
{
    ExecutionContext* context = executionContext();
    ASSERT(context);
    if (!context->activeDOMObjectsAreSuspended()) {
        suspendIfNeeded();
        executeAndDestroySelf();
        return;
    }
    startOneShot(0, FROM_HERE);
    suspendIfNeeded();
}

void SuspendableScriptExecutor::executeAndDestroySelf()
{
    // Ensure that this object is not deleted even if the context is destroyed.
    RefPtrWillBeRawPtr<SuspendableScriptExecutor> protect(this);

    // after calling the destructor of object - object will be unsubscribed from
    // resumed and contextDestroyed LifecycleObserver methods
    OwnPtr<UserGestureIndicator> indicator;
    if (m_userGesture)
        indicator = adoptPtr(new UserGestureIndicator(DefinitelyProcessingNewUserGesture));

    v8::HandleScope scope(v8::Isolate::GetCurrent());
    Vector<v8::Local<v8::Value>> results;
    if (m_worldID) {
        m_frame->script().executeScriptInIsolatedWorld(m_worldID, m_sources, m_extensionGroup, &results);
    } else {
        v8::Local<v8::Value> scriptValue = m_frame->script().executeScriptInMainWorldAndReturnValue(m_sources.first());
        results.append(scriptValue);
    }

    // The script may have removed the frame, in which case contextDestroyed()
    // will have handled the disposal/callback.
    if (!m_frame->client())
        return;

    m_callback->completed(results);
    dispose();
}

void SuspendableScriptExecutor::dispose()
{
#if ENABLE(ASSERT)
    m_disposed = true;
#endif

#if ENABLE(OILPAN)
    // Remove object as a ContextLifecycleObserver.
    ActiveDOMObject::clearContext();
#endif
    deref();
}

DEFINE_TRACE(SuspendableScriptExecutor)
{
#if ENABLE(OILPAN)
    visitor->trace(m_frame);
    visitor->trace(m_sources);
#endif
    SuspendableTimer::trace(visitor);
}

} // namespace blink
