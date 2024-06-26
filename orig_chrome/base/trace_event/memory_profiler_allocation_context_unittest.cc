// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/ref_counted.h"
#include "base/trace_event/memory_profiler_allocation_context.h"
#include "base/trace_event/trace_event.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace trace_event {

    // Define all strings once, because the pseudo stack requires pointer equality,
    // and string interning is unreliable.
    const char kCupcake[] = "Cupcake";
    const char kDonut[] = "Donut";
    const char kEclair[] = "Eclair";
    const char kFroyo[] = "Froyo";
    const char kGingerbread[] = "Gingerbread";

    const char kBrowserMain[] = "BrowserMain";
    const char kRendererMain[] = "RendererMain";
    const char kCreateWidget[] = "CreateWidget";
    const char kInitialize[] = "Initialize";
    const char kMalloc[] = "malloc";

    // Returns a pointer past the end of the fixed-size array |array| of |T| of
    // length |N|, identical to C++11 |std::end|.
    template <typename T, int N>
    const T* End(const T (&array)[N])
    {
        return array + N;
    }

    // Asserts that the fixed-size array |expected_backtrace| matches the backtrace
    // in |AllocationContextTracker::GetContextSnapshot|.
    template <size_t N>
    void AssertBacktraceEquals(const StackFrame (&expected_backtrace)[N])
    {
        AllocationContext ctx = AllocationContextTracker::GetContextSnapshot();

        auto actual = ctx.backtrace.frames;
        auto actual_bottom = End(ctx.backtrace.frames);
        auto expected = expected_backtrace;
        auto expected_bottom = End(expected_backtrace);

        // Note that this requires the pointers to be equal, this is not doing a deep
        // string comparison.
        for (; actual != actual_bottom && expected != expected_bottom;
             actual++, expected++)
            ASSERT_EQ(*expected, *actual);

        // Ensure that the height of the stacks is the same.
        ASSERT_EQ(actual, actual_bottom);
        ASSERT_EQ(expected, expected_bottom);
    }

    void AssertBacktraceEmpty()
    {
        AllocationContext ctx = AllocationContextTracker::GetContextSnapshot();

        for (StackFrame frame : ctx.backtrace.frames)
            ASSERT_EQ(nullptr, frame);
    }

    class AllocationContextTest : public testing::Test {
    public:
        void SetUp() override
        {
            TraceConfig config("");
            TraceLog::GetInstance()->SetEnabled(config, TraceLog::RECORDING_MODE);
            AllocationContextTracker::SetCaptureEnabled(true);
        }

        void TearDown() override
        {
            AllocationContextTracker::SetCaptureEnabled(false);
            TraceLog::GetInstance()->SetDisabled();
        }
    };

    class StackFrameDeduplicatorTest : public testing::Test {
    };

    // Check that |TRACE_EVENT| macros push and pop to the pseudo stack correctly.
    // Also check that |GetContextSnapshot| fills the backtrace with null pointers
    // when the pseudo stack height is less than the capacity.
    TEST_F(AllocationContextTest, PseudoStackScopedTrace)
    {
        StackFrame c = kCupcake;
        StackFrame d = kDonut;
        StackFrame e = kEclair;
        StackFrame f = kFroyo;

        AssertBacktraceEmpty();

        {
            TRACE_EVENT0("Testing", kCupcake);
            StackFrame frame_c[] = { c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            AssertBacktraceEquals(frame_c);

            {
                TRACE_EVENT0("Testing", kDonut);
                StackFrame frame_cd[] = { c, d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
                AssertBacktraceEquals(frame_cd);
            }

            AssertBacktraceEquals(frame_c);

            {
                TRACE_EVENT0("Testing", kEclair);
                StackFrame frame_ce[] = { c, e, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
                AssertBacktraceEquals(frame_ce);
            }

            AssertBacktraceEquals(frame_c);
        }

        AssertBacktraceEmpty();

        {
            TRACE_EVENT0("Testing", kFroyo);
            StackFrame frame_f[] = { f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            AssertBacktraceEquals(frame_f);
        }

        AssertBacktraceEmpty();
    }

    // Same as |PseudoStackScopedTrace|, but now test the |TRACE_EVENT_BEGIN| and
    // |TRACE_EVENT_END| macros.
    TEST_F(AllocationContextTest, PseudoStackBeginEndTrace)
    {
        StackFrame c = kCupcake;
        StackFrame d = kDonut;
        StackFrame e = kEclair;
        StackFrame f = kFroyo;

        StackFrame frame_c[] = { c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        StackFrame frame_cd[] = { c, d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        StackFrame frame_ce[] = { c, e, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        StackFrame frame_f[] = { f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        AssertBacktraceEmpty();

        TRACE_EVENT_BEGIN0("Testing", kCupcake);
        AssertBacktraceEquals(frame_c);

        TRACE_EVENT_BEGIN0("Testing", kDonut);
        AssertBacktraceEquals(frame_cd);
        TRACE_EVENT_END0("Testing", kDonut);

        AssertBacktraceEquals(frame_c);

        TRACE_EVENT_BEGIN0("Testing", kEclair);
        AssertBacktraceEquals(frame_ce);
        TRACE_EVENT_END0("Testing", kEclair);

        AssertBacktraceEquals(frame_c);
        TRACE_EVENT_END0("Testing", kCupcake);

        AssertBacktraceEmpty();

        TRACE_EVENT_BEGIN0("Testing", kFroyo);
        AssertBacktraceEquals(frame_f);
        TRACE_EVENT_END0("Testing", kFroyo);

        AssertBacktraceEmpty();
    }

    TEST_F(AllocationContextTest, PseudoStackMixedTrace)
    {
        StackFrame c = kCupcake;
        StackFrame d = kDonut;
        StackFrame e = kEclair;
        StackFrame f = kFroyo;

        StackFrame frame_c[] = { c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        StackFrame frame_cd[] = { c, d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        StackFrame frame_e[] = { e, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        StackFrame frame_ef[] = { e, f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        AssertBacktraceEmpty();

        TRACE_EVENT_BEGIN0("Testing", kCupcake);
        AssertBacktraceEquals(frame_c);

        {
            TRACE_EVENT0("Testing", kDonut);
            AssertBacktraceEquals(frame_cd);
        }

        AssertBacktraceEquals(frame_c);
        TRACE_EVENT_END0("Testing", kCupcake);
        AssertBacktraceEmpty();

        {
            TRACE_EVENT0("Testing", kEclair);
            AssertBacktraceEquals(frame_e);

            TRACE_EVENT_BEGIN0("Testing", kFroyo);
            AssertBacktraceEquals(frame_ef);
            TRACE_EVENT_END0("Testing", kFroyo);
            AssertBacktraceEquals(frame_e);
        }

        AssertBacktraceEmpty();
    }

    TEST_F(AllocationContextTest, BacktraceTakesTop)
    {
        // Push 12 events onto the pseudo stack.
        TRACE_EVENT0("Testing", kCupcake);
        TRACE_EVENT0("Testing", kCupcake);
        TRACE_EVENT0("Testing", kCupcake);
        TRACE_EVENT0("Testing", kCupcake);

        TRACE_EVENT0("Testing", kCupcake);
        TRACE_EVENT0("Testing", kCupcake);
        TRACE_EVENT0("Testing", kCupcake);
        TRACE_EVENT0("Testing", kCupcake);

        TRACE_EVENT0("Testing", kCupcake);
        TRACE_EVENT0("Testing", kDonut);
        TRACE_EVENT0("Testing", kEclair);
        TRACE_EVENT0("Testing", kFroyo);

        {
            TRACE_EVENT0("Testing", kGingerbread);
            AllocationContext ctx = AllocationContextTracker::GetContextSnapshot();

            // The pseudo stack relies on pointer equality, not deep string comparisons.
            ASSERT_EQ(kCupcake, ctx.backtrace.frames[0]);
            ASSERT_EQ(kFroyo, ctx.backtrace.frames[11]);
        }

        {
            AllocationContext ctx = AllocationContextTracker::GetContextSnapshot();
            ASSERT_EQ(kCupcake, ctx.backtrace.frames[0]);
            ASSERT_EQ(kFroyo, ctx.backtrace.frames[11]);
        }
    }

    TEST_F(StackFrameDeduplicatorTest, SingleBacktrace)
    {
        Backtrace bt = {
            { kBrowserMain, kCreateWidget, kMalloc, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
        };

        // The call tree should look like this (index in brackets).
        //
        // BrowserMain [0]
        // + CreateWidget [1]
        //   + malloc [2]

        scoped_refptr<StackFrameDeduplicator> dedup = new StackFrameDeduplicator;
        ASSERT_EQ(2, dedup->Insert(bt));

        auto iter = dedup->begin();
        ASSERT_EQ(kBrowserMain, (iter + 0)->frame);
        ASSERT_EQ(-1, (iter + 0)->parent_frame_index);

        ASSERT_EQ(kCreateWidget, (iter + 1)->frame);
        ASSERT_EQ(0, (iter + 1)->parent_frame_index);

        ASSERT_EQ(kMalloc, (iter + 2)->frame);
        ASSERT_EQ(1, (iter + 2)->parent_frame_index);

        ASSERT_EQ(iter + 3, dedup->end());
    }

    // Test that there can be different call trees (there can be multiple bottom
    // frames). Also verify that frames with the same name but a different caller
    // are represented as distinct nodes.
    TEST_F(StackFrameDeduplicatorTest, MultipleRoots)
    {
        Backtrace bt0 = { { kBrowserMain, kCreateWidget, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
        Backtrace bt1 = { { kRendererMain, kCreateWidget, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

        // The call tree should look like this (index in brackets).
        //
        // BrowserMain [0]
        // + CreateWidget [1]
        // RendererMain [2]
        // + CreateWidget [3]
        //
        // Note that there will be two instances of Donut, with different parents.

        scoped_refptr<StackFrameDeduplicator> dedup = new StackFrameDeduplicator;
        ASSERT_EQ(1, dedup->Insert(bt0));
        ASSERT_EQ(3, dedup->Insert(bt1));

        auto iter = dedup->begin();
        ASSERT_EQ(kBrowserMain, (iter + 0)->frame);
        ASSERT_EQ(-1, (iter + 0)->parent_frame_index);

        ASSERT_EQ(kCreateWidget, (iter + 1)->frame);
        ASSERT_EQ(0, (iter + 1)->parent_frame_index);

        ASSERT_EQ(kRendererMain, (iter + 2)->frame);
        ASSERT_EQ(-1, (iter + 2)->parent_frame_index);

        ASSERT_EQ(kCreateWidget, (iter + 3)->frame);
        ASSERT_EQ(2, (iter + 3)->parent_frame_index);

        ASSERT_EQ(iter + 4, dedup->end());
    }

    TEST_F(StackFrameDeduplicatorTest, Deduplication)
    {
        Backtrace bt0 = { { kBrowserMain, kCreateWidget, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
        Backtrace bt1 = { { kBrowserMain, kInitialize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

        // The call tree should look like this (index in brackets).
        //
        // BrowserMain [0]
        // + CreateWidget [1]
        // + Initialize [2]
        //
        // Note that Cupcake will be re-used.

        scoped_refptr<StackFrameDeduplicator> dedup = new StackFrameDeduplicator;
        ASSERT_EQ(1, dedup->Insert(bt0));
        ASSERT_EQ(2, dedup->Insert(bt1));

        auto iter = dedup->begin();
        ASSERT_EQ(kBrowserMain, (iter + 0)->frame);
        ASSERT_EQ(-1, (iter + 0)->parent_frame_index);

        ASSERT_EQ(kCreateWidget, (iter + 1)->frame);
        ASSERT_EQ(0, (iter + 1)->parent_frame_index);

        ASSERT_EQ(kInitialize, (iter + 2)->frame);
        ASSERT_EQ(0, (iter + 2)->parent_frame_index);

        ASSERT_EQ(iter + 3, dedup->end());

        // Inserting the same backtrace again should return the index of the existing
        // node.
        ASSERT_EQ(1, dedup->Insert(bt0));
        ASSERT_EQ(2, dedup->Insert(bt1));
        ASSERT_EQ(dedup->begin() + 3, dedup->end());
    }

} // namespace trace_event
} // namespace base
