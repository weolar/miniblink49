// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/threaded_channel.h"

#include "cc/test/layer_tree_test.h"
#include "cc/trees/single_thread_proxy.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

// The ThreadedChannel tests are run only for threaded and direct mode.
class ThreadedChannelTest : public LayerTreeTest {
protected:
    ThreadedChannelTest()
        : thread_proxy_(nullptr)
        , calls_received_(0)
    {
    }

    ~ThreadedChannelTest() override { }

    void BeginTest() override
    {
        DCHECK(HasImplThread());
        thread_proxy_ = static_cast<ThreadProxy*>(proxy());
        BeginChannelTest();
    };
    virtual void BeginChannelTest() { }

    void PostOnImplThread()
    {
        ImplThreadTaskRunner()->PostTask(
            FROM_HERE, base::Bind(&ThreadedChannelTest::StartTestOnImplThread, base::Unretained(this)));
    }

    virtual void StartTestOnImplThread() { DCHECK(proxy()->IsImplThread()); }

    void AfterTest() override { }

    // TODO(khushalsagar): Remove this once ProxyImpl is added to the
    // LayerTreeTest.
    ThreadProxy* thread_proxy_;
    int calls_received_;

private:
    DISALLOW_COPY_AND_ASSIGN(ThreadedChannelTest);
};

class ThreadedChannelTestInitializationAndShutdown
    : public ThreadedChannelTest {
    void SetVisibleOnImpl(bool visible) override { calls_received_++; }

    void ReceivedRequestNewOutputSurface() override { calls_received_++; }

    void InitializeOutputSurfaceOnImpl(OutputSurface* output_surface) override
    {
        calls_received_++;
    }

    void ReceivedSetRendererCapabilitiesMainCopy(
        const RendererCapabilities& capabilities) override
    {
        calls_received_++;
    }

    void ReceivedDidInitializeOutputSurface(
        bool success,
        const RendererCapabilities& capabilities) override
    {
        calls_received_++;
        EndTest();
    }

    void FinishGLOnImpl() override { calls_received_++; }

    void AfterTest() override { EXPECT_EQ(6, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(
    ThreadedChannelTestInitializationAndShutdown);

class ThreadedChannelTestThrottleFrameProduction : public ThreadedChannelTest {
    void BeginChannelTest() override
    {
        proxy()->SetThrottleFrameProduction(true);
    }

    void SetThrottleFrameProductionOnImpl(bool throttle) override
    {
        ASSERT_TRUE(throttle);
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestThrottleFrameProduction);

class ThreadedChannelTestTopControlsState : public ThreadedChannelTest {
    void BeginChannelTest() override
    {
        proxy()->UpdateTopControlsState(TopControlsState::BOTH,
            TopControlsState::BOTH, true);
    }

    void UpdateTopControlsStateOnImpl(TopControlsState constraints,
        TopControlsState current,
        bool animate) override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestTopControlsState);

class ThreadedChannelTestMainThreadStoppedFlinging
    : public ThreadedChannelTest {
    void BeginChannelTest() override { proxy()->MainThreadHasStoppedFlinging(); }

    void MainThreadHasStoppedFlingingOnImpl() override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(
    ThreadedChannelTestMainThreadStoppedFlinging);

class ThreadedChannelTestDeferCommits : public ThreadedChannelTest {
    void BeginChannelTest() override { DispatchSetDeferCommits(true); }

    void SetDeferCommitsOnImpl(bool defer_commits) override
    {
        ASSERT_TRUE(defer_commits);
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestDeferCommits);

class ThreadedChannelTestInputThrottled : public ThreadedChannelTest {
    void BeginChannelTest() override
    {
        proxy()->NotifyInputThrottledUntilCommit();
    }

    void SetInputThrottledUntilCommitOnImpl(bool is_throttled) override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestInputThrottled);

class ThreadedChannelTestNeedsRedraw : public ThreadedChannelTest {
    void BeginChannelTest() override { DispatchSetNeedsRedraw(); }

    void SetNeedsRedrawOnImpl(const gfx::Rect& damage_rect) override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestNeedsRedraw);

class ThreadedChannelTestFinishAllRendering : public ThreadedChannelTest {
    void BeginChannelTest() override { proxy()->FinishAllRendering(); }

    void FinishAllRenderingOnImpl() override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestFinishAllRendering);

class ThreadedChannelTestReleaseOutputSurface : public ThreadedChannelTest {
    void BeginChannelTest() override { proxy()->ReleaseOutputSurface(); }

    void ReleaseOutputSurfaceOnImpl() override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestReleaseOutputSurface);

class ThreadedChannelTestCommit : public ThreadedChannelTest {
    void BeginChannelTest() override { PostSetNeedsCommitToMainThread(); }

    void SetNeedsCommitOnImpl() override { EXPECT_EQ(0, calls_received_++); }

    void ReceivedBeginMainFrame() override { EXPECT_EQ(1, calls_received_++); }

    void StartCommitOnImpl() override { EXPECT_EQ(2, calls_received_++); }

    void ReceivedDidCommitAndDrawFrame() override
    {
        EXPECT_EQ(3, calls_received_++);
    }

    void ReceivedDidCompleteSwapBuffers() override
    {
        EXPECT_EQ(4, calls_received_++);
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(5, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestCommit);

class ThreadedChannelTestBeginMainFrameAborted : public ThreadedChannelTest {
    void BeginChannelTest() override { PostSetNeedsCommitToMainThread(); }

    void ScheduledActionWillSendBeginMainFrame() override
    {
        // Set visible to false to abort the commit.
        MainThreadTaskRunner()->PostTask(
            FROM_HERE,
            base::Bind(&ThreadedChannelTestBeginMainFrameAborted::SetVisibleFalse,
                base::Unretained(this)));
    }

    void SetVisibleFalse() { layer_tree_host()->SetVisible(false); }

    void BeginMainFrameAbortedOnImpl(CommitEarlyOutReason reason) override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestBeginMainFrameAborted);

class ThreadedChannelTestBeginMainFrameNotExpectedSoon
    : public ThreadedChannelTest {
    void BeginChannelTest() override { PostOnImplThread(); }

    void StartTestOnImplThread() override
    {
        thread_proxy_->SendBeginMainFrameNotExpectedSoon();
    }

    void ReceivedBeginMainFrameNotExpectedSoon() override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(
    ThreadedChannelTestBeginMainFrameNotExpectedSoon);

class ThreadedChannelTestSetAnimationEvents : public ThreadedChannelTest {
    void BeginChannelTest() override { PostOnImplThread(); }

    void StartTestOnImplThread() override
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        thread_proxy_->PostAnimationEventsToMainThreadOnImplThread(events.Pass());
    }

    void ReceivedSetAnimationEvents() override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestSetAnimationEvents);

class ThreadedChannelTestLoseOutputSurface : public ThreadedChannelTest {
    void BeginChannelTest() override { PostOnImplThread(); }

    void StartTestOnImplThread() override
    {
        thread_proxy_->DidLoseOutputSurfaceOnImplThread();
    }

    void ReceivedDidLoseOutputSurface() override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestLoseOutputSurface);

class ThreadedChannelTestPageScaleAnimation : public ThreadedChannelTest {
    void BeginChannelTest() override { PostOnImplThread(); }

    void StartTestOnImplThread() override
    {
        thread_proxy_->DidCompletePageScaleAnimationOnImplThread();
    }

    void ReceivedDidCompletePageScaleAnimation() override
    {
        calls_received_++;
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(1, calls_received_); }
};

MULTI_THREAD_DIRECT_RENDERER_TEST_F(ThreadedChannelTestPageScaleAnimation);

} // namespace cc
