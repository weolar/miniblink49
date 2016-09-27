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

#ifndef ExtendableEvent_h
#define ExtendableEvent_h

#include "bindings/core/v8/ScriptValue.h"
#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "modules/serviceworkers/ExtendableEventInit.h"

namespace blink {

class WaitUntilObserver;

class MODULES_EXPORT ExtendableEvent : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<ExtendableEvent> create();
    static PassRefPtrWillBeRawPtr<ExtendableEvent> create(const AtomicString& type, const ExtendableEventInit&);
    static PassRefPtrWillBeRawPtr<ExtendableEvent> create(const AtomicString& type, const ExtendableEventInit&, WaitUntilObserver*);

    ~ExtendableEvent() override;

    void waitUntil(ScriptState*, const ScriptValue&, ExceptionState&);

    const AtomicString& interfaceName() const override;
    DECLARE_VIRTUAL_TRACE();

protected:
    ExtendableEvent();
    ExtendableEvent(const AtomicString& type, const ExtendableEventInit&);
    ExtendableEvent(const AtomicString& type, const ExtendableEventInit&, WaitUntilObserver*);

    PersistentWillBeMember<WaitUntilObserver> m_observer;
};

} // namespace blink

#endif // ExtendableEvent_h
