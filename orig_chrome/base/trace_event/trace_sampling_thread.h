// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_TRACE_SAMPLING_THREAD_H_
#define BASE_TRACE_EVENT_TRACE_SAMPLING_THREAD_H_

#include "base/synchronization/cancellation_flag.h"
#include "base/synchronization/waitable_event.h"
#include "base/trace_event/trace_event.h"

namespace base {
namespace trace_event {

    class TraceBucketData;
    typedef base::Callback<void(TraceBucketData*)> TraceSampleCallback;

    // This object must be created on the IO thread.
    class TraceSamplingThread : public PlatformThread::Delegate {
    public:
        TraceSamplingThread();
        ~TraceSamplingThread() override;

        // Implementation of PlatformThread::Delegate:
        void ThreadMain() override;

        static void DefaultSamplingCallback(TraceBucketData* bucket_data);

        void Stop();
        void WaitSamplingEventForTesting();

    private:
        friend class TraceLog;

        void GetSamples();
        // Not thread-safe. Once the ThreadMain has been called, this can no longer
        // be called.
        void RegisterSampleBucket(TRACE_EVENT_API_ATOMIC_WORD* bucket,
            const char* const name,
            TraceSampleCallback callback);
        // Splits a combined "category\0name" into the two component parts.
        static void ExtractCategoryAndName(const char* combined,
            const char** category,
            const char** name);
        std::vector<TraceBucketData> sample_buckets_;
        bool thread_running_;
        CancellationFlag cancellation_flag_;
        WaitableEvent waitable_event_for_testing_;
    };

} // namespace trace_event
} // namespace base

#endif // BASE_TRACE_EVENT_TRACE_SAMPLING_THREAD_H_
