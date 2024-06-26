// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/fake_picture_layer.h"
#include "cc/test/layer_tree_test.h"
#include "cc/trees/thread_proxy.h"

#define THREAD_PROXY_TEST_F(TEST_FIXTURE_NAME) \
    TEST_F(TEST_FIXTURE_NAME, MultiThread) { Run(true); }

// Do common tests for single thread proxy and thread proxy.
// TODO(simonhong): Add SINGLE_THREAD_PROXY_TEST_F
#define PROXY_TEST_SCHEDULED_ACTION(TEST_FIXTURE_NAME) \
    THREAD_PROXY_TEST_F(TEST_FIXTURE_NAME);

namespace cc {

class ProxyTest : public LayerTreeTest {
protected:
    ProxyTest() { }
    ~ProxyTest() override { }

    void Run(bool threaded)
    {
        // We don't need to care about delegating mode.
        bool delegating_renderer = true;

        RunTest(threaded, delegating_renderer);
    }

    void BeginTest() override { }
    void AfterTest() override { }

private:
    DISALLOW_COPY_AND_ASSIGN(ProxyTest);
};

class ProxyTestScheduledActionsBasic : public ProxyTest {
protected:
    void BeginTest() override { proxy()->SetNeedsCommit(); }

    void ScheduledActionBeginOutputSurfaceCreation() override
    {
        EXPECT_EQ(0, action_phase_++);
    }

    void ScheduledActionSendBeginMainFrame() override
    {
        EXPECT_EQ(1, action_phase_++);
    }

    void ScheduledActionCommit() override { EXPECT_EQ(2, action_phase_++); }

    void ScheduledActionDrawAndSwapIfPossible() override
    {
        EXPECT_EQ(3, action_phase_++);
        EndTest();
    }

    void AfterTest() override { EXPECT_EQ(4, action_phase_); }

    ProxyTestScheduledActionsBasic()
        : action_phase_(0)
    {
    }
    ~ProxyTestScheduledActionsBasic() override { }

private:
    int action_phase_;

    DISALLOW_COPY_AND_ASSIGN(ProxyTestScheduledActionsBasic);
};

PROXY_TEST_SCHEDULED_ACTION(ProxyTestScheduledActionsBasic);

class ThreadProxyTest : public ProxyTest {
protected:
    ThreadProxyTest()
        : update_check_layer_(
            FakePictureLayer::Create(layer_settings(), &client_))
    {
    }
    ~ThreadProxyTest() override { }

    void SetupTree() override
    {
        layer_tree_host()->SetRootLayer(update_check_layer_);
        ProxyTest::SetupTree();
    }

    const ThreadProxy::MainThreadOnly& ThreadProxyMainOnly() const
    {
        DCHECK(proxy());
        DCHECK(proxy()->HasImplThread());
        return static_cast<const ThreadProxy*>(proxy())->main();
    }

    const ThreadProxy::CompositorThreadOnly& ThreadProxyImplOnly() const
    {
        DCHECK(proxy());
        DCHECK(proxy()->HasImplThread());
        return static_cast<const ThreadProxy*>(proxy())->impl();
    }

protected:
    FakeContentLayerClient client_;
    scoped_refptr<FakePictureLayer> update_check_layer_;

private:
    DISALLOW_COPY_AND_ASSIGN(ThreadProxyTest);
};

class ThreadProxyTestSetNeedsCommit : public ThreadProxyTest {
protected:
    ThreadProxyTestSetNeedsCommit() { }
    ~ThreadProxyTestSetNeedsCommit() override { }

    void BeginTest() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);

        proxy()->SetNeedsCommit();

        EXPECT_EQ(ThreadProxy::COMMIT_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
    }

    void DidBeginMainFrame() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
    }

    void DidCommit() override
    {
        EXPECT_EQ(1, update_check_layer_->update_count());
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
        EndTest();
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ThreadProxyTestSetNeedsCommit);
};

THREAD_PROXY_TEST_F(ThreadProxyTestSetNeedsCommit);

class ThreadProxyTestSetNeedsAnimate : public ThreadProxyTest {
protected:
    ThreadProxyTestSetNeedsAnimate() { }
    ~ThreadProxyTestSetNeedsAnimate() override { }

    void BeginTest() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);

        proxy()->SetNeedsAnimate();

        EXPECT_EQ(ThreadProxy::ANIMATE_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
    }

    void DidBeginMainFrame() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
    }

    void DidCommit() override
    {
        EXPECT_EQ(0, update_check_layer_->update_count());
        EndTest();
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ThreadProxyTestSetNeedsAnimate);
};

THREAD_PROXY_TEST_F(ThreadProxyTestSetNeedsAnimate);

class ThreadProxyTestSetNeedsUpdateLayers : public ThreadProxyTest {
protected:
    ThreadProxyTestSetNeedsUpdateLayers() { }
    ~ThreadProxyTestSetNeedsUpdateLayers() override { }

    void BeginTest() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);

        proxy()->SetNeedsUpdateLayers();

        EXPECT_EQ(ThreadProxy::UPDATE_LAYERS_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
    }

    void DidBeginMainFrame() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
    }

    void DidCommit() override
    {
        EXPECT_EQ(1, update_check_layer_->update_count());
        EndTest();
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ThreadProxyTestSetNeedsUpdateLayers);
};

THREAD_PROXY_TEST_F(ThreadProxyTestSetNeedsUpdateLayers);

class ThreadProxyTestSetNeedsUpdateLayersWhileAnimating
    : public ThreadProxyTest {
protected:
    ThreadProxyTestSetNeedsUpdateLayersWhileAnimating() { }
    ~ThreadProxyTestSetNeedsUpdateLayersWhileAnimating() override { }

    void BeginTest() override { proxy()->SetNeedsAnimate(); }

    void WillBeginMainFrame() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::ANIMATE_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
        EXPECT_EQ(ThreadProxy::ANIMATE_PIPELINE_STAGE,
            ThreadProxyMainOnly().final_pipeline_stage);

        proxy()->SetNeedsUpdateLayers();

        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::UPDATE_LAYERS_PIPELINE_STAGE,
            ThreadProxyMainOnly().final_pipeline_stage);
    }

    void DidBeginMainFrame() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
    }

    void DidCommit() override
    {
        EXPECT_EQ(1, update_check_layer_->update_count());
        EndTest();
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ThreadProxyTestSetNeedsUpdateLayersWhileAnimating);
};

THREAD_PROXY_TEST_F(ThreadProxyTestSetNeedsUpdateLayersWhileAnimating);

class ThreadProxyTestSetNeedsCommitWhileAnimating : public ThreadProxyTest {
protected:
    ThreadProxyTestSetNeedsCommitWhileAnimating() { }
    ~ThreadProxyTestSetNeedsCommitWhileAnimating() override { }

    void BeginTest() override { proxy()->SetNeedsAnimate(); }

    void WillBeginMainFrame() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::ANIMATE_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
        EXPECT_EQ(ThreadProxy::ANIMATE_PIPELINE_STAGE,
            ThreadProxyMainOnly().final_pipeline_stage);

        proxy()->SetNeedsCommit();

        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::COMMIT_PIPELINE_STAGE,
            ThreadProxyMainOnly().final_pipeline_stage);
    }

    void DidBeginMainFrame() override
    {
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().max_requested_pipeline_stage);
        EXPECT_EQ(ThreadProxy::NO_PIPELINE_STAGE,
            ThreadProxyMainOnly().current_pipeline_stage);
    }

    void DidCommit() override
    {
        EXPECT_EQ(1, update_check_layer_->update_count());
        EndTest();
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ThreadProxyTestSetNeedsCommitWhileAnimating);
};

THREAD_PROXY_TEST_F(ThreadProxyTestSetNeedsCommitWhileAnimating);

} // namespace cc
