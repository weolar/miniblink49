// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServicePort_h
#define ServicePort_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/MessagePort.h"
#include "modules/ModulesExport.h"
#include "public/platform/modules/navigator_services/WebServicePort.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class MessagePort;
class ScriptState;
class ScriptValue;
class SerializedScriptValue;
class ServicePortCollection;

class MODULES_EXPORT ServicePort final
    : public GarbageCollectedFinalized<ServicePort>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(ServicePort);
public:
    static ServicePort* create(ServicePortCollection*, const WebServicePort&);
    virtual ~ServicePort();

    WebServicePortID id() const { return m_port.id; }

    // ServicePort.idl
    String targetURL() const;
    String name() const;
    ScriptValue data(ScriptState*) const;
    void postMessage(ExecutionContext*, PassRefPtr<SerializedScriptValue> message, const MessagePortArray*, ExceptionState&);
    void close();

    DECLARE_VIRTUAL_TRACE();

private:
    ServicePort(ServicePortCollection*, const WebServicePort&);

    bool m_isOpen;
    WebServicePort m_port;
    RefPtr<SerializedScriptValue> m_serializedData;
    Member<ServicePortCollection> m_collection;
};

} // namespace blink

#endif // ServicePort_h
