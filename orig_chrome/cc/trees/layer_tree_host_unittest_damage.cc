// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_host.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/location.h"
#include "base/time/time.h"
#include "cc/layers/solid_color_layer.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/fake_painted_scrollbar_layer.h"
#include "cc/test/fake_picture_layer.h"
#include "cc/test/layer_tree_test.h"
#include "cc/trees/damage_tracker.h"
#include "cc/trees/layer_tree_impl.h"

namespace cc {
namespace {

    // These tests deal with damage tracking.
    class LayerTreeHostDamageTest : public LayerTreeTest {
    };

    // LayerTreeHost::SetNeedsRedraw should damage the whole viewport.
    class LayerTreeHostDamageTestSetNeedsRedraw
        : public LayerTreeHostDamageTest {
        void SetupTree() override
        {
            // Viewport is 10x10.
            scoped_refptr<FakePictureLayer> root = FakePictureLayer::Create(layer_settings(), &client_);
            root->SetBounds(gfx::Size(10, 10));

            layer_tree_host()->SetRootLayer(root);
            LayerTreeHostDamageTest::SetupTree();
        }

        void BeginTest() override
        {
            draw_count_ = 0;
            PostSetNeedsCommitToMainThread();
        }

        void DidCommitAndDrawFrame() override
        {
            switch (layer_tree_host()->source_frame_number()) {
            case 1:
                layer_tree_host()->SetNeedsRedraw();
                break;
            }
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* impl,
            LayerTreeHostImpl::FrameData* frame_data,
            DrawResult draw_result) override
        {
            EXPECT_EQ(DRAW_SUCCESS, draw_result);

            RenderSurfaceImpl* root_surface = impl->active_tree()->root_layer()->render_surface();
            gfx::Rect root_damage = root_surface->damage_tracker()->current_damage_rect();

            switch (draw_count_) {
            case 0:
                // The first frame has full damage.
                EXPECT_EQ(gfx::Rect(10, 10), root_damage);
                break;
            case 1:
                // The second frame has full damage.
                EXPECT_EQ(gfx::Rect(10, 10), root_damage);
                EndTest();
                break;
            case 2:
                NOTREACHED();
            }

            ++draw_count_;
            return draw_result;
        }

        void AfterTest() override { }

        int draw_count_;
        FakeContentLayerClient client_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostDamageTestSetNeedsRedraw);

    // LayerTreeHost::SetViewportSize should damage the whole viewport.
    class LayerTreeHostDamageTestSetViewportSize
        : public LayerTreeHostDamageTest {
        void SetupTree() override
        {
            // Viewport is 10x10.
            scoped_refptr<FakePictureLayer> root = FakePictureLayer::Create(layer_settings(), &client_);
            root->SetBounds(gfx::Size(10, 10));

            layer_tree_host()->SetRootLayer(root);
            LayerTreeHostDamageTest::SetupTree();
        }

        void BeginTest() override
        {
            draw_count_ = 0;
            PostSetNeedsCommitToMainThread();
        }

        void DidCommitAndDrawFrame() override
        {
            switch (layer_tree_host()->source_frame_number()) {
            case 1:
                layer_tree_host()->SetViewportSize(gfx::Size(15, 15));
                break;
            }
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* impl,
            LayerTreeHostImpl::FrameData* frame_data,
            DrawResult draw_result) override
        {
            EXPECT_EQ(DRAW_SUCCESS, draw_result);

            RenderSurfaceImpl* root_surface = impl->active_tree()->root_layer()->render_surface();
            gfx::Rect root_damage = root_surface->damage_tracker()->current_damage_rect();

            switch (draw_count_) {
            case 0:
                // The first frame has full damage.
                EXPECT_EQ(gfx::Rect(10, 10), root_damage);
                break;
            case 1:
                // The second frame has full damage.
                EXPECT_EQ(gfx::Rect(15, 15), root_damage);
                EndTest();
                break;
            case 2:
                NOTREACHED();
            }

            ++draw_count_;
            return draw_result;
        }

        void AfterTest() override { }

        int draw_count_;
        FakeContentLayerClient client_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostDamageTestSetViewportSize);

    class LayerTreeHostDamageTestNoDamageDoesNotSwap
        : public LayerTreeHostDamageTest {
        void BeginTest() override
        {
            expect_swap_and_succeed_ = 0;
            did_swaps_ = 0;
            did_swap_and_succeed_ = 0;
            PostSetNeedsCommitToMainThread();
        }

        void SetupTree() override
        {
            scoped_refptr<FakePictureLayer> root = FakePictureLayer::Create(layer_settings(), &client_);
            root->SetBounds(gfx::Size(10, 10));

            // Most of the layer isn't visible.
            content_ = FakePictureLayer::Create(layer_settings(), &client_);
            content_->SetBounds(gfx::Size(2000, 100));
            root->AddChild(content_);

            layer_tree_host()->SetRootLayer(root);
            LayerTreeHostDamageTest::SetupTree();
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* host_impl,
            LayerTreeHostImpl::FrameData* frame_data,
            DrawResult draw_result) override
        {
            EXPECT_EQ(DRAW_SUCCESS, draw_result);

            int source_frame = host_impl->active_tree()->source_frame_number();
            switch (source_frame) {
            case 0:
                // The first frame has damage, so we should draw and swap.
                ++expect_swap_and_succeed_;
                break;
            case 1:
                // The second frame has no damage, so we should not draw and swap.
                break;
            case 2:
                // The third frame has damage again, so we should draw and swap.
                ++expect_swap_and_succeed_;
                break;
            case 3:
                // The fourth frame has no visible damage, so we should not draw and
                // swap.
                EndTest();
                break;
            }
            return draw_result;
        }

        void SwapBuffersOnThread(LayerTreeHostImpl* host_impl, bool result) override
        {
            ++did_swaps_;
            if (result)
                ++did_swap_and_succeed_;
            EXPECT_EQ(expect_swap_and_succeed_, did_swap_and_succeed_);
        }

        void DidCommit() override
        {
            int next_frame = layer_tree_host()->source_frame_number();
            switch (next_frame) {
            case 1:
                layer_tree_host()->SetNeedsCommit();
                break;
            case 2:
                // Cause visible damage.
                content_->SetNeedsDisplayRect(
                    gfx::Rect(layer_tree_host()->device_viewport_size()));
                break;
            case 3:
                // Cause non-visible damage.
                content_->SetNeedsDisplayRect(gfx::Rect(1990, 1990, 10, 10));
                layer_tree_host()->SetNeedsCommit();
                break;
            }
        }

        void AfterTest() override
        {
            EXPECT_EQ(4, did_swaps_);
            EXPECT_EQ(2, expect_swap_and_succeed_);
            EXPECT_EQ(expect_swap_and_succeed_, did_swap_and_succeed_);
        }

        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> content_;
        int expect_swap_and_succeed_;
        int did_swaps_;
        int did_swap_and_succeed_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostDamageTestNoDamageDoesNotSwap);

    class LayerTreeHostDamageTestForcedFullDamage : public LayerTreeHostDamageTest {
        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void SetupTree() override
        {
            root_ = FakePictureLayer::Create(layer_settings(), &client_);
            child_ = FakePictureLayer::Create(layer_settings(), &client_);

            root_->SetBounds(gfx::Size(500, 500));
            child_->SetPosition(gfx::Point(100, 100));
            child_->SetBounds(gfx::Size(30, 30));

            root_->AddChild(child_);
            layer_tree_host()->SetRootLayer(root_);
            LayerTreeHostDamageTest::SetupTree();
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* host_impl,
            LayerTreeHostImpl::FrameData* frame_data,
            DrawResult draw_result) override
        {
            EXPECT_EQ(DRAW_SUCCESS, draw_result);

            RenderSurfaceImpl* root_surface = host_impl->active_tree()->root_layer()->render_surface();
            gfx::Rect root_damage = root_surface->damage_tracker()->current_damage_rect();
            root_damage.Intersect(root_surface->content_rect());

            int source_frame = host_impl->active_tree()->source_frame_number();
            switch (source_frame) {
            case 0:
                // The first frame draws and clears any damage.
                EXPECT_EQ(root_surface->content_rect(), root_damage);
                EXPECT_FALSE(frame_data->has_no_damage);
                break;
            case 1:
                // If we get a frame without damage then we don't draw.
                EXPECT_EQ(gfx::Rect(), root_damage);
                EXPECT_TRUE(frame_data->has_no_damage);

                // Then we set full damage for the next frame.
                host_impl->SetFullRootLayerDamage();
                break;
            case 2:
                // The whole frame should be damaged as requested.
                EXPECT_EQ(root_surface->content_rect(), root_damage);
                EXPECT_FALSE(frame_data->has_no_damage);

                // Just a part of the next frame should be damaged.
                child_damage_rect_ = gfx::Rect(10, 11, 12, 13);
                break;
            case 3:
                // The update rect in the child should be damaged and the damaged area
                // should match the invalidation.
                EXPECT_EQ(gfx::Rect(100 + 10, 100 + 11, 12, 13), root_damage);
                EXPECT_FALSE(frame_data->has_no_damage);

                // If we damage part of the frame, but also damage the full
                // frame, then the whole frame should be damaged.
                child_damage_rect_ = gfx::Rect(10, 11, 12, 13);
                host_impl->SetFullRootLayerDamage();
                break;
            case 4:
                // The whole frame is damaged.
                EXPECT_EQ(root_surface->content_rect(), root_damage);
                EXPECT_FALSE(frame_data->has_no_damage);

                EndTest();
                break;
            }
            return draw_result;
        }

        void DidCommitAndDrawFrame() override
        {
            if (!TestEnded())
                layer_tree_host()->SetNeedsCommit();

            if (!child_damage_rect_.IsEmpty()) {
                child_->SetNeedsDisplayRect(child_damage_rect_);
                child_damage_rect_ = gfx::Rect();
            }
        }

        void AfterTest() override { }

        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> root_;
        scoped_refptr<FakePictureLayer> child_;
        gfx::Rect child_damage_rect_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostDamageTestForcedFullDamage);

    class LayerTreeHostScrollbarDamageTest : public LayerTreeHostDamageTest {
        void SetupTree() override
        {
            scoped_refptr<Layer> root_layer = Layer::Create(layer_settings());
            root_layer->SetBounds(gfx::Size(400, 400));
            root_layer->SetMasksToBounds(true);
            layer_tree_host()->SetRootLayer(root_layer);

            scoped_refptr<Layer> scroll_clip_layer = Layer::Create(layer_settings());
            scoped_refptr<Layer> content_layer = FakePictureLayer::Create(layer_settings(), &client_);
            content_layer->SetScrollClipLayerId(scroll_clip_layer->id());
            content_layer->SetScrollOffset(gfx::ScrollOffset(10, 20));
            content_layer->SetBounds(gfx::Size(100, 200));
            content_layer->SetIsDrawable(true);
            scroll_clip_layer->SetBounds(
                gfx::Size(content_layer->bounds().width() - 30,
                    content_layer->bounds().height() - 50));
            scroll_clip_layer->AddChild(content_layer);
            root_layer->AddChild(scroll_clip_layer);

            scoped_refptr<Layer> scrollbar_layer = FakePaintedScrollbarLayer::Create(
                layer_settings(), false, true, content_layer->id());
            scrollbar_layer->SetPosition(gfx::Point(300, 300));
            scrollbar_layer->SetBounds(gfx::Size(10, 100));
            scrollbar_layer->ToScrollbarLayer()->SetClipLayer(scroll_clip_layer->id());
            scrollbar_layer->ToScrollbarLayer()->SetScrollLayer(content_layer->id());
            root_layer->AddChild(scrollbar_layer);

            gfx::RectF content_rect(content_layer->position(),
                gfx::SizeF(content_layer->bounds()));
            gfx::RectF scrollbar_rect(scrollbar_layer->position(),
                gfx::SizeF(scrollbar_layer->bounds()));
            EXPECT_FALSE(content_rect.Intersects(scrollbar_rect));

            LayerTreeHostDamageTest::SetupTree();
        }

    private:
        FakeContentLayerClient client_;
    };

    class LayerTreeHostDamageTestScrollbarDoesDamage
        : public LayerTreeHostScrollbarDamageTest {
        void BeginTest() override
        {
            did_swaps_ = 0;
            PostSetNeedsCommitToMainThread();
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* host_impl,
            LayerTreeHostImpl::FrameData* frame_data,
            DrawResult draw_result) override
        {
            EXPECT_EQ(DRAW_SUCCESS, draw_result);
            RenderSurfaceImpl* root_surface = host_impl->active_tree()->root_layer()->render_surface();
            gfx::Rect root_damage = root_surface->damage_tracker()->current_damage_rect();
            root_damage.Intersect(root_surface->content_rect());
            switch (did_swaps_) {
            case 0:
                // The first frame has damage, so we should draw and swap.
                break;
            case 1:
                // The second frame should not damage the scrollbars.
                EXPECT_FALSE(root_damage.Intersects(gfx::Rect(300, 300, 10, 100)));
                break;
            case 2:
                // The third frame should damage the scrollbars.
                EXPECT_TRUE(root_damage.Contains(gfx::Rect(300, 300, 10, 100)));
                break;
            case 3:
                // The fourth frame should damage the scrollbars.
                EXPECT_TRUE(root_damage.Contains(gfx::Rect(300, 300, 10, 100)));
                EndTest();
                break;
            }
            return draw_result;
        }

        void SwapBuffersOnThread(LayerTreeHostImpl* host_impl, bool result) override
        {
            ++did_swaps_;
            EXPECT_TRUE(result);
            LayerImpl* root = host_impl->active_tree()->root_layer();
            LayerImpl* scroll_clip_layer = root->children()[0];
            LayerImpl* scroll_layer = scroll_clip_layer->children()[0];
            switch (did_swaps_) {
            case 1:
                // Test that modifying the position of the content layer (not
                // scrolling) won't damage the scrollbar.
                MainThreadTaskRunner()->PostTask(
                    FROM_HERE, base::Bind(&LayerTreeHostDamageTestScrollbarDoesDamage::ModifyContentLayerPosition, base::Unretained(this)));
                break;
            case 2:
                scroll_layer->ScrollBy(gfx::Vector2dF(10.f, 10.f));
                host_impl->SetNeedsRedraw();
                break;
            case 3:
                // We will resize the content layer, on the main thread.
                MainThreadTaskRunner()->PostTask(
                    FROM_HERE,
                    base::Bind(
                        &LayerTreeHostDamageTestScrollbarDoesDamage::ResizeScrollLayer,
                        base::Unretained(this)));
                break;
            }
        }

        void ModifyContentLayerPosition()
        {
            EXPECT_EQ(1, did_swaps_);
            Layer* root = layer_tree_host()->root_layer();
            Layer* scroll_clip_layer = root->child_at(0);
            Layer* scroll_layer = scroll_clip_layer->child_at(0);
            scroll_layer->SetPosition(gfx::Point(10, 10));
        }

        void ResizeScrollLayer()
        {
            EXPECT_EQ(3, did_swaps_);
            Layer* root = layer_tree_host()->root_layer();
            Layer* scroll_clip_layer = root->child_at(0);
            Layer* scroll_layer = scroll_clip_layer->child_at(0);
            scroll_layer->SetBounds(
                gfx::Size(root->bounds().width() + 60, root->bounds().height() + 100));
        }

        void AfterTest() override { EXPECT_EQ(4, did_swaps_); }

        int did_swaps_;
    };

    MULTI_THREAD_TEST_F(LayerTreeHostDamageTestScrollbarDoesDamage);

    class LayerTreeHostDamageTestScrollbarCommitDoesNoDamage
        : public LayerTreeHostScrollbarDamageTest {
        void BeginTest() override
        {
            did_swaps_ = 0;
            PostSetNeedsCommitToMainThread();
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* host_impl,
            LayerTreeHostImpl::FrameData* frame_data,
            DrawResult draw_result) override
        {
            EXPECT_EQ(DRAW_SUCCESS, draw_result);
            RenderSurfaceImpl* root_surface = host_impl->active_tree()->root_layer()->render_surface();
            gfx::Rect root_damage = root_surface->damage_tracker()->current_damage_rect();
            root_damage.Intersect(root_surface->content_rect());
            int frame = host_impl->active_tree()->source_frame_number();
            switch (did_swaps_) {
            case 0:
                // The first frame has damage, so we should draw and swap.
                EXPECT_EQ(0, frame);
                break;
            case 1:
                // The second frame has scrolled, so the scrollbar should be damaged.
                EXPECT_EQ(0, frame);
                EXPECT_TRUE(root_damage.Contains(gfx::Rect(300, 300, 10, 100)));
                break;
            case 2:
                // The third frame (after the commit) has no changes, so it shouldn't.
                EXPECT_EQ(1, frame);
                EXPECT_FALSE(root_damage.Intersects(gfx::Rect(300, 300, 10, 100)));
                break;
            default:
                NOTREACHED();
                break;
            }
            return draw_result;
        }

        void SwapBuffersOnThread(LayerTreeHostImpl* host_impl, bool result) override
        {
            ++did_swaps_;
            EXPECT_TRUE(result);
            LayerImpl* root = host_impl->active_tree()->root_layer();
            LayerImpl* scroll_clip_layer = root->children()[0];
            LayerImpl* scroll_layer = scroll_clip_layer->children()[0];
            switch (did_swaps_) {
            case 1:
                // Scroll on the thread.  This should damage the scrollbar for the
                // next draw on the thread.
                scroll_layer->ScrollBy(gfx::Vector2dF(10.f, 10.f));
                host_impl->SetNeedsRedraw();
                break;
            case 2:
                // Forcibly send the scroll to the main thread.
                PostSetNeedsCommitToMainThread();
                break;
            case 3:
                // First swap after second commit.
                EndTest();
                break;
            default:
                NOTREACHED();
                break;
            }
        }

        void AfterTest() override { EXPECT_EQ(3, did_swaps_); }

        int did_swaps_;
    };

    MULTI_THREAD_TEST_F(LayerTreeHostDamageTestScrollbarCommitDoesNoDamage);

} // namespace
} // namespace cc
