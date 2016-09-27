// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/presentation/PresentationAvailability.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "modules/EventTargetModulesNames.h"
#include "modules/presentation/PresentationController.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/presentation/WebPresentationClient.h"

namespace blink {

namespace {

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

// static
PresentationAvailability* PresentationAvailability::take(ScriptPromiseResolver* resolver, bool value)
{
    PresentationAvailability* presentationAvailability = new PresentationAvailability(resolver->executionContext(), value);
    presentationAvailability->suspendIfNeeded();
    presentationAvailability->updateListening();
    return presentationAvailability;
}

PresentationAvailability::PresentationAvailability(ExecutionContext* executionContext, bool value)
    : ActiveDOMObject(executionContext)
    , DocumentVisibilityObserver(*toDocument(executionContext))
    , m_value(value)
    , m_state(State::Active)
{
    ASSERT(executionContext->isDocument());
}

PresentationAvailability::~PresentationAvailability()
{
    setState(State::Inactive);
}

const AtomicString& PresentationAvailability::interfaceName() const
{
    return EventTargetNames::PresentationAvailability;
}

ExecutionContext* PresentationAvailability::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

void PresentationAvailability::availabilityChanged(bool value)
{
    if (m_value == value)
        return;

    m_value = value;
    dispatchEvent(Event::create(EventTypeNames::change));
}

bool PresentationAvailability::hasPendingActivity() const
{
    return m_state != State::Inactive;
}

void PresentationAvailability::resume()
{
    setState(State::Active);
}

void PresentationAvailability::suspend()
{
    setState(State::Suspended);
}

void PresentationAvailability::stop()
{
    setState(State::Inactive);
}

void PresentationAvailability::didChangeVisibilityState(PageVisibilityState visibility)
{
    updateListening();
}

void PresentationAvailability::setState(State state)
{
    m_state = state;
    updateListening();
}

void PresentationAvailability::updateListening()
{
    WebPresentationClient* client = presentationClient(executionContext());
    if (!client)
        return;

    if (m_state == State::Active && (toDocument(executionContext())->pageVisibilityState() == PageVisibilityStateVisible))
        client->startListening(this);
    else
        client->stopListening(this);
}

bool PresentationAvailability::value() const
{
    return m_value;
}

DEFINE_TRACE(PresentationAvailability)
{
    RefCountedGarbageCollectedEventTargetWithInlineData<PresentationAvailability>::trace(visitor);
    DocumentVisibilityObserver::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace
