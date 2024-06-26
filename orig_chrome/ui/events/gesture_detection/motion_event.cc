// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gesture_detection/motion_event.h"

#include "base/logging.h"
#include "ui/events/gesture_detection/motion_event_generic.h"

namespace ui {

size_t MotionEvent::GetHistorySize() const
{
    return 0;
}

base::TimeTicks MotionEvent::GetHistoricalEventTime(
    size_t historical_index) const
{
    NOTIMPLEMENTED();
    return base::TimeTicks();
}

float MotionEvent::GetHistoricalTouchMajor(size_t pointer_index,
    size_t historical_index) const
{
    NOTIMPLEMENTED();
    return 0.f;
}

float MotionEvent::GetHistoricalX(size_t pointer_index,
    size_t historical_index) const
{
    NOTIMPLEMENTED();
    return 0.f;
}

float MotionEvent::GetHistoricalY(size_t pointer_index,
    size_t historical_index) const
{
    NOTIMPLEMENTED();
    return 0.f;
}

int MotionEvent::FindPointerIndexOfId(int id) const
{
    const size_t pointer_count = GetPointerCount();
    for (size_t i = 0; i < pointer_count; ++i) {
        if (GetPointerId(i) == id)
            return static_cast<int>(i);
    }
    return -1;
}

int MotionEvent::GetSourceDeviceId(size_t pointer_index) const
{
    NOTIMPLEMENTED();
    return 0;
}

scoped_ptr<MotionEvent> MotionEvent::Clone() const
{
    return MotionEventGeneric::CloneEvent(*this);
}

scoped_ptr<MotionEvent> MotionEvent::Cancel() const
{
    return MotionEventGeneric::CancelEvent(*this);
}

} // namespace ui
