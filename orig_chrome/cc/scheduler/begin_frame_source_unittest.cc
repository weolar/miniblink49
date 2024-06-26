// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <string>

#include "base/basictypes.h"
#include "base/test/test_simple_task_runner.h"
#include "cc/scheduler/begin_frame_source.h"
#include "cc/test/begin_frame_args_test.h"
#include "cc/test/scheduler_test_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

// Macros to help set up expected calls on the MockBeginFrameObserver.
#define EXPECT_BEGIN_FRAME_DROP(obs, frame_time, deadline, interval)                                                                                                                    \
    {                                                                                                                                                                                   \
        ::testing::Expectation exp = EXPECT_CALL((obs), OnBeginFrame(CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, frame_time, deadline, interval))).InSequence((obs).sequence); \
    }

#define EXPECT_BEGIN_FRAME_USED(obs, frame_time, deadline, interval)                                    \
    {                                                                                                   \
        BeginFrameArgs args = CreateBeginFrameArgsForTesting(                                           \
            BEGINFRAME_FROM_HERE, frame_time, deadline, interval);                                      \
        ::testing::Expectation exp = EXPECT_CALL((obs), OnBeginFrame(args)).InSequence((obs).sequence); \
        EXPECT_CALL((obs), LastUsedBeginFrameArgs())                                                    \
            .Times(::testing::AnyNumber())                                                              \
            .After(exp)                                                                                 \
            .WillRepeatedly(::testing::Return(args));                                                   \
    }

// Macros to send BeginFrameArgs on a FakeBeginFrameSink (and verify resulting
// observer behaviour).
#define SEND_BEGIN_FRAME(args_equal_to, source, frame_time, deadline,    \
    interval)                                                            \
    {                                                                    \
        BeginFrameArgs old_args = (source).TestLastUsedBeginFrameArgs(); \
        BeginFrameArgs new_args = CreateBeginFrameArgsForTesting(        \
            BEGINFRAME_FROM_HERE, frame_time, deadline, interval);       \
        ASSERT_FALSE(old_args == new_args);                              \
        (source).TestOnBeginFrame(new_args);                             \
        EXPECT_EQ(args_equal_to, (source).TestLastUsedBeginFrameArgs()); \
    }

// When dropping LastUsedBeginFrameArgs **shouldn't** change.
#define SEND_BEGIN_FRAME_DROP(source, frame_time, deadline, interval) \
    SEND_BEGIN_FRAME(old_args, source, frame_time, deadline, interval);

// When used LastUsedBeginFrameArgs **should** be updated.
#define SEND_BEGIN_FRAME_USED(source, frame_time, deadline, interval) \
    SEND_BEGIN_FRAME(new_args, source, frame_time, deadline, interval);

namespace cc {
namespace {

    class MockBeginFrameObserver : public BeginFrameObserver {
    public:
        MOCK_METHOD1(OnBeginFrame, void(const BeginFrameArgs&));
        MOCK_CONST_METHOD0(LastUsedBeginFrameArgs, const BeginFrameArgs());

        virtual void AsValueInto(base::trace_event::TracedValue* dict) const
        {
            dict->SetString("type", "MockBeginFrameObserver");
            dict->BeginDictionary("last_begin_frame_args");
            LastUsedBeginFrameArgs().AsValueInto(dict);
            dict->EndDictionary();
        }

        // A value different from the normal default returned by a BeginFrameObserver
        // so it is easiable traced back here.
        static const BeginFrameArgs kDefaultBeginFrameArgs;

        MockBeginFrameObserver()
        {
            // Set a "default" value returned by LastUsedBeginFrameArgs so that gMock
            // doesn't fail an assert and instead returns useful information.
            EXPECT_CALL(*this, LastUsedBeginFrameArgs())
                .Times(::testing::AnyNumber())
                .InSequence(sequence)
                .WillRepeatedly(::testing::Return(kDefaultBeginFrameArgs));
        }
        virtual ~MockBeginFrameObserver() { }

        ::testing::Sequence sequence;
    };

    TEST(MockBeginFrameObserverTest, ExpectOnBeginFrame)
    {
        ::testing::NiceMock<MockBeginFrameObserver> obs;
        EXPECT_BEGIN_FRAME_USED(obs, 100, 200, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 400, 600, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 700, 900, 300);

        EXPECT_EQ(obs.LastUsedBeginFrameArgs(),
            MockBeginFrameObserver::kDefaultBeginFrameArgs);

        obs.OnBeginFrame(CreateBeginFrameArgsForTesting(
            BEGINFRAME_FROM_HERE, 100, 200,
            300)); // One call to LastUsedBeginFrameArgs
        EXPECT_EQ(
            obs.LastUsedBeginFrameArgs(),
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 100, 200, 300));

        obs.OnBeginFrame(CreateBeginFrameArgsForTesting(
            BEGINFRAME_FROM_HERE, 400, 600,
            300)); // Multiple calls to LastUsedBeginFrameArgs
        EXPECT_EQ(
            obs.LastUsedBeginFrameArgs(),
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 400, 600, 300));
        EXPECT_EQ(
            obs.LastUsedBeginFrameArgs(),
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 400, 600, 300));

        obs.OnBeginFrame(CreateBeginFrameArgsForTesting(
            BEGINFRAME_FROM_HERE, 700, 900,
            300)); // No calls to LastUsedBeginFrameArgs
    }

    TEST(MockBeginFrameObserverTest, ExpectOnBeginFrameStatus)
    {
        ::testing::NiceMock<MockBeginFrameObserver> obs;
        EXPECT_BEGIN_FRAME_USED(obs, 100, 200, 300);
        EXPECT_BEGIN_FRAME_DROP(obs, 400, 600, 300);
        EXPECT_BEGIN_FRAME_DROP(obs, 450, 650, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 700, 900, 300);

        EXPECT_EQ(obs.LastUsedBeginFrameArgs(),
            MockBeginFrameObserver::kDefaultBeginFrameArgs);

        // Used
        obs.OnBeginFrame(
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 100, 200, 300));
        EXPECT_EQ(
            obs.LastUsedBeginFrameArgs(),
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 100, 200, 300));

        // Dropped
        obs.OnBeginFrame(
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 400, 600, 300));
        EXPECT_EQ(
            obs.LastUsedBeginFrameArgs(),
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 100, 200, 300));

        // Dropped
        obs.OnBeginFrame(
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 450, 650, 300));
        EXPECT_EQ(
            obs.LastUsedBeginFrameArgs(),
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 100, 200, 300));

        // Used
        obs.OnBeginFrame(
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 700, 900, 300));
        EXPECT_EQ(
            obs.LastUsedBeginFrameArgs(),
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 700, 900, 300));
    }

    const BeginFrameArgs MockBeginFrameObserver::kDefaultBeginFrameArgs = CreateBeginFrameArgsForTesting(
#ifdef NDEBUG
        nullptr,
#else
        FROM_HERE_WITH_EXPLICIT_FUNCTION(
            "MockBeginFrameObserver::kDefaultBeginFrameArgs"),
#endif
        -1,
        -1,
        -1);

    // BeginFrameObserverBase testing ---------------------------------------
    class MockMinimalBeginFrameObserverBase : public BeginFrameObserverBase {
    public:
        MOCK_METHOD1(OnBeginFrameDerivedImpl, bool(const BeginFrameArgs&));
        int64_t dropped_begin_frame_args() const { return dropped_begin_frame_args_; }
    };

    TEST(BeginFrameObserverBaseTest, OnBeginFrameImplementation)
    {
        using ::testing::Return;
        MockMinimalBeginFrameObserverBase obs;
        ::testing::InSequence ordered; // These calls should be ordered

        // Initial conditions
        EXPECT_EQ(BeginFrameArgs(), obs.LastUsedBeginFrameArgs());
        EXPECT_EQ(0, obs.dropped_begin_frame_args());

#ifndef NDEBUG
        EXPECT_DEATH({ obs.OnBeginFrame(BeginFrameArgs()); }, "");
#endif

        BeginFrameArgs args1 = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 100, 200, 300);
        EXPECT_CALL(obs, OnBeginFrameDerivedImpl(args1)).WillOnce(Return(true));
        obs.OnBeginFrame(args1);
        EXPECT_EQ(args1, obs.LastUsedBeginFrameArgs());
        EXPECT_EQ(0, obs.dropped_begin_frame_args());

#ifndef NDEBUG
        EXPECT_DEATH({
            obs.OnBeginFrame(CreateBeginFrameArgsForTesting(
                BEGINFRAME_FROM_HERE, 50, 200, 300));
        },
            "");
#endif

        // Returning false shouldn't update the LastUsedBeginFrameArgs value.
        BeginFrameArgs args2 = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 200, 300, 400);
        EXPECT_CALL(obs, OnBeginFrameDerivedImpl(args2)).WillOnce(Return(false));
        obs.OnBeginFrame(args2);
        EXPECT_EQ(args1, obs.LastUsedBeginFrameArgs());
        EXPECT_EQ(1, obs.dropped_begin_frame_args());

        BeginFrameArgs args3 = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 150, 300, 400);
        EXPECT_CALL(obs, OnBeginFrameDerivedImpl(args3)).WillOnce(Return(true));
        obs.OnBeginFrame(args3);
        EXPECT_EQ(args3, obs.LastUsedBeginFrameArgs());
        EXPECT_EQ(1, obs.dropped_begin_frame_args());
    }

    // BeginFrameSource testing ----------------------------------------------
    TEST(BeginFrameSourceBaseTest, ObserverManipulation)
    {
        MockBeginFrameObserver obs;
        MockBeginFrameObserver otherObs;
        FakeBeginFrameSource source;

        source.AddObserver(&obs);
        EXPECT_EQ(&obs, source.GetObserver());

#ifndef NDEBUG
        // Adding an observer when an observer already exists should DCHECK fail.
        EXPECT_DEATH({ source.AddObserver(&otherObs); }, "");

        // Removing wrong observer should DCHECK fail.
        EXPECT_DEATH({ source.RemoveObserver(&otherObs); }, "");

        // Removing an observer when there is no observer should DCHECK fail.
        EXPECT_DEATH({
            source.RemoveObserver(&obs);
            source.RemoveObserver(&obs);
        },
            "");
#endif
        source.RemoveObserver(&obs);

        source.AddObserver(&otherObs);
        EXPECT_EQ(&otherObs, source.GetObserver());
        source.RemoveObserver(&otherObs);
    }

    TEST(BeginFrameSourceBaseTest, Observer)
    {
        FakeBeginFrameSource source;
        MockBeginFrameObserver obs;
        source.AddObserver(&obs);
        EXPECT_BEGIN_FRAME_USED(obs, 100, 200, 300);
        EXPECT_BEGIN_FRAME_DROP(obs, 400, 600, 300);
        EXPECT_BEGIN_FRAME_DROP(obs, 450, 650, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 700, 900, 300);

        SEND_BEGIN_FRAME_USED(source, 100, 200, 300);
        SEND_BEGIN_FRAME_DROP(source, 400, 600, 300);
        SEND_BEGIN_FRAME_DROP(source, 450, 650, 300);
        SEND_BEGIN_FRAME_USED(source, 700, 900, 300);
    }

    TEST(BeginFrameSourceBaseTest, NoObserver)
    {
        FakeBeginFrameSource source;
        SEND_BEGIN_FRAME_DROP(source, 100, 200, 300);
    }

    TEST(BeginFrameSourceBaseTest, NeedsBeginFrames)
    {
        FakeBeginFrameSource source;
        EXPECT_FALSE(source.NeedsBeginFrames());
        source.SetNeedsBeginFrames(true);
        EXPECT_TRUE(source.NeedsBeginFrames());
        source.SetNeedsBeginFrames(false);
        EXPECT_FALSE(source.NeedsBeginFrames());
    }

    class LoopingBeginFrameObserver : public BeginFrameObserverBase {
    public:
        BeginFrameSource* source_;

        void AsValueInto(base::trace_event::TracedValue* dict) const override
        {
            dict->SetString("type", "LoopingBeginFrameObserver");
            dict->BeginDictionary("source");
            source_->AsValueInto(dict);
            dict->EndDictionary();
        }

    protected:
        // BeginFrameObserverBase
        bool OnBeginFrameDerivedImpl(const BeginFrameArgs& args) override
        {
            return true;
        }
    };

    TEST(BeginFrameSourceBaseTest, DetectAsValueIntoLoop)
    {
        LoopingBeginFrameObserver obs;
        FakeBeginFrameSource source;

        obs.source_ = &source;
        source.AddObserver(&obs);

        scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();
        source.AsValueInto(state.get());
    }

    // BackToBackBeginFrameSource testing -----------------------------------------
    class TestBackToBackBeginFrameSource : public BackToBackBeginFrameSource {
    public:
        static scoped_ptr<TestBackToBackBeginFrameSource> Create(
            base::SimpleTestTickClock* now_src,
            base::SingleThreadTaskRunner* task_runner)
        {
            return make_scoped_ptr(
                new TestBackToBackBeginFrameSource(now_src, task_runner));
        }

    protected:
        TestBackToBackBeginFrameSource(base::SimpleTestTickClock* now_src,
            base::SingleThreadTaskRunner* task_runner)
            : BackToBackBeginFrameSource(task_runner)
            , now_src_(now_src)
        {
        }

        base::TimeTicks Now() override { return now_src_->NowTicks(); }

        // Not owned.
        base::SimpleTestTickClock* now_src_;
    };

    class BackToBackBeginFrameSourceTest : public ::testing::Test {
    public:
        static const int64_t kDeadline;
        static const int64_t kInterval;

        scoped_ptr<base::SimpleTestTickClock> now_src_;
        scoped_refptr<OrderedSimpleTaskRunner> task_runner_;
        scoped_ptr<TestBackToBackBeginFrameSource> source_;
        scoped_ptr<MockBeginFrameObserver> obs_;

        void SetUp() override
        {
            now_src_.reset(new base::SimpleTestTickClock());
            now_src_->Advance(base::TimeDelta::FromMicroseconds(1000));
            task_runner_ = make_scoped_refptr(new OrderedSimpleTaskRunner(now_src_.get(), false));
            task_runner_->SetRunTaskLimit(1);
            source_ = TestBackToBackBeginFrameSource::Create(now_src_.get(),
                task_runner_.get());
            obs_ = make_scoped_ptr(new ::testing::StrictMock<MockBeginFrameObserver>());
            source_->AddObserver(obs_.get());
        }

        void TearDown() override { obs_.reset(); }
    };

    const int64_t BackToBackBeginFrameSourceTest::kDeadline = BeginFrameArgs::DefaultInterval().ToInternalValue();

    const int64_t BackToBackBeginFrameSourceTest::kInterval = BeginFrameArgs::DefaultInterval().ToInternalValue();

    TEST_F(BackToBackBeginFrameSourceTest, SetNeedsBeginFramesSendsBeginFrame)
    {
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        source_->SetNeedsBeginFrames(true);
        EXPECT_TRUE(task_runner_->HasPendingTasks());
        task_runner_->RunUntilIdle();

        EXPECT_BEGIN_FRAME_USED(*obs_, 1100, 1100 + kDeadline, kInterval);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));
        source_->DidFinishFrame(0);
        task_runner_->RunUntilIdle();
    }

    TEST_F(BackToBackBeginFrameSourceTest,
        DidFinishFrameThenSetNeedsBeginFramesProducesNoFrame)
    {
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        source_->SetNeedsBeginFrames(true);
        task_runner_->RunUntilIdle();

        source_->SetNeedsBeginFrames(false);
        source_->DidFinishFrame(0);

        EXPECT_FALSE(task_runner_->HasPendingTasks());
    }

    TEST_F(BackToBackBeginFrameSourceTest,
        SetNeedsBeginFramesThenDidFinishFrameProducesNoFrame)
    {
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        source_->SetNeedsBeginFrames(true);
        task_runner_->RunUntilIdle();

        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));
        source_->DidFinishFrame(0);
        source_->SetNeedsBeginFrames(false);

        EXPECT_TRUE(task_runner_->HasPendingTasks());
        task_runner_->RunUntilIdle();
    }

    TEST_F(BackToBackBeginFrameSourceTest,
        DidFinishFrameThenTogglingSetNeedsBeginFramesProducesCorrectFrame)
    {
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        source_->SetNeedsBeginFrames(true);
        task_runner_->RunUntilIdle();

        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));

        source_->SetNeedsBeginFrames(false);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(10));
        source_->DidFinishFrame(0);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(10));
        source_->SetNeedsBeginFrames(false);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(10));
        source_->SetNeedsBeginFrames(true);

        EXPECT_BEGIN_FRAME_USED(*obs_, 1130, 1130 + kDeadline, kInterval);
        EXPECT_TRUE(task_runner_->HasPendingTasks());
        task_runner_->RunUntilIdle();
    }

    TEST_F(BackToBackBeginFrameSourceTest,
        TogglingSetNeedsBeginFramesThenDidFinishFrameProducesCorrectFrame)
    {
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        source_->SetNeedsBeginFrames(true);
        task_runner_->RunUntilIdle();

        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));
        source_->DidFinishFrame(0);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(10));
        source_->SetNeedsBeginFrames(false);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(10));
        source_->SetNeedsBeginFrames(true);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(10));

        EXPECT_BEGIN_FRAME_USED(*obs_, 1130, 1130 + kDeadline, kInterval);
        EXPECT_TRUE(task_runner_->HasPendingTasks());
        task_runner_->RunUntilIdle();
    }

    TEST_F(BackToBackBeginFrameSourceTest, DidFinishFrameNeedsBeginFrameFalse)
    {
        source_->SetNeedsBeginFrames(false);
        source_->DidFinishFrame(0);
        EXPECT_FALSE(task_runner_->RunPendingTasks());
    }

    TEST_F(BackToBackBeginFrameSourceTest, DidFinishFrameRemainingFrames)
    {
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        source_->SetNeedsBeginFrames(true);
        task_runner_->RunUntilIdle();

        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));

        source_->DidFinishFrame(3);
        EXPECT_FALSE(task_runner_->HasPendingTasks());
        source_->DidFinishFrame(2);
        EXPECT_FALSE(task_runner_->HasPendingTasks());
        source_->DidFinishFrame(1);
        EXPECT_FALSE(task_runner_->HasPendingTasks());

        EXPECT_BEGIN_FRAME_USED(*obs_, 1100, 1100 + kDeadline, kInterval);
        source_->DidFinishFrame(0);
        EXPECT_EQ(base::TimeDelta(), task_runner_->DelayToNextTaskTime());
        task_runner_->RunUntilIdle();
    }

    TEST_F(BackToBackBeginFrameSourceTest, DidFinishFrameMultipleCallsIdempotent)
    {
        source_->SetNeedsBeginFrames(true);
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        task_runner_->RunUntilIdle();

        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));
        source_->DidFinishFrame(0);
        source_->DidFinishFrame(0);
        source_->DidFinishFrame(0);
        EXPECT_BEGIN_FRAME_USED(*obs_, 1100, 1100 + kDeadline, kInterval);
        task_runner_->RunUntilIdle();

        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));
        source_->DidFinishFrame(0);
        source_->DidFinishFrame(0);
        source_->DidFinishFrame(0);
        EXPECT_BEGIN_FRAME_USED(*obs_, 1200, 1200 + kDeadline, kInterval);
        task_runner_->RunUntilIdle();
    }

    TEST_F(BackToBackBeginFrameSourceTest, DelayInPostedTaskProducesCorrectFrame)
    {
        EXPECT_BEGIN_FRAME_USED(*obs_, 1000, 1000 + kDeadline, kInterval);
        source_->SetNeedsBeginFrames(true);
        task_runner_->RunUntilIdle();

        now_src_->Advance(base::TimeDelta::FromMicroseconds(100));
        source_->DidFinishFrame(0);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(50));
        EXPECT_BEGIN_FRAME_USED(*obs_, 1150, 1150 + kDeadline, kInterval);

        EXPECT_TRUE(task_runner_->HasPendingTasks());
        task_runner_->RunUntilIdle();
    }

    // SyntheticBeginFrameSource testing ------------------------------------------
    class SyntheticBeginFrameSourceTest : public ::testing::Test {
    public:
        scoped_ptr<base::SimpleTestTickClock> now_src_;
        scoped_refptr<OrderedSimpleTaskRunner> task_runner_;
        scoped_ptr<TestSyntheticBeginFrameSource> source_;
        scoped_ptr<MockBeginFrameObserver> obs_;

        void SetUp() override
        {
            now_src_.reset(new base::SimpleTestTickClock());
            now_src_->Advance(base::TimeDelta::FromMicroseconds(1000));
            task_runner_ = make_scoped_refptr(new OrderedSimpleTaskRunner(now_src_.get(), false));
            source_ = TestSyntheticBeginFrameSource::Create(
                now_src_.get(), task_runner_.get(),
                base::TimeDelta::FromMicroseconds(10000));
            obs_ = make_scoped_ptr(new MockBeginFrameObserver());
            source_->AddObserver(obs_.get());
        }

        void TearDown() override { obs_.reset(); }
    };

    TEST_F(SyntheticBeginFrameSourceTest,
        SetNeedsBeginFramesCallsOnBeginFrameWithMissedTick)
    {
        now_src_->Advance(base::TimeDelta::FromMicroseconds(9010));
        EXPECT_CALL((*obs_), OnBeginFrame(CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 10000, 20000, 10000, BeginFrameArgs::MISSED)));
        source_->SetNeedsBeginFrames(true); // Should cause the last tick to be sent
        // No tasks should need to be run for this to occur.
    }

    TEST_F(SyntheticBeginFrameSourceTest,
        SetNeedsBeginFramesCallsCausesOnBeginFrame)
    {
        source_->SetNeedsBeginFrames(true);
        EXPECT_EQ(10000, task_runner_->NextTaskTime().ToInternalValue());

        EXPECT_BEGIN_FRAME_USED(*obs_, 10000, 20000, 10000);
        now_src_->Advance(base::TimeDelta::FromMicroseconds(9010));
        task_runner_->RunPendingTasks();
    }

    TEST_F(SyntheticBeginFrameSourceTest, BasicOperation)
    {
        task_runner_->SetAutoAdvanceNowToPendingTasks(true);

        source_->SetNeedsBeginFrames(true);
        EXPECT_BEGIN_FRAME_USED(*obs_, 10000, 20000, 10000);
        EXPECT_BEGIN_FRAME_USED(*obs_, 20000, 30000, 10000);
        EXPECT_BEGIN_FRAME_USED(*obs_, 30000, 40000, 10000);
        task_runner_->RunUntilTime(base::TimeTicks::FromInternalValue(30001));

        source_->SetNeedsBeginFrames(false);
        // No new frames....
        task_runner_->RunUntilTime(base::TimeTicks::FromInternalValue(60000));
    }

    TEST_F(SyntheticBeginFrameSourceTest, VSyncChanges)
    {
        task_runner_->SetAutoAdvanceNowToPendingTasks(true);
        source_->SetNeedsBeginFrames(true);

        EXPECT_BEGIN_FRAME_USED(*obs_, 10000, 20000, 10000);
        EXPECT_BEGIN_FRAME_USED(*obs_, 20000, 30000, 10000);
        EXPECT_BEGIN_FRAME_USED(*obs_, 30000, 40000, 10000);
        task_runner_->RunUntilTime(base::TimeTicks::FromInternalValue(30001));

        // Update the vsync information
        source_->OnUpdateVSyncParameters(base::TimeTicks::FromInternalValue(27500),
            base::TimeDelta::FromMicroseconds(10001));

        EXPECT_BEGIN_FRAME_USED(*obs_, 40000, 47502, 10001);
        EXPECT_BEGIN_FRAME_USED(*obs_, 47502, 57503, 10001);
        EXPECT_BEGIN_FRAME_USED(*obs_, 57503, 67504, 10001);
        task_runner_->RunUntilTime(base::TimeTicks::FromInternalValue(60000));
    }

    // BeginFrameSourceMultiplexer testing -----------------------------------
    class BeginFrameSourceMultiplexerTest : public ::testing::Test {
    protected:
        void SetUp() override
        {
            mux_ = BeginFrameSourceMultiplexer::Create();

            source1_store_ = make_scoped_ptr(new FakeBeginFrameSource());
            source2_store_ = make_scoped_ptr(new FakeBeginFrameSource());
            source3_store_ = make_scoped_ptr(new FakeBeginFrameSource());

            source1_ = source1_store_.get();
            source2_ = source2_store_.get();
            source3_ = source3_store_.get();
        }

        void TearDown() override
        {
            // Make sure the mux is torn down before the sources.
            mux_.reset();
        }

        scoped_ptr<BeginFrameSourceMultiplexer> mux_;
        FakeBeginFrameSource* source1_;
        FakeBeginFrameSource* source2_;
        FakeBeginFrameSource* source3_;

    private:
        scoped_ptr<FakeBeginFrameSource> source1_store_;
        scoped_ptr<FakeBeginFrameSource> source2_store_;
        scoped_ptr<FakeBeginFrameSource> source3_store_;
    };

    TEST_F(BeginFrameSourceMultiplexerTest, SourcesManipulation)
    {
        EXPECT_EQ(NULL, mux_->ActiveSource());

        mux_->AddSource(source1_);
        EXPECT_EQ(source1_, mux_->ActiveSource());

        mux_->SetActiveSource(NULL);
        EXPECT_EQ(NULL, mux_->ActiveSource());

        mux_->SetActiveSource(source1_);

#ifndef NDEBUG
        // Setting a source which isn't in the mux as active should DCHECK fail.
        EXPECT_DEATH({ mux_->SetActiveSource(source2_); }, "");

        // Adding a source which is already added should DCHECK fail.
        EXPECT_DEATH({ mux_->AddSource(source1_); }, "");

        // Removing a source which isn't in the mux should DCHECK fail.
        EXPECT_DEATH({ mux_->RemoveSource(source2_); }, "");

        // Removing the active source fails
        EXPECT_DEATH({ mux_->RemoveSource(source1_); }, "");
#endif

        // Test manipulation doesn't segfault.
        mux_->AddSource(source2_);
        mux_->RemoveSource(source2_);

        mux_->AddSource(source2_);
        mux_->SetActiveSource(source2_);
        EXPECT_EQ(source2_, mux_->ActiveSource());

        mux_->RemoveSource(source1_);
    }

    TEST_F(BeginFrameSourceMultiplexerTest, NeedsBeginFrames)
    {
        mux_->AddSource(source1_);
        mux_->AddSource(source2_);
        mux_->SetActiveSource(source1_);
        EXPECT_EQ(source1_->NeedsBeginFrames(), false);
        EXPECT_EQ(source2_->NeedsBeginFrames(), false);

        // Check SetNeedsFrames works
        mux_->SetNeedsBeginFrames(true);
        EXPECT_EQ(source1_->NeedsBeginFrames(), true);
        EXPECT_EQ(source2_->NeedsBeginFrames(), false);

        mux_->SetNeedsBeginFrames(false);
        EXPECT_EQ(source1_->NeedsBeginFrames(), false);
        EXPECT_EQ(source2_->NeedsBeginFrames(), false);

        // Checking that switching the source makes SetNeedsFrames on the
        // subsources correctly.
        mux_->SetNeedsBeginFrames(true);

        mux_->SetActiveSource(source1_);
        EXPECT_EQ(source1_->NeedsBeginFrames(), true);
        EXPECT_EQ(source2_->NeedsBeginFrames(), false);

        mux_->SetActiveSource(source2_);
        EXPECT_EQ(source1_->NeedsBeginFrames(), false);
        EXPECT_EQ(source2_->NeedsBeginFrames(), true);
    }

    TEST_F(BeginFrameSourceMultiplexerTest, BeginFramesSimple)
    {
        mux_->AddSource(source1_);
        mux_->AddSource(source2_);
        mux_->SetActiveSource(source1_);

        MockBeginFrameObserver obs;
        mux_->AddObserver(&obs);
        EXPECT_BEGIN_FRAME_USED(obs, 100, 200, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 400, 600, 300);

        mux_->SetActiveSource(source1_);

        SEND_BEGIN_FRAME_USED(*source1_, 100, 200, 300);
        SEND_BEGIN_FRAME_DROP(*source2_, 200, 500, 300);

        mux_->SetActiveSource(source2_);
        SEND_BEGIN_FRAME_USED(*source2_, 400, 600, 300);
        SEND_BEGIN_FRAME_DROP(*source1_, 500, 700, 300);
    }

    TEST_F(BeginFrameSourceMultiplexerTest, BeginFramesBackwardsProtection)
    {
        mux_->AddSource(source1_);
        mux_->AddSource(source2_);

        MockBeginFrameObserver obs;
        mux_->AddObserver(&obs);
        EXPECT_BEGIN_FRAME_USED(obs, 400, 600, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 700, 900, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 1000, 1200, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 1001, 1201, 301);

        mux_->SetActiveSource(source1_);
        SEND_BEGIN_FRAME_USED(*source1_, 400, 600, 300);
        SEND_BEGIN_FRAME_USED(*source1_, 700, 900, 300);

        mux_->SetActiveSource(source2_);
        SEND_BEGIN_FRAME_DROP(*source2_, 699, 899, 300);
        SEND_BEGIN_FRAME_USED(*source2_, 1000, 1200, 300);

        mux_->SetActiveSource(source1_);
        SEND_BEGIN_FRAME_USED(*source1_, 1001, 1201, 301);
    }

    TEST_F(BeginFrameSourceMultiplexerTest, MinimumIntervalNegativeFails)
    {
#ifndef NDEBUG
        EXPECT_DEATH(
            { mux_->SetMinimumInterval(base::TimeDelta::FromInternalValue(-100)); },
            "");
#endif
    }

    TEST_F(BeginFrameSourceMultiplexerTest, MinimumIntervalZero)
    {
        mux_->SetMinimumInterval(base::TimeDelta());
        mux_->AddSource(source1_);

        MockBeginFrameObserver obs;
        mux_->AddObserver(&obs);
        EXPECT_BEGIN_FRAME_USED(obs, 100, 200, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 400, 600, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 700, 900, 300);

        SEND_BEGIN_FRAME_USED(*source1_, 100, 200, 300);
        SEND_BEGIN_FRAME_USED(*source1_, 400, 600, 300);
        SEND_BEGIN_FRAME_USED(*source1_, 700, 900, 300);
    }

    TEST_F(BeginFrameSourceMultiplexerTest, MinimumIntervalBasic)
    {
        mux_->SetMinimumInterval(base::TimeDelta::FromInternalValue(600));
        mux_->AddSource(source1_);

        MockBeginFrameObserver obs;
        mux_->AddObserver(&obs);
        EXPECT_BEGIN_FRAME_USED(obs, 100, 200, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 700, 900, 300);

        SEND_BEGIN_FRAME_USED(*source1_, 100, 200, 300);
        SEND_BEGIN_FRAME_DROP(*source1_, 400, 600, 300);
        SEND_BEGIN_FRAME_USED(*source1_, 700, 900, 300);
    }

    TEST_F(BeginFrameSourceMultiplexerTest, MinimumIntervalWithMultipleSources)
    {
        mux_->SetMinimumInterval(base::TimeDelta::FromMicroseconds(150));
        mux_->AddSource(source1_);
        mux_->AddSource(source2_);

        MockBeginFrameObserver obs;
        mux_->AddObserver(&obs);
        EXPECT_BEGIN_FRAME_USED(obs, 400, 600, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 700, 900, 300);
        EXPECT_BEGIN_FRAME_USED(obs, 1050, 1250, 300);

        mux_->SetActiveSource(source1_);
        SEND_BEGIN_FRAME_USED(*source1_, 400, 600, 300);
        SEND_BEGIN_FRAME_USED(*source1_, 700, 900, 300);

        mux_->SetActiveSource(source2_);
        SEND_BEGIN_FRAME_DROP(*source2_, 750, 1050, 300);
        SEND_BEGIN_FRAME_USED(*source2_, 1050, 1250, 300);

        mux_->SetActiveSource(source1_);
        SEND_BEGIN_FRAME_DROP(*source2_, 1100, 1400, 300);
    }

} // namespace
} // namespace cc
