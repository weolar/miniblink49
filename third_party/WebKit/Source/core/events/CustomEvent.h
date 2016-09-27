/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CustomEvent_h
#define CustomEvent_h

#include "core/CoreExport.h"
#include "core/events/CustomEventInit.h"
#include "core/events/Event.h"

namespace blink {

class SerializedScriptValue;

class CORE_EXPORT CustomEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    virtual ~CustomEvent();

    static PassRefPtrWillBeRawPtr<CustomEvent> create()
    {
        return adoptRefWillBeNoop(new CustomEvent);
    }

    static PassRefPtrWillBeRawPtr<CustomEvent> create(const AtomicString& type, const CustomEventInit& initializer)
    {
        return adoptRefWillBeNoop(new CustomEvent(type, initializer));
    }

    void initCustomEvent(const AtomicString& type, bool canBubble, bool cancelable, PassRefPtr<SerializedScriptValue>);

    virtual const AtomicString& interfaceName() const override;

    SerializedScriptValue* serializedDetail() { return m_serializedDetail.get(); }

    ScriptValue detail() const { return m_detail; }
    void setDetail(ScriptValue detail) { m_detail = detail; }

    DECLARE_VIRTUAL_TRACE();

private:
    CustomEvent();
    CustomEvent(const AtomicString& type, const CustomEventInit& initializer);

    ScriptValue m_detail;
    RefPtr<SerializedScriptValue> m_serializedDetail;
};

} // namespace blink

#endif // CustomEvent_h
