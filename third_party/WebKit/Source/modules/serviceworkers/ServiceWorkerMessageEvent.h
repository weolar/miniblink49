// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerMessageEvent_h
#define ServiceWorkerMessageEvent_h

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/MessagePort.h"
#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "modules/serviceworkers/ServiceWorkerMessageEventInit.h"

namespace blink {

class MODULES_EXPORT ServiceWorkerMessageEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<ServiceWorkerMessageEvent> create()
    {
        return adoptRefWillBeNoop(new ServiceWorkerMessageEvent);
    }

    static PassRefPtrWillBeRawPtr<ServiceWorkerMessageEvent> create(const AtomicString& type, const ServiceWorkerMessageEventInit& initializer)
    {
        return adoptRefWillBeNoop(new ServiceWorkerMessageEvent(type, initializer));
    }

    static PassRefPtrWillBeRawPtr<ServiceWorkerMessageEvent> create(MessagePortArray* ports, PassRefPtr<SerializedScriptValue> data, PassRefPtrWillBeRawPtr<ServiceWorker> source, const String& origin)
    {
        return adoptRefWillBeNoop(new ServiceWorkerMessageEvent(data, origin, String(), source, ports));
    }

    ~ServiceWorkerMessageEvent() override;

    ScriptValue data() const { return m_data; }
    SerializedScriptValue* serializedData() const { return m_serializedData.get(); }
    const String& origin() const { return m_origin; }
    const String& lastEventId() const { return m_lastEventId; }
    MessagePortArray ports(bool& isNull) const;
    MessagePortArray ports() const;
    void source(ServiceWorkerOrMessagePort& result) const;

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    ServiceWorkerMessageEvent();
    ServiceWorkerMessageEvent(const AtomicString& type, const ServiceWorkerMessageEventInit& initializer);
    ServiceWorkerMessageEvent(PassRefPtr<SerializedScriptValue> data, const String& origin, const String& lastEventId, PassRefPtrWillBeRawPtr<ServiceWorker> source, MessagePortArray* ports);

    ScriptValue m_data;
    RefPtr<SerializedScriptValue> m_serializedData;
    String m_origin;
    String m_lastEventId;
    RefPtrWillBeMember<ServiceWorker> m_sourceAsServiceWorker;
    PersistentWillBeMember<MessagePort> m_sourceAsMessagePort;
    PersistentWillBeMember<MessagePortArray> m_ports;
};

} // namespace blink

#endif // ServiceWorkerMessageEvent_h
