// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/FrameRequestCallbackCollection.h"

#include "core/dom/FrameRequestCallback.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"

namespace blink {

FrameRequestCallbackCollection::FrameRequestCallbackCollection(ExecutionContext* context)
    : m_context(context)
{
}

FrameRequestCallbackCollection::CallbackId FrameRequestCallbackCollection::registerCallback(FrameRequestCallback* callback)
{
    FrameRequestCallbackCollection::CallbackId id = ++m_nextCallbackId;
    callback->m_cancelled = false;
    callback->m_id = id;
    m_callbacks.append(callback);

    TRACE_EVENT_INSTANT1("devtools.timeline", "RequestAnimationFrame", TRACE_EVENT_SCOPE_THREAD, "data", InspectorAnimationFrameEvent::data(m_context, id));
    InspectorInstrumentation::didRequestAnimationFrame(m_context, id);

    return id;
}

void FrameRequestCallbackCollection::cancelCallback(CallbackId id)
{
    for (size_t i = 0; i < m_callbacks.size(); ++i) {
        if (m_callbacks[i]->m_id == id) {
            m_callbacks.remove(i);
            TRACE_EVENT_INSTANT1("devtools.timeline", "CancelAnimationFrame", TRACE_EVENT_SCOPE_THREAD, "data", InspectorAnimationFrameEvent::data(m_context, id));
            InspectorInstrumentation::didCancelAnimationFrame(m_context, id);
            return;
        }
    }
    for (size_t i = 0; i < m_callbacksToInvoke.size(); ++i) {
        if (m_callbacksToInvoke[i]->m_id == id) {
            TRACE_EVENT_INSTANT1("devtools.timeline", "CancelAnimationFrame", TRACE_EVENT_SCOPE_THREAD, "data", InspectorAnimationFrameEvent::data(m_context, id));
            InspectorInstrumentation::didCancelAnimationFrame(m_context, id);
            m_callbacksToInvoke[i]->m_cancelled = true;
            // will be removed at the end of executeCallbacks()
            return;
        }
    }
}

void FrameRequestCallbackCollection::executeCallbacks(double highResNowMs, double highResNowMsLegacy)
{
    // First, generate a list of callbacks to consider.  Callbacks registered from this point
    // on are considered only for the "next" frame, not this one.
    ASSERT(m_callbacksToInvoke.isEmpty());
    m_callbacksToInvoke.swap(m_callbacks);

    for (size_t i = 0; i < m_callbacksToInvoke.size(); ++i) {
        FrameRequestCallback* callback = m_callbacksToInvoke[i].get();
        if (!callback->m_cancelled) {
            TRACE_EVENT1("devtools.timeline", "FireAnimationFrame", "data", InspectorAnimationFrameEvent::data(m_context, callback->m_id));
            InspectorInstrumentationCookie cookie = InspectorInstrumentation::willFireAnimationFrame(m_context, callback->m_id);
            if (callback->m_useLegacyTimeBase)
                callback->handleEvent(highResNowMsLegacy);
            else
                callback->handleEvent(highResNowMs);
            InspectorInstrumentation::didFireAnimationFrame(cookie);
            TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateCounters", TRACE_EVENT_SCOPE_THREAD, "data", InspectorUpdateCountersEvent::data());
        }
    }

    m_callbacksToInvoke.clear();
}

DEFINE_TRACE(FrameRequestCallbackCollection)
{
#if ENABLE(OILPAN)
    visitor->trace(m_callbacks);
    visitor->trace(m_callbacksToInvoke);
    visitor->trace(m_context);
#endif
}

} // namespace blink
