// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_LAYOUT_XKB_XKB_EVDEV_CODES_H_
#define UI_EVENTS_OZONE_LAYOUT_XKB_XKB_EVDEV_CODES_H_

#include "ui/events/ozone/layout/events_ozone_layout_export.h"
#include "ui/events/ozone/layout/xkb/xkb_key_code_converter.h"

namespace ui {

class EVENTS_OZONE_LAYOUT_EXPORT XkbEvdevCodes : public XkbKeyCodeConverter {
public:
    XkbEvdevCodes();
    ~XkbEvdevCodes() override;
    xkb_keycode_t DomCodeToXkbKeyCode(DomCode dom_code) const override;
};

} // namespace ui

#endif // UI_EVENTS_OZONE_LAYOUT_XKB_XKB_EVDEV_CODES_H_
