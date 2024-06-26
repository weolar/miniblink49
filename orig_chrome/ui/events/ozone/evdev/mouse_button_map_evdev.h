// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_MOUSE_BUTTON_MAP_EVDEV_H_
#define UI_EVENTS_OZONE_EVDEV_MOUSE_BUTTON_MAP_EVDEV_H_

#include <stdint.h>

#include <map>

#include "base/macros.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"

namespace ui {

// Mouse button map for Evdev.
//
// Chrome relies on the underlying OS to interpret mouse buttons. The Linux
// input subsystem does not assign any special meaning to these keys, so
// this work must happen at a higher layer (normally X11 or the console driver).
// When using evdev directly, we must do it ourselves.
//
// The mouse button map is shared between all input devices connected to the
// system.
class EVENTS_OZONE_EVDEV_EXPORT MouseButtonMapEvdev {
public:
    typedef uint32_t Button;
    typedef std::map<uint32_t, uint32_t> ButtonMap;

    MouseButtonMapEvdev();
    ~MouseButtonMapEvdev();

    // Set mapping for one button.
    void UpdateButtonMap(Button button_from, Button button_to);

    // Reset the button map to the system default.
    void ResetButtonMap();

    // Return the mapped button.
    int GetMappedButton(const Button button) const;

    // Return current button map to use for incoming events.
    const ButtonMap& button_map() { return button_map_; }

private:
    // Mouse button map.
    ButtonMap button_map_;

    DISALLOW_COPY_AND_ASSIGN(MouseButtonMapEvdev);
};

} // namspace ui

#endif // UI_EVENTS_OZONE_EVDEV_MOUSE_BUTTON_MAP_EVDEV_H_
