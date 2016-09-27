/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2013 Google Inc. All Rights Reserved.
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
#include "core/dom/ContextLifecycleNotifier.h"

#include "core/dom/ActiveDOMObject.h"
#include "wtf/TemporaryChange.h"

namespace blink {

void ContextLifecycleNotifier::notifyResumingActiveDOMObjects()
{
    TemporaryChange<IterationType> scope(m_iterating, IteratingOverAll);
    Vector<ContextLifecycleObserver*> snapshotOfObservers;
    copyToVector(m_observers, snapshotOfObservers);
    for (ContextLifecycleObserver* observer : snapshotOfObservers) {
        // FIXME: Oilpan: At the moment, it's possible that a ActiveDOMObject
        // observer is destructed while iterating. Once we enable Oilpan by default
        // for all LifecycleObserver<T>s, we can remove the hack by making m_observers
        // a HeapHashSet<WeakMember<LifecycleObserver<T>>>.
        // (i.e., we can just iterate m_observers without taking a snapshot).
        // For more details, see https://codereview.chromium.org/247253002/.
        if (m_observers.contains(observer)) {
            if (observer->observerType() != ContextLifecycleObserver::ActiveDOMObjectType)
                continue;
            ActiveDOMObject* activeDOMObject = static_cast<ActiveDOMObject*>(observer);
            ASSERT(activeDOMObject->executionContext() == context());
            ASSERT(activeDOMObject->suspendIfNeededCalled());
            activeDOMObject->resume();
        }
    }
}

void ContextLifecycleNotifier::notifySuspendingActiveDOMObjects()
{
    TemporaryChange<IterationType> scope(m_iterating, IteratingOverAll);
    Vector<ContextLifecycleObserver*> snapshotOfObservers;
    copyToVector(m_observers, snapshotOfObservers);
    for (ContextLifecycleObserver* observer : snapshotOfObservers) {
        // It's possible that the ActiveDOMObject is already destructed.
        // See a FIXME above.
        if (m_observers.contains(observer)) {
            if (observer->observerType() != ContextLifecycleObserver::ActiveDOMObjectType)
                continue;
            ActiveDOMObject* activeDOMObject = static_cast<ActiveDOMObject*>(observer);
            ASSERT(activeDOMObject->executionContext() == context());
            ASSERT(activeDOMObject->suspendIfNeededCalled());
            activeDOMObject->suspend();
        }
    }
}

void ContextLifecycleNotifier::notifyStoppingActiveDOMObjects()
{
    TemporaryChange<IterationType> scope(m_iterating, IteratingOverAll);
    Vector<ContextLifecycleObserver*> snapshotOfObservers;
    copyToVector(m_observers, snapshotOfObservers);
    for (ContextLifecycleObserver* observer : snapshotOfObservers) {
        // It's possible that the ActiveDOMObject is already destructed.
        // See a FIXME above.
        if (m_observers.contains(observer)) {
            if (observer->observerType() != ContextLifecycleObserver::ActiveDOMObjectType)
                continue;
            ActiveDOMObject* activeDOMObject = static_cast<ActiveDOMObject*>(observer);
            ASSERT(activeDOMObject->executionContext() == context());
            ASSERT(activeDOMObject->suspendIfNeededCalled());
            activeDOMObject->stop();
        }
    }
}

unsigned ContextLifecycleNotifier::activeDOMObjectCount() const
{
    unsigned activeDOMObjects = 0;
    for (ContextLifecycleObserver* observer : m_observers) {
        if (observer->observerType() != ContextLifecycleObserver::ActiveDOMObjectType)
            continue;
        activeDOMObjects++;
    }
    return activeDOMObjects;
}

bool ContextLifecycleNotifier::hasPendingActivity() const
{
    for (ContextLifecycleObserver* observer : m_observers) {
        if (observer->observerType() != ContextLifecycleObserver::ActiveDOMObjectType)
            continue;
        ActiveDOMObject* activeDOMObject = static_cast<ActiveDOMObject*>(observer);
        if (activeDOMObject->hasPendingActivity())
            return true;
    }
    return false;
}

#if ENABLE(ASSERT)
bool ContextLifecycleNotifier::contains(ActiveDOMObject* object) const
{
    for (ContextLifecycleObserver* observer : m_observers) {
        if (observer->observerType() != ContextLifecycleObserver::ActiveDOMObjectType)
            continue;
        ActiveDOMObject* activeDOMObject = static_cast<ActiveDOMObject*>(observer);
        if (activeDOMObject == object)
            return true;
    }
    return false;
}
#endif

} // namespace blink
