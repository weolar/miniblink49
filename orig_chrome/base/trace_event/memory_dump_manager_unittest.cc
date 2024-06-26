// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_dump_manager.h"

#include "base/bind_helpers.h"
#include "base/memory/scoped_vector.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/strings/stringprintf.h"
#include "base/synchronization/waitable_event.h"
#include "base/test/test_io_thread.h"
#include "base/test/trace_event_analyzer.h"
#include "base/thread_task_runner_handle.h"
#include "base/threading/thread.h"
#include "base/trace_event/memory_dump_provider.h"
#include "base/trace_event/process_memory_dump.h"
#include "base/trace_event/trace_buffer.h"
#include "base/trace_event/trace_config_memory_test_util.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::_;
using testing::AnyNumber;
using testing::AtMost;
using testing::Between;
using testing::Invoke;
using testing::Return;

namespace base {
namespace trace_event {

    // GTest matchers for MemoryDumpRequestArgs arguments.
    MATCHER(IsDetailedDump, "")
    {
        return arg.level_of_detail == MemoryDumpLevelOfDetail::DETAILED;
    }

    MATCHER(IsLightDump, "")
    {
        return arg.level_of_detail == MemoryDumpLevelOfDetail::LIGHT;
    }

    namespace {

        void RegisterDumpProvider(
            MemoryDumpProvider* mdp,
            const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
            const MemoryDumpProvider::Options& options)
        {
            MemoryDumpManager* mdm = MemoryDumpManager::GetInstance();
            mdm->set_dumper_registrations_ignored_for_testing(false);
            mdm->RegisterDumpProvider(mdp, "TestDumpProvider", task_runner, options);
            mdm->set_dumper_registrations_ignored_for_testing(true);
        }

        void RegisterDumpProvider(MemoryDumpProvider* mdp)
        {
            RegisterDumpProvider(mdp, nullptr, MemoryDumpProvider::Options());
        }

        void OnTraceDataCollected(Closure quit_closure,
            trace_event::TraceResultBuffer* buffer,
            const scoped_refptr<RefCountedString>& json,
            bool has_more_events)
        {
            buffer->AddFragment(json->data());
            if (!has_more_events)
                quit_closure.Run();
        }

    } // namespace

    // Testing MemoryDumpManagerDelegate which, by default, short-circuits dump
    // requests locally to the MemoryDumpManager instead of performing IPC dances.
    class MemoryDumpManagerDelegateForTesting : public MemoryDumpManagerDelegate {
    public:
        MemoryDumpManagerDelegateForTesting()
        {
            ON_CALL(*this, RequestGlobalMemoryDump(_, _))
                .WillByDefault(Invoke(
                    this, &MemoryDumpManagerDelegateForTesting::CreateProcessDump));
        }

        MOCK_METHOD2(RequestGlobalMemoryDump,
            void(const MemoryDumpRequestArgs& args,
                const MemoryDumpCallback& callback));

        uint64 GetTracingProcessId() const override
        {
            NOTREACHED();
            return MemoryDumpManager::kInvalidTracingProcessId;
        }
    };

    class MockMemoryDumpProvider : public MemoryDumpProvider {
    public:
        MOCK_METHOD2(OnMemoryDump,
            bool(const MemoryDumpArgs& args, ProcessMemoryDump* pmd));
    };

    class MemoryDumpManagerTest : public testing::Test {
    public:
        MemoryDumpManagerTest()
            : testing::Test()
            , kDefaultOptions()
        {
        }

        void SetUp() override
        {
            last_callback_success_ = false;
            message_loop_.reset(new MessageLoop());
            mdm_.reset(new MemoryDumpManager());
            MemoryDumpManager::SetInstanceForTesting(mdm_.get());
            ASSERT_EQ(mdm_, MemoryDumpManager::GetInstance());
            delegate_.reset(new MemoryDumpManagerDelegateForTesting);
        }

        void TearDown() override
        {
            MemoryDumpManager::SetInstanceForTesting(nullptr);
            mdm_.reset();
            delegate_.reset();
            message_loop_.reset();
            TraceLog::DeleteForTesting();
        }

        // Turns a Closure into a MemoryDumpCallback, keeping track of the callback
        // result and taking care of posting the closure on the correct task runner.
        void DumpCallbackAdapter(scoped_refptr<SingleThreadTaskRunner> task_runner,
            Closure closure,
            uint64 dump_guid,
            bool success)
        {
            last_callback_success_ = success;
            task_runner->PostTask(FROM_HERE, closure);
        }

    protected:
        void InitializeMemoryDumpManager(bool is_coordinator)
        {
            mdm_->set_dumper_registrations_ignored_for_testing(true);
            mdm_->Initialize(delegate_.get(), is_coordinator);
        }

        void RequestGlobalDumpAndWait(MemoryDumpType dump_type,
            MemoryDumpLevelOfDetail level_of_detail)
        {
            RunLoop run_loop;
            MemoryDumpCallback callback = Bind(&MemoryDumpManagerTest::DumpCallbackAdapter, Unretained(this),
                MessageLoop::current()->task_runner(), run_loop.QuitClosure());
            mdm_->RequestGlobalDump(dump_type, level_of_detail, callback);
            run_loop.Run();
        }

        void EnableTracingWithLegacyCategories(const char* category)
        {
            TraceLog::GetInstance()->SetEnabled(TraceConfig(category, ""),
                TraceLog::RECORDING_MODE);
        }

        void EnableTracingWithTraceConfig(const std::string& trace_config)
        {
            TraceLog::GetInstance()->SetEnabled(TraceConfig(trace_config),
                TraceLog::RECORDING_MODE);
        }

        void DisableTracing() { TraceLog::GetInstance()->SetDisabled(); }

        bool IsPeriodicDumpingEnabled() const
        {
            return mdm_->periodic_dump_timer_.IsRunning();
        }

        int GetMaxConsecutiveFailuresCount() const
        {
            return MemoryDumpManager::kMaxConsecutiveFailuresCount;
        }

        const MemoryDumpProvider::Options kDefaultOptions;
        scoped_ptr<MemoryDumpManager> mdm_;
        scoped_ptr<MemoryDumpManagerDelegateForTesting> delegate_;
        bool last_callback_success_;

    private:
        scoped_ptr<MessageLoop> message_loop_;

        // We want our singleton torn down after each test.
        ShadowingAtExitManager at_exit_manager_;
    };

    // Basic sanity checks. Registers a memory dump provider and checks that it is
    // called, but only when memory-infra is enabled.
    TEST_F(MemoryDumpManagerTest, SingleDumper)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp;
        RegisterDumpProvider(&mdp);

        // Check that the dumper is not called if the memory category is not enabled.
        EnableTracingWithLegacyCategories("foobar-but-not-memory");
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(0);
        EXPECT_CALL(mdp, OnMemoryDump(_, _)).Times(0);
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();

        // Now repeat enabling the memory category and check that the dumper is
        // invoked this time.
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(3);
        EXPECT_CALL(mdp, OnMemoryDump(_, _)).Times(3).WillRepeatedly(Return(true));
        for (int i = 0; i < 3; ++i)
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();

        mdm_->UnregisterDumpProvider(&mdp);

        // Finally check the unregister logic: the delegate will be invoked but not
        // the dump provider, as it has been unregistered.
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp, OnMemoryDump(_, _)).Times(0);
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();
    }

    // Checks that requesting dumps with high level of detail actually propagates
    // the level of the detail properly to OnMemoryDump() call on dump providers.
    TEST_F(MemoryDumpManagerTest, CheckMemoryDumpArgs)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp;

        RegisterDumpProvider(&mdp);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp, OnMemoryDump(IsDetailedDump(), _)).WillOnce(Return(true));
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();
        mdm_->UnregisterDumpProvider(&mdp);

        // Check that requesting dumps with low level of detail actually propagates to
        // OnMemoryDump() call on dump providers.
        RegisterDumpProvider(&mdp);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp, OnMemoryDump(IsLightDump(), _)).WillOnce(Return(true));
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::LIGHT);
        DisableTracing();
        mdm_->UnregisterDumpProvider(&mdp);
    }

    // Checks that the SharedSessionState object is acqually shared over time.
    TEST_F(MemoryDumpManagerTest, SharedSessionState)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp1;
        MockMemoryDumpProvider mdp2;
        RegisterDumpProvider(&mdp1);
        RegisterDumpProvider(&mdp2);

        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        const MemoryDumpSessionState* session_state = mdm_->session_state().get();
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(2);
        EXPECT_CALL(mdp1, OnMemoryDump(_, _))
            .Times(2)
            .WillRepeatedly(Invoke([session_state](const MemoryDumpArgs&,
                                       ProcessMemoryDump* pmd) -> bool {
                EXPECT_EQ(session_state, pmd->session_state().get());
                return true;
            }));
        EXPECT_CALL(mdp2, OnMemoryDump(_, _))
            .Times(2)
            .WillRepeatedly(Invoke([session_state](const MemoryDumpArgs&,
                                       ProcessMemoryDump* pmd) -> bool {
                EXPECT_EQ(session_state, pmd->session_state().get());
                return true;
            }));

        for (int i = 0; i < 2; ++i) {
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
        }

        DisableTracing();
    }

    // Checks that the (Un)RegisterDumpProvider logic behaves sanely.
    TEST_F(MemoryDumpManagerTest, MultipleDumpers)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp1;
        MockMemoryDumpProvider mdp2;

        // Enable only mdp1.
        RegisterDumpProvider(&mdp1);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp1, OnMemoryDump(_, _)).WillOnce(Return(true));
        EXPECT_CALL(mdp2, OnMemoryDump(_, _)).Times(0);
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();

        // Invert: enable mdp1 and disable mdp2.
        mdm_->UnregisterDumpProvider(&mdp1);
        RegisterDumpProvider(&mdp2);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp1, OnMemoryDump(_, _)).Times(0);
        EXPECT_CALL(mdp2, OnMemoryDump(_, _)).WillOnce(Return(true));
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();

        // Enable both mdp1 and mdp2.
        RegisterDumpProvider(&mdp1);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp1, OnMemoryDump(_, _)).WillOnce(Return(true));
        EXPECT_CALL(mdp2, OnMemoryDump(_, _)).WillOnce(Return(true));
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();
    }

    // Checks that the dump provider invocations depend only on the current
    // registration state and not on previous registrations and dumps.
    TEST_F(MemoryDumpManagerTest, RegistrationConsistency)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp;

        RegisterDumpProvider(&mdp);

        {
            EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
            EXPECT_CALL(mdp, OnMemoryDump(_, _)).WillOnce(Return(true));
            EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
            DisableTracing();
        }

        mdm_->UnregisterDumpProvider(&mdp);

        {
            EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
            EXPECT_CALL(mdp, OnMemoryDump(_, _)).Times(0);
            EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
            DisableTracing();
        }

        RegisterDumpProvider(&mdp);
        mdm_->UnregisterDumpProvider(&mdp);

        {
            EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
            EXPECT_CALL(mdp, OnMemoryDump(_, _)).Times(0);
            EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
            DisableTracing();
        }

        RegisterDumpProvider(&mdp);
        mdm_->UnregisterDumpProvider(&mdp);
        RegisterDumpProvider(&mdp);

        {
            EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
            EXPECT_CALL(mdp, OnMemoryDump(_, _)).WillOnce(Return(true));
            EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
            DisableTracing();
        }
    }

    // Checks that the MemoryDumpManager respects the thread affinity when a
    // MemoryDumpProvider specifies a task_runner(). The test starts creating 8
    // threads and registering a MemoryDumpProvider on each of them. At each
    // iteration, one thread is removed, to check the live unregistration logic.
    TEST_F(MemoryDumpManagerTest, RespectTaskRunnerAffinity)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        const uint32 kNumInitialThreads = 8;

        ScopedVector<Thread> threads;
        ScopedVector<MockMemoryDumpProvider> mdps;

        // Create the threads and setup the expectations. Given that at each iteration
        // we will pop out one thread/MemoryDumpProvider, each MDP is supposed to be
        // invoked a number of times equal to its index.
        for (uint32 i = kNumInitialThreads; i > 0; --i) {
            Thread* thread = new Thread("test thread");
            threads.push_back(thread);
            threads.back()->Start();
            scoped_refptr<SingleThreadTaskRunner> task_runner = thread->task_runner();
            MockMemoryDumpProvider* mdp = new MockMemoryDumpProvider();
            mdps.push_back(mdp);
            RegisterDumpProvider(mdp, task_runner, kDefaultOptions);
            EXPECT_CALL(*mdp, OnMemoryDump(_, _))
                .Times(i)
                .WillRepeatedly(Invoke(
                    [task_runner](const MemoryDumpArgs&, ProcessMemoryDump*) -> bool {
                        EXPECT_TRUE(task_runner->RunsTasksOnCurrentThread());
                        return true;
                    }));
        }

        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);

        while (!threads.empty()) {
            last_callback_success_ = false;
            EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
            EXPECT_TRUE(last_callback_success_);

            // Unregister a MDP and destroy one thread at each iteration to check the
            // live unregistration logic. The unregistration needs to happen on the same
            // thread the MDP belongs to.
            {
                RunLoop run_loop;
                Closure unregistration = Bind(&MemoryDumpManager::UnregisterDumpProvider,
                    Unretained(mdm_.get()), Unretained(mdps.back()));
                threads.back()->task_runner()->PostTaskAndReply(FROM_HERE, unregistration,
                    run_loop.QuitClosure());
                run_loop.Run();
            }
            mdps.pop_back();
            threads.back()->Stop();
            threads.pop_back();
        }

        DisableTracing();
    }

    // Checks that providers get disabled after 3 consecutive failures, but not
    // otherwise (e.g., if interleaved).
    TEST_F(MemoryDumpManagerTest, DisableFailingDumpers)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp1;
        MockMemoryDumpProvider mdp2;

        RegisterDumpProvider(&mdp1);
        RegisterDumpProvider(&mdp2);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);

        const int kNumDumps = 2 * GetMaxConsecutiveFailuresCount();
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(kNumDumps);

        EXPECT_CALL(mdp1, OnMemoryDump(_, _))
            .Times(GetMaxConsecutiveFailuresCount())
            .WillRepeatedly(Return(false));

        EXPECT_CALL(mdp2, OnMemoryDump(_, _))
            .WillOnce(Return(false))
            .WillOnce(Return(true))
            .WillOnce(Return(false))
            .WillOnce(Return(false))
            .WillOnce(Return(true))
            .WillOnce(Return(false));

        for (int i = 0; i < kNumDumps; i++) {
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
        }

        DisableTracing();
    }

    // Sneakily registers an extra memory dump provider while an existing one is
    // dumping and expect it to take part in the already active tracing session.
    TEST_F(MemoryDumpManagerTest, RegisterDumperWhileDumping)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp1;
        MockMemoryDumpProvider mdp2;

        RegisterDumpProvider(&mdp1);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);

        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(4);

        EXPECT_CALL(mdp1, OnMemoryDump(_, _))
            .Times(4)
            .WillOnce(Return(true))
            .WillOnce(
                Invoke([&mdp2](const MemoryDumpArgs&, ProcessMemoryDump*) -> bool {
                    RegisterDumpProvider(&mdp2);
                    return true;
                }))
            .WillRepeatedly(Return(true));

        // Depending on the insertion order (before or after mdp1), mdp2 might be
        // called also immediately after it gets registered.
        EXPECT_CALL(mdp2, OnMemoryDump(_, _))
            .Times(Between(2, 3))
            .WillRepeatedly(Return(true));

        for (int i = 0; i < 4; i++) {
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
        }

        DisableTracing();
    }

    // Like RegisterDumperWhileDumping, but unregister the dump provider instead.
    TEST_F(MemoryDumpManagerTest, UnregisterDumperWhileDumping)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp1;
        MockMemoryDumpProvider mdp2;

        RegisterDumpProvider(&mdp1, ThreadTaskRunnerHandle::Get(), kDefaultOptions);
        RegisterDumpProvider(&mdp2, ThreadTaskRunnerHandle::Get(), kDefaultOptions);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);

        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(4);

        EXPECT_CALL(mdp1, OnMemoryDump(_, _))
            .Times(4)
            .WillOnce(Return(true))
            .WillOnce(
                Invoke([&mdp2](const MemoryDumpArgs&, ProcessMemoryDump*) -> bool {
                    MemoryDumpManager::GetInstance()->UnregisterDumpProvider(&mdp2);
                    return true;
                }))
            .WillRepeatedly(Return(true));

        // Depending on the insertion order (before or after mdp1), mdp2 might have
        // been already called when UnregisterDumpProvider happens.
        EXPECT_CALL(mdp2, OnMemoryDump(_, _))
            .Times(Between(1, 2))
            .WillRepeatedly(Return(true));

        for (int i = 0; i < 4; i++) {
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
        }

        DisableTracing();
    }

    // Checks that the dump does not abort when unregistering a provider while
    // dumping from a different thread than the dumping thread.
    TEST_F(MemoryDumpManagerTest, UnregisterDumperFromThreadWhileDumping)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        ScopedVector<TestIOThread> threads;
        ScopedVector<MockMemoryDumpProvider> mdps;

        for (int i = 0; i < 2; i++) {
            threads.push_back(new TestIOThread(TestIOThread::kAutoStart));
            mdps.push_back(new MockMemoryDumpProvider());
            RegisterDumpProvider(mdps.back(), threads.back()->task_runner(),
                kDefaultOptions);
        }

        int on_memory_dump_call_count = 0;

        // When OnMemoryDump is called on either of the dump providers, it will
        // unregister the other one.
        for (MockMemoryDumpProvider* mdp : mdps) {
            int other_idx = (mdps.front() == mdp);
            TestIOThread* other_thread = threads[other_idx];
            MockMemoryDumpProvider* other_mdp = mdps[other_idx];
            auto on_dump = [this, other_thread, other_mdp, &on_memory_dump_call_count](
                               const MemoryDumpArgs& args, ProcessMemoryDump* pmd) {
                other_thread->PostTaskAndWait(
                    FROM_HERE, base::Bind(&MemoryDumpManager::UnregisterDumpProvider, base::Unretained(&*mdm_), other_mdp));
                on_memory_dump_call_count++;
                return true;
            };

            // OnMemoryDump is called once for the provider that dumps first, and zero
            // times for the other provider.
            EXPECT_CALL(*mdp, OnMemoryDump(_, _))
                .Times(AtMost(1))
                .WillOnce(Invoke(on_dump));
        }

        last_callback_success_ = false;
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        ASSERT_EQ(1, on_memory_dump_call_count);
        ASSERT_EQ(true, last_callback_success_);

        DisableTracing();
    }

    // Checks that a NACK callback is invoked if RequestGlobalDump() is called when
    // tracing is not enabled.
    TEST_F(MemoryDumpManagerTest, CallbackCalledOnFailure)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MockMemoryDumpProvider mdp1;
        RegisterDumpProvider(&mdp1);

        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(0);
        EXPECT_CALL(mdp1, OnMemoryDump(_, _)).Times(0);

        last_callback_success_ = true;
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        EXPECT_FALSE(last_callback_success_);
    }

    // Checks that is the MemoryDumpManager is initialized after tracing already
    // began, it will still late-join the party (real use case: startup tracing).
    TEST_F(MemoryDumpManagerTest, InitializedAfterStartOfTracing)
    {
        MockMemoryDumpProvider mdp;
        RegisterDumpProvider(&mdp);
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);

        // First check that a RequestGlobalDump() issued before the MemoryDumpManager
        // initialization gets NACK-ed cleanly.
        {
            EXPECT_CALL(mdp, OnMemoryDump(_, _)).Times(0);
            EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(0);
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
            EXPECT_FALSE(last_callback_success_);
        }

        // Now late-initialize the MemoryDumpManager and check that the
        // RequestGlobalDump completes successfully.
        {
            EXPECT_CALL(mdp, OnMemoryDump(_, _)).Times(1);
            EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
            InitializeMemoryDumpManager(false /* is_coordinator */);
            RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
                MemoryDumpLevelOfDetail::DETAILED);
            EXPECT_TRUE(last_callback_success_);
        }
        DisableTracing();
    }

    // This test (and the MemoryDumpManagerTestCoordinator below) crystallizes the
    // expectations of the chrome://tracing UI and chrome telemetry w.r.t. periodic
    // dumps in memory-infra, handling gracefully the transition between the legacy
    // and the new-style (JSON-based) TraceConfig.
    TEST_F(MemoryDumpManagerTest, TraceConfigExpectations)
    {
        InitializeMemoryDumpManager(false /* is_coordinator */);
        MemoryDumpManagerDelegateForTesting& delegate = *delegate_;

        // Don't trigger the default behavior of the mock delegate in this test,
        // which would short-circuit the dump request to the actual
        // CreateProcessDump().
        // We don't want to create any dump in this test, only check whether the dumps
        // are requested or not.
        ON_CALL(delegate, RequestGlobalMemoryDump(_, _)).WillByDefault(Return());

        // Enabling memory-infra in a non-coordinator process should not trigger any
        // periodic dumps.
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_FALSE(IsPeriodicDumpingEnabled());
        DisableTracing();

        // Enabling memory-infra with the new (JSON) TraceConfig in a non-coordinator
        // process with a fully defined trigger config should NOT enable any periodic
        // dumps.
        EnableTracingWithTraceConfig(
            TraceConfigMemoryTestUtil::GetTraceConfig_PeriodicTriggers(1, 5));
        EXPECT_FALSE(IsPeriodicDumpingEnabled());
        DisableTracing();
    }

    TEST_F(MemoryDumpManagerTest, TraceConfigExpectationsWhenIsCoordinator)
    {
        InitializeMemoryDumpManager(true /* is_coordinator */);
        MemoryDumpManagerDelegateForTesting& delegate = *delegate_;
        ON_CALL(delegate, RequestGlobalMemoryDump(_, _)).WillByDefault(Return());

        // Enabling memory-infra with the legacy TraceConfig (category filter) in
        // a coordinator process should enable periodic dumps.
        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_TRUE(IsPeriodicDumpingEnabled());
        DisableTracing();

        // Enabling memory-infra with the new (JSON) TraceConfig in a coordinator
        // process without specifying any "memory_dump_config" section should enable
        // periodic dumps. This is to preserve the behavior chrome://tracing UI, that
        // is: ticking memory-infra should dump periodically with the default config.
        EnableTracingWithTraceConfig(
            TraceConfigMemoryTestUtil::GetTraceConfig_NoTriggers());
        EXPECT_TRUE(IsPeriodicDumpingEnabled());
        DisableTracing();

        // Enabling memory-infra with the new (JSON) TraceConfig in a coordinator
        // process with an empty "memory_dump_config" should NOT enable periodic
        // dumps. This is the way telemetry is supposed to use memory-infra with
        // only explicitly triggered dumps.
        EnableTracingWithTraceConfig(
            TraceConfigMemoryTestUtil::GetTraceConfig_EmptyTriggers());
        EXPECT_FALSE(IsPeriodicDumpingEnabled());
        DisableTracing();

        // Enabling memory-infra with the new (JSON) TraceConfig in a coordinator
        // process with a fully defined trigger config should cause periodic dumps to
        // be performed in the correct order.
        RunLoop run_loop;
        auto quit_closure = run_loop.QuitClosure();

        const int kHeavyDumpRate = 5;
        const int kLightDumpPeriodMs = 1;
        const int kHeavyDumpPeriodMs = kHeavyDumpRate * kLightDumpPeriodMs;
        // The expected sequence with light=1ms, heavy=5ms is H,L,L,L,L,H,...
        testing::InSequence sequence;
        EXPECT_CALL(delegate, RequestGlobalMemoryDump(IsDetailedDump(), _));
        EXPECT_CALL(delegate, RequestGlobalMemoryDump(IsLightDump(), _))
            .Times(kHeavyDumpRate - 1);
        EXPECT_CALL(delegate, RequestGlobalMemoryDump(IsDetailedDump(), _));
        EXPECT_CALL(delegate, RequestGlobalMemoryDump(IsLightDump(), _))
            .Times(kHeavyDumpRate - 2);
        EXPECT_CALL(delegate, RequestGlobalMemoryDump(IsLightDump(), _))
            .WillOnce(Invoke([quit_closure](const MemoryDumpRequestArgs& args,
                                 const MemoryDumpCallback& callback) {
                ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE, quit_closure);
            }));

        // Swallow all the final spurious calls until tracing gets disabled.
        EXPECT_CALL(delegate, RequestGlobalMemoryDump(_, _)).Times(AnyNumber());

        EnableTracingWithTraceConfig(
            TraceConfigMemoryTestUtil::GetTraceConfig_PeriodicTriggers(
                kLightDumpPeriodMs, kHeavyDumpPeriodMs));
        run_loop.Run();
        DisableTracing();
    }

    // Tests against race conditions that might arise when disabling tracing in the
    // middle of a global memory dump.
    TEST_F(MemoryDumpManagerTest, DisableTracingWhileDumping)
    {
        base::WaitableEvent tracing_disabled_event(false, false);
        InitializeMemoryDumpManager(false /* is_coordinator */);

        // Register a bound dump provider.
        scoped_ptr<Thread> mdp_thread(new Thread("test thread"));
        mdp_thread->Start();
        MockMemoryDumpProvider mdp_with_affinity;
        RegisterDumpProvider(&mdp_with_affinity, mdp_thread->task_runner(),
            kDefaultOptions);

        // Register also an unbound dump provider. Unbound dump providers are always
        // invoked after bound ones.
        MockMemoryDumpProvider unbound_mdp;
        RegisterDumpProvider(&unbound_mdp);

        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp_with_affinity, OnMemoryDump(_, _))
            .Times(1)
            .WillOnce(
                Invoke([&tracing_disabled_event](const MemoryDumpArgs&,
                           ProcessMemoryDump* pmd) -> bool {
                    tracing_disabled_event.Wait();

                    // At this point tracing has been disabled and the
                    // MemoryDumpManager.dump_thread_ has been shut down.
                    return true;
                }));

        // |unbound_mdp| should never be invoked because the thread for unbound dump
        // providers has been shutdown in the meanwhile.
        EXPECT_CALL(unbound_mdp, OnMemoryDump(_, _)).Times(0);

        last_callback_success_ = true;
        RunLoop run_loop;
        MemoryDumpCallback callback = Bind(&MemoryDumpManagerTest::DumpCallbackAdapter, Unretained(this),
            MessageLoop::current()->task_runner(), run_loop.QuitClosure());
        mdm_->RequestGlobalDump(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED, callback);
        DisableTracing();
        tracing_disabled_event.Signal();
        run_loop.Run();

        // RequestGlobalMemoryDump() should be NACK-ed because one of the threads
        // threads died before we had a chance to PostTask onto them.
        EXPECT_FALSE(last_callback_success_);
    }

    TEST_F(MemoryDumpManagerTest, DumpOnBehalfOfOtherProcess)
    {
        using trace_analyzer::Query;

        InitializeMemoryDumpManager(false /* is_coordinator */);

        // Standard provider with default options (create dump for current process).
        MemoryDumpProvider::Options options;
        MockMemoryDumpProvider mdp1;
        RegisterDumpProvider(&mdp1, nullptr, options);

        // Provider with out-of-process dumping.
        MockMemoryDumpProvider mdp2;
        options.target_pid = 123;
        RegisterDumpProvider(&mdp2, nullptr, options);

        // Another provider with out-of-process dumping.
        MockMemoryDumpProvider mdp3;
        options.target_pid = 456;
        RegisterDumpProvider(&mdp3, nullptr, options);

        EnableTracingWithLegacyCategories(MemoryDumpManager::kTraceCategory);
        EXPECT_CALL(*delegate_, RequestGlobalMemoryDump(_, _)).Times(1);
        EXPECT_CALL(mdp1, OnMemoryDump(_, _)).Times(1).WillRepeatedly(Return(true));
        EXPECT_CALL(mdp2, OnMemoryDump(_, _)).Times(1).WillRepeatedly(Return(true));
        EXPECT_CALL(mdp3, OnMemoryDump(_, _)).Times(1).WillRepeatedly(Return(true));
        RequestGlobalDumpAndWait(MemoryDumpType::EXPLICITLY_TRIGGERED,
            MemoryDumpLevelOfDetail::DETAILED);
        DisableTracing();

        // Flush the trace into JSON.
        trace_event::TraceResultBuffer buffer;
        TraceResultBuffer::SimpleOutput trace_output;
        buffer.SetOutputCallback(trace_output.GetCallback());
        RunLoop run_loop;
        buffer.Start();
        trace_event::TraceLog::GetInstance()->Flush(
            Bind(&OnTraceDataCollected, run_loop.QuitClosure(), Unretained(&buffer)));
        run_loop.Run();
        buffer.Finish();

        // Analyze the JSON.
        scoped_ptr<trace_analyzer::TraceAnalyzer> analyzer = make_scoped_ptr(
            trace_analyzer::TraceAnalyzer::Create(trace_output.json_output));
        trace_analyzer::TraceEventVector events;
        analyzer->FindEvents(Query::EventPhaseIs(TRACE_EVENT_PHASE_MEMORY_DUMP),
            &events);

        ASSERT_EQ(3u, events.size());
        ASSERT_EQ(1u, trace_analyzer::CountMatches(events, Query::EventPidIs(123)));
        ASSERT_EQ(1u, trace_analyzer::CountMatches(events, Query::EventPidIs(456)));
        ASSERT_EQ(1u, trace_analyzer::CountMatches(events, Query::EventPidIs(GetCurrentProcId())));
        ASSERT_EQ(events[0]->id, events[1]->id);
        ASSERT_EQ(events[0]->id, events[2]->id);
    }

} // namespace trace_event
} // namespace base
