// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_FRAME_TIMING_TRACKER_H_
#define CC_DEBUG_FRAME_TIMING_TRACKER_H_

#include <utility>
#include <vector>

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/base/delayed_unique_notifier.h"

namespace cc {

class LayerTreeHostImpl;

// This class maintains a history of timestamps and rect IDs to communicate
// frame events back to Blink
// TODO(mpb): Start using this. crbug.com/442554
class CC_EXPORT FrameTimingTracker {
public:
    struct CC_EXPORT CompositeTimingEvent {
        CompositeTimingEvent(int, base::TimeTicks);
        ~CompositeTimingEvent();

        int frame_id;
        base::TimeTicks timestamp;
    };

    using CompositeTimingSet = base::hash_map<int64_t, std::vector<CompositeTimingEvent>>;

    struct CC_EXPORT MainFrameTimingEvent {
        MainFrameTimingEvent(int frame_id,
            base::TimeTicks timestamp,
            base::TimeTicks end_time);
        ~MainFrameTimingEvent();

        int frame_id;
        base::TimeTicks timestamp;
        base::TimeTicks end_time;
    };

    using MainFrameTimingSet = base::hash_map<int64_t, std::vector<MainFrameTimingEvent>>;

    static scoped_ptr<FrameTimingTracker> Create(
        LayerTreeHostImpl* layer_tree_host_impl);

    ~FrameTimingTracker();

    // This routine takes all of the individual CompositeEvents stored in the
    // tracker and collects them by "rect_id", as in the example below.
    // [ {f_id1,r_id1,t1}, {f_id2,r_id1,t2}, {f_id3,r_id2,t3} ]
    // ====>
    // [ {r_id1,<{f_id1,t1},{f_id2,t2}>}, {r_id2,<{f_id3,t3}>} ]
    scoped_ptr<CompositeTimingSet> GroupCompositeCountsByRectId();

    // This routine takes all of the individual MainFrameEvents stored in the
    // tracker and collects them by "rect_id", as in the example below.
    scoped_ptr<MainFrameTimingSet> GroupMainFrameCountsByRectId();

    // This routine takes a timestamp and an array of frame_id,rect_id pairs
    // and generates CompositeTimingEvents (frame_id, timestamp) and adds them to
    // internal hash_map keyed on rect_id
    using FrameAndRectIds = std::pair<int, int64_t>;
    void SaveTimeStamps(base::TimeTicks timestamp,
        const std::vector<FrameAndRectIds>& frame_ids);

    void SaveMainFrameTimeStamps(const std::vector<int64_t>& request_ids,
        base::TimeTicks main_frame_time,
        base::TimeTicks end_time,
        int source_frame_number);

private:
    explicit FrameTimingTracker(LayerTreeHostImpl* layer_tree_host_impl);

    void PostEvents();

    scoped_ptr<CompositeTimingSet> composite_events_;
    scoped_ptr<MainFrameTimingSet> main_frame_events_;

    LayerTreeHostImpl* layer_tree_host_impl_;
    DelayedUniqueNotifier post_events_notifier_;

    DISALLOW_COPY_AND_ASSIGN(FrameTimingTracker);
};

} // namespace cc

#endif // CC_DEBUG_FRAME_TIMING_TRACKER_H_
