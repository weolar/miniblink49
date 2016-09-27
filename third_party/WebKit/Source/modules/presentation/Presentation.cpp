// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/presentation/Presentation.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/LocalFrame.h"
#include "modules/EventTargetModules.h"
#include "modules/presentation/DefaultSessionStartEvent.h"
#include "modules/presentation/PresentationAvailabilityCallback.h"
#include "modules/presentation/PresentationController.h"
#include "modules/presentation/PresentationSessionClientCallbacks.h"
#include "public/platform/modules/presentation/WebPresentationSessionClient.h"

namespace blink {

namespace {

// TODO(mlamouri): refactor in one common place.
WebPresentationClient* presentationClient(ExecutionContext* executionContext)
{
    ASSERT(executionContext && executionContext->isDocument());

    Document* document = toDocument(executionContext);
    if (!document->frame())
        return nullptr;
    PresentationController* controller = PresentationController::from(*document->frame());
    return controller ? controller->client() : nullptr;
}

} // anonymous namespace

Presentation::Presentation(LocalFrame* frame)
    : DOMWindowProperty(frame)
{
}

Presentation::~Presentation()
{
}

// static
Presentation* Presentation::create(LocalFrame* frame)
{
    ASSERT(frame);

    Presentation* presentation = new Presentation(frame);
    PresentationController* controller = PresentationController::from(*frame);
    if (controller)
        controller->setPresentation(presentation);
    return presentation;
}

const AtomicString& Presentation::interfaceName() const
{
    return EventTargetNames::Presentation;
}

ExecutionContext* Presentation::executionContext() const
{
    if (!frame())
        return nullptr;
    return frame()->document();
}

DEFINE_TRACE(Presentation)
{
    visitor->trace(m_session);
    visitor->trace(m_openSessions);
    RefCountedGarbageCollectedEventTargetWithInlineData<Presentation>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

PresentationSession* Presentation::session() const
{
    return m_session.get();
}

ScriptPromise Presentation::startSession(ScriptState* state, const String& presentationUrl)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(state);
    ScriptPromise promise = resolver->promise();

    WebPresentationClient* client = presentationClient(executionContext());
    if (!client) {
        resolver->reject(DOMException::create(InvalidStateError, "The object is no longer attached to the frame."));
        return promise;
    }
    client->startSession(presentationUrl, new PresentationSessionClientCallbacks(resolver, this));

    return promise;
}

ScriptPromise Presentation::joinSession(ScriptState* state, const String& presentationUrl, const String& presentationId)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(state);
    ScriptPromise promise = resolver->promise();

    WebPresentationClient* client = presentationClient(executionContext());
    if (!client) {
        resolver->reject(DOMException::create(InvalidStateError, "The object is no longer attached to the frame."));
        return promise;
    }
    client->joinSession(presentationUrl, presentationId, new PresentationSessionClientCallbacks(resolver, this));

    return promise;
}

ScriptPromise Presentation::getAvailability(ScriptState* state, const String& presentationUrl)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(state);
    ScriptPromise promise = resolver->promise();

    WebPresentationClient* client = presentationClient(executionContext());
    if (!client) {
        resolver->reject(DOMException::create(InvalidStateError, "The object is no longer attached to the frame."));
        return promise;
    }
    client->getAvailability(presentationUrl, new PresentationAvailabilityCallback(resolver));

    return promise;
}

void Presentation::didStartDefaultSession(PresentationSession* session)
{
    dispatchEvent(DefaultSessionStartEvent::create(EventTypeNames::defaultsessionstart, session));
}

void Presentation::didChangeSessionState(WebPresentationSessionClient* sessionClient, WebPresentationSessionState sessionState)
{
    PresentationSession* session = findSession(sessionClient);
    if (session)
        session->didChangeState(sessionState);

    delete sessionClient;
}

void Presentation::didReceiveSessionTextMessage(WebPresentationSessionClient* sessionClient, const String& message)
{
    PresentationSession* session = findSession(sessionClient);
    if (session)
        session->didReceiveTextMessage(message);

    delete sessionClient;
}

void Presentation::didReceiveSessionBinaryMessage(WebPresentationSessionClient* sessionClient, const uint8_t* data, size_t length)
{
    PresentationSession* session = findSession(sessionClient);
    if (session)
        session->didReceiveBinaryMessage(data, length);

    delete sessionClient;
}

void Presentation::registerSession(PresentationSession* session)
{
    m_openSessions.add(session);
}

PresentationSession* Presentation::findSession(WebPresentationSessionClient* sessionClient)
{
    for (const auto& session : m_openSessions) {
        if (session->matches(sessionClient))
            return session.get();
    }
    return nullptr;
}

} // namespace blink
