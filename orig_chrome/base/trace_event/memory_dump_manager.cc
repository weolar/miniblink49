// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_dump_manager.h"

#include <algorithm>

#include "base/atomic_sequence_num.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/thread_task_runner_handle.h"
#include "base/threading/thread.h"
#include "base/trace_event/memory_dump_provider.h"
#include "base/trace_event/memory_dump_session_state.h"
#include "base/trace_event/memory_profiler_allocation_context.h"
#include "base/trace_event/process_memory_dump.h"
#include "base/trace_event/trace_event_argument.h"
#include "build/build_config.h"

#if !defined(OS_NACL)
#include "base/trace_event/process_memory_totals_dump_provider.h"
#endif

#if defined(OS_LINUX) || defined(OS_ANDROID)
#include "base/trace_event/malloc_dump_provider.h"
#include "base/trace_event/process_memory_maps_dump_provider.h"
#endif

#if defined(OS_ANDROID)
#include "base/trace_event/java_heap_dump_provider_android.h"
#endif

#if defined(OS_WIN)
#include "base/trace_event/winheap_dump_provider_win.h"
#endif

namespace base {
namespace trace_event {

    namespace {

        const int kTraceEventNumArgs = 1;
        const char* kTraceEventArgNames[] = { "dumps" };
        const unsigned char kTraceEventArgTypes[] = { TRACE_VALUE_TYPE_CONVERTABLE };

        StaticAtomicSequenceNumber g_next_guid;
        uint32_t g_periodic_dumps_count = 0;
        uint32_t g_heavy_dumps_rate = 0;
        MemoryDumpManager* g_instance_for_testing = nullptr;

        void RequestPeriodicGlobalDump()
        {
            MemoryDumpLevelOfDetail level_of_detail;
            if (g_heavy_dumps_rate == 0) {
                level_of_detail = MemoryDumpLevelOfDetail::LIGHT;
            } else {
                level_of_detail = g_periodic_dumps_count == 0
                    ? MemoryDumpLevelOfDetail::DETAILED
                    : MemoryDumpLevelOfDetail::LIGHT;

                if (++g_periodic_dumps_count == g_heavy_dumps_rate)
                    g_periodic_dumps_count = 0;
            }

            MemoryDumpManager::GetInstance()->RequestGlobalDump(
                MemoryDumpType::PERIODIC_INTERVAL, level_of_detail);
        }

        // Callback wrapper to hook upon the completion of RequestGlobalDump() and
        // inject trace markers.
        void OnGlobalDumpDone(MemoryDumpCallback wrapped_callback,
            uint64_t dump_guid,
            bool success)
        {
            TRACE_EVENT_NESTABLE_ASYNC_END1(
                MemoryDumpManager::kTraceCategory, "GlobalMemoryDump",
                TRACE_ID_MANGLE(dump_guid), "success", success);

            if (!wrapped_callback.is_null()) {
                wrapped_callback.Run(dump_guid, success);
                wrapped_callback.Reset();
            }
        }

    } // namespace

    // static
    const char* const MemoryDumpManager::kTraceCategory = TRACE_DISABLED_BY_DEFAULT("memory-infra");

    // static
    const int MemoryDumpManager::kMaxConsecutiveFailuresCount = 3;

    // static
    const uint64_t MemoryDumpManager::kInvalidTracingProcessId = 0;

    // static
    const char* const MemoryDumpManager::kSystemAllocatorPoolName =
#if defined(OS_LINUX) || defined(OS_ANDROID)
        MallocDumpProvider::kAllocatedObjects;
#elif defined(OS_WIN)
        WinHeapDumpProvider::kAllocatedObjects;
#else
        nullptr;
#endif

    // static
    MemoryDumpManager* MemoryDumpManager::GetInstance()
    {
        if (g_instance_for_testing)
            return g_instance_for_testing;

        return Singleton<MemoryDumpManager,
            LeakySingletonTraits<MemoryDumpManager>>::get();
    }

    // static
    void MemoryDumpManager::SetInstanceForTesting(MemoryDumpManager* instance)
    {
        g_instance_for_testing = instance;
    }

    MemoryDumpManager::MemoryDumpManager()
        : delegate_(nullptr)
        , is_coordinator_(false)
        , memory_tracing_enabled_(0)
        , tracing_process_id_(kInvalidTracingProcessId)
        , dumper_registrations_ignored_for_testing_(false)
    {
        //   g_next_guid.GetNext();  // Make sure that first guid is not zero.
        //
        //   heap_profiling_enabled_ = CommandLine::InitializedForCurrentProcess()
        //                                 ? CommandLine::ForCurrentProcess()->HasSwitch(
        //                                       switches::kEnableHeapProfiling)
        //                                 : false;
        //
        //   if (heap_profiling_enabled_)
        //     AllocationContextTracker::SetCaptureEnabled(true);
    }

    MemoryDumpManager::~MemoryDumpManager()
    {
        TraceLog::GetInstance()->RemoveEnabledStateObserver(this);
    }

    void MemoryDumpManager::Initialize(MemoryDumpManagerDelegate* delegate,
        bool is_coordinator)
    {
        {
            AutoLock lock(lock_);
            DCHECK(delegate);
            DCHECK(!delegate_);
            delegate_ = delegate;
            is_coordinator_ = is_coordinator;
        }

// Enable the core dump providers.
#if !defined(OS_NACL)
        RegisterDumpProvider(ProcessMemoryTotalsDumpProvider::GetInstance(),
            "ProcessMemoryTotals", nullptr);
#endif

#if defined(OS_LINUX) || defined(OS_ANDROID)
        RegisterDumpProvider(ProcessMemoryMapsDumpProvider::GetInstance(),
            "ProcessMemoryMaps", nullptr);
        RegisterDumpProvider(MallocDumpProvider::GetInstance(), "Malloc", nullptr);
#endif

#if defined(OS_ANDROID)
        RegisterDumpProvider(JavaHeapDumpProvider::GetInstance(), "JavaHeap",
            nullptr);
#endif

#if defined(OS_WIN)
        RegisterDumpProvider(WinHeapDumpProvider::GetInstance(), "WinHeap", nullptr);
#endif

        // If tracing was enabled before initializing MemoryDumpManager, we missed the
        // OnTraceLogEnabled() event. Synthetize it so we can late-join the party.
        bool is_tracing_already_enabled = TraceLog::GetInstance()->IsEnabled();
        TRACE_EVENT0(kTraceCategory, "init"); // Add to trace-viewer category list.
        TraceLog::GetInstance()->AddEnabledStateObserver(this);
        if (is_tracing_already_enabled)
            OnTraceLogEnabled();
    }

    void MemoryDumpManager::RegisterDumpProvider(
        MemoryDumpProvider* mdp,
        const char* name,
        const scoped_refptr<SingleThreadTaskRunner>& task_runner,
        const MemoryDumpProvider::Options& options)
    {
        //   if (dumper_registrations_ignored_for_testing_)
        //     return;
        //
        //   MemoryDumpProviderInfo mdp_info(mdp, name, task_runner, options);
        //   AutoLock lock(lock_);
        //   auto iter_new = dump_providers_.insert(mdp_info);
        //
        //   // If there was a previous entry, replace it with the new one. This is to deal
        //   // with the case where a dump provider unregisters itself and then re-
        //   // registers before a memory dump happens, so its entry was still in the
        //   // collection but flagged |unregistered|.
        //   if (!iter_new.second) {
        //     dump_providers_.erase(iter_new.first);
        //     dump_providers_.insert(mdp_info);
        //   }
        //
        //   if (heap_profiling_enabled_)
        //     mdp->OnHeapProfilingEnabled(true);
    }

    void MemoryDumpManager::RegisterDumpProvider(
        MemoryDumpProvider* mdp,
        const char* name,
        const scoped_refptr<SingleThreadTaskRunner>& task_runner)
    {
        RegisterDumpProvider(mdp, name, task_runner, MemoryDumpProvider::Options());
    }

    void MemoryDumpManager::UnregisterDumpProvider(MemoryDumpProvider* mdp)
    {
        //   AutoLock lock(lock_);
        //
        //   auto mdp_iter = dump_providers_.begin();
        //   for (; mdp_iter != dump_providers_.end(); ++mdp_iter) {
        //     if (mdp_iter->dump_provider == mdp)
        //       break;
        //   }
        //
        //   if (mdp_iter == dump_providers_.end())
        //     return;
        //
        //   // Unregistration of a MemoryDumpProvider while tracing is ongoing is safe
        //   // only if the MDP has specified a thread affinity (via task_runner()) AND
        //   // the unregistration happens on the same thread (so the MDP cannot unregister
        //   // and OnMemoryDump() at the same time).
        //   // Otherwise, it is not possible to guarantee that its unregistration is
        //   // race-free. If you hit this DCHECK, your MDP has a bug.
        //   DCHECK(!subtle::NoBarrier_Load(&memory_tracing_enabled_) ||
        //          (mdp_iter->task_runner &&
        //           mdp_iter->task_runner->BelongsToCurrentThread()))
        //       << "MemoryDumpProvider \"" << mdp_iter->name << "\" attempted to "
        //       << "unregister itself in a racy way. Please file a crbug.";
        //
        //   mdp_iter->unregistered = true;
    }

    void MemoryDumpManager::RequestGlobalDump(
        MemoryDumpType dump_type,
        MemoryDumpLevelOfDetail level_of_detail,
        const MemoryDumpCallback& callback)
    {
        // Bail out immediately if tracing is not enabled at all.
        //   if (!UNLIKELY(subtle::NoBarrier_Load(&memory_tracing_enabled_))) {
        //     if (!callback.is_null())
        //       callback.Run(0u /* guid */, false /* success */);
        //     return;
        //   }
        //
        //   const uint64_t guid =
        //       TraceLog::GetInstance()->MangleEventId(g_next_guid.GetNext());
        //
        //   // Creates an async event to keep track of the global dump evolution.
        //   // The |wrapped_callback| will generate the ASYNC_END event and then invoke
        //   // the real |callback| provided by the caller.
        //   TRACE_EVENT_NESTABLE_ASYNC_BEGIN0(kTraceCategory, "GlobalMemoryDump",
        //                                     TRACE_ID_MANGLE(guid));
        //   MemoryDumpCallback wrapped_callback = Bind(&OnGlobalDumpDone, callback);
        //
        //   // Technically there is no need to grab the |lock_| here as the delegate is
        //   // long-lived and can only be set by Initialize(), which is locked and
        //   // necessarily happens before memory_tracing_enabled_ == true.
        //   // Not taking the |lock_|, though, is lakely make TSan barf and, at this point
        //   // (memory-infra is enabled) we're not in the fast-path anymore.
        //   MemoryDumpManagerDelegate* delegate;
        //   {
        //     AutoLock lock(lock_);
        //     delegate = delegate_;
        //   }
        //
        //   // The delegate will coordinate the IPC broadcast and at some point invoke
        //   // CreateProcessDump() to get a dump for the current process.
        //   MemoryDumpRequestArgs args = {guid, dump_type, level_of_detail};
        //   delegate->RequestGlobalMemoryDump(args, wrapped_callback);
    }

    void MemoryDumpManager::RequestGlobalDump(
        MemoryDumpType dump_type,
        MemoryDumpLevelOfDetail level_of_detail)
    {
        RequestGlobalDump(dump_type, level_of_detail, MemoryDumpCallback());
    }

    void MemoryDumpManager::CreateProcessDump(const MemoryDumpRequestArgs& args,
        const MemoryDumpCallback& callback)
    {
        TRACE_EVENT_NESTABLE_ASYNC_BEGIN0(kTraceCategory, "ProcessMemoryDump",
            TRACE_ID_MANGLE(args.dump_guid));

        scoped_ptr<ProcessMemoryDumpAsyncState> pmd_async_state;
        {
            AutoLock lock(lock_);
            pmd_async_state.reset(new ProcessMemoryDumpAsyncState(
                args, dump_providers_.begin(), session_state_, callback,
                dump_thread_->task_runner()));
        }

        TRACE_EVENT_WITH_FLOW0(kTraceCategory, "MemoryDumpManager::CreateProcessDump",
            TRACE_ID_MANGLE(args.dump_guid),
            TRACE_EVENT_FLAG_FLOW_OUT);

        // Start the thread hop. |dump_providers_| are kept sorted by thread, so
        // ContinueAsyncProcessDump will hop at most once per thread (w.r.t. thread
        // affinity specified by the MemoryDumpProvider(s) in RegisterDumpProvider()).
        ContinueAsyncProcessDump(pmd_async_state.Pass());
    }

    // At most one ContinueAsyncProcessDump() can be active at any time for a given
    // PMD, regardless of status of the |lock_|. |lock_| is used here purely to
    // ensure consistency w.r.t. (un)registrations of |dump_providers_|.
    // The linearization of dump providers' OnMemoryDump invocations is achieved by
    // means of subsequent PostTask(s).
    //
    // 1) Prologue:
    //   - Check if the dump provider is disabled, if so skip the dump.
    //   - Check if we are on the right thread. If not hop and continue there.
    // 2) Invoke the dump provider's OnMemoryDump() (unless skipped).
    // 3) Epilogue:
    //  - Unregister the dump provider if it failed too many times consecutively.
    //  - Advance the |next_dump_provider| iterator to the next dump provider.
    //  - If this was the last hop, create a trace event, add it to the trace
    //    and finalize (invoke callback).

    void MemoryDumpManager::ContinueAsyncProcessDump(
        scoped_ptr<ProcessMemoryDumpAsyncState> pmd_async_state)
    {
        // Initalizes the ThreadLocalEventBuffer to guarantee that the TRACE_EVENTs
        // in the PostTask below don't end up registering their own dump providers
        // (for discounting trace memory overhead) while holding the |lock_|.
        TraceLog::GetInstance()->InitializeThreadLocalEventBufferIfSupported();

        const uint64_t dump_guid = pmd_async_state->req_args.dump_guid;
        const char* dump_provider_name = nullptr;

        // Pid of the target process being dumped. Often kNullProcessId (= current
        // process), non-zero when the coordinator process creates dumps on behalf
        // of child processes (see crbug.com/461788).
        ProcessId pid;

        // DO NOT put any LOG() statement in the locked sections, as in some contexts
        // (GPU process) LOG() ends up performing PostTask/IPCs.
        MemoryDumpProvider* mdp;
        bool skip_dump = false;
        {
            AutoLock lock(lock_);

            auto mdp_info = pmd_async_state->next_dump_provider;
            mdp = mdp_info->dump_provider;
            dump_provider_name = mdp_info->name;
            pid = mdp_info->options.target_pid;

            // If the dump provider did not specify a thread affinity, dump on
            // |dump_thread_|.
            SingleThreadTaskRunner* task_runner = mdp_info->task_runner.get();
            if (!task_runner)
                task_runner = pmd_async_state->dump_thread_task_runner.get();

            // |dump_thread_| might have been Stop()-ed at this point (if tracing was
            // disabled in the meanwhile). In such case the PostTask() below will fail.
            // |task_runner|, however, should always be non-null.
            DCHECK(task_runner);

            if (mdp_info->disabled || mdp_info->unregistered) {
                skip_dump = true;
            } else if (!task_runner->BelongsToCurrentThread()) {
                // It's time to hop onto another thread.

                // Copy the callback + arguments just for the unlikley case in which
                // PostTask fails. In such case the Bind helper will destroy the
                // pmd_async_state and we must keep a copy of the fields to notify the
                // abort.
                MemoryDumpCallback callback = pmd_async_state->callback;
                scoped_refptr<SingleThreadTaskRunner> callback_task_runner = pmd_async_state->callback_task_runner;

                const bool did_post_task = task_runner->PostTask(
                    FROM_HERE, Bind(&MemoryDumpManager::ContinueAsyncProcessDump, Unretained(this), Passed(pmd_async_state.Pass())));
                if (did_post_task)
                    return;

                // The thread is gone. At this point the best thing we can do is to
                // disable the dump provider and abort this dump.
                mdp_info->disabled = true;
                return AbortDumpLocked(callback, callback_task_runner, dump_guid);
            }
        } // AutoLock(lock_)

        // Invoke the dump provider without holding the |lock_|.
        bool finalize = false;
        bool dump_successful = false;

        if (!skip_dump) {
            TRACE_EVENT_WITH_FLOW1(kTraceCategory,
                "MemoryDumpManager::ContinueAsyncProcessDump",
                TRACE_ID_MANGLE(dump_guid),
                TRACE_EVENT_FLAG_FLOW_IN | TRACE_EVENT_FLAG_FLOW_OUT,
                "dump_provider.name", dump_provider_name);
            MemoryDumpArgs args = { pmd_async_state->req_args.level_of_detail };
            ProcessMemoryDump* process_memory_dump = pmd_async_state->GetOrCreateMemoryDumpContainerForProcess(pid);
            dump_successful = mdp->OnMemoryDump(args, process_memory_dump);
        }

        {
            AutoLock lock(lock_);
            auto mdp_info = pmd_async_state->next_dump_provider;
            if (dump_successful) {
                mdp_info->consecutive_failures = 0;
            } else if (!skip_dump) {
                ++mdp_info->consecutive_failures;
                if (mdp_info->consecutive_failures >= kMaxConsecutiveFailuresCount) {
                    mdp_info->disabled = true;
                }
            }
            ++pmd_async_state->next_dump_provider;
            finalize = pmd_async_state->next_dump_provider == dump_providers_.end();

            if (mdp_info->unregistered)
                dump_providers_.erase(mdp_info);
        }

        if (!skip_dump && !dump_successful) {
            LOG(ERROR) << "MemoryDumpProvider \"" << dump_provider_name << "\" failed, "
                       << "possibly due to sandboxing (crbug.com/461788)."
                       << "Disabling dumper for current process. Try --no-sandbox.";
        }

        if (finalize)
            return FinalizeDumpAndAddToTrace(pmd_async_state.Pass());

        ContinueAsyncProcessDump(pmd_async_state.Pass());
    }

    // static
    void MemoryDumpManager::FinalizeDumpAndAddToTrace(
        scoped_ptr<ProcessMemoryDumpAsyncState> pmd_async_state)
    {
        const uint64_t dump_guid = pmd_async_state->req_args.dump_guid;
        if (!pmd_async_state->callback_task_runner->BelongsToCurrentThread()) {
            scoped_refptr<SingleThreadTaskRunner> callback_task_runner = pmd_async_state->callback_task_runner;
            callback_task_runner->PostTask(
                FROM_HERE, Bind(&MemoryDumpManager::FinalizeDumpAndAddToTrace, Passed(pmd_async_state.Pass())));
            return;
        }

        TRACE_EVENT_WITH_FLOW0(kTraceCategory,
            "MemoryDumpManager::FinalizeDumpAndAddToTrace",
            TRACE_ID_MANGLE(dump_guid), TRACE_EVENT_FLAG_FLOW_IN);

        for (const auto& kv : pmd_async_state->process_dumps) {
            ProcessId pid = kv.first; // kNullProcessId for the current process.
            ProcessMemoryDump* process_memory_dump = kv.second;
            TracedValue* traced_value = new TracedValue();
            scoped_refptr<ConvertableToTraceFormat> event_value(traced_value);
            process_memory_dump->AsValueInto(traced_value);
            traced_value->SetString("level_of_detail",
                MemoryDumpLevelOfDetailToString(
                    pmd_async_state->req_args.level_of_detail));
            const char* const event_name = MemoryDumpTypeToString(pmd_async_state->req_args.dump_type);

            TRACE_EVENT_API_ADD_TRACE_EVENT_WITH_PROCESS_ID(
                TRACE_EVENT_PHASE_MEMORY_DUMP,
                TraceLog::GetCategoryGroupEnabled(kTraceCategory), event_name,
                dump_guid, pid, kTraceEventNumArgs, kTraceEventArgNames,
                kTraceEventArgTypes, nullptr /* arg_values */, &event_value,
                TRACE_EVENT_FLAG_HAS_ID);
        }

        if (!pmd_async_state->callback.is_null()) {
            pmd_async_state->callback.Run(dump_guid, true /* success */);
            pmd_async_state->callback.Reset();
        }

        TRACE_EVENT_NESTABLE_ASYNC_END0(kTraceCategory, "ProcessMemoryDump",
            TRACE_ID_MANGLE(dump_guid));
    }

    // static
    void MemoryDumpManager::AbortDumpLocked(
        MemoryDumpCallback callback,
        scoped_refptr<SingleThreadTaskRunner> task_runner,
        uint64_t dump_guid)
    {
        if (callback.is_null())
            return; // There is nothing to NACK.

        // Post the callback even if we are already on the right thread to avoid
        // invoking the callback while holding the lock_.
        task_runner->PostTask(FROM_HERE,
            Bind(callback, dump_guid, false /* success */));
    }

    void MemoryDumpManager::OnTraceLogEnabled()
    {
        //   bool enabled;
        //   TRACE_EVENT_CATEGORY_GROUP_ENABLED(kTraceCategory, &enabled);
        //   if (!enabled)
        //     return;
        //
        //   // Initialize the TraceLog for the current thread. This is to avoid that the
        //   // TraceLog memory dump provider is registered lazily in the PostTask() below
        //   // while the |lock_| is taken;
        //   TraceLog::GetInstance()->InitializeThreadLocalEventBufferIfSupported();
        //
        //   // Spin-up the thread used to invoke unbound dump providers.
        //   scoped_ptr<Thread> dump_thread(new Thread("MemoryInfra"));
        //   if (!dump_thread->Start()) {
        //     LOG(ERROR) << "Failed to start the memory-infra thread for tracing";
        //     return;
        //   }
        //
        //   AutoLock lock(lock_);
        //
        //   DCHECK(delegate_);  // At this point we must have a delegate.
        //
        //   scoped_refptr<StackFrameDeduplicator> stack_frame_deduplicator = nullptr;
        //
        //   if (heap_profiling_enabled_) {
        //     // If heap profiling is enabled, the stack frame deduplicator will be in
        //     // use. Add a metadata event to write its frames.
        //     stack_frame_deduplicator = new StackFrameDeduplicator;
        //     TRACE_EVENT_API_ADD_METADATA_EVENT(
        //         "stackFrames", "stackFrames",
        //         scoped_refptr<ConvertableToTraceFormat>(stack_frame_deduplicator));
        //   }
        //
        //   DCHECK(!dump_thread_);
        //   dump_thread_ = dump_thread.Pass();
        //   session_state_ = new MemoryDumpSessionState(stack_frame_deduplicator);
        //
        //   for (auto it = dump_providers_.begin(); it != dump_providers_.end(); ++it) {
        //     it->disabled = false;
        //     it->consecutive_failures = 0;
        //   }
        //
        //   subtle::NoBarrier_Store(&memory_tracing_enabled_, 1);
        //
        //   // TODO(primiano): This is a temporary hack to disable periodic memory dumps
        //   // when running memory benchmarks until telemetry uses TraceConfig to
        //   // enable/disable periodic dumps. See crbug.com/529184 .
        //   if (!is_coordinator_ ||
        //       CommandLine::ForCurrentProcess()->HasSwitch(
        //           "enable-memory-benchmarking")) {
        //     return;
        //   }
        //
        //   // Enable periodic dumps. At the moment the periodic support is limited to at
        //   // most one low-detail periodic dump and at most one high-detail periodic
        //   // dump. If both are specified the high-detail period must be an integer
        //   // multiple of the low-level one.
        //   g_periodic_dumps_count = 0;
        //   const TraceConfig trace_config =
        //       TraceLog::GetInstance()->GetCurrentTraceConfig();
        //   const TraceConfig::MemoryDumpConfig& config_list =
        //       trace_config.memory_dump_config();
        //   if (config_list.empty())
        //     return;
        //
        //   uint32_t min_timer_period_ms = std::numeric_limits<uint32_t>::max();
        //   uint32_t heavy_dump_period_ms = 0;
        //   DCHECK_LE(config_list.size(), 2u);
        //   for (const TraceConfig::MemoryDumpTriggerConfig& config : config_list) {
        //     DCHECK(config.periodic_interval_ms);
        //     if (config.level_of_detail == MemoryDumpLevelOfDetail::DETAILED)
        //       heavy_dump_period_ms = config.periodic_interval_ms;
        //     min_timer_period_ms =
        //         std::min(min_timer_period_ms, config.periodic_interval_ms);
        //   }
        //   DCHECK_EQ(0u, heavy_dump_period_ms % min_timer_period_ms);
        //   g_heavy_dumps_rate = heavy_dump_period_ms / min_timer_period_ms;
        //
        //   periodic_dump_timer_.Start(FROM_HERE,
        //                              TimeDelta::FromMilliseconds(min_timer_period_ms),
        //                              base::Bind(&RequestPeriodicGlobalDump));
        DebugBreak();
    }

    void MemoryDumpManager::OnTraceLogDisabled()
    {
        subtle::NoBarrier_Store(&memory_tracing_enabled_, 0);
        scoped_ptr<Thread> dump_thread;
        {
            AutoLock lock(lock_);
            dump_thread = dump_thread_.Pass();
            session_state_ = nullptr;
        }

        // Thread stops are blocking and must be performed outside of the |lock_|
        // or will deadlock (e.g., if ContinueAsyncProcessDump() tries to acquire it).
        periodic_dump_timer_.Stop();
        if (dump_thread)
            dump_thread->Stop();
    }

    uint64_t MemoryDumpManager::GetTracingProcessId() const
    {
        return delegate_->GetTracingProcessId();
    }

    MemoryDumpManager::MemoryDumpProviderInfo::MemoryDumpProviderInfo(
        MemoryDumpProvider* dump_provider,
        const char* name,
        const scoped_refptr<SingleThreadTaskRunner>& task_runner,
        const MemoryDumpProvider::Options& options)
        : dump_provider(dump_provider)
        , name(name)
        , task_runner(task_runner)
        , options(options)
        , consecutive_failures(0)
        , disabled(false)
        , unregistered(false)
    {
    }

    MemoryDumpManager::MemoryDumpProviderInfo::~MemoryDumpProviderInfo() { }

    bool MemoryDumpManager::MemoryDumpProviderInfo::operator<(
        const MemoryDumpProviderInfo& other) const
    {
        if (task_runner == other.task_runner)
            return dump_provider < other.dump_provider;
        // Ensure that unbound providers (task_runner == nullptr) always run last.
        return !(task_runner < other.task_runner);
    }

    MemoryDumpManager::ProcessMemoryDumpAsyncState::ProcessMemoryDumpAsyncState(
        MemoryDumpRequestArgs req_args,
        MemoryDumpProviderInfoSet::iterator next_dump_provider,
        const scoped_refptr<MemoryDumpSessionState>& session_state,
        MemoryDumpCallback callback,
        const scoped_refptr<SingleThreadTaskRunner>& dump_thread_task_runner)
        : req_args(req_args)
        , next_dump_provider(next_dump_provider)
        , session_state(session_state)
        , callback(callback)
        , callback_task_runner(MessageLoop::current()->task_runner())
        , dump_thread_task_runner(dump_thread_task_runner)
    {
    }

    MemoryDumpManager::ProcessMemoryDumpAsyncState::~ProcessMemoryDumpAsyncState()
    {
    }

    ProcessMemoryDump* MemoryDumpManager::ProcessMemoryDumpAsyncState::
        GetOrCreateMemoryDumpContainerForProcess(ProcessId pid)
    {
        auto iter = process_dumps.find(pid);
        if (iter == process_dumps.end()) {
            scoped_ptr<ProcessMemoryDump> new_pmd(new ProcessMemoryDump(session_state));
            iter = process_dumps.insert(pid, new_pmd.Pass()).first;
        }
        return iter->second;
    }

} // namespace trace_event
} // namespace base
