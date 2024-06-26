// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/input/top_controls_manager.h"

#include <algorithm>
#include <cmath>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/input/top_controls_manager_client.h"
#include "cc/layers/layer_impl.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/layer_tree_impl.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/vector2d_f.h"

namespace cc {
namespace {

    class MockTopControlsManagerClient : public TopControlsManagerClient {
    public:
        MockTopControlsManagerClient(float top_controls_height,
            float top_controls_show_threshold,
            float top_controls_hide_threshold)
            : host_impl_(&proxy_, &shared_bitmap_manager_, &task_graph_runner_)
            , redraw_needed_(false)
            , update_draw_properties_needed_(false)
            , top_controls_shown_ratio_(1.f)
            , top_controls_height_(top_controls_height)
            , top_controls_show_threshold_(top_controls_show_threshold)
            , top_controls_hide_threshold_(top_controls_hide_threshold)
        {
            active_tree_ = LayerTreeImpl::create(
                &host_impl_, new SyncedProperty<ScaleGroup>, new SyncedTopControls,
                new SyncedElasticOverscroll);
            root_scroll_layer_ = LayerImpl::Create(active_tree_.get(), 1);
        }

        ~MockTopControlsManagerClient() override { }

        void DidChangeTopControlsPosition() override
        {
            redraw_needed_ = true;
            update_draw_properties_needed_ = true;
        }

        bool HaveRootScrollLayer() const override { return true; }

        float TopControlsHeight() const override { return top_controls_height_; }

        void SetCurrentTopControlsShownRatio(float ratio) override
        {
            ASSERT_FALSE(std::isnan(ratio));
            ASSERT_FALSE(ratio == std::numeric_limits<float>::infinity());
            ASSERT_FALSE(ratio == -std::numeric_limits<float>::infinity());
            ratio = std::max(ratio, 0.f);
            ratio = std::min(ratio, 1.f);
            top_controls_shown_ratio_ = ratio;
        }

        float CurrentTopControlsShownRatio() const override
        {
            return top_controls_shown_ratio_;
        }

        LayerImpl* rootScrollLayer()
        {
            return root_scroll_layer_.get();
        }

        TopControlsManager* manager()
        {
            if (!manager_) {
                manager_ = TopControlsManager::Create(this,
                    top_controls_show_threshold_,
                    top_controls_hide_threshold_);
            }
            return manager_.get();
        }

        void SetTopControlsHeight(float height) { top_controls_height_ = height; }

    private:
        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeLayerTreeHostImpl host_impl_;
        scoped_ptr<LayerTreeImpl> active_tree_;
        scoped_ptr<LayerImpl> root_scroll_layer_;
        scoped_ptr<TopControlsManager> manager_;
        bool redraw_needed_;
        bool update_draw_properties_needed_;

        float top_controls_shown_ratio_;
        float top_controls_height_;
        float top_controls_show_threshold_;
        float top_controls_hide_threshold_;
    };

    TEST(TopControlsManagerTest, EnsureScrollThresholdApplied)
    {
        MockTopControlsManagerClient client(100.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();

        manager->ScrollBegin();

        // Scroll down to hide the controls entirely.
        manager->ScrollBy(gfx::Vector2dF(0.f, 30.f));
        EXPECT_FLOAT_EQ(-30.f, manager->ControlsTopOffset());

        manager->ScrollBy(gfx::Vector2dF(0.f, 30.f));
        EXPECT_FLOAT_EQ(-60.f, manager->ControlsTopOffset());

        manager->ScrollBy(gfx::Vector2dF(0.f, 100.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());

        // Scroll back up a bit and ensure the controls don't move until we cross
        // the threshold.
        manager->ScrollBy(gfx::Vector2dF(0.f, -10.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());

        manager->ScrollBy(gfx::Vector2dF(0.f, -50.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());

        // After hitting the threshold, further scrolling up should result in the top
        // controls showing.
        manager->ScrollBy(gfx::Vector2dF(0.f, -10.f));
        EXPECT_FLOAT_EQ(-90.f, manager->ControlsTopOffset());

        manager->ScrollBy(gfx::Vector2dF(0.f, -50.f));
        EXPECT_FLOAT_EQ(-40.f, manager->ControlsTopOffset());

        // Reset the scroll threshold by going further up the page than the initial
        // threshold.
        manager->ScrollBy(gfx::Vector2dF(0.f, -100.f));
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());

        // See that scrolling down the page now will result in the controls hiding.
        manager->ScrollBy(gfx::Vector2dF(0.f, 20.f));
        EXPECT_FLOAT_EQ(-20.f, manager->ControlsTopOffset());

        manager->ScrollEnd();
    }

    TEST(TopControlsManagerTest, PartialShownHideAnimation)
    {
        MockTopControlsManagerClient client(100.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();
        manager->ScrollBegin();
        manager->ScrollBy(gfx::Vector2dF(0.f, 300.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
        manager->ScrollEnd();

        manager->ScrollBegin();
        manager->ScrollBy(gfx::Vector2dF(0.f, -15.f));
        EXPECT_FLOAT_EQ(-85.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(15.f, manager->ContentTopOffset());
        manager->ScrollEnd();

        EXPECT_TRUE(manager->animation());

        base::TimeTicks time = base::TimeTicks::Now();
        float previous;
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_LT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, PartialShownShowAnimation)
    {
        MockTopControlsManagerClient client(100.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();
        manager->ScrollBegin();
        manager->ScrollBy(gfx::Vector2dF(0.f, 300.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
        manager->ScrollEnd();

        manager->ScrollBegin();
        manager->ScrollBy(gfx::Vector2dF(0.f, -70.f));
        EXPECT_FLOAT_EQ(-30.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(70.f, manager->ContentTopOffset());
        manager->ScrollEnd();

        EXPECT_TRUE(manager->animation());

        base::TimeTicks time = base::TimeTicks::Now();
        float previous;
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_GT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(100.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, PartialHiddenWithAmbiguousThresholdShows)
    {
        MockTopControlsManagerClient client(100.f, 0.25f, 0.25f);
        TopControlsManager* manager = client.manager();

        manager->ScrollBegin();

        manager->ScrollBy(gfx::Vector2dF(0.f, 20.f));
        EXPECT_FLOAT_EQ(-20.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(80.f, manager->ContentTopOffset());

        manager->ScrollEnd();
        EXPECT_TRUE(manager->animation());

        base::TimeTicks time = base::TimeTicks::Now();
        float previous;
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_GT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(100.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, PartialHiddenWithAmbiguousThresholdHides)
    {
        MockTopControlsManagerClient client(100.f, 0.25f, 0.25f);
        TopControlsManager* manager = client.manager();

        manager->ScrollBegin();

        manager->ScrollBy(gfx::Vector2dF(0.f, 30.f));
        EXPECT_FLOAT_EQ(-30.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(70.f, manager->ContentTopOffset());

        manager->ScrollEnd();
        EXPECT_TRUE(manager->animation());

        base::TimeTicks time = base::TimeTicks::Now();
        float previous;
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_LT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, PartialShownWithAmbiguousThresholdHides)
    {
        MockTopControlsManagerClient client(100.f, 0.25f, 0.25f);
        TopControlsManager* manager = client.manager();

        manager->ScrollBy(gfx::Vector2dF(0.f, 200.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());

        manager->ScrollBegin();

        manager->ScrollBy(gfx::Vector2dF(0.f, -20.f));
        EXPECT_FLOAT_EQ(-80.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(20.f, manager->ContentTopOffset());

        manager->ScrollEnd();
        EXPECT_TRUE(manager->animation());

        base::TimeTicks time = base::TimeTicks::Now();
        float previous;
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_LT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, PartialShownWithAmbiguousThresholdShows)
    {
        MockTopControlsManagerClient client(100.f, 0.25f, 0.25f);
        TopControlsManager* manager = client.manager();

        manager->ScrollBy(gfx::Vector2dF(0.f, 200.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());

        manager->ScrollBegin();

        manager->ScrollBy(gfx::Vector2dF(0.f, -30.f));
        EXPECT_FLOAT_EQ(-70.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(30.f, manager->ContentTopOffset());

        manager->ScrollEnd();
        EXPECT_TRUE(manager->animation());

        base::TimeTicks time = base::TimeTicks::Now();
        float previous;
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_GT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(100.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, PinchIgnoresScroll)
    {
        MockTopControlsManagerClient client(100.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();

        // Hide the controls.
        manager->ScrollBegin();
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());

        manager->ScrollBy(gfx::Vector2dF(0.f, 300.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());

        manager->PinchBegin();
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());

        // Scrolls are ignored during pinch.
        manager->ScrollBy(gfx::Vector2dF(0.f, -15.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        manager->PinchEnd();
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());

        // Scrolls should no long be ignored.
        manager->ScrollBy(gfx::Vector2dF(0.f, -15.f));
        EXPECT_FLOAT_EQ(-85.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(15.f, manager->ContentTopOffset());
        manager->ScrollEnd();

        EXPECT_TRUE(manager->animation());
    }

    TEST(TopControlsManagerTest, PinchBeginStartsAnimationIfNecessary)
    {
        MockTopControlsManagerClient client(100.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();

        manager->ScrollBegin();
        manager->ScrollBy(gfx::Vector2dF(0.f, 300.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());

        manager->PinchBegin();
        EXPECT_FALSE(manager->animation());

        manager->PinchEnd();
        EXPECT_FALSE(manager->animation());

        manager->ScrollBy(gfx::Vector2dF(0.f, -15.f));
        EXPECT_FLOAT_EQ(-85.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(15.f, manager->ContentTopOffset());

        manager->PinchBegin();
        EXPECT_TRUE(manager->animation());

        base::TimeTicks time = base::TimeTicks::Now();
        float previous;
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_LT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());

        manager->PinchEnd();
        EXPECT_FALSE(manager->animation());

        manager->ScrollBy(gfx::Vector2dF(0.f, -55.f));
        EXPECT_FLOAT_EQ(-45.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(55.f, manager->ContentTopOffset());
        EXPECT_FALSE(manager->animation());

        manager->ScrollEnd();
        EXPECT_TRUE(manager->animation());

        time = base::TimeTicks::Now();
        while (manager->animation()) {
            previous = manager->TopControlsShownRatio();
            time = base::TimeDelta::FromMicroseconds(100) + time;
            manager->Animate(time);
            EXPECT_GT(manager->TopControlsShownRatio(), previous);
        }
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());
    }

    TEST(TopControlsManagerTest, HeightChangeMaintainsFullyVisibleControls)
    {
        MockTopControlsManagerClient client(0.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();

        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());

        client.SetTopControlsHeight(100.f);
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(100.f, manager->TopControlsHeight());
        EXPECT_FLOAT_EQ(0, manager->ControlsTopOffset());

        client.SetTopControlsHeight(50.f);
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(50.f, manager->TopControlsHeight());
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());
    }

    TEST(TopControlsManagerTest, GrowingHeightKeepsTopControlsHidden)
    {
        MockTopControlsManagerClient client(0.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();
        client.SetTopControlsHeight(1.f);
        manager->UpdateTopControlsState(HIDDEN, HIDDEN, false);
        EXPECT_EQ(-1.f, manager->ControlsTopOffset());
        EXPECT_EQ(0.f, manager->ContentTopOffset());

        client.SetTopControlsHeight(50.f);
        EXPECT_FALSE(manager->animation());
        EXPECT_EQ(-50.f, manager->ControlsTopOffset());
        EXPECT_EQ(0.f, manager->ContentTopOffset());

        client.SetTopControlsHeight(100.f);
        EXPECT_FALSE(manager->animation());
        EXPECT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_EQ(0.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, ShrinkingHeightKeepsTopControlsHidden)
    {
        MockTopControlsManagerClient client(100.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();

        manager->ScrollBegin();
        manager->ScrollBy(gfx::Vector2dF(0.f, 300.f));
        EXPECT_FLOAT_EQ(-100.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
        manager->ScrollEnd();

        client.SetTopControlsHeight(50.f);
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(-50.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());

        client.SetTopControlsHeight(0.f);
        EXPECT_FALSE(manager->animation());
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
    }

    TEST(TopControlsManagerTest, ScrollByWithZeroHeightControlsIsNoop)
    {
        MockTopControlsManagerClient client(0.f, 0.5f, 0.5f);
        TopControlsManager* manager = client.manager();
        manager->UpdateTopControlsState(BOTH, BOTH, false);

        manager->ScrollBegin();
        gfx::Vector2dF pending = manager->ScrollBy(gfx::Vector2dF(0.f, 20.f));
        EXPECT_FLOAT_EQ(20.f, pending.y());
        EXPECT_FLOAT_EQ(0.f, manager->ControlsTopOffset());
        EXPECT_FLOAT_EQ(0.f, manager->ContentTopOffset());
        EXPECT_FLOAT_EQ(1.f, client.CurrentTopControlsShownRatio());
        manager->ScrollEnd();
    }

} // namespace
} // namespace cc
