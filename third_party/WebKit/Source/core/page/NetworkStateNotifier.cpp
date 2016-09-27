/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/page/NetworkStateNotifier.h"

#include "core/dom/CrossThreadTask.h"
#include "core/dom/ExecutionContext.h"
#include "core/page/Page.h"
#include "wtf/Assertions.h"
#include "wtf/Functional.h"
#include "wtf/MainThread.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Threading.h"

namespace blink {

NetworkStateNotifier& networkStateNotifier()
{
    AtomicallyInitializedStaticReference(NetworkStateNotifier, networkStateNotifier, new NetworkStateNotifier);
    return networkStateNotifier;
}

void NetworkStateNotifier::setOnLine(bool onLine)
{
    ASSERT(isMainThread());

    {
        MutexLocker locker(m_mutex);
        if (m_isOnLine == onLine)
            return;

        m_isOnLine = onLine;
    }

    Page::networkStateChanged(onLine);
}

void NetworkStateNotifier::setWebConnectionType(WebConnectionType type)
{
    ASSERT(isMainThread());
    if (m_testUpdatesOnly)
        return;

    setWebConnectionTypeImpl(type);
}

void NetworkStateNotifier::setWebConnectionTypeImpl(WebConnectionType type)
{
    ASSERT(isMainThread());

    MutexLocker locker(m_mutex);
    if (m_type == type)
        return;
    m_type = type;

    for (const auto& entry : m_observers) {
        ExecutionContext* context = entry.key;
        context->postTask(FROM_HERE, createCrossThreadTask(&NetworkStateNotifier::notifyObserversOnContext, this, type));
    }
}

void NetworkStateNotifier::addObserver(NetworkStateObserver* observer, ExecutionContext* context)
{
    ASSERT(context->isContextThread());
    ASSERT(observer);

    MutexLocker locker(m_mutex);
    ObserverListMap::AddResult result = m_observers.add(context, nullptr);
    if (result.isNewEntry)
        result.storedValue->value = adoptPtr(new ObserverList);

    ASSERT(result.storedValue->value->observers.find(observer) == kNotFound);
    result.storedValue->value->observers.append(observer);
}

void NetworkStateNotifier::removeObserver(NetworkStateObserver* observer, ExecutionContext* context)
{
    ASSERT(context->isContextThread());
    ASSERT(observer);

    ObserverList* observerList = lockAndFindObserverList(context);
    if (!observerList)
        return;

    Vector<NetworkStateObserver*>& observers = observerList->observers;
    size_t index = observers.find(observer);
    if (index != kNotFound) {
        observers[index] = 0;
        observerList->zeroedObservers.append(index);
    }

    if (!observerList->iterating && !observerList->zeroedObservers.isEmpty())
        collectZeroedObservers(observerList, context);
}

void NetworkStateNotifier::setTestUpdatesOnly(bool updatesOnly)
{
    ASSERT(isMainThread());
    m_testUpdatesOnly = updatesOnly;
}

void NetworkStateNotifier::setWebConnectionTypeForTest(WebConnectionType type)
{
    ASSERT(isMainThread());
    ASSERT(m_testUpdatesOnly);
    setWebConnectionTypeImpl(type);
}

void NetworkStateNotifier::notifyObserversOnContext(WebConnectionType type, ExecutionContext* context)
{
    ObserverList* observerList = lockAndFindObserverList(context);

    // The context could have been removed before the notification task got to run.
    if (!observerList)
        return;

    ASSERT(context->isContextThread());

    observerList->iterating = true;

    for (size_t i = 0; i < observerList->observers.size(); ++i) {
        // Observers removed during iteration are zeroed out, skip them.
        if (observerList->observers[i])
            observerList->observers[i]->connectionTypeChange(type);
    }

    observerList->iterating = false;

    if (!observerList->zeroedObservers.isEmpty())
        collectZeroedObservers(observerList, context);
}

NetworkStateNotifier::ObserverList* NetworkStateNotifier::lockAndFindObserverList(ExecutionContext* context)
{
    MutexLocker locker(m_mutex);
    ObserverListMap::iterator it = m_observers.find(context);
    return it == m_observers.end() ? 0 : it->value.get();
}

void NetworkStateNotifier::collectZeroedObservers(ObserverList* list, ExecutionContext* context)
{
    ASSERT(context->isContextThread());
    ASSERT(!list->iterating);

    // If any observers were removed during the iteration they will have
    // 0 values, clean them up.
    for (size_t i = 0; i < list->zeroedObservers.size(); ++i)
        list->observers.remove(list->zeroedObservers[i]);

    list->zeroedObservers.clear();

    if (list->observers.isEmpty()) {
        MutexLocker locker(m_mutex);
        m_observers.remove(context); // deletes list
    }
}

} // namespace blink
