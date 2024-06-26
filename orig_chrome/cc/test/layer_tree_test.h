// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_LAYER_TREE_TEST_H_
#define CC_TEST_LAYER_TREE_TEST_H_

#include "base/memory/ref_counted.h"
#include "base/threading/thread.h"
#include "cc/animation/animation_delegate.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
class AnimationPlayer;
class FakeExternalBeginFrameSource;
class FakeLayerTreeHostClient;
class FakeOutputSurface;
class LayerImpl;
class LayerTreeHost;
class LayerTreeHostClient;
class LayerTreeHostImpl;
class TestContextProvider;
class TestGpuMemoryBufferManager;
class TestWebGraphicsContext3D;

// Creates the virtual viewport layer hierarchy under the given root_layer.
// Convenient overload of the method below that creates a scrolling layer as
// the outer viewport scroll layer.
void CreateVirtualViewportLayers(Layer* root_layer,
    const gfx::Size& inner_bounds,
    const gfx::Size& outer_bounds,
    const gfx::Size& scroll_bounds,
    LayerTreeHost* host,
    const LayerSettings& layer_settings);

// Creates the virtual viewport layer hierarchy under the given root_layer.
// Uses the given scroll layer as the content "outer viewport scroll layer".
void CreateVirtualViewportLayers(Layer* root_layer,
    scoped_refptr<Layer> outer_scroll_layer,
    const gfx::Size& outer_bounds,
    const gfx::Size& scroll_bounds,
    LayerTreeHost* host,
    const LayerSettings& layer_settings);

// Used by test stubs to notify the test when something interesting happens.
class TestHooks : public AnimationDelegate {
public:
    TestHooks();
    ~TestHooks() override;

    void ReadSettings(const LayerTreeSettings& settings);

    virtual void CreateResourceAndTileTaskWorkerPool(
        LayerTreeHostImpl* host_impl,
        scoped_ptr<TileTaskWorkerPool>* tile_task_worker_pool,
        scoped_ptr<ResourcePool>* resource_pool);
    virtual void WillBeginImplFrameOnThread(LayerTreeHostImpl* host_impl,
        const BeginFrameArgs& args) { }
    virtual void DidFinishImplFrameOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void BeginMainFrameAbortedOnThread(LayerTreeHostImpl* host_impl,
        CommitEarlyOutReason reason) { }
    virtual void WillPrepareTiles(LayerTreeHostImpl* host_impl) { }
    virtual void BeginCommitOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void WillCommitCompleteOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void CommitCompleteOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void WillActivateTreeOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void DidActivateTreeOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void InitializedRendererOnThread(LayerTreeHostImpl* host_impl,
        bool success) { }
    virtual DrawResult PrepareToDrawOnThread(
        LayerTreeHostImpl* host_impl,
        LayerTreeHostImpl::FrameData* frame_data,
        DrawResult draw_result);
    virtual void DrawLayersOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void SwapBuffersOnThread(LayerTreeHostImpl* host_impl, bool result) { }
    virtual void SwapBuffersCompleteOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void NotifyReadyToActivateOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void NotifyReadyToDrawOnThread(LayerTreeHostImpl* host_impl) { }
    virtual void NotifyAllTileTasksCompleted(LayerTreeHostImpl* host_impl) { }
    virtual void NotifyTileStateChangedOnThread(LayerTreeHostImpl* host_impl,
        const Tile* tile) { }
    virtual void AnimateLayers(LayerTreeHostImpl* host_impl,
        base::TimeTicks monotonic_time) { }
    virtual void UpdateAnimationState(LayerTreeHostImpl* host_impl,
        bool has_unfinished_animation) { }
    virtual void WillAnimateLayers(LayerTreeHostImpl* host_impl,
        base::TimeTicks monotonic_time) { }
    virtual void ApplyViewportDeltas(
        const gfx::Vector2dF& inner_delta,
        const gfx::Vector2dF& outer_delta,
        const gfx::Vector2dF& elastic_overscroll_delta,
        float scale,
        float top_controls_delta) { }
    virtual void BeginMainFrame(const BeginFrameArgs& args) { }
    virtual void WillBeginMainFrame() { }
    virtual void DidBeginMainFrame() { }
    virtual void Layout() { }
    virtual void DidInitializeOutputSurface() { }
    virtual void DidFailToInitializeOutputSurface() { }
    virtual void DidAddAnimation() { }
    virtual void WillCommit() { }
    virtual void DidCommit() { }
    virtual void DidCommitAndDrawFrame() { }
    virtual void DidCompleteSwapBuffers() { }
    virtual void DidSetVisibleOnImplTree(LayerTreeHostImpl* host_impl,
        bool visible) { }
    virtual void ScheduleComposite() { }
    virtual void DidSetNeedsUpdateLayers() { }

    // Hooks for SchedulerClient.
    virtual void ScheduledActionWillSendBeginMainFrame() { }
    virtual void ScheduledActionSendBeginMainFrame() { }
    virtual void ScheduledActionDrawAndSwapIfPossible() { }
    virtual void ScheduledActionAnimate() { }
    virtual void ScheduledActionCommit() { }
    virtual void ScheduledActionBeginOutputSurfaceCreation() { }
    virtual void ScheduledActionPrepareTiles() { }
    virtual void ScheduledActionInvalidateOutputSurface() { }
    virtual void SendBeginFramesToChildren(const BeginFrameArgs& args) { }
    virtual void SendBeginMainFrameNotExpectedSoon() { }

    // Implementation of AnimationDelegate:
    void NotifyAnimationStarted(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group) override { }
    void NotifyAnimationFinished(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group) override { }

    virtual void RequestNewOutputSurface() = 0;
};

class BeginTask;
class LayerTreeHostClientForTesting;
class TimeoutTask;

// The LayerTreeTests runs with the main loop running. It instantiates a single
// LayerTreeHostForTesting and associated LayerTreeHostImplForTesting and
// LayerTreeHostClientForTesting.
//
// BeginTest() is called once the main message loop is running and the layer
// tree host is initialized.
//
// Key stages of the drawing loop, e.g. drawing or commiting, redirect to
// LayerTreeTest methods of similar names. To track the commit process, override
// these functions.
//
// The test continues until someone calls EndTest. EndTest can be called on any
// thread, but be aware that ending the test is an asynchronous process.
class LayerTreeTest : public testing::Test, public TestHooks {
public:
    ~LayerTreeTest() override;

    virtual void EndTest();
    void EndTestAfterDelayMs(int delay_milliseconds);

    void PostAddAnimationToMainThread(Layer* layer_to_receive_animation);
    void PostAddInstantAnimationToMainThread(Layer* layer_to_receive_animation);
    void PostAddLongAnimationToMainThread(Layer* layer_to_receive_animation);
    void PostAddAnimationToMainThreadPlayer(
        AnimationPlayer* player_to_receive_animation);
    void PostAddInstantAnimationToMainThreadPlayer(
        AnimationPlayer* player_to_receive_animation);
    void PostAddLongAnimationToMainThreadPlayer(
        AnimationPlayer* player_to_receive_animation);
    void PostSetDeferCommitsToMainThread(bool defer_commits);
    void PostSetNeedsCommitToMainThread();
    void PostSetNeedsUpdateLayersToMainThread();
    void PostSetNeedsRedrawToMainThread();
    void PostSetNeedsRedrawRectToMainThread(const gfx::Rect& damage_rect);
    void PostSetVisibleToMainThread(bool visible);
    void PostSetNextCommitForcesRedrawToMainThread();
    void PostCompositeImmediatelyToMainThread();

    void DoBeginTest();
    void Timeout();

    bool verify_property_trees() const { return verify_property_trees_; }
    void set_verify_property_trees(bool verify_property_trees)
    {
        verify_property_trees_ = verify_property_trees;
    }

    const LayerSettings& layer_settings() { return layer_settings_; }

protected:
    LayerTreeTest();

    virtual void InitializeSettings(LayerTreeSettings* settings) { }
    virtual void InitializeLayerSettings(LayerSettings* layer_settings) { }

    void RealEndTest();

    virtual void DispatchAddAnimation(Layer* layer_to_receive_animation,
        double animation_duration);
    virtual void DispatchAddAnimationToPlayer(
        AnimationPlayer* player_to_receive_animation,
        double animation_duration);
    void DispatchSetDeferCommits(bool defer_commits);
    void DispatchSetNeedsCommit();
    void DispatchSetNeedsUpdateLayers();
    void DispatchSetNeedsRedraw();
    void DispatchSetNeedsRedrawRect(const gfx::Rect& damage_rect);
    void DispatchSetVisible(bool visible);
    void DispatchSetNextCommitForcesRedraw();
    void DispatchDidAddAnimation();
    void DispatchCompositeImmediately();

    virtual void AfterTest() = 0;
    virtual void WillBeginTest();
    virtual void BeginTest() = 0;
    virtual void SetupTree();

    virtual void RunTest(bool threaded, bool delegating_renderer);

    bool HasImplThread() { return !!impl_thread_; }
    base::SingleThreadTaskRunner* ImplThreadTaskRunner()
    {
        DCHECK(proxy());
        return proxy()->ImplThreadTaskRunner() ? proxy()->ImplThreadTaskRunner()
                                               : main_task_runner_.get();
    }
    base::SingleThreadTaskRunner* MainThreadTaskRunner()
    {
        return main_task_runner_.get();
    }
    Proxy* proxy() const
    {
        return layer_tree_host_ ? layer_tree_host_->proxy() : NULL;
    }
    TaskGraphRunner* task_graph_runner() const
    {
        return task_graph_runner_.get();
    }

    bool TestEnded() const { return ended_; }

    LayerTreeHost* layer_tree_host();
    bool delegating_renderer() const { return delegating_renderer_; }
    FakeOutputSurface* output_surface() { return output_surface_; }
    int LastCommittedSourceFrameNumber(LayerTreeHostImpl* impl) const;

    void DestroyLayerTreeHost();

    // By default, output surface recreation is synchronous.
    void RequestNewOutputSurface() override;
    // Override this for pixel tests, where you need a real output surface.
    virtual scoped_ptr<OutputSurface> CreateOutputSurface();
    // Override this for unit tests, which should not produce pixel output.
    virtual scoped_ptr<FakeOutputSurface> CreateFakeOutputSurface();

    TestWebGraphicsContext3D* TestContext();

    TestGpuMemoryBufferManager* GetTestGpuMemoryBufferManager()
    {
        return gpu_memory_buffer_manager_.get();
    }

private:
    LayerTreeSettings settings_;
    LayerSettings layer_settings_;

    scoped_ptr<LayerTreeHostClientForTesting> client_;
    scoped_ptr<LayerTreeHost> layer_tree_host_;
    FakeOutputSurface* output_surface_;
    FakeExternalBeginFrameSource* external_begin_frame_source_;

    bool beginning_;
    bool end_when_begin_returns_;
    bool timed_out_;
    bool scheduled_;
    bool started_;
    bool ended_;
    bool delegating_renderer_;
    bool verify_property_trees_;

    int timeout_seconds_;

    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;
    scoped_ptr<base::Thread> impl_thread_;
    scoped_ptr<SharedBitmapManager> shared_bitmap_manager_;
    scoped_ptr<TestGpuMemoryBufferManager> gpu_memory_buffer_manager_;
    scoped_ptr<TaskGraphRunner> task_graph_runner_;
    base::CancelableClosure timeout_;
    scoped_refptr<TestContextProvider> compositor_contexts_;
    base::WeakPtr<LayerTreeTest> main_thread_weak_ptr_;
    base::WeakPtrFactory<LayerTreeTest> weak_factory_;
};

} // namespace cc

#define SINGLE_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME)     \
    TEST_F(TEST_FIXTURE_NAME, RunSingleThread_DirectRenderer)       \
    {                                                               \
        RunTest(false, false);                                      \
    }                                                               \
    class SingleThreadDirectImplNeedsSemicolon##TEST_FIXTURE_NAME { \
    }

#define SINGLE_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME)     \
    TEST_F(TEST_FIXTURE_NAME, RunSingleThread_DelegatingRenderer)       \
    {                                                                   \
        RunTest(false, true);                                           \
    }                                                                   \
    class SingleThreadDelegatingImplNeedsSemicolon##TEST_FIXTURE_NAME { \
    }

#define SINGLE_THREAD_TEST_F(TEST_FIXTURE_NAME)              \
    SINGLE_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME); \
    SINGLE_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME)

#define MULTI_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME)     \
    TEST_F(TEST_FIXTURE_NAME, RunMultiThread_DirectRenderer)       \
    {                                                              \
        RunTest(true, false);                                      \
    }                                                              \
    class MultiThreadDirectImplNeedsSemicolon##TEST_FIXTURE_NAME { \
    }

#define MULTI_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME)     \
    TEST_F(TEST_FIXTURE_NAME, RunMultiThread_DelegatingRenderer)       \
    {                                                                  \
        RunTest(true, true);                                           \
    }                                                                  \
    class MultiThreadDelegatingImplNeedsSemicolon##TEST_FIXTURE_NAME { \
    }

#define MULTI_THREAD_TEST_F(TEST_FIXTURE_NAME)              \
    MULTI_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME); \
    MULTI_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME)

#define SINGLE_AND_MULTI_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME) \
    SINGLE_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME);              \
    MULTI_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME)

#define SINGLE_AND_MULTI_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME) \
    SINGLE_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME);              \
    MULTI_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME)

#define SINGLE_AND_MULTI_THREAD_TEST_F(TEST_FIXTURE_NAME)              \
    SINGLE_AND_MULTI_THREAD_DIRECT_RENDERER_TEST_F(TEST_FIXTURE_NAME); \
    SINGLE_AND_MULTI_THREAD_DELEGATING_RENDERER_TEST_F(TEST_FIXTURE_NAME)

// Some tests want to control when notify ready for activation occurs,
// but this is not supported in the single-threaded case.
#define MULTI_THREAD_BLOCKNOTIFY_TEST_F(TEST_FIXTURE_NAME) \
    MULTI_THREAD_TEST_F(TEST_FIXTURE_NAME)

#endif // CC_TEST_LAYER_TREE_TEST_H_
