// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_PAINT_TIME_COUNTER_H_
#define CC_DEBUG_PAINT_TIME_COUNTER_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/debug/ring_buffer.h"

namespace cc {

// Maintains a history of paint times for each frame
class PaintTimeCounter {
public:
    static scoped_ptr<PaintTimeCounter> Create();

    size_t HistorySize() const { return ring_buffer_.BufferSize(); }

    // n = 0 returns the oldest and
    // n = PaintTimeHistorySize() - 1 the most recent paint time.
    base::TimeDelta GetPaintTimeOfRecentFrame(const size_t& n) const;

    void SavePaintTime(const base::TimeDelta& total_paint_time);
    void GetMinAndMaxPaintTime(base::TimeDelta* min, base::TimeDelta* max) const;

    void ClearHistory();

    typedef RingBuffer<base::TimeDelta, 200> RingBufferType;
    RingBufferType::Iterator Begin() const { return ring_buffer_.Begin(); }
    RingBufferType::Iterator End() const { return ring_buffer_.End(); }

private:
    PaintTimeCounter();

    RingBufferType ring_buffer_;

    DISALLOW_COPY_AND_ASSIGN(PaintTimeCounter);
};

} // namespace cc

#endif // CC_DEBUG_PAINT_TIME_COUNTER_H_
