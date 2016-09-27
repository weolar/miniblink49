// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InputDevice_h
#define InputDevice_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/input/InputDeviceInit.h"

namespace blink {

class CORE_EXPORT InputDevice : public GarbageCollectedFinalized<InputDevice>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();

public:
    ~InputDevice();

    // This return a static local InputDevice pointer which has firesTouchEvents set to be true.
    static InputDevice* firesTouchEventsInputDevice();

    // This return a static local InputDevice pointer which has firesTouchEvents set to be false.
    static InputDevice* doesntFireTouchEventsInputDevice();

    static InputDevice* create(bool firesTouchEvents)
    {
        return new InputDevice(firesTouchEvents);
    }

    static InputDevice* create(
        const InputDeviceInit& initializer)
    {
        return new InputDevice(initializer);
    }

    bool firesTouchEvents() const { return m_firesTouchEvents; }

    DEFINE_INLINE_TRACE() { }

private:
    InputDevice(bool firesTouchEvents);
    InputDevice(const InputDeviceInit&);

    // Whether this device dispatches touch events. This mainly lets developers
    // avoid handling both touch and mouse events dispatched for a single user
    // action.
    bool m_firesTouchEvents;
};

} // namespace blink

#endif // InputDevice_h
