// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/ServicePortCollection.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/MessagePort.h"
#include "core/events/MessageEvent.h"
#include "modules/EventTargetModules.h"
#include "modules/navigatorconnect/AcceptConnectionObserver.h"
#include "modules/navigatorconnect/ServicePort.h"
#include "modules/navigatorconnect/ServicePortConnectEvent.h"
#include "modules/navigatorconnect/ServicePortConnectEventInit.h"
#include "modules/navigatorconnect/ServicePortConnectOptions.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/navigator_services/WebServicePortProvider.h"

namespace blink {

namespace {

class ConnectCallbacks : public WebServicePortConnectCallbacks {
public:
    ConnectCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver, ServicePortCollection* collection, const KURL& targetUrl, const String& portName, const String& serializedPortData)
        : m_resolver(resolver), m_collection(collection), m_targetUrl(targetUrl), m_portName(portName), m_serializedPortData(serializedPortData)
    {
        ASSERT(m_resolver);
    }

    ~ConnectCallbacks() override { }

    void onSuccess(WebServicePortID* portIdRaw) override
    {
        OwnPtr<WebServicePortID> webPortId = adoptPtr(portIdRaw);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped()) {
            return;
        }
        WebServicePort webPort;
        webPort.id = *webPortId;
        webPort.targetUrl = m_targetUrl;
        webPort.name = m_portName;
        webPort.data = m_serializedPortData;
        ServicePort* port = ServicePort::create(m_collection, webPort);
        m_collection->addPort(port);
        m_resolver->resolve(port);
    }

    void onError() override
    {
        // TODO(mek): Pass actual error code back.
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped()) {
            return;
        }
        m_resolver->reject(DOMException::create(AbortError));
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
    Persistent<ServicePortCollection> m_collection;
    KURL m_targetUrl;
    String m_portName;
    String m_serializedPortData;

    WTF_MAKE_NONCOPYABLE(ConnectCallbacks);
};

} // namespace

ServicePortCollection* ServicePortCollection::create(ExecutionContext* context)
{
    return new ServicePortCollection(context);
}

ServicePortCollection::~ServicePortCollection()
{
}

void ServicePortCollection::addPort(ServicePort* port)
{
    m_ports.append(port);
}

void ServicePortCollection::closePort(ServicePort* port)
{
    m_ports.remove(m_ports.find(port));
    if (m_provider)
        m_provider->closePort(port->id());
}

ScriptPromise ServicePortCollection::connect(ScriptState* scriptState, const String& url, const ServicePortConnectOptions& options, ExceptionState& exceptionState)
{
    if (!m_provider)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));

    RefPtr<SerializedScriptValue> portData;
    if (options.hasData()) {
        portData = SerializedScriptValueFactory::instance().create(options.data().isolate(), options.data(), nullptr, exceptionState);
        if (exceptionState.hadException())
            return exceptionState.reject(scriptState);
    }
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    KURL targetUrl = scriptState->executionContext()->completeURL(url);
    m_provider->connect(
        targetUrl,
        scriptState->executionContext()->securityOrigin()->toString(),
        new ConnectCallbacks(resolver, this, targetUrl, options.name(), portData ? portData->toWireString() : String()));
    return promise;
}

ScriptPromise ServicePortCollection::match(ScriptState* scriptState, const ServicePortMatchOptions& options)
{
    return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));
}

ScriptPromise ServicePortCollection::matchAll(ScriptState* scriptState, const ServicePortMatchOptions& options)
{
    return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));
}

const AtomicString& ServicePortCollection::interfaceName() const
{
    return EventTargetNames::ServicePortCollection;
}

ExecutionContext* ServicePortCollection::executionContext() const
{
    return ContextLifecycleObserver::executionContext();
}

void ServicePortCollection::postMessage(WebServicePortID portId, const WebString& messageString, const WebMessagePortChannelArray& webChannels)
{
    OwnPtr<MessagePortChannelArray> channels;
    if (webChannels.size()) {
        channels = adoptPtr(new MessagePortChannelArray(webChannels.size()));
        for (size_t i = 0; i < webChannels.size(); ++i)
            (*channels)[i] = adoptPtr(webChannels[i]);
    }
    RefPtr<SerializedScriptValue> message = SerializedScriptValueFactory::instance().createFromWire(messageString);

    MessagePortArray* ports = MessagePort::entanglePorts(*executionContext(), channels.release());
    RefPtrWillBeRawPtr<Event> evt = MessageEvent::create(ports, message.release());
    // TODO(mek): Lookup ServicePort and set events source attribute.
    dispatchEvent(evt.release(), ASSERT_NO_EXCEPTION);
}

void ServicePortCollection::dispatchConnectEvent(PassOwnPtr<WebServicePortConnectEventCallbacks> callbacks, const WebURL& targetURL, const WebString& origin, WebServicePortID portID)
{
    AcceptConnectionObserver* observer = AcceptConnectionObserver::create(this, callbacks, portID, targetURL);
    ServicePortConnectEventInit init;
    init.setTargetURL(targetURL.string());
    init.setOrigin(origin);
    RefPtrWillBeRawPtr<Event> event = ServicePortConnectEvent::create(EventTypeNames::connect, init, observer);
    dispatchEvent(event.release());
    observer->didDispatchEvent();
}

DEFINE_TRACE(ServicePortCollection)
{
    visitor->trace(m_ports);
    EventTargetWithInlineData::trace(visitor);
    ContextLifecycleObserver::trace(visitor);
}

ServicePortCollection::ServicePortCollection(ExecutionContext* context)
    : ContextLifecycleObserver(context)
    , m_provider(adoptPtr(Platform::current()->createServicePortProvider(this)))
{
}

} // namespace blink
