// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_RENDERING_STATS_H_
#define CC_DEBUG_RENDERING_STATS_H_

#include <vector>

#include "base/basictypes.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "cc/base/cc_export.h"
#include "cc/debug/traced_value.h"

namespace cc {

struct CC_EXPORT RenderingStats {
    // Stores a sequence of TimeDelta objects.
    class CC_EXPORT TimeDeltaList {
    public:
        TimeDeltaList();
        ~TimeDeltaList();

        void Append(base::TimeDelta value);
        void AddToTracedValue(const char* name,
            base::trace_event::TracedValue* list_value) const;

        void Add(const TimeDeltaList& other);

        base::TimeDelta GetLastTimeDelta() const;

    private:
        std::vector<base::TimeDelta> values;
    };

    RenderingStats();
    ~RenderingStats();

    // Note: when adding new members, please remember to update Add in
    // rendering_stats.cc.

    int64 frame_count;
    int64 visible_content_area;
    int64 approximated_visible_content_area;
    int64 checkerboarded_visible_content_area;
    int64 checkerboarded_no_recording_content_area;
    int64 checkerboarded_needs_raster_content_area;

    TimeDeltaList draw_duration;
    TimeDeltaList draw_duration_estimate;
    TimeDeltaList begin_main_frame_to_commit_duration;
    TimeDeltaList begin_main_frame_to_commit_duration_estimate;
    TimeDeltaList commit_to_activate_duration;
    TimeDeltaList commit_to_activate_duration_estimate;

    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsTraceableData()
        const;
    void Add(const RenderingStats& other);
};

} // namespace cc

#endif // CC_DEBUG_RENDERING_STATS_H_
