// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/permissions/PermissionStatus.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "modules/EventTargetModulesNames.h"
#include "modules/permissions/PermissionController.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/permissions/WebPermissionClient.h"

namespace blink {

namespace {

// TODO(mlamouri): move this to a common place.
WebPermissionClient* permissionClient(ExecutionContext* executionContext)
{
    if (executionContext->isDocument()) {
        Document* document = toDocument(executionContext);
        if (!document->frame())
            return nullptr;
        PermissionController* controller = PermissionController::from(*document->frame());
        return controller ? controller->client() : nullptr;
    }
    return Platform::current()->permissionClient();
}

} // anonymous namespace

// static
PermissionStatus* PermissionStatus::take(ScriptPromiseResolver* resolver, WebPermissionStatus* status, WebPermissionType type)
{
    PermissionStatus* permissionStatus = PermissionStatus::create(resolver->executionContext(), *status, type);
    permissionStatus->startListening();
    delete status;
    return permissionStatus;
}

// static
void PermissionStatus::dispose(WebPermissionStatus* status)
{
    delete status;
}

PermissionStatus* PermissionStatus::create(ExecutionContext* executionContext, WebPermissionStatus status, WebPermissionType type)
{
    PermissionStatus* permissionStatus = new PermissionStatus(executionContext, status, type);
    permissionStatus->suspendIfNeeded();
    return permissionStatus;
}

PermissionStatus::PermissionStatus(ExecutionContext* executionContext, WebPermissionStatus status, WebPermissionType type)
    : ActiveDOMObject(executionContext)
    , m_status(status)
    , m_type(type)
    , m_listening(false)
{
}

PermissionStatus::~PermissionStatus()
{
    stopListening();
}

const AtomicString& PermissionStatus::interfaceName() const
{
    return EventTargetNames::PermissionStatus;
}

ExecutionContext* PermissionStatus::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

void PermissionStatus::permissionChanged(WebPermissionType type, WebPermissionStatus status)
{
    ASSERT(m_type == type);
    if (m_status == status)
        return;

    m_status = status;
    dispatchEvent(Event::create(EventTypeNames::change));
}

bool PermissionStatus::hasPendingActivity() const
{
    return m_listening;
}

void PermissionStatus::resume()
{
    startListening();
}

void PermissionStatus::suspend()
{
    stopListening();
}

void PermissionStatus::stop()
{
    stopListening();
}

void PermissionStatus::startListening()
{
    ASSERT(!m_listening);

    WebPermissionClient* client = permissionClient(executionContext());
    if (!client)
        return;
    m_listening = true;
    client->startListening(m_type, KURL(KURL(), executionContext()->securityOrigin()->toString()), this);
}

void PermissionStatus::stopListening()
{
    if (!m_listening)
        return;

    ASSERT(executionContext());

    m_listening = false;
    WebPermissionClient* client = permissionClient(executionContext());
    if (!client)
        return;
    client->stopListening(this);
}

String PermissionStatus::state() const
{
    switch (m_status) {
    case WebPermissionStatusGranted:
        return "granted";
    case WebPermissionStatusDenied:
        return "denied";
    case WebPermissionStatusPrompt:
        return "prompt";
    }

    ASSERT_NOT_REACHED();
    return "denied";
}

DEFINE_TRACE(PermissionStatus)
{
    RefCountedGarbageCollectedEventTargetWithInlineData<PermissionStatus>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
