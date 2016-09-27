/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 *           (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2011 Andreas Kling (kling@webkit.org)
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
 *
 */

#include "config.h"
#include "core/events/EventListenerMap.h"

#include "core/events/EventTarget.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Vector.h"

#if ENABLE(ASSERT)
#include "wtf/Threading.h"
#include "wtf/ThreadingPrimitives.h"
#endif

using namespace WTF;

namespace blink {

#if ENABLE(ASSERT)
static Mutex& activeIteratorCountMutex()
{
    AtomicallyInitializedStaticReference(Mutex, mutex, new Mutex());
    return mutex;
}

void EventListenerMap::assertNoActiveIterators()
{
    MutexLocker locker(activeIteratorCountMutex());
    ASSERT(!m_activeIteratorCount);
}
#endif

EventListenerMap::EventListenerMap()
#if ENABLE(ASSERT)
    : m_activeIteratorCount(0)
#endif
{
}

bool EventListenerMap::contains(const AtomicString& eventType) const
{
    for (const auto& entry : m_entries) {
        if (entry.first == eventType)
            return true;
    }
    return false;
}

bool EventListenerMap::containsCapturing(const AtomicString& eventType) const
{
    for (const auto& entry : m_entries) {
        if (entry.first == eventType) {
            for (const auto& eventListener: *entry.second) {
                if (eventListener.useCapture)
                    return true;
            }
        }
    }
    return false;
}

void EventListenerMap::clear()
{
    assertNoActiveIterators();

    m_entries.clear();
}

Vector<AtomicString> EventListenerMap::eventTypes() const
{
    Vector<AtomicString> types;
    types.reserveInitialCapacity(m_entries.size());

    for (const auto& entry : m_entries)
        types.uncheckedAppend(entry.first);

    return types;
}

static bool addListenerToVector(EventListenerVector* vector, PassRefPtr<EventListener> listener, bool useCapture)
{
    RegisteredEventListener registeredListener(listener, useCapture);

    if (vector->find(registeredListener) != kNotFound)
        return false; // Duplicate listener.

    vector->append(registeredListener);
    return true;
}

bool EventListenerMap::add(const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    assertNoActiveIterators();

    for (const auto& entry : m_entries) {
        if (entry.first == eventType)
            return addListenerToVector(entry.second.get(), listener, useCapture);
    }

    m_entries.append(std::make_pair(eventType, adoptPtr(new EventListenerVector)));
    return addListenerToVector(m_entries.last().second.get(), listener, useCapture);
}

static bool removeListenerFromVector(EventListenerVector* listenerVector, EventListener* listener, bool useCapture, size_t& indexOfRemovedListener)
{
    RegisteredEventListener registeredListener(listener, useCapture);
    indexOfRemovedListener = listenerVector->find(registeredListener);
    if (indexOfRemovedListener == kNotFound)
        return false;
    listenerVector->remove(indexOfRemovedListener);
    return true;
}

bool EventListenerMap::remove(const AtomicString& eventType, EventListener* listener, bool useCapture, size_t& indexOfRemovedListener)
{
    assertNoActiveIterators();

    for (unsigned i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].first == eventType) {
            bool wasRemoved = removeListenerFromVector(m_entries[i].second.get(), listener, useCapture, indexOfRemovedListener);
            if (m_entries[i].second->isEmpty())
                m_entries.remove(i);
            return wasRemoved;
        }
    }

    return false;
}

EventListenerVector* EventListenerMap::find(const AtomicString& eventType)
{
    assertNoActiveIterators();

    for (const auto& entry : m_entries) {
        if (entry.first == eventType)
            return entry.second.get();
    }

    return 0;
}

static void copyListenersNotCreatedFromMarkupToTarget(const AtomicString& eventType, EventListenerVector* listenerVector, EventTarget* target)
{
    for (const auto& eventListener : *listenerVector) {
        // Event listeners created from markup have already been transfered to the shadow tree during cloning.
        if (eventListener.listener->wasCreatedFromMarkup())
            continue;
        target->addEventListener(eventType, eventListener.listener, eventListener.useCapture);
    }
}

void EventListenerMap::copyEventListenersNotCreatedFromMarkupToTarget(EventTarget* target)
{
    assertNoActiveIterators();

    for (const auto& eventListener : m_entries)
        copyListenersNotCreatedFromMarkupToTarget(eventListener.first, eventListener.second.get(), target);
}

EventListenerIterator::EventListenerIterator(EventTarget* target)
    : m_map(nullptr)
    , m_entryIndex(0)
    , m_index(0)
{
    ASSERT(target);
    EventTargetData* data = target->eventTargetData();

    if (!data)
        return;

    m_map = &data->eventListenerMap;

#if ENABLE(ASSERT)
    {
        MutexLocker locker(activeIteratorCountMutex());
        m_map->m_activeIteratorCount++;
    }
#endif
}

#if ENABLE(ASSERT)
EventListenerIterator::~EventListenerIterator()
{
    if (m_map) {
        MutexLocker locker(activeIteratorCountMutex());
        m_map->m_activeIteratorCount--;
    }
}
#endif

EventListener* EventListenerIterator::nextListener()
{
    if (!m_map)
        return 0;

    for (; m_entryIndex < m_map->m_entries.size(); ++m_entryIndex) {
        EventListenerVector& listeners = *m_map->m_entries[m_entryIndex].second;
        if (m_index < listeners.size())
            return listeners[m_index++].listener.get();
        m_index = 0;
    }

    return 0;
}

} // namespace blink
