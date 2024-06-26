// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_BLINK_BLINK_EVENT_UTIL_H_
#define UI_EVENTS_BLINK_BLINK_EVENT_UTIL_H_

#include "base/memory/scoped_ptr.h"

namespace base {
class TimeDelta;
}

namespace blink {
class WebGestureEvent;
class WebInputEvent;
class WebTouchEvent;
}

namespace gfx {
class PointF;
}

namespace ui {
struct GestureEventData;
struct GestureEventDetails;
class MotionEvent;

blink::WebTouchEvent CreateWebTouchEventFromMotionEvent(
    const MotionEvent& event,
    bool may_cause_scrolling);

blink::WebGestureEvent CreateWebGestureEvent(const GestureEventDetails& details,
    base::TimeDelta timestamp,
    const gfx::PointF& location,
    const gfx::PointF& raw_location,
    int flags);

// Convenience wrapper for |CreateWebGestureEvent| using the supplied |data|.
blink::WebGestureEvent CreateWebGestureEventFromGestureEventData(
    const GestureEventData& data);

int EventFlagsToWebEventModifiers(int flags);

scoped_ptr<blink::WebInputEvent> ScaleWebInputEvent(
    const blink::WebInputEvent& event,
    float scale);

} // namespace ui

#endif // UI_EVENTS_BLINK_BLINK_EVENT_UTIL_H_
