// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/paint_time_counter.h"

namespace cc {

// static
scoped_ptr<PaintTimeCounter> PaintTimeCounter::Create()
{
    return make_scoped_ptr(new PaintTimeCounter());
}

PaintTimeCounter::PaintTimeCounter()
{
}

void PaintTimeCounter::SavePaintTime(const base::TimeDelta& paint_time)
{
    ring_buffer_.SaveToBuffer(paint_time);
}

void PaintTimeCounter::GetMinAndMaxPaintTime(base::TimeDelta* min,
    base::TimeDelta* max) const
{
    *min = base::TimeDelta::FromDays(1);
    *max = base::TimeDelta();

    for (RingBufferType::Iterator it = ring_buffer_.Begin(); it; ++it) {
        const base::TimeDelta paint_time = **it;

        if (paint_time < *min)
            *min = paint_time;
        if (paint_time > *max)
            *max = paint_time;
    }

    if (*min > *max)
        *min = *max;
}

void PaintTimeCounter::ClearHistory()
{
    ring_buffer_.Clear();
}

} // namespace cc
