// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GamepadButton_h
#define GamepadButton_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Vector.h"

namespace blink {

class GamepadButton final : public GarbageCollected<GamepadButton>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static GamepadButton* create();

    double value() const { return m_value; }
    void setValue(double val) { m_value = val; }

    bool pressed() const { return m_pressed; }
    void setPressed(bool val) { m_pressed = val; }

    DEFINE_INLINE_TRACE() { }

private:
    GamepadButton();
    double m_value;
    bool m_pressed;
};

typedef HeapVector<Member<GamepadButton>> GamepadButtonVector;

} // namespace blink

#endif // GamepadButton_h
