// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webmidi/MIDIAccessInitializer.h"

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/Navigator.h"
#include "modules/webmidi/MIDIAccess.h"
#include "modules/webmidi/MIDIController.h"
#include "modules/webmidi/MIDIOptions.h"
#include "modules/webmidi/MIDIPort.h"

namespace blink {

using PortState = WebMIDIAccessorClient::MIDIPortState;

MIDIAccessInitializer::MIDIAccessInitializer(ScriptState* scriptState, const MIDIOptions& options)
    : ScriptPromiseResolver(scriptState)
    , m_requestSysex(false)
    , m_hasBeenDisposed(false)
    , m_sysexPermissionResolved(false)
{
    if (options.hasSysex())
        m_requestSysex = options.sysex();
}

MIDIAccessInitializer::~MIDIAccessInitializer()
{
    dispose();
}

void MIDIAccessInitializer::contextDestroyed()
{
    dispose();
    LifecycleObserver::contextDestroyed();
}

void MIDIAccessInitializer::dispose()
{
    if (m_hasBeenDisposed)
        return;

    if (!executionContext())
        return;

    if (!m_sysexPermissionResolved) {
        Document* document = toDocument(executionContext());
        ASSERT(document);
        if (MIDIController* controller = MIDIController::from(document->frame()))
            controller->cancelSysexPermissionRequest(this);
        m_sysexPermissionResolved = true;
    }

    m_hasBeenDisposed = true;
}

ScriptPromise MIDIAccessInitializer::start()
{
    ScriptPromise promise = this->promise();
    m_accessor = MIDIAccessor::create(this);

    if (!m_requestSysex) {
        m_accessor->startSession();
        return promise;
    }
    Document* document = toDocument(executionContext());
    ASSERT(document);
    if (MIDIController* controller = MIDIController::from(document->frame()))
        controller->requestSysexPermission(this);
    else
        reject(DOMException::create(SecurityError));

    return promise;
}

void MIDIAccessInitializer::didAddInputPort(const String& id, const String& manufacturer, const String& name, const String& version, PortState state)
{
    ASSERT(m_accessor);
    m_portDescriptors.append(PortDescriptor(id, manufacturer, name, MIDIPort::TypeInput, version, state));
}

void MIDIAccessInitializer::didAddOutputPort(const String& id, const String& manufacturer, const String& name, const String& version, PortState state)
{
    ASSERT(m_accessor);
    m_portDescriptors.append(PortDescriptor(id, manufacturer, name, MIDIPort::TypeOutput, version, state));
}

void MIDIAccessInitializer::didSetInputPortState(unsigned portIndex, PortState state)
{
    // didSetInputPortState() is not allowed to call before didStartSession()
    // is called. Once didStartSession() is called, MIDIAccessorClient methods
    // are delegated to MIDIAccess. See constructor of MIDIAccess.
    ASSERT_NOT_REACHED();
}

void MIDIAccessInitializer::didSetOutputPortState(unsigned portIndex, PortState state)
{
    // See comments on didSetInputPortState().
    ASSERT_NOT_REACHED();
}

void MIDIAccessInitializer::didStartSession(bool success, const String& error, const String& message)
{
    ASSERT(m_accessor);
    if (success) {
        resolve(MIDIAccess::create(m_accessor.release(), m_requestSysex, m_portDescriptors, executionContext()));
    } else {
        // The spec says the name is one of
        //  - SecurityError
        //  - AbortError
        //  - InvalidStateError
        //  - NotSupportedError
        // TODO(toyoshim): Do not rely on |error| string. Instead an enum
        // representing an ExceptionCode should be defined and deliverred.
        ExceptionCode ec = InvalidStateError;
        if (error == DOMException::getErrorName(SecurityError)) {
            ec = SecurityError;
        } else if (error == DOMException::getErrorName(AbortError)) {
            ec = AbortError;
        } else if (error == DOMException::getErrorName(InvalidStateError)) {
            ec = InvalidStateError;
        } else if (error == DOMException::getErrorName(NotSupportedError)) {
            ec = NotSupportedError;
        }
        reject(DOMException::create(ec, message));
    }
}

void MIDIAccessInitializer::resolveSysexPermission(bool allowed)
{
    m_sysexPermissionResolved = true;
    if (allowed)
        m_accessor->startSession();
    else
        reject(DOMException::create(SecurityError));
}

SecurityOrigin* MIDIAccessInitializer::securityOrigin() const
{
    return executionContext()->securityOrigin();
}

ExecutionContext* MIDIAccessInitializer::executionContext() const
{
    return scriptState()->executionContext();
}

} // namespace blink
