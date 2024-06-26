// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/scrollbar_animation_controller_thinning.h"

#include "cc/layers/solid_color_scrollbar_layer_impl.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/geometry_test_utils.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/layer_tree_impl.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class ScrollbarAnimationControllerThinningTest
        : public testing::Test,
          public ScrollbarAnimationControllerClient {
    public:
        ScrollbarAnimationControllerThinningTest()
            : host_impl_(&proxy_, &shared_bitmap_manager_, &task_graph_runner_)
        {
        }

        void StartAnimatingScrollbarAnimationController(
            ScrollbarAnimationController* controller) override
        {
            is_animating_ = true;
        }
        void StopAnimatingScrollbarAnimationController(
            ScrollbarAnimationController* controller) override
        {
            is_animating_ = false;
        }
        void PostDelayedScrollbarAnimationTask(const base::Closure& start_fade,
            base::TimeDelta delay) override
        {
            start_fade_ = start_fade;
            delay_ = delay;
        }
        void SetNeedsRedrawForScrollbarAnimation() override
        {
            did_request_redraw_ = true;
        }

    protected:
        void SetUp() override
        {
            scoped_ptr<LayerImpl> scroll_layer = LayerImpl::Create(host_impl_.active_tree(), 1);
            clip_layer_ = LayerImpl::Create(host_impl_.active_tree(), 3);
            scroll_layer->SetScrollClipLayer(clip_layer_->id());
            LayerImpl* scroll_layer_ptr = scroll_layer.get();
            clip_layer_->AddChild(scroll_layer.Pass());

            const int kId = 2;
            const int kThumbThickness = 10;
            const int kTrackStart = 0;
            const bool kIsLeftSideVerticalScrollbar = false;
            const bool kIsOverlayScrollbar = true;
            scrollbar_layer_ = SolidColorScrollbarLayerImpl::Create(host_impl_.active_tree(),
                kId,
                HORIZONTAL,
                kThumbThickness,
                kTrackStart,
                kIsLeftSideVerticalScrollbar,
                kIsOverlayScrollbar);

            scrollbar_layer_->SetScrollLayerAndClipLayerByIds(scroll_layer_ptr->id(),
                clip_layer_->id());
            clip_layer_->SetBounds(gfx::Size(100, 100));
            scroll_layer_ptr->SetBounds(gfx::Size(200, 200));

            scrollbar_controller_ = ScrollbarAnimationControllerThinning::Create(
                scroll_layer_ptr, this, base::TimeDelta::FromSeconds(2),
                base::TimeDelta::FromSeconds(5), base::TimeDelta::FromSeconds(3));
        }

        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeLayerTreeHostImpl host_impl_;
        scoped_ptr<ScrollbarAnimationControllerThinning> scrollbar_controller_;
        scoped_ptr<LayerImpl> clip_layer_;
        scoped_ptr<SolidColorScrollbarLayerImpl> scrollbar_layer_;

        base::Closure start_fade_;
        base::TimeDelta delay_;
        bool is_animating_;
        bool did_request_redraw_;
    };

    // Check initialization of scrollbar.
    TEST_F(ScrollbarAnimationControllerThinningTest, Idle)
    {
        scrollbar_controller_->Animate(base::TimeTicks());
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());
    }

    // Check that scrollbar disappears when the layer becomes non-scrollable.
    TEST_F(ScrollbarAnimationControllerThinningTest, HideOnResize)
    {
        LayerImpl* scroll_layer = host_impl_.active_tree()->LayerById(1);
        ASSERT_TRUE(scroll_layer);
        EXPECT_EQ(gfx::Size(200, 200), scroll_layer->bounds());

        EXPECT_EQ(HORIZONTAL, scrollbar_layer_->orientation());

        // Shrink along X axis, horizontal scrollbar should appear.
        clip_layer_->SetBounds(gfx::Size(100, 200));
        EXPECT_EQ(gfx::Size(100, 200), clip_layer_->bounds());

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();

        // Shrink along Y axis and expand along X, horizontal scrollbar
        // should disappear.
        clip_layer_->SetBounds(gfx::Size(200, 100));
        EXPECT_EQ(gfx::Size(200, 100), clip_layer_->bounds());

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();
    }

    // Scroll content. Confirm the scrollbar gets dark and then becomes light
    // after stopping.
    TEST_F(ScrollbarAnimationControllerThinningTest, AwakenByProgrammaticScroll)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        // Scrollbar doesn't change size if triggered by scroll.
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        start_fade_.Run();

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Subsequent scroll restarts animation.
        scrollbar_controller_->DidScrollUpdate(false);

        start_fade_.Run();

        time += base::TimeDelta::FromSeconds(2);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.9f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());
    }

    // Initiate a scroll when the pointer is already near the scrollbar. It should
    // remain thick.
    TEST_F(ScrollbarAnimationControllerThinningTest, ScrollWithMouseNear)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);

        scrollbar_controller_->DidMouseMoveNear(1);
        scrollbar_controller_->Animate(time);
        time += base::TimeDelta::FromSeconds(3);

        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        scrollbar_controller_->DidScrollUpdate(false);
        start_fade_.Run();
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        // Scrollbar should still be thick.
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(5);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());
    }

    // Move the pointer near the scrollbar. Confirm it gets thick and narrow when
    // moved away.
    TEST_F(ScrollbarAnimationControllerThinningTest, MouseNear)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidMouseMoveNear(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Should animate to thickened but not darken.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.6f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Subsequent moves should not change anything.
        scrollbar_controller_->DidMouseMoveNear(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Now move away from bar.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidMouseMoveNear(26);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Animate to narrow.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.6f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());
    }

    // Move the pointer over the scrollbar. Make sure it gets thick and dark
    // and that it gets thin and light when moved away.
    TEST_F(ScrollbarAnimationControllerThinningTest, MouseOver)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidMouseMoveNear(0);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Should animate to thickened and darkened.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.6f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.9f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Subsequent moves should not change anything.
        scrollbar_controller_->DidMouseMoveNear(0);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Now move away from bar.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidMouseMoveNear(26);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Animate to narrow.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.9f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.6f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());
    }

    // First move the pointer near the scrollbar, then over it, then back near
    // then far away. Confirm that first the bar gets thick, then dark, then light,
    // then narrow.
    TEST_F(ScrollbarAnimationControllerThinningTest, MouseNearThenOver)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidMouseMoveNear(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.4f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Should animate to thickened but not darken.
        time += base::TimeDelta::FromSeconds(3);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // Now move over.
        scrollbar_controller_->DidMouseMoveNear(0);
        scrollbar_controller_->Animate(time);

        // Should animate to darkened.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.9f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());

        // This is tricky. The DidMouseMoveOffScrollbar() is sent before the
        // subsequent DidMouseMoveNear(), if the mouse moves in that direction.
        // This results in the thumb thinning. We want to make sure that when the
        // thumb starts expanding it doesn't first narrow to the idle thinness.
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidMouseMoveOffScrollbar();
        scrollbar_controller_->Animate(time);

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.9f, scrollbar_layer_->opacity());
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->thumb_thickness_scale_factor());

        scrollbar_controller_->DidMouseMoveNear(1);
        scrollbar_controller_->Animate(time);
        // A new animation is kicked off.

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        // We will initiate the narrowing again, but it won't get decremented until
        // the new animation catches up to it.
        EXPECT_FLOAT_EQ(0.9f, scrollbar_layer_->opacity());
        // Now the thickness should be increasing, but it shouldn't happen until the
        // animation catches up.
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->opacity());
        // The thickness now gets big again.
        EXPECT_FLOAT_EQ(0.8f, scrollbar_layer_->thumb_thickness_scale_factor());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.7f, scrollbar_layer_->opacity());
        // The thickness now gets big again.
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->thumb_thickness_scale_factor());
    }

} // namespace
} // namespace cc
