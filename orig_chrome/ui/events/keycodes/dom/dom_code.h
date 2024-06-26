// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_KEYCODES_DOM3_DOM_CODE_H_
#define UI_EVENTS_KEYCODES_DOM3_DOM_CODE_H_

namespace ui {

#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) id = usb
#define USB_KEYMAP_DECLARATION enum class DomCode
#include "ui/events/keycodes/dom/keycode_converter_data.inc"
#undef USB_KEYMAP
#undef USB_KEYMAP_DECLARATION

} // namespace ui

#endif // UI_EVENTS_KEYCODES_DOM3_DOM_CODE_H_
