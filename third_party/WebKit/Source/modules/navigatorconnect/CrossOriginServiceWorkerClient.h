// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CrossOriginServiceWorkerClient_h
#define CrossOriginServiceWorkerClient_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/events/EventTarget.h"
#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebCrossOriginServiceWorkerClient.h"
#include "wtf/Forward.h"

namespace blink {

class MODULES_EXPORT CrossOriginServiceWorkerClient final
    : public GarbageCollectedFinalized<CrossOriginServiceWorkerClient>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static CrossOriginServiceWorkerClient* create(const WebCrossOriginServiceWorkerClient&);
    virtual ~CrossOriginServiceWorkerClient();

    String origin() const;
    String targetUrl() const;
    void postMessage(ExecutionContext*, PassRefPtr<SerializedScriptValue> message, const MessagePortArray*, ExceptionState&);

    DEFINE_INLINE_VIRTUAL_TRACE() { }

private:
    explicit CrossOriginServiceWorkerClient(const WebCrossOriginServiceWorkerClient&);

    WebCrossOriginServiceWorkerClient m_webClient;
};

} // namespace blink

#endif // CrossOriginServiceWorkerClient_h
