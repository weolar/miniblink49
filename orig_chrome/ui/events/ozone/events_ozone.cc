// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/events_ozone.h"

#include "ui/events/event.h"

namespace ui {

void DispatchEventFromNativeUiEvent(const base::NativeEvent& native_event,
    base::Callback<void(ui::Event*)> callback)
{
    ui::Event* native_ui_event = static_cast<ui::Event*>(native_event);
    if (native_ui_event->IsKeyEvent()) {
        ui::KeyEvent key_event(native_event);
        callback.Run(&key_event);
    } else if (native_ui_event->IsMouseWheelEvent()) {
        ui::MouseWheelEvent wheel_event(native_event);
        callback.Run(&wheel_event);
    } else if (native_ui_event->IsMouseEvent()) {
        ui::MouseEvent mouse_event(native_event);
        callback.Run(&mouse_event);
    } else if (native_ui_event->IsTouchEvent()) {
        ui::TouchEvent touch_event(native_event);
        callback.Run(&touch_event);
    } else if (native_ui_event->IsScrollEvent()) {
        ui::ScrollEvent scroll_event(native_event);
        callback.Run(&scroll_event);
    } else if (native_ui_event->IsGestureEvent()) {
        callback.Run(native_ui_event);
        // TODO(mohsen): Use the same pattern for scroll/touch/wheel events.
        // Apparently, there is no need for them to wrap the |native_event|.
    } else {
        NOTREACHED();
    }
}

} // namespace ui
