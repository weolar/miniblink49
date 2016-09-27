/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ExtendableEvent.h"

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/serviceworkers/WaitUntilObserver.h"

namespace blink {

PassRefPtrWillBeRawPtr<ExtendableEvent> ExtendableEvent::create()
{
    return adoptRefWillBeNoop(new ExtendableEvent());
}

PassRefPtrWillBeRawPtr<ExtendableEvent> ExtendableEvent::create(const AtomicString& type, const ExtendableEventInit& eventInit)
{
    return adoptRefWillBeNoop(new ExtendableEvent(type, eventInit));
}

PassRefPtrWillBeRawPtr<ExtendableEvent> ExtendableEvent::create(const AtomicString& type, const ExtendableEventInit& eventInit, WaitUntilObserver* observer)
{
    return adoptRefWillBeNoop(new ExtendableEvent(type, eventInit, observer));
}

ExtendableEvent::~ExtendableEvent()
{
}

void ExtendableEvent::waitUntil(ScriptState* scriptState, const ScriptValue& value, ExceptionState& exceptionState)
{
    if (m_observer)
        m_observer->waitUntil(scriptState, value, exceptionState);
}

ExtendableEvent::ExtendableEvent()
{
}

ExtendableEvent::ExtendableEvent(const AtomicString& type, const ExtendableEventInit& initializer)
    : Event(type, initializer)
{
}

ExtendableEvent::ExtendableEvent(const AtomicString& type, const ExtendableEventInit& initializer, WaitUntilObserver* observer)
    : Event(type, initializer)
    , m_observer(observer)
{
}

const AtomicString& ExtendableEvent::interfaceName() const
{
    return EventNames::ExtendableEvent;
}

DEFINE_TRACE(ExtendableEvent)
{
    visitor->trace(m_observer);
    Event::trace(visitor);
}

} // namespace blink
