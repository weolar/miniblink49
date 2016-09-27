// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RespondWithObserver_h
#define RespondWithObserver_h

#include "core/dom/ContextLifecycleObserver.h"
#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebServiceWorkerResponseError.h"
#include "public/platform/WebURLRequest.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class ScriptState;
class ScriptValue;

// This class observes the service worker's handling of a FetchEvent and
// notifies the client.
class MODULES_EXPORT RespondWithObserver final : public GarbageCollectedFinalized<RespondWithObserver>, public ContextLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(RespondWithObserver);
public:
    static RespondWithObserver* create(ExecutionContext*, int eventID, WebURLRequest::FetchRequestMode, WebURLRequest::FrameType);

    void contextDestroyed() override;

    void didDispatchEvent(bool defaultPrevented);

    // Observes the promise and delays calling didHandleFetchEvent() until the
    // given promise is resolved or rejected.
    void respondWith(ScriptState*, const ScriptValue&, ExceptionState&);

    void responseWasRejected(WebServiceWorkerResponseError);
    void responseWasFulfilled(const ScriptValue&);

    DECLARE_VIRTUAL_TRACE();

private:
    class ThenFunction;

    RespondWithObserver(ExecutionContext*, int eventID, WebURLRequest::FetchRequestMode, WebURLRequest::FrameType);

    int m_eventID;
    WebURLRequest::FetchRequestMode m_requestMode;
    WebURLRequest::FrameType m_frameType;

    enum State { Initial, Pending, Done };
    State m_state;
};

} // namespace blink

#endif // RespondWithObserver_h
