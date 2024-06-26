// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_vector.h"
#include "base/message_loop/message_loop.h"
#include "base/native_library.h"
#include "base/path_service.h"
#include "base/profiler/native_stack_sampler.h"
#include "base/profiler/stack_sampling_profiler.h"
#include "base/run_loop.h"
#include "base/scoped_native_library.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"

#if defined(OS_WIN)
#include <intrin.h>
#include <malloc.h>
#include <windows.h>
#else
#include <alloca.h>
#endif

// STACK_SAMPLING_PROFILER_SUPPORTED is used to conditionally enable the tests
// below for supported platforms (currently Win x64).
#if defined(_WIN64)
#define STACK_SAMPLING_PROFILER_SUPPORTED 1
#endif

#if defined(OS_WIN)
#pragma intrinsic(_ReturnAddress)
#endif

namespace base {

using SamplingParams = StackSamplingProfiler::SamplingParams;
using Frame = StackSamplingProfiler::Frame;
using Module = StackSamplingProfiler::Module;
using Sample = StackSamplingProfiler::Sample;
using CallStackProfile = StackSamplingProfiler::CallStackProfile;
using CallStackProfiles = StackSamplingProfiler::CallStackProfiles;

namespace {

    // Configuration for the frames that appear on the stack.
    struct StackConfiguration {
        enum Config { NORMAL,
            WITH_ALLOCA,
            WITH_OTHER_LIBRARY };

        explicit StackConfiguration(Config config)
            : StackConfiguration(config, nullptr)
        {
            EXPECT_NE(config, WITH_OTHER_LIBRARY);
        }

        StackConfiguration(Config config, NativeLibrary library)
            : config(config)
            , library(library)
        {
            EXPECT_TRUE(config != WITH_OTHER_LIBRARY || library);
        }

        Config config;

        // Only used if config == WITH_OTHER_LIBRARY.
        NativeLibrary library;
    };

    // Signature for a target function that is expected to appear in the stack. See
    // SignalAndWaitUntilSignaled() below. The return value should be a program
    // counter pointer near the end of the function.
    using TargetFunction = const void* (*)(WaitableEvent*, WaitableEvent*,
        const StackConfiguration*);

    // A thread to target for profiling, whose stack is guaranteed to contain
    // SignalAndWaitUntilSignaled() when coordinated with the main thread.
    class TargetThread : public PlatformThread::Delegate {
    public:
        TargetThread(const StackConfiguration& stack_config);

        // PlatformThread::Delegate:
        void ThreadMain() override;

        // Waits for the thread to have started and be executing in
        // SignalAndWaitUntilSignaled().
        void WaitForThreadStart();

        // Allows the thread to return from SignalAndWaitUntilSignaled() and finish
        // execution.
        void SignalThreadToFinish();

        // This function is guaranteed to be executing between calls to
        // WaitForThreadStart() and SignalThreadToFinish() when invoked with
        // |thread_started_event_| and |finish_event_|. Returns a program counter
        // value near the end of the function. May be invoked with null WaitableEvents
        // to just return the program counter.
        //
        // This function is static so that we can get a straightforward address
        // for it in one of the tests below, rather than dealing with the complexity
        // of a member function pointer representation.
        static const void* SignalAndWaitUntilSignaled(
            WaitableEvent* thread_started_event,
            WaitableEvent* finish_event,
            const StackConfiguration* stack_config);

        // Calls into SignalAndWaitUntilSignaled() after allocating memory on the
        // stack with alloca.
        static const void* CallWithAlloca(WaitableEvent* thread_started_event,
            WaitableEvent* finish_event,
            const StackConfiguration* stack_config);

        // Calls into SignalAndWaitUntilSignaled() via a function in
        // base_profiler_test_support_library.
        static const void* CallThroughOtherLibrary(
            WaitableEvent* thread_started_event,
            WaitableEvent* finish_event,
            const StackConfiguration* stack_config);

        PlatformThreadId id() const { return id_; }

    private:
        struct TargetFunctionArgs {
            WaitableEvent* thread_started_event;
            WaitableEvent* finish_event;
            const StackConfiguration* stack_config;
        };

        // Callback function to be provided when calling through the other library.
        static void OtherLibraryCallback(void* arg);

        // Returns the current program counter, or a value very close to it.
        static const void* GetProgramCounter();

        WaitableEvent thread_started_event_;
        WaitableEvent finish_event_;
        PlatformThreadId id_;
        const StackConfiguration stack_config_;

        DISALLOW_COPY_AND_ASSIGN(TargetThread);
    };

    TargetThread::TargetThread(const StackConfiguration& stack_config)
        : thread_started_event_(false, false)
        , finish_event_(false, false)
        , id_(0)
        , stack_config_(stack_config)
    {
    }

    void TargetThread::ThreadMain()
    {
        id_ = PlatformThread::CurrentId();
        switch (stack_config_.config) {
        case StackConfiguration::NORMAL:
            SignalAndWaitUntilSignaled(&thread_started_event_, &finish_event_,
                &stack_config_);
            break;

        case StackConfiguration::WITH_ALLOCA:
            CallWithAlloca(&thread_started_event_, &finish_event_, &stack_config_);
            break;

        case StackConfiguration::WITH_OTHER_LIBRARY:
            CallThroughOtherLibrary(&thread_started_event_, &finish_event_,
                &stack_config_);
            break;
        }
    }

    void TargetThread::WaitForThreadStart()
    {
        thread_started_event_.Wait();
    }

    void TargetThread::SignalThreadToFinish()
    {
        finish_event_.Signal();
    }

    // static
    // Disable inlining for this function so that it gets its own stack frame.
    NOINLINE const void* TargetThread::SignalAndWaitUntilSignaled(
        WaitableEvent* thread_started_event,
        WaitableEvent* finish_event,
        const StackConfiguration* stack_config)
    {
        if (thread_started_event && finish_event) {
            thread_started_event->Signal();
            finish_event->Wait();
        }

        // Volatile to prevent a tail call to GetProgramCounter().
        const void* volatile program_counter = GetProgramCounter();
        return program_counter;
    }

    // static
    // Disable inlining for this function so that it gets its own stack frame.
    NOINLINE const void* TargetThread::CallWithAlloca(
        WaitableEvent* thread_started_event,
        WaitableEvent* finish_event,
        const StackConfiguration* stack_config)
    {
        const size_t alloca_size = 100;
        // Memset to 0 to generate a clean failure.
        std::memset(alloca(alloca_size), 0, alloca_size);

        SignalAndWaitUntilSignaled(thread_started_event, finish_event, stack_config);

        // Volatile to prevent a tail call to GetProgramCounter().
        const void* volatile program_counter = GetProgramCounter();
        return program_counter;
    }

    // static
    NOINLINE const void* TargetThread::CallThroughOtherLibrary(
        WaitableEvent* thread_started_event,
        WaitableEvent* finish_event,
        const StackConfiguration* stack_config)
    {
        if (stack_config) {
            // A function whose arguments are a function accepting void*, and a void*.
            using InvokeCallbackFunction = void (*)(void (*)(void*), void*);
            EXPECT_TRUE(stack_config->library);
            InvokeCallbackFunction function = reinterpret_cast<InvokeCallbackFunction>(
                GetFunctionPointerFromNativeLibrary(stack_config->library,
                    "InvokeCallbackFunction"));
            EXPECT_TRUE(function);

            TargetFunctionArgs args = {
                thread_started_event,
                finish_event,
                stack_config
            };
            (*function)(&OtherLibraryCallback, &args);
        }

        // Volatile to prevent a tail call to GetProgramCounter().
        const void* volatile program_counter = GetProgramCounter();
        return program_counter;
    }

    // static
    void TargetThread::OtherLibraryCallback(void* arg)
    {
        const TargetFunctionArgs* args = static_cast<TargetFunctionArgs*>(arg);
        SignalAndWaitUntilSignaled(args->thread_started_event, args->finish_event,
            args->stack_config);
        // Prevent tail call.
        volatile int i = 0;
        ALLOW_UNUSED_LOCAL(i);
    }

    // static
    // Disable inlining for this function so that it gets its own stack frame.
    NOINLINE const void* TargetThread::GetProgramCounter()
    {
#if defined(OS_WIN)
        return _ReturnAddress();
#else
        return __builtin_return_address(0);
#endif
    }

    // Loads the other library, which defines a function to be called in the
    // WITH_OTHER_LIBRARY configuration.
    NativeLibrary LoadOtherLibrary()
    {
        // The lambda gymnastics works around the fact that we can't use ASSERT_*
        // macros in a function returning non-null.
        const auto load = [](NativeLibrary* library) {
            FilePath other_library_path;
            ASSERT_TRUE(PathService::Get(DIR_EXE, &other_library_path));
            other_library_path = other_library_path.Append(FilePath::FromUTF16Unsafe(
                GetNativeLibraryName(ASCIIToUTF16(
                    "base_profiler_test_support_library"))));
            NativeLibraryLoadError load_error;
            *library = LoadNativeLibrary(other_library_path, &load_error);
            ASSERT_TRUE(*library) << "error loading " << other_library_path.value()
                                  << ": " << load_error.ToString();
        };

        NativeLibrary library;
        load(&library);
        return library;
    }

    // Unloads |library| and returns when it has completed unloading. Unloading a
    // library is asynchronous on Windows, so simply calling UnloadNativeLibrary()
    // is insufficient to ensure it's been unloaded.
    void SynchronousUnloadNativeLibrary(NativeLibrary library)
    {
        UnloadNativeLibrary(library);
#if defined(OS_WIN)
        // NativeLibrary is a typedef for HMODULE, which is actually the base address
        // of the module.
        uintptr_t module_base_address = reinterpret_cast<uintptr_t>(library);
        HMODULE module_handle;
        // Keep trying to get the module handle until the call fails.
        while (::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                   reinterpret_cast<LPCTSTR>(module_base_address),
                   &module_handle)
            || ::GetLastError() != ERROR_MOD_NOT_FOUND) {
            PlatformThread::Sleep(TimeDelta::FromMilliseconds(1));
        }
#else
        NOTIMPLEMENTED();
#endif
    }

    // Called on the profiler thread when complete, to collect profiles.
    void SaveProfiles(CallStackProfiles* profiles,
        const CallStackProfiles& pending_profiles)
    {
        *profiles = pending_profiles;
    }

    // Called on the profiler thread when complete. Collects profiles produced by
    // the profiler, and signals an event to allow the main thread to know that that
    // the profiler is done.
    void SaveProfilesAndSignalEvent(CallStackProfiles* profiles,
        WaitableEvent* event,
        const CallStackProfiles& pending_profiles)
    {
        *profiles = pending_profiles;
        event->Signal();
    }

    // Executes the function with the target thread running and executing within
    // SignalAndWaitUntilSignaled(). Performs all necessary target thread startup
    // and shutdown work before and afterward.
    template <class Function>
    void WithTargetThread(Function function,
        const StackConfiguration& stack_config)
    {
        TargetThread target_thread(stack_config);
        PlatformThreadHandle target_thread_handle;
        EXPECT_TRUE(PlatformThread::Create(0, &target_thread, &target_thread_handle));

        target_thread.WaitForThreadStart();

        function(target_thread.id());

        target_thread.SignalThreadToFinish();

        PlatformThread::Join(target_thread_handle);
    }

    template <class Function>
    void WithTargetThread(Function function)
    {
        WithTargetThread(function, StackConfiguration(StackConfiguration::NORMAL));
    }

    // Captures profiles as specified by |params| on the TargetThread, and returns
    // them in |profiles|. Waits up to |profiler_wait_time| for the profiler to
    // complete.
    void CaptureProfiles(const SamplingParams& params, TimeDelta profiler_wait_time,
        CallStackProfiles* profiles)
    {
        profiles->clear();

        WithTargetThread([&params, profiles, profiler_wait_time](
                             PlatformThreadId target_thread_id) {
            WaitableEvent sampling_thread_completed(true, false);
            const StackSamplingProfiler::CompletedCallback callback = Bind(&SaveProfilesAndSignalEvent, Unretained(profiles),
                Unretained(&sampling_thread_completed));
            StackSamplingProfiler profiler(target_thread_id, params, callback);
            profiler.Start();
            sampling_thread_completed.TimedWait(profiler_wait_time);
            profiler.Stop();
            sampling_thread_completed.Wait();
        });
    }

    // If this executable was linked with /INCREMENTAL (the default for non-official
    // debug and release builds on Windows), function addresses do not correspond to
    // function code itself, but instead to instructions in the Incremental Link
    // Table that jump to the functions. Checks for a jump instruction and if
    // present does a little decompilation to find the function's actual starting
    // address.
    const void* MaybeFixupFunctionAddressForILT(const void* function_address)
    {
#if defined(_WIN64)
        const unsigned char* opcode = reinterpret_cast<const unsigned char*>(function_address);
        if (*opcode == 0xe9) {
            // This is a relative jump instruction. Assume we're in the ILT and compute
            // the function start address from the instruction offset.
            const int32* offset = reinterpret_cast<const int32*>(opcode + 1);
            const unsigned char* next_instruction = reinterpret_cast<const unsigned char*>(offset + 1);
            return next_instruction + *offset;
        }
#endif
        return function_address;
    }

    // Searches through the frames in |sample|, returning an iterator to the first
    // frame that has an instruction pointer within |target_function|. Returns
    // sample.end() if no such frames are found.
    Sample::const_iterator FindFirstFrameWithinFunction(
        const Sample& sample,
        TargetFunction target_function)
    {
        uintptr_t function_start = reinterpret_cast<uintptr_t>(
            MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
                target_function)));
        uintptr_t function_end = reinterpret_cast<uintptr_t>(target_function(nullptr, nullptr, nullptr));
        for (auto it = sample.begin(); it != sample.end(); ++it) {
            if ((it->instruction_pointer >= function_start) && (it->instruction_pointer <= function_end))
                return it;
        }
        return sample.end();
    }

    // Formats a sample into a string that can be output for test diagnostics.
    std::string FormatSampleForDiagnosticOutput(
        const Sample& sample,
        const std::vector<Module>& modules)
    {
        std::string output;
        for (const Frame& frame : sample) {
            output += StringPrintf(
                "0x%p %s\n", reinterpret_cast<const void*>(frame.instruction_pointer),
                modules[frame.module_index].filename.AsUTF8Unsafe().c_str());
        }
        return output;
    }

    // Returns a duration that is longer than the test timeout. We would use
    // TimeDelta::Max() but https://crbug.com/465948.
    TimeDelta AVeryLongTimeDelta() { return TimeDelta::FromDays(1); }

    // Tests the scenario where the library is unloaded after copying the stack, but
    // before walking it. If |wait_until_unloaded| is true, ensures that the
    // asynchronous library loading has completed before walking the stack. If
    // false, the unloading may still be occurring during the stack walk.
    void TestLibraryUnload(bool wait_until_unloaded)
    {
        // Test delegate that supports intervening between the copying of the stack
        // and the walking of the stack.
        class StackCopiedSignaler : public NativeStackSamplerTestDelegate {
        public:
            StackCopiedSignaler(WaitableEvent* stack_copied,
                WaitableEvent* start_stack_walk,
                bool wait_to_walk_stack)
                : stack_copied_(stack_copied)
                , start_stack_walk_(start_stack_walk)
                , wait_to_walk_stack_(wait_to_walk_stack)
            {
            }

            void OnPreStackWalk() override
            {
                stack_copied_->Signal();
                if (wait_to_walk_stack_)
                    start_stack_walk_->Wait();
            }

        private:
            WaitableEvent* const stack_copied_;
            WaitableEvent* const start_stack_walk_;
            const bool wait_to_walk_stack_;
        };

        SamplingParams params;
        params.sampling_interval = TimeDelta::FromMilliseconds(0);
        params.samples_per_burst = 1;

        NativeLibrary other_library = LoadOtherLibrary();
        TargetThread target_thread(StackConfiguration(
            StackConfiguration::WITH_OTHER_LIBRARY,
            other_library));

        PlatformThreadHandle target_thread_handle;
        EXPECT_TRUE(PlatformThread::Create(0, &target_thread, &target_thread_handle));

        target_thread.WaitForThreadStart();

        WaitableEvent sampling_thread_completed(true, false);
        std::vector<CallStackProfile> profiles;
        const StackSamplingProfiler::CompletedCallback callback = Bind(&SaveProfilesAndSignalEvent, Unretained(&profiles),
            Unretained(&sampling_thread_completed));
        WaitableEvent stack_copied(true, false);
        WaitableEvent start_stack_walk(true, false);
        StackCopiedSignaler test_delegate(&stack_copied, &start_stack_walk,
            wait_until_unloaded);
        StackSamplingProfiler profiler(target_thread.id(), params, callback,
            &test_delegate);

        profiler.Start();

        // Wait for the stack to be copied and the target thread to be resumed.
        stack_copied.Wait();

        // Cause the target thread to finish, so that it's no longer executing code in
        // the library we're about to unload.
        target_thread.SignalThreadToFinish();
        PlatformThread::Join(target_thread_handle);

        // Unload the library now that it's not being used.
        if (wait_until_unloaded)
            SynchronousUnloadNativeLibrary(other_library);
        else
            UnloadNativeLibrary(other_library);

        // Let the stack walk commence after unloading the library, if we're waiting
        // on that event.
        start_stack_walk.Signal();

        // Wait for the sampling thread to complete and fill out |profiles|.
        sampling_thread_completed.Wait();

        // Look up the sample.
        ASSERT_EQ(1u, profiles.size());
        const CallStackProfile& profile = profiles[0];
        ASSERT_EQ(1u, profile.samples.size());
        const Sample& sample = profile.samples[0];

        // Check that the stack contains a frame for
        // TargetThread::SignalAndWaitUntilSignaled().
        Sample::const_iterator end_frame = FindFirstFrameWithinFunction(
            sample,
            &TargetThread::SignalAndWaitUntilSignaled);
        ASSERT_TRUE(end_frame != sample.end())
            << "Function at "
            << MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
                   &TargetThread::SignalAndWaitUntilSignaled))
            << " was not found in stack:\n"
            << FormatSampleForDiagnosticOutput(sample, profile.modules);

        if (wait_until_unloaded) {
            // The stack should look like this, resulting in two frames between
            // SignalAndWaitUntilSignaled and the last frame, which should be the one in
            // the now-unloaded library:
            //
            // ... WaitableEvent and system frames ...
            // TargetThread::SignalAndWaitUntilSignaled
            // TargetThread::OtherLibraryCallback
            // InvokeCallbackFunction (in other library, now unloaded)
            EXPECT_EQ(2, (sample.end() - 1) - end_frame)
                << "Stack:\n"
                << FormatSampleForDiagnosticOutput(sample, profile.modules);
        } else {
            // We didn't wait for the asynchonous unloading to complete, so the results
            // are non-deterministic: if the library finished unloading we should have
            // the same stack as |wait_until_unloaded|, if not we should have the full
            // stack. The important thing is that we should not crash.

            if ((sample.end() - 1) - end_frame == 2) {
                // This is the same case as |wait_until_unloaded|.
                return;
            }

            // Check that the stack contains a frame for
            // TargetThread::CallThroughOtherLibrary().
            Sample::const_iterator other_library_frame = FindFirstFrameWithinFunction(
                sample,
                &TargetThread::CallThroughOtherLibrary);
            ASSERT_TRUE(other_library_frame != sample.end())
                << "Function at "
                << MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
                       &TargetThread::CallThroughOtherLibrary))
                << " was not found in stack:\n"
                << FormatSampleForDiagnosticOutput(sample, profile.modules);

            // The stack should look like this, resulting in three frames between
            // SignalAndWaitUntilSignaled and CallThroughOtherLibrary:
            //
            // ... WaitableEvent and system frames ...
            // TargetThread::SignalAndWaitUntilSignaled
            // TargetThread::OtherLibraryCallback
            // InvokeCallbackFunction (in other library)
            // TargetThread::CallThroughOtherLibrary
            EXPECT_EQ(3, other_library_frame - end_frame)
                << "Stack:\n"
                << FormatSampleForDiagnosticOutput(sample, profile.modules);
        }
    }

} // namespace

// Checks that the basic expected information is present in a sampled call stack
// profile.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_Basic Basic
#else
#define MAYBE_Basic DISABLED_Basic
#endif
TEST(StackSamplingProfilerTest, MAYBE_Basic)
{
    SamplingParams params;
    params.sampling_interval = TimeDelta::FromMilliseconds(0);
    params.samples_per_burst = 1;

    std::vector<CallStackProfile> profiles;
    CaptureProfiles(params, AVeryLongTimeDelta(), &profiles);

    // Check that the profile and samples sizes are correct, and the module
    // indices are in range.
    ASSERT_EQ(1u, profiles.size());
    const CallStackProfile& profile = profiles[0];
    ASSERT_EQ(1u, profile.samples.size());
    EXPECT_EQ(params.sampling_interval, profile.sampling_period);
    const Sample& sample = profile.samples[0];
    for (const auto& frame : sample) {
        ASSERT_GE(frame.module_index, 0u);
        ASSERT_LT(frame.module_index, profile.modules.size());
    }

    // Check that the stack contains a frame for
    // TargetThread::SignalAndWaitUntilSignaled() and that the frame has this
    // executable's module.
    Sample::const_iterator loc = FindFirstFrameWithinFunction(
        sample,
        &TargetThread::SignalAndWaitUntilSignaled);
    ASSERT_TRUE(loc != sample.end())
        << "Function at "
        << MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
               &TargetThread::SignalAndWaitUntilSignaled))
        << " was not found in stack:\n"
        << FormatSampleForDiagnosticOutput(sample, profile.modules);
    FilePath executable_path;
    EXPECT_TRUE(PathService::Get(FILE_EXE, &executable_path));
    EXPECT_EQ(executable_path, profile.modules[loc->module_index].filename);
}

// Checks that the profiler handles stacks containing dynamically-allocated
// stack memory.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_Alloca Alloca
#else
#define MAYBE_Alloca DISABLED_Alloca
#endif
TEST(StackSamplingProfilerTest, MAYBE_Alloca)
{
    SamplingParams params;
    params.sampling_interval = TimeDelta::FromMilliseconds(0);
    params.samples_per_burst = 1;

    std::vector<CallStackProfile> profiles;
    WithTargetThread([&params, &profiles](
                         PlatformThreadId target_thread_id) {
        WaitableEvent sampling_thread_completed(true, false);
        const StackSamplingProfiler::CompletedCallback callback = Bind(&SaveProfilesAndSignalEvent, Unretained(&profiles),
            Unretained(&sampling_thread_completed));
        StackSamplingProfiler profiler(target_thread_id, params, callback);
        profiler.Start();
        sampling_thread_completed.Wait();
    },
        StackConfiguration(StackConfiguration::WITH_ALLOCA));

    // Look up the sample.
    ASSERT_EQ(1u, profiles.size());
    const CallStackProfile& profile = profiles[0];
    ASSERT_EQ(1u, profile.samples.size());
    const Sample& sample = profile.samples[0];

    // Check that the stack contains a frame for
    // TargetThread::SignalAndWaitUntilSignaled().
    Sample::const_iterator end_frame = FindFirstFrameWithinFunction(
        sample,
        &TargetThread::SignalAndWaitUntilSignaled);
    ASSERT_TRUE(end_frame != sample.end())
        << "Function at "
        << MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
               &TargetThread::SignalAndWaitUntilSignaled))
        << " was not found in stack:\n"
        << FormatSampleForDiagnosticOutput(sample, profile.modules);

    // Check that the stack contains a frame for TargetThread::CallWithAlloca().
    Sample::const_iterator alloca_frame = FindFirstFrameWithinFunction(
        sample,
        &TargetThread::CallWithAlloca);
    ASSERT_TRUE(alloca_frame != sample.end())
        << "Function at "
        << MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
               &TargetThread::CallWithAlloca))
        << " was not found in stack:\n"
        << FormatSampleForDiagnosticOutput(sample, profile.modules);

    // These frames should be adjacent on the stack.
    EXPECT_EQ(1, alloca_frame - end_frame)
        << "Stack:\n"
        << FormatSampleForDiagnosticOutput(sample, profile.modules);
}

// Checks that the fire-and-forget interface works.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_StartAndRunAsync StartAndRunAsync
#else
#define MAYBE_StartAndRunAsync DISABLED_StartAndRunAsync
#endif
TEST(StackSamplingProfilerTest, MAYBE_StartAndRunAsync)
{
    // StartAndRunAsync requires the caller to have a message loop.
    MessageLoop message_loop;

    SamplingParams params;
    params.samples_per_burst = 1;

    CallStackProfiles profiles;
    WithTargetThread([&params, &profiles](PlatformThreadId target_thread_id) {
        WaitableEvent sampling_thread_completed(false, false);
        const StackSamplingProfiler::CompletedCallback callback = Bind(&SaveProfilesAndSignalEvent, Unretained(&profiles),
            Unretained(&sampling_thread_completed));
        StackSamplingProfiler::StartAndRunAsync(target_thread_id, params, callback);
        RunLoop().RunUntilIdle();
        sampling_thread_completed.Wait();
    });

    ASSERT_EQ(1u, profiles.size());
}

// Checks that the expected number of profiles and samples are present in the
// call stack profiles produced.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_MultipleProfilesAndSamples MultipleProfilesAndSamples
#else
#define MAYBE_MultipleProfilesAndSamples DISABLED_MultipleProfilesAndSamples
#endif
TEST(StackSamplingProfilerTest, MAYBE_MultipleProfilesAndSamples)
{
    SamplingParams params;
    params.burst_interval = params.sampling_interval = TimeDelta::FromMilliseconds(0);
    params.bursts = 2;
    params.samples_per_burst = 3;

    std::vector<CallStackProfile> profiles;
    CaptureProfiles(params, AVeryLongTimeDelta(), &profiles);

    ASSERT_EQ(2u, profiles.size());
    EXPECT_EQ(3u, profiles[0].samples.size());
    EXPECT_EQ(3u, profiles[1].samples.size());
}

// Checks that no call stack profiles are captured if the profiling is stopped
// during the initial delay.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_StopDuringInitialDelay StopDuringInitialDelay
#else
#define MAYBE_StopDuringInitialDelay DISABLED_StopDuringInitialDelay
#endif
TEST(StackSamplingProfilerTest, MAYBE_StopDuringInitialDelay)
{
    SamplingParams params;
    params.initial_delay = TimeDelta::FromSeconds(60);

    std::vector<CallStackProfile> profiles;
    CaptureProfiles(params, TimeDelta::FromMilliseconds(0), &profiles);

    EXPECT_TRUE(profiles.empty());
}

// Checks that the single completed call stack profile is captured if the
// profiling is stopped between bursts.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_StopDuringInterBurstInterval StopDuringInterBurstInterval
#else
#define MAYBE_StopDuringInterBurstInterval DISABLED_StopDuringInterBurstInterval
#endif
TEST(StackSamplingProfilerTest, MAYBE_StopDuringInterBurstInterval)
{
    SamplingParams params;
    params.sampling_interval = TimeDelta::FromMilliseconds(0);
    params.burst_interval = TimeDelta::FromSeconds(60);
    params.bursts = 2;
    params.samples_per_burst = 1;

    std::vector<CallStackProfile> profiles;
    CaptureProfiles(params, TimeDelta::FromMilliseconds(50), &profiles);

    ASSERT_EQ(1u, profiles.size());
    EXPECT_EQ(1u, profiles[0].samples.size());
}

// Checks that incomplete call stack profiles are captured.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_StopDuringInterSampleInterval StopDuringInterSampleInterval
#else
#define MAYBE_StopDuringInterSampleInterval \
    DISABLED_StopDuringInterSampleInterval
#endif
TEST(StackSamplingProfilerTest, MAYBE_StopDuringInterSampleInterval)
{
    SamplingParams params;
    params.sampling_interval = TimeDelta::FromSeconds(60);
    params.samples_per_burst = 2;

    std::vector<CallStackProfile> profiles;
    CaptureProfiles(params, TimeDelta::FromMilliseconds(50), &profiles);

    ASSERT_EQ(1u, profiles.size());
    EXPECT_EQ(1u, profiles[0].samples.size());
}

// Checks that we can destroy the profiler while profiling.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_DestroyProfilerWhileProfiling DestroyProfilerWhileProfiling
#else
#define MAYBE_DestroyProfilerWhileProfiling \
    DISABLED_DestroyProfilerWhileProfiling
#endif
TEST(StackSamplingProfilerTest, MAYBE_DestroyProfilerWhileProfiling)
{
    SamplingParams params;
    params.sampling_interval = TimeDelta::FromMilliseconds(10);

    CallStackProfiles profiles;
    WithTargetThread([&params, &profiles](PlatformThreadId target_thread_id) {
        scoped_ptr<StackSamplingProfiler> profiler;
        profiler.reset(new StackSamplingProfiler(
            target_thread_id, params, Bind(&SaveProfiles, Unretained(&profiles))));
        profiler->Start();
        profiler.reset();

        // Wait longer than a sample interval to catch any use-after-free actions by
        // the profiler thread.
        PlatformThread::Sleep(TimeDelta::FromMilliseconds(50));
    });
}

// Checks that the same profiler may be run multiple times.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_CanRunMultipleTimes CanRunMultipleTimes
#else
#define MAYBE_CanRunMultipleTimes DISABLED_CanRunMultipleTimes
#endif
TEST(StackSamplingProfilerTest, MAYBE_CanRunMultipleTimes)
{
    SamplingParams params;
    params.sampling_interval = TimeDelta::FromMilliseconds(0);
    params.samples_per_burst = 1;

    std::vector<CallStackProfile> profiles;
    CaptureProfiles(params, AVeryLongTimeDelta(), &profiles);
    ASSERT_EQ(1u, profiles.size());

    profiles.clear();
    CaptureProfiles(params, AVeryLongTimeDelta(), &profiles);
    ASSERT_EQ(1u, profiles.size());
}

// Checks that requests to start profiling while another profile is taking place
// are ignored.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_ConcurrentProfiling ConcurrentProfiling
#else
#define MAYBE_ConcurrentProfiling DISABLED_ConcurrentProfiling
#endif
TEST(StackSamplingProfilerTest, MAYBE_ConcurrentProfiling)
{
    WithTargetThread([](PlatformThreadId target_thread_id) {
        SamplingParams params[2];
        params[0].initial_delay = TimeDelta::FromMilliseconds(10);
        params[0].sampling_interval = TimeDelta::FromMilliseconds(0);
        params[0].samples_per_burst = 1;

        params[1].sampling_interval = TimeDelta::FromMilliseconds(0);
        params[1].samples_per_burst = 1;

        CallStackProfiles profiles[2];
        ScopedVector<WaitableEvent> sampling_completed;
        ScopedVector<StackSamplingProfiler> profiler;
        for (int i = 0; i < 2; ++i) {
            sampling_completed.push_back(new WaitableEvent(false, false));
            const StackSamplingProfiler::CompletedCallback callback = Bind(&SaveProfilesAndSignalEvent, Unretained(&profiles[i]),
                Unretained(sampling_completed[i]));
            profiler.push_back(
                new StackSamplingProfiler(target_thread_id, params[i], callback));
        }

        profiler[0]->Start();
        profiler[1]->Start();

        // Wait for one profiler to finish.
        size_t completed_profiler = WaitableEvent::WaitMany(&sampling_completed[0], 2);
        EXPECT_EQ(1u, profiles[completed_profiler].size());

        size_t other_profiler = 1 - completed_profiler;
        // Give the other profiler a chance to run and observe that it hasn't.
        EXPECT_FALSE(sampling_completed[other_profiler]->TimedWait(
            TimeDelta::FromMilliseconds(25)));

        // Start the other profiler again and it should run.
        profiler[other_profiler]->Start();
        sampling_completed[other_profiler]->Wait();
        EXPECT_EQ(1u, profiles[other_profiler].size());
    });
}

// Checks that a stack that runs through another library produces a stack with
// the expected functions.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_OtherLibrary OtherLibrary
#else
#define MAYBE_OtherLibrary DISABLED_OtherLibrary
#endif
TEST(StackSamplingProfilerTest, MAYBE_OtherLibrary)
{
    SamplingParams params;
    params.sampling_interval = TimeDelta::FromMilliseconds(0);
    params.samples_per_burst = 1;

    std::vector<CallStackProfile> profiles;
    {
        ScopedNativeLibrary other_library(LoadOtherLibrary());
        WithTargetThread([&params, &profiles](
                             PlatformThreadId target_thread_id) {
            WaitableEvent sampling_thread_completed(true, false);
            const StackSamplingProfiler::CompletedCallback callback = Bind(&SaveProfilesAndSignalEvent, Unretained(&profiles),
                Unretained(&sampling_thread_completed));
            StackSamplingProfiler profiler(target_thread_id, params, callback);
            profiler.Start();
            sampling_thread_completed.Wait();
        },
            StackConfiguration(StackConfiguration::WITH_OTHER_LIBRARY, other_library.get()));
    }

    // Look up the sample.
    ASSERT_EQ(1u, profiles.size());
    const CallStackProfile& profile = profiles[0];
    ASSERT_EQ(1u, profile.samples.size());
    const Sample& sample = profile.samples[0];

    // Check that the stack contains a frame for
    // TargetThread::CallThroughOtherLibrary().
    Sample::const_iterator other_library_frame = FindFirstFrameWithinFunction(
        sample,
        &TargetThread::CallThroughOtherLibrary);
    ASSERT_TRUE(other_library_frame != sample.end())
        << "Function at "
        << MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
               &TargetThread::CallThroughOtherLibrary))
        << " was not found in stack:\n"
        << FormatSampleForDiagnosticOutput(sample, profile.modules);

    // Check that the stack contains a frame for
    // TargetThread::SignalAndWaitUntilSignaled().
    Sample::const_iterator end_frame = FindFirstFrameWithinFunction(
        sample,
        &TargetThread::SignalAndWaitUntilSignaled);
    ASSERT_TRUE(end_frame != sample.end())
        << "Function at "
        << MaybeFixupFunctionAddressForILT(reinterpret_cast<const void*>(
               &TargetThread::SignalAndWaitUntilSignaled))
        << " was not found in stack:\n"
        << FormatSampleForDiagnosticOutput(sample, profile.modules);

    // The stack should look like this, resulting in three frames between
    // SignalAndWaitUntilSignaled and CallThroughOtherLibrary:
    //
    // ... WaitableEvent and system frames ...
    // TargetThread::SignalAndWaitUntilSignaled
    // TargetThread::OtherLibraryCallback
    // InvokeCallbackFunction (in other library)
    // TargetThread::CallThroughOtherLibrary
    EXPECT_EQ(3, other_library_frame - end_frame)
        << "Stack:\n"
        << FormatSampleForDiagnosticOutput(sample, profile.modules);
}

// Checks that a stack that runs through a library that is unloading produces a
// stack, and doesn't crash.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_UnloadingLibrary UnloadingLibrary
#else
#define MAYBE_UnloadingLibrary DISABLED_UnloadingLibrary
#endif
TEST(StackSamplingProfilerTest, MAYBE_UnloadingLibrary)
{
    TestLibraryUnload(false);
}

// Checks that a stack that runs through a library that has been unloaded
// produces a stack, and doesn't crash.
#if defined(STACK_SAMPLING_PROFILER_SUPPORTED)
#define MAYBE_UnloadedLibrary UnloadedLibrary
#else
#define MAYBE_UnloadedLibrary DISABLED_UnloadedLibrary
#endif
TEST(StackSamplingProfilerTest, MAYBE_UnloadedLibrary)
{
    TestLibraryUnload(true);
}

} // namespace base
