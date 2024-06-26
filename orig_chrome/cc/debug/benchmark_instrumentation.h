// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_BENCHMARK_INSTRUMENTATION_H_
#define CC_DEBUG_BENCHMARK_INSTRUMENTATION_H_

#include "cc/base/cc_export.h"
#include "cc/debug/rendering_stats.h"

namespace cc {
namespace benchmark_instrumentation {

    // Please do not change the trace events in this file without updating
    // tools/perf/measurements/rendering_stats.py accordingly.
    // The benchmarks search for events and their arguments by name.

    namespace internal {
        const char kCategory[] = "cc,benchmark";
        const char kBeginFrameId[] = "begin_frame_id";
    } // namespace internal

    const char kSendBeginFrame[] = "ThreadProxy::ScheduledActionSendBeginMainFrame";
    const char kDoBeginFrame[] = "ThreadProxy::BeginMainFrame";

    class ScopedBeginFrameTask {
    public:
        ScopedBeginFrameTask(const char* event_name, unsigned int begin_frame_id)
            : event_name_(event_name)
        {
            TRACE_EVENT_BEGIN1(internal::kCategory,
                event_name_,
                internal::kBeginFrameId,
                begin_frame_id);
        }
        ~ScopedBeginFrameTask()
        {
            TRACE_EVENT_END0(internal::kCategory, event_name_);
        }

    private:
        const char* event_name_;

        DISALLOW_COPY_AND_ASSIGN(ScopedBeginFrameTask);
    };

    void IssueImplThreadRenderingStatsEvent(const RenderingStats& stats);
    void CC_EXPORT IssueDisplayRenderingStatsEvent();

} // namespace benchmark_instrumentation
} // namespace cc

#endif // CC_DEBUG_BENCHMARK_INSTRUMENTATION_H_
