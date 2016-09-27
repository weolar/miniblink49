/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IDBVersionChangeEvent_h
#define IDBVersionChangeEvent_h

#include "bindings/core/v8/Nullable.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "modules/EventModules.h"
#include "modules/indexeddb/IDBAny.h"
#include "modules/indexeddb/IDBRequest.h"
#include "modules/indexeddb/IDBVersionChangeEventInit.h"
#include "public/platform/modules/indexeddb/WebIDBTypes.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class IDBVersionChangeEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<IDBVersionChangeEvent> create()
    {
        return adoptRefWillBeNoop(new IDBVersionChangeEvent());
    }
    static PassRefPtrWillBeRawPtr<IDBVersionChangeEvent> create(const AtomicString& eventType, unsigned long long oldVersion, const Nullable<unsigned long long>& newVersion, WebIDBDataLoss dataLoss = WebIDBDataLossNone, const String& dataLossMessage = String())
    {
        return adoptRefWillBeNoop(new IDBVersionChangeEvent(eventType, oldVersion, newVersion, dataLoss, dataLossMessage));
    }
    static PassRefPtrWillBeRawPtr<IDBVersionChangeEvent> create(const AtomicString& eventType, const IDBVersionChangeEventInit& initializer)
    {
        return adoptRefWillBeNoop(new IDBVersionChangeEvent(eventType, initializer));
    }

    unsigned long long oldVersion() const { return m_oldVersion; }
    unsigned long long newVersion(bool& isNull) const;

    const AtomicString& dataLoss() const;
    const String& dataLossMessage() const { return m_dataLossMessage; }

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    IDBVersionChangeEvent();
    IDBVersionChangeEvent(const AtomicString& eventType, unsigned long long oldVersion, const Nullable<unsigned long long>& newVersion, WebIDBDataLoss, const String& dataLoss);
    IDBVersionChangeEvent(const AtomicString& eventType, const IDBVersionChangeEventInit&);

    unsigned long long m_oldVersion;
    Nullable<unsigned long long> m_newVersion;
    WebIDBDataLoss m_dataLoss;
    String m_dataLossMessage;
};

} // namespace blink

#endif // IDBVersionChangeEvent_h
