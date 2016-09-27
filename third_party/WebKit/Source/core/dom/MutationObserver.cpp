/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "core/dom/MutationObserver.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/Microtask.h"
#include "core/dom/MutationCallback.h"
#include "core/dom/MutationObserverInit.h"
#include "core/dom/MutationObserverRegistration.h"
#include "core/dom/MutationRecord.h"
#include "core/dom/Node.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "wtf/MainThread.h"
#include <algorithm>

namespace blink {

static unsigned s_observerPriority = 0;

struct MutationObserver::ObserverLessThan {
    bool operator()(const RefPtrWillBeMember<MutationObserver>& lhs, const RefPtrWillBeMember<MutationObserver>& rhs)
    {
        return lhs->m_priority < rhs->m_priority;
    }
};

PassRefPtrWillBeRawPtr<MutationObserver> MutationObserver::create(PassOwnPtrWillBeRawPtr<MutationCallback> callback)
{
    ASSERT(isMainThread());
    return adoptRefWillBeNoop(new MutationObserver(callback));
}

MutationObserver::MutationObserver(PassOwnPtrWillBeRawPtr<MutationCallback> callback)
    : m_callback(callback)
    , m_priority(s_observerPriority++)
{
}

MutationObserver::~MutationObserver()
{
#if !ENABLE(OILPAN)
    ASSERT(m_registrations.isEmpty());
#endif
    if (!m_records.isEmpty())
        InspectorInstrumentation::didClearAllMutationRecords(m_callback->executionContext(), this);
}

void MutationObserver::observe(Node* node, const MutationObserverInit& observerInit, ExceptionState& exceptionState)
{
    if (!node) {
        exceptionState.throwDOMException(NotFoundError, "The provided node was null.");
        return;
    }

    MutationObserverOptions options = 0;

    if (observerInit.hasAttributeOldValue() && observerInit.attributeOldValue())
        options |= AttributeOldValue;

    HashSet<AtomicString> attributeFilter;
    if (observerInit.hasAttributeFilter()) {
        const Vector<String>& sequence = observerInit.attributeFilter();
        for (unsigned i = 0; i < sequence.size(); ++i)
            attributeFilter.add(AtomicString(sequence[i]));
        options |= AttributeFilter;
    }

    bool attributes = observerInit.hasAttributes() && observerInit.attributes();
    if (attributes || (!observerInit.hasAttributes() && (observerInit.hasAttributeOldValue() || observerInit.hasAttributeFilter())))
        options |= Attributes;

    if (observerInit.hasCharacterDataOldValue() && observerInit.characterDataOldValue())
        options |= CharacterDataOldValue;

    bool characterData = observerInit.hasCharacterData() && observerInit.characterData();
    if (characterData || (!observerInit.hasCharacterData() && observerInit.hasCharacterDataOldValue()))
        options |= CharacterData;

    if (observerInit.childList())
        options |= ChildList;

    if (observerInit.subtree())
        options |= Subtree;

    if (!(options & Attributes)) {
        if (options & AttributeOldValue) {
            exceptionState.throwTypeError("The options object may only set 'attributeOldValue' to true when 'attributes' is true or not present.");
            return;
        }
        if (options & AttributeFilter) {
            exceptionState.throwTypeError("The options object may only set 'attributeFilter' when 'attributes' is true or not present.");
            return;
        }
    }
    if (!((options & CharacterData) || !(options & CharacterDataOldValue))) {
        exceptionState.throwTypeError("The options object may only set 'characterDataOldValue' to true when 'characterData' is true or not present.");
        return;
    }

    if (!(options & (Attributes | CharacterData | ChildList))) {
        exceptionState.throwTypeError("The options object must set at least one of 'attributes', 'characterData', or 'childList' to true.");
        return;
    }

    node->registerMutationObserver(*this, options, attributeFilter);
}

MutationRecordVector MutationObserver::takeRecords()
{
    MutationRecordVector records;
    records.swap(m_records);
    InspectorInstrumentation::didClearAllMutationRecords(m_callback->executionContext(), this);
    return records;
}

void MutationObserver::disconnect()
{
    m_records.clear();
    InspectorInstrumentation::didClearAllMutationRecords(m_callback->executionContext(), this);
    MutationObserverRegistrationSet registrations(m_registrations);
    for (auto& registration : registrations) {
        // The registration may be already unregistered while iteration.
        // Only call unregister if it is still in the original set.
        if (m_registrations.contains(registration))
            registration->unregister();
    }
    ASSERT(m_registrations.isEmpty());
}

void MutationObserver::observationStarted(MutationObserverRegistration* registration)
{
    ASSERT(!m_registrations.contains(registration));
    m_registrations.add(registration);
}

void MutationObserver::observationEnded(MutationObserverRegistration* registration)
{
    ASSERT(m_registrations.contains(registration));
    m_registrations.remove(registration);
}

static MutationObserverSet& activeMutationObservers()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<MutationObserverSet>, activeObservers, (adoptPtrWillBeNoop(new MutationObserverSet())));
    return *activeObservers;
}

static MutationObserverSet& suspendedMutationObservers()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<MutationObserverSet>, suspendedObservers, (adoptPtrWillBeNoop(new MutationObserverSet())));
    return *suspendedObservers;
}

static void activateObserver(PassRefPtrWillBeRawPtr<MutationObserver> observer)
{
    if (activeMutationObservers().isEmpty())
        Microtask::enqueueMicrotask(WTF::bind(&MutationObserver::deliverMutations));

    activeMutationObservers().add(observer);
}

void MutationObserver::enqueueMutationRecord(PassRefPtrWillBeRawPtr<MutationRecord> mutation)
{
    ASSERT(isMainThread());
    m_records.append(mutation);
    activateObserver(this);
    InspectorInstrumentation::didEnqueueMutationRecord(m_callback->executionContext(), this);
}

void MutationObserver::setHasTransientRegistration()
{
    ASSERT(isMainThread());
    activateObserver(this);
}

WillBeHeapHashSet<RawPtrWillBeMember<Node>> MutationObserver::getObservedNodes() const
{
    WillBeHeapHashSet<RawPtrWillBeMember<Node>> observedNodes;
    for (const auto& registration : m_registrations)
        registration->addRegistrationNodesToSet(observedNodes);
    return observedNodes;
}

bool MutationObserver::shouldBeSuspended() const
{
    return m_callback->executionContext() && m_callback->executionContext()->activeDOMObjectsAreSuspended();
}

void MutationObserver::deliver()
{
    ASSERT(!shouldBeSuspended());

    // Calling clearTransientRegistrations() can modify m_registrations, so it's necessary
    // to make a copy of the transient registrations before operating on them.
    WillBeHeapVector<RawPtrWillBeMember<MutationObserverRegistration>, 1> transientRegistrations;
    for (auto& registration : m_registrations) {
        if (registration->hasTransientRegistrations())
            transientRegistrations.append(registration);
    }
    for (size_t i = 0; i < transientRegistrations.size(); ++i)
        transientRegistrations[i]->clearTransientRegistrations();

    if (m_records.isEmpty())
        return;

    MutationRecordVector records;
    records.swap(m_records);

    InspectorInstrumentation::willDeliverMutationRecords(m_callback->executionContext(), this);
    m_callback->call(records, this);
    InspectorInstrumentation::didDeliverMutationRecords(m_callback->executionContext());
}

void MutationObserver::resumeSuspendedObservers()
{
    ASSERT(isMainThread());
    if (suspendedMutationObservers().isEmpty())
        return;

    MutationObserverVector suspended;
    copyToVector(suspendedMutationObservers(), suspended);
    for (size_t i = 0; i < suspended.size(); ++i) {
        if (!suspended[i]->shouldBeSuspended()) {
            suspendedMutationObservers().remove(suspended[i]);
            activateObserver(suspended[i]);
        }
    }
}

void MutationObserver::deliverMutations()
{
    ASSERT(isMainThread());
    MutationObserverVector observers;
    copyToVector(activeMutationObservers(), observers);
    activeMutationObservers().clear();
    std::sort(observers.begin(), observers.end(), ObserverLessThan());
    for (size_t i = 0; i < observers.size(); ++i) {
        if (observers[i]->shouldBeSuspended())
            suspendedMutationObservers().add(observers[i]);
        else
            observers[i]->deliver();
    }
}

DEFINE_TRACE(MutationObserver)
{
#if ENABLE(OILPAN)
    visitor->trace(m_callback);
    visitor->trace(m_records);
    visitor->trace(m_registrations);
    visitor->trace(m_callback);
#endif
}

} // namespace blink
