// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PointerEvent_h
#define PointerEvent_h

#include "core/events/MouseEvent.h"
#include "core/events/PointerEventInit.h"

namespace blink {

class PointerEvent final : public MouseEvent {
    DEFINE_WRAPPERTYPEINFO();

public:
    static PassRefPtrWillBeRawPtr<PointerEvent> create()
    {
        return adoptRefWillBeNoop(new PointerEvent);
    }

    static PassRefPtrWillBeRawPtr<PointerEvent> create(const AtomicString& type, const PointerEventInit& initializer)
    {
        return adoptRefWillBeNoop(new PointerEvent(type, initializer));
    }

    long pointerId() const { return m_pointerId; }
    double width() const { return m_width; }
    double height() const { return m_height; }
    float pressure() const { return m_pressure; }
    long tiltX() const { return m_tiltX; }
    long tiltY() const { return m_tiltY; }
    const String& pointerType() const { return m_pointerType; }
    bool isPrimary() const { return m_isPrimary; }

    virtual bool isMouseEvent() const override;
    virtual bool isPointerEvent() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    PointerEvent();
    PointerEvent(const AtomicString&, const PointerEventInit&);

    long m_pointerId;
    double m_width;
    double m_height;
    float m_pressure;
    long m_tiltX;
    long m_tiltY;
    String m_pointerType;
    bool m_isPrimary;
};


class PointerEventDispatchMediator final : public EventDispatchMediator {
public:
    static PassRefPtrWillBeRawPtr<PointerEventDispatchMediator> create(PassRefPtrWillBeRawPtr<PointerEvent>);

private:
    explicit PointerEventDispatchMediator(PassRefPtrWillBeRawPtr<PointerEvent>);
    PointerEvent& event() const;
    virtual bool dispatchEvent(EventDispatcher&) const override;
};

DEFINE_EVENT_TYPE_CASTS(PointerEvent);

} // namespace blink

#endif // PointerEvent_h
