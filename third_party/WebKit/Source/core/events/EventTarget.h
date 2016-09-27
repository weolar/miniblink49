/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 *           (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef EventTarget_h
#define EventTarget_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/EventNames.h"
#include "core/EventTargetNames.h"
#include "core/EventTypeNames.h"
#include "core/events/EventListenerMap.h"
#include "platform/heap/Handle.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class LocalDOMWindow;
class Event;
class ExceptionState;
class MessagePort;
class Node;

struct FiringEventIterator {
    FiringEventIterator(const AtomicString& eventType, size_t& iterator, size_t& end)
        : eventType(eventType)
        , iterator(iterator)
        , end(end)
    {
    }

    const AtomicString& eventType;
    size_t& iterator;
    size_t& end;
};
typedef Vector<FiringEventIterator, 1> FiringEventIteratorVector;

struct CORE_EXPORT EventTargetData {
    WTF_MAKE_NONCOPYABLE(EventTargetData); WTF_MAKE_FAST_ALLOCATED(EventTargetData);
public:
    EventTargetData();
    ~EventTargetData();

    EventListenerMap eventListenerMap;
    OwnPtr<FiringEventIteratorVector> firingEventIterators;
};

// This is the base class for all DOM event targets. To make your class an
// EventTarget, follow these steps:
// - Make your IDL interface inherit from EventTarget.
//   Optionally add "attribute EventHandler onfoo;" attributes.
// - Inherit from EventTargetWithInlineData (only in rare cases should you use
//   EventTarget directly); or, if you want YourClass to be inherited from
//   RefCountedGarbageCollected<YourClass> in addition to EventTargetWithInlineData,
//   inherit from RefCountedGarbageCollectedEventTargetWithInlineData<YourClass>.
// - In your class declaration, EventTargetWithInlineData (or
//   RefCountedGarbageCollectedEventTargetWithInlineData<>) must come first in
//   the base class list. If your class is non-final, classes inheriting from
//   your class need to come first, too.
// - Figure out if you now need to inherit from ActiveDOMObject as well.
// - In your class declaration, you will typically use
//   REFCOUNTED_EVENT_TARGET(YourClass) if YourClass is a RefCounted<>,
//   or REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(OtherRefCounted<YourClass>)
//   if YourClass uses a different kind of reference counting template such as
//   RefCountedGarbageCollected<YourClass>.
// - Make sure to include this header file in your .h file, or you will get
//   very strange compiler errors.
// - If you added an onfoo attribute, use DEFINE_ATTRIBUTE_EVENT_LISTENER(foo)
//   in your class declaration.
// - Override EventTarget::interfaceName() and executionContext(). The former
//   will typically return EventTargetNames::YourClassName. The latter will
//   return ActiveDOMObject::executionContext (if you are an ActiveDOMObject)
//   or the document you're in.
// - Your trace() method will need to call EventTargetWithInlineData::trace
//   or RefCountedGarbageCollectedEventTargetWithInlineData<YourClass>::trace,
//   depending on the base class of your class.
//
// Optionally, add a FooEvent.idl class, but that's outside the scope of this
// comment (and much more straightforward).
class CORE_EXPORT EventTarget : public NoBaseWillBeGarbageCollectedFinalized<EventTarget>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    virtual ~EventTarget();

#if !ENABLE(OILPAN)
    void ref() { refEventTarget(); }
    void deref() { derefEventTarget(); }
#endif

    virtual const AtomicString& interfaceName() const = 0;
    virtual ExecutionContext* executionContext() const = 0;

    virtual Node* toNode();
    virtual LocalDOMWindow* toDOMWindow();
    virtual MessagePort* toMessagePort();

    virtual bool addEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture);
    virtual bool removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture);
    virtual void removeAllEventListeners();
    virtual bool dispatchEvent(PassRefPtrWillBeRawPtr<Event>);
    bool dispatchEvent(PassRefPtrWillBeRawPtr<Event>, ExceptionState&); // DOM API
    virtual void uncaughtExceptionInEventHandler();

    // Used for legacy "onEvent" attribute APIs.
    bool setAttributeEventListener(const AtomicString& eventType, PassRefPtr<EventListener>);
    EventListener* getAttributeEventListener(const AtomicString& eventType);

    bool hasEventListeners() const;
    bool hasEventListeners(const AtomicString& eventType) const;
    bool hasCapturingEventListeners(const AtomicString& eventType);
    const EventListenerVector& getEventListeners(const AtomicString& eventType);
    Vector<AtomicString> eventTypes();

    bool fireEventListeners(Event*);

    DEFINE_INLINE_VIRTUAL_TRACE() { }

    virtual bool keepEventInNode(Event*) { return false; }

protected:
    EventTarget();

    // Subclasses should likely not override these themselves; instead, they should subclass EventTargetWithInlineData.
    virtual EventTargetData* eventTargetData() = 0;
    virtual EventTargetData& ensureEventTargetData() = 0;

private:
#if !ENABLE(OILPAN)
    // Subclasses should likely not override these themselves; instead, they should use the REFCOUNTED_EVENT_TARGET() macro.
    virtual void refEventTarget() = 0;
    virtual void derefEventTarget() = 0;
#endif

    LocalDOMWindow* executingWindow();
    void fireEventListeners(Event*, EventTargetData*, EventListenerVector&);
    void countLegacyEvents(const AtomicString& legacyTypeName, EventListenerVector*, EventListenerVector*);

    bool clearAttributeEventListener(const AtomicString& eventType);

    friend class EventListenerIterator;
};

class CORE_EXPORT EventTargetWithInlineData : public EventTarget {
protected:
    virtual EventTargetData* eventTargetData() override final { return &m_eventTargetData; }
    virtual EventTargetData& ensureEventTargetData() override final { return m_eventTargetData; }
private:
    EventTargetData m_eventTargetData;
};

// Base class for classes that wish to inherit from RefCountedGarbageCollected (in non-Oilpan world) and
// EventTargetWithInlineData (in both worlds). For details about how to use this class template, see the comments for
// EventTargetWithInlineData above.
//
// This class template exists to circumvent Oilpan's "leftmost class rule", where the Oilpan classes must come first in
// the base class list to avoid memory offset adjustment. In non-Oilpan world, RefCountedGarbageCollected<T> must come
// first, but in Oilpan world EventTargetWithInlineData needs to come first. This class templates does the required
// #if-switch here, in order to avoid a lot of "#if ENABLE(OILPAN)"-s sprinkled in the derived classes.
#if ENABLE(OILPAN)
template <typename T>
class RefCountedGarbageCollectedEventTargetWithInlineData : public EventTargetWithInlineData {
public:
    GC_PLUGIN_IGNORE("491488")
    void* operator new(size_t size)
    {
        // If T is eagerly finalized, it needs to be allocated accordingly.
        // Redefinition of the operator is needed to accomplish that, as otherwise
        // it would be allocated using GarbageCollected<EventTarget>'s operator new.
        // EventTarget is not eagerly finalized.
        return allocateObject(size, IsEagerlyFinalizedType<T>::value);
    }
};
#else
template <typename T>
class RefCountedGarbageCollectedEventTargetWithInlineData : public RefCountedGarbageCollected<T>, public EventTargetWithInlineData {
public:
    DEFINE_INLINE_VIRTUAL_TRACE() { EventTargetWithInlineData::trace(visitor); }
};
#endif

// FIXME: These macros should be split into separate DEFINE and DECLARE
// macros to avoid causing so many header includes.
#define DEFINE_ATTRIBUTE_EVENT_LISTENER(attribute) \
    EventListener* on##attribute() { return getAttributeEventListener(EventTypeNames::attribute); } \
    void setOn##attribute(PassRefPtr<EventListener> listener) { setAttributeEventListener(EventTypeNames::attribute, listener); } \

#define DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(attribute) \
    static EventListener* on##attribute(EventTarget& eventTarget) { return eventTarget.getAttributeEventListener(EventTypeNames::attribute); } \
    static void setOn##attribute(EventTarget& eventTarget, PassRefPtr<EventListener> listener) { eventTarget.setAttributeEventListener(EventTypeNames::attribute, listener); } \

#define DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(attribute) \
    EventListener* on##attribute() { return document().getWindowAttributeEventListener(EventTypeNames::attribute); } \
    void setOn##attribute(PassRefPtr<EventListener> listener) { document().setWindowAttributeEventListener(EventTypeNames::attribute, listener); } \

#define DEFINE_STATIC_WINDOW_ATTRIBUTE_EVENT_LISTENER(attribute) \
    static EventListener* on##attribute(EventTarget& eventTarget) { \
        if (Node* node = eventTarget.toNode()) \
            return node->document().getWindowAttributeEventListener(EventTypeNames::attribute); \
        ASSERT(eventTarget.toDOMWindow()); \
        return eventTarget.getAttributeEventListener(EventTypeNames::attribute); \
    } \
    static void setOn##attribute(EventTarget& eventTarget, PassRefPtr<EventListener> listener) { \
        if (Node* node = eventTarget.toNode()) \
            node->document().setWindowAttributeEventListener(EventTypeNames::attribute, listener); \
        else { \
            ASSERT(eventTarget.toDOMWindow()); \
            eventTarget.setAttributeEventListener(EventTypeNames::attribute, listener); \
        } \
    }

#define DEFINE_MAPPED_ATTRIBUTE_EVENT_LISTENER(attribute, eventName) \
    EventListener* on##attribute() { return getAttributeEventListener(EventTypeNames::eventName); } \
    void setOn##attribute(PassRefPtr<EventListener> listener) { setAttributeEventListener(EventTypeNames::eventName, listener); } \

#define DECLARE_FORWARDING_ATTRIBUTE_EVENT_LISTENER(recipient, attribute) \
    EventListener* on##attribute(); \
    void setOn##attribute(PassRefPtr<EventListener> listener);

#define DEFINE_FORWARDING_ATTRIBUTE_EVENT_LISTENER(type, recipient, attribute) \
    EventListener* type::on##attribute() { return recipient ? recipient->getAttributeEventListener(EventTypeNames::attribute) : 0; } \
    void type::setOn##attribute(PassRefPtr<EventListener> listener) \
    { \
        if (recipient) \
            recipient->setAttributeEventListener(EventTypeNames::attribute, listener); \
    }

inline bool EventTarget::hasEventListeners() const
{
    // FIXME: We should have a const version of eventTargetData.
    if (const EventTargetData* d = const_cast<EventTarget*>(this)->eventTargetData())
        return !d->eventListenerMap.isEmpty();
    return false;
}

inline bool EventTarget::hasEventListeners(const AtomicString& eventType) const
{
    // FIXME: We should have const version of eventTargetData.
    if (const EventTargetData* d = const_cast<EventTarget*>(this)->eventTargetData())
        return d->eventListenerMap.contains(eventType);
    return false;
}

inline bool EventTarget::hasCapturingEventListeners(const AtomicString& eventType)
{
    EventTargetData* d = eventTargetData();
    if (!d)
        return false;
    return d->eventListenerMap.containsCapturing(eventType);
}

} // namespace blink

// If the EventTarget class is RefCounted on non-oilpan builds,
// use REFCOUNTED_EVENT_TARGET.
// If the EventTarget class is RefCountedGarbageCollected on non-oilpan builds,
// use REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET.
#if ENABLE(OILPAN)
#define DEFINE_EVENT_TARGET_REFCOUNTING(baseClass)
#define REFCOUNTED_EVENT_TARGET(baseClass)
#define REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(baseClass)
#else // !ENABLE(OILPAN)
#define DEFINE_EVENT_TARGET_REFCOUNTING(baseClass) \
public: \
    using baseClass::ref; \
    using baseClass::deref; \
private: \
    virtual void refEventTarget() override final { ref(); } \
    virtual void derefEventTarget() override final { deref(); } \
    typedef int thisIsHereToForceASemiColonAfterThisEventTargetMacro
#define REFCOUNTED_EVENT_TARGET(baseClass) DEFINE_EVENT_TARGET_REFCOUNTING(RefCounted<baseClass>)
#define REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(baseClass) DEFINE_EVENT_TARGET_REFCOUNTING(RefCountedGarbageCollected<baseClass>)
#endif // ENABLE(OILPAN)

#endif // EventTarget_h
