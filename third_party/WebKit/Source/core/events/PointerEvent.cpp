// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/events/PointerEvent.h"

#include "core/dom/Element.h"
#include "core/events/EventDispatcher.h"

namespace blink {

PointerEvent::PointerEvent()
    : m_pointerId(0)
    , m_width(0)
    , m_height(0)
    , m_pressure(0)
    , m_tiltX(0)
    , m_tiltY(0)
    , m_isPrimary(false)
{
}

PointerEvent::PointerEvent(const AtomicString& type, const PointerEventInit& initializer)
    : MouseEvent(type, initializer)
    , m_pointerId(0)
    , m_width(0)
    , m_height(0)
    , m_pressure(0)
    , m_tiltX(0)
    , m_tiltY(0)
    , m_isPrimary(false)
{
    if (initializer.hasPointerId())
        m_pointerId = initializer.pointerId();
    if (initializer.hasWidth())
        m_width = initializer.width();
    if (initializer.hasHeight())
        m_height = initializer.height();
    if (initializer.hasPressure())
        m_pressure = initializer.pressure();
    if (initializer.hasTiltX())
        m_tiltX = initializer.tiltX();
    if (initializer.hasTiltY())
        m_tiltY = initializer.tiltY();
    if (initializer.hasPointerType())
        m_pointerType = initializer.pointerType();
    if (initializer.hasIsPrimary())
        m_isPrimary = initializer.isPrimary();
}

bool PointerEvent::isMouseEvent() const
{
    return false;
}

bool PointerEvent::isPointerEvent() const
{
    return true;
}

DEFINE_TRACE(PointerEvent)
{
    MouseEvent::trace(visitor);
}

PassRefPtrWillBeRawPtr<PointerEventDispatchMediator> PointerEventDispatchMediator::create(PassRefPtrWillBeRawPtr<PointerEvent> pointerEvent)
{
    return adoptRefWillBeNoop(new PointerEventDispatchMediator(pointerEvent));
}

PointerEventDispatchMediator::PointerEventDispatchMediator(PassRefPtrWillBeRawPtr<PointerEvent> pointerEvent)
    : EventDispatchMediator(pointerEvent)
{
}

PointerEvent& PointerEventDispatchMediator::event() const
{
    return toPointerEvent(EventDispatchMediator::event());
}

bool PointerEventDispatchMediator::dispatchEvent(EventDispatcher& dispatcher) const
{
    if (isDisabledFormControl(&dispatcher.node()))
        return false;

    if (event().type().isEmpty())
        return true; // Shouldn't happen.

    ASSERT(!event().target() || event().target() != event().relatedTarget());

    EventTarget* relatedTarget = event().relatedTarget();
    event().eventPath().adjustForRelatedTarget(dispatcher.node(), relatedTarget);

    dispatcher.dispatch();
    return !event().defaultHandled() && !event().defaultPrevented();
}

} // namespace blink
