// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/scrollbar_animation_controller_linear_fade.h"

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

    class ScrollbarAnimationControllerLinearFadeTest
        : public testing::Test,
          public ScrollbarAnimationControllerClient {
    public:
        ScrollbarAnimationControllerLinearFadeTest()
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
            const int kThumbThickness = 10;
            const int kTrackStart = 0;
            const bool kIsLeftSideVerticalScrollbar = false;
            const bool kIsOverlayScrollbar = true; // Allow opacity animations.

            scoped_ptr<LayerImpl> scroll_layer = LayerImpl::Create(host_impl_.active_tree(), 1);
            scrollbar_layer_ = SolidColorScrollbarLayerImpl::Create(host_impl_.active_tree(),
                2,
                orientation(),
                kThumbThickness,
                kTrackStart,
                kIsLeftSideVerticalScrollbar,
                kIsOverlayScrollbar);
            clip_layer_ = LayerImpl::Create(host_impl_.active_tree(), 3);
            scroll_layer->SetScrollClipLayer(clip_layer_->id());
            LayerImpl* scroll_layer_ptr = scroll_layer.get();
            clip_layer_->AddChild(scroll_layer.Pass());

            scrollbar_layer_->SetScrollLayerAndClipLayerByIds(scroll_layer_ptr->id(),
                clip_layer_->id());
            clip_layer_->SetBounds(gfx::Size(100, 100));
            scroll_layer_ptr->SetBounds(gfx::Size(200, 200));

            scrollbar_controller_ = ScrollbarAnimationControllerLinearFade::Create(
                scroll_layer_ptr, this, base::TimeDelta::FromSeconds(2),
                base::TimeDelta::FromSeconds(5), base::TimeDelta::FromSeconds(3));
        }

        virtual ScrollbarOrientation orientation() const { return HORIZONTAL; }

        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeLayerTreeHostImpl host_impl_;
        scoped_ptr<ScrollbarAnimationControllerLinearFade> scrollbar_controller_;
        scoped_ptr<LayerImpl> clip_layer_;
        scoped_ptr<SolidColorScrollbarLayerImpl> scrollbar_layer_;

        base::Closure start_fade_;
        base::TimeDelta delay_;
        bool is_animating_;
        bool did_request_redraw_;
    };

    class VerticalScrollbarAnimationControllerLinearFadeTest
        : public ScrollbarAnimationControllerLinearFadeTest {
    protected:
        ScrollbarOrientation orientation() const override { return VERTICAL; }
    };

    TEST_F(ScrollbarAnimationControllerLinearFadeTest, DelayAnimationOnResize)
    {
        scrollbar_layer_->SetOpacity(0.0f);
        scrollbar_controller_->DidScrollBegin();
        scrollbar_controller_->DidScrollUpdate(true);
        scrollbar_controller_->DidScrollEnd();
        // Normal Animation delay of 2 seconds.
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_EQ(delay_, base::TimeDelta::FromSeconds(2));

        scrollbar_layer_->SetOpacity(0.0f);
        scrollbar_controller_->DidScrollUpdate(true);
        // Delay animation on resize to 5 seconds.
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        EXPECT_EQ(delay_, base::TimeDelta::FromSeconds(5));
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest, HiddenInBegin)
    {
        scrollbar_layer_->SetOpacity(0.0f);
        scrollbar_controller_->Animate(base::TimeTicks());
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest,
        HiddenAfterNonScrollingGesture)
    {
        scrollbar_layer_->SetOpacity(0.0f);
        scrollbar_controller_->DidScrollBegin();

        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(100);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());
        scrollbar_controller_->DidScrollEnd();

        EXPECT_TRUE(start_fade_.Equals(base::Closure()));

        time += base::TimeDelta::FromSeconds(100);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest, HideOnResize)
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

    TEST_F(VerticalScrollbarAnimationControllerLinearFadeTest, HideOnResize)
    {
        LayerImpl* scroll_layer = host_impl_.active_tree()->LayerById(1);
        ASSERT_TRUE(scroll_layer);
        EXPECT_EQ(gfx::Size(200, 200), scroll_layer->bounds());

        EXPECT_EQ(VERTICAL, scrollbar_layer_->orientation());

        // Shrink along X axis, vertical scrollbar should remain invisible.
        clip_layer_->SetBounds(gfx::Size(100, 200));
        EXPECT_EQ(gfx::Size(100, 200), clip_layer_->bounds());

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();

        // Shrink along Y axis and expand along X, vertical scrollbar should appear.
        clip_layer_->SetBounds(gfx::Size(200, 100));
        EXPECT_EQ(gfx::Size(200, 100), clip_layer_->bounds());

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest,
        HideOnUserNonScrollableHorz)
    {
        EXPECT_EQ(HORIZONTAL, scrollbar_layer_->orientation());

        LayerImpl* scroll_layer = host_impl_.active_tree()->LayerById(1);
        ASSERT_TRUE(scroll_layer);
        scroll_layer->set_user_scrollable_horizontal(false);

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest,
        ShowOnUserNonScrollableVert)
    {
        EXPECT_EQ(HORIZONTAL, scrollbar_layer_->orientation());

        LayerImpl* scroll_layer = host_impl_.active_tree()->LayerById(1);
        ASSERT_TRUE(scroll_layer);
        scroll_layer->set_user_scrollable_vertical(false);

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();
    }

    TEST_F(VerticalScrollbarAnimationControllerLinearFadeTest,
        HideOnUserNonScrollableVert)
    {
        EXPECT_EQ(VERTICAL, scrollbar_layer_->orientation());

        LayerImpl* scroll_layer = host_impl_.active_tree()->LayerById(1);
        ASSERT_TRUE(scroll_layer);
        scroll_layer->set_user_scrollable_vertical(false);

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();
    }

    TEST_F(VerticalScrollbarAnimationControllerLinearFadeTest,
        ShowOnUserNonScrollableHorz)
    {
        EXPECT_EQ(VERTICAL, scrollbar_layer_->orientation());

        LayerImpl* scroll_layer = host_impl_.active_tree()->LayerById(1);
        ASSERT_TRUE(scroll_layer);
        scroll_layer->set_user_scrollable_horizontal(false);

        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest, AwakenByScrollingGesture)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollBegin();

        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        EXPECT_TRUE(start_fade_.Equals(base::Closure()));

        time += base::TimeDelta::FromSeconds(100);

        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());
        scrollbar_controller_->DidScrollEnd();
        start_fade_.Run();

        time += base::TimeDelta::FromSeconds(2);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);

        scrollbar_controller_->DidScrollBegin();
        scrollbar_controller_->DidScrollUpdate(false);
        scrollbar_controller_->DidScrollEnd();

        start_fade_.Run();

        time += base::TimeDelta::FromSeconds(2);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());
        EXPECT_FALSE(is_animating_);
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest, AwakenByProgrammaticScroll)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollUpdate(false);

        start_fade_.Run();
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());
        scrollbar_controller_->DidScrollUpdate(false);

        start_fade_.Run();
        time += base::TimeDelta::FromSeconds(2);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollUpdate(false);
        start_fade_.Run();
        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());
        EXPECT_FALSE(is_animating_);
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest,
        AnimationPreservedByNonScrollingGesture)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollUpdate(false);
        start_fade_.Run();
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollBegin();
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f / 3.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollEnd();
        EXPECT_FLOAT_EQ(1.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(0.0f, scrollbar_layer_->opacity());
        EXPECT_FALSE(is_animating_);
    }

    TEST_F(ScrollbarAnimationControllerLinearFadeTest,
        AnimationOverriddenByScrollingGesture)
    {
        base::TimeTicks time;
        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollUpdate(false);
        start_fade_.Run();
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        scrollbar_controller_->DidScrollBegin();
        EXPECT_FLOAT_EQ(2.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        EXPECT_TRUE(is_animating_);
        scrollbar_controller_->Animate(time);
        EXPECT_FLOAT_EQ(1.0f / 3.0f, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollUpdate(false);
        EXPECT_FLOAT_EQ(1, scrollbar_layer_->opacity());

        time += base::TimeDelta::FromSeconds(1);
        scrollbar_controller_->DidScrollEnd();
        EXPECT_FLOAT_EQ(1, scrollbar_layer_->opacity());
    }

} // namespace
} // namespace cc
