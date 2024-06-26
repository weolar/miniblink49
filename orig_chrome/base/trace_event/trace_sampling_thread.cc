// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/trace_sampling_thread.h"
#include "base/trace_event/trace_event_impl.h"
#include "base/trace_event/trace_log.h"

namespace base {
namespace trace_event {

    class TraceBucketData {
    public:
        TraceBucketData(base::subtle::AtomicWord* bucket,
            const char* name,
            TraceSampleCallback callback);
        ~TraceBucketData();

        TRACE_EVENT_API_ATOMIC_WORD* bucket;
        const char* bucket_name;
        TraceSampleCallback callback;
    };

    TraceSamplingThread::TraceSamplingThread()
        : thread_running_(false)
        , waitable_event_for_testing_(false, false)
    {
    }

    TraceSamplingThread::~TraceSamplingThread() { }

    void TraceSamplingThread::ThreadMain()
    {
        PlatformThread::SetName("Sampling Thread");
        thread_running_ = true;
        const int kSamplingFrequencyMicroseconds = 1000;
        while (!cancellation_flag_.IsSet()) {
            PlatformThread::Sleep(
                TimeDelta::FromMicroseconds(kSamplingFrequencyMicroseconds));
            GetSamples();
            waitable_event_for_testing_.Signal();
        }
    }

    // static
    void TraceSamplingThread::DefaultSamplingCallback(
        TraceBucketData* bucket_data)
    {
        TRACE_EVENT_API_ATOMIC_WORD category_and_name = TRACE_EVENT_API_ATOMIC_LOAD(*bucket_data->bucket);
        if (!category_and_name)
            return;
        const char* const combined = reinterpret_cast<const char* const>(category_and_name);
        const char* category_group;
        const char* name;
        ExtractCategoryAndName(combined, &category_group, &name);
        TRACE_EVENT_API_ADD_TRACE_EVENT(
            TRACE_EVENT_PHASE_SAMPLE,
            TraceLog::GetCategoryGroupEnabled(category_group), name, 0, 0, NULL, NULL,
            NULL, NULL, 0);
    }

    void TraceSamplingThread::GetSamples()
    {
        for (size_t i = 0; i < sample_buckets_.size(); ++i) {
            TraceBucketData* bucket_data = &sample_buckets_[i];
            bucket_data->callback.Run(bucket_data);
        }
    }

    void TraceSamplingThread::RegisterSampleBucket(
        TRACE_EVENT_API_ATOMIC_WORD* bucket,
        const char* const name,
        TraceSampleCallback callback)
    {
        // Access to sample_buckets_ doesn't cause races with the sampling thread
        // that uses the sample_buckets_, because it is guaranteed that
        // RegisterSampleBucket is called before the sampling thread is created.
        DCHECK(!thread_running_);
        sample_buckets_.push_back(TraceBucketData(bucket, name, callback));
    }

    // static
    void TraceSamplingThread::ExtractCategoryAndName(const char* combined,
        const char** category,
        const char** name)
    {
        *category = combined;
        *name = &combined[strlen(combined) + 1];
    }

    void TraceSamplingThread::Stop()
    {
        cancellation_flag_.Set();
    }

    void TraceSamplingThread::WaitSamplingEventForTesting()
    {
        waitable_event_for_testing_.Wait();
    }

    TraceBucketData::TraceBucketData(base::subtle::AtomicWord* bucket,
        const char* name,
        TraceSampleCallback callback)
        : bucket(bucket)
        , bucket_name(name)
        , callback(callback)
    {
    }

    TraceBucketData::~TraceBucketData() { }

} // namespace trace_event
} // namespace base
