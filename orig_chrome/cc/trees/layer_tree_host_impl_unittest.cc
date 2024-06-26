// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_host_impl.h"

#include <algorithm>
#include <cmath>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/containers/hash_tables.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/location.h"
#include "base/thread_task_runner_handle.h"
#include "cc/animation/scrollbar_animation_controller_thinning.h"
#include "cc/animation/transform_operations.h"
#include "cc/base/math_util.h"
#include "cc/input/page_scale_animation.h"
#include "cc/input/top_controls_manager.h"
#include "cc/layers/append_quads_data.h"
#include "cc/layers/delegated_renderer_layer_impl.h"
#include "cc/layers/heads_up_display_layer_impl.h"
#include "cc/layers/io_surface_layer_impl.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/painted_scrollbar_layer_impl.h"
#include "cc/layers/render_surface_impl.h"
#include "cc/layers/solid_color_layer_impl.h"
#include "cc/layers/solid_color_scrollbar_layer_impl.h"
#include "cc/layers/texture_layer_impl.h"
#include "cc/layers/video_layer_impl.h"
#include "cc/layers/viewport.h"
#include "cc/output/begin_frame_args.h"
#include "cc/output/compositor_frame_ack.h"
#include "cc/output/compositor_frame_metadata.h"
#include "cc/output/copy_output_request.h"
#include "cc/output/copy_output_result.h"
#include "cc/output/gl_renderer.h"
#include "cc/output/latency_info_swap_promise.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/quads/tile_draw_quad.h"
#include "cc/test/animation_test_common.h"
#include "cc/test/begin_frame_args_test.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_picture_layer_impl.h"
#include "cc/test/fake_proxy.h"
#include "cc/test/fake_video_frame_provider.h"
#include "cc/test/geometry_test_utils.h"
#include "cc/test/gpu_rasterization_enabled_settings.h"
#include "cc/test/layer_test_common.h"
#include "cc/test/layer_tree_test.h"
#include "cc/test/test_gpu_memory_buffer_manager.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/test/test_web_graphics_context_3d.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/single_thread_proxy.h"
#include "media/base/media.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkMallocPixelRef.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

using media::VideoFrame;
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::Mock;
using ::testing::Return;

namespace cc {
namespace {

    class LayerTreeHostImplTest : public testing::Test,
                                  public LayerTreeHostImplClient {
    public:
        LayerTreeHostImplTest()
            : proxy_(base::ThreadTaskRunnerHandle::Get(),
                base::ThreadTaskRunnerHandle::Get())
            , always_impl_thread_(&proxy_)
            , always_main_thread_blocked_(&proxy_)
            , on_can_draw_state_changed_called_(false)
            , did_notify_ready_to_activate_(false)
            , did_request_commit_(false)
            , did_request_redraw_(false)
            , did_request_animate_(false)
            , did_request_prepare_tiles_(false)
            , did_complete_page_scale_animation_(false)
            , reduce_memory_result_(true)
        {
            media::InitializeMediaLibrary();
        }

        LayerTreeSettings DefaultSettings()
        {
            LayerTreeSettings settings;
            settings.minimum_occlusion_tracking_size = gfx::Size();
            settings.renderer_settings.texture_id_allocation_chunk_size = 1;
            settings.gpu_rasterization_enabled = true;
            settings.verify_property_trees = true;
            return settings;
        }

        void SetUp() override
        {
            CreateHostImpl(DefaultSettings(), CreateOutputSurface());
        }

        void TearDown() override { }

        void UpdateRendererCapabilitiesOnImplThread() override { }
        void DidLoseOutputSurfaceOnImplThread() override { }
        void CommitVSyncParameters(base::TimeTicks timebase,
            base::TimeDelta interval) override { }
        void SetEstimatedParentDrawTime(base::TimeDelta draw_time) override { }
        void SetMaxSwapsPendingOnImplThread(int max) override { }
        void DidSwapBuffersOnImplThread() override { }
        void DidSwapBuffersCompleteOnImplThread() override { }
        void OnCanDrawStateChanged(bool can_draw) override
        {
            on_can_draw_state_changed_called_ = true;
        }
        void NotifyReadyToActivate() override
        {
            did_notify_ready_to_activate_ = true;
            host_impl_->ActivateSyncTree();
        }
        void NotifyReadyToDraw() override { }
        void SetNeedsRedrawOnImplThread() override { did_request_redraw_ = true; }
        void SetNeedsRedrawRectOnImplThread(const gfx::Rect& damage_rect) override
        {
            did_request_redraw_ = true;
        }
        void SetNeedsAnimateOnImplThread() override { did_request_animate_ = true; }
        void SetNeedsPrepareTilesOnImplThread() override
        {
            did_request_prepare_tiles_ = true;
        }
        void SetNeedsCommitOnImplThread() override { did_request_commit_ = true; }
        void SetVideoNeedsBeginFrames(bool needs_begin_frames) override { }
        void PostAnimationEventsToMainThreadOnImplThread(
            scoped_ptr<AnimationEventsVector> events) override { }
        bool IsInsideDraw() override { return false; }
        void RenewTreePriority() override { }
        void PostDelayedAnimationTaskOnImplThread(const base::Closure& task,
            base::TimeDelta delay) override
        {
            animation_task_ = task;
            requested_animation_delay_ = delay;
        }
        void DidActivateSyncTree() override { }
        void WillPrepareTiles() override { }
        void DidPrepareTiles() override { }
        void DidCompletePageScaleAnimationOnImplThread() override
        {
            did_complete_page_scale_animation_ = true;
        }
        void OnDrawForOutputSurface() override { }
        void PostFrameTimingEventsOnImplThread(
            scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
            scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
            override { }

        void set_reduce_memory_result(bool reduce_memory_result)
        {
            reduce_memory_result_ = reduce_memory_result;
        }

        virtual bool CreateHostImpl(const LayerTreeSettings& settings,
            scoped_ptr<OutputSurface> output_surface)
        {
            host_impl_ = LayerTreeHostImpl::Create(
                settings, this, &proxy_, &stats_instrumentation_,
                &shared_bitmap_manager_, &gpu_memory_buffer_manager_,
                &task_graph_runner_, 0);
            output_surface_ = output_surface.Pass();
            bool init = host_impl_->InitializeRenderer(output_surface_.get());
            host_impl_->SetViewportSize(gfx::Size(10, 10));
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 1.f);
            // Set the BeginFrameArgs so that methods which use it are able to.
            host_impl_->WillBeginImplFrame(CreateBeginFrameArgsForTesting(
                BEGINFRAME_FROM_HERE,
                base::TimeTicks() + base::TimeDelta::FromMilliseconds(1)));
            host_impl_->DidFinishImplFrame();
            return init;
        }

        void SetupRootLayerImpl(scoped_ptr<LayerImpl> root)
        {
            root->SetPosition(gfx::PointF());
            root->SetBounds(gfx::Size(10, 10));
            root->SetDrawsContent(true);
            root->draw_properties().visible_layer_rect = gfx::Rect(0, 0, 10, 10);
            root->SetHasRenderSurface(true);
            host_impl_->active_tree()->SetRootLayer(root.Pass());
        }

        static void ExpectClearedScrollDeltasRecursive(LayerImpl* layer)
        {
            ASSERT_EQ(layer->ScrollDelta(), gfx::Vector2d());
            for (size_t i = 0; i < layer->children().size(); ++i)
                ExpectClearedScrollDeltasRecursive(layer->children()[i]);
        }

        static ::testing::AssertionResult ScrollInfoContains(
            const ScrollAndScaleSet& scroll_info,
            int id,
            const gfx::Vector2d& scroll_delta)
        {
            int times_encountered = 0;

            for (size_t i = 0; i < scroll_info.scrolls.size(); ++i) {
                if (scroll_info.scrolls[i].layer_id != id)
                    continue;

                if (scroll_delta != scroll_info.scrolls[i].scroll_delta) {
                    return ::testing::AssertionFailure()
                        << "Expected " << scroll_delta.ToString() << ", not "
                        << scroll_info.scrolls[i].scroll_delta.ToString();
                }
                times_encountered++;
            }

            if (times_encountered != 1)
                return ::testing::AssertionFailure() << "No layer found with id " << id;
            return ::testing::AssertionSuccess();
        }

        static void ExpectNone(const ScrollAndScaleSet& scroll_info, int id)
        {
            int times_encountered = 0;

            for (size_t i = 0; i < scroll_info.scrolls.size(); ++i) {
                if (scroll_info.scrolls[i].layer_id != id)
                    continue;
                times_encountered++;
            }

            ASSERT_EQ(0, times_encountered);
        }

        LayerImpl* CreateScrollAndContentsLayers(LayerTreeImpl* layer_tree_impl,
            const gfx::Size& content_size)
        {
            // Create both an inner viewport scroll layer and an outer viewport scroll
            // layer. The MaxScrollOffset of the outer viewport scroll layer will be
            // 0x0, so the scrolls will be applied directly to the inner viewport.
            const int kOuterViewportClipLayerId = 116;
            const int kOuterViewportScrollLayerId = 117;
            const int kContentLayerId = 118;
            const int kInnerViewportScrollLayerId = 2;
            const int kInnerViewportClipLayerId = 4;
            const int kPageScaleLayerId = 5;

            scoped_ptr<LayerImpl> root = LayerImpl::Create(layer_tree_impl, 1);
            root->SetBounds(content_size);
            root->SetPosition(gfx::PointF());
            root->SetHasRenderSurface(true);

            scoped_ptr<LayerImpl> inner_scroll = LayerImpl::Create(layer_tree_impl, kInnerViewportScrollLayerId);
            inner_scroll->SetIsContainerForFixedPositionLayers(true);
            inner_scroll->PushScrollOffsetFromMainThread(gfx::ScrollOffset());

            scoped_ptr<LayerImpl> inner_clip = LayerImpl::Create(layer_tree_impl, kInnerViewportClipLayerId);
            inner_clip->SetBounds(
                gfx::Size(content_size.width() / 2, content_size.height() / 2));

            scoped_ptr<LayerImpl> page_scale = LayerImpl::Create(layer_tree_impl, kPageScaleLayerId);

            inner_scroll->SetScrollClipLayer(inner_clip->id());
            inner_scroll->SetBounds(content_size);
            inner_scroll->SetPosition(gfx::PointF());

            scoped_ptr<LayerImpl> outer_clip = LayerImpl::Create(layer_tree_impl, kOuterViewportClipLayerId);
            outer_clip->SetBounds(content_size);
            outer_clip->SetIsContainerForFixedPositionLayers(true);

            scoped_ptr<LayerImpl> outer_scroll = LayerImpl::Create(layer_tree_impl, kOuterViewportScrollLayerId);
            outer_scroll->SetScrollClipLayer(outer_clip->id());
            outer_scroll->PushScrollOffsetFromMainThread(gfx::ScrollOffset());
            outer_scroll->SetBounds(content_size);
            outer_scroll->SetPosition(gfx::PointF());

            scoped_ptr<LayerImpl> contents = LayerImpl::Create(layer_tree_impl, kContentLayerId);
            contents->SetDrawsContent(true);
            contents->SetBounds(content_size);
            contents->SetPosition(gfx::PointF());

            outer_scroll->AddChild(contents.Pass());
            outer_clip->AddChild(outer_scroll.Pass());
            inner_scroll->AddChild(outer_clip.Pass());
            page_scale->AddChild(inner_scroll.Pass());
            inner_clip->AddChild(page_scale.Pass());
            root->AddChild(inner_clip.Pass());

            layer_tree_impl->SetRootLayer(root.Pass());
            layer_tree_impl->SetViewportLayersFromIds(
                Layer::INVALID_ID, kPageScaleLayerId, kInnerViewportScrollLayerId,
                kOuterViewportScrollLayerId);

            layer_tree_impl->DidBecomeActive();
            return layer_tree_impl->InnerViewportScrollLayer();
        }

        LayerImpl* SetupScrollAndContentsLayers(const gfx::Size& content_size)
        {
            LayerImpl* scroll_layer = CreateScrollAndContentsLayers(
                host_impl_->active_tree(), content_size);
            host_impl_->active_tree()->BuildPropertyTreesForTesting();
            host_impl_->active_tree()->DidBecomeActive();
            return scroll_layer;
        }

        // Sets up a typical virtual viewport setup with one child content layer.
        // Returns a pointer to the content layer.
        LayerImpl* CreateBasicVirtualViewportLayers(const gfx::Size& viewport_size,
            const gfx::Size& content_size)
        {
            // CreateScrollAndContentsLayers makes the outer viewport unscrollable and
            // the inner a different size from the outer. We'll reuse its layer
            // hierarchy but adjust the sizing to our needs.
            CreateScrollAndContentsLayers(host_impl_->active_tree(), content_size);

            LayerImpl* content_layer = host_impl_->OuterViewportScrollLayer()->children().back();
            content_layer->SetBounds(content_size);
            host_impl_->OuterViewportScrollLayer()->SetBounds(content_size);

            LayerImpl* outer_clip = host_impl_->OuterViewportScrollLayer()->parent();
            outer_clip->SetBounds(viewport_size);

            LayerImpl* inner_clip_layer = host_impl_->InnerViewportScrollLayer()->parent()->parent();
            inner_clip_layer->SetBounds(viewport_size);
            host_impl_->InnerViewportScrollLayer()->SetBounds(viewport_size);

            host_impl_->SetViewportSize(viewport_size);
            host_impl_->active_tree()->DidBecomeActive();

            return content_layer;
        }

        scoped_ptr<LayerImpl> CreateScrollableLayer(int id,
            const gfx::Size& size,
            LayerImpl* clip_layer)
        {
            DCHECK(clip_layer);
            DCHECK(id != clip_layer->id());
            scoped_ptr<LayerImpl> layer = LayerImpl::Create(host_impl_->active_tree(), id);
            layer->SetScrollClipLayer(clip_layer->id());
            layer->SetDrawsContent(true);
            layer->SetBounds(size);
            clip_layer->SetBounds(gfx::Size(size.width() / 2, size.height() / 2));
            return layer.Pass();
        }

        void DrawFrame()
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
        }

        void RebuildPropertyTrees()
        {
            host_impl_->active_tree()->property_trees()->needs_rebuild = true;
            host_impl_->active_tree()->BuildPropertyTreesForTesting();
        }

        DrawResult PrepareToDrawFrame(LayerTreeHostImpl::FrameData* frame)
        {
            // We need to build property trees here before drawing the frame as they are
            // not built on the impl thread.
            RebuildPropertyTrees();
            return host_impl_->PrepareToDraw(frame);
        }

        void pinch_zoom_pan_viewport_forces_commit_redraw(float device_scale_factor);
        void pinch_zoom_pan_viewport_test(float device_scale_factor);
        void pinch_zoom_pan_viewport_and_scroll_test(float device_scale_factor);
        void pinch_zoom_pan_viewport_and_scroll_boundary_test(
            float device_scale_factor);

        void CheckNotifyCalledIfCanDrawChanged(bool always_draw)
        {
            // Note: It is not possible to disable the renderer once it has been set,
            // so we do not need to test that disabling the renderer notifies us
            // that can_draw changed.
            EXPECT_FALSE(host_impl_->CanDraw());
            on_can_draw_state_changed_called_ = false;

            // Set up the root layer, which allows us to draw.
            SetupScrollAndContentsLayers(gfx::Size(100, 100));
            EXPECT_TRUE(host_impl_->CanDraw());
            EXPECT_TRUE(on_can_draw_state_changed_called_);
            on_can_draw_state_changed_called_ = false;

            // Toggle the root layer to make sure it toggles can_draw
            host_impl_->active_tree()->SetRootLayer(nullptr);
            EXPECT_FALSE(host_impl_->CanDraw());
            EXPECT_TRUE(on_can_draw_state_changed_called_);
            on_can_draw_state_changed_called_ = false;

            SetupScrollAndContentsLayers(gfx::Size(100, 100));
            EXPECT_TRUE(host_impl_->CanDraw());
            EXPECT_TRUE(on_can_draw_state_changed_called_);
            on_can_draw_state_changed_called_ = false;

            // Toggle the device viewport size to make sure it toggles can_draw.
            host_impl_->SetViewportSize(gfx::Size());
            if (always_draw) {
                EXPECT_TRUE(host_impl_->CanDraw());
            } else {
                EXPECT_FALSE(host_impl_->CanDraw());
            }
            EXPECT_TRUE(on_can_draw_state_changed_called_);
            on_can_draw_state_changed_called_ = false;

            host_impl_->SetViewportSize(gfx::Size(100, 100));
            EXPECT_TRUE(host_impl_->CanDraw());
            EXPECT_TRUE(on_can_draw_state_changed_called_);
            on_can_draw_state_changed_called_ = false;
        }

        void SetupMouseMoveAtWithDeviceScale(float device_scale_factor);

    protected:
        virtual scoped_ptr<OutputSurface> CreateOutputSurface()
        {
            return FakeOutputSurface::Create3d();
        }

        void DrawOneFrame()
        {
            LayerTreeHostImpl::FrameData frame_data;
            PrepareToDrawFrame(&frame_data);
            host_impl_->DidDrawAllLayers(frame_data);
        }

        FakeProxy proxy_;
        DebugScopedSetImplThread always_impl_thread_;
        DebugScopedSetMainThreadBlocked always_main_thread_blocked_;

        TestSharedBitmapManager shared_bitmap_manager_;
        TestGpuMemoryBufferManager gpu_memory_buffer_manager_;
        TestTaskGraphRunner task_graph_runner_;
        scoped_ptr<OutputSurface> output_surface_;
        scoped_ptr<LayerTreeHostImpl> host_impl_;
        FakeRenderingStatsInstrumentation stats_instrumentation_;
        bool on_can_draw_state_changed_called_;
        bool did_notify_ready_to_activate_;
        bool did_request_commit_;
        bool did_request_redraw_;
        bool did_request_animate_;
        bool did_request_prepare_tiles_;
        bool did_complete_page_scale_animation_;
        bool reduce_memory_result_;
        base::Closure animation_task_;
        base::TimeDelta requested_animation_delay_;
    };

    // A test fixture for new animation timelines tests.
    class LayerTreeHostImplTimelinesTest : public LayerTreeHostImplTest {
    public:
        void SetUp() override
        {
            LayerTreeSettings settings = DefaultSettings();
            settings.use_compositor_animation_timelines = true;
            CreateHostImpl(settings, CreateOutputSurface());
        }
    };

    TEST_F(LayerTreeHostImplTest, NotifyIfCanDrawChanged)
    {
        bool always_draw = false;
        CheckNotifyCalledIfCanDrawChanged(always_draw);
    }

    TEST_F(LayerTreeHostImplTest, CanDrawIncompleteFrames)
    {
        CreateHostImpl(DefaultSettings(),
            FakeOutputSurface::CreateAlwaysDrawAndSwap3d());

        bool always_draw = true;
        CheckNotifyCalledIfCanDrawChanged(always_draw);
    }

    TEST_F(LayerTreeHostImplTest, ScrollDeltaNoLayers)
    {
        ASSERT_FALSE(host_impl_->active_tree()->root_layer());

        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        ASSERT_EQ(scroll_info->scrolls.size(), 0u);
    }

    TEST_F(LayerTreeHostImplTest, ScrollDeltaTreeButNoChanges)
    {
        {
            scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
            root->AddChild(LayerImpl::Create(host_impl_->active_tree(), 2));
            root->AddChild(LayerImpl::Create(host_impl_->active_tree(), 3));
            root->children()[1]->AddChild(
                LayerImpl::Create(host_impl_->active_tree(), 4));
            root->children()[1]->AddChild(
                LayerImpl::Create(host_impl_->active_tree(), 5));
            root->children()[1]->children()[0]->AddChild(
                LayerImpl::Create(host_impl_->active_tree(), 6));
            host_impl_->active_tree()->SetRootLayer(root.Pass());
        }
        LayerImpl* root = host_impl_->active_tree()->root_layer();

        ExpectClearedScrollDeltasRecursive(root);

        scoped_ptr<ScrollAndScaleSet> scroll_info;

        scroll_info = host_impl_->ProcessScrollDeltas();
        ASSERT_EQ(scroll_info->scrolls.size(), 0u);
        ExpectClearedScrollDeltasRecursive(root);

        scroll_info = host_impl_->ProcessScrollDeltas();
        ASSERT_EQ(scroll_info->scrolls.size(), 0u);
        ExpectClearedScrollDeltasRecursive(root);
    }

    TEST_F(LayerTreeHostImplTest, ScrollDeltaRepeatedScrolls)
    {
        gfx::ScrollOffset scroll_offset(20, 30);
        gfx::Vector2d scroll_delta(11, -15);
        {
            scoped_ptr<LayerImpl> root_clip = LayerImpl::Create(host_impl_->active_tree(), 2);
            scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
            root_clip->SetBounds(gfx::Size(10, 10));
            LayerImpl* root_layer = root.get();
            root_clip->AddChild(root.Pass());
            root_layer->SetBounds(gfx::Size(110, 110));
            root_layer->SetScrollClipLayer(root_clip->id());
            root_layer->PushScrollOffsetFromMainThread(scroll_offset);
            root_layer->ScrollBy(scroll_delta);
            host_impl_->active_tree()->SetRootLayer(root_clip.Pass());
        }
        LayerImpl* root = host_impl_->active_tree()->root_layer()->children()[0];

        scoped_ptr<ScrollAndScaleSet> scroll_info;

        scroll_info = host_impl_->ProcessScrollDeltas();
        ASSERT_EQ(scroll_info->scrolls.size(), 1u);
        EXPECT_TRUE(ScrollInfoContains(*scroll_info, root->id(), scroll_delta));

        gfx::Vector2d scroll_delta2(-5, 27);
        root->ScrollBy(scroll_delta2);
        scroll_info = host_impl_->ProcessScrollDeltas();
        ASSERT_EQ(scroll_info->scrolls.size(), 1u);
        EXPECT_TRUE(ScrollInfoContains(*scroll_info, root->id(),
            scroll_delta + scroll_delta2));

        root->ScrollBy(gfx::Vector2d());
        scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info, root->id(),
            scroll_delta + scroll_delta2));
    }

    TEST_F(LayerTreeHostImplTest, ScrollRootCallsCommitAndRedraw)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(),
            InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(0, 10),
            InputHandler::WHEEL));
        host_impl_->ScrollEnd();
        EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(),
            InputHandler::WHEEL));
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_TRUE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, ScrollActiveOnlyAfterScrollMovement)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        EXPECT_FALSE(host_impl_->IsActivelyScrolling());
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        EXPECT_TRUE(host_impl_->IsActivelyScrolling());
        host_impl_->ScrollEnd();
        EXPECT_FALSE(host_impl_->IsActivelyScrolling());
    }

    TEST_F(LayerTreeHostImplTest, ScrollWithoutRootLayer)
    {
        // We should not crash when trying to scroll an empty layer tree.
        EXPECT_EQ(InputHandler::SCROLL_IGNORED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
    }

    TEST_F(LayerTreeHostImplTest, ScrollWithoutRenderer)
    {
        scoped_ptr<TestWebGraphicsContext3D> context_owned = TestWebGraphicsContext3D::Create();
        context_owned->set_context_lost(true);

        // Initialization will fail.
        EXPECT_FALSE(CreateHostImpl(
            DefaultSettings(), FakeOutputSurface::Create3d(context_owned.Pass())));

        SetupScrollAndContentsLayers(gfx::Size(100, 100));

        // We should not crash when trying to scroll after the renderer initialization
        // fails.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
    }

    TEST_F(LayerTreeHostImplTest, ReplaceTreeWhileScrolling)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        // We should not crash if the tree is replaced while we are scrolling.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        host_impl_->active_tree()->DetachLayerTree();

        scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));

        // We should still be scrolling, because the scrolled layer also exists in the
        // new tree.
        gfx::Vector2d scroll_delta(0, 10);
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();
        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(
            ScrollInfoContains(*scroll_info, scroll_layer->id(), scroll_delta));
    }

    TEST_F(LayerTreeHostImplTest, ScrollBlocksOnWheelEventHandlers)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();
        LayerImpl* root = host_impl_->active_tree()->root_layer();

        // With registered event handlers, wheel scrolls don't necessarily
        // have to go to the main thread.
        root->SetHaveWheelEventHandlers(true);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        host_impl_->ScrollEnd();

        // But typically the scroll-blocks-on mode will require them to.
        root->SetScrollBlocksOn(SCROLL_BLOCKS_ON_WHEEL_EVENT | SCROLL_BLOCKS_ON_START_TOUCH);
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

        // But gesture scrolls can still be handled.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollEnd();

        // And if the handlers go away, wheel scrolls can again be processed
        // on impl (despite the scroll-blocks-on mode).
        root->SetHaveWheelEventHandlers(false);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        host_impl_->ScrollEnd();
    }

    TEST_F(LayerTreeHostImplTest, ScrollBlocksOnTouchEventHandlers)
    {
        LayerImpl* scroll = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();
        LayerImpl* root = host_impl_->active_tree()->root_layer();

        LayerImpl* child = 0;
        {
            scoped_ptr<LayerImpl> child_layer = LayerImpl::Create(host_impl_->active_tree(), 6);
            child = child_layer.get();
            child_layer->SetDrawsContent(true);
            child_layer->SetPosition(gfx::PointF(0, 20));
            child_layer->SetBounds(gfx::Size(50, 50));
            scroll->AddChild(child_layer.Pass());
            RebuildPropertyTrees();
        }

        // Touch handler regions determine whether touch events block scroll.
        root->SetTouchEventHandlerRegion(gfx::Rect(0, 0, 100, 100));
        EXPECT_FALSE(host_impl_->DoTouchEventsBlockScrollAt(gfx::Point(10, 10)));
        root->SetScrollBlocksOn(SCROLL_BLOCKS_ON_START_TOUCH | SCROLL_BLOCKS_ON_WHEEL_EVENT);
        EXPECT_TRUE(host_impl_->DoTouchEventsBlockScrollAt(gfx::Point(10, 10)));

        // But they don't influence the actual handling of the scroll gestures.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollEnd();

        // It's the union of scroll-blocks-on mode bits across all layers in the
        // scroll paret chain that matters.
        EXPECT_TRUE(host_impl_->DoTouchEventsBlockScrollAt(gfx::Point(10, 30)));
        root->SetScrollBlocksOn(SCROLL_BLOCKS_ON_NONE);
        EXPECT_FALSE(host_impl_->DoTouchEventsBlockScrollAt(gfx::Point(10, 30)));
        child->SetScrollBlocksOn(SCROLL_BLOCKS_ON_START_TOUCH);
        EXPECT_TRUE(host_impl_->DoTouchEventsBlockScrollAt(gfx::Point(10, 30)));
    }

    TEST_F(LayerTreeHostImplTest, ScrollBlocksOnScrollEventHandlers)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();
        LayerImpl* root = host_impl_->active_tree()->root_layer();

        // With registered scroll handlers, scrolls don't generally have to go
        // to the main thread.
        root->SetHaveScrollEventHandlers(true);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        host_impl_->ScrollEnd();

        // Even the default scroll blocks on mode doesn't require this.
        root->SetScrollBlocksOn(SCROLL_BLOCKS_ON_WHEEL_EVENT | SCROLL_BLOCKS_ON_START_TOUCH);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollEnd();

        // But the page can opt in to blocking on scroll event handlers.
        root->SetScrollBlocksOn(SCROLL_BLOCKS_ON_SCROLL_EVENT);
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        // GESTURE and WHEEL scrolls behave identically in this regard.
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

        // And if the handlers go away, scrolls can again be processed on impl
        // (despite the scroll-blocks-on mode).
        root->SetHaveScrollEventHandlers(false);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollEnd();
    }

    TEST_F(LayerTreeHostImplTest, ScrollBlocksOnLayerTopology)
    {
        host_impl_->SetViewportSize(gfx::Size(50, 50));

        // Create a normal scrollable root layer
        LayerImpl* root_scroll = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        LayerImpl* root_child = root_scroll->children()[0];
        LayerImpl* root = host_impl_->active_tree()->root_layer();
        DrawFrame();

        // Create two child scrollable layers
        LayerImpl* child1 = 0;
        {
            scoped_ptr<LayerImpl> scrollable_child_clip_1 = LayerImpl::Create(host_impl_->active_tree(), 6);
            scoped_ptr<LayerImpl> scrollable_child_1 = CreateScrollableLayer(
                7, gfx::Size(10, 10), scrollable_child_clip_1.get());
            child1 = scrollable_child_1.get();
            scrollable_child_1->SetPosition(gfx::Point(5, 5));
            scrollable_child_1->SetHaveWheelEventHandlers(true);
            scrollable_child_1->SetHaveScrollEventHandlers(true);
            scrollable_child_clip_1->AddChild(scrollable_child_1.Pass());
            root_child->AddChild(scrollable_child_clip_1.Pass());
            RebuildPropertyTrees();
        }

        LayerImpl* child2 = 0;
        {
            scoped_ptr<LayerImpl> scrollable_child_clip_2 = LayerImpl::Create(host_impl_->active_tree(), 8);
            scoped_ptr<LayerImpl> scrollable_child_2 = CreateScrollableLayer(
                9, gfx::Size(10, 10), scrollable_child_clip_2.get());
            child2 = scrollable_child_2.get();
            scrollable_child_2->SetPosition(gfx::Point(5, 20));
            scrollable_child_2->SetHaveWheelEventHandlers(true);
            scrollable_child_2->SetHaveScrollEventHandlers(true);
            scrollable_child_clip_2->AddChild(scrollable_child_2.Pass());
            root_child->AddChild(scrollable_child_clip_2.Pass());
            RebuildPropertyTrees();
        }

        // Scroll-blocks-on on a layer affects scrolls that hit that layer.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(10, 10), InputHandler::GESTURE));
        host_impl_->ScrollEnd();
        child1->SetScrollBlocksOn(SCROLL_BLOCKS_ON_SCROLL_EVENT);
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(10, 10), InputHandler::GESTURE));

        // But not those that hit only other layers.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(10, 25), InputHandler::GESTURE));
        host_impl_->ScrollEnd();

        // It's the union of bits set across the scroll ancestor chain that matters.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(10, 25), InputHandler::GESTURE));
        host_impl_->ScrollEnd();
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(10, 25), InputHandler::WHEEL));
        host_impl_->ScrollEnd();
        root->SetScrollBlocksOn(SCROLL_BLOCKS_ON_WHEEL_EVENT);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(10, 25), InputHandler::GESTURE));
        host_impl_->ScrollEnd();
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(10, 25), InputHandler::WHEEL));
        child2->SetScrollBlocksOn(SCROLL_BLOCKS_ON_SCROLL_EVENT);
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(10, 25), InputHandler::WHEEL));
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(10, 25), InputHandler::GESTURE));
    }

    TEST_F(LayerTreeHostImplTest, FlingOnlyWhenScrollingTouchscreen)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        // Ignore the fling since no layer is being scrolled
        EXPECT_EQ(InputHandler::SCROLL_IGNORED, host_impl_->FlingScrollBegin());

        // Start scrolling a layer
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        // Now the fling should go ahead since we've started scrolling a layer
        EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
    }

    TEST_F(LayerTreeHostImplTest, FlingOnlyWhenScrollingTouchpad)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        // Ignore the fling since no layer is being scrolled
        EXPECT_EQ(InputHandler::SCROLL_IGNORED, host_impl_->FlingScrollBegin());

        // Start scrolling a layer
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

        // Now the fling should go ahead since we've started scrolling a layer
        EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
    }

    TEST_F(LayerTreeHostImplTest, NoFlingWhenScrollingOnMain)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();
        LayerImpl* root = host_impl_->active_tree()->root_layer();

        root->SetShouldScrollOnMainThread(true);

        // Start scrolling a layer
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        // The fling should be ignored since there's no layer being scrolled impl-side
        EXPECT_EQ(InputHandler::SCROLL_IGNORED, host_impl_->FlingScrollBegin());
    }

    TEST_F(LayerTreeHostImplTest, ShouldScrollOnMainThread)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();
        LayerImpl* root = host_impl_->active_tree()->root_layer();

        root->SetShouldScrollOnMainThread(true);

        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
    }

    TEST_F(LayerTreeHostImplTest, NonFastScrollableRegionBasic)
    {
        SetupScrollAndContentsLayers(gfx::Size(200, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));

        LayerImpl* root = host_impl_->active_tree()->root_layer();
        root->SetNonFastScrollableRegion(gfx::Rect(0, 0, 50, 50));

        DrawFrame();

        // All scroll types inside the non-fast scrollable region should fail.
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(25, 25), InputHandler::WHEEL));
        EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(25, 25),
            InputHandler::WHEEL));
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(25, 25), InputHandler::GESTURE));
        EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(25, 25),
            InputHandler::GESTURE));

        // All scroll types outside this region should succeed.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(75, 75), InputHandler::WHEEL));
        EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(75, 75),
            InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(25, 25),
            InputHandler::GESTURE));
        host_impl_->ScrollEnd();
        EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(75, 75),
            InputHandler::GESTURE));
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(75, 75), InputHandler::GESTURE));
        EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(75, 75),
            InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        host_impl_->ScrollEnd();
        EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(75, 75),
            InputHandler::GESTURE));
    }

    TEST_F(LayerTreeHostImplTest, NonFastScrollableRegionWithOffset)
    {
        SetupScrollAndContentsLayers(gfx::Size(200, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));

        LayerImpl* root = host_impl_->active_tree()->root_layer();
        root->SetNonFastScrollableRegion(gfx::Rect(0, 0, 50, 50));
        root->SetPosition(gfx::PointF(-25.f, 0.f));

        DrawFrame();

        // This point would fall into the non-fast scrollable region except that we've
        // moved the layer down by 25 pixels.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(40, 10), InputHandler::WHEEL));
        EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(40, 10),
            InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 1));
        host_impl_->ScrollEnd();

        // This point is still inside the non-fast region.
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(10, 10), InputHandler::WHEEL));
    }

    TEST_F(LayerTreeHostImplTest, ScrollHandlerNotPresent)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(200, 200));
        EXPECT_FALSE(scroll_layer->have_scroll_event_handlers());
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        EXPECT_FALSE(host_impl_->scroll_affects_scroll_handler());
        host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
        EXPECT_FALSE(host_impl_->scroll_affects_scroll_handler());
        host_impl_->ScrollEnd();
        EXPECT_FALSE(host_impl_->scroll_affects_scroll_handler());
    }

    TEST_F(LayerTreeHostImplTest, ScrollHandlerPresent)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(200, 200));
        scroll_layer->SetHaveScrollEventHandlers(true);
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        EXPECT_FALSE(host_impl_->scroll_affects_scroll_handler());
        host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
        EXPECT_TRUE(host_impl_->scroll_affects_scroll_handler());
        host_impl_->ScrollEnd();
        EXPECT_FALSE(host_impl_->scroll_affects_scroll_handler());
    }

    TEST_F(LayerTreeHostImplTest, ScrollByReturnsCorrectValue)
    {
        SetupScrollAndContentsLayers(gfx::Size(200, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));

        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        // Trying to scroll to the left/top will not succeed.
        EXPECT_FALSE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(-10, 0)).did_scroll);
        EXPECT_FALSE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -10)).did_scroll);
        EXPECT_FALSE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(-10, -10)).did_scroll);

        // Scrolling to the right/bottom will succeed.
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(10, 0)).did_scroll);
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10)).did_scroll);
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(10, 10)).did_scroll);

        // Scrolling to left/top will now succeed.
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(-10, 0)).did_scroll);
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -10)).did_scroll);
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(-10, -10)).did_scroll);

        // Scrolling diagonally against an edge will succeed.
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(10, -10)).did_scroll);
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(-10, 0)).did_scroll);
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(-10, 10)).did_scroll);

        // Trying to scroll more than the available space will also succeed.
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(5000, 5000)).did_scroll);
    }

    TEST_F(LayerTreeHostImplTest, ScrollVerticallyByPageReturnsCorrectValue)
    {
        SetupScrollAndContentsLayers(gfx::Size(200, 2000));
        host_impl_->SetViewportSize(gfx::Size(100, 1000));

        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

        // Trying to scroll without a vertical scrollbar will fail.
        EXPECT_FALSE(host_impl_->ScrollVerticallyByPage(
            gfx::Point(), SCROLL_FORWARD));
        EXPECT_FALSE(host_impl_->ScrollVerticallyByPage(
            gfx::Point(), SCROLL_BACKWARD));

        scoped_ptr<PaintedScrollbarLayerImpl> vertical_scrollbar(
            PaintedScrollbarLayerImpl::Create(
                host_impl_->active_tree(),
                20,
                VERTICAL));
        vertical_scrollbar->SetBounds(gfx::Size(15, 1000));
        host_impl_->InnerViewportScrollLayer()->AddScrollbar(
            vertical_scrollbar.get());

        // Trying to scroll with a vertical scrollbar will succeed.
        EXPECT_TRUE(host_impl_->ScrollVerticallyByPage(
            gfx::Point(), SCROLL_FORWARD));
        EXPECT_FLOAT_EQ(875.f,
            host_impl_->InnerViewportScrollLayer()->ScrollDelta().y());
        EXPECT_TRUE(host_impl_->ScrollVerticallyByPage(
            gfx::Point(), SCROLL_BACKWARD));
    }

    TEST_F(LayerTreeHostImplTest, ScrollWithUserUnscrollableLayers)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(200, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));

        gfx::Size overflow_size(400, 400);
        ASSERT_EQ(1u, scroll_layer->children().size());
        LayerImpl* overflow = scroll_layer->children()[0];
        overflow->SetBounds(overflow_size);
        overflow->SetScrollClipLayer(scroll_layer->parent()->id());
        overflow->PushScrollOffsetFromMainThread(gfx::ScrollOffset());
        overflow->SetPosition(gfx::PointF());

        DrawFrame();
        gfx::Point scroll_position(10, 10);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(scroll_position, InputHandler::WHEEL));
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), overflow->CurrentScrollOffset());

        gfx::Vector2dF scroll_delta(10, 10);
        host_impl_->ScrollBy(scroll_position, scroll_delta);
        host_impl_->ScrollEnd();
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(10, 10), overflow->CurrentScrollOffset());

        overflow->set_user_scrollable_horizontal(false);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(scroll_position, InputHandler::WHEEL));
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(10, 10), overflow->CurrentScrollOffset());

        host_impl_->ScrollBy(scroll_position, scroll_delta);
        host_impl_->ScrollEnd();
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, 0), scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(10, 20), overflow->CurrentScrollOffset());

        overflow->set_user_scrollable_vertical(false);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(scroll_position, InputHandler::WHEEL));
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, 0), scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(10, 20), overflow->CurrentScrollOffset());

        host_impl_->ScrollBy(scroll_position, scroll_delta);
        host_impl_->ScrollEnd();
        EXPECT_VECTOR_EQ(gfx::Vector2dF(10, 10), scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(10, 20), overflow->CurrentScrollOffset());
    }

    TEST_F(LayerTreeHostImplTest, AnimationSchedulingPendingTree)
    {
        host_impl_->SetViewportSize(gfx::Size(50, 50));

        host_impl_->CreatePendingTree();
        host_impl_->pending_tree()->SetRootLayer(
            LayerImpl::Create(host_impl_->pending_tree(), 1));
        LayerImpl* root = host_impl_->pending_tree()->root_layer();
        root->SetBounds(gfx::Size(50, 50));
        root->SetHasRenderSurface(true);

        root->AddChild(LayerImpl::Create(host_impl_->pending_tree(), 2));
        LayerImpl* child = root->children()[0];
        child->SetBounds(gfx::Size(10, 10));
        child->draw_properties().visible_layer_rect = gfx::Rect(10, 10);
        child->SetDrawsContent(true);
        AddAnimatedTransformToLayer(child, 10.0, 3, 0);

        EXPECT_FALSE(did_request_animate_);
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);

        host_impl_->Animate();

        // An animation exists on the pending layer. Doing Animate() requests another
        // frame.
        // In reality, animations without has_set_start_time() == true do not need to
        // be continuously ticked on the pending tree, so it should not request
        // another animation frame here. But we currently do so blindly if any
        // animation exists.
        EXPECT_TRUE(did_request_animate_);
        // The pending tree with an animation does not need to draw after animating.
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);

        did_request_animate_ = false;
        did_request_redraw_ = false;
        did_request_commit_ = false;

        host_impl_->ActivateSyncTree();

        // When the animation activates, we should request another animation frame
        // to keep the animation moving.
        EXPECT_TRUE(did_request_animate_);
        // On activation we don't need to request a redraw for the animation,
        // activating will draw on its own when it's ready.
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, AnimationSchedulingActiveTree)
    {
        host_impl_->SetViewportSize(gfx::Size(50, 50));

        host_impl_->active_tree()->SetRootLayer(
            LayerImpl::Create(host_impl_->active_tree(), 1));
        LayerImpl* root = host_impl_->active_tree()->root_layer();
        root->SetBounds(gfx::Size(50, 50));
        root->SetHasRenderSurface(true);

        root->AddChild(LayerImpl::Create(host_impl_->active_tree(), 2));
        LayerImpl* child = root->children()[0];
        child->SetBounds(gfx::Size(10, 10));
        child->draw_properties().visible_layer_rect = gfx::Rect(10, 10);
        child->SetDrawsContent(true);

        // Add a translate from 6,7 to 8,9.
        TransformOperations start;
        start.AppendTranslate(6.f, 7.f, 0.f);
        TransformOperations end;
        end.AppendTranslate(8.f, 9.f, 0.f);
        AddAnimatedTransformToLayer(child, 4.0, start, end);

        base::TimeTicks now = base::TimeTicks::Now();
        host_impl_->WillBeginImplFrame(
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now));

        EXPECT_FALSE(did_request_animate_);
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);

        host_impl_->ActivateAnimations();
        did_request_animate_ = false;
        did_request_redraw_ = false;
        did_request_commit_ = false;

        host_impl_->Animate();

        // An animation exists on the active layer. Doing Animate() requests another
        // frame after the current one.
        EXPECT_TRUE(did_request_animate_);
        // TODO(danakj): We also need to draw in the current frame if something
        // animated, but this is currently handled by
        // SchedulerStateMachine::WillAnimate.
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, ImplPinchZoom)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        EXPECT_EQ(scroll_layer, host_impl_->InnerViewportScrollLayer());
        LayerImpl* container_layer = scroll_layer->scroll_clip_layer();
        EXPECT_EQ(gfx::Size(50, 50), container_layer->bounds());

        float min_page_scale = 1.f, max_page_scale = 4.f;
        float page_scale_factor = 1.f;

        // The impl-based pinch zoom should adjust the max scroll position.
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(
                page_scale_factor, min_page_scale, max_page_scale);
            host_impl_->active_tree()->SetPageScaleOnActiveTree(page_scale_factor);
            scroll_layer->SetScrollDelta(gfx::Vector2d());

            float page_scale_delta = 2.f;

            host_impl_->ScrollBegin(gfx::Point(50, 50), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point(50, 50));
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();
            EXPECT_FALSE(did_request_animate_);
            EXPECT_TRUE(did_request_redraw_);
            EXPECT_TRUE(did_request_commit_);
            EXPECT_EQ(gfx::Size(50, 50), container_layer->bounds());

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, page_scale_delta);

            EXPECT_EQ(gfx::ScrollOffset(75.0, 75.0).ToString(),
                scroll_layer->MaxScrollOffset().ToString());
        }

        // Scrolling after a pinch gesture should always be in local space.  The
        // scroll deltas have the page scale factor applied.
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(
                page_scale_factor, min_page_scale, max_page_scale);
            host_impl_->active_tree()->SetPageScaleOnActiveTree(page_scale_factor);
            scroll_layer->SetScrollDelta(gfx::Vector2d());

            float page_scale_delta = 2.f;
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point());
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();

            gfx::Vector2d scroll_delta(0, 10);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_TRUE(ScrollInfoContains(
                *scroll_info.get(), scroll_layer->id(),
                gfx::Vector2d(0, scroll_delta.y() / page_scale_delta)));
        }
    }

    TEST_F(LayerTreeHostImplTest, ViewportScrollOrder)
    {
        LayerTreeSettings settings = DefaultSettings();
        CreateHostImpl(settings,
            CreateOutputSurface());
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.25f, 4.f);

        const gfx::Size content_size(1000, 1000);
        const gfx::Size viewport_size(500, 500);
        CreateBasicVirtualViewportLayers(viewport_size, content_size);

        LayerImpl* outer_scroll_layer = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll_layer = host_impl_->InnerViewportScrollLayer();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(500, 500),
            outer_scroll_layer->MaxScrollOffset());

        RebuildPropertyTrees();
        host_impl_->ScrollBegin(gfx::Point(250, 250), InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(2.f, gfx::Point(0, 0));
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();

        // Sanity check - we're zoomed in, starting from the origin.
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(0, 0),
            outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(0, 0),
            inner_scroll_layer->CurrentScrollOffset());

        // Scroll down - only the inner viewport should scroll.
        host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::GESTURE);
        host_impl_->ScrollBy(gfx::Point(0, 0), gfx::Vector2dF(100.f, 100.f));
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(50, 50),
            inner_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(0, 0),
            outer_scroll_layer->CurrentScrollOffset());

        // Scroll down - outer viewport should start scrolling after the inner is at
        // its maximum.
        host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::GESTURE);
        host_impl_->ScrollBy(gfx::Point(0, 0), gfx::Vector2dF(1000.f, 1000.f));
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(250, 250),
            inner_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(300, 300),
            outer_scroll_layer->CurrentScrollOffset());
    }

    // Make sure scrolls smaller than a unit applied to the viewport don't get
    // dropped. crbug.com/539334.
    TEST_F(LayerTreeHostImplTest, ScrollViewportWithFractionalAmounts)
    {
        LayerTreeSettings settings = DefaultSettings();
        CreateHostImpl(settings, CreateOutputSurface());
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 2.f);

        const gfx::Size content_size(1000, 1000);
        const gfx::Size viewport_size(500, 500);
        CreateBasicVirtualViewportLayers(viewport_size, content_size);

        LayerImpl* outer_scroll_layer = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll_layer = host_impl_->InnerViewportScrollLayer();

        // Sanity checks.
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(500, 500),
            outer_scroll_layer->MaxScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), inner_scroll_layer->CurrentScrollOffset());

        RebuildPropertyTrees();

        // Scroll only the layout viewport.
        host_impl_->ScrollBegin(gfx::Point(250, 250), InputHandler::GESTURE);
        host_impl_->ScrollBy(gfx::Point(250, 250), gfx::Vector2dF(0.125f, 0.125f));
        EXPECT_VECTOR2DF_EQ(
            gfx::Vector2dF(0.125f, 0.125f),
            outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR2DF_EQ(
            gfx::Vector2dF(0, 0),
            inner_scroll_layer->CurrentScrollOffset());
        host_impl_->ScrollEnd();

        host_impl_->active_tree()->PushPageScaleFromMainThread(2.f, 1.f, 2.f);

        // Now that we zoomed in, the scroll should be applied to the inner viewport.
        host_impl_->ScrollBegin(gfx::Point(250, 250), InputHandler::GESTURE);
        host_impl_->ScrollBy(gfx::Point(250, 250), gfx::Vector2dF(0.5f, 0.5f));
        EXPECT_VECTOR2DF_EQ(
            gfx::Vector2dF(0.125f, 0.125f),
            outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR2DF_EQ(
            gfx::Vector2dF(0.25f, 0.25f),
            inner_scroll_layer->CurrentScrollOffset());
        host_impl_->ScrollEnd();
    }

    // Tests that scrolls during a pinch gesture (i.e. "two-finger" scrolls) work
    // as expected. That is, scrolling during a pinch should bubble from the inner
    // to the outer viewport.
    TEST_F(LayerTreeHostImplTest, ScrollDuringPinchGesture)
    {
        LayerTreeSettings settings = DefaultSettings();
        CreateHostImpl(settings,
            CreateOutputSurface());
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 2.f);

        const gfx::Size content_size(1000, 1000);
        const gfx::Size viewport_size(500, 500);
        CreateBasicVirtualViewportLayers(viewport_size, content_size);

        LayerImpl* outer_scroll_layer = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll_layer = host_impl_->InnerViewportScrollLayer();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(500, 500),
            outer_scroll_layer->MaxScrollOffset());

        RebuildPropertyTrees();
        host_impl_->ScrollBegin(gfx::Point(250, 250), InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();

        host_impl_->PinchGestureUpdate(2, gfx::Point(250, 250));
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(0, 0),
            outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(125, 125),
            inner_scroll_layer->CurrentScrollOffset());

        // Needed so that the pinch is accounted for in draw properties.
        DrawFrame();

        host_impl_->ScrollBy(gfx::Point(250, 250), gfx::Vector2dF(10.f, 10.f));
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(0, 0),
            outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(130, 130),
            inner_scroll_layer->CurrentScrollOffset());

        DrawFrame();

        host_impl_->ScrollBy(gfx::Point(250, 250), gfx::Vector2dF(400.f, 400.f));
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(80, 80),
            outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(250, 250),
            inner_scroll_layer->CurrentScrollOffset());

        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();
    }

    // Tests the "snapping" of pinch-zoom gestures to the screen edge. That is, when
    // a pinch zoom is anchored within a certain margin of the screen edge, we
    // should assume the user means to scroll into the edge of the screen.
    TEST_F(LayerTreeHostImplTest, PinchZoomSnapsToScreenEdge)
    {
        LayerTreeSettings settings = DefaultSettings();
        CreateHostImpl(settings,
            CreateOutputSurface());
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 2.f);

        const gfx::Size content_size(1000, 1000);
        const gfx::Size viewport_size(500, 500);
        CreateBasicVirtualViewportLayers(viewport_size, content_size);

        int offsetFromEdge = Viewport::kPinchZoomSnapMarginDips - 5;
        gfx::Point anchor(viewport_size.width() - offsetFromEdge,
            viewport_size.height() - offsetFromEdge);

        // Pinch in within the margins. The scroll should stay exactly locked to the
        // bottom and right.
        RebuildPropertyTrees();
        host_impl_->ScrollBegin(anchor, InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(2, anchor);
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(250, 250),
            host_impl_->InnerViewportScrollLayer()->CurrentScrollOffset());

        // Reset.
        host_impl_->active_tree()->SetPageScaleOnActiveTree(1.f);
        host_impl_->InnerViewportScrollLayer()->SetScrollDelta(gfx::Vector2d());
        host_impl_->OuterViewportScrollLayer()->SetScrollDelta(gfx::Vector2d());

        // Pinch in within the margins. The scroll should stay exactly locked to the
        // top and left.
        anchor = gfx::Point(offsetFromEdge, offsetFromEdge);
        host_impl_->ScrollBegin(anchor, InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(2, anchor);
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(0, 0),
            host_impl_->InnerViewportScrollLayer()->CurrentScrollOffset());

        // Reset.
        host_impl_->active_tree()->SetPageScaleOnActiveTree(1.f);
        host_impl_->InnerViewportScrollLayer()->SetScrollDelta(gfx::Vector2d());
        host_impl_->OuterViewportScrollLayer()->SetScrollDelta(gfx::Vector2d());

        // Pinch in just outside the margin. There should be no snapping.
        offsetFromEdge = Viewport::kPinchZoomSnapMarginDips;
        anchor = gfx::Point(offsetFromEdge, offsetFromEdge);
        host_impl_->ScrollBegin(anchor, InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(2, anchor);
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(50, 50),
            host_impl_->InnerViewportScrollLayer()->CurrentScrollOffset());

        // Reset.
        host_impl_->active_tree()->SetPageScaleOnActiveTree(1.f);
        host_impl_->InnerViewportScrollLayer()->SetScrollDelta(gfx::Vector2d());
        host_impl_->OuterViewportScrollLayer()->SetScrollDelta(gfx::Vector2d());

        // Pinch in just outside the margin. There should be no snapping.
        offsetFromEdge = Viewport::kPinchZoomSnapMarginDips;
        anchor = gfx::Point(viewport_size.width() - offsetFromEdge,
            viewport_size.height() - offsetFromEdge);
        host_impl_->ScrollBegin(anchor, InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(2, anchor);
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(200, 200),
            host_impl_->InnerViewportScrollLayer()->CurrentScrollOffset());
    }

    TEST_F(LayerTreeHostImplTest, ImplPinchZoomWheelBubbleBetweenViewports)
    {
        const gfx::Size content_size(200, 200);
        const gfx::Size viewport_size(100, 100);
        CreateBasicVirtualViewportLayers(viewport_size, content_size);

        LayerImpl* outer_scroll_layer = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll_layer = host_impl_->InnerViewportScrollLayer();

        // Zoom into the page by a 2X factor
        float min_page_scale = 1.f, max_page_scale = 4.f;
        float page_scale_factor = 2.f;
        RebuildPropertyTrees();
        host_impl_->active_tree()->PushPageScaleFromMainThread(
            page_scale_factor, min_page_scale, max_page_scale);
        host_impl_->active_tree()->SetPageScaleOnActiveTree(page_scale_factor);

        // Scroll by a small amount, there should be no bubbling to the outer
        // viewport.
        host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::WHEEL);
        host_impl_->ScrollBy(gfx::Point(0, 0), gfx::Vector2dF(10.f, 20.f));
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(5, 10),
            inner_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(),
            outer_scroll_layer->CurrentScrollOffset());

        // Scroll by the inner viewport's max scroll extent, the remainder
        // should bubble up to the outer viewport.
        host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::WHEEL);
        host_impl_->ScrollBy(gfx::Point(0, 0), gfx::Vector2dF(100.f, 100.f));
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(50, 50),
            inner_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(5, 10),
            outer_scroll_layer->CurrentScrollOffset());

        // Scroll by the outer viewport's max scroll extent, it should all go to the
        // outer viewport.
        host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::WHEEL);
        host_impl_->ScrollBy(gfx::Point(0, 0), gfx::Vector2dF(190.f, 180.f));
        host_impl_->ScrollEnd();

        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(100, 100),
            outer_scroll_layer->CurrentScrollOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(50, 50),
            inner_scroll_layer->CurrentScrollOffset());
    }

    TEST_F(LayerTreeHostImplTest, ScrollWithSwapPromises)
    {
        ui::LatencyInfo latency_info;
        latency_info.AddLatencyNumber(ui::INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT, 0,
            1234);
        scoped_ptr<SwapPromise> swap_promise(
            new LatencyInfoSwapPromise(latency_info));

        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        host_impl_->QueueSwapPromiseForMainThreadScrollUpdate(swap_promise.Pass());
        host_impl_->ScrollEnd();

        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_EQ(1u, scroll_info->swap_promises.size());
        EXPECT_EQ(latency_info.trace_id(), scroll_info->swap_promises[0]->TraceId());
    }

    // Test that scrolls targeting a layer with a non-null scroll_parent() don't
    // bubble up.
    TEST_F(LayerTreeHostImplTest, ScrollDoesntBubble)
    {
        LayerImpl* viewport_scroll = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));

        // Set up two scrolling children of the root, one of which is a scroll parent
        // to the other. Scrolls shouldn't bubbling from the child.
        LayerImpl* parent;
        LayerImpl* child;
        LayerImpl* child_clip;

        scoped_ptr<LayerImpl> scroll_parent_clip = LayerImpl::Create(host_impl_->active_tree(), 6);
        scoped_ptr<LayerImpl> scroll_parent = CreateScrollableLayer(
            7, gfx::Size(10, 10), scroll_parent_clip.get());
        parent = scroll_parent.get();
        scroll_parent_clip->AddChild(scroll_parent.Pass());

        viewport_scroll->AddChild(scroll_parent_clip.Pass());

        scoped_ptr<LayerImpl> scroll_child_clip = LayerImpl::Create(host_impl_->active_tree(), 8);
        scoped_ptr<LayerImpl> scroll_child = CreateScrollableLayer(
            9, gfx::Size(10, 10), scroll_child_clip.get());
        child = scroll_child.get();
        scroll_child->SetPosition(gfx::Point(20, 20));
        scroll_child_clip->AddChild(scroll_child.Pass());

        child_clip = scroll_child_clip.get();
        viewport_scroll->AddChild(scroll_child_clip.Pass());

        child_clip->SetScrollParent(parent);
        scoped_ptr<std::set<LayerImpl*>> scroll_children(new std::set<LayerImpl*>);
        scroll_children->insert(child_clip);
        parent->SetScrollChildren(scroll_children.release());

        DrawFrame();

        {
            host_impl_->ScrollBegin(gfx::Point(21, 21), InputHandler::GESTURE);
            host_impl_->ScrollBy(gfx::Point(21, 21), gfx::Vector2d(5, 5));
            host_impl_->ScrollBy(gfx::Point(21, 21), gfx::Vector2d(100, 100));
            host_impl_->ScrollEnd();

            // The child should be fully scrolled by the first ScrollBy.
            EXPECT_VECTOR_EQ(gfx::Vector2dF(5, 5), child->CurrentScrollOffset());

            // The scroll_parent shouldn't receive the second ScrollBy.
            EXPECT_VECTOR_EQ(gfx::Vector2dF(0, 0), parent->CurrentScrollOffset());

            // The viewport shouldn't have been scrolled at all.
            EXPECT_VECTOR_EQ(
                gfx::Vector2dF(0, 0),
                host_impl_->InnerViewportScrollLayer()->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(
                gfx::Vector2dF(0, 0),
                host_impl_->OuterViewportScrollLayer()->CurrentScrollOffset());
        }

        {
            host_impl_->ScrollBegin(gfx::Point(21, 21), InputHandler::GESTURE);
            host_impl_->ScrollBy(gfx::Point(21, 21), gfx::Vector2d(3, 4));
            host_impl_->ScrollBy(gfx::Point(21, 21), gfx::Vector2d(2, 1));
            host_impl_->ScrollBy(gfx::Point(21, 21), gfx::Vector2d(2, 1));
            host_impl_->ScrollBy(gfx::Point(21, 21), gfx::Vector2d(2, 1));
            host_impl_->ScrollEnd();

            // The ScrollBy's should scroll the parent to its extent.
            EXPECT_VECTOR_EQ(gfx::Vector2dF(5, 5), parent->CurrentScrollOffset());

            // The viewport shouldn't receive any scroll delta.
            EXPECT_VECTOR_EQ(
                gfx::Vector2dF(0, 0),
                host_impl_->InnerViewportScrollLayer()->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(
                gfx::Vector2dF(0, 0),
                host_impl_->OuterViewportScrollLayer()->CurrentScrollOffset());
        }
    }

    TEST_F(LayerTreeHostImplTest, PinchGesture)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        LayerImpl* scroll_layer = host_impl_->InnerViewportScrollLayer();
        DCHECK(scroll_layer);

        float min_page_scale = 1.f;
        float max_page_scale = 4.f;

        // Basic pinch zoom in gesture
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->SetScrollDelta(gfx::Vector2d());

            float page_scale_delta = 2.f;
            host_impl_->ScrollBegin(gfx::Point(50, 50), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point(50, 50));
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();
            EXPECT_FALSE(did_request_animate_);
            EXPECT_TRUE(did_request_redraw_);
            EXPECT_TRUE(did_request_commit_);

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, page_scale_delta);
        }

        // Zoom-in clamping
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->SetScrollDelta(gfx::Vector2d());
            float page_scale_delta = 10.f;

            host_impl_->ScrollBegin(gfx::Point(50, 50), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point(50, 50));
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, max_page_scale);
        }

        // Zoom-out clamping
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->SetScrollDelta(gfx::Vector2d());
            scroll_layer->PullDeltaForMainThread();
            scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(50, 50));

            float page_scale_delta = 0.1f;
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point());
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, min_page_scale);

            EXPECT_TRUE(scroll_info->scrolls.empty());
        }

        // Two-finger panning should not happen based on pinch events only
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->SetScrollDelta(gfx::Vector2d());
            scroll_layer->PullDeltaForMainThread();
            scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(20, 20));

            float page_scale_delta = 1.f;
            host_impl_->ScrollBegin(gfx::Point(10, 10), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point(10, 10));
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point(20, 20));
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, page_scale_delta);
            EXPECT_TRUE(scroll_info->scrolls.empty());
        }

        // Two-finger panning should work with interleaved scroll events
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->SetScrollDelta(gfx::Vector2d());
            scroll_layer->PullDeltaForMainThread();
            scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(20, 20));

            float page_scale_delta = 1.f;
            host_impl_->ScrollBegin(gfx::Point(10, 10), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point(10, 10));
            host_impl_->ScrollBy(gfx::Point(10, 10), gfx::Vector2d(-10, -10));
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point(20, 20));
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, page_scale_delta);
            EXPECT_TRUE(ScrollInfoContains(*scroll_info, scroll_layer->id(),
                gfx::Vector2d(-10, -10)));
        }

        // Two-finger panning should work when starting fully zoomed out.
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(0.5f, 0.5f, 4.f);
            scroll_layer->SetScrollDelta(gfx::Vector2d());
            scroll_layer->PullDeltaForMainThread();
            scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 0));

            host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(2.f, gfx::Point(0, 0));
            host_impl_->PinchGestureUpdate(1.f, gfx::Point(0, 0));

            // Needed so layer transform includes page scale.
            DrawFrame();

            host_impl_->ScrollBy(gfx::Point(0, 0), gfx::Vector2d(10, 10));
            host_impl_->PinchGestureUpdate(1.f, gfx::Point(10, 10));
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, 2.f);
            EXPECT_TRUE(ScrollInfoContains(*scroll_info, scroll_layer->id(),
                gfx::Vector2d(10, 10)));
        }
    }

    TEST_F(LayerTreeHostImplTest, PageScaleAnimation)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        LayerImpl* scroll_layer = host_impl_->InnerViewportScrollLayer();
        DCHECK(scroll_layer);

        float min_page_scale = 0.5f;
        float max_page_scale = 4.f;
        base::TimeTicks start_time = base::TimeTicks() + base::TimeDelta::FromSeconds(1);
        base::TimeDelta duration = base::TimeDelta::FromMilliseconds(100);
        base::TimeTicks halfway_through_animation = start_time + duration / 2;
        base::TimeTicks end_time = start_time + duration;

        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);

        // Non-anchor zoom-in
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(50, 50));

            did_request_redraw_ = false;
            did_request_animate_ = false;
            host_impl_->active_tree()->SetPendingPageScaleAnimation(
                scoped_ptr<PendingPageScaleAnimation>(new PendingPageScaleAnimation(
                    gfx::Vector2d(),
                    false,
                    2.f,
                    duration)));
            host_impl_->ActivateSyncTree();
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_TRUE(did_request_animate_);

            did_request_redraw_ = false;
            did_request_animate_ = false;
            begin_frame_args.frame_time = start_time;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_redraw_);
            EXPECT_TRUE(did_request_animate_);
            host_impl_->DidFinishImplFrame();

            did_request_redraw_ = false;
            did_request_animate_ = false;
            begin_frame_args.frame_time = halfway_through_animation;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_redraw_);
            EXPECT_TRUE(did_request_animate_);
            host_impl_->DidFinishImplFrame();

            did_request_redraw_ = false;
            did_request_animate_ = false;
            did_request_commit_ = false;
            begin_frame_args.frame_time = end_time;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_commit_);
            EXPECT_FALSE(did_request_animate_);
            host_impl_->DidFinishImplFrame();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, 2);
            EXPECT_TRUE(ScrollInfoContains(*scroll_info, scroll_layer->id(),
                gfx::Vector2d(-50, -50)));
        }

        start_time += base::TimeDelta::FromSeconds(10);
        halfway_through_animation += base::TimeDelta::FromSeconds(10);
        end_time += base::TimeDelta::FromSeconds(10);

        // Anchor zoom-out
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(50, 50));

            did_request_redraw_ = false;
            did_request_animate_ = false;
            host_impl_->active_tree()->SetPendingPageScaleAnimation(
                scoped_ptr<PendingPageScaleAnimation>(new PendingPageScaleAnimation(
                    gfx::Vector2d(25, 25),
                    true,
                    min_page_scale,
                    duration)));
            host_impl_->ActivateSyncTree();
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_TRUE(did_request_animate_);

            did_request_redraw_ = false;
            did_request_animate_ = false;
            begin_frame_args.frame_time = start_time;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_redraw_);
            EXPECT_TRUE(did_request_animate_);
            host_impl_->DidFinishImplFrame();

            did_request_redraw_ = false;
            did_request_commit_ = false;
            did_request_animate_ = false;
            begin_frame_args.frame_time = end_time;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_redraw_);
            EXPECT_FALSE(did_request_animate_);
            EXPECT_TRUE(did_request_commit_);
            host_impl_->DidFinishImplFrame();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, min_page_scale);
            // Pushed to (0,0) via clamping against contents layer size.
            EXPECT_TRUE(ScrollInfoContains(*scroll_info, scroll_layer->id(),
                gfx::Vector2d(-50, -50)));
        }
    }

    TEST_F(LayerTreeHostImplTest, PageScaleAnimationNoOp)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        LayerImpl* scroll_layer = host_impl_->InnerViewportScrollLayer();
        DCHECK(scroll_layer);

        float min_page_scale = 0.5f;
        float max_page_scale = 4.f;
        base::TimeTicks start_time = base::TimeTicks() + base::TimeDelta::FromSeconds(1);
        base::TimeDelta duration = base::TimeDelta::FromMilliseconds(100);
        base::TimeTicks halfway_through_animation = start_time + duration / 2;
        base::TimeTicks end_time = start_time + duration;

        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);

        // Anchor zoom with unchanged page scale should not change scroll or scale.
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
                max_page_scale);
            scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(50, 50));

            host_impl_->active_tree()->SetPendingPageScaleAnimation(
                scoped_ptr<PendingPageScaleAnimation>(new PendingPageScaleAnimation(
                    gfx::Vector2d(),
                    true,
                    1.f,
                    duration)));
            host_impl_->ActivateSyncTree();
            begin_frame_args.frame_time = start_time;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            host_impl_->DidFinishImplFrame();

            begin_frame_args.frame_time = halfway_through_animation;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_redraw_);
            host_impl_->DidFinishImplFrame();

            begin_frame_args.frame_time = end_time;
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_commit_);
            host_impl_->DidFinishImplFrame();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_EQ(scroll_info->page_scale_delta, 1);
            ExpectNone(*scroll_info, scroll_layer->id());
        }
    }

    TEST_F(LayerTreeHostImplTest, PageScaleAnimationTransferedOnSyncTreeActivate)
    {
        host_impl_->CreatePendingTree();
        host_impl_->pending_tree()->PushPageScaleFromMainThread(1.f, 1.f, 1.f);
        CreateScrollAndContentsLayers(
            host_impl_->pending_tree(),
            gfx::Size(100, 100));
        host_impl_->ActivateSyncTree();
        DrawFrame();

        LayerImpl* scroll_layer = host_impl_->InnerViewportScrollLayer();
        DCHECK(scroll_layer);

        float min_page_scale = 0.5f;
        float max_page_scale = 4.f;
        host_impl_->sync_tree()->PushPageScaleFromMainThread(1.f, min_page_scale,
            max_page_scale);
        host_impl_->ActivateSyncTree();

        base::TimeTicks start_time = base::TimeTicks() + base::TimeDelta::FromSeconds(1);
        base::TimeDelta duration = base::TimeDelta::FromMilliseconds(100);
        base::TimeTicks third_through_animation = start_time + duration / 3;
        base::TimeTicks halfway_through_animation = start_time + duration / 2;
        base::TimeTicks end_time = start_time + duration;
        float target_scale = 2.f;

        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);

        scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(50, 50));

        // Make sure TakePageScaleAnimation works properly.

        host_impl_->sync_tree()->SetPendingPageScaleAnimation(
            scoped_ptr<PendingPageScaleAnimation>(new PendingPageScaleAnimation(
                gfx::Vector2d(),
                false,
                target_scale,
                duration)));
        scoped_ptr<PendingPageScaleAnimation> psa = host_impl_->sync_tree()->TakePendingPageScaleAnimation();
        EXPECT_EQ(target_scale, psa->scale);
        EXPECT_EQ(duration, psa->duration);
        EXPECT_EQ(nullptr, host_impl_->sync_tree()->TakePendingPageScaleAnimation());

        // Recreate the PSA. Nothing should happen here since the tree containing the
        // PSA hasn't been activated yet.
        did_request_redraw_ = false;
        did_request_animate_ = false;
        host_impl_->sync_tree()->SetPendingPageScaleAnimation(
            scoped_ptr<PendingPageScaleAnimation>(new PendingPageScaleAnimation(
                gfx::Vector2d(),
                false,
                target_scale,
                duration)));
        begin_frame_args.frame_time = halfway_through_animation;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_FALSE(did_request_animate_);
        EXPECT_FALSE(did_request_redraw_);
        host_impl_->DidFinishImplFrame();

        // Activate the sync tree. This should cause the animation to become enabled.
        // It should also clear the pointer on the sync tree.
        host_impl_->ActivateSyncTree();
        EXPECT_EQ(nullptr,
            host_impl_->sync_tree()->TakePendingPageScaleAnimation().get());
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_TRUE(did_request_animate_);

        start_time += base::TimeDelta::FromSeconds(10);
        third_through_animation += base::TimeDelta::FromSeconds(10);
        halfway_through_animation += base::TimeDelta::FromSeconds(10);
        end_time += base::TimeDelta::FromSeconds(10);

        // From here on, make sure the animation runs as normal.
        did_request_redraw_ = false;
        did_request_animate_ = false;
        begin_frame_args.frame_time = start_time;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_TRUE(did_request_animate_);
        host_impl_->DidFinishImplFrame();

        did_request_redraw_ = false;
        did_request_animate_ = false;
        begin_frame_args.frame_time = third_through_animation;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_TRUE(did_request_animate_);
        host_impl_->DidFinishImplFrame();

        // Another activation shouldn't have any effect on the animation.
        host_impl_->ActivateSyncTree();

        did_request_redraw_ = false;
        did_request_animate_ = false;
        begin_frame_args.frame_time = halfway_through_animation;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_TRUE(did_request_animate_);
        host_impl_->DidFinishImplFrame();

        did_request_redraw_ = false;
        did_request_animate_ = false;
        did_request_commit_ = false;
        begin_frame_args.frame_time = end_time;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_TRUE(did_request_commit_);
        EXPECT_FALSE(did_request_animate_);
        host_impl_->DidFinishImplFrame();

        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_EQ(scroll_info->page_scale_delta, target_scale);
        EXPECT_TRUE(ScrollInfoContains(*scroll_info, scroll_layer->id(),
            gfx::Vector2d(-50, -50)));
    }

    TEST_F(LayerTreeHostImplTest, PageScaleAnimationCompletedNotification)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        LayerImpl* scroll_layer = host_impl_->InnerViewportScrollLayer();
        DCHECK(scroll_layer);

        base::TimeTicks start_time = base::TimeTicks() + base::TimeDelta::FromSeconds(1);
        base::TimeDelta duration = base::TimeDelta::FromMilliseconds(100);
        base::TimeTicks halfway_through_animation = start_time + duration / 2;
        base::TimeTicks end_time = start_time + duration;

        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);

        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.5f, 4.f);
        scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(50, 50));

        did_complete_page_scale_animation_ = false;
        host_impl_->active_tree()->SetPendingPageScaleAnimation(
            scoped_ptr<PendingPageScaleAnimation>(new PendingPageScaleAnimation(
                gfx::Vector2d(), false, 2.f, duration)));
        host_impl_->ActivateSyncTree();
        begin_frame_args.frame_time = start_time;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_FALSE(did_complete_page_scale_animation_);
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = halfway_through_animation;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_FALSE(did_complete_page_scale_animation_);
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = end_time;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_TRUE(did_complete_page_scale_animation_);
        host_impl_->DidFinishImplFrame();
    }

    class LayerTreeHostImplOverridePhysicalTime : public LayerTreeHostImpl {
    public:
        LayerTreeHostImplOverridePhysicalTime(
            const LayerTreeSettings& settings,
            LayerTreeHostImplClient* client,
            Proxy* proxy,
            SharedBitmapManager* manager,
            TaskGraphRunner* task_graph_runner,
            RenderingStatsInstrumentation* rendering_stats_instrumentation)
            : LayerTreeHostImpl(settings,
                client,
                proxy,
                rendering_stats_instrumentation,
                manager,
                nullptr,
                task_graph_runner,
                0)
        {
        }

        BeginFrameArgs CurrentBeginFrameArgs() const override
        {
            return CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE,
                fake_current_physical_time_);
        }

        void SetCurrentPhysicalTimeTicksForTest(base::TimeTicks fake_now)
        {
            fake_current_physical_time_ = fake_now;
        }

    private:
        base::TimeTicks fake_current_physical_time_;
    };

    class LayerTreeHostImplTestScrollbarAnimation : public LayerTreeHostImplTest {
    protected:
        void SetupLayers(LayerTreeSettings settings)
        {
            gfx::Size content_size(100, 100);

            LayerTreeHostImplOverridePhysicalTime* host_impl_override_time = new LayerTreeHostImplOverridePhysicalTime(
                settings, this, &proxy_, &shared_bitmap_manager_,
                &task_graph_runner_, &stats_instrumentation_);
            host_impl_ = make_scoped_ptr(host_impl_override_time);
            output_surface_ = CreateOutputSurface();
            host_impl_->InitializeRenderer(output_surface_.get());

            SetupScrollAndContentsLayers(content_size);
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 4.f);
            host_impl_->SetViewportSize(
                gfx::Size(content_size.width() / 2, content_size.height() / 2));

            scoped_ptr<SolidColorScrollbarLayerImpl> scrollbar = SolidColorScrollbarLayerImpl::Create(host_impl_->active_tree(), 400,
                VERTICAL, 10, 0, false, true);
            EXPECT_FLOAT_EQ(0.f, scrollbar->opacity());

            LayerImpl* scroll = host_impl_->InnerViewportScrollLayer();
            LayerImpl* root = scroll->parent()->parent();
            scrollbar->SetScrollLayerAndClipLayerByIds(scroll->id(), root->id());
            root->AddChild(scrollbar.Pass());

            host_impl_->active_tree()->DidBecomeActive();
            DrawFrame();
        }

        void RunTest(LayerTreeSettings::ScrollbarAnimator animator)
        {
            LayerTreeSettings settings;
            settings.scrollbar_animator = animator;
            settings.scrollbar_fade_delay_ms = 20;
            settings.scrollbar_fade_duration_ms = 20;

            SetupLayers(settings);

            base::TimeTicks fake_now = base::TimeTicks::Now();

            EXPECT_FALSE(did_request_animate_);
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_EQ(base::TimeDelta(), requested_animation_delay_);
            EXPECT_TRUE(animation_task_.Equals(base::Closure()));

            // If no scroll happened during a scroll gesture, it should have no effect.
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL);
            host_impl_->ScrollEnd();
            EXPECT_FALSE(did_request_animate_);
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_EQ(base::TimeDelta(), requested_animation_delay_);
            EXPECT_TRUE(animation_task_.Equals(base::Closure()));

            // After a scroll, a scrollbar animation should be scheduled about 20ms from
            // now.
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL);
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0, 5));
            EXPECT_FALSE(did_request_animate_);
            EXPECT_TRUE(did_request_redraw_);
            did_request_redraw_ = false;
            EXPECT_EQ(base::TimeDelta(), requested_animation_delay_);
            EXPECT_TRUE(animation_task_.Equals(base::Closure()));

            host_impl_->ScrollEnd();
            EXPECT_FALSE(did_request_animate_);
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_EQ(base::TimeDelta::FromMilliseconds(20),
                requested_animation_delay_);
            EXPECT_FALSE(animation_task_.Equals(base::Closure()));

            fake_now += requested_animation_delay_;
            requested_animation_delay_ = base::TimeDelta();
            animation_task_.Run();
            animation_task_ = base::Closure();
            EXPECT_TRUE(did_request_animate_);
            did_request_animate_ = false;
            EXPECT_FALSE(did_request_redraw_);

            // After the scrollbar animation begins, we should start getting redraws.
            BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, fake_now);
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_TRUE(did_request_animate_);
            did_request_animate_ = false;
            EXPECT_TRUE(did_request_redraw_);
            did_request_redraw_ = false;
            EXPECT_EQ(base::TimeDelta(), requested_animation_delay_);
            EXPECT_TRUE(animation_task_.Equals(base::Closure()));
            host_impl_->DidFinishImplFrame();

            // Setting the scroll offset outside a scroll should also cause the
            // scrollbar to appear and to schedule a scrollbar animation.
            host_impl_->InnerViewportScrollLayer()->PushScrollOffsetFromMainThread(
                gfx::ScrollOffset(5, 5));
            EXPECT_FALSE(did_request_animate_);
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_EQ(base::TimeDelta::FromMilliseconds(20),
                requested_animation_delay_);
            EXPECT_FALSE(animation_task_.Equals(base::Closure()));
            requested_animation_delay_ = base::TimeDelta();
            animation_task_ = base::Closure();

            // Scrollbar animation is not triggered unnecessarily.
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL);
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(5, 0));
            EXPECT_FALSE(did_request_animate_);
            EXPECT_TRUE(did_request_redraw_);
            did_request_redraw_ = false;
            EXPECT_EQ(base::TimeDelta(), requested_animation_delay_);
            EXPECT_TRUE(animation_task_.Equals(base::Closure()));

            host_impl_->ScrollEnd();
            EXPECT_FALSE(did_request_animate_);
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_EQ(base::TimeDelta(), requested_animation_delay_);
            EXPECT_TRUE(animation_task_.Equals(base::Closure()));

            // Changing page scale triggers scrollbar animation.
            host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 4.f);
            host_impl_->active_tree()->SetPageScaleOnActiveTree(1.1f);
            EXPECT_FALSE(did_request_animate_);
            EXPECT_FALSE(did_request_redraw_);
            EXPECT_EQ(base::TimeDelta::FromMilliseconds(20),
                requested_animation_delay_);
            EXPECT_FALSE(animation_task_.Equals(base::Closure()));
            requested_animation_delay_ = base::TimeDelta();
            animation_task_ = base::Closure();
        }
    };

    TEST_F(LayerTreeHostImplTestScrollbarAnimation, LinearFade)
    {
        RunTest(LayerTreeSettings::LINEAR_FADE);
    }

    TEST_F(LayerTreeHostImplTestScrollbarAnimation, Thinning)
    {
        RunTest(LayerTreeSettings::THINNING);
    }

    class LayerTreeHostImplTestScrollbarOpacity : public LayerTreeHostImplTest {
    protected:
        void RunTest(LayerTreeSettings::ScrollbarAnimator animator)
        {
            LayerTreeSettings settings;
            settings.scrollbar_animator = animator;
            settings.scrollbar_fade_delay_ms = 20;
            settings.scrollbar_fade_duration_ms = 20;
            settings.verify_property_trees = true;
            gfx::Size content_size(100, 100);

            CreateHostImpl(settings, CreateOutputSurface());
            host_impl_->CreatePendingTree();
            CreateScrollAndContentsLayers(host_impl_->pending_tree(), content_size);
            scoped_ptr<SolidColorScrollbarLayerImpl> scrollbar = SolidColorScrollbarLayerImpl::Create(host_impl_->pending_tree(), 400,
                VERTICAL, 10, 0, false, true);
            LayerImpl* scroll = host_impl_->pending_tree()->InnerViewportScrollLayer();
            LayerImpl* root = scroll->parent()->parent();
            scrollbar->SetScrollLayerAndClipLayerByIds(scroll->id(), root->id());
            root->AddChild(scrollbar.Pass());
            host_impl_->pending_tree()->PushPageScaleFromMainThread(1.f, 1.f, 1.f);
            host_impl_->pending_tree()->BuildPropertyTreesForTesting();
            host_impl_->ActivateSyncTree();

            LayerImpl* scrollbar_layer = host_impl_->active_tree()->LayerById(400);

            EffectNode* active_tree_node = host_impl_->active_tree()->property_trees()->effect_tree.Node(
                scrollbar_layer->effect_tree_index());
            EXPECT_FLOAT_EQ(scrollbar_layer->opacity(), active_tree_node->data.opacity);

            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL);
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0, 5));
            host_impl_->ScrollEnd();
            host_impl_->CreatePendingTree();
            EffectNode* pending_tree_node = host_impl_->pending_tree()->property_trees()->effect_tree.Node(
                scrollbar_layer->effect_tree_index());
            EXPECT_FLOAT_EQ(1.f, active_tree_node->data.opacity);
            EXPECT_FLOAT_EQ(1.f, scrollbar_layer->opacity());
            EXPECT_FLOAT_EQ(0.f, pending_tree_node->data.opacity);
            host_impl_->ActivateSyncTree();
            active_tree_node = host_impl_->active_tree()->property_trees()->effect_tree.Node(
                scrollbar_layer->effect_tree_index());
            EXPECT_FLOAT_EQ(1.f, active_tree_node->data.opacity);
            EXPECT_FLOAT_EQ(1.f, scrollbar_layer->opacity());
        }
    };

    TEST_F(LayerTreeHostImplTestScrollbarOpacity, LinearFade)
    {
        RunTest(LayerTreeSettings::LINEAR_FADE);
    }

    TEST_F(LayerTreeHostImplTestScrollbarOpacity, Thinning)
    {
        RunTest(LayerTreeSettings::THINNING);
    }

    void LayerTreeHostImplTest::SetupMouseMoveAtWithDeviceScale(
        float device_scale_factor)
    {
        LayerTreeSettings settings;
        settings.scrollbar_fade_delay_ms = 500;
        settings.scrollbar_fade_duration_ms = 300;
        settings.scrollbar_animator = LayerTreeSettings::THINNING;

        gfx::Size viewport_size(300, 200);
        gfx::Size device_viewport_size = gfx::ScaleToFlooredSize(viewport_size, device_scale_factor);
        gfx::Size content_size(1000, 1000);

        CreateHostImpl(settings, CreateOutputSurface());
        host_impl_->active_tree()->SetDeviceScaleFactor(device_scale_factor);
        host_impl_->SetViewportSize(device_viewport_size);

        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetBounds(viewport_size);
        root->SetHasRenderSurface(true);

        scoped_ptr<LayerImpl> scroll = LayerImpl::Create(host_impl_->active_tree(), 2);
        scroll->SetScrollClipLayer(root->id());
        scroll->PushScrollOffsetFromMainThread(gfx::ScrollOffset());
        scroll->SetBounds(content_size);
        scroll->SetIsContainerForFixedPositionLayers(true);

        scoped_ptr<LayerImpl> contents = LayerImpl::Create(host_impl_->active_tree(), 3);
        contents->SetDrawsContent(true);
        contents->SetBounds(content_size);

        // The scrollbar is on the right side.
        scoped_ptr<PaintedScrollbarLayerImpl> scrollbar = PaintedScrollbarLayerImpl::Create(host_impl_->active_tree(), 5, VERTICAL);
        scrollbar->SetDrawsContent(true);
        scrollbar->SetBounds(gfx::Size(15, viewport_size.height()));
        scrollbar->SetPosition(gfx::Point(285, 0));

        scroll->AddChild(contents.Pass());
        root->AddChild(scroll.Pass());
        scrollbar->SetScrollLayerAndClipLayerByIds(2, 1);
        root->AddChild(scrollbar.Pass());

        host_impl_->active_tree()->SetRootLayer(root.Pass());
        host_impl_->active_tree()->SetViewportLayersFromIds(Layer::INVALID_ID, 1, 2,
            Layer::INVALID_ID);
        host_impl_->active_tree()->DidBecomeActive();
        DrawFrame();

        LayerImpl* root_scroll = host_impl_->active_tree()->InnerViewportScrollLayer();
        ASSERT_TRUE(root_scroll->scrollbar_animation_controller());
        ScrollbarAnimationControllerThinning* scrollbar_animation_controller = static_cast<ScrollbarAnimationControllerThinning*>(
            root_scroll->scrollbar_animation_controller());
        scrollbar_animation_controller->set_mouse_move_distance_for_test(100.f);

        host_impl_->MouseMoveAt(gfx::Point(1, 1));
        EXPECT_FALSE(scrollbar_animation_controller->mouse_is_near_scrollbar());

        host_impl_->MouseMoveAt(gfx::Point(200, 50));
        EXPECT_TRUE(scrollbar_animation_controller->mouse_is_near_scrollbar());

        host_impl_->MouseMoveAt(gfx::Point(184, 100));
        EXPECT_FALSE(scrollbar_animation_controller->mouse_is_near_scrollbar());

        scrollbar_animation_controller->set_mouse_move_distance_for_test(102.f);
        host_impl_->MouseMoveAt(gfx::Point(184, 100));
        EXPECT_TRUE(scrollbar_animation_controller->mouse_is_near_scrollbar());

        did_request_redraw_ = false;
        EXPECT_EQ(0, host_impl_->scroll_layer_id_when_mouse_over_scrollbar());
        host_impl_->MouseMoveAt(gfx::Point(290, 100));
        EXPECT_EQ(2, host_impl_->scroll_layer_id_when_mouse_over_scrollbar());
        host_impl_->MouseMoveAt(gfx::Point(290, 120));
        EXPECT_EQ(2, host_impl_->scroll_layer_id_when_mouse_over_scrollbar());
        host_impl_->MouseMoveAt(gfx::Point(150, 120));
        EXPECT_EQ(0, host_impl_->scroll_layer_id_when_mouse_over_scrollbar());
    }

    TEST_F(LayerTreeHostImplTest, MouseMoveAtWithDeviceScaleOf1)
    {
        SetupMouseMoveAtWithDeviceScale(1.f);
    }

    TEST_F(LayerTreeHostImplTest, MouseMoveAtWithDeviceScaleOf2)
    {
        SetupMouseMoveAtWithDeviceScale(2.f);
    }

    TEST_F(LayerTreeHostImplTest, CompositorFrameMetadata)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.5f, 4.f);
        DrawFrame();
        {
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_EQ(gfx::Vector2dF(), metadata.root_scroll_offset);
            EXPECT_EQ(1.f, metadata.page_scale_factor);
            EXPECT_EQ(gfx::SizeF(50.f, 50.f), metadata.scrollable_viewport_size);
            EXPECT_EQ(gfx::SizeF(100.f, 100.f), metadata.root_layer_size);
            EXPECT_EQ(0.5f, metadata.min_page_scale_factor);
            EXPECT_EQ(4.f, metadata.max_page_scale_factor);
            EXPECT_FALSE(metadata.root_overflow_x_hidden);
            EXPECT_FALSE(metadata.root_overflow_y_hidden);
        }

        // Scrolling should update metadata immediately.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        {
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_EQ(gfx::Vector2dF(0.f, 10.f), metadata.root_scroll_offset);
        }
        host_impl_->ScrollEnd();
        {
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_EQ(gfx::Vector2dF(0.f, 10.f), metadata.root_scroll_offset);
        }

        // Root "overflow: hidden" properties should be reflected on the outer
        // viewport scroll layer.
        {
            host_impl_->active_tree()
                ->OuterViewportScrollLayer()
                ->set_user_scrollable_horizontal(false);
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_TRUE(metadata.root_overflow_x_hidden);
            EXPECT_FALSE(metadata.root_overflow_y_hidden);

            host_impl_->active_tree()
                ->OuterViewportScrollLayer()
                ->set_user_scrollable_vertical(false);
            metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_TRUE(metadata.root_overflow_x_hidden);
            EXPECT_TRUE(metadata.root_overflow_y_hidden);
        }

        // Re-enable scrollability and verify that overflows are no longer hidden.
        {
            host_impl_->active_tree()
                ->OuterViewportScrollLayer()
                ->set_user_scrollable_horizontal(true);
            host_impl_->active_tree()
                ->OuterViewportScrollLayer()
                ->set_user_scrollable_vertical(true);
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_FALSE(metadata.root_overflow_x_hidden);
            EXPECT_FALSE(metadata.root_overflow_y_hidden);
        }

        // Root "overflow: hidden" properties should also be reflected on the
        // inner viewport scroll layer.
        {
            host_impl_->active_tree()
                ->InnerViewportScrollLayer()
                ->set_user_scrollable_horizontal(false);
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_TRUE(metadata.root_overflow_x_hidden);
            EXPECT_FALSE(metadata.root_overflow_y_hidden);

            host_impl_->active_tree()
                ->InnerViewportScrollLayer()
                ->set_user_scrollable_vertical(false);
            metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_TRUE(metadata.root_overflow_x_hidden);
            EXPECT_TRUE(metadata.root_overflow_y_hidden);
        }

        // Page scale should update metadata correctly (shrinking only the viewport).
        host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(2.f, gfx::Point());
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();
        {
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_EQ(gfx::Vector2dF(0.f, 10.f), metadata.root_scroll_offset);
            EXPECT_EQ(2.f, metadata.page_scale_factor);
            EXPECT_EQ(gfx::SizeF(25.f, 25.f), metadata.scrollable_viewport_size);
            EXPECT_EQ(gfx::SizeF(100.f, 100.f), metadata.root_layer_size);
            EXPECT_EQ(0.5f, metadata.min_page_scale_factor);
            EXPECT_EQ(4.f, metadata.max_page_scale_factor);
        }

        // Likewise if set from the main thread.
        host_impl_->ProcessScrollDeltas();
        host_impl_->active_tree()->PushPageScaleFromMainThread(4.f, 0.5f, 4.f);
        host_impl_->active_tree()->SetPageScaleOnActiveTree(4.f);
        {
            CompositorFrameMetadata metadata = host_impl_->MakeCompositorFrameMetadata();
            EXPECT_EQ(gfx::Vector2dF(0.f, 10.f), metadata.root_scroll_offset);
            EXPECT_EQ(4.f, metadata.page_scale_factor);
            EXPECT_EQ(gfx::SizeF(12.5f, 12.5f), metadata.scrollable_viewport_size);
            EXPECT_EQ(gfx::SizeF(100.f, 100.f), metadata.root_layer_size);
            EXPECT_EQ(0.5f, metadata.min_page_scale_factor);
            EXPECT_EQ(4.f, metadata.max_page_scale_factor);
        }
    }

    class DidDrawCheckLayer : public LayerImpl {
    public:
        static scoped_ptr<LayerImpl> Create(LayerTreeImpl* tree_impl, int id)
        {
            return make_scoped_ptr(new DidDrawCheckLayer(tree_impl, id));
        }

        bool WillDraw(DrawMode draw_mode, ResourceProvider* provider) override
        {
            will_draw_called_ = true;
            if (will_draw_returns_false_)
                return false;
            return LayerImpl::WillDraw(draw_mode, provider);
        }

        void AppendQuads(RenderPass* render_pass,
            AppendQuadsData* append_quads_data) override
        {
            append_quads_called_ = true;
            LayerImpl::AppendQuads(render_pass, append_quads_data);
        }

        void DidDraw(ResourceProvider* provider) override
        {
            did_draw_called_ = true;
            LayerImpl::DidDraw(provider);
        }

        bool will_draw_called() const { return will_draw_called_; }
        bool append_quads_called() const { return append_quads_called_; }
        bool did_draw_called() const { return did_draw_called_; }

        void set_will_draw_returns_false() { will_draw_returns_false_ = true; }

        void ClearDidDrawCheck()
        {
            will_draw_called_ = false;
            append_quads_called_ = false;
            did_draw_called_ = false;
        }

        static void IgnoreResult(scoped_ptr<CopyOutputResult> result) { }

        void AddCopyRequest()
        {
            ScopedPtrVector<CopyOutputRequest> requests;
            requests.push_back(
                CopyOutputRequest::CreateRequest(base::Bind(&IgnoreResult)));
            SetHasRenderSurface(true);
            PassCopyRequests(&requests);
        }

    protected:
        DidDrawCheckLayer(LayerTreeImpl* tree_impl, int id)
            : LayerImpl(tree_impl, id)
            , will_draw_returns_false_(false)
            , will_draw_called_(false)
            , append_quads_called_(false)
            , did_draw_called_(false)
        {
            SetBounds(gfx::Size(10, 10));
            SetDrawsContent(true);
            draw_properties().visible_layer_rect = gfx::Rect(0, 0, 10, 10);
        }

    private:
        bool will_draw_returns_false_;
        bool will_draw_called_;
        bool append_quads_called_;
        bool did_draw_called_;
    };

    TEST_F(LayerTreeHostImplTest, WillDrawReturningFalseDoesNotCall)
    {
        // The root layer is always drawn, so run this test on a child layer that
        // will be masked out by the root layer's bounds.
        host_impl_->active_tree()->SetRootLayer(
            DidDrawCheckLayer::Create(host_impl_->active_tree(), 1));
        DidDrawCheckLayer* root = static_cast<DidDrawCheckLayer*>(
            host_impl_->active_tree()->root_layer());

        root->AddChild(DidDrawCheckLayer::Create(host_impl_->active_tree(), 2));
        root->SetHasRenderSurface(true);
        DidDrawCheckLayer* layer = static_cast<DidDrawCheckLayer*>(root->children()[0]);

        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);

            EXPECT_TRUE(layer->will_draw_called());
            EXPECT_TRUE(layer->append_quads_called());
            EXPECT_TRUE(layer->did_draw_called());
        }

        host_impl_->SetViewportDamage(gfx::Rect(10, 10));

        {
            LayerTreeHostImpl::FrameData frame;

            layer->set_will_draw_returns_false();
            layer->ClearDidDrawCheck();

            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);

            EXPECT_TRUE(layer->will_draw_called());
            EXPECT_FALSE(layer->append_quads_called());
            EXPECT_FALSE(layer->did_draw_called());
        }
    }

    TEST_F(LayerTreeHostImplTest, DidDrawNotCalledOnHiddenLayer)
    {
        // The root layer is always drawn, so run this test on a child layer that
        // will be masked out by the root layer's bounds.
        host_impl_->active_tree()->SetRootLayer(
            DidDrawCheckLayer::Create(host_impl_->active_tree(), 1));
        DidDrawCheckLayer* root = static_cast<DidDrawCheckLayer*>(
            host_impl_->active_tree()->root_layer());
        root->SetMasksToBounds(true);
        root->SetHasRenderSurface(true);
        root->AddChild(DidDrawCheckLayer::Create(host_impl_->active_tree(), 2));
        DidDrawCheckLayer* layer = static_cast<DidDrawCheckLayer*>(root->children()[0]);
        // Ensure visible_layer_rect for layer is empty.
        layer->SetPosition(gfx::PointF(100.f, 100.f));
        layer->SetBounds(gfx::Size(10, 10));

        LayerTreeHostImpl::FrameData frame;

        EXPECT_FALSE(layer->will_draw_called());
        EXPECT_FALSE(layer->did_draw_called());

        host_impl_->active_tree()->BuildPropertyTreesForTesting();
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);

        EXPECT_FALSE(layer->will_draw_called());
        EXPECT_FALSE(layer->did_draw_called());

        EXPECT_TRUE(layer->visible_layer_rect().IsEmpty());

        // Ensure visible_layer_rect for layer is not empty
        layer->SetPosition(gfx::PointF());

        EXPECT_FALSE(layer->will_draw_called());
        EXPECT_FALSE(layer->did_draw_called());

        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);

        EXPECT_TRUE(layer->will_draw_called());
        EXPECT_TRUE(layer->did_draw_called());

        EXPECT_FALSE(layer->visible_layer_rect().IsEmpty());
    }

    TEST_F(LayerTreeHostImplTest, WillDrawNotCalledOnOccludedLayer)
    {
        gfx::Size big_size(1000, 1000);
        host_impl_->SetViewportSize(big_size);

        host_impl_->active_tree()->SetRootLayer(
            DidDrawCheckLayer::Create(host_impl_->active_tree(), 1));
        DidDrawCheckLayer* root = static_cast<DidDrawCheckLayer*>(host_impl_->active_tree()->root_layer());

        root->AddChild(DidDrawCheckLayer::Create(host_impl_->active_tree(), 2));
        DidDrawCheckLayer* occluded_layer = static_cast<DidDrawCheckLayer*>(root->children()[0]);

        root->AddChild(DidDrawCheckLayer::Create(host_impl_->active_tree(), 3));
        root->SetHasRenderSurface(true);
        DidDrawCheckLayer* top_layer = static_cast<DidDrawCheckLayer*>(root->children()[1]);
        // This layer covers the occluded_layer above. Make this layer large so it can
        // occlude.
        top_layer->SetBounds(big_size);
        top_layer->SetContentsOpaque(true);

        LayerTreeHostImpl::FrameData frame;

        EXPECT_FALSE(occluded_layer->will_draw_called());
        EXPECT_FALSE(occluded_layer->did_draw_called());
        EXPECT_FALSE(top_layer->will_draw_called());
        EXPECT_FALSE(top_layer->did_draw_called());

        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);

        EXPECT_FALSE(occluded_layer->will_draw_called());
        EXPECT_FALSE(occluded_layer->did_draw_called());
        EXPECT_TRUE(top_layer->will_draw_called());
        EXPECT_TRUE(top_layer->did_draw_called());
    }

    TEST_F(LayerTreeHostImplTest, DidDrawCalledOnAllLayers)
    {
        host_impl_->active_tree()->SetRootLayer(
            DidDrawCheckLayer::Create(host_impl_->active_tree(), 1));
        DidDrawCheckLayer* root = static_cast<DidDrawCheckLayer*>(host_impl_->active_tree()->root_layer());

        root->AddChild(DidDrawCheckLayer::Create(host_impl_->active_tree(), 2));
        root->SetHasRenderSurface(true);
        DidDrawCheckLayer* layer1 = static_cast<DidDrawCheckLayer*>(root->children()[0]);

        layer1->AddChild(DidDrawCheckLayer::Create(host_impl_->active_tree(), 3));
        DidDrawCheckLayer* layer2 = static_cast<DidDrawCheckLayer*>(layer1->children()[0]);

        layer1->SetHasRenderSurface(true);
        layer1->SetShouldFlattenTransform(true);

        EXPECT_FALSE(root->did_draw_called());
        EXPECT_FALSE(layer1->did_draw_called());
        EXPECT_FALSE(layer2->did_draw_called());

        LayerTreeHostImpl::FrameData frame;
        FakeLayerTreeHostImpl::RecursiveUpdateNumChildren(
            host_impl_->active_tree()->root_layer());
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);

        EXPECT_TRUE(root->did_draw_called());
        EXPECT_TRUE(layer1->did_draw_called());
        EXPECT_TRUE(layer2->did_draw_called());

        EXPECT_NE(root->render_surface(), layer1->render_surface());
        EXPECT_TRUE(layer1->render_surface());
    }

    class MissingTextureAnimatingLayer : public DidDrawCheckLayer {
    public:
        static scoped_ptr<LayerImpl> Create(LayerTreeImpl* tree_impl,
            int id,
            bool tile_missing,
            bool had_incomplete_tile,
            bool animating,
            ResourceProvider* resource_provider)
        {
            return make_scoped_ptr(new MissingTextureAnimatingLayer(tree_impl,
                id,
                tile_missing,
                had_incomplete_tile,
                animating,
                resource_provider));
        }

        void AppendQuads(RenderPass* render_pass,
            AppendQuadsData* append_quads_data) override
        {
            LayerImpl::AppendQuads(render_pass, append_quads_data);
            if (had_incomplete_tile_)
                append_quads_data->num_incomplete_tiles++;
            if (tile_missing_)
                append_quads_data->num_missing_tiles++;
        }

    private:
        MissingTextureAnimatingLayer(LayerTreeImpl* tree_impl,
            int id,
            bool tile_missing,
            bool had_incomplete_tile,
            bool animating,
            ResourceProvider* resource_provider)
            : DidDrawCheckLayer(tree_impl, id)
            , tile_missing_(tile_missing)
            , had_incomplete_tile_(had_incomplete_tile)
        {
            if (animating)
                AddAnimatedTransformToLayer(this, 10.0, 3, 0);
        }

        bool tile_missing_;
        bool had_incomplete_tile_;
    };

    struct PrepareToDrawSuccessTestCase {
        struct State {
            bool has_missing_tile = false;
            bool has_incomplete_tile = false;
            bool is_animating = false;
            bool has_copy_request = false;
        };
        bool high_res_required = false;
        State layer_before;
        State layer_between;
        State layer_after;
        DrawResult expected_result;

        explicit PrepareToDrawSuccessTestCase(DrawResult result)
            : expected_result(result)
        {
        }
    };

    TEST_F(LayerTreeHostImplTest, PrepareToDrawSucceedsAndFails)
    {
        std::vector<PrepareToDrawSuccessTestCase> cases;

        // 0. Default case.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        // 1. Animated layer first.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_before.is_animating = true;
        // 2. Animated layer between.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_between.is_animating = true;
        // 3. Animated layer last.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_after.is_animating = true;
        // 4. Missing tile first.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_before.has_missing_tile = true;
        // 5. Missing tile between.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_between.has_missing_tile = true;
        // 6. Missing tile last.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_after.has_missing_tile = true;
        // 7. Incomplete tile first.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_before.has_incomplete_tile = true;
        // 8. Incomplete tile between.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_between.has_incomplete_tile = true;
        // 9. Incomplete tile last.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_after.has_incomplete_tile = true;
        // 10. Animation with missing tile.
        cases.push_back(
            PrepareToDrawSuccessTestCase(DRAW_ABORTED_CHECKERBOARD_ANIMATIONS));
        cases.back().layer_between.has_missing_tile = true;
        cases.back().layer_between.is_animating = true;
        // 11. Animation with incomplete tile.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_between.has_incomplete_tile = true;
        cases.back().layer_between.is_animating = true;

        // 12. High res required.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().high_res_required = true;
        // 13. High res required with incomplete tile.
        cases.push_back(
            PrepareToDrawSuccessTestCase(DRAW_ABORTED_MISSING_HIGH_RES_CONTENT));
        cases.back().high_res_required = true;
        cases.back().layer_between.has_incomplete_tile = true;
        // 14. High res required with missing tile.
        cases.push_back(
            PrepareToDrawSuccessTestCase(DRAW_ABORTED_MISSING_HIGH_RES_CONTENT));
        cases.back().high_res_required = true;
        cases.back().layer_between.has_missing_tile = true;

        // 15. High res required is higher priority than animating missing tiles.
        cases.push_back(
            PrepareToDrawSuccessTestCase(DRAW_ABORTED_MISSING_HIGH_RES_CONTENT));
        cases.back().high_res_required = true;
        cases.back().layer_between.has_missing_tile = true;
        cases.back().layer_after.has_missing_tile = true;
        cases.back().layer_after.is_animating = true;
        // 16. High res required is higher priority than animating missing tiles.
        cases.push_back(
            PrepareToDrawSuccessTestCase(DRAW_ABORTED_MISSING_HIGH_RES_CONTENT));
        cases.back().high_res_required = true;
        cases.back().layer_between.has_missing_tile = true;
        cases.back().layer_before.has_missing_tile = true;
        cases.back().layer_before.is_animating = true;

        host_impl_->active_tree()->SetRootLayer(
            DidDrawCheckLayer::Create(host_impl_->active_tree(), 1));
        DidDrawCheckLayer* root = static_cast<DidDrawCheckLayer*>(host_impl_->active_tree()->root_layer());
        root->SetHasRenderSurface(true);

        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        host_impl_->SwapBuffers(frame);

        for (size_t i = 0; i < cases.size(); ++i) {
            const auto& testcase = cases[i];
            std::vector<LayerImpl*> to_remove;
            for (auto* child : root->children())
                to_remove.push_back(child);
            for (auto* child : to_remove)
                root->RemoveChild(child);

            std::ostringstream scope;
            scope << "Test case: " << i;
            SCOPED_TRACE(scope.str());

            root->AddChild(MissingTextureAnimatingLayer::Create(
                host_impl_->active_tree(), 2, testcase.layer_before.has_missing_tile,
                testcase.layer_before.has_incomplete_tile,
                testcase.layer_before.is_animating, host_impl_->resource_provider()));
            DidDrawCheckLayer* before = static_cast<DidDrawCheckLayer*>(root->children().back());
            if (testcase.layer_before.has_copy_request)
                before->AddCopyRequest();

            root->AddChild(MissingTextureAnimatingLayer::Create(
                host_impl_->active_tree(), 3, testcase.layer_between.has_missing_tile,
                testcase.layer_between.has_incomplete_tile,
                testcase.layer_between.is_animating, host_impl_->resource_provider()));
            DidDrawCheckLayer* between = static_cast<DidDrawCheckLayer*>(root->children().back());
            if (testcase.layer_between.has_copy_request)
                between->AddCopyRequest();

            root->AddChild(MissingTextureAnimatingLayer::Create(
                host_impl_->active_tree(), 4, testcase.layer_after.has_missing_tile,
                testcase.layer_after.has_incomplete_tile,
                testcase.layer_after.is_animating, host_impl_->resource_provider()));
            DidDrawCheckLayer* after = static_cast<DidDrawCheckLayer*>(root->children().back());
            if (testcase.layer_after.has_copy_request)
                after->AddCopyRequest();

            if (testcase.high_res_required)
                host_impl_->SetRequiresHighResToDraw();

            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(testcase.expected_result, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
            host_impl_->SwapBuffers(frame);
        }
    }

    TEST_F(LayerTreeHostImplTest,
        PrepareToDrawWhenDrawAndSwapFullViewportEveryFrame)
    {
        CreateHostImpl(DefaultSettings(),
            FakeOutputSurface::CreateAlwaysDrawAndSwap3d());
        EXPECT_TRUE(host_impl_->output_surface()
                        ->capabilities()
                        .draw_and_swap_full_viewport_every_frame);

        std::vector<PrepareToDrawSuccessTestCase> cases;

        // 0. Default case.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        // 1. Animation with missing tile.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().layer_between.has_missing_tile = true;
        cases.back().layer_between.is_animating = true;
        // 2. High res required with incomplete tile.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().high_res_required = true;
        cases.back().layer_between.has_incomplete_tile = true;
        // 3. High res required with missing tile.
        cases.push_back(PrepareToDrawSuccessTestCase(DRAW_SUCCESS));
        cases.back().high_res_required = true;
        cases.back().layer_between.has_missing_tile = true;

        host_impl_->active_tree()->SetRootLayer(
            DidDrawCheckLayer::Create(host_impl_->active_tree(), 1));
        DidDrawCheckLayer* root = static_cast<DidDrawCheckLayer*>(host_impl_->active_tree()->root_layer());
        root->SetHasRenderSurface(true);

        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        host_impl_->SwapBuffers(frame);

        for (size_t i = 0; i < cases.size(); ++i) {
            const auto& testcase = cases[i];
            std::vector<LayerImpl*> to_remove;
            for (auto* child : root->children())
                to_remove.push_back(child);
            for (auto* child : to_remove)
                root->RemoveChild(child);

            std::ostringstream scope;
            scope << "Test case: " << i;
            SCOPED_TRACE(scope.str());

            root->AddChild(MissingTextureAnimatingLayer::Create(
                host_impl_->active_tree(), 2, testcase.layer_before.has_missing_tile,
                testcase.layer_before.has_incomplete_tile,
                testcase.layer_before.is_animating, host_impl_->resource_provider()));
            DidDrawCheckLayer* before = static_cast<DidDrawCheckLayer*>(root->children().back());
            if (testcase.layer_before.has_copy_request)
                before->AddCopyRequest();

            root->AddChild(MissingTextureAnimatingLayer::Create(
                host_impl_->active_tree(), 3, testcase.layer_between.has_missing_tile,
                testcase.layer_between.has_incomplete_tile,
                testcase.layer_between.is_animating, host_impl_->resource_provider()));
            DidDrawCheckLayer* between = static_cast<DidDrawCheckLayer*>(root->children().back());
            if (testcase.layer_between.has_copy_request)
                between->AddCopyRequest();

            root->AddChild(MissingTextureAnimatingLayer::Create(
                host_impl_->active_tree(), 4, testcase.layer_after.has_missing_tile,
                testcase.layer_after.has_incomplete_tile,
                testcase.layer_after.is_animating, host_impl_->resource_provider()));
            DidDrawCheckLayer* after = static_cast<DidDrawCheckLayer*>(root->children().back());
            if (testcase.layer_after.has_copy_request)
                after->AddCopyRequest();

            if (testcase.high_res_required)
                host_impl_->SetRequiresHighResToDraw();

            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(testcase.expected_result, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
            host_impl_->SwapBuffers(frame);
        }
    }

    TEST_F(LayerTreeHostImplTest, ScrollRootIgnored)
    {
        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetScrollClipLayer(Layer::INVALID_ID);
        root->SetHasRenderSurface(true);
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        DrawFrame();

        // Scroll event is ignored because layer is not scrollable.
        EXPECT_EQ(InputHandler::SCROLL_IGNORED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, ClampingAfterActivation)
    {
        host_impl_->CreatePendingTree();
        host_impl_->pending_tree()->PushPageScaleFromMainThread(1.f, 1.f, 1.f);
        CreateScrollAndContentsLayers(host_impl_->pending_tree(),
            gfx::Size(100, 100));
        host_impl_->ActivateSyncTree();

        host_impl_->CreatePendingTree();
        const gfx::ScrollOffset pending_scroll = gfx::ScrollOffset(-100, -100);
        LayerImpl* active_outer_layer = host_impl_->active_tree()->OuterViewportScrollLayer();
        LayerImpl* pending_outer_layer = host_impl_->pending_tree()->OuterViewportScrollLayer();
        pending_outer_layer->PushScrollOffsetFromMainThread(pending_scroll);

        host_impl_->ActivateSyncTree();
        // Scrolloffsets on the active tree will be clamped after activation.
        EXPECT_EQ(active_outer_layer->CurrentScrollOffset(), gfx::ScrollOffset(0, 0));
    }

    class LayerTreeHostImplTopControlsTest : public LayerTreeHostImplTest {
    public:
        LayerTreeHostImplTopControlsTest()
            // Make the clip size the same as the layer (content) size so the layer is
            // non-scrollable.
            : layer_size_(10, 10)
            , clip_size_(layer_size_)
            , top_controls_height_(50)
        {
            viewport_size_ = gfx::Size(clip_size_.width(),
                clip_size_.height() + top_controls_height_);
        }

        bool CreateHostImpl(const LayerTreeSettings& settings,
            scoped_ptr<OutputSurface> output_surface) override
        {
            bool init = LayerTreeHostImplTest::CreateHostImpl(settings, output_surface.Pass());
            if (init) {
                host_impl_->active_tree()->set_top_controls_height(top_controls_height_);
                host_impl_->active_tree()->SetCurrentTopControlsShownRatio(1.f);
                host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 1.f);
            }
            return init;
        }

        void SetupTopControlsAndScrollLayerWithVirtualViewport(
            const gfx::Size& inner_viewport_size,
            const gfx::Size& outer_viewport_size,
            const gfx::Size& scroll_layer_size)
        {
            CreateHostImpl(settings_, CreateOutputSurface());
            host_impl_->sync_tree()->set_top_controls_shrink_blink_size(true);
            host_impl_->sync_tree()->set_top_controls_height(top_controls_height_);
            host_impl_->DidChangeTopControlsPosition();

            scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
            scoped_ptr<LayerImpl> root_clip = LayerImpl::Create(host_impl_->active_tree(), 2);
            scoped_ptr<LayerImpl> page_scale = LayerImpl::Create(host_impl_->active_tree(), 3);

            scoped_ptr<LayerImpl> outer_scroll = LayerImpl::Create(host_impl_->active_tree(), 4);
            scoped_ptr<LayerImpl> outer_clip = LayerImpl::Create(host_impl_->active_tree(), 5);

            root_clip->SetBounds(inner_viewport_size);
            root->SetScrollClipLayer(root_clip->id());
            root->SetBounds(outer_viewport_size);
            root->SetPosition(gfx::PointF());
            root->SetDrawsContent(false);
            root->SetIsContainerForFixedPositionLayers(true);
            root_clip->SetHasRenderSurface(true);
            outer_clip->SetBounds(outer_viewport_size);
            outer_scroll->SetScrollClipLayer(outer_clip->id());
            outer_scroll->SetBounds(scroll_layer_size);
            outer_scroll->SetPosition(gfx::PointF());
            outer_scroll->SetDrawsContent(false);
            outer_scroll->SetIsContainerForFixedPositionLayers(true);

            int inner_viewport_scroll_layer_id = root->id();
            int outer_viewport_scroll_layer_id = outer_scroll->id();
            int page_scale_layer_id = page_scale->id();

            outer_clip->AddChild(outer_scroll.Pass());
            root->AddChild(outer_clip.Pass());
            page_scale->AddChild(root.Pass());
            root_clip->AddChild(page_scale.Pass());

            host_impl_->active_tree()->SetRootLayer(root_clip.Pass());
            host_impl_->active_tree()->SetViewportLayersFromIds(
                Layer::INVALID_ID, page_scale_layer_id, inner_viewport_scroll_layer_id,
                outer_viewport_scroll_layer_id);

            host_impl_->SetViewportSize(inner_viewport_size);
            LayerImpl* root_clip_ptr = host_impl_->active_tree()->root_layer();
            EXPECT_EQ(inner_viewport_size, root_clip_ptr->bounds());
        }

    protected:
        gfx::Size layer_size_;
        gfx::Size clip_size_;
        gfx::Size viewport_size_;
        float top_controls_height_;

        LayerTreeSettings settings_;
    }; // class LayerTreeHostImplTopControlsTest

    // Tests that, on a page with content the same size as the viewport, hiding
    // the top controls also increases the ScrollableSize (i.e. the content size).
    // Since the viewport got larger, the effective scrollable "content" also did.
    // This ensures, for one thing, that the overscroll glow is shown in the right
    // place.
    TEST_F(LayerTreeHostImplTopControlsTest,
        HidingTopControlsExpandsScrollableSize)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(50, 50), gfx::Size(50, 50), gfx::Size(50, 50));

        LayerTreeImpl* active_tree = host_impl_->active_tree();

        // Create a content layer beneath the outer viewport scroll layer.
        int id = host_impl_->OuterViewportScrollLayer()->id();
        host_impl_->OuterViewportScrollLayer()->AddChild(
            LayerImpl::Create(host_impl_->active_tree(), id + 2));
        LayerImpl* content = active_tree->OuterViewportScrollLayer()->children()[0];
        content->SetBounds(gfx::Size(50, 50));

        DrawFrame();

        LayerImpl* inner_container = active_tree->InnerViewportContainerLayer();
        LayerImpl* outer_container = active_tree->OuterViewportContainerLayer();

        // The top controls should start off showing so the viewport should be shrunk.
        ASSERT_EQ(gfx::Size(50, 50), inner_container->bounds());
        ASSERT_EQ(gfx::Size(50, 50), outer_container->bounds());

        EXPECT_EQ(gfx::SizeF(50, 50), active_tree->ScrollableSize());

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        host_impl_->top_controls_manager()->ScrollBegin();

        // Hide the top controls by a bit, the scrollable size should increase but the
        // actual content bounds shouldn't.
        {
            host_impl_->top_controls_manager()->ScrollBy(gfx::Vector2dF(0.f, 25.f));
            ASSERT_EQ(gfx::Size(50, 75), inner_container->bounds());
            ASSERT_EQ(gfx::Size(50, 75), outer_container->bounds());
            EXPECT_EQ(gfx::SizeF(50, 75), active_tree->ScrollableSize());
            EXPECT_EQ(gfx::SizeF(50, 50), content->BoundsForScrolling());
        }

        // Fully hide the top controls.
        {
            host_impl_->top_controls_manager()->ScrollBy(gfx::Vector2dF(0.f, 25.f));
            ASSERT_EQ(gfx::Size(50, 100), inner_container->bounds());
            ASSERT_EQ(gfx::Size(50, 100), outer_container->bounds());
            EXPECT_EQ(gfx::SizeF(50, 100), active_tree->ScrollableSize());
            EXPECT_EQ(gfx::SizeF(50, 50), content->BoundsForScrolling());
        }

        // Scrolling additionally shouldn't have any effect.
        {
            host_impl_->top_controls_manager()->ScrollBy(gfx::Vector2dF(0.f, 25.f));
            ASSERT_EQ(gfx::Size(50, 100), inner_container->bounds());
            ASSERT_EQ(gfx::Size(50, 100), outer_container->bounds());
            EXPECT_EQ(gfx::SizeF(50, 100), active_tree->ScrollableSize());
            EXPECT_EQ(gfx::SizeF(50, 50), content->BoundsForScrolling());
        }

        host_impl_->top_controls_manager()->ScrollEnd();
        host_impl_->ScrollEnd();
    }

    TEST_F(LayerTreeHostImplTopControlsTest, ScrollTopControlsByFractionalAmount)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(10, 10), gfx::Size(10, 10), gfx::Size(10, 10));
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        // Make the test scroll delta a fractional amount, to verify that the
        // fixed container size delta is (1) non-zero, and (2) fractional, and
        // (3) matches the movement of the top controls.
        gfx::Vector2dF top_controls_scroll_delta(0.f, 5.25f);
        host_impl_->top_controls_manager()->ScrollBegin();
        host_impl_->top_controls_manager()->ScrollBy(top_controls_scroll_delta);
        host_impl_->top_controls_manager()->ScrollEnd();

        LayerImpl* inner_viewport_scroll_layer = host_impl_->active_tree()->InnerViewportScrollLayer();
        DCHECK(inner_viewport_scroll_layer);
        host_impl_->ScrollEnd();
        EXPECT_FLOAT_EQ(top_controls_scroll_delta.y(),
            inner_viewport_scroll_layer->FixedContainerSizeDelta().y());
    }

    // In this test, the outer viewport is initially unscrollable. We test that a
    // scroll initiated on the inner viewport, causing the top controls to show and
    // thus making the outer viewport scrollable, still scrolls the outer viewport.
    TEST_F(LayerTreeHostImplTopControlsTest,
        TopControlsOuterViewportBecomesScrollable)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(10, 50), gfx::Size(10, 50), gfx::Size(10, 100));
        DrawFrame();

        LayerImpl* inner_scroll = host_impl_->active_tree()->InnerViewportScrollLayer();
        LayerImpl* inner_container = host_impl_->active_tree()->InnerViewportContainerLayer();
        LayerImpl* outer_scroll = host_impl_->active_tree()->OuterViewportScrollLayer();
        LayerImpl* outer_container = host_impl_->active_tree()->OuterViewportContainerLayer();

        // Need SetDrawsContent so ScrollBegin's hit test finds an actual layer.
        outer_scroll->SetDrawsContent(true);
        host_impl_->active_tree()->PushPageScaleFromMainThread(2.f, 1.f, 2.f);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0.f, 50.f));

        // The entire scroll delta should have been used to hide the top controls.
        // The viewport layers should be resized back to their full sizes.
        EXPECT_EQ(0.f, host_impl_->active_tree()->CurrentTopControlsShownRatio());
        EXPECT_EQ(0.f, inner_scroll->CurrentScrollOffset().y());
        EXPECT_EQ(100.f, inner_container->BoundsForScrolling().height());
        EXPECT_EQ(100.f, outer_container->BoundsForScrolling().height());

        // The inner viewport should be scrollable by 50px * page_scale.
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0.f, 100.f));
        EXPECT_EQ(50.f, inner_scroll->CurrentScrollOffset().y());
        EXPECT_EQ(0.f, outer_scroll->CurrentScrollOffset().y());
        EXPECT_EQ(gfx::ScrollOffset(), outer_scroll->MaxScrollOffset());

        host_impl_->ScrollEnd();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), inner_scroll);

        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0.f, -50.f));

        // The entire scroll delta should have been used to show the top controls.
        // The outer viewport should be resized to accomodate and scrolled to the
        // bottom of the document to keep the viewport in place.
        EXPECT_EQ(1.f, host_impl_->active_tree()->CurrentTopControlsShownRatio());
        EXPECT_EQ(50.f, outer_container->BoundsForScrolling().height());
        EXPECT_EQ(50.f, inner_container->BoundsForScrolling().height());
        EXPECT_EQ(25.f, outer_scroll->CurrentScrollOffset().y());
        EXPECT_EQ(25.f, inner_scroll->CurrentScrollOffset().y());

        // Now when we continue scrolling, make sure the outer viewport gets scrolled
        // since it wasn't scrollable when the scroll began.
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0.f, -20.f));
        EXPECT_EQ(25.f, outer_scroll->CurrentScrollOffset().y());
        EXPECT_EQ(15.f, inner_scroll->CurrentScrollOffset().y());

        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0.f, -30.f));
        EXPECT_EQ(25.f, outer_scroll->CurrentScrollOffset().y());
        EXPECT_EQ(0.f, inner_scroll->CurrentScrollOffset().y());

        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0.f, -50.f));
        host_impl_->ScrollEnd();

        EXPECT_EQ(0.f, outer_scroll->CurrentScrollOffset().y());
        EXPECT_EQ(0.f, inner_scroll->CurrentScrollOffset().y());
    }

    // Test that the fixed position container delta is appropriately adjusted
    // by the top controls showing/hiding and page scale doesn't affect it.
    TEST_F(LayerTreeHostImplTopControlsTest, FixedContainerDelta)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(100, 100), gfx::Size(100, 100), gfx::Size(100, 100));
        DrawFrame();
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 2.f);

        float page_scale = 1.5f;
        LayerImpl* outer_viewport_scroll_layer = host_impl_->active_tree()->OuterViewportScrollLayer();

        // Zoom in, since the fixed container is the outer viewport, the delta should
        // not be scaled.
        host_impl_->active_tree()->PushPageScaleFromMainThread(page_scale, 1.f, 2.f);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        // Scroll down, the top controls hiding should expand the viewport size so
        // the delta should be equal to the scroll distance.
        gfx::Vector2dF top_controls_scroll_delta(0.f, 20.f);
        host_impl_->top_controls_manager()->ScrollBegin();
        host_impl_->top_controls_manager()->ScrollBy(top_controls_scroll_delta);
        EXPECT_FLOAT_EQ(top_controls_height_ - top_controls_scroll_delta.y(),
            host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_FLOAT_EQ(top_controls_scroll_delta.y(),
            outer_viewport_scroll_layer->FixedContainerSizeDelta().y());
        host_impl_->ScrollEnd();

        // Scroll past the maximum extent. The delta shouldn't be greater than the
        // top controls height.
        host_impl_->top_controls_manager()->ScrollBegin();
        host_impl_->top_controls_manager()->ScrollBy(top_controls_scroll_delta);
        host_impl_->top_controls_manager()->ScrollBy(top_controls_scroll_delta);
        host_impl_->top_controls_manager()->ScrollBy(top_controls_scroll_delta);
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, top_controls_height_),
            outer_viewport_scroll_layer->FixedContainerSizeDelta());
        host_impl_->ScrollEnd();

        // Scroll in the direction to make the top controls show.
        host_impl_->top_controls_manager()->ScrollBegin();
        host_impl_->top_controls_manager()->ScrollBy(-top_controls_scroll_delta);
        EXPECT_EQ(top_controls_scroll_delta.y(),
            host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_VECTOR_EQ(
            gfx::Vector2dF(0, top_controls_height_ - top_controls_scroll_delta.y()),
            outer_viewport_scroll_layer->FixedContainerSizeDelta());
        host_impl_->top_controls_manager()->ScrollEnd();
    }

    // Test that if only the top controls are scrolled, we shouldn't request a
    // commit.
    TEST_F(LayerTreeHostImplTopControlsTest, TopControlsDontTriggerCommit)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(100, 50), gfx::Size(100, 100), gfx::Size(100, 100));
        DrawFrame();

        // Show top controls
        EXPECT_EQ(1.f, host_impl_->active_tree()->CurrentTopControlsShownRatio());

        // Scroll 25px to hide top controls
        gfx::Vector2dF scroll_delta(0.f, 25.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        EXPECT_FALSE(did_request_commit_);
    }

    // Test that if a scrollable sublayer doesn't consume the scroll,
    // top controls should hide when scrolling down.
    TEST_F(LayerTreeHostImplTopControlsTest, TopControlsScrollableSublayer)
    {
        gfx::Size sub_content_size(100, 400);
        gfx::Size sub_content_layer_size(100, 300);
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(100, 50), gfx::Size(100, 100), gfx::Size(100, 100));
        DrawFrame();

        // Show top controls
        EXPECT_EQ(1.f, host_impl_->active_tree()->CurrentTopControlsShownRatio());

        LayerImpl* outer_viewport_scroll_layer = host_impl_->active_tree()->OuterViewportScrollLayer();
        int id = outer_viewport_scroll_layer->id();

        scoped_ptr<LayerImpl> child = LayerImpl::Create(host_impl_->active_tree(), id + 2);
        scoped_ptr<LayerImpl> child_clip = LayerImpl::Create(host_impl_->active_tree(), id + 3);

        child_clip->SetBounds(sub_content_layer_size);
        child->SetScrollClipLayer(child_clip->id());
        child->SetBounds(sub_content_size);
        child->SetPosition(gfx::PointF());
        child->SetDrawsContent(true);
        child->SetIsContainerForFixedPositionLayers(true);

        // scroll child to limit
        child->SetScrollDelta(gfx::Vector2dF(0, 100.f));
        child_clip->AddChild(child.Pass());
        outer_viewport_scroll_layer->AddChild(child_clip.Pass());

        // Scroll 25px to hide top controls
        gfx::Vector2dF scroll_delta(0.f, 25.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();

        // Top controls should be hidden
        EXPECT_EQ(scroll_delta.y(),
            top_controls_height_ - host_impl_->top_controls_manager()->ContentTopOffset());
    }

    // Ensure setting the top controls position explicitly using the setters on the
    // TreeImpl correctly affects the top controls manager and viewport bounds.
    TEST_F(LayerTreeHostImplTopControlsTest, PositionTopControlsExplicitly)
    {
        CreateHostImpl(settings_, CreateOutputSurface());
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            layer_size_, layer_size_, layer_size_);
        DrawFrame();

        host_impl_->active_tree()->SetCurrentTopControlsShownRatio(0.f);
        host_impl_->active_tree()->top_controls_shown_ratio()->PushFromMainThread(
            30.f / top_controls_height_);
        host_impl_->active_tree()->top_controls_shown_ratio()->PushPendingToActive();
        EXPECT_FLOAT_EQ(30.f, host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_FLOAT_EQ(-20.f,
            host_impl_->top_controls_manager()->ControlsTopOffset());

        host_impl_->active_tree()->SetCurrentTopControlsShownRatio(0.f);
        EXPECT_FLOAT_EQ(0.f, host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_FLOAT_EQ(-50.f,
            host_impl_->top_controls_manager()->ControlsTopOffset());

        host_impl_->DidChangeTopControlsPosition();

        // Now that top controls have moved, expect the clip to resize.
        LayerImpl* inner_clip_ptr = host_impl_->InnerViewportScrollLayer()->parent()->parent();
        EXPECT_EQ(viewport_size_, inner_clip_ptr->bounds());
    }

    // Test that the top_controls delta and sent delta are appropriately
    // applied on sync tree activation. The total top controls offset shouldn't
    // change after the activation.
    TEST_F(LayerTreeHostImplTopControlsTest, ApplyDeltaOnTreeActivation)
    {
        CreateHostImpl(settings_, CreateOutputSurface());
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            layer_size_, layer_size_, layer_size_);
        DrawFrame();

        host_impl_->active_tree()->top_controls_shown_ratio()->PushFromMainThread(
            20.f / top_controls_height_);
        host_impl_->active_tree()->top_controls_shown_ratio()->PushPendingToActive();
        host_impl_->active_tree()->SetCurrentTopControlsShownRatio(
            15.f / top_controls_height_);
        host_impl_->active_tree()
            ->top_controls_shown_ratio()
            ->PullDeltaForMainThread();
        host_impl_->active_tree()->SetCurrentTopControlsShownRatio(0.f);
        host_impl_->sync_tree()->PushTopControlsFromMainThread(15.f / top_controls_height_);

        host_impl_->DidChangeTopControlsPosition();
        LayerImpl* inner_clip_ptr = host_impl_->InnerViewportScrollLayer()->parent()->parent();
        EXPECT_EQ(viewport_size_, inner_clip_ptr->bounds());
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ContentTopOffset());

        host_impl_->ActivateSyncTree();

        inner_clip_ptr = host_impl_->InnerViewportScrollLayer()->parent()->parent();
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_EQ(viewport_size_, inner_clip_ptr->bounds());

        EXPECT_FLOAT_EQ(
            -15.f, host_impl_->active_tree()->top_controls_shown_ratio()->Delta() * top_controls_height_);
        EXPECT_FLOAT_EQ(
            15.f,
            host_impl_->active_tree()->top_controls_shown_ratio()->ActiveBase() * top_controls_height_);
    }

    // Test that changing the top controls layout height is correctly applied to
    // the inner viewport container bounds. That is, the top controls layout
    // height is the amount that the inner viewport container was shrunk outside
    // the compositor to accommodate the top controls.
    TEST_F(LayerTreeHostImplTopControlsTest, TopControlsLayoutHeightChanged)
    {
        CreateHostImpl(settings_, CreateOutputSurface());
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            layer_size_, layer_size_, layer_size_);
        DrawFrame();

        host_impl_->sync_tree()->PushTopControlsFromMainThread(1.f);
        host_impl_->sync_tree()->set_top_controls_shrink_blink_size(true);

        host_impl_->active_tree()->top_controls_shown_ratio()->PushFromMainThread(
            1.f);
        host_impl_->active_tree()->top_controls_shown_ratio()->PushPendingToActive();
        host_impl_->active_tree()->SetCurrentTopControlsShownRatio(0.f);

        host_impl_->DidChangeTopControlsPosition();
        LayerImpl* inner_clip_ptr = host_impl_->InnerViewportScrollLayer()->parent()->parent();
        EXPECT_EQ(viewport_size_, inner_clip_ptr->bounds());
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ContentTopOffset());

        host_impl_->sync_tree()->root_layer()->SetBounds(
            gfx::Size(inner_clip_ptr->bounds().width(),
                inner_clip_ptr->bounds().height() - 50.f));

        host_impl_->ActivateSyncTree();

        inner_clip_ptr = host_impl_->InnerViewportScrollLayer()->parent()->parent();
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ContentTopOffset());

        // The total bounds should remain unchanged since the bounds delta should
        // account for the difference between the layout height and the current
        // top controls offset.
        EXPECT_EQ(viewport_size_, inner_clip_ptr->bounds());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0.f, 50.f), inner_clip_ptr->bounds_delta());

        host_impl_->active_tree()->SetCurrentTopControlsShownRatio(1.f);
        host_impl_->DidChangeTopControlsPosition();

        EXPECT_EQ(1.f, host_impl_->top_controls_manager()->TopControlsShownRatio());
        EXPECT_EQ(50.f, host_impl_->top_controls_manager()->TopControlsHeight());
        EXPECT_EQ(50.f, host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0.f, 0.f), inner_clip_ptr->bounds_delta());
        EXPECT_EQ(gfx::Size(viewport_size_.width(), viewport_size_.height() - 50.f),
            inner_clip_ptr->bounds());
    }

    // Test that showing/hiding the top controls when the viewport is fully scrolled
    // doesn't incorrectly change the viewport offset due to clamping from changing
    // viewport bounds.
    TEST_F(LayerTreeHostImplTopControlsTest, TopControlsViewportOffsetClamping)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(100, 100), gfx::Size(200, 200), gfx::Size(200, 400));
        DrawFrame();

        EXPECT_EQ(1.f, host_impl_->active_tree()->CurrentTopControlsShownRatio());

        LayerImpl* outer_scroll = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();

        // Scroll the viewports to max scroll offset.
        outer_scroll->SetScrollDelta(gfx::Vector2dF(0, 200.f));
        inner_scroll->SetScrollDelta(gfx::Vector2dF(100, 100.f));

        gfx::ScrollOffset viewport_offset = host_impl_->active_tree()->TotalScrollOffset();
        EXPECT_EQ(host_impl_->active_tree()->TotalMaxScrollOffset(), viewport_offset);

        // Hide the top controls by 25px.
        gfx::Vector2dF scroll_delta(0.f, 25.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);

        // scrolling down at the max extents no longer hides the top controls
        EXPECT_EQ(1.f, host_impl_->active_tree()->CurrentTopControlsShownRatio());

        // forcefully hide the top controls by 25px
        host_impl_->top_controls_manager()->ScrollBy(scroll_delta);
        host_impl_->ScrollEnd();

        EXPECT_FLOAT_EQ(scroll_delta.y(),
            top_controls_height_ - host_impl_->top_controls_manager()->ContentTopOffset());

        inner_scroll->ClampScrollToMaxScrollOffset();
        outer_scroll->ClampScrollToMaxScrollOffset();

        // We should still be fully scrolled.
        EXPECT_EQ(host_impl_->active_tree()->TotalMaxScrollOffset(),
            host_impl_->active_tree()->TotalScrollOffset());

        viewport_offset = host_impl_->active_tree()->TotalScrollOffset();

        // Bring the top controls down by 25px.
        scroll_delta = gfx::Vector2dF(0.f, -25.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();

        // The viewport offset shouldn't have changed.
        EXPECT_EQ(viewport_offset,
            host_impl_->active_tree()->TotalScrollOffset());

        // Scroll the viewports to max scroll offset.
        outer_scroll->SetScrollDelta(gfx::Vector2dF(0, 200.f));
        inner_scroll->SetScrollDelta(gfx::Vector2dF(100, 100.f));
        EXPECT_EQ(host_impl_->active_tree()->TotalMaxScrollOffset(),
            host_impl_->active_tree()->TotalScrollOffset());
    }

    // Test that the top controls coming in and out maintains the same aspect ratio
    // between the inner and outer viewports.
    TEST_F(LayerTreeHostImplTopControlsTest, TopControlsAspectRatio)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(100, 100), gfx::Size(200, 200), gfx::Size(200, 400));
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.5f, 2.f);
        DrawFrame();

        EXPECT_FLOAT_EQ(top_controls_height_,
            host_impl_->top_controls_manager()->ContentTopOffset());

        gfx::Vector2dF scroll_delta(0.f, 25.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();

        EXPECT_FLOAT_EQ(scroll_delta.y(),
            top_controls_height_ - host_impl_->top_controls_manager()->ContentTopOffset());

        // Top controls were hidden by 25px so the inner viewport should have expanded
        // by that much.
        LayerImpl* outer_container = host_impl_->active_tree()->OuterViewportContainerLayer();
        LayerImpl* inner_container = host_impl_->active_tree()->InnerViewportContainerLayer();
        EXPECT_EQ(gfx::SizeF(100.f, 100.f + 25.f),
            inner_container->BoundsForScrolling());

        // Outer viewport should match inner's aspect ratio. The bounds are ceiled.
        float aspect_ratio = inner_container->BoundsForScrolling().width() / inner_container->BoundsForScrolling().height();
        gfx::SizeF expected = gfx::SizeF(gfx::ToCeiledSize(gfx::SizeF(200, 200 / aspect_ratio)));
        EXPECT_EQ(expected, outer_container->BoundsForScrolling());
        EXPECT_EQ(expected,
            host_impl_->InnerViewportScrollLayer()->BoundsForScrolling());
    }

    // Test that scrolling the outer viewport affects the top controls.
    TEST_F(LayerTreeHostImplTopControlsTest, TopControlsScrollOuterViewport)
    {
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            gfx::Size(100, 100), gfx::Size(200, 200), gfx::Size(200, 400));
        DrawFrame();

        EXPECT_EQ(top_controls_height_,
            host_impl_->top_controls_manager()->ContentTopOffset());

        // Send a gesture scroll that will scroll the outer viewport, make sure the
        // top controls get scrolled.
        gfx::Vector2dF scroll_delta(0.f, 15.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);

        EXPECT_EQ(host_impl_->InnerViewportScrollLayer(),
            host_impl_->CurrentlyScrollingLayer());
        host_impl_->ScrollEnd();

        EXPECT_FLOAT_EQ(scroll_delta.y(),
            top_controls_height_ - host_impl_->top_controls_manager()->ContentTopOffset());

        scroll_delta = gfx::Vector2dF(0.f, 50.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);

        EXPECT_EQ(0, host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_EQ(host_impl_->InnerViewportScrollLayer(),
            host_impl_->CurrentlyScrollingLayer());

        host_impl_->ScrollEnd();

        // Position the viewports such that the inner viewport will be scrolled.
        gfx::Vector2dF inner_viewport_offset(0.f, 25.f);
        host_impl_->OuterViewportScrollLayer()->SetScrollDelta(gfx::Vector2dF());
        host_impl_->InnerViewportScrollLayer()->SetScrollDelta(inner_viewport_offset);

        scroll_delta = gfx::Vector2dF(0.f, -65.f);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);

        EXPECT_EQ(top_controls_height_,
            host_impl_->top_controls_manager()->ContentTopOffset());
        EXPECT_FLOAT_EQ(
            inner_viewport_offset.y() + (scroll_delta.y() + top_controls_height_),
            host_impl_->InnerViewportScrollLayer()->ScrollDelta().y());

        host_impl_->ScrollEnd();
    }

    TEST_F(LayerTreeHostImplTopControlsTest,
        ScrollNonScrollableRootWithTopControls)
    {
        CreateHostImpl(settings_, CreateOutputSurface());
        SetupTopControlsAndScrollLayerWithVirtualViewport(
            layer_size_, layer_size_, layer_size_);
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        host_impl_->top_controls_manager()->ScrollBegin();
        host_impl_->top_controls_manager()->ScrollBy(gfx::Vector2dF(0.f, 50.f));
        host_impl_->top_controls_manager()->ScrollEnd();
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ContentTopOffset());
        // Now that top controls have moved, expect the clip to resize.
        LayerImpl* inner_clip_ptr = host_impl_->InnerViewportScrollLayer()->parent()->parent();
        EXPECT_EQ(viewport_size_, inner_clip_ptr->bounds());

        host_impl_->ScrollEnd();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

        float scroll_increment_y = -25.f;
        host_impl_->top_controls_manager()->ScrollBegin();
        host_impl_->top_controls_manager()->ScrollBy(
            gfx::Vector2dF(0.f, scroll_increment_y));
        EXPECT_FLOAT_EQ(-scroll_increment_y,
            host_impl_->top_controls_manager()->ContentTopOffset());
        // Now that top controls have moved, expect the clip to resize.
        EXPECT_EQ(gfx::Size(viewport_size_.width(),
                      viewport_size_.height() + scroll_increment_y),
            inner_clip_ptr->bounds());

        host_impl_->top_controls_manager()->ScrollBy(
            gfx::Vector2dF(0.f, scroll_increment_y));
        host_impl_->top_controls_manager()->ScrollEnd();
        EXPECT_FLOAT_EQ(-2 * scroll_increment_y,
            host_impl_->top_controls_manager()->ContentTopOffset());
        // Now that top controls have moved, expect the clip to resize.
        EXPECT_EQ(clip_size_, inner_clip_ptr->bounds());

        host_impl_->ScrollEnd();

        // Verify the layer is once-again non-scrollable.
        EXPECT_EQ(
            gfx::ScrollOffset(),
            host_impl_->active_tree()->InnerViewportScrollLayer()->MaxScrollOffset());

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
    }

    TEST_F(LayerTreeHostImplTest, ScrollNonCompositedRoot)
    {
        // Test the configuration where a non-composited root layer is embedded in a
        // scrollable outer layer.
        gfx::Size surface_size(10, 10);
        gfx::Size contents_size(20, 20);

        scoped_ptr<LayerImpl> content_layer = LayerImpl::Create(host_impl_->active_tree(), 1);
        content_layer->SetDrawsContent(true);
        content_layer->SetPosition(gfx::PointF());
        content_layer->SetBounds(contents_size);

        scoped_ptr<LayerImpl> scroll_clip_layer = LayerImpl::Create(host_impl_->active_tree(), 3);
        scroll_clip_layer->SetBounds(surface_size);

        scoped_ptr<LayerImpl> scroll_layer = LayerImpl::Create(host_impl_->active_tree(), 2);
        scroll_layer->SetScrollClipLayer(3);
        scroll_layer->SetBounds(contents_size);
        scroll_layer->SetPosition(gfx::PointF());
        scroll_layer->AddChild(content_layer.Pass());
        scroll_clip_layer->AddChild(scroll_layer.Pass());

        scroll_clip_layer->SetHasRenderSurface(true);
        host_impl_->active_tree()->SetRootLayer(scroll_clip_layer.Pass());
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        host_impl_->ScrollEnd();
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_TRUE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, ScrollChildCallsCommitAndRedraw)
    {
        gfx::Size surface_size(10, 10);
        gfx::Size contents_size(20, 20);
        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetBounds(surface_size);
        root->AddChild(CreateScrollableLayer(2, contents_size, root.get()));
        root->SetHasRenderSurface(true);
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        host_impl_->ScrollEnd();
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_TRUE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, ScrollMissesChild)
    {
        gfx::Size surface_size(10, 10);
        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->AddChild(CreateScrollableLayer(2, surface_size, root.get()));
        root->SetHasRenderSurface(true);
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();

        // Scroll event is ignored because the input coordinate is outside the layer
        // boundaries.
        EXPECT_EQ(InputHandler::SCROLL_IGNORED,
            host_impl_->ScrollBegin(gfx::Point(15, 5), InputHandler::WHEEL));
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, ScrollMissesBackfacingChild)
    {
        gfx::Size surface_size(10, 10);
        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetHasRenderSurface(true);
        scoped_ptr<LayerImpl> child = CreateScrollableLayer(2, surface_size, root.get());
        host_impl_->SetViewportSize(surface_size);

        gfx::Transform matrix;
        matrix.RotateAboutXAxis(180.0);
        child->SetTransform(matrix);
        child->SetDoubleSided(false);

        root->AddChild(child.Pass());
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        DrawFrame();

        // Scroll event is ignored because the scrollable layer is not facing the
        // viewer and there is nothing scrollable behind it.
        EXPECT_EQ(InputHandler::SCROLL_IGNORED,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
        EXPECT_FALSE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);
    }

    TEST_F(LayerTreeHostImplTest, ScrollBlockedByContentLayer)
    {
        gfx::Size surface_size(10, 10);
        scoped_ptr<LayerImpl> clip_layer = LayerImpl::Create(host_impl_->active_tree(), 3);
        scoped_ptr<LayerImpl> content_layer = CreateScrollableLayer(1, surface_size, clip_layer.get());
        content_layer->SetShouldScrollOnMainThread(true);
        content_layer->SetScrollClipLayer(Layer::INVALID_ID);

        // Note: we can use the same clip layer for both since both calls to
        // CreateScrollableLayer() use the same surface size.
        scoped_ptr<LayerImpl> scroll_layer = CreateScrollableLayer(2, surface_size, clip_layer.get());
        scroll_layer->AddChild(content_layer.Pass());
        clip_layer->AddChild(scroll_layer.Pass());
        clip_layer->SetHasRenderSurface(true);

        host_impl_->active_tree()->SetRootLayer(clip_layer.Pass());
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();

        // Scrolling fails because the content layer is asking to be scrolled on the
        // main thread.
        EXPECT_EQ(InputHandler::SCROLL_ON_MAIN_THREAD,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
    }

    TEST_F(LayerTreeHostImplTest, ScrollRootAndChangePageScaleOnMainThread)
    {
        gfx::Size viewport_size(20, 20);
        float page_scale = 2.f;

        SetupScrollAndContentsLayers(viewport_size);

        // Setup the layers so that the outer viewport is scrollable.
        host_impl_->active_tree()->InnerViewportScrollLayer()->parent()->SetBounds(
            viewport_size);
        host_impl_->active_tree()->OuterViewportScrollLayer()->SetBounds(
            gfx::Size(40, 40));
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 2.f);
        DrawFrame();

        LayerImpl* root_scroll = host_impl_->active_tree()->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->active_tree()->InnerViewportScrollLayer();
        EXPECT_EQ(viewport_size, root_scroll->scroll_clip_layer()->bounds());

        gfx::Vector2d scroll_delta(0, 10);
        gfx::Vector2d expected_scroll_delta = scroll_delta;
        gfx::ScrollOffset expected_max_scroll = root_scroll->MaxScrollOffset();
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();

        // Set new page scale from main thread.
        host_impl_->active_tree()->PushPageScaleFromMainThread(page_scale, 1.f, 2.f);

        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), inner_scroll->id(),
            expected_scroll_delta));

        // The scroll range should also have been updated.
        EXPECT_EQ(expected_max_scroll, root_scroll->MaxScrollOffset());

        // The page scale delta remains constant because the impl thread did not
        // scale.
        EXPECT_EQ(1.f, host_impl_->active_tree()->page_scale_delta());
    }

    TEST_F(LayerTreeHostImplTest, ScrollRootAndChangePageScaleOnImplThread)
    {
        gfx::Size viewport_size(20, 20);
        float page_scale = 2.f;

        SetupScrollAndContentsLayers(viewport_size);

        // Setup the layers so that the outer viewport is scrollable.
        host_impl_->active_tree()->InnerViewportScrollLayer()->parent()->SetBounds(
            viewport_size);
        host_impl_->active_tree()->OuterViewportScrollLayer()->SetBounds(
            gfx::Size(40, 40));
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 2.f);
        DrawFrame();

        LayerImpl* root_scroll = host_impl_->active_tree()->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->active_tree()->InnerViewportScrollLayer();
        EXPECT_EQ(viewport_size, root_scroll->scroll_clip_layer()->bounds());

        gfx::Vector2d scroll_delta(0, 10);
        gfx::Vector2d expected_scroll_delta = scroll_delta;
        gfx::ScrollOffset expected_max_scroll = root_scroll->MaxScrollOffset();
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();

        // Set new page scale on impl thread by pinching.
        host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(page_scale, gfx::Point());
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();
        DrawOneFrame();

        // The scroll delta is not scaled because the main thread did not scale.
        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), inner_scroll->id(),
            expected_scroll_delta));

        // The scroll range should also have been updated.
        EXPECT_EQ(expected_max_scroll, root_scroll->MaxScrollOffset());

        // The page scale delta should match the new scale on the impl side.
        EXPECT_EQ(page_scale, host_impl_->active_tree()->current_page_scale_factor());
    }

    TEST_F(LayerTreeHostImplTest, PageScaleDeltaAppliedToRootScrollLayerOnly)
    {
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 1.f, 2.f);
        gfx::Size surface_size(10, 10);
        float default_page_scale = 1.f;
        gfx::Transform default_page_scale_matrix;
        default_page_scale_matrix.Scale(default_page_scale, default_page_scale);

        float new_page_scale = 2.f;
        gfx::Transform new_page_scale_matrix;
        new_page_scale_matrix.Scale(new_page_scale, new_page_scale);

        // Create a normal scrollable root layer and another scrollable child layer.
        LayerImpl* scroll = SetupScrollAndContentsLayers(surface_size);
        LayerImpl* root = host_impl_->active_tree()->root_layer();
        LayerImpl* child = scroll->children()[0];

        scoped_ptr<LayerImpl> scrollable_child_clip = LayerImpl::Create(host_impl_->active_tree(), 6);
        scoped_ptr<LayerImpl> scrollable_child = CreateScrollableLayer(7, surface_size, scrollable_child_clip.get());
        scrollable_child_clip->AddChild(scrollable_child.Pass());
        child->AddChild(scrollable_child_clip.Pass());
        LayerImpl* grand_child = child->children()[0];

        // Set new page scale on impl thread by pinching.
        RebuildPropertyTrees();
        host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(new_page_scale, gfx::Point());
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();
        DrawOneFrame();

        // Make sure all the layers are drawn with the page scale delta applied, i.e.,
        // the page scale delta on the root layer is applied hierarchically.
        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);

        EXPECT_EQ(1.f, root->draw_transform().matrix().getDouble(0, 0));
        EXPECT_EQ(1.f, root->draw_transform().matrix().getDouble(1, 1));
        EXPECT_EQ(new_page_scale, scroll->draw_transform().matrix().getDouble(0, 0));
        EXPECT_EQ(new_page_scale, scroll->draw_transform().matrix().getDouble(1, 1));
        EXPECT_EQ(new_page_scale, child->draw_transform().matrix().getDouble(0, 0));
        EXPECT_EQ(new_page_scale, child->draw_transform().matrix().getDouble(1, 1));
        EXPECT_EQ(new_page_scale,
            grand_child->draw_transform().matrix().getDouble(0, 0));
        EXPECT_EQ(new_page_scale,
            grand_child->draw_transform().matrix().getDouble(1, 1));
    }

    TEST_F(LayerTreeHostImplTest, ScrollChildAndChangePageScaleOnMainThread)
    {
        SetupScrollAndContentsLayers(gfx::Size(30, 30));

        LayerImpl* outer_scroll = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();

        // Make the outer scroll layer scrollable.
        outer_scroll->SetBounds(gfx::Size(50, 50));

        DrawFrame();

        gfx::Vector2d scroll_delta(0, 10);
        gfx::Vector2d expected_scroll_delta(scroll_delta);
        gfx::ScrollOffset expected_max_scroll(outer_scroll->MaxScrollOffset());
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();

        float page_scale = 2.f;
        host_impl_->active_tree()->PushPageScaleFromMainThread(page_scale, 1.f,
            page_scale);

        DrawOneFrame();

        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), inner_scroll->id(),
            expected_scroll_delta));

        // The scroll range should not have changed.
        EXPECT_EQ(outer_scroll->MaxScrollOffset(), expected_max_scroll);

        // The page scale delta remains constant because the impl thread did not
        // scale.
        EXPECT_EQ(1.f, host_impl_->active_tree()->page_scale_delta());
    }

    TEST_F(LayerTreeHostImplTest, ScrollChildBeyondLimit)
    {
        // Scroll a child layer beyond its maximum scroll range and make sure the
        // parent layer isn't scrolled.
        gfx::Size surface_size(10, 10);
        gfx::Size content_size(20, 20);
        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetBounds(surface_size);
        root->SetHasRenderSurface(true);
        scoped_ptr<LayerImpl> grand_child = CreateScrollableLayer(3, content_size, root.get());

        scoped_ptr<LayerImpl> child = CreateScrollableLayer(2, content_size, root.get());
        LayerImpl* grand_child_layer = grand_child.get();
        child->AddChild(grand_child.Pass());

        LayerImpl* child_layer = child.get();
        root->AddChild(child.Pass());
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        host_impl_->active_tree()->DidBecomeActive();
        host_impl_->SetViewportSize(surface_size);
        grand_child_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 5));
        child_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(3, 0));

        DrawFrame();
        {
            gfx::Vector2d scroll_delta(-8, -7);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();

            // The grand child should have scrolled up to its limit.
            LayerImpl* child = host_impl_->active_tree()->root_layer()->children()[0];
            LayerImpl* grand_child = child->children()[0];
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), grand_child->id(),
                gfx::Vector2d(0, -5)));

            // The child should not have scrolled.
            ExpectNone(*scroll_info.get(), child->id());
        }
    }

    TEST_F(LayerTreeHostImplTest, ScrollWithoutBubbling)
    {
        // Scroll a child layer beyond its maximum scroll range and make sure the
        // the scroll doesn't bubble up to the parent layer.
        gfx::Size surface_size(20, 20);
        gfx::Size viewport_size(10, 10);
        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetHasRenderSurface(true);
        scoped_ptr<LayerImpl> root_scrolling = CreateScrollableLayer(2, surface_size, root.get());
        root_scrolling->SetIsContainerForFixedPositionLayers(true);

        scoped_ptr<LayerImpl> grand_child = CreateScrollableLayer(4, surface_size, root.get());

        scoped_ptr<LayerImpl> child = CreateScrollableLayer(3, surface_size, root.get());
        LayerImpl* grand_child_layer = grand_child.get();
        child->AddChild(grand_child.Pass());

        LayerImpl* child_layer = child.get();
        root_scrolling->AddChild(child.Pass());
        root->AddChild(root_scrolling.Pass());
        EXPECT_EQ(viewport_size, root->bounds());
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        host_impl_->active_tree()->SetViewportLayersFromIds(Layer::INVALID_ID, 1, 2,
            Layer::INVALID_ID);
        host_impl_->active_tree()->DidBecomeActive();
        host_impl_->SetViewportSize(viewport_size);

        grand_child_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 2));
        child_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 3));

        DrawFrame();
        {
            gfx::Vector2d scroll_delta(0, -10);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(),
                    InputHandler::NON_BUBBLING_GESTURE));
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();

            // The grand child should have scrolled up to its limit.
            LayerImpl* child = host_impl_->active_tree()->root_layer()->children()[0]->children()[0];
            LayerImpl* grand_child = child->children()[0];
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), grand_child->id(),
                gfx::Vector2d(0, -2)));

            // The child should not have scrolled.
            ExpectNone(*scroll_info.get(), child->id());

            // The next time we scroll we should only scroll the parent.
            scroll_delta = gfx::Vector2d(0, -3);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(5, 5),
                    InputHandler::NON_BUBBLING_GESTURE));
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), child);
            host_impl_->ScrollEnd();

            scroll_info = host_impl_->ProcessScrollDeltas();

            // The child should have scrolled up to its limit.
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), child->id(),
                gfx::Vector2d(0, -3)));

            // The grand child should not have scrolled.
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), grand_child->id(),
                gfx::Vector2d(0, -2)));

            // After scrolling the parent, another scroll on the opposite direction
            // should still scroll the child.
            scroll_delta = gfx::Vector2d(0, 7);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(5, 5),
                    InputHandler::NON_BUBBLING_GESTURE));
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child);
            host_impl_->ScrollEnd();

            scroll_info = host_impl_->ProcessScrollDeltas();

            // The grand child should have scrolled.
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), grand_child->id(),
                gfx::Vector2d(0, 5)));

            // The child should not have scrolled.
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), child->id(),
                gfx::Vector2d(0, -3)));

            // Scrolling should be adjusted from viewport space.
            host_impl_->active_tree()->PushPageScaleFromMainThread(2.f, 2.f, 2.f);
            host_impl_->active_tree()->SetPageScaleOnActiveTree(2.f);

            scroll_delta = gfx::Vector2d(0, -2);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(1, 1),
                    InputHandler::NON_BUBBLING_GESTURE));
            EXPECT_EQ(grand_child, host_impl_->CurrentlyScrollingLayer());
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollEnd();

            scroll_info = host_impl_->ProcessScrollDeltas();

            // Should have scrolled by half the amount in layer space (5 - 2/2)
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), grand_child->id(),
                gfx::Vector2d(0, 4)));
        }
    }
    TEST_F(LayerTreeHostImplTest, ScrollEventBubbling)
    {
        // When we try to scroll a non-scrollable child layer, the scroll delta
        // should be applied to one of its ancestors if possible.
        gfx::Size surface_size(10, 10);
        gfx::Size content_size(20, 20);
        scoped_ptr<LayerImpl> root_clip = LayerImpl::Create(host_impl_->active_tree(), 3);
        root_clip->SetHasRenderSurface(true);
        scoped_ptr<LayerImpl> root = CreateScrollableLayer(1, content_size, root_clip.get());
        // Make 'root' the clip layer for child: since they have the same sizes the
        // child will have zero max_scroll_offset and scrolls will bubble.
        scoped_ptr<LayerImpl> child = CreateScrollableLayer(2, content_size, root.get());
        child->SetIsContainerForFixedPositionLayers(true);
        root->SetBounds(content_size);

        int root_scroll_id = root->id();
        root->AddChild(child.Pass());
        root_clip->AddChild(root.Pass());

        host_impl_->SetViewportSize(surface_size);
        host_impl_->active_tree()->SetRootLayer(root_clip.Pass());
        host_impl_->active_tree()->SetViewportLayersFromIds(Layer::INVALID_ID, 3, 2,
            Layer::INVALID_ID);
        host_impl_->active_tree()->DidBecomeActive();
        DrawFrame();
        {
            gfx::Vector2d scroll_delta(0, 4);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();

            // Only the root scroll should have scrolled.
            ASSERT_EQ(scroll_info->scrolls.size(), 1u);
            EXPECT_TRUE(
                ScrollInfoContains(*scroll_info.get(), root_scroll_id, scroll_delta));
        }
    }

    TEST_F(LayerTreeHostImplTest, ScrollBeforeRedraw)
    {
        gfx::Size surface_size(10, 10);
        scoped_ptr<LayerImpl> root_clip = LayerImpl::Create(host_impl_->active_tree(), 1);
        scoped_ptr<LayerImpl> root_scroll = CreateScrollableLayer(2, surface_size, root_clip.get());
        root_scroll->SetIsContainerForFixedPositionLayers(true);
        root_clip->SetHasRenderSurface(true);
        root_clip->AddChild(root_scroll.Pass());
        host_impl_->active_tree()->SetRootLayer(root_clip.Pass());
        host_impl_->active_tree()->SetViewportLayersFromIds(Layer::INVALID_ID, 1, 2,
            Layer::INVALID_ID);
        host_impl_->active_tree()->DidBecomeActive();
        host_impl_->SetViewportSize(surface_size);

        // Draw one frame and then immediately rebuild the layer tree to mimic a tree
        // synchronization.
        DrawFrame();
        host_impl_->active_tree()->DetachLayerTree();
        scoped_ptr<LayerImpl> root_clip2 = LayerImpl::Create(host_impl_->active_tree(), 3);
        scoped_ptr<LayerImpl> root_scroll2 = CreateScrollableLayer(4, surface_size, root_clip2.get());
        root_scroll2->SetIsContainerForFixedPositionLayers(true);
        root_clip2->AddChild(root_scroll2.Pass());
        root_clip2->SetHasRenderSurface(true);
        host_impl_->active_tree()->SetRootLayer(root_clip2.Pass());
        host_impl_->active_tree()->SetViewportLayersFromIds(Layer::INVALID_ID, 3, 4,
            Layer::INVALID_ID);
        host_impl_->active_tree()->DidBecomeActive();

        // Scrolling should still work even though we did not draw yet.
        RebuildPropertyTrees();
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
    }

    TEST_F(LayerTreeHostImplTest, ScrollAxisAlignedRotatedLayer)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));

        // Rotate the root layer 90 degrees counter-clockwise about its center.
        gfx::Transform rotate_transform;
        rotate_transform.Rotate(-90.0);
        host_impl_->active_tree()->root_layer()->SetTransform(rotate_transform);

        gfx::Size surface_size(50, 50);
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();

        // Scroll to the right in screen coordinates with a gesture.
        gfx::Vector2d gesture_scroll_delta(10, 0);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), gesture_scroll_delta);
        host_impl_->ScrollEnd();

        // The layer should have scrolled down in its local coordinates.
        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), scroll_layer->id(),
            gfx::Vector2d(0, gesture_scroll_delta.x())));

        // Reset and scroll down with the wheel.
        scroll_layer->SetScrollDelta(gfx::Vector2dF());
        gfx::Vector2d wheel_scroll_delta(0, 10);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), wheel_scroll_delta);
        host_impl_->ScrollEnd();

        // The layer should have scrolled down in its local coordinates.
        scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), scroll_layer->id(),
            wheel_scroll_delta));
    }

    TEST_F(LayerTreeHostImplTest, ScrollNonAxisAlignedRotatedLayer)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        int child_clip_layer_id = 6;
        int child_layer_id = 7;
        float child_layer_angle = -20.f;

        // Create a child layer that is rotated to a non-axis-aligned angle.
        scoped_ptr<LayerImpl> clip_layer = LayerImpl::Create(host_impl_->active_tree(), child_clip_layer_id);
        scoped_ptr<LayerImpl> child = CreateScrollableLayer(
            child_layer_id, scroll_layer->bounds(), clip_layer.get());
        gfx::Transform rotate_transform;
        rotate_transform.Translate(-50.0, -50.0);
        rotate_transform.Rotate(child_layer_angle);
        rotate_transform.Translate(50.0, 50.0);
        clip_layer->SetTransform(rotate_transform);

        // Only allow vertical scrolling.
        clip_layer->SetBounds(
            gfx::Size(child->bounds().width(), child->bounds().height() / 2));
        // The rotation depends on the layer's transform origin, and the child layer
        // is a different size than the clip, so make sure the clip layer's origin
        // lines up over the child.
        clip_layer->SetTransformOrigin(gfx::Point3F(
            clip_layer->bounds().width() * 0.5f, clip_layer->bounds().height(), 0.f));
        LayerImpl* child_ptr = child.get();
        clip_layer->AddChild(child.Pass());
        scroll_layer->AddChild(clip_layer.Pass());

        gfx::Size surface_size(50, 50);
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();
        {
            // Scroll down in screen coordinates with a gesture.
            gfx::Vector2d gesture_scroll_delta(0, 10);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(1, 1), InputHandler::GESTURE));
            host_impl_->ScrollBy(gfx::Point(), gesture_scroll_delta);
            host_impl_->ScrollEnd();

            // The child layer should have scrolled down in its local coordinates an
            // amount proportional to the angle between it and the input scroll delta.
            gfx::Vector2d expected_scroll_delta(
                0, gesture_scroll_delta.y() * std::cos(MathUtil::Deg2Rad(child_layer_angle)));
            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), child_layer_id,
                expected_scroll_delta));

            // The root scroll layer should not have scrolled, because the input delta
            // was close to the layer's axis of movement.
            EXPECT_EQ(scroll_info->scrolls.size(), 1u);
        }
        {
            // Now reset and scroll the same amount horizontally.
            child_ptr->SetScrollDelta(gfx::Vector2dF());
            gfx::Vector2d gesture_scroll_delta(10, 0);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(1, 1), InputHandler::GESTURE));
            host_impl_->ScrollBy(gfx::Point(), gesture_scroll_delta);
            host_impl_->ScrollEnd();

            // The child layer shouldn't have scrolled.
            gfx::Vector2d expected_scroll_delta(
                0, -gesture_scroll_delta.x() * std::sin(MathUtil::Deg2Rad(child_layer_angle)));
            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), child_layer_id,
                expected_scroll_delta));

            // The root scroll layer shouldn't have scrolled.
            ExpectNone(*scroll_info.get(), scroll_layer->id());
        }
    }

    TEST_F(LayerTreeHostImplTest, ScrollPerspectiveTransformedLayer)
    {
        // When scrolling an element with perspective, the distance scrolled
        // depends on the point at which the scroll begins.
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        int child_clip_layer_id = 6;
        int child_layer_id = 7;

        // Create a child layer that is rotated on its x axis, with perspective.
        scoped_ptr<LayerImpl> clip_layer = LayerImpl::Create(host_impl_->active_tree(), child_clip_layer_id);
        scoped_ptr<LayerImpl> child = CreateScrollableLayer(
            child_layer_id, scroll_layer->bounds(), clip_layer.get());
        LayerImpl* child_ptr = child.get();
        gfx::Transform perspective_transform;
        perspective_transform.Translate(-50.0, -50.0);
        perspective_transform.ApplyPerspectiveDepth(20);
        perspective_transform.RotateAboutXAxis(45);
        perspective_transform.Translate(50.0, 50.0);
        clip_layer->SetTransform(perspective_transform);

        clip_layer->SetBounds(gfx::Size(child_ptr->bounds().width() / 2,
            child_ptr->bounds().height() / 2));
        // The transform depends on the layer's transform origin, and the child layer
        // is a different size than the clip, so make sure the clip layer's origin
        // lines up over the child.
        clip_layer->SetTransformOrigin(gfx::Point3F(
            clip_layer->bounds().width(), clip_layer->bounds().height(), 0.f));
        clip_layer->AddChild(child.Pass());
        scroll_layer->AddChild(clip_layer.Pass());

        gfx::Size surface_size(50, 50);
        host_impl_->SetViewportSize(surface_size);

        scoped_ptr<ScrollAndScaleSet> scroll_info;

        gfx::Vector2d gesture_scroll_deltas[4];
        gesture_scroll_deltas[0] = gfx::Vector2d(4, 10);
        gesture_scroll_deltas[1] = gfx::Vector2d(4, 10);
        gesture_scroll_deltas[2] = gfx::Vector2d(10, 0);
        gesture_scroll_deltas[3] = gfx::Vector2d(10, 0);

        gfx::Vector2d expected_scroll_deltas[4];
        // Perspective affects the vertical delta by a different
        // amount depending on the vertical position of the |viewport_point|.
        expected_scroll_deltas[0] = gfx::Vector2d(2, 8);
        expected_scroll_deltas[1] = gfx::Vector2d(1, 4);
        // Deltas which start with the same vertical position of the
        // |viewport_point| are subject to identical perspective effects.
        expected_scroll_deltas[2] = gfx::Vector2d(4, 0);
        expected_scroll_deltas[3] = gfx::Vector2d(4, 0);

        gfx::Point viewport_point(1, 1);

        // Scroll in screen coordinates with a gesture. Each scroll starts
        // where the previous scroll ended, but the scroll position is reset
        // for each scroll.
        for (int i = 0; i < 4; ++i) {
            child_ptr->SetScrollDelta(gfx::Vector2dF());
            DrawFrame();
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(viewport_point, InputHandler::GESTURE));
            host_impl_->ScrollBy(viewport_point, gesture_scroll_deltas[i]);
            viewport_point += gesture_scroll_deltas[i];
            host_impl_->ScrollEnd();

            scroll_info = host_impl_->ProcessScrollDeltas();
            EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), child_layer_id,
                expected_scroll_deltas[i]));

            // The root scroll layer should not have scrolled, because the input delta
            // was close to the layer's axis of movement.
            EXPECT_EQ(scroll_info->scrolls.size(), 1u);
        }
    }

    TEST_F(LayerTreeHostImplTest, ScrollScaledLayer)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));

        // Scale the layer to twice its normal size.
        int scale = 2;
        gfx::Transform scale_transform;
        scale_transform.Scale(scale, scale);
        scroll_layer->parent()->SetTransform(scale_transform);

        gfx::Size surface_size(50, 50);
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();

        // Scroll down in screen coordinates with a gesture.
        gfx::Vector2d scroll_delta(0, 10);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        host_impl_->ScrollEnd();

        // The layer should have scrolled down in its local coordinates, but half the
        // amount.
        scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), scroll_layer->id(),
            gfx::Vector2d(0, scroll_delta.y() / scale)));

        // Reset and scroll down with the wheel.
        scroll_layer->SetScrollDelta(gfx::Vector2dF());
        gfx::Vector2d wheel_scroll_delta(0, 10);
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        host_impl_->ScrollBy(gfx::Point(), wheel_scroll_delta);
        host_impl_->ScrollEnd();

        // It should apply the scale factor to the scroll delta for the wheel event.
        scroll_info = host_impl_->ProcessScrollDeltas();
        EXPECT_TRUE(ScrollInfoContains(*scroll_info.get(), scroll_layer->id(),
            wheel_scroll_delta));
    }

    TEST_F(LayerTreeHostImplTest, ScrollViewportRounding)
    {
        int width = 332;
        int height = 20;
        int scale = 3;
        SetupScrollAndContentsLayers(gfx::Size(width, height));
        host_impl_->active_tree()->InnerViewportContainerLayer()->SetBounds(
            gfx::Size(width * scale - 1, height * scale));
        host_impl_->active_tree()->SetDeviceScaleFactor(scale);
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.5f, 4.f);

        LayerImpl* inner_viewport_scroll_layer = host_impl_->active_tree()->InnerViewportScrollLayer();
        EXPECT_EQ(gfx::ScrollOffset(0, 0),
            inner_viewport_scroll_layer->MaxScrollOffset());
    }

    class TestInputHandlerClient : public InputHandlerClient {
    public:
        TestInputHandlerClient()
            : page_scale_factor_(0.f)
            , min_page_scale_factor_(-1.f)
            , max_page_scale_factor_(-1.f)
        {
        }
        ~TestInputHandlerClient() override { }

        // InputHandlerClient implementation.
        void WillShutdown() override { }
        void Animate(base::TimeTicks time) override { }
        void MainThreadHasStoppedFlinging() override { }
        void ReconcileElasticOverscrollAndRootScroll() override { }
        void UpdateRootLayerStateForSynchronousInputHandler(
            const gfx::ScrollOffset& total_scroll_offset,
            const gfx::ScrollOffset& max_scroll_offset,
            const gfx::SizeF& scrollable_size,
            float page_scale_factor,
            float min_page_scale_factor,
            float max_page_scale_factor) override
        {
            DCHECK(total_scroll_offset.x() <= max_scroll_offset.x());
            DCHECK(total_scroll_offset.y() <= max_scroll_offset.y());
            last_set_scroll_offset_ = total_scroll_offset;
            max_scroll_offset_ = max_scroll_offset;
            scrollable_size_ = scrollable_size;
            page_scale_factor_ = page_scale_factor;
            min_page_scale_factor_ = min_page_scale_factor;
            max_page_scale_factor_ = max_page_scale_factor;
        }

        gfx::ScrollOffset last_set_scroll_offset()
        {
            return last_set_scroll_offset_;
        }

        gfx::ScrollOffset max_scroll_offset() const
        {
            return max_scroll_offset_;
        }

        gfx::SizeF scrollable_size() const
        {
            return scrollable_size_;
        }

        float page_scale_factor() const
        {
            return page_scale_factor_;
        }

        float min_page_scale_factor() const
        {
            return min_page_scale_factor_;
        }

        float max_page_scale_factor() const
        {
            return max_page_scale_factor_;
        }

    private:
        gfx::ScrollOffset last_set_scroll_offset_;
        gfx::ScrollOffset max_scroll_offset_;
        gfx::SizeF scrollable_size_;
        float page_scale_factor_;
        float min_page_scale_factor_;
        float max_page_scale_factor_;
    };

    TEST_F(LayerTreeHostImplTest, RootLayerScrollOffsetDelegation)
    {
        TestInputHandlerClient scroll_watcher;
        host_impl_->SetViewportSize(gfx::Size(10, 20));
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        LayerImpl* clip_layer = scroll_layer->parent()->parent();
        clip_layer->SetBounds(gfx::Size(10, 20));

        host_impl_->BindToClient(&scroll_watcher);

        gfx::Vector2dF initial_scroll_delta(10.f, 10.f);
        scroll_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset());
        scroll_layer->SetScrollDelta(initial_scroll_delta);

        EXPECT_EQ(gfx::ScrollOffset(), scroll_watcher.last_set_scroll_offset());

        // Requesting an update results in the current scroll offset being set.
        host_impl_->RequestUpdateForSynchronousInputHandler();
        EXPECT_EQ(gfx::ScrollOffset(initial_scroll_delta),
            scroll_watcher.last_set_scroll_offset());

        // Setting the delegate results in the scrollable_size, max_scroll_offset,
        // page_scale_factor and {min|max}_page_scale_factor being set.
        EXPECT_EQ(gfx::SizeF(100, 100), scroll_watcher.scrollable_size());
        EXPECT_EQ(gfx::ScrollOffset(90, 80), scroll_watcher.max_scroll_offset());
        EXPECT_EQ(1.f, scroll_watcher.page_scale_factor());
        EXPECT_EQ(1.f, scroll_watcher.min_page_scale_factor());
        EXPECT_EQ(1.f, scroll_watcher.max_page_scale_factor());

        // Put a page scale on the tree.
        host_impl_->active_tree()->PushPageScaleFromMainThread(2.f, 0.5f, 4.f);
        EXPECT_EQ(1.f, scroll_watcher.page_scale_factor());
        EXPECT_EQ(1.f, scroll_watcher.min_page_scale_factor());
        EXPECT_EQ(1.f, scroll_watcher.max_page_scale_factor());
        // Activation will update the delegate.
        host_impl_->ActivateSyncTree();
        EXPECT_EQ(2.f, scroll_watcher.page_scale_factor());
        EXPECT_EQ(.5f, scroll_watcher.min_page_scale_factor());
        EXPECT_EQ(4.f, scroll_watcher.max_page_scale_factor());

        // Reset the page scale for the rest of the test.
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.5f, 4.f);
        EXPECT_EQ(2.f, scroll_watcher.page_scale_factor());
        EXPECT_EQ(.5f, scroll_watcher.min_page_scale_factor());
        EXPECT_EQ(4.f, scroll_watcher.max_page_scale_factor());

        // Animating page scale can change the root offset, so it should update the
        // delegate.
        host_impl_->Animate();
        EXPECT_EQ(1.f, scroll_watcher.page_scale_factor());
        EXPECT_EQ(.5f, scroll_watcher.min_page_scale_factor());
        EXPECT_EQ(4.f, scroll_watcher.max_page_scale_factor());

        // The pinch gesture doesn't put the delegate into a state where the scroll
        // offset is outside of the scroll range.  (this is verified by DCHECKs in the
        // delegate).
        host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
        host_impl_->PinchGestureBegin();
        host_impl_->PinchGestureUpdate(2.f, gfx::Point());
        host_impl_->PinchGestureUpdate(.5f, gfx::Point());
        host_impl_->PinchGestureEnd();
        host_impl_->ScrollEnd();

        // Scrolling should be relative to the offset as given by the delegate.
        gfx::Vector2dF scroll_delta(0.f, 10.f);
        gfx::ScrollOffset current_offset(7.f, 8.f);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        host_impl_->SetSynchronousInputHandlerRootScrollOffset(current_offset);

        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        EXPECT_EQ(ScrollOffsetWithDelta(current_offset, scroll_delta),
            scroll_watcher.last_set_scroll_offset());

        current_offset = gfx::ScrollOffset(42.f, 41.f);
        host_impl_->SetSynchronousInputHandlerRootScrollOffset(current_offset);
        host_impl_->ScrollBy(gfx::Point(), scroll_delta);
        EXPECT_EQ(current_offset + gfx::ScrollOffset(scroll_delta),
            scroll_watcher.last_set_scroll_offset());
        host_impl_->ScrollEnd();
        host_impl_->SetSynchronousInputHandlerRootScrollOffset(gfx::ScrollOffset());

        // Forces a full tree synchronization and ensures that the scroll delegate
        // sees the correct size of the new tree.
        gfx::Size new_size(42, 24);
        host_impl_->CreatePendingTree();
        host_impl_->pending_tree()->PushPageScaleFromMainThread(1.f, 1.f, 1.f);
        CreateScrollAndContentsLayers(host_impl_->pending_tree(), new_size);
        host_impl_->ActivateSyncTree();
        EXPECT_EQ(gfx::SizeF(new_size), scroll_watcher.scrollable_size());

        // Tear down the LayerTreeHostImpl before the InputHandlerClient.
        host_impl_.reset();
    }

    void CheckLayerScrollDelta(LayerImpl* layer, gfx::Vector2dF scroll_delta)
    {
        const gfx::Transform target_space_transform = layer->draw_properties().target_space_transform;
        EXPECT_TRUE(target_space_transform.IsScaleOrTranslation());
        gfx::Point translated_point;
        target_space_transform.TransformPoint(&translated_point);
        gfx::Point expected_point = gfx::Point() - ToRoundedVector2d(scroll_delta);
        EXPECT_EQ(expected_point.ToString(), translated_point.ToString());
    }

    TEST_F(LayerTreeHostImplTest,
        ExternalRootLayerScrollOffsetDelegationReflectedInNextDraw)
    {
        host_impl_->SetViewportSize(gfx::Size(10, 20));
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        LayerImpl* clip_layer = scroll_layer->parent()->parent();
        clip_layer->SetBounds(gfx::Size(10, 20));

        // Draw first frame to clear any pending draws and check scroll.
        DrawFrame();
        CheckLayerScrollDelta(scroll_layer, gfx::Vector2dF(0.f, 0.f));
        EXPECT_FALSE(host_impl_->active_tree()->needs_update_draw_properties());

        // Set external scroll delta on delegate and notify LayerTreeHost.
        gfx::ScrollOffset scroll_offset(10.f, 10.f);
        host_impl_->SetSynchronousInputHandlerRootScrollOffset(scroll_offset);

        // Check scroll delta reflected in layer.
        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        EXPECT_FALSE(frame.has_no_damage);
        CheckLayerScrollDelta(scroll_layer, ScrollOffsetToVector2dF(scroll_offset));
    }

    TEST_F(LayerTreeHostImplTest, OverscrollRoot)
    {
        InputHandlerScrollResult scroll_result;
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.5f, 4.f);
        DrawFrame();
        EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());

        // In-bounds scrolling does not affect overscroll.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_FALSE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());

        // Overscroll events are reflected immediately.
        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 50));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, 10), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, 10), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        // In-bounds scrolling resets accumulated overscroll for the scrolled axes.
        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -50));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_FALSE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, 0), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -10));
        EXPECT_FALSE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, -10), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, -10), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(10, 0));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_FALSE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, 0), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, -10), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(-15, 0));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(-5, 0), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(-5, -10), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 60));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, 10), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(-5, 10), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(10, -60));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, -10), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, -10), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        // Overscroll accumulates within the scope of ScrollBegin/ScrollEnd as long
        // as no scroll occurs.
        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -20));
        EXPECT_FALSE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, -20), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, -30), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -20));
        EXPECT_FALSE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, -20), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, -50), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        // Overscroll resets on valid scroll.
        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_FALSE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, 0), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, 0), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -20));
        EXPECT_TRUE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, -10), scroll_result.unused_scroll_delta);
        EXPECT_EQ(gfx::Vector2dF(0, -10), host_impl_->accumulated_root_overscroll());
        EXPECT_EQ(scroll_result.accumulated_root_overscroll,
            host_impl_->accumulated_root_overscroll());

        host_impl_->ScrollEnd();
    }

    TEST_F(LayerTreeHostImplTest, OverscrollChildWithoutBubbling)
    {
        // Scroll child layers beyond their maximum scroll range and make sure root
        // overscroll does not accumulate.
        InputHandlerScrollResult scroll_result;
        gfx::Size surface_size(10, 10);
        scoped_ptr<LayerImpl> root_clip = LayerImpl::Create(host_impl_->active_tree(), 4);
        root_clip->SetHasRenderSurface(true);

        scoped_ptr<LayerImpl> root = CreateScrollableLayer(1, surface_size, root_clip.get());

        scoped_ptr<LayerImpl> grand_child = CreateScrollableLayer(3, surface_size, root_clip.get());

        scoped_ptr<LayerImpl> child = CreateScrollableLayer(2, surface_size, root_clip.get());
        LayerImpl* grand_child_layer = grand_child.get();
        child->AddChild(grand_child.Pass());

        LayerImpl* child_layer = child.get();
        root->AddChild(child.Pass());
        root_clip->AddChild(root.Pass());
        child_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 3));
        grand_child_layer->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 2));
        host_impl_->active_tree()->SetRootLayer(root_clip.Pass());
        host_impl_->active_tree()->DidBecomeActive();
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();
        {
            gfx::Vector2d scroll_delta(0, -10);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(),
                    InputHandler::NON_BUBBLING_GESTURE));
            scroll_result = host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());
            host_impl_->ScrollEnd();

            // The next time we scroll we should only scroll the parent, but overscroll
            // should still not reach the root layer.
            scroll_delta = gfx::Vector2d(0, -30);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(5, 5),
                    InputHandler::NON_BUBBLING_GESTURE));
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child_layer);
            EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());
            scroll_result = host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), child_layer);
            EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());
            host_impl_->ScrollEnd();

            // After scrolling the parent, another scroll on the opposite direction
            // should scroll the child.
            scroll_delta = gfx::Vector2d(0, 70);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(5, 5),
                    InputHandler::NON_BUBBLING_GESTURE));
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child_layer);
            scroll_result = host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child_layer);
            EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());
            host_impl_->ScrollEnd();
        }
    }

    TEST_F(LayerTreeHostImplTest, OverscrollChildEventBubbling)
    {
        // When we try to scroll a non-scrollable child layer, the scroll delta
        // should be applied to one of its ancestors if possible. Overscroll should
        // be reflected only when it has bubbled up to the root scrolling layer.
        InputHandlerScrollResult scroll_result;
        SetupScrollAndContentsLayers(gfx::Size(20, 20));
        DrawFrame();
        {
            gfx::Vector2d scroll_delta(0, 8);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(5, 5), InputHandler::WHEEL));
            scroll_result = host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());
            scroll_result = host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_TRUE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF(0, 6), host_impl_->accumulated_root_overscroll());
            scroll_result = host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            EXPECT_FALSE(scroll_result.did_scroll);
            EXPECT_TRUE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF(0, 14), host_impl_->accumulated_root_overscroll());
            host_impl_->ScrollEnd();
        }
    }

    TEST_F(LayerTreeHostImplTest, OverscrollAlways)
    {
        InputHandlerScrollResult scroll_result;
        LayerTreeSettings settings;
        CreateHostImpl(settings, CreateOutputSurface());

        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(50, 50));
        LayerImpl* clip_layer = scroll_layer->parent()->parent();
        clip_layer->SetBounds(gfx::Size(50, 50));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        host_impl_->active_tree()->PushPageScaleFromMainThread(1.f, 0.5f, 4.f);
        DrawFrame();
        EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());

        // Even though the layer can't scroll the overscroll still happens.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10));
        EXPECT_FALSE(scroll_result.did_scroll);
        EXPECT_TRUE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(0, 10), host_impl_->accumulated_root_overscroll());
    }

    TEST_F(LayerTreeHostImplTest, NoOverscrollWhenNotAtEdge)
    {
        InputHandlerScrollResult scroll_result;
        SetupScrollAndContentsLayers(gfx::Size(200, 200));

        DrawFrame();
        {
            // Edge glow effect should be applicable only upon reaching Edges
            // of the content. unnecessary glow effect calls shouldn't be
            // called while scrolling up without reaching the edge of the content.
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::WHEEL));
            scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0, 100));
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF().ToString(),
                host_impl_->accumulated_root_overscroll().ToString());
            scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0, -2.30f));
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF().ToString(),
                host_impl_->accumulated_root_overscroll().ToString());
            host_impl_->ScrollEnd();
            // unusedrootDelta should be subtracted from applied delta so that
            // unwanted glow effect calls are not called.
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(0, 0),
                    InputHandler::NON_BUBBLING_GESTURE));
            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
            scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0, 20));
            EXPECT_TRUE(scroll_result.did_scroll);
            EXPECT_TRUE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF(0.000000f, 17.699997f).ToString(),
                host_impl_->accumulated_root_overscroll().ToString());

            scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0.02f, -0.01f));
            EXPECT_FALSE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF(0.000000f, 17.699997f).ToString(),
                host_impl_->accumulated_root_overscroll().ToString());
            host_impl_->ScrollEnd();
            // TestCase to check  kEpsilon, which prevents minute values to trigger
            // gloweffect without reaching edge.
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(0, 0), InputHandler::WHEEL));
            scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(-0.12f, 0.1f));
            EXPECT_FALSE(scroll_result.did_scroll);
            EXPECT_FALSE(scroll_result.did_overscroll_root);
            EXPECT_EQ(gfx::Vector2dF().ToString(),
                host_impl_->accumulated_root_overscroll().ToString());
            host_impl_->ScrollEnd();
        }
    }

    class BlendStateCheckLayer : public LayerImpl {
    public:
        static scoped_ptr<LayerImpl> Create(LayerTreeImpl* tree_impl,
            int id,
            ResourceProvider* resource_provider)
        {
            return make_scoped_ptr(
                new BlendStateCheckLayer(tree_impl, id, resource_provider));
        }

        void AppendQuads(RenderPass* render_pass,
            AppendQuadsData* append_quads_data) override
        {
            quads_appended_ = true;

            gfx::Rect opaque_rect;
            if (contents_opaque())
                opaque_rect = quad_rect_;
            else
                opaque_rect = opaque_content_rect_;
            gfx::Rect visible_quad_rect = quad_rect_;

            SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
            PopulateSharedQuadState(shared_quad_state);

            TileDrawQuad* test_blending_draw_quad = render_pass->CreateAndAppendDrawQuad<TileDrawQuad>();
            test_blending_draw_quad->SetNew(shared_quad_state,
                quad_rect_,
                opaque_rect,
                visible_quad_rect,
                resource_id_,
                gfx::RectF(0.f, 0.f, 1.f, 1.f),
                gfx::Size(1, 1),
                false,
                false);
            test_blending_draw_quad->visible_rect = quad_visible_rect_;
            EXPECT_EQ(blend_, test_blending_draw_quad->ShouldDrawWithBlending());
            EXPECT_EQ(has_render_surface_, !!render_surface());
        }

        void SetExpectation(bool blend, bool has_render_surface)
        {
            blend_ = blend;
            has_render_surface_ = has_render_surface;
            quads_appended_ = false;
        }

        bool quads_appended() const { return quads_appended_; }

        void SetQuadRect(const gfx::Rect& rect) { quad_rect_ = rect; }
        void SetQuadVisibleRect(const gfx::Rect& rect) { quad_visible_rect_ = rect; }
        void SetOpaqueContentRect(const gfx::Rect& rect)
        {
            opaque_content_rect_ = rect;
        }

    private:
        BlendStateCheckLayer(LayerTreeImpl* tree_impl,
            int id,
            ResourceProvider* resource_provider)
            : LayerImpl(tree_impl, id)
            , blend_(false)
            , has_render_surface_(false)
            , quads_appended_(false)
            , quad_rect_(5, 5, 5, 5)
            , quad_visible_rect_(5, 5, 5, 5)
            , resource_id_(resource_provider->CreateResource(
                  gfx::Size(1, 1),
                  GL_CLAMP_TO_EDGE,
                  ResourceProvider::TEXTURE_HINT_IMMUTABLE,
                  RGBA_8888))
        {
            resource_provider->AllocateForTesting(resource_id_);
            SetBounds(gfx::Size(10, 10));
            SetDrawsContent(true);
        }

        bool blend_;
        bool has_render_surface_;
        bool quads_appended_;
        gfx::Rect quad_rect_;
        gfx::Rect opaque_content_rect_;
        gfx::Rect quad_visible_rect_;
        ResourceId resource_id_;
    };

    TEST_F(LayerTreeHostImplTest, BlendingOffWhenDrawingOpaqueLayers)
    {
        {
            scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
            root->SetBounds(gfx::Size(10, 10));
            root->SetDrawsContent(false);
            root->SetHasRenderSurface(true);
            host_impl_->active_tree()->SetRootLayer(root.Pass());
        }
        LayerImpl* root = host_impl_->active_tree()->root_layer();

        root->AddChild(
            BlendStateCheckLayer::Create(host_impl_->active_tree(),
                2,
                host_impl_->resource_provider()));
        BlendStateCheckLayer* layer1 = static_cast<BlendStateCheckLayer*>(root->children()[0]);
        layer1->SetPosition(gfx::PointF(2.f, 2.f));

        LayerTreeHostImpl::FrameData frame;

        // Opaque layer, drawn without blending.
        layer1->SetContentsOpaque(true);
        layer1->SetExpectation(false, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Layer with translucent content and painting, so drawn with blending.
        layer1->SetContentsOpaque(false);
        layer1->SetExpectation(true, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Layer with translucent opacity, drawn with blending.
        layer1->SetContentsOpaque(true);
        layer1->SetOpacity(0.5f);
        layer1->SetExpectation(true, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Layer with translucent opacity and painting, drawn with blending.
        layer1->SetContentsOpaque(true);
        layer1->SetOpacity(0.5f);
        layer1->SetExpectation(true, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        layer1->AddChild(
            BlendStateCheckLayer::Create(host_impl_->active_tree(),
                3,
                host_impl_->resource_provider()));
        BlendStateCheckLayer* layer2 = static_cast<BlendStateCheckLayer*>(layer1->children()[0]);
        layer2->SetPosition(gfx::PointF(4.f, 4.f));

        // 2 opaque layers, drawn without blending.
        layer1->SetContentsOpaque(true);
        layer1->SetOpacity(1.f);
        layer1->SetExpectation(false, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        layer2->SetContentsOpaque(true);
        layer2->SetOpacity(1.f);
        layer2->SetExpectation(false, false);
        layer2->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        EXPECT_TRUE(layer2->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Parent layer with translucent content, drawn with blending.
        // Child layer with opaque content, drawn without blending.
        layer1->SetContentsOpaque(false);
        layer1->SetExpectation(true, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        layer2->SetExpectation(false, false);
        layer2->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        EXPECT_TRUE(layer2->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Parent layer with translucent content but opaque painting, drawn without
        // blending.
        // Child layer with opaque content, drawn without blending.
        layer1->SetContentsOpaque(true);
        layer1->SetExpectation(false, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        layer2->SetExpectation(false, false);
        layer2->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        EXPECT_TRUE(layer2->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Parent layer with translucent opacity and opaque content. Since it has a
        // drawing child, it's drawn to a render surface which carries the opacity,
        // so it's itself drawn without blending.
        // Child layer with opaque content, drawn without blending (parent surface
        // carries the inherited opacity).
        layer1->SetContentsOpaque(true);
        layer1->SetOpacity(0.5f);
        layer1->SetHasRenderSurface(true);
        layer1->SetExpectation(false, true);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        layer2->SetExpectation(false, false);
        layer2->SetUpdateRect(gfx::Rect(layer1->bounds()));
        FakeLayerTreeHostImpl::RecursiveUpdateNumChildren(
            host_impl_->active_tree()->root_layer());
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        EXPECT_TRUE(layer2->quads_appended());
        host_impl_->DidDrawAllLayers(frame);
        layer1->SetHasRenderSurface(false);

        // Draw again, but with child non-opaque, to make sure
        // layer1 not culled.
        layer1->SetContentsOpaque(true);
        layer1->SetOpacity(1.f);
        layer1->SetExpectation(false, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        layer2->SetContentsOpaque(true);
        layer2->SetOpacity(0.5f);
        layer2->SetExpectation(true, false);
        layer2->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        EXPECT_TRUE(layer2->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // A second way of making the child non-opaque.
        layer1->SetContentsOpaque(true);
        layer1->SetOpacity(1.f);
        layer1->SetExpectation(false, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        layer2->SetContentsOpaque(false);
        layer2->SetOpacity(1.f);
        layer2->SetExpectation(true, false);
        layer2->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        EXPECT_TRUE(layer2->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // And when the layer says its not opaque but is painted opaque, it is not
        // blended.
        layer1->SetContentsOpaque(true);
        layer1->SetOpacity(1.f);
        layer1->SetExpectation(false, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        layer2->SetContentsOpaque(true);
        layer2->SetOpacity(1.f);
        layer2->SetExpectation(false, false);
        layer2->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        EXPECT_TRUE(layer2->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Layer with partially opaque contents, drawn with blending.
        layer1->SetContentsOpaque(false);
        layer1->SetQuadRect(gfx::Rect(5, 5, 5, 5));
        layer1->SetQuadVisibleRect(gfx::Rect(5, 5, 5, 5));
        layer1->SetOpaqueContentRect(gfx::Rect(5, 5, 2, 5));
        layer1->SetExpectation(true, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Layer with partially opaque contents partially culled, drawn with blending.
        layer1->SetContentsOpaque(false);
        layer1->SetQuadRect(gfx::Rect(5, 5, 5, 5));
        layer1->SetQuadVisibleRect(gfx::Rect(5, 5, 5, 2));
        layer1->SetOpaqueContentRect(gfx::Rect(5, 5, 2, 5));
        layer1->SetExpectation(true, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Layer with partially opaque contents culled, drawn with blending.
        layer1->SetContentsOpaque(false);
        layer1->SetQuadRect(gfx::Rect(5, 5, 5, 5));
        layer1->SetQuadVisibleRect(gfx::Rect(7, 5, 3, 5));
        layer1->SetOpaqueContentRect(gfx::Rect(5, 5, 2, 5));
        layer1->SetExpectation(true, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);

        // Layer with partially opaque contents and translucent contents culled, drawn
        // without blending.
        layer1->SetContentsOpaque(false);
        layer1->SetQuadRect(gfx::Rect(5, 5, 5, 5));
        layer1->SetQuadVisibleRect(gfx::Rect(5, 5, 2, 5));
        layer1->SetOpaqueContentRect(gfx::Rect(5, 5, 2, 5));
        layer1->SetExpectation(false, false);
        layer1->SetUpdateRect(gfx::Rect(layer1->bounds()));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(layer1->quads_appended());
        host_impl_->DidDrawAllLayers(frame);
    }

    class LayerTreeHostImplViewportCoveredTest : public LayerTreeHostImplTest {
    protected:
        LayerTreeHostImplViewportCoveredTest()
            : gutter_quad_material_(DrawQuad::SOLID_COLOR)
            , child_(NULL)
            , did_activate_pending_tree_(false)
        {
        }

        scoped_ptr<OutputSurface> CreateFakeOutputSurface(bool always_draw)
        {
            if (always_draw) {
                return FakeOutputSurface::CreateAlwaysDrawAndSwap3d();
            }
            return FakeOutputSurface::Create3d();
        }

        void SetupActiveTreeLayers()
        {
            host_impl_->active_tree()->set_background_color(SK_ColorGRAY);
            host_impl_->active_tree()->SetRootLayer(
                LayerImpl::Create(host_impl_->active_tree(), 1));
            host_impl_->active_tree()->root_layer()->SetHasRenderSurface(true);
            host_impl_->active_tree()->root_layer()->AddChild(
                BlendStateCheckLayer::Create(host_impl_->active_tree(),
                    2,
                    host_impl_->resource_provider()));
            child_ = static_cast<BlendStateCheckLayer*>(
                host_impl_->active_tree()->root_layer()->children()[0]);
            child_->SetExpectation(false, false);
            child_->SetContentsOpaque(true);
        }

        // Expect no gutter rects.
        void TestLayerCoversFullViewport()
        {
            gfx::Rect layer_rect(viewport_size_);
            child_->SetPosition(layer_rect.origin());
            child_->SetBounds(layer_rect.size());
            child_->SetQuadRect(gfx::Rect(layer_rect.size()));
            child_->SetQuadVisibleRect(gfx::Rect(layer_rect.size()));

            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            ASSERT_EQ(1u, frame.render_passes.size());

            EXPECT_EQ(0u, CountGutterQuads(frame.render_passes[0]->quad_list));
            EXPECT_EQ(1u, frame.render_passes[0]->quad_list.size());
            ValidateTextureDrawQuads(frame.render_passes[0]->quad_list);

            VerifyQuadsExactlyCoverViewport(frame.render_passes[0]->quad_list);
            host_impl_->DidDrawAllLayers(frame);
        }

        // Expect fullscreen gutter rect.
        void TestEmptyLayer()
        {
            gfx::Rect layer_rect(0, 0, 0, 0);
            child_->SetPosition(layer_rect.origin());
            child_->SetBounds(layer_rect.size());
            child_->SetQuadRect(gfx::Rect(layer_rect.size()));
            child_->SetQuadVisibleRect(gfx::Rect(layer_rect.size()));

            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            ASSERT_EQ(1u, frame.render_passes.size());

            EXPECT_EQ(1u, CountGutterQuads(frame.render_passes[0]->quad_list));
            EXPECT_EQ(1u, frame.render_passes[0]->quad_list.size());
            ValidateTextureDrawQuads(frame.render_passes[0]->quad_list);

            VerifyQuadsExactlyCoverViewport(frame.render_passes[0]->quad_list);
            host_impl_->DidDrawAllLayers(frame);
        }

        // Expect four surrounding gutter rects.
        void TestLayerInMiddleOfViewport()
        {
            gfx::Rect layer_rect(500, 500, 200, 200);
            child_->SetPosition(layer_rect.origin());
            child_->SetBounds(layer_rect.size());
            child_->SetQuadRect(gfx::Rect(layer_rect.size()));
            child_->SetQuadVisibleRect(gfx::Rect(layer_rect.size()));

            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            ASSERT_EQ(1u, frame.render_passes.size());

            EXPECT_EQ(4u, CountGutterQuads(frame.render_passes[0]->quad_list));
            EXPECT_EQ(5u, frame.render_passes[0]->quad_list.size());
            ValidateTextureDrawQuads(frame.render_passes[0]->quad_list);

            VerifyQuadsExactlyCoverViewport(frame.render_passes[0]->quad_list);
            host_impl_->DidDrawAllLayers(frame);
        }

        // Expect no gutter rects.
        void TestLayerIsLargerThanViewport()
        {
            gfx::Rect layer_rect(viewport_size_.width() + 10,
                viewport_size_.height() + 10);
            child_->SetPosition(layer_rect.origin());
            child_->SetBounds(layer_rect.size());
            child_->SetQuadRect(gfx::Rect(layer_rect.size()));
            child_->SetQuadVisibleRect(gfx::Rect(layer_rect.size()));

            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            ASSERT_EQ(1u, frame.render_passes.size());

            EXPECT_EQ(0u, CountGutterQuads(frame.render_passes[0]->quad_list));
            EXPECT_EQ(1u, frame.render_passes[0]->quad_list.size());
            ValidateTextureDrawQuads(frame.render_passes[0]->quad_list);

            host_impl_->DidDrawAllLayers(frame);
        }

        void DidActivateSyncTree() override { did_activate_pending_tree_ = true; }

        void set_gutter_quad_material(DrawQuad::Material material)
        {
            gutter_quad_material_ = material;
        }
        void set_gutter_texture_size(const gfx::Size& gutter_texture_size)
        {
            gutter_texture_size_ = gutter_texture_size;
        }

    protected:
        size_t CountGutterQuads(const QuadList& quad_list)
        {
            size_t num_gutter_quads = 0;
            for (const auto& quad : quad_list) {
                num_gutter_quads += (quad->material == gutter_quad_material_) ? 1 : 0;
            }
            return num_gutter_quads;
        }

        void VerifyQuadsExactlyCoverViewport(const QuadList& quad_list)
        {
            LayerTestCommon::VerifyQuadsExactlyCoverRect(
                quad_list, gfx::Rect(DipSizeToPixelSize(viewport_size_)));
        }

        // Make sure that the texture coordinates match their expectations.
        void ValidateTextureDrawQuads(const QuadList& quad_list)
        {
            for (const auto& quad : quad_list) {
                if (quad->material != DrawQuad::TEXTURE_CONTENT)
                    continue;
                const TextureDrawQuad* texture_quad = TextureDrawQuad::MaterialCast(quad);
                gfx::SizeF gutter_texture_size_pixels = gfx::ScaleSize(gfx::SizeF(gutter_texture_size_),
                    host_impl_->active_tree()->device_scale_factor());
                EXPECT_EQ(texture_quad->uv_top_left.x(),
                    texture_quad->rect.x() / gutter_texture_size_pixels.width());
                EXPECT_EQ(texture_quad->uv_top_left.y(),
                    texture_quad->rect.y() / gutter_texture_size_pixels.height());
                EXPECT_EQ(
                    texture_quad->uv_bottom_right.x(),
                    texture_quad->rect.right() / gutter_texture_size_pixels.width());
                EXPECT_EQ(
                    texture_quad->uv_bottom_right.y(),
                    texture_quad->rect.bottom() / gutter_texture_size_pixels.height());
            }
        }

        gfx::Size DipSizeToPixelSize(const gfx::Size& size)
        {
            return gfx::ScaleToRoundedSize(
                size, host_impl_->active_tree()->device_scale_factor());
        }

        DrawQuad::Material gutter_quad_material_;
        gfx::Size gutter_texture_size_;
        gfx::Size viewport_size_;
        BlendStateCheckLayer* child_;
        bool did_activate_pending_tree_;
    };

    TEST_F(LayerTreeHostImplViewportCoveredTest, ViewportCovered)
    {
        viewport_size_ = gfx::Size(1000, 1000);

        bool always_draw = false;
        CreateHostImpl(DefaultSettings(), CreateFakeOutputSurface(always_draw));

        host_impl_->SetViewportSize(DipSizeToPixelSize(viewport_size_));
        SetupActiveTreeLayers();
        TestLayerCoversFullViewport();
        TestEmptyLayer();
        TestLayerInMiddleOfViewport();
        TestLayerIsLargerThanViewport();
    }

    TEST_F(LayerTreeHostImplViewportCoveredTest, ViewportCoveredScaled)
    {
        viewport_size_ = gfx::Size(1000, 1000);

        bool always_draw = false;
        CreateHostImpl(DefaultSettings(), CreateFakeOutputSurface(always_draw));

        host_impl_->active_tree()->SetDeviceScaleFactor(2.f);
        host_impl_->SetViewportSize(DipSizeToPixelSize(viewport_size_));
        SetupActiveTreeLayers();
        TestLayerCoversFullViewport();
        TestEmptyLayer();
        TestLayerInMiddleOfViewport();
        TestLayerIsLargerThanViewport();
    }

    TEST_F(LayerTreeHostImplViewportCoveredTest, ActiveTreeGrowViewportInvalid)
    {
        viewport_size_ = gfx::Size(1000, 1000);

        bool always_draw = true;
        CreateHostImpl(DefaultSettings(), CreateFakeOutputSurface(always_draw));

        // Pending tree to force active_tree size invalid. Not used otherwise.
        host_impl_->CreatePendingTree();
        host_impl_->SetViewportSize(DipSizeToPixelSize(viewport_size_));
        EXPECT_TRUE(host_impl_->active_tree()->ViewportSizeInvalid());

        SetupActiveTreeLayers();
        TestEmptyLayer();
        TestLayerInMiddleOfViewport();
        TestLayerIsLargerThanViewport();
    }

    TEST_F(LayerTreeHostImplViewportCoveredTest, ActiveTreeShrinkViewportInvalid)
    {
        viewport_size_ = gfx::Size(1000, 1000);

        bool always_draw = true;
        CreateHostImpl(DefaultSettings(), CreateFakeOutputSurface(always_draw));

        // Set larger viewport and activate it to active tree.
        host_impl_->CreatePendingTree();
        gfx::Size larger_viewport(viewport_size_.width() + 100,
            viewport_size_.height() + 100);
        host_impl_->SetViewportSize(DipSizeToPixelSize(larger_viewport));
        EXPECT_TRUE(host_impl_->active_tree()->ViewportSizeInvalid());
        host_impl_->ActivateSyncTree();
        EXPECT_TRUE(did_activate_pending_tree_);
        EXPECT_FALSE(host_impl_->active_tree()->ViewportSizeInvalid());

        // Shrink pending tree viewport without activating.
        host_impl_->CreatePendingTree();
        host_impl_->SetViewportSize(DipSizeToPixelSize(viewport_size_));
        EXPECT_TRUE(host_impl_->active_tree()->ViewportSizeInvalid());

        SetupActiveTreeLayers();
        TestEmptyLayer();
        TestLayerInMiddleOfViewport();
        TestLayerIsLargerThanViewport();
    }

    class FakeDrawableLayerImpl : public LayerImpl {
    public:
        static scoped_ptr<LayerImpl> Create(LayerTreeImpl* tree_impl, int id)
        {
            return make_scoped_ptr(new FakeDrawableLayerImpl(tree_impl, id));
        }

    protected:
        FakeDrawableLayerImpl(LayerTreeImpl* tree_impl, int id)
            : LayerImpl(tree_impl, id)
        {
        }
    };

    // Only reshape when we know we are going to draw. Otherwise, the reshape
    // can leave the window at the wrong size if we never draw and the proper
    // viewport size is never set.
    TEST_F(LayerTreeHostImplTest, ReshapeNotCalledUntilDraw)
    {
        scoped_refptr<TestContextProvider> provider(TestContextProvider::Create());
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(provider));
        CreateHostImpl(DefaultSettings(), output_surface.Pass());

        scoped_ptr<LayerImpl> root = FakeDrawableLayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetBounds(gfx::Size(10, 10));
        root->SetDrawsContent(true);
        root->SetHasRenderSurface(true);
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        EXPECT_FALSE(provider->TestContext3d()->reshape_called());
        provider->TestContext3d()->clear_reshape_called();

        LayerTreeHostImpl::FrameData frame;
        host_impl_->SetViewportSize(gfx::Size(10, 10));
        host_impl_->active_tree()->SetDeviceScaleFactor(1.f);
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(provider->TestContext3d()->reshape_called());
        EXPECT_EQ(provider->TestContext3d()->width(), 10);
        EXPECT_EQ(provider->TestContext3d()->height(), 10);
        EXPECT_EQ(provider->TestContext3d()->scale_factor(), 1.f);
        host_impl_->DidDrawAllLayers(frame);
        provider->TestContext3d()->clear_reshape_called();

        host_impl_->SetViewportSize(gfx::Size(20, 30));
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(provider->TestContext3d()->reshape_called());
        EXPECT_EQ(provider->TestContext3d()->width(), 20);
        EXPECT_EQ(provider->TestContext3d()->height(), 30);
        EXPECT_EQ(provider->TestContext3d()->scale_factor(), 1.f);
        host_impl_->DidDrawAllLayers(frame);
        provider->TestContext3d()->clear_reshape_called();

        host_impl_->active_tree()->SetDeviceScaleFactor(2.f);
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        EXPECT_TRUE(provider->TestContext3d()->reshape_called());
        EXPECT_EQ(provider->TestContext3d()->width(), 20);
        EXPECT_EQ(provider->TestContext3d()->height(), 30);
        EXPECT_EQ(provider->TestContext3d()->scale_factor(), 2.f);
        host_impl_->DidDrawAllLayers(frame);
        provider->TestContext3d()->clear_reshape_called();
    }

    // Make sure damage tracking propagates all the way to the graphics context,
    // where it should request to swap only the sub-buffer that is damaged.
    TEST_F(LayerTreeHostImplTest, PartialSwapReceivesDamageRect)
    {
        scoped_refptr<TestContextProvider> context_provider(
            TestContextProvider::Create());
        context_provider->BindToCurrentThread();
        context_provider->TestContext3d()->set_have_post_sub_buffer(true);

        scoped_ptr<FakeOutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_provider));
        FakeOutputSurface* fake_output_surface = output_surface.get();

        // This test creates its own LayerTreeHostImpl, so
        // that we can force partial swap enabled.
        LayerTreeSettings settings;
        settings.renderer_settings.partial_swap_enabled = true;
        scoped_ptr<LayerTreeHostImpl> layer_tree_host_impl = LayerTreeHostImpl::Create(
            settings, this, &proxy_, &stats_instrumentation_,
            &shared_bitmap_manager_, NULL, &task_graph_runner_, 0);
        layer_tree_host_impl->InitializeRenderer(output_surface.get());
        layer_tree_host_impl->WillBeginImplFrame(
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE));
        layer_tree_host_impl->SetViewportSize(gfx::Size(500, 500));

        scoped_ptr<LayerImpl> root = FakeDrawableLayerImpl::Create(layer_tree_host_impl->active_tree(), 1);
        root->SetHasRenderSurface(true);
        scoped_ptr<LayerImpl> child = FakeDrawableLayerImpl::Create(layer_tree_host_impl->active_tree(), 2);
        child->SetPosition(gfx::PointF(12.f, 13.f));
        child->SetBounds(gfx::Size(14, 15));
        child->SetDrawsContent(true);
        root->SetBounds(gfx::Size(500, 500));
        root->SetDrawsContent(true);
        root->AddChild(child.Pass());
        layer_tree_host_impl->active_tree()->SetRootLayer(root.Pass());

        LayerTreeHostImpl::FrameData frame;

        // First frame, the entire screen should get swapped.
        EXPECT_EQ(DRAW_SUCCESS, layer_tree_host_impl->PrepareToDraw(&frame));
        layer_tree_host_impl->DrawLayers(&frame);
        layer_tree_host_impl->DidDrawAllLayers(frame);
        layer_tree_host_impl->SwapBuffers(frame);
        gfx::Rect expected_swap_rect(0, 0, 500, 500);
        EXPECT_EQ(expected_swap_rect.ToString(),
            fake_output_surface->last_swap_rect().ToString());

        // Second frame, only the damaged area should get swapped. Damage should be
        // the union of old and new child rects.
        // expected damage rect: gfx::Rect(26, 28);
        // expected swap rect: vertically flipped, with origin at bottom left corner.
        layer_tree_host_impl->active_tree()->root_layer()->children()[0]->SetPosition(
            gfx::PointF());
        EXPECT_EQ(DRAW_SUCCESS, layer_tree_host_impl->PrepareToDraw(&frame));
        layer_tree_host_impl->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        layer_tree_host_impl->SwapBuffers(frame);

        // Make sure that partial swap is constrained to the viewport dimensions
        // expected damage rect: gfx::Rect(500, 500);
        // expected swap rect: flipped damage rect, but also clamped to viewport
        expected_swap_rect = gfx::Rect(0, 500 - 28, 26, 28);
        EXPECT_EQ(expected_swap_rect.ToString(),
            fake_output_surface->last_swap_rect().ToString());

        layer_tree_host_impl->SetViewportSize(gfx::Size(10, 10));
        // This will damage everything.
        layer_tree_host_impl->active_tree()->root_layer()->SetBackgroundColor(
            SK_ColorBLACK);
        EXPECT_EQ(DRAW_SUCCESS, layer_tree_host_impl->PrepareToDraw(&frame));
        layer_tree_host_impl->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        layer_tree_host_impl->SwapBuffers(frame);

        expected_swap_rect = gfx::Rect(0, 0, 10, 10);
        EXPECT_EQ(expected_swap_rect.ToString(),
            fake_output_surface->last_swap_rect().ToString());
    }

    TEST_F(LayerTreeHostImplTest, RootLayerDoesntCreateExtraSurface)
    {
        scoped_ptr<LayerImpl> root = FakeDrawableLayerImpl::Create(host_impl_->active_tree(), 1);
        scoped_ptr<LayerImpl> child = FakeDrawableLayerImpl::Create(host_impl_->active_tree(), 2);
        child->SetBounds(gfx::Size(10, 10));
        child->SetDrawsContent(true);
        root->SetBounds(gfx::Size(10, 10));
        root->SetDrawsContent(true);
        root->SetHasRenderSurface(true);
        root->AddChild(child.Pass());

        host_impl_->active_tree()->SetRootLayer(root.Pass());

        LayerTreeHostImpl::FrameData frame;

        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        EXPECT_EQ(1u, frame.render_surface_layer_list->size());
        EXPECT_EQ(1u, frame.render_passes.size());
        host_impl_->DidDrawAllLayers(frame);
    }

    class FakeLayerWithQuads : public LayerImpl {
    public:
        static scoped_ptr<LayerImpl> Create(LayerTreeImpl* tree_impl, int id)
        {
            return make_scoped_ptr(new FakeLayerWithQuads(tree_impl, id));
        }

        void AppendQuads(RenderPass* render_pass,
            AppendQuadsData* append_quads_data) override
        {
            SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
            PopulateSharedQuadState(shared_quad_state);

            SkColor gray = SkColorSetRGB(100, 100, 100);
            gfx::Rect quad_rect(bounds());
            gfx::Rect visible_quad_rect(quad_rect);
            SolidColorDrawQuad* my_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            my_quad->SetNew(
                shared_quad_state, quad_rect, visible_quad_rect, gray, false);
        }

    private:
        FakeLayerWithQuads(LayerTreeImpl* tree_impl, int id)
            : LayerImpl(tree_impl, id)
        {
        }
    };

    class MockContext : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD1(useProgram, void(GLuint program));
        MOCK_METHOD5(uniform4f, void(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w));
        MOCK_METHOD4(uniformMatrix4fv, void(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
        MOCK_METHOD4(drawElements, void(GLenum mode, GLsizei count, GLenum type, GLintptr offset));
        MOCK_METHOD1(enable, void(GLenum cap));
        MOCK_METHOD1(disable, void(GLenum cap));
        MOCK_METHOD4(scissor, void(GLint x, GLint y, GLsizei width, GLsizei height));
    };

    class MockContextHarness {
    private:
        MockContext* context_;

    public:
        explicit MockContextHarness(MockContext* context)
            : context_(context)
        {
            context_->set_have_post_sub_buffer(true);

            // Catch "uninteresting" calls
            EXPECT_CALL(*context_, useProgram(_))
                .Times(0);

            EXPECT_CALL(*context_, drawElements(_, _, _, _))
                .Times(0);

            // These are not asserted
            EXPECT_CALL(*context_, uniformMatrix4fv(_, _, _, _))
                .WillRepeatedly(Return());

            EXPECT_CALL(*context_, uniform4f(_, _, _, _, _))
                .WillRepeatedly(Return());

            // Any un-sanctioned calls to enable() are OK
            EXPECT_CALL(*context_, enable(_))
                .WillRepeatedly(Return());

            // Any un-sanctioned calls to disable() are OK
            EXPECT_CALL(*context_, disable(_))
                .WillRepeatedly(Return());
        }

        void MustDrawSolidQuad()
        {
            EXPECT_CALL(*context_, drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0))
                .WillOnce(Return())
                .RetiresOnSaturation();

            EXPECT_CALL(*context_, useProgram(_))
                .WillOnce(Return())
                .RetiresOnSaturation();
        }

        void MustSetScissor(int x, int y, int width, int height)
        {
            EXPECT_CALL(*context_, enable(GL_SCISSOR_TEST))
                .WillRepeatedly(Return());

            EXPECT_CALL(*context_, scissor(x, y, width, height))
                .Times(AtLeast(1))
                .WillRepeatedly(Return());
        }

        void MustSetNoScissor()
        {
            EXPECT_CALL(*context_, disable(GL_SCISSOR_TEST))
                .WillRepeatedly(Return());

            EXPECT_CALL(*context_, enable(GL_SCISSOR_TEST))
                .Times(0);

            EXPECT_CALL(*context_, scissor(_, _, _, _))
                .Times(0);
        }
    };

    TEST_F(LayerTreeHostImplTest, NoPartialSwap)
    {
        scoped_ptr<MockContext> mock_context_owned(new MockContext);
        MockContext* mock_context = mock_context_owned.get();
        MockContextHarness harness(mock_context);

        // Run test case
        LayerTreeSettings settings = DefaultSettings();
        settings.renderer_settings.partial_swap_enabled = false;
        CreateHostImpl(settings,
            FakeOutputSurface::Create3d(mock_context_owned.Pass()));
        SetupRootLayerImpl(FakeLayerWithQuads::Create(host_impl_->active_tree(), 1));

        // Without partial swap, and no clipping, no scissor is set.
        harness.MustDrawSolidQuad();
        harness.MustSetNoScissor();
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
        }
        Mock::VerifyAndClearExpectations(&mock_context);

        // Without partial swap, but a layer does clip its subtree, one scissor is
        // set.
        host_impl_->active_tree()->root_layer()->SetMasksToBounds(true);
        harness.MustDrawSolidQuad();
        harness.MustSetScissor(0, 0, 10, 10);
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
        }
        Mock::VerifyAndClearExpectations(&mock_context);
    }

    TEST_F(LayerTreeHostImplTest, PartialSwap)
    {
        scoped_ptr<MockContext> context_owned(new MockContext);
        MockContext* mock_context = context_owned.get();
        MockContextHarness harness(mock_context);

        LayerTreeSettings settings = DefaultSettings();
        settings.renderer_settings.partial_swap_enabled = true;
        CreateHostImpl(settings, FakeOutputSurface::Create3d(context_owned.Pass()));
        SetupRootLayerImpl(FakeLayerWithQuads::Create(host_impl_->active_tree(), 1));

        // The first frame is not a partially-swapped one. No scissor should be set.
        harness.MustSetNoScissor();
        harness.MustDrawSolidQuad();
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
        }
        Mock::VerifyAndClearExpectations(&mock_context);

        // Damage a portion of the frame.
        host_impl_->active_tree()->root_layer()->SetUpdateRect(
            gfx::Rect(0, 0, 2, 3));

        // The second frame will be partially-swapped (the y coordinates are flipped).
        harness.MustSetScissor(0, 7, 2, 3);
        harness.MustDrawSolidQuad();
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
        }
        Mock::VerifyAndClearExpectations(&mock_context);
    }

    static scoped_ptr<LayerTreeHostImpl> SetupLayersForOpacity(
        bool partial_swap,
        LayerTreeHostImplClient* client,
        Proxy* proxy,
        SharedBitmapManager* manager,
        TaskGraphRunner* task_graph_runner,
        RenderingStatsInstrumentation* stats_instrumentation,
        OutputSurface* output_surface)
    {
        LayerTreeSettings settings;
        settings.renderer_settings.partial_swap_enabled = partial_swap;
        scoped_ptr<LayerTreeHostImpl> my_host_impl = LayerTreeHostImpl::Create(settings, client, proxy, stats_instrumentation,
            manager, nullptr, task_graph_runner, 0);
        my_host_impl->InitializeRenderer(output_surface);
        my_host_impl->WillBeginImplFrame(
            CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE));
        my_host_impl->SetViewportSize(gfx::Size(100, 100));

        /*
    Layers are created as follows:

    +--------------------+
    |                  1 |
    |  +-----------+     |
    |  |         2 |     |
    |  | +-------------------+
    |  | |   3               |
    |  | +-------------------+
    |  |           |     |
    |  +-----------+     |
    |                    |
    |                    |
    +--------------------+

    Layers 1, 2 have render surfaces
  */
        scoped_ptr<LayerImpl> root = LayerImpl::Create(my_host_impl->active_tree(), 1);
        scoped_ptr<LayerImpl> child = LayerImpl::Create(my_host_impl->active_tree(), 2);
        scoped_ptr<LayerImpl> grand_child = FakeLayerWithQuads::Create(my_host_impl->active_tree(), 3);

        gfx::Rect root_rect(0, 0, 100, 100);
        gfx::Rect child_rect(10, 10, 50, 50);
        gfx::Rect grand_child_rect(5, 5, 150, 150);

        root->SetHasRenderSurface(true);
        root->SetPosition(root_rect.origin());
        root->SetBounds(root_rect.size());
        root->draw_properties().visible_layer_rect = root_rect;
        root->SetDrawsContent(false);
        root->render_surface()->SetContentRect(gfx::Rect(root_rect.size()));

        child->SetPosition(gfx::PointF(child_rect.x(), child_rect.y()));
        child->SetOpacity(0.5f);
        child->SetBounds(gfx::Size(child_rect.width(), child_rect.height()));
        child->draw_properties().visible_layer_rect = child_rect;
        child->SetDrawsContent(false);
        child->SetHasRenderSurface(true);

        grand_child->SetPosition(grand_child_rect.origin());
        grand_child->SetBounds(grand_child_rect.size());
        grand_child->draw_properties().visible_layer_rect = grand_child_rect;
        grand_child->SetDrawsContent(true);

        child->AddChild(grand_child.Pass());
        root->AddChild(child.Pass());

        my_host_impl->active_tree()->SetRootLayer(root.Pass());
        return my_host_impl.Pass();
    }

    TEST_F(LayerTreeHostImplTest, ContributingLayerEmptyScissorPartialSwap)
    {
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        scoped_refptr<TestContextProvider> provider(TestContextProvider::Create());
        provider->BindToCurrentThread();
        provider->TestContext3d()->set_have_post_sub_buffer(true);
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(provider));
        scoped_ptr<LayerTreeHostImpl> my_host_impl = SetupLayersForOpacity(
            true, this, &proxy_, &shared_bitmap_manager, &task_graph_runner,
            &stats_instrumentation_, output_surface.get());
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, my_host_impl->PrepareToDraw(&frame));

            // Verify all quads have been computed
            ASSERT_EQ(2U, frame.render_passes.size());
            ASSERT_EQ(1U, frame.render_passes[0]->quad_list.size());
            ASSERT_EQ(1U, frame.render_passes[1]->quad_list.size());
            EXPECT_EQ(DrawQuad::SOLID_COLOR,
                frame.render_passes[0]->quad_list.front()->material);
            EXPECT_EQ(DrawQuad::RENDER_PASS,
                frame.render_passes[1]->quad_list.front()->material);

            my_host_impl->DrawLayers(&frame);
            my_host_impl->DidDrawAllLayers(frame);
        }
    }

    TEST_F(LayerTreeHostImplTest, ContributingLayerEmptyScissorNoPartialSwap)
    {
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        scoped_refptr<TestContextProvider> provider(TestContextProvider::Create());
        provider->BindToCurrentThread();
        provider->TestContext3d()->set_have_post_sub_buffer(true);
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(provider));
        scoped_ptr<LayerTreeHostImpl> my_host_impl = SetupLayersForOpacity(
            false, this, &proxy_, &shared_bitmap_manager, &task_graph_runner,
            &stats_instrumentation_, output_surface.get());
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, my_host_impl->PrepareToDraw(&frame));

            // Verify all quads have been computed
            ASSERT_EQ(2U, frame.render_passes.size());
            ASSERT_EQ(1U, frame.render_passes[0]->quad_list.size());
            ASSERT_EQ(1U, frame.render_passes[1]->quad_list.size());
            EXPECT_EQ(DrawQuad::SOLID_COLOR,
                frame.render_passes[0]->quad_list.front()->material);
            EXPECT_EQ(DrawQuad::RENDER_PASS,
                frame.render_passes[1]->quad_list.front()->material);

            my_host_impl->DrawLayers(&frame);
            my_host_impl->DidDrawAllLayers(frame);
        }
    }

    TEST_F(LayerTreeHostImplTest, LayersFreeTextures)
    {
        scoped_ptr<TestWebGraphicsContext3D> context = TestWebGraphicsContext3D::Create();
        TestWebGraphicsContext3D* context3d = context.get();
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context.Pass()));
        CreateHostImpl(DefaultSettings(), output_surface.Pass());

        scoped_ptr<LayerImpl> root_layer = LayerImpl::Create(host_impl_->active_tree(), 1);
        root_layer->SetBounds(gfx::Size(10, 10));
        root_layer->SetHasRenderSurface(true);

        scoped_refptr<VideoFrame> softwareFrame = media::VideoFrame::CreateColorFrame(
            gfx::Size(4, 4), 0x80, 0x80, 0x80, base::TimeDelta());
        FakeVideoFrameProvider provider;
        provider.set_frame(softwareFrame);
        scoped_ptr<VideoLayerImpl> video_layer = VideoLayerImpl::Create(
            host_impl_->active_tree(), 4, &provider, media::VIDEO_ROTATION_0);
        video_layer->SetBounds(gfx::Size(10, 10));
        video_layer->SetDrawsContent(true);
        root_layer->AddChild(video_layer.Pass());

        scoped_ptr<IOSurfaceLayerImpl> io_surface_layer = IOSurfaceLayerImpl::Create(host_impl_->active_tree(), 5);
        io_surface_layer->SetBounds(gfx::Size(10, 10));
        io_surface_layer->SetDrawsContent(true);
        io_surface_layer->SetIOSurfaceProperties(1, gfx::Size(10, 10));
        root_layer->AddChild(io_surface_layer.Pass());

        host_impl_->active_tree()->SetRootLayer(root_layer.Pass());

        EXPECT_EQ(0u, context3d->NumTextures());

        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        host_impl_->SwapBuffers(frame);

        EXPECT_GT(context3d->NumTextures(), 0u);

        // Kill the layer tree.
        host_impl_->active_tree()->SetRootLayer(
            LayerImpl::Create(host_impl_->active_tree(), 100));
        // There should be no textures left in use after.
        EXPECT_EQ(0u, context3d->NumTextures());
    }

    class MockDrawQuadsToFillScreenContext : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD1(useProgram, void(GLuint program));
        MOCK_METHOD4(drawElements, void(GLenum mode, GLsizei count, GLenum type, GLintptr offset));
    };

    TEST_F(LayerTreeHostImplTest, HasTransparentBackground)
    {
        scoped_ptr<MockDrawQuadsToFillScreenContext> mock_context_owned(
            new MockDrawQuadsToFillScreenContext);
        MockDrawQuadsToFillScreenContext* mock_context = mock_context_owned.get();

        // Run test case
        LayerTreeSettings settings = DefaultSettings();
        settings.renderer_settings.partial_swap_enabled = false;
        CreateHostImpl(settings,
            FakeOutputSurface::Create3d(mock_context_owned.Pass()));
        SetupRootLayerImpl(LayerImpl::Create(host_impl_->active_tree(), 1));
        host_impl_->active_tree()->set_background_color(SK_ColorWHITE);

        // Verify one quad is drawn when transparent background set is not set.
        host_impl_->active_tree()->set_has_transparent_background(false);
        EXPECT_CALL(*mock_context, useProgram(_))
            .Times(1);
        EXPECT_CALL(*mock_context, drawElements(_, _, _, _))
            .Times(1);
        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        Mock::VerifyAndClearExpectations(&mock_context);

        // Verify no quads are drawn when transparent background is set.
        host_impl_->active_tree()->set_has_transparent_background(true);
        host_impl_->SetFullRootLayerDamage();
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        Mock::VerifyAndClearExpectations(&mock_context);
    }

    class LayerTreeHostImplTestWithDelegatingRenderer
        : public LayerTreeHostImplTest {
    protected:
        scoped_ptr<OutputSurface> CreateOutputSurface() override
        {
            return FakeOutputSurface::CreateDelegating3d();
        }

        void DrawFrameAndTestDamage(const gfx::Rect& expected_damage)
        {
            bool expect_to_draw = !expected_damage.IsEmpty();

            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));

            if (!expect_to_draw) {
                // With no damage, we don't draw, and no quads are created.
                ASSERT_EQ(0u, frame.render_passes.size());
            } else {
                ASSERT_EQ(1u, frame.render_passes.size());

                // Verify the damage rect for the root render pass.
                const RenderPass* root_render_pass = frame.render_passes.back();
                EXPECT_EQ(expected_damage, root_render_pass->damage_rect);

                // Verify the root and child layers' quads are generated and not being
                // culled.
                ASSERT_EQ(2u, root_render_pass->quad_list.size());

                LayerImpl* child = host_impl_->active_tree()->root_layer()->children()[0];
                gfx::Rect expected_child_visible_rect(child->bounds());
                EXPECT_EQ(expected_child_visible_rect,
                    root_render_pass->quad_list.front()->visible_rect);

                LayerImpl* root = host_impl_->active_tree()->root_layer();
                gfx::Rect expected_root_visible_rect(root->bounds());
                EXPECT_EQ(expected_root_visible_rect,
                    root_render_pass->quad_list.ElementAt(1)->visible_rect);
            }

            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
            EXPECT_EQ(expect_to_draw, host_impl_->SwapBuffers(frame));
        }
    };

    TEST_F(LayerTreeHostImplTestWithDelegatingRenderer, FrameIncludesDamageRect)
    {
        scoped_ptr<SolidColorLayerImpl> root = SolidColorLayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetPosition(gfx::PointF());
        root->SetBounds(gfx::Size(10, 10));
        root->SetDrawsContent(true);
        root->SetHasRenderSurface(true);

        // Child layer is in the bottom right corner.
        scoped_ptr<SolidColorLayerImpl> child = SolidColorLayerImpl::Create(host_impl_->active_tree(), 2);
        child->SetPosition(gfx::PointF(9.f, 9.f));
        child->SetBounds(gfx::Size(1, 1));
        child->SetDrawsContent(true);
        root->AddChild(child.Pass());

        host_impl_->active_tree()->SetRootLayer(root.Pass());

        // Draw a frame. In the first frame, the entire viewport should be damaged.
        gfx::Rect full_frame_damage(host_impl_->DrawViewportSize());
        DrawFrameAndTestDamage(full_frame_damage);

        // The second frame has damage that doesn't touch the child layer. Its quads
        // should still be generated.
        gfx::Rect small_damage = gfx::Rect(0, 0, 1, 1);
        host_impl_->active_tree()->root_layer()->SetUpdateRect(small_damage);
        DrawFrameAndTestDamage(small_damage);

        // The third frame should have no damage, so no quads should be generated.
        gfx::Rect no_damage;
        DrawFrameAndTestDamage(no_damage);
    }

    // TODO(reveman): Remove this test and the ability to prevent on demand raster
    // when delegating renderer supports PictureDrawQuads. crbug.com/342121
    TEST_F(LayerTreeHostImplTestWithDelegatingRenderer, PreventRasterizeOnDemand)
    {
        LayerTreeSettings settings;
        CreateHostImpl(settings, CreateOutputSurface());
        EXPECT_FALSE(host_impl_->GetRendererCapabilities().allow_rasterize_on_demand);
    }

    class FakeMaskLayerImpl : public LayerImpl {
    public:
        static scoped_ptr<FakeMaskLayerImpl> Create(LayerTreeImpl* tree_impl,
            int id)
        {
            return make_scoped_ptr(new FakeMaskLayerImpl(tree_impl, id));
        }

        void GetContentsResourceId(ResourceId* resource_id,
            gfx::Size* resource_size) const override
        {
            *resource_id = 0;
        }

    private:
        FakeMaskLayerImpl(LayerTreeImpl* tree_impl, int id)
            : LayerImpl(tree_impl, id)
        {
        }
    };

    class GLRendererWithSetupQuadForAntialiasing : public GLRenderer {
    public:
        using GLRenderer::ShouldAntialiasQuad;
    };

    TEST_F(LayerTreeHostImplTest, FarAwayQuadsDontNeedAA)
    {
        // Due to precision issues (especially on Android), sometimes far
        // away quads can end up thinking they need AA.
        float device_scale_factor = 4.f / 3.f;
        gfx::Size root_size(2000, 1000);
        gfx::Size device_viewport_size = gfx::ScaleToCeiledSize(root_size, device_scale_factor);
        host_impl_->SetViewportSize(device_viewport_size);

        host_impl_->CreatePendingTree();
        host_impl_->pending_tree()->SetDeviceScaleFactor(device_scale_factor);
        host_impl_->pending_tree()->PushPageScaleFromMainThread(1.f, 1.f / 16.f,
            16.f);

        scoped_ptr<LayerImpl> scoped_root = LayerImpl::Create(host_impl_->pending_tree(), 1);
        LayerImpl* root = scoped_root.get();
        root->SetHasRenderSurface(true);

        host_impl_->pending_tree()->SetRootLayer(scoped_root.Pass());

        scoped_ptr<LayerImpl> scoped_scrolling_layer = LayerImpl::Create(host_impl_->pending_tree(), 2);
        LayerImpl* scrolling_layer = scoped_scrolling_layer.get();
        root->AddChild(scoped_scrolling_layer.Pass());

        gfx::Size content_layer_bounds(100000, 100);
        scoped_refptr<FakeDisplayListRasterSource> raster_source(
            FakeDisplayListRasterSource::CreateFilled(content_layer_bounds));

        scoped_ptr<FakePictureLayerImpl> scoped_content_layer = FakePictureLayerImpl::CreateWithRasterSource(host_impl_->pending_tree(),
            3, raster_source);
        LayerImpl* content_layer = scoped_content_layer.get();
        scrolling_layer->AddChild(scoped_content_layer.Pass());
        content_layer->SetBounds(content_layer_bounds);
        content_layer->SetDrawsContent(true);

        root->SetBounds(root_size);

        gfx::ScrollOffset scroll_offset(100000, 0);
        scrolling_layer->SetScrollClipLayer(root->id());
        scrolling_layer->PushScrollOffsetFromMainThread(scroll_offset);

        host_impl_->pending_tree()->BuildPropertyTreesForTesting();
        host_impl_->ActivateSyncTree();

        bool update_lcd_text = false;
        host_impl_->active_tree()->UpdateDrawProperties(update_lcd_text);
        ASSERT_EQ(1u, host_impl_->active_tree()->RenderSurfaceLayerList().size());

        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));

        ASSERT_EQ(1u, frame.render_passes.size());
        ASSERT_LE(1u, frame.render_passes[0]->quad_list.size());
        const DrawQuad* quad = frame.render_passes[0]->quad_list.front();

        bool clipped = false, force_aa = false;
        gfx::QuadF device_layer_quad = MathUtil::MapQuad(
            quad->shared_quad_state->quad_to_target_transform,
            gfx::QuadF(gfx::RectF(quad->shared_quad_state->visible_quad_layer_rect)),
            &clipped);
        EXPECT_FALSE(clipped);
        bool antialiased = GLRendererWithSetupQuadForAntialiasing::ShouldAntialiasQuad(
            device_layer_quad, clipped, force_aa);
        EXPECT_FALSE(antialiased);

        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
    }

    class CompositorFrameMetadataTest : public LayerTreeHostImplTest {
    public:
        CompositorFrameMetadataTest()
            : swap_buffers_complete_(0)
        {
        }

        void DidSwapBuffersCompleteOnImplThread() override
        {
            swap_buffers_complete_++;
        }

        int swap_buffers_complete_;
    };

    TEST_F(CompositorFrameMetadataTest, CompositorFrameAckCountsAsSwapComplete)
    {
        SetupRootLayerImpl(FakeLayerWithQuads::Create(host_impl_->active_tree(), 1));
        {
            LayerTreeHostImpl::FrameData frame;
            EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
            host_impl_->DrawLayers(&frame);
            host_impl_->DidDrawAllLayers(frame);
        }
        CompositorFrameAck ack;
        host_impl_->ReclaimResources(&ack);
        host_impl_->DidSwapBuffersComplete();
        EXPECT_EQ(swap_buffers_complete_, 1);
    }

    class CountingSoftwareDevice : public SoftwareOutputDevice {
    public:
        CountingSoftwareDevice()
            : frames_began_(0)
            , frames_ended_(0)
        {
        }

        SkCanvas* BeginPaint(const gfx::Rect& damage_rect) override
        {
            ++frames_began_;
            return SoftwareOutputDevice::BeginPaint(damage_rect);
        }
        void EndPaint() override
        {
            SoftwareOutputDevice::EndPaint();
            ++frames_ended_;
        }

        int frames_began_, frames_ended_;
    };

    TEST_F(LayerTreeHostImplTest, ForcedDrawToSoftwareDeviceBasicRender)
    {
        // No main thread evictions in resourceless software mode.
        set_reduce_memory_result(false);
        CountingSoftwareDevice* software_device = new CountingSoftwareDevice();
        EXPECT_TRUE(CreateHostImpl(
            DefaultSettings(),
            FakeOutputSurface::CreateSoftware(make_scoped_ptr(software_device))));
        host_impl_->SetViewportSize(gfx::Size(50, 50));

        SetupScrollAndContentsLayers(gfx::Size(100, 100));

        const gfx::Transform external_transform;
        const gfx::Rect external_viewport;
        const gfx::Rect external_clip;
        const bool resourceless_software_draw = true;
        host_impl_->SetExternalDrawConstraints(external_transform,
            external_viewport,
            external_clip,
            external_viewport,
            external_transform,
            resourceless_software_draw);

        EXPECT_EQ(0, software_device->frames_began_);
        EXPECT_EQ(0, software_device->frames_ended_);

        DrawFrame();

        EXPECT_EQ(1, software_device->frames_began_);
        EXPECT_EQ(1, software_device->frames_ended_);

        // Call another API method that is likely to hit nullptr in this mode.
        scoped_refptr<base::trace_event::TracedValue> state = make_scoped_refptr(new base::trace_event::TracedValue());
        host_impl_->ActivationStateAsValueInto(state.get());
    }

    TEST_F(LayerTreeHostImplTest,
        ForcedDrawToSoftwareDeviceSkipsUnsupportedLayers)
    {
        set_reduce_memory_result(false);
        EXPECT_TRUE(CreateHostImpl(DefaultSettings(),
            FakeOutputSurface::CreateSoftware(
                make_scoped_ptr(new CountingSoftwareDevice))));

        const gfx::Transform external_transform;
        const gfx::Rect external_viewport;
        const gfx::Rect external_clip;
        const bool resourceless_software_draw = true;
        host_impl_->SetExternalDrawConstraints(external_transform,
            external_viewport,
            external_clip,
            external_viewport,
            external_transform,
            resourceless_software_draw);

        // SolidColorLayerImpl will be drawn.
        scoped_ptr<SolidColorLayerImpl> root_layer = SolidColorLayerImpl::Create(host_impl_->active_tree(), 1);

        // VideoLayerImpl will not be drawn.
        FakeVideoFrameProvider provider;
        scoped_ptr<VideoLayerImpl> video_layer = VideoLayerImpl::Create(
            host_impl_->active_tree(), 2, &provider, media::VIDEO_ROTATION_0);
        video_layer->SetBounds(gfx::Size(10, 10));
        video_layer->SetDrawsContent(true);
        root_layer->AddChild(video_layer.Pass());
        SetupRootLayerImpl(root_layer.Pass());

        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);

        EXPECT_EQ(1u, frame.will_draw_layers.size());
        EXPECT_EQ(host_impl_->active_tree()->root_layer(), frame.will_draw_layers[0]);
    }

    // Checks that we have a non-0 default allocation if we pass a context that
    // doesn't support memory management extensions.
    TEST_F(LayerTreeHostImplTest, DefaultMemoryAllocation)
    {
        LayerTreeSettings settings;
        host_impl_ = LayerTreeHostImpl::Create(
            settings, this, &proxy_, &stats_instrumentation_, &shared_bitmap_manager_,
            &gpu_memory_buffer_manager_, &task_graph_runner_, 0);

        output_surface_ = FakeOutputSurface::Create3d(TestWebGraphicsContext3D::Create());
        host_impl_->InitializeRenderer(output_surface_.get());
        EXPECT_LT(0ul, host_impl_->memory_allocation_limit_bytes());
    }

    TEST_F(LayerTreeHostImplTest, RequireHighResWhenVisible)
    {
        ASSERT_TRUE(host_impl_->active_tree());

        // RequiresHighResToDraw is set when new output surface is used.
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());

        host_impl_->ResetRequiresHighResToDraw();

        host_impl_->SetVisible(false);
        EXPECT_FALSE(host_impl_->RequiresHighResToDraw());
        host_impl_->SetVisible(true);
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());
        host_impl_->SetVisible(false);
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());

        host_impl_->ResetRequiresHighResToDraw();

        EXPECT_FALSE(host_impl_->RequiresHighResToDraw());
        host_impl_->SetVisible(true);
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());
    }

    TEST_F(LayerTreeHostImplTest, RequireHighResAfterGpuRasterizationToggles)
    {
        ASSERT_TRUE(host_impl_->active_tree());
        EXPECT_FALSE(host_impl_->use_gpu_rasterization());

        // RequiresHighResToDraw is set when new output surface is used.
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());

        host_impl_->ResetRequiresHighResToDraw();

        host_impl_->SetContentIsSuitableForGpuRasterization(true);
        host_impl_->SetHasGpuRasterizationTrigger(false);
        host_impl_->UpdateTreeResourcesForGpuRasterizationIfNeeded();
        EXPECT_FALSE(host_impl_->RequiresHighResToDraw());
        host_impl_->SetHasGpuRasterizationTrigger(true);
        host_impl_->UpdateTreeResourcesForGpuRasterizationIfNeeded();
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());
        host_impl_->SetHasGpuRasterizationTrigger(false);
        host_impl_->UpdateTreeResourcesForGpuRasterizationIfNeeded();
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());

        host_impl_->ResetRequiresHighResToDraw();

        EXPECT_FALSE(host_impl_->RequiresHighResToDraw());
        host_impl_->SetHasGpuRasterizationTrigger(true);
        host_impl_->UpdateTreeResourcesForGpuRasterizationIfNeeded();
        EXPECT_TRUE(host_impl_->RequiresHighResToDraw());
    }

    class LayerTreeHostImplTestPrepareTiles : public LayerTreeHostImplTest {
    public:
        void SetUp() override
        {
            fake_host_impl_ = new FakeLayerTreeHostImpl(LayerTreeSettings(), &proxy_,
                &shared_bitmap_manager_, &task_graph_runner_);
            host_impl_.reset(fake_host_impl_);
            output_surface_ = CreateOutputSurface();
            host_impl_->InitializeRenderer(output_surface_.get());
            host_impl_->SetViewportSize(gfx::Size(10, 10));
        }

        FakeLayerTreeHostImpl* fake_host_impl_;
    };

    TEST_F(LayerTreeHostImplTestPrepareTiles, PrepareTilesWhenInvisible)
    {
        fake_host_impl_->DidModifyTilePriorities();
        EXPECT_TRUE(fake_host_impl_->prepare_tiles_needed());
        fake_host_impl_->SetVisible(false);
        EXPECT_FALSE(fake_host_impl_->prepare_tiles_needed());
    }

    TEST_F(LayerTreeHostImplTest, UIResourceManagement)
    {
        scoped_ptr<TestWebGraphicsContext3D> context = TestWebGraphicsContext3D::Create();
        TestWebGraphicsContext3D* context3d = context.get();
        scoped_ptr<FakeOutputSurface> output_surface = FakeOutputSurface::Create3d();
        CreateHostImpl(DefaultSettings(), output_surface.Pass());

        EXPECT_EQ(0u, context3d->NumTextures());

        UIResourceId ui_resource_id = 1;
        bool is_opaque = false;
        UIResourceBitmap bitmap(gfx::Size(1, 1), is_opaque);
        host_impl_->CreateUIResource(ui_resource_id, bitmap);
        EXPECT_EQ(1u, context3d->NumTextures());
        ResourceId id1 = host_impl_->ResourceIdForUIResource(ui_resource_id);
        EXPECT_NE(0u, id1);

        // Multiple requests with the same id is allowed.  The previous texture is
        // deleted.
        host_impl_->CreateUIResource(ui_resource_id, bitmap);
        EXPECT_EQ(1u, context3d->NumTextures());
        ResourceId id2 = host_impl_->ResourceIdForUIResource(ui_resource_id);
        EXPECT_NE(0u, id2);
        EXPECT_NE(id1, id2);

        // Deleting invalid UIResourceId is allowed and does not change state.
        host_impl_->DeleteUIResource(-1);
        EXPECT_EQ(1u, context3d->NumTextures());

        // Should return zero for invalid UIResourceId.  Number of textures should
        // not change.
        EXPECT_EQ(0u, host_impl_->ResourceIdForUIResource(-1));
        EXPECT_EQ(1u, context3d->NumTextures());

        host_impl_->DeleteUIResource(ui_resource_id);
        EXPECT_EQ(0u, host_impl_->ResourceIdForUIResource(ui_resource_id));
        EXPECT_EQ(0u, context3d->NumTextures());

        // Should not change state for multiple deletion on one UIResourceId
        host_impl_->DeleteUIResource(ui_resource_id);
        EXPECT_EQ(0u, context3d->NumTextures());
    }

    TEST_F(LayerTreeHostImplTest, CreateETC1UIResource)
    {
        scoped_ptr<TestWebGraphicsContext3D> context = TestWebGraphicsContext3D::Create();
        TestWebGraphicsContext3D* context3d = context.get();
        CreateHostImpl(DefaultSettings(), FakeOutputSurface::Create3d());

        EXPECT_EQ(0u, context3d->NumTextures());

        gfx::Size size(4, 4);
        // SkImageInfo has no support for ETC1.  The |info| below contains the right
        // total pixel size for the bitmap but not the right height and width.  The
        // correct width/height are passed directly to UIResourceBitmap.
        SkImageInfo info = SkImageInfo::Make(4, 2, kAlpha_8_SkColorType, kPremul_SkAlphaType);
        skia::RefPtr<SkPixelRef> pixel_ref = skia::AdoptRef(SkMallocPixelRef::NewAllocate(info, 0, 0));
        pixel_ref->setImmutable();
        UIResourceBitmap bitmap(pixel_ref, size);
        UIResourceId ui_resource_id = 1;
        host_impl_->CreateUIResource(ui_resource_id, bitmap);
        EXPECT_EQ(1u, context3d->NumTextures());
        ResourceId id1 = host_impl_->ResourceIdForUIResource(ui_resource_id);
        EXPECT_NE(0u, id1);
    }

    void ShutdownReleasesContext_Callback(scoped_ptr<CopyOutputResult> result)
    {
    }

    TEST_F(LayerTreeHostImplTest, ShutdownReleasesContext)
    {
        scoped_refptr<TestContextProvider> context_provider = TestContextProvider::Create();

        CreateHostImpl(DefaultSettings(),
            FakeOutputSurface::Create3d(context_provider));

        SetupRootLayerImpl(LayerImpl::Create(host_impl_->active_tree(), 1));

        ScopedPtrVector<CopyOutputRequest> requests;
        requests.push_back(CopyOutputRequest::CreateRequest(
            base::Bind(&ShutdownReleasesContext_Callback)));

        LayerImpl* root = host_impl_->active_tree()->root_layer();
        root->PassCopyRequests(&requests);
        root->set_num_layer_or_descendant_with_copy_request(1);

        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);

        // The CopyOutputResult's callback has a ref on the ContextProvider and a
        // texture in a texture mailbox.
        EXPECT_FALSE(context_provider->HasOneRef());
        EXPECT_EQ(1u, context_provider->TestContext3d()->NumTextures());

        host_impl_ = nullptr;

        // The CopyOutputResult's callback was cancelled, the CopyOutputResult
        // released, and the texture deleted.
        EXPECT_TRUE(context_provider->HasOneRef());
        EXPECT_EQ(0u, context_provider->TestContext3d()->NumTextures());
    }

    TEST_F(LayerTreeHostImplTest, TouchFlingShouldNotBubble)
    {
        // When flinging via touch, only the child should scroll (we should not
        // bubble).
        gfx::Size surface_size(10, 10);
        gfx::Size content_size(20, 20);
        scoped_ptr<LayerImpl> root_clip = LayerImpl::Create(host_impl_->active_tree(), 3);
        root_clip->SetHasRenderSurface(true);

        scoped_ptr<LayerImpl> root = CreateScrollableLayer(1, content_size, root_clip.get());
        root->SetIsContainerForFixedPositionLayers(true);
        scoped_ptr<LayerImpl> child = CreateScrollableLayer(2, content_size, root_clip.get());

        root->AddChild(child.Pass());
        int root_id = root->id();
        root_clip->AddChild(root.Pass());

        host_impl_->SetViewportSize(surface_size);
        host_impl_->active_tree()->SetRootLayer(root_clip.Pass());
        host_impl_->active_tree()->SetViewportLayersFromIds(Layer::INVALID_ID, 3, 1,
            Layer::INVALID_ID);
        host_impl_->active_tree()->DidBecomeActive();
        DrawFrame();
        {
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));

            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());

            gfx::Vector2d scroll_delta(0, 100);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);

            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();

            // Only the child should have scrolled.
            ASSERT_EQ(1u, scroll_info->scrolls.size());
            ExpectNone(*scroll_info.get(), root_id);
        }
    }

    TEST_F(LayerTreeHostImplTest, TouchFlingShouldContinueScrollingCurrentLayer)
    {
        // Scroll a child layer beyond its maximum scroll range and make sure the
        // the scroll doesn't bubble up to the parent layer.
        gfx::Size surface_size(10, 10);
        scoped_ptr<LayerImpl> root = LayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetHasRenderSurface(true);
        scoped_ptr<LayerImpl> root_scrolling = CreateScrollableLayer(2, surface_size, root.get());

        scoped_ptr<LayerImpl> grand_child = CreateScrollableLayer(4, surface_size, root.get());
        grand_child->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 2));

        scoped_ptr<LayerImpl> child = CreateScrollableLayer(3, surface_size, root.get());
        child->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 4));
        child->AddChild(grand_child.Pass());

        root_scrolling->AddChild(child.Pass());
        root->AddChild(root_scrolling.Pass());
        host_impl_->active_tree()->SetRootLayer(root.Pass());
        host_impl_->active_tree()->DidBecomeActive();
        host_impl_->SetViewportSize(surface_size);
        DrawFrame();
        {
            scoped_ptr<ScrollAndScaleSet> scroll_info;
            LayerImpl* child = host_impl_->active_tree()->root_layer()->children()[0]->children()[0];
            LayerImpl* grand_child = child->children()[0];

            gfx::Vector2d scroll_delta(0, -2);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
            EXPECT_TRUE(host_impl_->ScrollBy(gfx::Point(), scroll_delta).did_scroll);

            // The grand child should have scrolled up to its limit.
            scroll_info = host_impl_->ProcessScrollDeltas();
            ASSERT_EQ(1u, scroll_info->scrolls.size());
            EXPECT_TRUE(
                ScrollInfoContains(*scroll_info, grand_child->id(), scroll_delta));
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child);

            // The locked scrolling layer should remain set as the grand child.
            EXPECT_FALSE(host_impl_->ScrollBy(gfx::Point(), scroll_delta).did_scroll);
            scroll_info = host_impl_->ProcessScrollDeltas();
            ASSERT_EQ(1u, scroll_info->scrolls.size());
            EXPECT_TRUE(
                ScrollInfoContains(*scroll_info, grand_child->id(), scroll_delta));
            ExpectNone(*scroll_info, child->id());
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child);

            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
            EXPECT_FALSE(host_impl_->ScrollBy(gfx::Point(), scroll_delta).did_scroll);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child);

            // The child should not have scrolled.
            scroll_info = host_impl_->ProcessScrollDeltas();
            ASSERT_EQ(1u, scroll_info->scrolls.size());
            EXPECT_TRUE(
                ScrollInfoContains(*scroll_info, grand_child->id(), scroll_delta));
            ExpectNone(*scroll_info, child->id());

            // As the locked layer is at it's limit, no further scrolling can occur.
            EXPECT_FALSE(host_impl_->ScrollBy(gfx::Point(), scroll_delta).did_scroll);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), grand_child);
            host_impl_->ScrollEnd();
        }
    }

    TEST_F(LayerTreeHostImplTest, WheelFlingShouldntBubble)
    {
        // When flinging via wheel, we shouldn't bubble.
        gfx::Size surface_size(10, 10);
        gfx::Size content_size(20, 20);
        scoped_ptr<LayerImpl> root_clip = LayerImpl::Create(host_impl_->active_tree(), 3);
        root_clip->SetHasRenderSurface(true);
        scoped_ptr<LayerImpl> root_scroll = CreateScrollableLayer(1, content_size, root_clip.get());
        int root_scroll_id = root_scroll->id();
        scoped_ptr<LayerImpl> child = CreateScrollableLayer(2, content_size, root_clip.get());

        root_scroll->AddChild(child.Pass());
        root_clip->AddChild(root_scroll.Pass());

        host_impl_->SetViewportSize(surface_size);
        host_impl_->active_tree()->SetRootLayer(root_clip.Pass());
        host_impl_->active_tree()->DidBecomeActive();
        DrawFrame();
        {
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());

            gfx::Vector2d scroll_delta(0, 100);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);

            host_impl_->ScrollEnd();

            scoped_ptr<ScrollAndScaleSet> scroll_info = host_impl_->ProcessScrollDeltas();

            // The root shouldn't have scrolled.
            ASSERT_EQ(1u, scroll_info->scrolls.size());
            ExpectNone(*scroll_info.get(), root_scroll_id);
        }
    }

    TEST_F(LayerTreeHostImplTest, ScrollUnknownNotOnAncestorChain)
    {
        // If we ray cast a scroller that is not on the first layer's ancestor chain,
        // we should return SCROLL_UNKNOWN.
        gfx::Size content_size(100, 100);
        SetupScrollAndContentsLayers(content_size);

        int scroll_layer_id = 2;
        LayerImpl* scroll_layer = host_impl_->active_tree()->LayerById(scroll_layer_id);
        scroll_layer->SetDrawsContent(true);

        int page_scale_layer_id = 5;
        LayerImpl* page_scale_layer = host_impl_->active_tree()->LayerById(page_scale_layer_id);

        int occluder_layer_id = 6;
        scoped_ptr<LayerImpl> occluder_layer = LayerImpl::Create(host_impl_->active_tree(), occluder_layer_id);
        occluder_layer->SetDrawsContent(true);
        occluder_layer->SetBounds(content_size);
        occluder_layer->SetPosition(gfx::PointF());

        // The parent of the occluder is *above* the scroller.
        page_scale_layer->AddChild(occluder_layer.Pass());

        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_UNKNOWN,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
    }

    TEST_F(LayerTreeHostImplTest, ScrollUnknownScrollAncestorMismatch)
    {
        // If we ray cast a scroller this is on the first layer's ancestor chain, but
        // is not the first scroller we encounter when walking up from the layer, we
        // should also return SCROLL_UNKNOWN.
        gfx::Size content_size(100, 100);
        SetupScrollAndContentsLayers(content_size);

        int scroll_layer_id = 2;
        LayerImpl* scroll_layer = host_impl_->active_tree()->LayerById(scroll_layer_id);
        scroll_layer->SetDrawsContent(true);

        int occluder_layer_id = 6;
        scoped_ptr<LayerImpl> occluder_layer = LayerImpl::Create(host_impl_->active_tree(), occluder_layer_id);
        occluder_layer->SetDrawsContent(true);
        occluder_layer->SetBounds(content_size);
        occluder_layer->SetPosition(gfx::PointF(-10.f, -10.f));

        int child_scroll_clip_layer_id = 7;
        scoped_ptr<LayerImpl> child_scroll_clip = LayerImpl::Create(host_impl_->active_tree(), child_scroll_clip_layer_id);

        int child_scroll_layer_id = 8;
        scoped_ptr<LayerImpl> child_scroll = CreateScrollableLayer(
            child_scroll_layer_id, content_size, child_scroll_clip.get());

        child_scroll->SetPosition(gfx::PointF(10.f, 10.f));

        child_scroll->AddChild(occluder_layer.Pass());
        scroll_layer->AddChild(child_scroll.Pass());

        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_UNKNOWN,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
    }

    TEST_F(LayerTreeHostImplTest, NotScrollInvisibleScroller)
    {
        gfx::Size content_size(100, 100);
        SetupScrollAndContentsLayers(content_size);

        LayerImpl* root = host_impl_->active_tree()->LayerById(1);

        int scroll_layer_id = 2;
        LayerImpl* scroll_layer = host_impl_->active_tree()->LayerById(scroll_layer_id);

        int child_scroll_layer_id = 7;
        scoped_ptr<LayerImpl> child_scroll = CreateScrollableLayer(child_scroll_layer_id, content_size, root);
        child_scroll->SetDrawsContent(false);

        scroll_layer->AddChild(child_scroll.Pass());

        DrawFrame();

        // We should not have scrolled |child_scroll| even though we technically "hit"
        // it. The reason for this is that if the scrolling the scroll would not move
        // any layer that is a drawn RSLL member, then we can ignore the hit.
        //
        // Why SCROLL_STARTED? In this case, it's because we've bubbled out and
        // started scrolling the inner viewport.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

        EXPECT_EQ(2, host_impl_->CurrentlyScrollingLayer()->id());
    }

    TEST_F(LayerTreeHostImplTest, ScrollInvisibleScrollerWithVisibleDescendent)
    {
        gfx::Size content_size(100, 100);
        SetupScrollAndContentsLayers(content_size);

        LayerImpl* root = host_impl_->active_tree()->LayerById(1);
        LayerImpl* root_scroll_layer = host_impl_->active_tree()->LayerById(2);

        scoped_ptr<LayerImpl> invisible_scroll_layer = CreateScrollableLayer(7, content_size, root);
        invisible_scroll_layer->SetDrawsContent(false);

        scoped_ptr<LayerImpl> child_layer = LayerImpl::Create(host_impl_->active_tree(), 8);
        child_layer->SetDrawsContent(false);

        scoped_ptr<LayerImpl> grand_child_layer = LayerImpl::Create(host_impl_->active_tree(), 9);
        grand_child_layer->SetDrawsContent(true);
        grand_child_layer->SetBounds(content_size);
        // Move the grand child so it's not hit by our test point.
        grand_child_layer->SetPosition(gfx::PointF(10.f, 10.f));

        child_layer->AddChild(grand_child_layer.Pass());
        invisible_scroll_layer->AddChild(child_layer.Pass());
        root_scroll_layer->AddChild(invisible_scroll_layer.Pass());

        DrawFrame();

        // We should have scrolled |invisible_scroll_layer| as it was hit and it has
        // a descendant which is a drawn RSLL member.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

        EXPECT_EQ(7, host_impl_->CurrentlyScrollingLayer()->id());
    }

    TEST_F(LayerTreeHostImplTest, ScrollInvisibleScrollerWithVisibleScrollChild)
    {
        // This test case is very similar to the one above with one key difference:
        // the invisible scroller has a scroll child that is indeed draw contents.
        // If we attempt to initiate a gesture scroll off of the visible scroll child
        // we should still start the scroll child.
        gfx::Size content_size(100, 100);
        SetupScrollAndContentsLayers(content_size);

        LayerImpl* root = host_impl_->active_tree()->LayerById(1);

        int scroll_layer_id = 2;
        LayerImpl* scroll_layer = host_impl_->active_tree()->LayerById(scroll_layer_id);

        int scroll_child_id = 6;
        scoped_ptr<LayerImpl> scroll_child = LayerImpl::Create(host_impl_->active_tree(), scroll_child_id);
        scroll_child->SetDrawsContent(true);
        scroll_child->SetBounds(content_size);
        // Move the scroll child so it's not hit by our test point.
        scroll_child->SetPosition(gfx::PointF(10.f, 10.f));

        int invisible_scroll_layer_id = 7;
        scoped_ptr<LayerImpl> invisible_scroll = CreateScrollableLayer(invisible_scroll_layer_id, content_size, root);
        invisible_scroll->SetDrawsContent(false);

        int container_id = 8;
        scoped_ptr<LayerImpl> container = LayerImpl::Create(host_impl_->active_tree(), container_id);

        scoped_ptr<std::set<LayerImpl*>> scroll_children(new std::set<LayerImpl*>);
        scroll_children->insert(scroll_child.get());
        invisible_scroll->SetScrollChildren(scroll_children.release());

        scroll_child->SetScrollParent(invisible_scroll.get());

        container->AddChild(invisible_scroll.Pass());
        container->AddChild(scroll_child.Pass());

        scroll_layer->AddChild(container.Pass());

        DrawFrame();

        // We should have scrolled |child_scroll| even though it is invisible.
        // The reason for this is that if the scrolling the scroll would move a layer
        // that is a drawn RSLL member, then we should accept this hit.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));

        EXPECT_EQ(7, host_impl_->CurrentlyScrollingLayer()->id());
    }

    // Make sure LatencyInfo carried by LatencyInfoSwapPromise are passed
    // to CompositorFrameMetadata after SwapBuffers();
    TEST_F(LayerTreeHostImplTest, LatencyInfoPassedToCompositorFrameMetadata)
    {
        scoped_ptr<SolidColorLayerImpl> root = SolidColorLayerImpl::Create(host_impl_->active_tree(), 1);
        root->SetPosition(gfx::PointF());
        root->SetBounds(gfx::Size(10, 10));
        root->SetDrawsContent(true);
        root->SetHasRenderSurface(true);

        host_impl_->active_tree()->SetRootLayer(root.Pass());

        FakeOutputSurface* fake_output_surface = static_cast<FakeOutputSurface*>(host_impl_->output_surface());

        const std::vector<ui::LatencyInfo>& metadata_latency_before = fake_output_surface->last_sent_frame().metadata.latency_info;
        EXPECT_TRUE(metadata_latency_before.empty());

        ui::LatencyInfo latency_info;
        latency_info.AddLatencyNumber(
            ui::INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT, 0, 0);
        scoped_ptr<SwapPromise> swap_promise(
            new LatencyInfoSwapPromise(latency_info));
        host_impl_->active_tree()->QueuePinnedSwapPromise(swap_promise.Pass());
        host_impl_->SetNeedsRedraw();

        gfx::Rect full_frame_damage(host_impl_->DrawViewportSize());
        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        EXPECT_TRUE(host_impl_->SwapBuffers(frame));

        const std::vector<ui::LatencyInfo>& metadata_latency_after = fake_output_surface->last_sent_frame().metadata.latency_info;
        EXPECT_EQ(1u, metadata_latency_after.size());
        EXPECT_TRUE(metadata_latency_after[0].FindLatency(
            ui::INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT, 0, NULL));
    }

    TEST_F(LayerTreeHostImplTest, SelectionBoundsPassedToCompositorFrameMetadata)
    {
        int root_layer_id = 1;
        scoped_ptr<SolidColorLayerImpl> root = SolidColorLayerImpl::Create(host_impl_->active_tree(), root_layer_id);
        root->SetPosition(gfx::PointF());
        root->SetBounds(gfx::Size(10, 10));
        root->SetDrawsContent(true);
        root->SetHasRenderSurface(true);

        host_impl_->active_tree()->SetRootLayer(root.Pass());

        // Ensure the default frame selection bounds are empty.
        FakeOutputSurface* fake_output_surface = static_cast<FakeOutputSurface*>(host_impl_->output_surface());
        const ViewportSelection& selection_before = fake_output_surface->last_sent_frame().metadata.selection;
        EXPECT_EQ(ViewportSelectionBound(), selection_before.start);
        EXPECT_EQ(ViewportSelectionBound(), selection_before.end);

        // Plumb the layer-local selection bounds.
        gfx::PointF selection_top(5, 0);
        gfx::PointF selection_bottom(5, 5);
        LayerSelection selection;
        selection.start.type = SELECTION_BOUND_CENTER;
        selection.start.layer_id = root_layer_id;
        selection.start.edge_bottom = selection_bottom;
        selection.start.edge_top = selection_top;
        selection.end = selection.start;
        host_impl_->active_tree()->RegisterSelection(selection);

        // Trigger a draw-swap sequence.
        host_impl_->SetNeedsRedraw();

        gfx::Rect full_frame_damage(host_impl_->DrawViewportSize());
        LayerTreeHostImpl::FrameData frame;
        EXPECT_EQ(DRAW_SUCCESS, PrepareToDrawFrame(&frame));
        host_impl_->DrawLayers(&frame);
        host_impl_->DidDrawAllLayers(frame);
        EXPECT_TRUE(host_impl_->SwapBuffers(frame));

        // Ensure the selection bounds have propagated to the frame metadata.
        const ViewportSelection& selection_after = fake_output_surface->last_sent_frame().metadata.selection;
        EXPECT_EQ(selection.start.type, selection_after.start.type);
        EXPECT_EQ(selection.end.type, selection_after.end.type);
        EXPECT_EQ(selection_bottom, selection_after.start.edge_bottom);
        EXPECT_EQ(selection_top, selection_after.start.edge_top);
        EXPECT_TRUE(selection_after.start.visible);
        EXPECT_TRUE(selection_after.start.visible);
    }

    class SimpleSwapPromiseMonitor : public SwapPromiseMonitor {
    public:
        SimpleSwapPromiseMonitor(LayerTreeHost* layer_tree_host,
            LayerTreeHostImpl* layer_tree_host_impl,
            int* set_needs_commit_count,
            int* set_needs_redraw_count,
            int* forward_to_main_count)
            : SwapPromiseMonitor(layer_tree_host, layer_tree_host_impl)
            , set_needs_commit_count_(set_needs_commit_count)
            , set_needs_redraw_count_(set_needs_redraw_count)
            , forward_to_main_count_(forward_to_main_count)
        {
        }

        ~SimpleSwapPromiseMonitor() override { }

        void OnSetNeedsCommitOnMain() override { (*set_needs_commit_count_)++; }

        void OnSetNeedsRedrawOnImpl() override { (*set_needs_redraw_count_)++; }

        void OnForwardScrollUpdateToMainThreadOnImpl() override
        {
            (*forward_to_main_count_)++;
        }

    private:
        int* set_needs_commit_count_;
        int* set_needs_redraw_count_;
        int* forward_to_main_count_;
    };

    TEST_F(LayerTreeHostImplTest, SimpleSwapPromiseMonitor)
    {
        int set_needs_commit_count = 0;
        int set_needs_redraw_count = 0;
        int forward_to_main_count = 0;

        {
            scoped_ptr<SimpleSwapPromiseMonitor> swap_promise_monitor(
                new SimpleSwapPromiseMonitor(NULL,
                    host_impl_.get(),
                    &set_needs_commit_count,
                    &set_needs_redraw_count,
                    &forward_to_main_count));
            host_impl_->SetNeedsRedraw();
            EXPECT_EQ(0, set_needs_commit_count);
            EXPECT_EQ(1, set_needs_redraw_count);
            EXPECT_EQ(0, forward_to_main_count);
        }

        // Now the monitor is destroyed, SetNeedsRedraw() is no longer being
        // monitored.
        host_impl_->SetNeedsRedraw();
        EXPECT_EQ(0, set_needs_commit_count);
        EXPECT_EQ(1, set_needs_redraw_count);
        EXPECT_EQ(0, forward_to_main_count);

        {
            scoped_ptr<SimpleSwapPromiseMonitor> swap_promise_monitor(
                new SimpleSwapPromiseMonitor(NULL,
                    host_impl_.get(),
                    &set_needs_commit_count,
                    &set_needs_redraw_count,
                    &forward_to_main_count));
            host_impl_->SetNeedsRedrawRect(gfx::Rect(10, 10));
            EXPECT_EQ(0, set_needs_commit_count);
            EXPECT_EQ(2, set_needs_redraw_count);
            EXPECT_EQ(0, forward_to_main_count);
        }

        {
            scoped_ptr<SimpleSwapPromiseMonitor> swap_promise_monitor(
                new SimpleSwapPromiseMonitor(NULL,
                    host_impl_.get(),
                    &set_needs_commit_count,
                    &set_needs_redraw_count,
                    &forward_to_main_count));
            // Empty damage rect won't signal the monitor.
            host_impl_->SetNeedsRedrawRect(gfx::Rect());
            EXPECT_EQ(0, set_needs_commit_count);
            EXPECT_EQ(2, set_needs_redraw_count);
            EXPECT_EQ(0, forward_to_main_count);
        }

        {
            set_needs_commit_count = 0;
            set_needs_redraw_count = 0;
            forward_to_main_count = 0;
            scoped_ptr<SimpleSwapPromiseMonitor> swap_promise_monitor(
                new SimpleSwapPromiseMonitor(NULL,
                    host_impl_.get(),
                    &set_needs_commit_count,
                    &set_needs_redraw_count,
                    &forward_to_main_count));
            LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));

            // Scrolling normally should not trigger any forwarding.
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
            EXPECT_TRUE(
                host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10)).did_scroll);
            host_impl_->ScrollEnd();

            EXPECT_EQ(0, set_needs_commit_count);
            EXPECT_EQ(1, set_needs_redraw_count);
            EXPECT_EQ(0, forward_to_main_count);

            // Scrolling with a scroll handler should defer the swap to the main
            // thread.
            scroll_layer->SetHaveScrollEventHandlers(true);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
            EXPECT_TRUE(
                host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, 10)).did_scroll);
            host_impl_->ScrollEnd();

            EXPECT_EQ(0, set_needs_commit_count);
            EXPECT_EQ(2, set_needs_redraw_count);
            EXPECT_EQ(1, forward_to_main_count);
        }
    }

    class LayerTreeHostImplWithTopControlsTest : public LayerTreeHostImplTest {
    public:
        void SetUp() override
        {
            LayerTreeSettings settings = DefaultSettings();
            CreateHostImpl(settings, CreateOutputSurface());
            host_impl_->active_tree()->set_top_controls_height(top_controls_height_);
            host_impl_->sync_tree()->set_top_controls_height(top_controls_height_);
            host_impl_->active_tree()->SetCurrentTopControlsShownRatio(1.f);
        }

    protected:
        static const int top_controls_height_;
    };

    const int LayerTreeHostImplWithTopControlsTest::top_controls_height_ = 50;

    TEST_F(LayerTreeHostImplWithTopControlsTest, NoIdleAnimations)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100))
            ->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 10));
        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        EXPECT_FALSE(did_request_redraw_);
        host_impl_->DidFinishImplFrame();
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest, TopControlsHeightIsCommitted)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        EXPECT_FALSE(did_request_redraw_);
        host_impl_->CreatePendingTree();
        host_impl_->sync_tree()->set_top_controls_height(100);
        host_impl_->ActivateSyncTree();
        EXPECT_EQ(100, host_impl_->top_controls_manager()->TopControlsHeight());
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest,
        TopControlsStayFullyVisibleOnHeightChange)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100));
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ControlsTopOffset());

        host_impl_->CreatePendingTree();
        host_impl_->sync_tree()->set_top_controls_height(0);
        host_impl_->ActivateSyncTree();
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ControlsTopOffset());

        host_impl_->CreatePendingTree();
        host_impl_->sync_tree()->set_top_controls_height(50);
        host_impl_->ActivateSyncTree();
        EXPECT_EQ(0.f, host_impl_->top_controls_manager()->ControlsTopOffset());
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest, TopControlsAnimationScheduling)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 100))
            ->PushScrollOffsetFromMainThread(gfx::ScrollOffset(0, 10));
        host_impl_->DidChangeTopControlsPosition();
        EXPECT_TRUE(did_request_animate_);
        EXPECT_TRUE(did_request_redraw_);
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest, ScrollHandledByTopControls)
    {
        InputHandlerScrollResult result;
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));
        host_impl_->top_controls_manager()->UpdateTopControlsState(
            BOTH, SHOWN, false);
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // Scroll just the top controls and verify that the scroll succeeds.
        const float residue = 10;
        float offset = top_controls_height_ - residue;
        result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset));
        EXPECT_EQ(result.unused_scroll_delta, gfx::Vector2d(0, 0));
        EXPECT_TRUE(result.did_scroll);
        EXPECT_FLOAT_EQ(-offset,
            host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // Scroll across the boundary
        const float content_scroll = 20;
        offset = residue + content_scroll;
        result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset));
        EXPECT_TRUE(result.did_scroll);
        EXPECT_EQ(result.unused_scroll_delta, gfx::Vector2d(0, 0));
        EXPECT_EQ(-top_controls_height_,
            host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF(0, content_scroll).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // Now scroll back to the top of the content
        offset = -content_scroll;
        result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset));
        EXPECT_TRUE(result.did_scroll);
        EXPECT_EQ(result.unused_scroll_delta, gfx::Vector2d(0, 0));
        EXPECT_EQ(-top_controls_height_,
            host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // And scroll the top controls completely into view
        offset = -top_controls_height_;
        result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset));
        EXPECT_TRUE(result.did_scroll);
        EXPECT_EQ(result.unused_scroll_delta, gfx::Vector2d(0, 0));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // And attempt to scroll past the end
        result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset));
        EXPECT_FALSE(result.did_scroll);
        EXPECT_EQ(result.unused_scroll_delta, gfx::Vector2d(0, -50));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        host_impl_->ScrollEnd();
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest, WheelUnhandledByTopControls)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 200));
        host_impl_->SetViewportSize(gfx::Size(50, 100));
        host_impl_->active_tree()->set_top_controls_shrink_blink_size(true);
        host_impl_->top_controls_manager()->UpdateTopControlsState(BOTH, SHOWN,
            false);
        DrawFrame();

        LayerImpl* viewport_layer = host_impl_->InnerViewportScrollLayer();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), viewport_layer->CurrentScrollOffset());

        // Wheel scrolls should not affect the top controls, and should pass
        // directly through to the viewport.
        const float delta = top_controls_height_;
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, delta)).did_scroll);
        EXPECT_FLOAT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, delta),
            viewport_layer->CurrentScrollOffset());

        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, delta)).did_scroll);
        EXPECT_FLOAT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, delta * 2),
            viewport_layer->CurrentScrollOffset());
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest, TopControlsAnimationAtOrigin)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 200));
        host_impl_->top_controls_manager()->UpdateTopControlsState(
            BOTH, SHOWN, false);
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // Scroll the top controls partially.
        const float residue = 35;
        float offset = top_controls_height_ - residue;
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset)).did_scroll);
        EXPECT_FLOAT_EQ(-offset,
            host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        did_request_redraw_ = false;
        did_request_animate_ = false;
        did_request_commit_ = false;

        // End the scroll while the controls are still offset from their limit.
        host_impl_->ScrollEnd();
        ASSERT_TRUE(host_impl_->top_controls_manager()->animation());
        EXPECT_TRUE(did_request_animate_);
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);

        // The top controls should properly animate until finished, despite the scroll
        // offset being at the origin.
        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(
            BEGINFRAME_FROM_HERE, base::TimeTicks::Now());
        while (did_request_animate_) {
            did_request_redraw_ = false;
            did_request_animate_ = false;
            did_request_commit_ = false;

            float old_offset = host_impl_->top_controls_manager()->ControlsTopOffset();

            begin_frame_args.frame_time += base::TimeDelta::FromMilliseconds(5);
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();
            EXPECT_EQ(gfx::Vector2dF().ToString(),
                scroll_layer->CurrentScrollOffset().ToString());

            float new_offset = host_impl_->top_controls_manager()->ControlsTopOffset();

            // No commit is needed as the controls are animating the content offset,
            // not the scroll offset.
            EXPECT_FALSE(did_request_commit_);

            if (new_offset != old_offset)
                EXPECT_TRUE(did_request_redraw_);

            if (new_offset != 0) {
                EXPECT_TRUE(host_impl_->top_controls_manager()->animation());
                EXPECT_TRUE(did_request_animate_);
            }
            host_impl_->DidFinishImplFrame();
        }
        EXPECT_FALSE(host_impl_->top_controls_manager()->animation());
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest, TopControlsAnimationAfterScroll)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));
        host_impl_->top_controls_manager()->UpdateTopControlsState(
            BOTH, SHOWN, false);
        float initial_scroll_offset = 50;
        scroll_layer->PushScrollOffsetFromMainThread(
            gfx::ScrollOffset(0, initial_scroll_offset));
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF(0, initial_scroll_offset).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // Scroll the top controls partially.
        const float residue = 15;
        float offset = top_controls_height_ - residue;
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset)).did_scroll);
        EXPECT_FLOAT_EQ(-offset,
            host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF(0, initial_scroll_offset).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        did_request_redraw_ = false;
        did_request_animate_ = false;
        did_request_commit_ = false;

        // End the scroll while the controls are still offset from the limit.
        host_impl_->ScrollEnd();
        ASSERT_TRUE(host_impl_->top_controls_manager()->animation());
        EXPECT_TRUE(did_request_animate_);
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);

        // Animate the top controls to the limit.
        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(
            BEGINFRAME_FROM_HERE, base::TimeTicks::Now());
        while (did_request_animate_) {
            did_request_redraw_ = false;
            did_request_animate_ = false;
            did_request_commit_ = false;

            float old_offset = host_impl_->top_controls_manager()->ControlsTopOffset();

            begin_frame_args.frame_time += base::TimeDelta::FromMilliseconds(5);
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();

            float new_offset = host_impl_->top_controls_manager()->ControlsTopOffset();

            if (new_offset != old_offset) {
                EXPECT_TRUE(did_request_redraw_);
                EXPECT_TRUE(did_request_commit_);
            }
            host_impl_->DidFinishImplFrame();
        }
        EXPECT_FALSE(host_impl_->top_controls_manager()->animation());
        EXPECT_EQ(-top_controls_height_,
            host_impl_->top_controls_manager()->ControlsTopOffset());
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest,
        TopControlsAnimationAfterMainThreadFlingStopped)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));
        host_impl_->top_controls_manager()->UpdateTopControlsState(BOTH, SHOWN,
            false);
        float initial_scroll_offset = 50;
        scroll_layer->PushScrollOffsetFromMainThread(
            gfx::ScrollOffset(0, initial_scroll_offset));
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF(0, initial_scroll_offset).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // Scroll the top controls partially.
        const float residue = 15;
        float offset = top_controls_height_ - residue;
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset)).did_scroll);
        EXPECT_FLOAT_EQ(-offset,
            host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF(0, initial_scroll_offset).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        did_request_redraw_ = false;
        did_request_animate_ = false;
        did_request_commit_ = false;

        // End the fling while the controls are still offset from the limit.
        host_impl_->MainThreadHasStoppedFlinging();
        ASSERT_TRUE(host_impl_->top_controls_manager()->animation());
        EXPECT_TRUE(did_request_animate_);
        EXPECT_TRUE(did_request_redraw_);
        EXPECT_FALSE(did_request_commit_);

        // Animate the top controls to the limit.
        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(
            BEGINFRAME_FROM_HERE, base::TimeTicks::Now());
        while (did_request_animate_) {
            did_request_redraw_ = false;
            did_request_animate_ = false;
            did_request_commit_ = false;

            float old_offset = host_impl_->top_controls_manager()->ControlsTopOffset();

            begin_frame_args.frame_time += base::TimeDelta::FromMilliseconds(5);
            host_impl_->WillBeginImplFrame(begin_frame_args);
            host_impl_->Animate();

            float new_offset = host_impl_->top_controls_manager()->ControlsTopOffset();

            if (new_offset != old_offset) {
                EXPECT_TRUE(did_request_redraw_);
                EXPECT_TRUE(did_request_commit_);
            }
            host_impl_->DidFinishImplFrame();
        }
        EXPECT_FALSE(host_impl_->top_controls_manager()->animation());
        EXPECT_EQ(-top_controls_height_,
            host_impl_->top_controls_manager()->ControlsTopOffset());
    }

    TEST_F(LayerTreeHostImplWithTopControlsTest,
        TopControlsScrollDeltaInOverScroll)
    {
        // Verifies that the overscroll delta should not have accumulated in
        // the top controls if we do a hide and show without releasing finger.

        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 200));
        host_impl_->SetViewportSize(gfx::Size(100, 100));
        host_impl_->top_controls_manager()->UpdateTopControlsState(BOTH, SHOWN,
            false);
        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());

        float offset = 50;
        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset)).did_scroll);
        EXPECT_EQ(-offset, host_impl_->top_controls_manager()->ControlsTopOffset());
        EXPECT_EQ(gfx::Vector2dF().ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset)).did_scroll);
        EXPECT_EQ(gfx::Vector2dF(0, offset).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, offset)).did_scroll);

        // Should have fully scrolled
        EXPECT_EQ(gfx::Vector2dF(0, scroll_layer->MaxScrollOffset().y()).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        float overscrollamount = 10;

        // Overscroll the content
        EXPECT_FALSE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, overscrollamount))
                .did_scroll);
        EXPECT_EQ(gfx::Vector2dF(0, 2 * offset).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());
        EXPECT_EQ(gfx::Vector2dF(0, overscrollamount).ToString(),
            host_impl_->accumulated_root_overscroll().ToString());

        EXPECT_TRUE(host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -2 * offset))
                        .did_scroll);
        EXPECT_EQ(gfx::Vector2dF(0, 0).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());
        EXPECT_EQ(-offset, host_impl_->top_controls_manager()->ControlsTopOffset());

        EXPECT_TRUE(
            host_impl_->ScrollBy(gfx::Point(), gfx::Vector2d(0, -offset)).did_scroll);
        EXPECT_EQ(gfx::Vector2dF(0, 0).ToString(),
            scroll_layer->CurrentScrollOffset().ToString());

        // Top controls should be fully visible
        EXPECT_EQ(0, host_impl_->top_controls_manager()->ControlsTopOffset());

        host_impl_->ScrollEnd();
    }

    class LayerTreeHostImplVirtualViewportTest : public LayerTreeHostImplTest {
    public:
        void SetupVirtualViewportLayers(const gfx::Size& content_size,
            const gfx::Size& outer_viewport,
            const gfx::Size& inner_viewport)
        {
            LayerTreeImpl* layer_tree_impl = host_impl_->active_tree();
            const int kOuterViewportClipLayerId = 6;
            const int kOuterViewportScrollLayerId = 7;
            const int kInnerViewportScrollLayerId = 2;
            const int kInnerViewportClipLayerId = 4;
            const int kPageScaleLayerId = 5;

            scoped_ptr<LayerImpl> inner_scroll = LayerImpl::Create(layer_tree_impl, kInnerViewportScrollLayerId);
            inner_scroll->SetIsContainerForFixedPositionLayers(true);
            inner_scroll->PushScrollOffsetFromMainThread(gfx::ScrollOffset());

            scoped_ptr<LayerImpl> inner_clip = LayerImpl::Create(layer_tree_impl, kInnerViewportClipLayerId);
            inner_clip->SetBounds(inner_viewport);

            scoped_ptr<LayerImpl> page_scale = LayerImpl::Create(layer_tree_impl, kPageScaleLayerId);

            inner_scroll->SetScrollClipLayer(inner_clip->id());
            inner_scroll->SetBounds(outer_viewport);
            inner_scroll->SetPosition(gfx::PointF());

            scoped_ptr<LayerImpl> outer_clip = LayerImpl::Create(layer_tree_impl, kOuterViewportClipLayerId);
            outer_clip->SetBounds(outer_viewport);
            outer_clip->SetIsContainerForFixedPositionLayers(true);

            scoped_ptr<LayerImpl> outer_scroll = LayerImpl::Create(layer_tree_impl, kOuterViewportScrollLayerId);
            outer_scroll->SetScrollClipLayer(outer_clip->id());
            outer_scroll->PushScrollOffsetFromMainThread(gfx::ScrollOffset());
            outer_scroll->SetBounds(content_size);
            outer_scroll->SetPosition(gfx::PointF());

            scoped_ptr<LayerImpl> contents = LayerImpl::Create(layer_tree_impl, 8);
            contents->SetDrawsContent(true);
            contents->SetBounds(content_size);
            contents->SetPosition(gfx::PointF());

            outer_scroll->AddChild(contents.Pass());
            outer_clip->AddChild(outer_scroll.Pass());
            inner_scroll->AddChild(outer_clip.Pass());
            page_scale->AddChild(inner_scroll.Pass());
            inner_clip->AddChild(page_scale.Pass());

            inner_clip->SetHasRenderSurface(true);
            layer_tree_impl->SetRootLayer(inner_clip.Pass());
            layer_tree_impl->SetViewportLayersFromIds(
                Layer::INVALID_ID, kPageScaleLayerId, kInnerViewportScrollLayerId,
                kOuterViewportScrollLayerId);

            host_impl_->active_tree()->DidBecomeActive();
        }
    };

    TEST_F(LayerTreeHostImplVirtualViewportTest, ScrollBothInnerAndOuterLayer)
    {
        gfx::Size content_size = gfx::Size(100, 160);
        gfx::Size outer_viewport = gfx::Size(50, 80);
        gfx::Size inner_viewport = gfx::Size(25, 40);

        SetupVirtualViewportLayers(content_size, outer_viewport, inner_viewport);

        LayerImpl* outer_scroll = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();
        DrawFrame();
        {
            gfx::ScrollOffset inner_expected;
            gfx::ScrollOffset outer_expected;
            EXPECT_EQ(inner_expected, inner_scroll->CurrentScrollOffset());
            EXPECT_EQ(outer_expected, outer_scroll->CurrentScrollOffset());

            gfx::ScrollOffset current_offset(70.f, 100.f);

            host_impl_->SetSynchronousInputHandlerRootScrollOffset(current_offset);
            EXPECT_EQ(gfx::ScrollOffset(25.f, 40.f), inner_scroll->MaxScrollOffset());
            EXPECT_EQ(gfx::ScrollOffset(50.f, 80.f), outer_scroll->MaxScrollOffset());

            // Outer viewport scrolls first. Then the rest is applied to the inner
            // viewport.
            EXPECT_EQ(gfx::ScrollOffset(20.f, 20.f),
                inner_scroll->CurrentScrollOffset());
            EXPECT_EQ(gfx::ScrollOffset(50.f, 80.f),
                outer_scroll->CurrentScrollOffset());
        }
    }

    TEST_F(LayerTreeHostImplVirtualViewportTest, FlingScrollBubblesToInner)
    {
        gfx::Size content_size = gfx::Size(200, 320);
        gfx::Size outer_viewport = gfx::Size(100, 160);
        gfx::Size inner_viewport = gfx::Size(50, 80);

        SetupVirtualViewportLayers(content_size, outer_viewport, inner_viewport);

        LayerImpl* outer_scroll = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();
        DrawFrame();
        {
            gfx::Vector2dF inner_expected;
            gfx::Vector2dF outer_expected;
            EXPECT_VECTOR_EQ(inner_expected, inner_scroll->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(outer_expected, outer_scroll->CurrentScrollOffset());

            // Scrolling the viewport always sets the outer scroll layer as the
            // currently scrolling layer.
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
            EXPECT_EQ(inner_scroll, host_impl_->CurrentlyScrollingLayer());
            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
            EXPECT_EQ(inner_scroll, host_impl_->CurrentlyScrollingLayer());

            gfx::Vector2d scroll_delta(inner_viewport.width() / 2.f,
                inner_viewport.height() / 2.f);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            inner_expected += gfx::Vector2dF(scroll_delta.x(), scroll_delta.y());
            EXPECT_EQ(inner_scroll, host_impl_->CurrentlyScrollingLayer());

            host_impl_->ScrollEnd();
            EXPECT_EQ(nullptr, host_impl_->CurrentlyScrollingLayer());

            EXPECT_VECTOR_EQ(inner_expected, inner_scroll->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(outer_expected, outer_scroll->CurrentScrollOffset());

            // Fling past the inner viewport boundry, make sure outer viewport scrolls.
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
            EXPECT_EQ(inner_scroll, host_impl_->CurrentlyScrollingLayer());
            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
            EXPECT_EQ(inner_scroll, host_impl_->CurrentlyScrollingLayer());

            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            inner_expected += gfx::Vector2dF(scroll_delta.x(), scroll_delta.y());
            EXPECT_EQ(inner_scroll, host_impl_->CurrentlyScrollingLayer());

            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            outer_expected += gfx::Vector2dF(scroll_delta.x(), scroll_delta.y());
            EXPECT_EQ(inner_scroll, host_impl_->CurrentlyScrollingLayer());

            host_impl_->ScrollEnd();
            EXPECT_EQ(nullptr, host_impl_->CurrentlyScrollingLayer());

            EXPECT_VECTOR_EQ(inner_expected, inner_scroll->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(outer_expected, outer_scroll->CurrentScrollOffset());
        }
    }

    TEST_F(LayerTreeHostImplVirtualViewportTest,
        DiagonalScrollBubblesPerfectlyToInner)
    {
        gfx::Size content_size = gfx::Size(200, 320);
        gfx::Size outer_viewport = gfx::Size(100, 160);
        gfx::Size inner_viewport = gfx::Size(50, 80);

        SetupVirtualViewportLayers(content_size, outer_viewport, inner_viewport);

        LayerImpl* outer_scroll = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();
        DrawFrame();
        {
            gfx::Vector2dF inner_expected;
            gfx::Vector2dF outer_expected;
            EXPECT_VECTOR_EQ(inner_expected, inner_scroll->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(outer_expected, outer_scroll->CurrentScrollOffset());

            // Make sure the scroll goes to the inner viewport first.
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
            EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(),
                InputHandler::GESTURE));

            // Scroll near the edge of the outer viewport.
            gfx::Vector2d scroll_delta(inner_viewport.width() / 2.f,
                inner_viewport.height() / 2.f);
            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            inner_expected += scroll_delta;
            EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(),
                InputHandler::GESTURE));

            EXPECT_VECTOR_EQ(inner_expected, inner_scroll->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(outer_expected, outer_scroll->CurrentScrollOffset());

            // Now diagonal scroll across the outer viewport boundary in a single event.
            // The entirety of the scroll should be consumed, as bubbling between inner
            // and outer viewport layers is perfect.
            host_impl_->ScrollBy(gfx::Point(), gfx::ScaleVector2d(scroll_delta, 2));
            EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(),
                InputHandler::GESTURE));
            outer_expected += scroll_delta;
            inner_expected += scroll_delta;
            host_impl_->ScrollEnd();
            EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(
                gfx::Point(), InputHandler::GESTURE));

            EXPECT_VECTOR_EQ(inner_expected, inner_scroll->CurrentScrollOffset());
            EXPECT_VECTOR_EQ(outer_expected, outer_scroll->CurrentScrollOffset());
        }
    }

    TEST_F(LayerTreeHostImplVirtualViewportTest,
        TouchFlingDoesntSwitchScrollingLayer)
    {
        gfx::Size content_size = gfx::Size(100, 160);
        gfx::Size outer_viewport = gfx::Size(50, 80);
        gfx::Size inner_viewport = gfx::Size(25, 40);

        SetupVirtualViewportLayers(content_size, outer_viewport, inner_viewport);

        LayerImpl* outer_scroll = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();

        scoped_ptr<LayerImpl> child = CreateScrollableLayer(10, outer_viewport, outer_scroll);
        LayerImpl* child_scroll = child.get();
        outer_scroll->children()[0]->AddChild(child.Pass());

        DrawFrame();
        {
            scoped_ptr<ScrollAndScaleSet> scroll_info;

            gfx::Vector2d scroll_delta(0, inner_viewport.height());
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
            EXPECT_TRUE(host_impl_->ScrollBy(gfx::Point(), scroll_delta).did_scroll);
            EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(),
                InputHandler::GESTURE));

            // The child should have scrolled up to its limit.
            scroll_info = host_impl_->ProcessScrollDeltas();
            ASSERT_EQ(1u, scroll_info->scrolls.size());
            EXPECT_TRUE(
                ScrollInfoContains(*scroll_info, child_scroll->id(), scroll_delta));
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), child_scroll);

            // The fling have no effect on the currently scrolling layer.
            EXPECT_EQ(InputHandler::SCROLL_STARTED, host_impl_->FlingScrollBegin());
            EXPECT_FALSE(host_impl_->ScrollBy(gfx::Point(), scroll_delta).did_scroll);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), child_scroll);
            EXPECT_TRUE(host_impl_->IsCurrentlyScrollingLayerAt(gfx::Point(),
                InputHandler::GESTURE));

            // The inner viewport shouldn't have scrolled.
            scroll_info = host_impl_->ProcessScrollDeltas();
            ASSERT_EQ(1u, scroll_info->scrolls.size());
            EXPECT_TRUE(
                ScrollInfoContains(*scroll_info, child_scroll->id(), scroll_delta));
            ExpectNone(*scroll_info, inner_scroll->id());

            // As the locked layer is at its limit, no further scrolling can occur.
            EXPECT_FALSE(host_impl_->ScrollBy(gfx::Point(), scroll_delta).did_scroll);
            EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), child_scroll);
            host_impl_->ScrollEnd();
            EXPECT_FALSE(host_impl_->IsCurrentlyScrollingLayerAt(
                gfx::Point(), InputHandler::GESTURE));
        }
    }

    TEST_F(LayerTreeHostImplVirtualViewportTest,
        ScrollBeginEventThatTargetsViewportLayerSkipsHitTest)
    {
        gfx::Size content_size = gfx::Size(100, 160);
        gfx::Size outer_viewport = gfx::Size(50, 80);
        gfx::Size inner_viewport = gfx::Size(25, 40);

        SetupVirtualViewportLayers(content_size, outer_viewport, inner_viewport);

        LayerImpl* outer_scroll = host_impl_->OuterViewportScrollLayer();
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();

        scoped_ptr<LayerImpl> child = CreateScrollableLayer(10, outer_viewport, outer_scroll);
        LayerImpl* child_scroll = child.get();
        outer_scroll->children()[0]->AddChild(child.Pass());

        DrawFrame();

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->RootScrollBegin(InputHandler::GESTURE));
        EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), inner_scroll);
        host_impl_->ScrollEnd();
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        EXPECT_EQ(host_impl_->CurrentlyScrollingLayer(), child_scroll);
        host_impl_->ScrollEnd();
    }

    TEST_F(LayerTreeHostImplVirtualViewportTest,
        NoOverscrollWhenInnerViewportCantScroll)
    {
        InputHandlerScrollResult scroll_result;
        gfx::Size content_size = gfx::Size(100, 160);
        gfx::Size outer_viewport = gfx::Size(50, 80);
        gfx::Size inner_viewport = gfx::Size(25, 40);
        SetupVirtualViewportLayers(content_size, outer_viewport, inner_viewport);
        DrawFrame();

        // Make inner viewport unscrollable.
        LayerImpl* inner_scroll = host_impl_->InnerViewportScrollLayer();
        inner_scroll->set_user_scrollable_horizontal(false);
        inner_scroll->set_user_scrollable_vertical(false);

        // Ensure inner viewport doesn't react to scrolls (test it's unscrollable).
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), inner_scroll->CurrentScrollOffset());
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE));
        scroll_result = host_impl_->ScrollBy(gfx::Point(), gfx::Vector2dF(0, 100));
        EXPECT_VECTOR_EQ(gfx::Vector2dF(), inner_scroll->CurrentScrollOffset());

        // When inner viewport is unscrollable, a fling gives zero overscroll.
        EXPECT_FALSE(scroll_result.did_overscroll_root);
        EXPECT_EQ(gfx::Vector2dF(), host_impl_->accumulated_root_overscroll());
    }

    class LayerTreeHostImplWithImplicitLimitsTest : public LayerTreeHostImplTest {
    public:
        void SetUp() override
        {
            LayerTreeSettings settings = DefaultSettings();
            settings.max_memory_for_prepaint_percentage = 50;
            CreateHostImpl(settings, CreateOutputSurface());
        }
    };

    TEST_F(LayerTreeHostImplWithImplicitLimitsTest, ImplicitMemoryLimits)
    {
        // Set up a memory policy and percentages which could cause
        // 32-bit integer overflows.
        ManagedMemoryPolicy mem_policy(300 * 1024 * 1024); // 300MB

        // Verify implicit limits are calculated correctly with no overflows
        host_impl_->SetMemoryPolicy(mem_policy);
        EXPECT_EQ(host_impl_->global_tile_state().hard_memory_limit_in_bytes,
            300u * 1024u * 1024u);
        EXPECT_EQ(host_impl_->global_tile_state().soft_memory_limit_in_bytes,
            150u * 1024u * 1024u);
    }

    TEST_F(LayerTreeHostImplTest, ExternalTransformReflectedInNextDraw)
    {
        const gfx::Size layer_size(100, 100);
        gfx::Transform external_transform;
        const gfx::Rect external_viewport(layer_size);
        const gfx::Rect external_clip(layer_size);
        const bool resourceless_software_draw = false;
        LayerImpl* layer = SetupScrollAndContentsLayers(layer_size);

        host_impl_->SetExternalDrawConstraints(external_transform,
            external_viewport,
            external_clip,
            external_viewport,
            external_transform,
            resourceless_software_draw);
        DrawFrame();
        EXPECT_TRANSFORMATION_MATRIX_EQ(
            external_transform, layer->draw_properties().target_space_transform);

        external_transform.Translate(20, 20);
        host_impl_->SetExternalDrawConstraints(external_transform,
            external_viewport,
            external_clip,
            external_viewport,
            external_transform,
            resourceless_software_draw);
        DrawFrame();
        EXPECT_TRANSFORMATION_MATRIX_EQ(
            external_transform, layer->draw_properties().target_space_transform);
    }

    TEST_F(LayerTreeHostImplTest, ScrollAnimated)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 200));

        // Shrink the outer viewport clip layer so that the outer viewport can scroll.
        host_impl_->OuterViewportScrollLayer()->parent()->SetBounds(
            gfx::Size(50, 100));

        DrawFrame();

        base::TimeTicks start_time = base::TimeTicks() + base::TimeDelta::FromMilliseconds(100);

        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollAnimated(gfx::Point(), gfx::Vector2d(0, 50)));

        LayerImpl* scrolling_layer = host_impl_->CurrentlyScrollingLayer();
        EXPECT_EQ(host_impl_->OuterViewportScrollLayer(), scrolling_layer);

        begin_frame_args.frame_time = start_time;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        EXPECT_EQ(gfx::ScrollOffset(), scrolling_layer->CurrentScrollOffset());
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = start_time + base::TimeDelta::FromMilliseconds(50);
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        float y = scrolling_layer->CurrentScrollOffset().y();
        EXPECT_TRUE(y > 1 && y < 49);

        // Update target.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollAnimated(gfx::Point(), gfx::Vector2d(0, 50)));
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = start_time + base::TimeDelta::FromMilliseconds(200);
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        y = scrolling_layer->CurrentScrollOffset().y();
        EXPECT_TRUE(y > 50 && y < 100);
        EXPECT_EQ(scrolling_layer, host_impl_->CurrentlyScrollingLayer());
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = start_time + base::TimeDelta::FromMilliseconds(250);
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        EXPECT_VECTOR_EQ(gfx::ScrollOffset(0, 100),
            scrolling_layer->CurrentScrollOffset());
        EXPECT_EQ(NULL, host_impl_->CurrentlyScrollingLayer());
        host_impl_->DidFinishImplFrame();
    }

    // Evolved from LayerTreeHostImplTest.ScrollAnimated.
    TEST_F(LayerTreeHostImplTimelinesTest, ScrollAnimated)
    {
        SetupScrollAndContentsLayers(gfx::Size(100, 200));
        DrawFrame();

        base::TimeTicks start_time = base::TimeTicks() + base::TimeDelta::FromMilliseconds(100);

        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);

        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollAnimated(gfx::Point(), gfx::Vector2d(0, 50)));

        LayerImpl* scrolling_layer = host_impl_->CurrentlyScrollingLayer();

        begin_frame_args.frame_time = start_time;
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        EXPECT_EQ(gfx::ScrollOffset(), scrolling_layer->CurrentScrollOffset());
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = start_time + base::TimeDelta::FromMilliseconds(50);
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        float y = scrolling_layer->CurrentScrollOffset().y();
        EXPECT_TRUE(y > 1 && y < 49);

        // Update target.
        EXPECT_EQ(InputHandler::SCROLL_STARTED,
            host_impl_->ScrollAnimated(gfx::Point(), gfx::Vector2d(0, 50)));
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = start_time + base::TimeDelta::FromMilliseconds(200);
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        y = scrolling_layer->CurrentScrollOffset().y();
        EXPECT_TRUE(y > 50 && y < 100);
        EXPECT_EQ(scrolling_layer, host_impl_->CurrentlyScrollingLayer());
        host_impl_->DidFinishImplFrame();

        begin_frame_args.frame_time = start_time + base::TimeDelta::FromMilliseconds(250);
        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->Animate();
        host_impl_->UpdateAnimationState(true);

        EXPECT_VECTOR_EQ(gfx::ScrollOffset(0, 100),
            scrolling_layer->CurrentScrollOffset());
        EXPECT_EQ(NULL, host_impl_->CurrentlyScrollingLayer());
        host_impl_->DidFinishImplFrame();
    }

    TEST_F(LayerTreeHostImplTest, InvalidLayerNotAddedToRasterQueue)
    {
        host_impl_->CreatePendingTree();

        Region empty_invalidation;
        scoped_refptr<RasterSource> raster_source_with_tiles(
            FakeDisplayListRasterSource::CreateFilled(gfx::Size(10, 10)));

        scoped_ptr<FakePictureLayerImpl> layer = FakePictureLayerImpl::Create(host_impl_->pending_tree(), 11);
        layer->SetBounds(gfx::Size(10, 10));
        layer->set_gpu_raster_max_texture_size(host_impl_->device_viewport_size());
        layer->SetDrawsContent(true);
        layer->tilings()->AddTiling(1.0f, raster_source_with_tiles);
        layer->UpdateRasterSource(raster_source_with_tiles, &empty_invalidation,
            nullptr);
        layer->tilings()->tiling_at(0)->set_resolution(
            TileResolution::HIGH_RESOLUTION);
        layer->tilings()->tiling_at(0)->CreateAllTilesForTesting();
        layer->tilings()->tiling_at(0)->ComputeTilePriorityRects(
            gfx::Rect(gfx::Size(10, 10)), 1.f, 1.0, Occlusion());
        host_impl_->pending_tree()->SetRootLayer(layer.Pass());

        FakePictureLayerImpl* root_layer = static_cast<FakePictureLayerImpl*>(
            host_impl_->pending_tree()->root_layer());

        root_layer->set_has_valid_tile_priorities(true);
        scoped_ptr<RasterTilePriorityQueue> non_empty_raster_priority_queue_all = host_impl_->BuildRasterQueue(TreePriority::SAME_PRIORITY_FOR_BOTH_TREES,
            RasterTilePriorityQueue::Type::ALL);
        EXPECT_FALSE(non_empty_raster_priority_queue_all->IsEmpty());

        root_layer->set_has_valid_tile_priorities(false);
        scoped_ptr<RasterTilePriorityQueue> empty_raster_priority_queue_all = host_impl_->BuildRasterQueue(TreePriority::SAME_PRIORITY_FOR_BOTH_TREES,
            RasterTilePriorityQueue::Type::ALL);
        EXPECT_TRUE(empty_raster_priority_queue_all->IsEmpty());
    }

    TEST_F(LayerTreeHostImplTest, DidBecomeActive)
    {
        host_impl_->CreatePendingTree();
        host_impl_->ActivateSyncTree();
        host_impl_->CreatePendingTree();

        LayerTreeImpl* pending_tree = host_impl_->pending_tree();

        scoped_ptr<FakePictureLayerImpl> pending_layer = FakePictureLayerImpl::Create(pending_tree, 10);
        FakePictureLayerImpl* raw_pending_layer = pending_layer.get();
        pending_tree->SetRootLayer(pending_layer.Pass());
        ASSERT_EQ(raw_pending_layer, pending_tree->root_layer());

        EXPECT_EQ(0u, raw_pending_layer->did_become_active_call_count());
        pending_tree->DidBecomeActive();
        EXPECT_EQ(1u, raw_pending_layer->did_become_active_call_count());

        scoped_ptr<FakePictureLayerImpl> mask_layer = FakePictureLayerImpl::Create(pending_tree, 11);
        FakePictureLayerImpl* raw_mask_layer = mask_layer.get();
        raw_pending_layer->SetMaskLayer(mask_layer.Pass());
        ASSERT_EQ(raw_mask_layer, raw_pending_layer->mask_layer());

        EXPECT_EQ(1u, raw_pending_layer->did_become_active_call_count());
        EXPECT_EQ(0u, raw_mask_layer->did_become_active_call_count());
        pending_tree->DidBecomeActive();
        EXPECT_EQ(2u, raw_pending_layer->did_become_active_call_count());
        EXPECT_EQ(1u, raw_mask_layer->did_become_active_call_count());

        scoped_ptr<FakePictureLayerImpl> replica_layer = FakePictureLayerImpl::Create(pending_tree, 12);
        scoped_ptr<FakePictureLayerImpl> replica_mask_layer = FakePictureLayerImpl::Create(pending_tree, 13);
        FakePictureLayerImpl* raw_replica_mask_layer = replica_mask_layer.get();
        replica_layer->SetMaskLayer(replica_mask_layer.Pass());
        raw_pending_layer->SetReplicaLayer(replica_layer.Pass());
        ASSERT_EQ(raw_replica_mask_layer,
            raw_pending_layer->replica_layer()->mask_layer());

        EXPECT_EQ(2u, raw_pending_layer->did_become_active_call_count());
        EXPECT_EQ(1u, raw_mask_layer->did_become_active_call_count());
        EXPECT_EQ(0u, raw_replica_mask_layer->did_become_active_call_count());
        pending_tree->DidBecomeActive();
        EXPECT_EQ(3u, raw_pending_layer->did_become_active_call_count());
        EXPECT_EQ(2u, raw_mask_layer->did_become_active_call_count());
        EXPECT_EQ(1u, raw_replica_mask_layer->did_become_active_call_count());
    }

    TEST_F(LayerTreeHostImplTest, WheelScrollWithPageScaleFactorOnInnerLayer)
    {
        LayerImpl* scroll_layer = SetupScrollAndContentsLayers(gfx::Size(100, 100));
        host_impl_->SetViewportSize(gfx::Size(50, 50));
        DrawFrame();

        EXPECT_EQ(scroll_layer, host_impl_->InnerViewportScrollLayer());

        float min_page_scale = 1.f, max_page_scale = 4.f;
        float page_scale_factor = 1.f;

        // The scroll deltas should have the page scale factor applied.
        {
            host_impl_->active_tree()->PushPageScaleFromMainThread(
                page_scale_factor, min_page_scale, max_page_scale);
            host_impl_->active_tree()->SetPageScaleOnActiveTree(page_scale_factor);
            scroll_layer->SetScrollDelta(gfx::Vector2d());

            float page_scale_delta = 2.f;
            host_impl_->ScrollBegin(gfx::Point(), InputHandler::GESTURE);
            host_impl_->PinchGestureBegin();
            host_impl_->PinchGestureUpdate(page_scale_delta, gfx::Point());
            host_impl_->PinchGestureEnd();
            host_impl_->ScrollEnd();

            gfx::Vector2dF scroll_delta(0, 5);
            EXPECT_EQ(InputHandler::SCROLL_STARTED,
                host_impl_->ScrollBegin(gfx::Point(), InputHandler::WHEEL));
            EXPECT_VECTOR_EQ(gfx::Vector2dF(), scroll_layer->CurrentScrollOffset());

            host_impl_->ScrollBy(gfx::Point(), scroll_delta);
            host_impl_->ScrollEnd();
            EXPECT_VECTOR_EQ(gfx::Vector2dF(0, 2.5),
                scroll_layer->CurrentScrollOffset());
        }
    }

    class LayerTreeHostImplCountingLostSurfaces : public LayerTreeHostImplTest {
    public:
        LayerTreeHostImplCountingLostSurfaces()
            : num_lost_surfaces_(0)
        {
        }
        void DidLoseOutputSurfaceOnImplThread() override { num_lost_surfaces_++; }

    protected:
        int num_lost_surfaces_;
    };

    TEST_F(LayerTreeHostImplCountingLostSurfaces, TwiceLostSurface)
    {
        // Really we just need at least one client notification each time
        // we go from having a valid output surface to not having a valid output
        // surface.
        EXPECT_EQ(0, num_lost_surfaces_);
        host_impl_->DidLoseOutputSurface();
        EXPECT_EQ(1, num_lost_surfaces_);
        host_impl_->DidLoseOutputSurface();
        EXPECT_LE(1, num_lost_surfaces_);
    }

    TEST_F(LayerTreeHostImplTest, RemoveUnreferencedRenderPass)
    {
        LayerTreeHostImpl::FrameData frame;
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass3 = frame.render_passes.back();
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass2 = frame.render_passes.back();
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass1 = frame.render_passes.back();

        pass1->SetNew(RenderPassId(1, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());
        pass2->SetNew(RenderPassId(2, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());
        pass3->SetNew(RenderPassId(3, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());

        frame.render_passes_by_id[pass1->id] = pass1;
        frame.render_passes_by_id[pass2->id] = pass2;
        frame.render_passes_by_id[pass3->id] = pass3;

        // Add a quad to each pass so they aren't empty.
        SolidColorDrawQuad* color_quad;
        color_quad = pass1->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->material = DrawQuad::SOLID_COLOR;
        color_quad = pass2->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->material = DrawQuad::SOLID_COLOR;
        color_quad = pass3->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->material = DrawQuad::SOLID_COLOR;

        // pass3 is referenced by pass2.
        RenderPassDrawQuad* rpdq = pass2->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        rpdq->material = DrawQuad::RENDER_PASS;
        rpdq->render_pass_id = pass3->id;

        // But pass2 is not referenced by pass1. So pass2 and pass3 should be culled.
        FakeLayerTreeHostImpl::RemoveRenderPasses(&frame);
        EXPECT_EQ(1u, frame.render_passes_by_id.size());
        EXPECT_TRUE(frame.render_passes_by_id[RenderPassId(1, 0)]);
        EXPECT_FALSE(frame.render_passes_by_id[RenderPassId(2, 0)]);
        EXPECT_FALSE(frame.render_passes_by_id[RenderPassId(3, 0)]);
        EXPECT_EQ(1u, frame.render_passes.size());
        EXPECT_EQ(RenderPassId(1, 0), frame.render_passes[0]->id);
    }

    TEST_F(LayerTreeHostImplTest, RemoveEmptyRenderPass)
    {
        LayerTreeHostImpl::FrameData frame;
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass3 = frame.render_passes.back();
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass2 = frame.render_passes.back();
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass1 = frame.render_passes.back();

        pass1->SetNew(RenderPassId(1, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());
        pass2->SetNew(RenderPassId(2, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());
        pass3->SetNew(RenderPassId(3, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());

        frame.render_passes_by_id[pass1->id] = pass1;
        frame.render_passes_by_id[pass2->id] = pass2;
        frame.render_passes_by_id[pass3->id] = pass3;

        // pass1 is not empty, but pass2 and pass3 are.
        SolidColorDrawQuad* color_quad;
        color_quad = pass1->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->material = DrawQuad::SOLID_COLOR;

        // pass3 is referenced by pass2.
        RenderPassDrawQuad* rpdq = pass2->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        rpdq->material = DrawQuad::RENDER_PASS;
        rpdq->render_pass_id = pass3->id;

        // pass2 is referenced by pass1.
        rpdq = pass1->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        rpdq->material = DrawQuad::RENDER_PASS;
        rpdq->render_pass_id = pass2->id;

        // Since pass3 is empty it should be removed. Then pass2 is empty too, and
        // should be removed.
        FakeLayerTreeHostImpl::RemoveRenderPasses(&frame);
        EXPECT_EQ(1u, frame.render_passes_by_id.size());
        EXPECT_TRUE(frame.render_passes_by_id[RenderPassId(1, 0)]);
        EXPECT_FALSE(frame.render_passes_by_id[RenderPassId(2, 0)]);
        EXPECT_FALSE(frame.render_passes_by_id[RenderPassId(3, 0)]);
        EXPECT_EQ(1u, frame.render_passes.size());
        EXPECT_EQ(RenderPassId(1, 0), frame.render_passes[0]->id);
        // The RenderPassDrawQuad should be removed from pass1.
        EXPECT_EQ(1u, pass1->quad_list.size());
        EXPECT_EQ(DrawQuad::SOLID_COLOR, pass1->quad_list.ElementAt(0)->material);
    }

    TEST_F(LayerTreeHostImplTest, DoNotRemoveEmptyRootRenderPass)
    {
        LayerTreeHostImpl::FrameData frame;
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass3 = frame.render_passes.back();
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass2 = frame.render_passes.back();
        frame.render_passes.push_back(RenderPass::Create());
        RenderPass* pass1 = frame.render_passes.back();

        pass1->SetNew(RenderPassId(1, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());
        pass2->SetNew(RenderPassId(2, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());
        pass3->SetNew(RenderPassId(3, 0), gfx::Rect(), gfx::Rect(), gfx::Transform());

        frame.render_passes_by_id[pass1->id] = pass1;
        frame.render_passes_by_id[pass2->id] = pass2;
        frame.render_passes_by_id[pass3->id] = pass3;

        // pass3 is referenced by pass2.
        RenderPassDrawQuad* rpdq = pass2->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        rpdq->material = DrawQuad::RENDER_PASS;
        rpdq->render_pass_id = pass3->id;

        // pass2 is referenced by pass1.
        rpdq = pass1->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        rpdq->material = DrawQuad::RENDER_PASS;
        rpdq->render_pass_id = pass2->id;

        // Since pass3 is empty it should be removed. Then pass2 is empty too, and
        // should be removed. Then pass1 is empty too, but it's the root so it should
        // not be removed.
        FakeLayerTreeHostImpl::RemoveRenderPasses(&frame);
        EXPECT_EQ(1u, frame.render_passes_by_id.size());
        EXPECT_TRUE(frame.render_passes_by_id[RenderPassId(1, 0)]);
        EXPECT_FALSE(frame.render_passes_by_id[RenderPassId(2, 0)]);
        EXPECT_FALSE(frame.render_passes_by_id[RenderPassId(3, 0)]);
        EXPECT_EQ(1u, frame.render_passes.size());
        EXPECT_EQ(RenderPassId(1, 0), frame.render_passes[0]->id);
        // The RenderPassDrawQuad should be removed from pass1.
        EXPECT_EQ(0u, pass1->quad_list.size());
    }

    class FakeVideoFrameController : public VideoFrameController {
    public:
        void OnBeginFrame(const BeginFrameArgs& args) override
        {
            begin_frame_args_ = args;
            did_draw_frame_ = false;
        }

        void DidDrawFrame() override { did_draw_frame_ = true; }

        const BeginFrameArgs& begin_frame_args() const { return begin_frame_args_; }

        bool did_draw_frame() const { return did_draw_frame_; }

    private:
        BeginFrameArgs begin_frame_args_;
        bool did_draw_frame_ = false;
    };

    TEST_F(LayerTreeHostImplTest, AddVideoFrameControllerInsideFrame)
    {
        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);
        FakeVideoFrameController controller;

        host_impl_->WillBeginImplFrame(begin_frame_args);
        EXPECT_FALSE(controller.begin_frame_args().IsValid());
        host_impl_->AddVideoFrameController(&controller);
        EXPECT_TRUE(controller.begin_frame_args().IsValid());
        host_impl_->DidFinishImplFrame();

        EXPECT_FALSE(controller.did_draw_frame());
        LayerTreeHostImpl::FrameData frame;
        host_impl_->DidDrawAllLayers(frame);
        EXPECT_TRUE(controller.did_draw_frame());

        controller.OnBeginFrame(begin_frame_args);
        EXPECT_FALSE(controller.did_draw_frame());
        host_impl_->RemoveVideoFrameController(&controller);
        host_impl_->DidDrawAllLayers(frame);
        EXPECT_FALSE(controller.did_draw_frame());
    }

    TEST_F(LayerTreeHostImplTest, AddVideoFrameControllerOutsideFrame)
    {
        BeginFrameArgs begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);
        FakeVideoFrameController controller;

        host_impl_->WillBeginImplFrame(begin_frame_args);
        host_impl_->DidFinishImplFrame();

        EXPECT_FALSE(controller.begin_frame_args().IsValid());
        host_impl_->AddVideoFrameController(&controller);
        EXPECT_FALSE(controller.begin_frame_args().IsValid());

        begin_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE);
        EXPECT_FALSE(controller.begin_frame_args().IsValid());
        host_impl_->WillBeginImplFrame(begin_frame_args);
        EXPECT_TRUE(controller.begin_frame_args().IsValid());

        EXPECT_FALSE(controller.did_draw_frame());
        LayerTreeHostImpl::FrameData frame;
        host_impl_->DidDrawAllLayers(frame);
        EXPECT_TRUE(controller.did_draw_frame());

        controller.OnBeginFrame(begin_frame_args);
        EXPECT_FALSE(controller.did_draw_frame());
        host_impl_->RemoveVideoFrameController(&controller);
        host_impl_->DidDrawAllLayers(frame);
        EXPECT_FALSE(controller.did_draw_frame());
    }

    TEST_F(LayerTreeHostImplTest, GpuRasterizationStatusModes)
    {
        EXPECT_FALSE(host_impl_->use_gpu_rasterization());

        host_impl_->SetHasGpuRasterizationTrigger(true);
        host_impl_->SetContentIsSuitableForGpuRasterization(true);
        EXPECT_EQ(GpuRasterizationStatus::ON, host_impl_->gpu_rasterization_status());
        EXPECT_TRUE(host_impl_->use_gpu_rasterization());

        host_impl_->SetHasGpuRasterizationTrigger(false);
        host_impl_->SetContentIsSuitableForGpuRasterization(true);
        EXPECT_EQ(GpuRasterizationStatus::OFF_VIEWPORT,
            host_impl_->gpu_rasterization_status());
        EXPECT_FALSE(host_impl_->use_gpu_rasterization());

        host_impl_->SetHasGpuRasterizationTrigger(true);
        host_impl_->SetContentIsSuitableForGpuRasterization(false);
        EXPECT_EQ(GpuRasterizationStatus::OFF_CONTENT,
            host_impl_->gpu_rasterization_status());
        EXPECT_FALSE(host_impl_->use_gpu_rasterization());
        EXPECT_FALSE(host_impl_->use_msaa());

        scoped_ptr<TestWebGraphicsContext3D> context_with_msaa = TestWebGraphicsContext3D::Create();
        context_with_msaa->SetMaxSamples(8);

        LayerTreeSettings msaaSettings = GpuRasterizationEnabledSettings();
        msaaSettings.gpu_rasterization_msaa_sample_count = 4;
        EXPECT_TRUE(CreateHostImpl(
            msaaSettings, FakeOutputSurface::Create3d(context_with_msaa.Pass())));
        host_impl_->SetHasGpuRasterizationTrigger(true);
        host_impl_->SetContentIsSuitableForGpuRasterization(false);
        EXPECT_EQ(GpuRasterizationStatus::MSAA_CONTENT,
            host_impl_->gpu_rasterization_status());
        EXPECT_TRUE(host_impl_->use_gpu_rasterization());
        EXPECT_TRUE(host_impl_->use_msaa());

        LayerTreeSettings settings = DefaultSettings();
        settings.gpu_rasterization_enabled = false;
        EXPECT_TRUE(CreateHostImpl(settings, FakeOutputSurface::Create3d()));
        host_impl_->SetHasGpuRasterizationTrigger(true);
        host_impl_->SetContentIsSuitableForGpuRasterization(true);
        EXPECT_EQ(GpuRasterizationStatus::OFF_DEVICE,
            host_impl_->gpu_rasterization_status());
        EXPECT_FALSE(host_impl_->use_gpu_rasterization());

        settings.gpu_rasterization_forced = true;
        EXPECT_TRUE(CreateHostImpl(settings, FakeOutputSurface::Create3d()));

        host_impl_->SetHasGpuRasterizationTrigger(false);
        host_impl_->SetContentIsSuitableForGpuRasterization(false);
        EXPECT_EQ(GpuRasterizationStatus::ON_FORCED,
            host_impl_->gpu_rasterization_status());
        EXPECT_TRUE(host_impl_->use_gpu_rasterization());
    }

    // A mock output surface which lets us detect calls to ForceReclaimResources.
    class MockReclaimResourcesOutputSurface : public FakeOutputSurface {
    public:
        static scoped_ptr<MockReclaimResourcesOutputSurface> Create3d()
        {
            return make_scoped_ptr(new MockReclaimResourcesOutputSurface(
                TestContextProvider::Create(), TestContextProvider::CreateWorker(),
                false));
        }

        MOCK_METHOD0(ForceReclaimResources, void());

    protected:
        MockReclaimResourcesOutputSurface(
            scoped_refptr<ContextProvider> context_provider,
            scoped_refptr<ContextProvider> worker_context_provider,
            bool delegated_rendering)
            : FakeOutputSurface(context_provider,
                worker_context_provider,
                delegated_rendering)
        {
        }
    };

    // Display::Draw (and the planned Display Scheduler) currently rely on resources
    // being reclaimed to block drawing between BeginCommit / Swap. This test
    // ensures that BeginCommit triggers ForceReclaimResources. See
    // crbug.com/489515.
    TEST_F(LayerTreeHostImplTest, BeginCommitReclaimsResources)
    {
        scoped_ptr<MockReclaimResourcesOutputSurface> output_surface(
            MockReclaimResourcesOutputSurface::Create3d());
        // Hold an unowned pointer to the output surface to use for mock expectations.
        MockReclaimResourcesOutputSurface* mock_output_surface = output_surface.get();

        CreateHostImpl(DefaultSettings(), output_surface.Pass());
        EXPECT_CALL(*mock_output_surface, ForceReclaimResources()).Times(1);
        host_impl_->BeginCommit();
    }

} // namespace
} // namespace cc
