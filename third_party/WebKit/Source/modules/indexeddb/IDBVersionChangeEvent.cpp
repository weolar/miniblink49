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

#include "config.h"
#include "modules/indexeddb/IDBVersionChangeEvent.h"

#include "modules/IndexedDBNames.h"

namespace blink {

IDBVersionChangeEvent::IDBVersionChangeEvent()
    : m_dataLoss(WebIDBDataLossNone)
{
}

IDBVersionChangeEvent::IDBVersionChangeEvent(const AtomicString& eventType, unsigned long long oldVersion, const Nullable<unsigned long long>& newVersion, WebIDBDataLoss dataLoss, const String& dataLossMessage)
    : Event(eventType, false /*canBubble*/, false /*cancelable*/)
    , m_oldVersion(oldVersion)
    , m_newVersion(newVersion)
    , m_dataLoss(dataLoss)
    , m_dataLossMessage(dataLossMessage)
{
}

IDBVersionChangeEvent::IDBVersionChangeEvent(const AtomicString& eventType, const IDBVersionChangeEventInit& initializer)
    : Event(eventType, false /*canBubble*/, false /*cancelable*/)
    , m_oldVersion(initializer.oldVersion())
    , m_newVersion(nullptr)
    , m_dataLoss(WebIDBDataLossNone)
{
    if (initializer.hasNewVersion())
        m_newVersion = initializer.newVersion();
    if (initializer.dataLoss() == "total")
        m_dataLoss = WebIDBDataLossTotal;
}

unsigned long long IDBVersionChangeEvent::newVersion(bool& isNull) const
{
    isNull = m_newVersion.isNull();
    return isNull ? 0 : m_newVersion.get();
}

const AtomicString& IDBVersionChangeEvent::dataLoss() const
{
    if (m_dataLoss == WebIDBDataLossTotal)
        return IndexedDBNames::total;
    return IndexedDBNames::none;
}

const AtomicString& IDBVersionChangeEvent::interfaceName() const
{
    return EventNames::IDBVersionChangeEvent;
}

DEFINE_TRACE(IDBVersionChangeEvent)
{
    Event::trace(visitor);
}

} // namespace blink
