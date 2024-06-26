// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_host.h"

#include "cc/animation/animation_curve.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/animation_id_provider.h"
#include "cc/animation/animation_player.h"
#include "cc/animation/animation_timeline.h"
#include "cc/animation/element_animations.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/animation/scroll_offset_animation_curve.h"
#include "cc/animation/timing_function.h"
#include "cc/base/completion_event.h"
#include "cc/base/time_util.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_impl.h"
#include "cc/test/animation_test_common.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/fake_picture_layer.h"
#include "cc/test/layer_tree_test.h"
#include "cc/trees/layer_tree_impl.h"

namespace cc {
namespace {

    class LayerTreeHostTimelinesTest : public LayerTreeTest {
    public:
        LayerTreeHostTimelinesTest()
            : timeline_id_(AnimationIdProvider::NextTimelineId())
            , player_id_(AnimationIdProvider::NextPlayerId())
            , player_child_id_(AnimationIdProvider::NextPlayerId())
        {
            timeline_ = AnimationTimeline::Create(timeline_id_);
            player_ = AnimationPlayer::Create(player_id_);
            player_child_ = AnimationPlayer::Create(player_child_id_);

            player_->set_layer_animation_delegate(this);
        }

        void InitializeSettings(LayerTreeSettings* settings) override
        {
            settings->use_compositor_animation_timelines = true;
        }

        void InitializeLayerSettings(LayerSettings* layer_settings) override
        {
            layer_settings->use_compositor_animation_timelines = true;
        }

        void SetupTree() override { LayerTreeTest::SetupTree(); }

        void AttachPlayersToTimeline()
        {
            layer_tree_host()->animation_host()->AddAnimationTimeline(timeline_.get());
            timeline_->AttachPlayer(player_.get());
            timeline_->AttachPlayer(player_child_.get());
        }

    protected:
        scoped_refptr<AnimationTimeline> timeline_;
        scoped_refptr<AnimationPlayer> player_;
        scoped_refptr<AnimationPlayer> player_child_;

        const int timeline_id_;
        const int player_id_;
        const int player_child_id_;
    };

    // Add a layer animation and confirm that
    // LayerTreeHostImpl::UpdateAnimationState does get called.
    // Evolved frome LayerTreeHostAnimationTestAddAnimation
    class LayerTreeHostTimelinesTestAddAnimation
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestAddAnimation()
            : update_animation_state_was_called_(false)
        {
        }

        void BeginTest() override
        {
            AttachPlayersToTimeline();
            player_->AttachLayer(layer_tree_host()->root_layer()->id());
            PostAddInstantAnimationToMainThreadPlayer(player_.get());
        }

        void UpdateAnimationState(LayerTreeHostImpl* host_impl,
            bool has_unfinished_animation) override
        {
            EXPECT_FALSE(has_unfinished_animation);
            update_animation_state_was_called_ = true;
        }

        void NotifyAnimationStarted(base::TimeTicks monotonic_time,
            Animation::TargetProperty target_property,
            int group) override
        {
            EXPECT_LT(base::TimeTicks(), monotonic_time);

            LayerAnimationController* controller = player_->element_animations()->layer_animation_controller();
            Animation* animation = controller->GetAnimation(Animation::OPACITY);
            if (animation)
                player_->RemoveAnimation(animation->id());

            EndTest();
        }

        void AfterTest() override { EXPECT_TRUE(update_animation_state_was_called_); }

    private:
        bool update_animation_state_was_called_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostTimelinesTestAddAnimation);

    // Add a layer animation to a layer, but continually fail to draw. Confirm that
    // after a while, we do eventually force a draw.
    // Evolved from LayerTreeHostAnimationTestCheckerboardDoesNotStarveDraws.
    class LayerTreeHostTimelinesTestCheckerboardDoesNotStarveDraws
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestCheckerboardDoesNotStarveDraws()
            : started_animating_(false)
        {
        }

        void BeginTest() override
        {
            AttachPlayersToTimeline();
            player_->AttachLayer(layer_tree_host()->root_layer()->id());
            PostAddAnimationToMainThreadPlayer(player_.get());
        }

        void AnimateLayers(LayerTreeHostImpl* host_impl,
            base::TimeTicks monotonic_time) override
        {
            started_animating_ = true;
        }

        void DrawLayersOnThread(LayerTreeHostImpl* host_impl) override
        {
            if (started_animating_)
                EndTest();
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* host_impl,
            LayerTreeHostImpl::FrameData* frame,
            DrawResult draw_result) override
        {
            return DRAW_ABORTED_CHECKERBOARD_ANIMATIONS;
        }

        void AfterTest() override { }

    private:
        bool started_animating_;
    };

    // Starvation can only be an issue with the MT compositor.
    MULTI_THREAD_TEST_F(LayerTreeHostTimelinesTestCheckerboardDoesNotStarveDraws);

    // Ensures that animations eventually get deleted.
    // Evolved from LayerTreeHostAnimationTestAnimationsGetDeleted.
    class LayerTreeHostTimelinesTestAnimationsGetDeleted
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestAnimationsGetDeleted()
            : started_animating_(false)
        {
        }

        void BeginTest() override
        {
            AttachPlayersToTimeline();
            player_->AttachLayer(layer_tree_host()->root_layer()->id());
            PostAddAnimationToMainThreadPlayer(player_.get());
        }

        void AnimateLayers(LayerTreeHostImpl* host_impl,
            base::TimeTicks monotonic_time) override
        {
            bool have_animations = !host_impl->animation_host()
                                        ->animation_registrar()
                                        ->active_animation_controllers_for_testing()
                                        .empty();
            if (!started_animating_ && have_animations) {
                started_animating_ = true;
                return;
            }

            if (started_animating_ && !have_animations)
                EndTest();
        }

        void NotifyAnimationFinished(base::TimeTicks monotonic_time,
            Animation::TargetProperty target_property,
            int group) override
        {
            // Animations on the impl-side controller only get deleted during a commit,
            // so we need to schedule a commit.
            layer_tree_host()->SetNeedsCommit();
        }

        void AfterTest() override { }

    private:
        bool started_animating_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostTimelinesTestAnimationsGetDeleted);

    // Ensure that an animation's timing function is respected.
    // Evolved from LayerTreeHostAnimationTestAddAnimationWithTimingFunction.
    class LayerTreeHostTimelinesTestAddAnimationWithTimingFunction
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestAddAnimationWithTimingFunction() { }

        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();
            picture_ = FakePictureLayer::Create(layer_settings(), &client_);
            picture_->SetBounds(gfx::Size(4, 4));
            layer_tree_host()->root_layer()->AddChild(picture_);

            AttachPlayersToTimeline();
            player_child_->AttachLayer(picture_->id());
        }

        void BeginTest() override
        {
            PostAddAnimationToMainThreadPlayer(player_child_.get());
        }

        void AnimateLayers(LayerTreeHostImpl* host_impl,
            base::TimeTicks monotonic_time) override
        {
            scoped_refptr<AnimationTimeline> timeline_impl = host_impl->animation_host()->GetTimelineById(timeline_id_);
            scoped_refptr<AnimationPlayer> player_child_impl = timeline_impl->GetPlayerById(player_child_id_);

            LayerAnimationController* controller_impl = player_child_impl->element_animations()->layer_animation_controller();
            if (!controller_impl)
                return;

            Animation* animation = controller_impl->GetAnimation(Animation::OPACITY);
            if (!animation)
                return;

            const FloatAnimationCurve* curve = animation->curve()->ToFloatAnimationCurve();
            float start_opacity = curve->GetValue(base::TimeDelta());
            float end_opacity = curve->GetValue(curve->Duration());
            float linearly_interpolated_opacity = 0.25f * end_opacity + 0.75f * start_opacity;
            base::TimeDelta time = TimeUtil::Scale(curve->Duration(), 0.25f);
            // If the linear timing function associated with this animation was not
            // picked up, then the linearly interpolated opacity would be different
            // because of the default ease timing function.
            EXPECT_FLOAT_EQ(linearly_interpolated_opacity, curve->GetValue(time));

            EndTest();
        }

        void AfterTest() override { }

        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> picture_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestAddAnimationWithTimingFunction);

    // Ensures that main thread animations have their start times synchronized with
    // impl thread animations.
    // Evolved from LayerTreeHostAnimationTestSynchronizeAnimationStartTimes.
    class LayerTreeHostTimelinesTestSynchronizeAnimationStartTimes
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestSynchronizeAnimationStartTimes() { }

        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();
            picture_ = FakePictureLayer::Create(layer_settings(), &client_);
            picture_->SetBounds(gfx::Size(4, 4));

            layer_tree_host()->root_layer()->AddChild(picture_);

            AttachPlayersToTimeline();
            player_child_->set_layer_animation_delegate(this);
            player_child_->AttachLayer(picture_->id());
        }

        void BeginTest() override
        {
            PostAddAnimationToMainThreadPlayer(player_child_.get());
        }

        void NotifyAnimationStarted(base::TimeTicks monotonic_time,
            Animation::TargetProperty target_property,
            int group) override
        {
            LayerAnimationController* controller = player_child_->element_animations()->layer_animation_controller();
            Animation* animation = controller->GetAnimation(Animation::OPACITY);
            main_start_time_ = animation->start_time();
            controller->RemoveAnimation(animation->id());
            EndTest();
        }

        void UpdateAnimationState(LayerTreeHostImpl* impl_host,
            bool has_unfinished_animation) override
        {
            scoped_refptr<AnimationTimeline> timeline_impl = impl_host->animation_host()->GetTimelineById(timeline_id_);
            scoped_refptr<AnimationPlayer> player_child_impl = timeline_impl->GetPlayerById(player_child_id_);

            LayerAnimationController* controller = player_child_impl->element_animations()->layer_animation_controller();
            Animation* animation = controller->GetAnimation(Animation::OPACITY);
            if (!animation)
                return;

            impl_start_time_ = animation->start_time();
        }

        void AfterTest() override
        {
            EXPECT_EQ(impl_start_time_, main_start_time_);
            EXPECT_LT(base::TimeTicks(), impl_start_time_);
        }

    private:
        base::TimeTicks main_start_time_;
        base::TimeTicks impl_start_time_;
        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> picture_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestSynchronizeAnimationStartTimes);

    // Ensures that notify animation finished is called.
    // Evolved from LayerTreeHostAnimationTestAnimationFinishedEvents.
    class LayerTreeHostTimelinesTestAnimationFinishedEvents
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestAnimationFinishedEvents() { }

        void BeginTest() override
        {
            AttachPlayersToTimeline();
            player_->AttachLayer(layer_tree_host()->root_layer()->id());
            PostAddInstantAnimationToMainThreadPlayer(player_.get());
        }

        void NotifyAnimationFinished(base::TimeTicks monotonic_time,
            Animation::TargetProperty target_property,
            int group) override
        {
            LayerAnimationController* controller = player_->element_animations()->layer_animation_controller();
            Animation* animation = controller->GetAnimation(Animation::OPACITY);
            if (animation)
                controller->RemoveAnimation(animation->id());
            EndTest();
        }

        void AfterTest() override { }
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestAnimationFinishedEvents);

    // Ensures that when opacity is being animated, this value does not cause the
    // subtree to be skipped.
    // Evolved from LayerTreeHostAnimationTestDoNotSkipLayersWithAnimatedOpacity.
    class LayerTreeHostTimelinesTestDoNotSkipLayersWithAnimatedOpacity
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestDoNotSkipLayersWithAnimatedOpacity()
            : update_check_layer_(
                FakePictureLayer::Create(layer_settings(), &client_))
        {
        }

        void SetupTree() override
        {
            update_check_layer_->SetOpacity(0.f);
            layer_tree_host()->SetRootLayer(update_check_layer_);
            LayerTreeHostTimelinesTest::SetupTree();

            AttachPlayersToTimeline();
            player_->AttachLayer(update_check_layer_->id());
        }

        void BeginTest() override
        {
            PostAddAnimationToMainThreadPlayer(player_.get());
        }

        void DidActivateTreeOnThread(LayerTreeHostImpl* host_impl) override
        {
            scoped_refptr<AnimationTimeline> timeline_impl = host_impl->animation_host()->GetTimelineById(timeline_id_);
            scoped_refptr<AnimationPlayer> player_impl = timeline_impl->GetPlayerById(player_id_);

            LayerAnimationController* controller_impl = player_impl->element_animations()->layer_animation_controller();
            Animation* animation_impl = controller_impl->GetAnimation(Animation::OPACITY);
            controller_impl->RemoveAnimation(animation_impl->id());
            EndTest();
        }

        void AfterTest() override
        {
            // Update() should have been called once, proving that the layer was not
            // skipped.
            EXPECT_EQ(1, update_check_layer_->update_count());

            // clear update_check_layer_ so LayerTreeHost dies.
            update_check_layer_ = NULL;
        }

    private:
        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> update_check_layer_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestDoNotSkipLayersWithAnimatedOpacity);

    // Layers added to tree with existing active animations should have the
    // animation correctly recognized.
    // Evolved from LayerTreeHostAnimationTestLayerAddedWithAnimation.
    class LayerTreeHostTimelinesTestLayerAddedWithAnimation
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestLayerAddedWithAnimation() { }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DidCommit() override
        {
            if (layer_tree_host()->source_frame_number() == 1) {
                AttachPlayersToTimeline();

                scoped_refptr<Layer> layer = Layer::Create(layer_settings());
                player_->AttachLayer(layer->id());
                player_->set_layer_animation_delegate(this);

                // Any valid AnimationCurve will do here.
                scoped_ptr<AnimationCurve> curve(new FakeFloatAnimationCurve());
                scoped_ptr<Animation> animation(
                    Animation::Create(curve.Pass(), 1, 1, Animation::OPACITY));
                player_->AddAnimation(animation.Pass());

                // We add the animation *before* attaching the layer to the tree.
                layer_tree_host()->root_layer()->AddChild(layer);
            }
        }

        void AnimateLayers(LayerTreeHostImpl* impl_host,
            base::TimeTicks monotonic_time) override
        {
            EndTest();
        }

        void AfterTest() override { }
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestLayerAddedWithAnimation);

    // Animations should not be started when frames are being skipped due to
    // checkerboard.
    // Evolved from LayerTreeHostAnimationTestCheckerboardDoesntStartAnimations.
    class LayerTreeHostTimelinesTestCheckerboardDoesntStartAnimations
        : public LayerTreeHostTimelinesTest {
        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();
            picture_ = FakePictureLayer::Create(layer_settings(), &client_);
            picture_->SetBounds(gfx::Size(4, 4));
            layer_tree_host()->root_layer()->AddChild(picture_);

            AttachPlayersToTimeline();
            player_child_->AttachLayer(picture_->id());
            player_child_->set_layer_animation_delegate(this);
        }

        void InitializeSettings(LayerTreeSettings* settings) override
        {
            // Make sure that drawing many times doesn't cause a checkerboarded
            // animation to start so we avoid flake in this test.
            settings->timeout_and_draw_when_animation_checkerboards = false;
            LayerTreeHostTimelinesTest::InitializeSettings(settings);
        }

        void BeginTest() override
        {
            prevented_draw_ = 0;
            started_times_ = 0;

            PostSetNeedsCommitToMainThread();
        }

        DrawResult PrepareToDrawOnThread(LayerTreeHostImpl* host_impl,
            LayerTreeHostImpl::FrameData* frame_data,
            DrawResult draw_result) override
        {
            // Don't checkerboard when the first animation wants to start.
            if (host_impl->active_tree()->source_frame_number() < 2)
                return draw_result;
            if (TestEnded())
                return draw_result;
            // Act like there is checkerboard when the second animation wants to draw.
            ++prevented_draw_;
            if (prevented_draw_ > 2)
                EndTest();
            return DRAW_ABORTED_CHECKERBOARD_ANIMATIONS;
        }

        void DidCommitAndDrawFrame() override
        {
            switch (layer_tree_host()->source_frame_number()) {
            case 1:
                // The animation is longer than 1 BeginFrame interval.
                AddOpacityTransitionToPlayer(player_child_.get(), 0.1, 0.2f, 0.8f,
                    false);
                break;
            case 2:
                // This second animation will not be drawn so it should not start.
                AddAnimatedTransformToPlayer(player_child_.get(), 0.1, 5, 5);
                break;
            }
        }

        void NotifyAnimationStarted(base::TimeTicks monotonic_time,
            Animation::TargetProperty target_property,
            int group) override
        {
            if (TestEnded())
                return;
            started_times_++;
        }

        void AfterTest() override
        {
            // Make sure we tried to draw the second animation but failed.
            EXPECT_LT(0, prevented_draw_);
            // The first animation should be started, but the second should not because
            // of checkerboard.
            EXPECT_EQ(1, started_times_);
        }

        int prevented_draw_;
        int started_times_;
        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> picture_;
    };

    MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestCheckerboardDoesntStartAnimations);

    // Verifies that scroll offset animations are only accepted when impl-scrolling
    // is supported, and that when scroll offset animations are accepted,
    // scroll offset updates are sent back to the main thread.
    // Evolved from LayerTreeHostAnimationTestScrollOffsetChangesArePropagated
    class LayerTreeHostTimelinesTestScrollOffsetChangesArePropagated
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestScrollOffsetChangesArePropagated() { }

        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();

            scroll_layer_ = FakePictureLayer::Create(layer_settings(), &client_);
            scroll_layer_->SetScrollClipLayerId(layer_tree_host()->root_layer()->id());
            scroll_layer_->SetBounds(gfx::Size(1000, 1000));
            scroll_layer_->SetScrollOffset(gfx::ScrollOffset(10, 20));
            layer_tree_host()->root_layer()->AddChild(scroll_layer_);

            AttachPlayersToTimeline();
            player_child_->AttachLayer(scroll_layer_->id());
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DidCommit() override
        {
            switch (layer_tree_host()->source_frame_number()) {
            case 1: {
                scoped_ptr<ScrollOffsetAnimationCurve> curve(
                    ScrollOffsetAnimationCurve::Create(
                        gfx::ScrollOffset(500.f, 550.f),
                        EaseInOutTimingFunction::Create()));
                scoped_ptr<Animation> animation(
                    Animation::Create(curve.Pass(), 1, 0, Animation::SCROLL_OFFSET));
                animation->set_needs_synchronized_start_time(true);
                bool impl_scrolling_supported = layer_tree_host()->proxy()->SupportsImplScrolling();
                if (impl_scrolling_supported)
                    player_child_->AddAnimation(animation.Pass());
                else
                    EndTest();
                break;
            }
            default:
                if (scroll_layer_->scroll_offset().x() > 10 && scroll_layer_->scroll_offset().y() > 20)
                    EndTest();
            }
        }

        void AfterTest() override { }

    private:
        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> scroll_layer_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestScrollOffsetChangesArePropagated);

    // Verifies that when the main thread removes a scroll animation and sets a new
    // scroll position, the active tree takes on exactly this new scroll position
    // after activation, and the main thread doesn't receive a spurious scroll
    // delta.
    // Evolved from LayerTreeHostAnimationTestScrollOffsetAnimationRemoval
    class LayerTreeHostTimelinesTestScrollOffsetAnimationRemoval
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestScrollOffsetAnimationRemoval()
            : final_postion_(50.0, 100.0)
        {
        }

        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();

            scroll_layer_ = FakePictureLayer::Create(layer_settings(), &client_);
            scroll_layer_->SetScrollClipLayerId(layer_tree_host()->root_layer()->id());
            scroll_layer_->SetBounds(gfx::Size(10000, 10000));
            scroll_layer_->SetScrollOffset(gfx::ScrollOffset(100.0, 200.0));
            layer_tree_host()->root_layer()->AddChild(scroll_layer_);

            scoped_ptr<ScrollOffsetAnimationCurve> curve(
                ScrollOffsetAnimationCurve::Create(gfx::ScrollOffset(6500.f, 7500.f),
                    EaseInOutTimingFunction::Create()));
            scoped_ptr<Animation> animation(
                Animation::Create(curve.Pass(), 1, 0, Animation::SCROLL_OFFSET));
            animation->set_needs_synchronized_start_time(true);

            AttachPlayersToTimeline();
            player_child_->AttachLayer(scroll_layer_->id());
            player_child_->AddAnimation(animation.Pass());
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void BeginMainFrame(const BeginFrameArgs& args) override
        {
            switch (layer_tree_host()->source_frame_number()) {
            case 0:
                break;
            case 1: {
                Animation* animation = player_child_->element_animations()
                                           ->layer_animation_controller()
                                           ->GetAnimation(Animation::SCROLL_OFFSET);
                player_child_->RemoveAnimation(animation->id());
                scroll_layer_->SetScrollOffset(final_postion_);
                break;
            }
            default:
                EXPECT_EQ(final_postion_, scroll_layer_->scroll_offset());
            }
        }

        void BeginCommitOnThread(LayerTreeHostImpl* host_impl) override
        {
            host_impl->BlockNotifyReadyToActivateForTesting(true);
        }

        void WillBeginImplFrameOnThread(LayerTreeHostImpl* host_impl,
            const BeginFrameArgs& args) override
        {
            if (!host_impl->pending_tree())
                return;

            if (!host_impl->active_tree()->root_layer()) {
                host_impl->BlockNotifyReadyToActivateForTesting(false);
                return;
            }

            scoped_refptr<AnimationTimeline> timeline_impl = host_impl->animation_host()->GetTimelineById(timeline_id_);
            scoped_refptr<AnimationPlayer> player_impl = timeline_impl->GetPlayerById(player_child_id_);

            LayerImpl* scroll_layer_impl = host_impl->active_tree()->root_layer()->children()[0];
            Animation* animation = player_impl->element_animations()
                                       ->layer_animation_controller()
                                       ->GetAnimation(Animation::SCROLL_OFFSET);

            if (!animation || animation->run_state() != Animation::RUNNING) {
                host_impl->BlockNotifyReadyToActivateForTesting(false);
                return;
            }

            // Block activation until the running animation has a chance to produce a
            // scroll delta.
            gfx::Vector2dF scroll_delta = scroll_layer_impl->ScrollDelta();
            if (scroll_delta.x() < 1.f || scroll_delta.y() < 1.f)
                return;

            host_impl->BlockNotifyReadyToActivateForTesting(false);
        }

        void WillActivateTreeOnThread(LayerTreeHostImpl* host_impl) override
        {
            if (host_impl->pending_tree()->source_frame_number() != 1)
                return;
            LayerImpl* scroll_layer_impl = host_impl->pending_tree()->root_layer()->children()[0];
            EXPECT_EQ(final_postion_, scroll_layer_impl->CurrentScrollOffset());
        }

        void DidActivateTreeOnThread(LayerTreeHostImpl* host_impl) override
        {
            if (host_impl->active_tree()->source_frame_number() != 1)
                return;
            LayerImpl* scroll_layer_impl = host_impl->active_tree()->root_layer()->children()[0];
            EXPECT_EQ(final_postion_, scroll_layer_impl->CurrentScrollOffset());
            EndTest();
        }

        void AfterTest() override
        {
            EXPECT_EQ(final_postion_, scroll_layer_->scroll_offset());
        }

    private:
        FakeContentLayerClient client_;
        scoped_refptr<FakePictureLayer> scroll_layer_;
        const gfx::ScrollOffset final_postion_;
    };

    MULTI_THREAD_TEST_F(LayerTreeHostTimelinesTestScrollOffsetAnimationRemoval);

    // When animations are simultaneously added to an existing layer and to a new
    // layer, they should start at the same time, even when there's already a
    // running animation on the existing layer.
    // Evolved from LayerTreeHostAnimationTestAnimationsAddedToNewAndExistingLayers.
    class LayerTreeHostTimelinesTestAnimationsAddedToNewAndExistingLayers
        : public LayerTreeHostTimelinesTest {
    public:
        LayerTreeHostTimelinesTestAnimationsAddedToNewAndExistingLayers()
            : frame_count_with_pending_tree_(0)
        {
        }

        void BeginTest() override
        {
            AttachPlayersToTimeline();
            PostSetNeedsCommitToMainThread();
        }

        void DidCommit() override
        {
            if (layer_tree_host()->source_frame_number() == 1) {
                player_->AttachLayer(layer_tree_host()->root_layer()->id());
                AddAnimatedTransformToPlayer(player_.get(), 4, 1, 1);
            } else if (layer_tree_host()->source_frame_number() == 2) {
                AddOpacityTransitionToPlayer(player_.get(), 1, 0.f, 0.5f, true);

                scoped_refptr<Layer> layer = Layer::Create(layer_settings());
                layer_tree_host()->root_layer()->AddChild(layer);
                layer->SetBounds(gfx::Size(4, 4));

                player_child_->AttachLayer(layer->id());
                player_child_->set_layer_animation_delegate(this);
                AddOpacityTransitionToPlayer(player_child_.get(), 1, 0.f, 0.5f, true);
            }
        }

        void BeginCommitOnThread(LayerTreeHostImpl* host_impl) override
        {
            host_impl->BlockNotifyReadyToActivateForTesting(true);
        }

        void CommitCompleteOnThread(LayerTreeHostImpl* host_impl) override
        {
            // For the commit that added animations to new and existing layers, keep
            // blocking activation. We want to verify that even with activation blocked,
            // the animation on the layer that's already in the active tree won't get a
            // head start.
            if (host_impl->pending_tree()->source_frame_number() != 2) {
                host_impl->BlockNotifyReadyToActivateForTesting(false);
            }
        }

        void WillBeginImplFrameOnThread(LayerTreeHostImpl* host_impl,
            const BeginFrameArgs& args) override
        {
            if (!host_impl->pending_tree() || host_impl->pending_tree()->source_frame_number() != 2)
                return;

            frame_count_with_pending_tree_++;
            if (frame_count_with_pending_tree_ == 2) {
                host_impl->BlockNotifyReadyToActivateForTesting(false);
            }
        }

        void UpdateAnimationState(LayerTreeHostImpl* host_impl,
            bool has_unfinished_animation) override
        {
            scoped_refptr<AnimationTimeline> timeline_impl = host_impl->animation_host()->GetTimelineById(timeline_id_);
            scoped_refptr<AnimationPlayer> player_impl = timeline_impl->GetPlayerById(player_id_);
            scoped_refptr<AnimationPlayer> player_child_impl = timeline_impl->GetPlayerById(player_child_id_);

            // wait for tree activation.
            if (!player_impl->element_animations())
                return;

            LayerAnimationController* root_controller_impl = player_impl->element_animations()->layer_animation_controller();
            Animation* root_animation = root_controller_impl->GetAnimation(Animation::OPACITY);
            if (!root_animation || root_animation->run_state() != Animation::RUNNING)
                return;

            LayerAnimationController* child_controller_impl = player_child_impl->element_animations()->layer_animation_controller();
            Animation* child_animation = child_controller_impl->GetAnimation(Animation::OPACITY);
            EXPECT_EQ(Animation::RUNNING, child_animation->run_state());
            EXPECT_EQ(root_animation->start_time(), child_animation->start_time());
            root_controller_impl->AbortAnimations(Animation::OPACITY);
            root_controller_impl->AbortAnimations(Animation::TRANSFORM);
            child_controller_impl->AbortAnimations(Animation::OPACITY);
            EndTest();
        }

        void AfterTest() override { }

    private:
        int frame_count_with_pending_tree_;
    };

    // This test blocks activation which is not supported for single thread mode.
    MULTI_THREAD_BLOCKNOTIFY_TEST_F(
        LayerTreeHostTimelinesTestAnimationsAddedToNewAndExistingLayers);

    // Evolved from LayerTreeHostAnimationTestAddAnimationAfterAnimating.
    class LayerTreeHostTimelinesTestAddAnimationAfterAnimating
        : public LayerTreeHostTimelinesTest {
    public:
        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();
            content_ = Layer::Create(layer_settings());
            content_->SetBounds(gfx::Size(4, 4));
            layer_tree_host()->root_layer()->AddChild(content_);

            AttachPlayersToTimeline();

            player_->AttachLayer(layer_tree_host()->root_layer()->id());
            player_child_->AttachLayer(content_->id());
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DidCommit() override
        {
            switch (layer_tree_host()->source_frame_number()) {
            case 1:
                // First frame: add an animation to the root layer.
                AddAnimatedTransformToPlayer(player_.get(), 0.1, 5, 5);
                break;
            case 2:
                // Second frame: add an animation to the content layer. The root layer
                // animation has caused us to animate already during this frame.
                AddOpacityTransitionToPlayer(player_child_.get(), 0.1, 5, 5, false);
                break;
            }
        }

        void SwapBuffersOnThread(LayerTreeHostImpl* host_impl, bool result) override
        {
            // After both animations have started, verify that they have valid
            // start times.
            if (host_impl->active_tree()->source_frame_number() < 2)
                return;
            AnimationRegistrar::AnimationControllerMap controllers_copy = host_impl->animation_host()
                                                                              ->animation_registrar()
                                                                              ->active_animation_controllers_for_testing();
            EXPECT_EQ(2u, controllers_copy.size());
            for (auto& it : controllers_copy) {
                int id = it.first;
                if (id == host_impl->RootLayer()->id()) {
                    Animation* anim = it.second->GetAnimation(Animation::TRANSFORM);
                    EXPECT_GT((anim->start_time() - base::TimeTicks()).InSecondsF(), 0);
                } else if (id == host_impl->RootLayer()->children()[0]->id()) {
                    Animation* anim = it.second->GetAnimation(Animation::OPACITY);
                    EXPECT_GT((anim->start_time() - base::TimeTicks()).InSecondsF(), 0);
                }
                EndTest();
            }
        }

        void AfterTest() override { }

    private:
        scoped_refptr<Layer> content_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(
        LayerTreeHostTimelinesTestAddAnimationAfterAnimating);

    class LayerTreeHostTimelinesTestRemoveAnimation
        : public LayerTreeHostTimelinesTest {
    public:
        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();
            layer_ = FakePictureLayer::Create(layer_settings(), &client_);
            layer_->SetBounds(gfx::Size(4, 4));
            layer_tree_host()->root_layer()->AddChild(layer_);

            AttachPlayersToTimeline();

            player_->AttachLayer(layer_tree_host()->root_layer()->id());
            player_child_->AttachLayer(layer_->id());
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DidCommit() override
        {
            switch (layer_tree_host()->source_frame_number()) {
            case 1:
                AddAnimatedTransformToPlayer(player_child_.get(), 1.0, 5, 5);
                break;
            case 2:
                LayerAnimationController* controller = player_child_->element_animations()->layer_animation_controller();
                Animation* animation = controller->GetAnimation(Animation::TRANSFORM);
                player_child_->RemoveAnimation(animation->id());
                gfx::Transform transform;
                transform.Translate(10.f, 10.f);
                layer_->SetTransform(transform);

                // Do something that causes property trees to get rebuilt.
                layer_->AddChild(Layer::Create(layer_settings()));
                break;
            }
        }

        void DrawLayersOnThread(LayerTreeHostImpl* host_impl) override
        {
            if (host_impl->active_tree()->source_frame_number() < 2)
                return;
            gfx::Transform expected_transform;
            expected_transform.Translate(10.f, 10.f);
            EXPECT_EQ(expected_transform, host_impl->active_tree()->root_layer()->children()[0]->draw_transform());
            EndTest();
        }

        void AfterTest() override { }

    private:
        scoped_refptr<Layer> layer_;
        FakeContentLayerClient client_;
    };

    SINGLE_AND_MULTI_THREAD_TEST_F(LayerTreeHostTimelinesTestRemoveAnimation);

    class LayerTreeHostTimelinesTestAnimationFinishesDuringCommit
        : public LayerTreeHostTimelinesTest {
    public:
        void SetupTree() override
        {
            LayerTreeHostTimelinesTest::SetupTree();
            layer_ = FakePictureLayer::Create(layer_settings(), &client_);
            layer_->SetBounds(gfx::Size(4, 4));
            layer_tree_host()->root_layer()->AddChild(layer_);

            AttachPlayersToTimeline();

            player_->AttachLayer(layer_tree_host()->root_layer()->id());
            player_child_->AttachLayer(layer_->id());
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DidCommit() override
        {
            if (layer_tree_host()->source_frame_number() == 1)
                AddAnimatedTransformToPlayer(player_child_.get(), 0.04, 5, 5);
        }

        void WillCommit() override
        {
            if (layer_tree_host()->source_frame_number() == 2) {
                // Block until the animation finishes on the compositor thread. Since
                // animations have already been ticked on the main thread, when the commit
                // happens the state on the main thread will be consistent with having a
                // running animation but the state on the compositor thread will be
                // consistent with having only a finished animation.
                completion_.Wait();
            }
        }

        void CommitCompleteOnThread(LayerTreeHostImpl* host_impl) override
        {
            switch (host_impl->sync_tree()->source_frame_number()) {
            case 1:
                PostSetNeedsCommitToMainThread();
                break;
            case 2:
                gfx::Transform expected_transform;
                expected_transform.Translate(5.f, 5.f);
                LayerImpl* layer_impl = host_impl->sync_tree()->root_layer()->children()[0];
                EXPECT_EQ(expected_transform, layer_impl->draw_transform());
                EndTest();
                break;
            }
        }

        void UpdateAnimationState(LayerTreeHostImpl* host_impl,
            bool has_unfinished_animation) override
        {
            if (host_impl->active_tree()->source_frame_number() == 1 && !has_unfinished_animation) {
                // The animation has finished, so allow the main thread to commit.
                completion_.Signal();
            }
        }

        void AfterTest() override { }

    private:
        scoped_refptr<Layer> layer_;
        FakeContentLayerClient client_;
        CompletionEvent completion_;
    };

    // An animation finishing during commit can only happen when we have a separate
    // compositor thread.
    MULTI_THREAD_TEST_F(LayerTreeHostTimelinesTestAnimationFinishesDuringCommit);

} // namespace
} // namespace cc
