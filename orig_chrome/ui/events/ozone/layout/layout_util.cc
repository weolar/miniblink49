// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/layout/layout_util.h"

#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/dom_key.h"

namespace ui {

int ModifierDomKeyToEventFlag(DomKey key)
{
    switch (key) {
    case DomKey::ALT:
        return EF_ALT_DOWN;
    case DomKey::ALT_GRAPH:
        return EF_ALTGR_DOWN;
    // ChromeOS uses F16 to represent CapsLock before the rewriting stage,
    // based on the historical X11 implementation.
    // TODO post-X11: Switch to use CapsLock uniformly.
    case DomKey::F16:
    case DomKey::CAPS_LOCK:
        return EF_CAPS_LOCK_ON;
    case DomKey::CONTROL:
        return EF_CONTROL_DOWN;
    case DomKey::META:
        return EF_COMMAND_DOWN;
    case DomKey::SHIFT:
        return EF_SHIFT_DOWN;
    case DomKey::SHIFT_LEVEL5:
        return EF_MOD3_DOWN;
    default:
        return EF_NONE;
    }
    // Not represented:
    //   DomKey::ACCEL
    //   DomKey::FN
    //   DomKey::FN_LOCK
    //   DomKey::HYPER
    //   DomKey::NUM_LOCK
    //   DomKey::SCROLL_LOCK
    //   DomKey::SUPER
    //   DomKey::SYMBOL_LOCK
}

} // namespace ui
