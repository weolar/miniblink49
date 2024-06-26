// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/profiler/stack_sampling_profiler.h"

#include <algorithm>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/callback.h"
#include "base/lazy_instance.h"
#include "base/location.h"
#include "base/profiler/native_stack_sampler.h"
#include "base/synchronization/lock.h"
#include "base/thread_task_runner_handle.h"
#include "base/timer/elapsed_timer.h"

namespace base {

namespace {

    // Used to ensure only one profiler is running at a time.
    LazyInstance<Lock> concurrent_profiling_lock = LAZY_INSTANCE_INITIALIZER;

    // AsyncRunner ----------------------------------------------------------------

    // Helper class to allow a profiler to be run completely asynchronously from the
    // initiator, without being concerned with the profiler's lifetime.
    class AsyncRunner {
    public:
        // Sets up a profiler and arranges for it to be deleted on its completed
        // callback.
        static void Run(PlatformThreadId thread_id,
            const StackSamplingProfiler::SamplingParams& params,
            const StackSamplingProfiler::CompletedCallback& callback);

    private:
        AsyncRunner();

        // Runs the callback and deletes the AsyncRunner instance.
        static void RunCallbackAndDeleteInstance(
            scoped_ptr<AsyncRunner> object_to_be_deleted,
            const StackSamplingProfiler::CompletedCallback& callback,
            scoped_refptr<SingleThreadTaskRunner> task_runner,
            const StackSamplingProfiler::CallStackProfiles& profiles);

        scoped_ptr<StackSamplingProfiler> profiler_;

        DISALLOW_COPY_AND_ASSIGN(AsyncRunner);
    };

    // static
    void AsyncRunner::Run(
        PlatformThreadId thread_id,
        const StackSamplingProfiler::SamplingParams& params,
        const StackSamplingProfiler::CompletedCallback& callback)
    {
        scoped_ptr<AsyncRunner> runner(new AsyncRunner);
        AsyncRunner* temp_ptr = runner.get();
        temp_ptr->profiler_.reset(
            new StackSamplingProfiler(thread_id, params,
                Bind(&AsyncRunner::RunCallbackAndDeleteInstance,
                    Passed(&runner), callback,
                    ThreadTaskRunnerHandle::Get())));
        // The callback won't be called until after Start(), so temp_ptr will still
        // be valid here.
        temp_ptr->profiler_->Start();
    }

    AsyncRunner::AsyncRunner() { }

    void AsyncRunner::RunCallbackAndDeleteInstance(
        scoped_ptr<AsyncRunner> object_to_be_deleted,
        const StackSamplingProfiler::CompletedCallback& callback,
        scoped_refptr<SingleThreadTaskRunner> task_runner,
        const StackSamplingProfiler::CallStackProfiles& profiles)
    {
        callback.Run(profiles);
        // Delete the instance on the original calling thread.
        task_runner->DeleteSoon(FROM_HERE, object_to_be_deleted.release());
    }

} // namespace

// StackSamplingProfiler::Module ----------------------------------------------

StackSamplingProfiler::Module::Module()
    : base_address(0u)
{
}
StackSamplingProfiler::Module::Module(uintptr_t base_address,
    const std::string& id,
    const FilePath& filename)
    : base_address(base_address)
    , id(id)
    , filename(filename)
{
}

StackSamplingProfiler::Module::~Module() { }

// StackSamplingProfiler::Frame -----------------------------------------------

StackSamplingProfiler::Frame::Frame(uintptr_t instruction_pointer,
    size_t module_index)
    : instruction_pointer(instruction_pointer)
    , module_index(module_index)
{
}

StackSamplingProfiler::Frame::~Frame() { }

StackSamplingProfiler::Frame::Frame() { }

// StackSamplingProfiler::CallStackProfile ------------------------------------

StackSamplingProfiler::CallStackProfile::CallStackProfile() { }

StackSamplingProfiler::CallStackProfile::~CallStackProfile() { }

// StackSamplingProfiler::SamplingThread --------------------------------------

StackSamplingProfiler::SamplingThread::SamplingThread(
    scoped_ptr<NativeStackSampler> native_sampler,
    const SamplingParams& params,
    const CompletedCallback& completed_callback)
    : native_sampler_(native_sampler.Pass())
    , params_(params)
    , stop_event_(false, false)
    , completed_callback_(completed_callback)
{
}

StackSamplingProfiler::SamplingThread::~SamplingThread() { }

void StackSamplingProfiler::SamplingThread::ThreadMain()
{
    PlatformThread::SetName("Chrome_SamplingProfilerThread");

    // For now, just ignore any requests to profile while another profiler is
    // working.
    if (!concurrent_profiling_lock.Get().Try())
        return;

    CallStackProfiles profiles;
    CollectProfiles(&profiles);
    concurrent_profiling_lock.Get().Release();
    completed_callback_.Run(profiles);
}

// Depending on how long the sampling takes and the length of the sampling
// interval, a burst of samples could take arbitrarily longer than
// samples_per_burst * sampling_interval. In this case, we (somewhat
// arbitrarily) honor the number of samples requested rather than strictly
// adhering to the sampling intervals. Once we have established users for the
// StackSamplingProfiler and the collected data to judge, we may go the other
// way or make this behavior configurable.
void StackSamplingProfiler::SamplingThread::CollectProfile(
    CallStackProfile* profile,
    TimeDelta* elapsed_time,
    bool* was_stopped)
{
    ElapsedTimer profile_timer;
    native_sampler_->ProfileRecordingStarting(&profile->modules);
    profile->sampling_period = params_.sampling_interval;
    *was_stopped = false;
    TimeDelta previous_elapsed_sample_time;
    for (int i = 0; i < params_.samples_per_burst; ++i) {
        if (i != 0) {
            // Always wait, even if for 0 seconds, so we can observe a signal on
            // stop_event_.
            if (stop_event_.TimedWait(
                    std::max(params_.sampling_interval - previous_elapsed_sample_time,
                        TimeDelta()))) {
                *was_stopped = true;
                break;
            }
        }
        ElapsedTimer sample_timer;
        profile->samples.push_back(Sample());
        native_sampler_->RecordStackSample(&profile->samples.back());
        previous_elapsed_sample_time = sample_timer.Elapsed();
    }

    *elapsed_time = profile_timer.Elapsed();
    profile->profile_duration = *elapsed_time;
    native_sampler_->ProfileRecordingStopped();
}

// In an analogous manner to CollectProfile() and samples exceeding the expected
// total sampling time, bursts may also exceed the burst_interval. We adopt the
// same wait-and-see approach here.
void StackSamplingProfiler::SamplingThread::CollectProfiles(
    CallStackProfiles* profiles)
{
    if (stop_event_.TimedWait(params_.initial_delay))
        return;

    TimeDelta previous_elapsed_profile_time;
    for (int i = 0; i < params_.bursts; ++i) {
        if (i != 0) {
            // Always wait, even if for 0 seconds, so we can observe a signal on
            // stop_event_.
            if (stop_event_.TimedWait(
                    std::max(params_.burst_interval - previous_elapsed_profile_time,
                        TimeDelta())))
                return;
        }

        CallStackProfile profile;
        bool was_stopped = false;
        CollectProfile(&profile, &previous_elapsed_profile_time, &was_stopped);
        if (!profile.samples.empty())
            profiles->push_back(profile);

        if (was_stopped)
            return;
    }
}

void StackSamplingProfiler::SamplingThread::Stop()
{
    stop_event_.Signal();
}

// StackSamplingProfiler ------------------------------------------------------

StackSamplingProfiler::SamplingParams::SamplingParams()
    : initial_delay(TimeDelta::FromMilliseconds(0))
    , bursts(1)
    , burst_interval(TimeDelta::FromMilliseconds(10000))
    , samples_per_burst(300)
    , sampling_interval(TimeDelta::FromMilliseconds(100))
{
}

StackSamplingProfiler::StackSamplingProfiler(
    PlatformThreadId thread_id,
    const SamplingParams& params,
    const CompletedCallback& callback)
    : StackSamplingProfiler(thread_id, params, callback, nullptr)
{
}

StackSamplingProfiler::StackSamplingProfiler(
    PlatformThreadId thread_id,
    const SamplingParams& params,
    const CompletedCallback& callback,
    NativeStackSamplerTestDelegate* test_delegate)
    : thread_id_(thread_id)
    , params_(params)
    , completed_callback_(callback)
    , test_delegate_(test_delegate)
{
}

StackSamplingProfiler::~StackSamplingProfiler()
{
    Stop();
    if (!sampling_thread_handle_.is_null())
        PlatformThread::Join(sampling_thread_handle_);
}

// static
void StackSamplingProfiler::StartAndRunAsync(
    PlatformThreadId thread_id,
    const SamplingParams& params,
    const CompletedCallback& callback)
{
    CHECK(ThreadTaskRunnerHandle::Get());
    AsyncRunner::Run(thread_id, params, callback);
}

void StackSamplingProfiler::Start()
{
    if (completed_callback_.is_null())
        return;

    scoped_ptr<NativeStackSampler> native_sampler = NativeStackSampler::Create(thread_id_, test_delegate_);
    if (!native_sampler)
        return;

    sampling_thread_.reset(
        new SamplingThread(native_sampler.Pass(), params_, completed_callback_));
    if (!PlatformThread::Create(0, sampling_thread_.get(),
            &sampling_thread_handle_))
        sampling_thread_.reset();
}

void StackSamplingProfiler::Stop()
{
    if (sampling_thread_)
        sampling_thread_->Stop();
}

// StackSamplingProfiler::Frame global functions ------------------------------

bool operator==(const StackSamplingProfiler::Frame& a,
    const StackSamplingProfiler::Frame& b)
{
    return a.instruction_pointer == b.instruction_pointer && a.module_index == b.module_index;
}

bool operator<(const StackSamplingProfiler::Frame& a,
    const StackSamplingProfiler::Frame& b)
{
    return (a.module_index < b.module_index) || (a.module_index == b.module_index && a.instruction_pointer < b.instruction_pointer);
}

} // namespace base
