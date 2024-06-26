// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/event_utils.h"

#include <vector>

#include "ui/gfx/display.h"
#include "ui/gfx/screen.h"

namespace ui {

namespace {
    int g_custom_event_types = ET_LAST;
} // namespace

scoped_ptr<Event> EventFromNative(const base::NativeEvent& native_event)
{
    scoped_ptr<Event> event;
    EventType type = EventTypeFromNative(native_event);
    switch (type) {
    case ET_KEY_PRESSED:
    case ET_KEY_RELEASED:
        event.reset(new KeyEvent(native_event));
        break;

    case ET_MOUSE_PRESSED:
    case ET_MOUSE_DRAGGED:
    case ET_MOUSE_RELEASED:
    case ET_MOUSE_MOVED:
    case ET_MOUSE_ENTERED:
    case ET_MOUSE_EXITED:
        event.reset(new MouseEvent(native_event));
        break;

    case ET_MOUSEWHEEL:
        event.reset(new MouseWheelEvent(native_event));
        break;

    case ET_SCROLL_FLING_START:
    case ET_SCROLL_FLING_CANCEL:
    case ET_SCROLL:
        event.reset(new ScrollEvent(native_event));
        break;

    case ET_TOUCH_RELEASED:
    case ET_TOUCH_PRESSED:
    case ET_TOUCH_MOVED:
    case ET_TOUCH_CANCELLED:
        event.reset(new TouchEvent(native_event));
        break;

    default:
        break;
    }
    return event;
}

int RegisterCustomEventType()
{
    return ++g_custom_event_types;
}

base::TimeDelta EventTimeForNow()
{
    return base::TimeDelta::FromInternalValue(
        base::TimeTicks::Now().ToInternalValue());
}

bool ShouldDefaultToNaturalScroll()
{
    return GetInternalDisplayTouchSupport() == gfx::Display::TOUCH_SUPPORT_AVAILABLE;
}

gfx::Display::TouchSupport GetInternalDisplayTouchSupport()
{
    gfx::Screen* screen = gfx::Screen::GetScreen();
    // No screen in some unit tests.
    if (!screen)
        return gfx::Display::TOUCH_SUPPORT_UNKNOWN;
    const std::vector<gfx::Display>& displays = screen->GetAllDisplays();
    for (std::vector<gfx::Display>::const_iterator it = displays.begin();
         it != displays.end(); ++it) {
        if (it->IsInternal())
            return it->touch_support();
    }
    return gfx::Display::TOUCH_SUPPORT_UNAVAILABLE;
}

} // namespace ui
