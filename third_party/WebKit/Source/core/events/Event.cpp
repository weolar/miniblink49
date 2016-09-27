/*
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2001 Tobias Anton (anton@stud.fbi.fh-darmstadt.de)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2003, 2005, 2006, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/events/Event.h"

#include "core/dom/StaticNodeList.h"
#include "core/events/EventTarget.h"
#include "core/frame/OriginsUsingFeatures.h"
#include "core/frame/UseCounter.h"
#include "core/svg/SVGElement.h"
#include "wtf/CurrentTime.h"

namespace blink {

Event::Event()
    : Event("", false, false)
{
}

Event::Event(const AtomicString& eventType, bool canBubbleArg, bool cancelableArg)
    : m_type(eventType)
    , m_canBubble(canBubbleArg)
    , m_cancelable(cancelableArg)
    , m_propagationStopped(false)
    , m_immediatePropagationStopped(false)
    , m_defaultPrevented(false)
    , m_defaultHandled(false)
    , m_cancelBubble(false)
    , m_eventPhase(0)
    , m_currentTarget(nullptr)
    , m_createTime(convertSecondsToDOMTimeStamp(currentTime()))
    , m_uiCreateTime(0)
{
}

Event::Event(const AtomicString& eventType, const EventInit& initializer)
    : Event(eventType, initializer.bubbles(), initializer.cancelable())
{
}

Event::~Event()
{
}

void Event::initEvent(const AtomicString& eventTypeArg, bool canBubbleArg, bool cancelableArg)
{
    if (dispatched())
        return;

    m_propagationStopped = false;
    m_immediatePropagationStopped = false;
    m_defaultPrevented = false;

    m_type = eventTypeArg;
    m_canBubble = canBubbleArg;
    m_cancelable = cancelableArg;
}

bool Event::legacyReturnValue(ExecutionContext* executionContext) const
{
    bool returnValue = !defaultPrevented();
    if (returnValue)
        UseCounter::count(executionContext, UseCounter::EventGetReturnValueTrue);
    else
        UseCounter::count(executionContext, UseCounter::EventGetReturnValueFalse);
    return returnValue;
}

void Event::setLegacyReturnValue(ExecutionContext* executionContext, bool returnValue)
{
    if (returnValue)
        UseCounter::count(executionContext, UseCounter::EventSetReturnValueTrue);
    else
        UseCounter::count(executionContext, UseCounter::EventSetReturnValueFalse);
    setDefaultPrevented(!returnValue);
}

const AtomicString& Event::interfaceName() const
{
    return EventNames::Event;
}

bool Event::hasInterface(const AtomicString& name) const
{
    return interfaceName() == name;
}

bool Event::isUIEvent() const
{
    return false;
}

bool Event::isMouseEvent() const
{
    return false;
}

bool Event::isFocusEvent() const
{
    return false;
}

bool Event::isKeyboardEvent() const
{
    return false;
}

bool Event::isTouchEvent() const
{
    return false;
}

bool Event::isGestureEvent() const
{
    return false;
}

bool Event::isWheelEvent() const
{
    return false;
}

bool Event::isRelatedEvent() const
{
    return false;
}

bool Event::isPointerEvent() const
{
    return false;
}

bool Event::isDragEvent() const
{
    return false;
}

bool Event::isClipboardEvent() const
{
    return false;
}

bool Event::isBeforeTextInsertedEvent() const
{
    return false;
}

bool Event::isBeforeUnloadEvent() const
{
    return false;
}

void Event::setTarget(PassRefPtrWillBeRawPtr<EventTarget> target)
{
    if (m_target == target)
        return;

    m_target = target;
    if (m_target)
        receivedTarget();
}

void Event::receivedTarget()
{
}

void Event::setUnderlyingEvent(PassRefPtrWillBeRawPtr<Event> ue)
{
    // Prohibit creation of a cycle -- just do nothing in that case.
    for (Event* e = ue.get(); e; e = e->underlyingEvent())
        if (e == this)
            return;
    m_underlyingEvent = ue;
}

void Event::initEventPath(Node& node)
{
    if (!m_eventPath) {
        m_eventPath = adoptPtrWillBeNoop(new EventPath(node, this));
    } else {
        m_eventPath->initializeWith(node, this);
    }
}

WillBeHeapVector<RefPtrWillBeMember<EventTarget>> Event::path(ScriptState* scriptState) const
{
    if (m_target)
        OriginsUsingFeatures::countOriginOrIsolatedWorldHumanReadableName(scriptState, *m_target, OriginsUsingFeatures::Feature::EventPath);

    if (!m_currentTarget) {
        ASSERT(m_eventPhase == Event::NONE);
        if (!m_eventPath) {
            // Before dispatching the event
            return WillBeHeapVector<RefPtrWillBeMember<EventTarget>>();
        }
        ASSERT(!m_eventPath->isEmpty());
        // After dispatching the event
        return m_eventPath->last().treeScopeEventContext().ensureEventPath(*m_eventPath);
    }

    if (Node* node = m_currentTarget->toNode()) {
        ASSERT(m_eventPath);
        size_t eventPathSize = m_eventPath->size();
        for (size_t i = 0; i < eventPathSize; ++i) {
            if (node == (*m_eventPath)[i].node()) {
                return (*m_eventPath)[i].treeScopeEventContext().ensureEventPath(*m_eventPath);
            }
        }
        ASSERT_NOT_REACHED();
    }

    // Returns [window] for events that are directly dispatched to the window object;
    // e.g., window.load, pageshow, etc.
    if (LocalDOMWindow* window = m_currentTarget->toDOMWindow())
        return WillBeHeapVector<RefPtrWillBeMember<EventTarget>>(1, window);

    return WillBeHeapVector<RefPtrWillBeMember<EventTarget>>();
}

EventTarget* Event::currentTarget() const
{
    if (!m_currentTarget)
        return 0;
    Node* node = m_currentTarget->toNode();
    if (node && node->isSVGElement()) {
        if (SVGElement* svgElement = toSVGElement(node)->correspondingElement())
            return svgElement;
    }
    return m_currentTarget.get();
}

DEFINE_TRACE(Event)
{
    visitor->trace(m_currentTarget);
    visitor->trace(m_target);
    visitor->trace(m_underlyingEvent);
    visitor->trace(m_eventPath);
}

} // namespace blink
