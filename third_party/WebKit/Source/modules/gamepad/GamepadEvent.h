// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GamepadEvent_h
#define GamepadEvent_h

#include "modules/EventModules.h"
#include "modules/gamepad/Gamepad.h"
#include "modules/gamepad/GamepadEventInit.h"

namespace blink {

class GamepadEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<GamepadEvent> create()
    {
        return adoptRefWillBeNoop(new GamepadEvent);
    }
    static PassRefPtrWillBeRawPtr<GamepadEvent> create(const AtomicString& type, bool canBubble, bool cancelable, Gamepad* gamepad)
    {
        return adoptRefWillBeNoop(new GamepadEvent(type, canBubble, cancelable, gamepad));
    }
    static PassRefPtrWillBeRawPtr<GamepadEvent> create(const AtomicString& type, const GamepadEventInit& initializer)
    {
        return adoptRefWillBeNoop(new GamepadEvent(type, initializer));
    }
    ~GamepadEvent() override;

    Gamepad* gamepad() const { return m_gamepad.get(); }

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    GamepadEvent();
    GamepadEvent(const AtomicString& type, bool canBubble, bool cancelable, Gamepad*);
    GamepadEvent(const AtomicString&, const GamepadEventInit&);

    PersistentWillBeMember<Gamepad> m_gamepad;
};

} // namespace blink

#endif // GamepadEvent_h
