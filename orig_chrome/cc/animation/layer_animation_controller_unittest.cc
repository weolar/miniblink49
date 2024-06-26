// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/layer_animation_controller.h"

#include "cc/animation/animation.h"
#include "cc/animation/animation_curve.h"
#include "cc/animation/animation_delegate.h"
#include "cc/animation/animation_registrar.h"
#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/scroll_offset_animation_curve.h"
#include "cc/animation/transform_operations.h"
#include "cc/test/animation_test_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/box_f.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {

    using base::TimeDelta;
    using base::TimeTicks;

    static base::TimeTicks TicksFromSecondsF(double seconds)
    {
        return base::TimeTicks::FromInternalValue(seconds * base::Time::kMicrosecondsPerSecond);
    }

    // A LayerAnimationController cannot be ticked at 0.0, since an animation
    // with start time 0.0 is treated as an animation whose start time has
    // not yet been set.
    const TimeTicks kInitialTickTime = TicksFromSecondsF(1.0);

    scoped_ptr<Animation> CreateAnimation(scoped_ptr<AnimationCurve> curve,
        int group_id,
        Animation::TargetProperty property)
    {
        return Animation::Create(curve.Pass(), 0, group_id, property);
    }

    TEST(LayerAnimationControllerTest, SyncNewAnimation)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        EXPECT_FALSE(controller_impl->GetAnimation(Animation::OPACITY));

        EXPECT_FALSE(controller->needs_to_start_animations_for_testing());
        EXPECT_FALSE(controller_impl->needs_to_start_animations_for_testing());

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0, 1, false);
        EXPECT_TRUE(controller->needs_to_start_animations_for_testing());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_TRUE(controller_impl->needs_to_start_animations_for_testing());
        controller_impl->ActivateAnimations();

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());
    }

    // If an animation is started on the impl thread before it is ticked on the main
    // thread, we must be sure to respect the synchronized start time.
    TEST(LayerAnimationControllerTest, DoNotClobberStartTimes)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        EXPECT_FALSE(controller_impl->GetAnimation(Animation::OPACITY));

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0, 1, false);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());

        AnimationEventsVector events;
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, &events);

        // Synchronize the start times.
        EXPECT_EQ(1u, events.size());
        controller->NotifyAnimationStarted(events[0]);
        EXPECT_EQ(controller->GetAnimationById(animation_id)->start_time(),
            controller_impl->GetAnimationById(animation_id)->start_time());

        // Start the animation on the main thread. Should not affect the start time.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller->UpdateState(true, nullptr);
        EXPECT_EQ(controller->GetAnimationById(animation_id)->start_time(),
            controller_impl->GetAnimationById(animation_id)->start_time());
    }

    TEST(LayerAnimationControllerTest, UseSpecifiedStartTimes)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0, 1, false);

        const TimeTicks start_time = TicksFromSecondsF(123);
        controller->GetAnimation(Animation::OPACITY)->set_start_time(start_time);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());

        AnimationEventsVector events;
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, &events);

        // Synchronize the start times.
        EXPECT_EQ(1u, events.size());
        controller->NotifyAnimationStarted(events[0]);

        EXPECT_EQ(start_time,
            controller->GetAnimationById(animation_id)->start_time());
        EXPECT_EQ(controller->GetAnimationById(animation_id)->start_time(),
            controller_impl->GetAnimationById(animation_id)->start_time());

        // Start the animation on the main thread. Should not affect the start time.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller->UpdateState(true, nullptr);
        EXPECT_EQ(start_time,
            controller->GetAnimationById(animation_id)->start_time());
        EXPECT_EQ(controller->GetAnimationById(animation_id)->start_time(),
            controller_impl->GetAnimationById(animation_id)->start_time());
    }

    // Tests that controllers activate and deactivate as expected.
    TEST(LayerAnimationControllerTest, Activation)
    {
        scoped_ptr<AnimationRegistrar> registrar = AnimationRegistrar::Create();
        scoped_ptr<AnimationRegistrar> registrar_impl = AnimationRegistrar::Create();

        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));

        controller->SetAnimationRegistrar(registrar.get());
        controller_impl->SetAnimationRegistrar(registrar_impl.get());
        EXPECT_EQ(1u, registrar->all_animation_controllers_for_testing().size());
        EXPECT_EQ(1u, registrar_impl->all_animation_controllers_for_testing().size());

        // Initially, both controllers should be inactive.
        EXPECT_EQ(0u, registrar->active_animation_controllers_for_testing().size());
        EXPECT_EQ(0u,
            registrar_impl->active_animation_controllers_for_testing().size());

        AddOpacityTransitionToController(controller.get(), 1, 0, 1, false);
        // The main thread controller should now be active.
        EXPECT_EQ(1u, registrar->active_animation_controllers_for_testing().size());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        // Both controllers should now be active.
        EXPECT_EQ(1u, registrar->active_animation_controllers_for_testing().size());
        EXPECT_EQ(1u,
            registrar_impl->active_animation_controllers_for_testing().size());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_EQ(1u, events->size());
        controller->NotifyAnimationStarted((*events)[0]);

        EXPECT_EQ(1u, registrar->active_animation_controllers_for_testing().size());
        EXPECT_EQ(1u,
            registrar_impl->active_animation_controllers_for_testing().size());

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller->UpdateState(true, nullptr);
        EXPECT_EQ(1u, registrar->active_animation_controllers_for_testing().size());

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, nullptr);
        EXPECT_EQ(Animation::FINISHED,
            controller->GetAnimation(Animation::OPACITY)->run_state());
        EXPECT_EQ(1u, registrar->active_animation_controllers_for_testing().size());

        events.reset(new AnimationEventsVector);
        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1500));
        controller_impl->UpdateState(true, events.get());

        EXPECT_EQ(Animation::WAITING_FOR_DELETION,
            controller_impl->GetAnimation(Animation::OPACITY)->run_state());
        // The impl thread controller should have de-activated.
        EXPECT_EQ(0u,
            registrar_impl->active_animation_controllers_for_testing().size());

        EXPECT_EQ(1u, events->size());
        controller->NotifyAnimationFinished((*events)[0]);
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1500));
        controller->UpdateState(true, nullptr);

        EXPECT_EQ(Animation::WAITING_FOR_DELETION,
            controller->GetAnimation(Animation::OPACITY)->run_state());
        // The main thread controller should have de-activated.
        EXPECT_EQ(0u, registrar->active_animation_controllers_for_testing().size());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->has_any_animation());
        EXPECT_FALSE(controller_impl->has_any_animation());
        EXPECT_EQ(0u, registrar->active_animation_controllers_for_testing().size());
        EXPECT_EQ(0u,
            registrar_impl->active_animation_controllers_for_testing().size());

        controller->SetAnimationRegistrar(nullptr);
        controller_impl->SetAnimationRegistrar(nullptr);
    }

    TEST(LayerAnimationControllerTest, SyncPause)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        EXPECT_FALSE(controller_impl->GetAnimation(Animation::OPACITY));

        // Two steps, three ranges: [0-1) -> 0.2, [1-2) -> 0.3, [2-3] -> 0.4.
        const double duration = 3.0;
        const int animation_id = AddOpacityStepsToController(controller.get(), duration, 0.2f, 0.4f, 2);

        // Set start offset to be at the beginning of the second range.
        controller->GetAnimationById(animation_id)
            ->set_time_offset(TimeDelta::FromSecondsD(1.01));

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());

        TimeTicks time = kInitialTickTime;

        // Start the animations on each controller.
        AnimationEventsVector events;
        controller_impl->Animate(time);
        controller_impl->UpdateState(true, &events);
        EXPECT_EQ(1u, events.size());

        controller->Animate(time);
        controller->UpdateState(true, nullptr);
        controller->NotifyAnimationStarted(events[0]);

        EXPECT_EQ(Animation::RUNNING,
            controller_impl->GetAnimationById(animation_id)->run_state());
        EXPECT_EQ(Animation::RUNNING,
            controller->GetAnimationById(animation_id)->run_state());

        EXPECT_EQ(0.3f, dummy.opacity());
        EXPECT_EQ(0.3f, dummy_impl.opacity());

        EXPECT_EQ(kInitialTickTime,
            controller->GetAnimationById(animation_id)->start_time());
        EXPECT_EQ(kInitialTickTime,
            controller_impl->GetAnimationById(animation_id)->start_time());

        // Pause the animation at the middle of the second range so the offset
        // delays animation until the middle of the third range.
        controller->PauseAnimation(animation_id, TimeDelta::FromSecondsD(1.5));
        EXPECT_EQ(Animation::PAUSED,
            controller->GetAnimationById(animation_id)->run_state());

        // The pause run state change should make it to the impl thread controller.
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        // Advance time so it stays within the first range.
        time += TimeDelta::FromMilliseconds(10);
        controller->Animate(time);
        controller_impl->Animate(time);

        EXPECT_EQ(Animation::PAUSED,
            controller_impl->GetAnimationById(animation_id)->run_state());

        // Opacity value doesn't depend on time if paused at specified time offset.
        EXPECT_EQ(0.4f, dummy.opacity());
        EXPECT_EQ(0.4f, dummy_impl.opacity());
    }

    TEST(LayerAnimationControllerTest, DoNotSyncFinishedAnimation)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));

        EXPECT_FALSE(controller_impl->GetAnimation(Animation::OPACITY));

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0, 1, false);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());

        events.reset(new AnimationEventsVector);
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_EQ(1u, events->size());
        EXPECT_EQ(AnimationEvent::STARTED, (*events)[0].type);

        // Notify main thread controller that the animation has started.
        controller->NotifyAnimationStarted((*events)[0]);

        // Complete animation on impl thread.
        events.reset(new AnimationEventsVector);
        controller_impl->Animate(kInitialTickTime + TimeDelta::FromSeconds(1));
        controller_impl->UpdateState(true, events.get());
        EXPECT_EQ(1u, events->size());
        EXPECT_EQ(AnimationEvent::FINISHED, (*events)[0].type);

        controller->NotifyAnimationFinished((*events)[0]);

        controller->Animate(kInitialTickTime + TimeDelta::FromSeconds(2));
        controller->UpdateState(true, nullptr);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->GetAnimationById(animation_id));
        EXPECT_FALSE(controller_impl->GetAnimationById(animation_id));
    }

    // Ensure that a finished animation is eventually deleted by both the
    // main-thread and the impl-thread controllers.
    TEST(LayerAnimationControllerTest, AnimationsAreDeleted)
    {
        FakeLayerAnimationValueObserver dummy;
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        controller_impl->AddValueObserver(&dummy_impl);

        AddOpacityTransitionToController(controller.get(), 1.0, 0.0f, 1.0f, false);
        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, nullptr);
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller_impl->UpdateState(true, events.get());

        // There should be a STARTED event for the animation.
        EXPECT_EQ(1u, events->size());
        EXPECT_EQ(AnimationEvent::STARTED, (*events)[0].type);
        controller->NotifyAnimationStarted((*events)[0]);

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, nullptr);

        EXPECT_FALSE(dummy.animation_waiting_for_deletion());
        EXPECT_FALSE(dummy_impl.animation_waiting_for_deletion());

        events.reset(new AnimationEventsVector);
        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller_impl->UpdateState(true, events.get());

        EXPECT_TRUE(dummy_impl.animation_waiting_for_deletion());

        // There should be a FINISHED event for the animation.
        EXPECT_EQ(1u, events->size());
        EXPECT_EQ(AnimationEvent::FINISHED, (*events)[0].type);

        // Neither controller should have deleted the animation yet.
        EXPECT_TRUE(controller->GetAnimation(Animation::OPACITY));
        EXPECT_TRUE(controller_impl->GetAnimation(Animation::OPACITY));

        controller->NotifyAnimationFinished((*events)[0]);

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(3000));
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(dummy.animation_waiting_for_deletion());

        controller->PushAnimationUpdatesTo(controller_impl.get());

        // Both controllers should now have deleted the animation. The impl controller
        // should have deleted the animation even though activation has not occurred,
        // since the animation was already waiting for deletion when
        // PushAnimationUpdatesTo was called.
        EXPECT_FALSE(controller->has_any_animation());
        EXPECT_FALSE(controller_impl->has_any_animation());
    }

    // Tests that transitioning opacity from 0 to 1 works as expected.

    static const AnimationEvent* GetMostRecentPropertyUpdateEvent(
        const AnimationEventsVector* events)
    {
        const AnimationEvent* event = 0;
        for (size_t i = 0; i < events->size(); ++i)
            if ((*events)[i].type == AnimationEvent::PROPERTY_UPDATE)
                event = &(*events)[i];

        return event;
    }

    TEST(LayerAnimationControllerTest, TrivialTransition)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));

        EXPECT_FALSE(controller->needs_to_start_animations_for_testing());
        controller->AddAnimation(to_add.Pass());
        EXPECT_TRUE(controller->needs_to_start_animations_for_testing());
        controller->Animate(kInitialTickTime);
        EXPECT_FALSE(controller->needs_to_start_animations_for_testing());
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        // A non-impl-only animation should not generate property updates.
        const AnimationEvent* event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(1.f, dummy.opacity());
        EXPECT_FALSE(controller->HasActiveAnimation());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);
    }

    TEST(LayerAnimationControllerTest, TrivialTransitionOnImpl)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        to_add->set_is_impl_only(true);

        controller_impl->AddAnimation(to_add.Pass());
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_TRUE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy_impl.opacity());
        EXPECT_EQ(1u, events->size());
        const AnimationEvent* start_opacity_event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_EQ(0.f, start_opacity_event->opacity);

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());
        EXPECT_EQ(1.f, dummy_impl.opacity());
        EXPECT_FALSE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(2u, events->size());
        const AnimationEvent* end_opacity_event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_EQ(1.f, end_opacity_event->opacity);
    }

    TEST(LayerAnimationControllerTest, TrivialTransformOnImpl)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);

        // Choose different values for x and y to avoid coincidental values in the
        // observed transforms.
        const float delta_x = 3;
        const float delta_y = 4;

        scoped_ptr<KeyframedTransformAnimationCurve> curve(
            KeyframedTransformAnimationCurve::Create());

        // Create simple TRANSFORM animation.
        TransformOperations operations;
        curve->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations, nullptr));
        operations.AppendTranslate(delta_x, delta_y, 0);
        curve->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations, nullptr));

        scoped_ptr<Animation> animation(
            Animation::Create(curve.Pass(), 1, 0, Animation::TRANSFORM));
        animation->set_is_impl_only(true);
        controller_impl->AddAnimation(animation.Pass());

        // Run animation.
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_TRUE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(gfx::Transform(), dummy_impl.transform());
        EXPECT_EQ(1u, events->size());
        const AnimationEvent* start_transform_event = GetMostRecentPropertyUpdateEvent(events.get());
        ASSERT_TRUE(start_transform_event);
        EXPECT_EQ(gfx::Transform(), start_transform_event->transform);
        EXPECT_TRUE(start_transform_event->is_impl_only);

        gfx::Transform expected_transform;
        expected_transform.Translate(delta_x, delta_y);

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());
        EXPECT_EQ(expected_transform, dummy_impl.transform());
        EXPECT_FALSE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(2u, events->size());
        const AnimationEvent* end_transform_event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_EQ(expected_transform, end_transform_event->transform);
        EXPECT_TRUE(end_transform_event->is_impl_only);
    }

    TEST(LayerAnimationControllerTest, FilterTransition)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        scoped_ptr<KeyframedFilterAnimationCurve> curve(
            KeyframedFilterAnimationCurve::Create());

        FilterOperations start_filters;
        start_filters.Append(FilterOperation::CreateBrightnessFilter(1.f));
        curve->AddKeyframe(
            FilterKeyframe::Create(base::TimeDelta(), start_filters, nullptr));
        FilterOperations end_filters;
        end_filters.Append(FilterOperation::CreateBrightnessFilter(2.f));
        curve->AddKeyframe(FilterKeyframe::Create(base::TimeDelta::FromSecondsD(1.0),
            end_filters, nullptr));

        scoped_ptr<Animation> animation(
            Animation::Create(curve.Pass(), 1, 0, Animation::FILTER));
        controller->AddAnimation(animation.Pass());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(start_filters, dummy.filters());
        // A non-impl-only animation should not generate property updates.
        const AnimationEvent* event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(1u, dummy.filters().size());
        EXPECT_EQ(FilterOperation::CreateBrightnessFilter(1.5f),
            dummy.filters().at(0));
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(end_filters, dummy.filters());
        EXPECT_FALSE(controller->HasActiveAnimation());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);
    }

    TEST(LayerAnimationControllerTest, FilterTransitionOnImplOnly)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);

        scoped_ptr<KeyframedFilterAnimationCurve> curve(
            KeyframedFilterAnimationCurve::Create());

        // Create simple FILTER animation.
        FilterOperations start_filters;
        start_filters.Append(FilterOperation::CreateBrightnessFilter(1.f));
        curve->AddKeyframe(
            FilterKeyframe::Create(base::TimeDelta(), start_filters, nullptr));
        FilterOperations end_filters;
        end_filters.Append(FilterOperation::CreateBrightnessFilter(2.f));
        curve->AddKeyframe(FilterKeyframe::Create(base::TimeDelta::FromSecondsD(1.0),
            end_filters, nullptr));

        scoped_ptr<Animation> animation(
            Animation::Create(curve.Pass(), 1, 0, Animation::FILTER));
        animation->set_is_impl_only(true);
        controller_impl->AddAnimation(animation.Pass());

        // Run animation.
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_TRUE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(start_filters, dummy_impl.filters());
        EXPECT_EQ(1u, events->size());
        const AnimationEvent* start_filter_event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_TRUE(start_filter_event);
        EXPECT_EQ(start_filters, start_filter_event->filters);
        EXPECT_TRUE(start_filter_event->is_impl_only);

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());
        EXPECT_EQ(end_filters, dummy_impl.filters());
        EXPECT_FALSE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(2u, events->size());
        const AnimationEvent* end_filter_event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_TRUE(end_filter_event);
        EXPECT_EQ(end_filters, end_filter_event->filters);
        EXPECT_TRUE(end_filter_event->is_impl_only);
    }

    TEST(LayerAnimationControllerTest, ScrollOffsetTransition)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        FakeLayerAnimationValueProvider dummy_provider_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        controller_impl->set_value_provider(&dummy_provider_impl);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        FakeLayerAnimationValueProvider dummy_provider;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        controller->set_value_provider(&dummy_provider);

        gfx::ScrollOffset initial_value(100.f, 300.f);
        gfx::ScrollOffset target_value(300.f, 200.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value,
                EaseInOutTimingFunction::Create().Pass()));

        scoped_ptr<Animation> animation(
            Animation::Create(curve.Pass(), 1, 0, Animation::SCROLL_OFFSET));
        animation->set_needs_synchronized_start_time(true);
        controller->AddAnimation(animation.Pass());

        dummy_provider_impl.set_scroll_offset(initial_value);
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->GetAnimation(Animation::SCROLL_OFFSET));
        TimeDelta duration = controller_impl->GetAnimation(Animation::SCROLL_OFFSET)
                                 ->curve()
                                 ->Duration();
        EXPECT_EQ(
            duration,
            controller->GetAnimation(Animation::SCROLL_OFFSET)->curve()->Duration());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(initial_value, dummy.scroll_offset());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_TRUE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(initial_value, dummy_impl.scroll_offset());
        // Scroll offset animations should not generate property updates.
        const AnimationEvent* event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller->NotifyAnimationStarted((*events)[0]);
        controller->Animate(kInitialTickTime + duration / 2);
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_VECTOR2DF_EQ(gfx::Vector2dF(200.f, 250.f), dummy.scroll_offset());

        controller_impl->Animate(kInitialTickTime + duration / 2);
        controller_impl->UpdateState(true, events.get());
        EXPECT_VECTOR2DF_EQ(gfx::Vector2dF(200.f, 250.f),
            dummy_impl.scroll_offset());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller_impl->Animate(kInitialTickTime + duration);
        controller_impl->UpdateState(true, events.get());
        EXPECT_VECTOR2DF_EQ(target_value, dummy_impl.scroll_offset());
        EXPECT_FALSE(controller_impl->HasActiveAnimation());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller->Animate(kInitialTickTime + duration);
        controller->UpdateState(true, nullptr);
        EXPECT_VECTOR2DF_EQ(target_value, dummy.scroll_offset());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    // Ensure that when the impl controller doesn't have a value provider,
    // the main-thread controller's value provider is used to obtain the intial
    // scroll offset.
    TEST(LayerAnimationControllerTest, ScrollOffsetTransitionNoImplProvider)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        FakeLayerAnimationValueProvider dummy_provider;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        controller->set_value_provider(&dummy_provider);

        gfx::ScrollOffset initial_value(500.f, 100.f);
        gfx::ScrollOffset target_value(300.f, 200.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value,
                EaseInOutTimingFunction::Create().Pass()));

        scoped_ptr<Animation> animation(
            Animation::Create(curve.Pass(), 1, 0, Animation::SCROLL_OFFSET));
        animation->set_needs_synchronized_start_time(true);
        controller->AddAnimation(animation.Pass());

        dummy_provider.set_scroll_offset(initial_value);
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->GetAnimation(Animation::SCROLL_OFFSET));
        TimeDelta duration = controller_impl->GetAnimation(Animation::SCROLL_OFFSET)
                                 ->curve()
                                 ->Duration();
        EXPECT_EQ(
            duration,
            controller->GetAnimation(Animation::SCROLL_OFFSET)->curve()->Duration());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(initial_value, dummy.scroll_offset());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_TRUE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(initial_value, dummy_impl.scroll_offset());
        // Scroll offset animations should not generate property updates.
        const AnimationEvent* event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller->NotifyAnimationStarted((*events)[0]);
        controller->Animate(kInitialTickTime + duration / 2);
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_VECTOR2DF_EQ(gfx::Vector2dF(400.f, 150.f), dummy.scroll_offset());

        controller_impl->Animate(kInitialTickTime + duration / 2);
        controller_impl->UpdateState(true, events.get());
        EXPECT_VECTOR2DF_EQ(gfx::Vector2dF(400.f, 150.f),
            dummy_impl.scroll_offset());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller_impl->Animate(kInitialTickTime + duration);
        controller_impl->UpdateState(true, events.get());
        EXPECT_VECTOR2DF_EQ(target_value, dummy_impl.scroll_offset());
        EXPECT_FALSE(controller_impl->HasActiveAnimation());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller->Animate(kInitialTickTime + duration);
        controller->UpdateState(true, nullptr);
        EXPECT_VECTOR2DF_EQ(target_value, dummy.scroll_offset());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    TEST(LayerAnimationControllerTest, ScrollOffsetTransitionOnImplOnly)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));

        gfx::ScrollOffset initial_value(100.f, 300.f);
        gfx::ScrollOffset target_value(300.f, 200.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value,
                EaseInOutTimingFunction::Create().Pass()));
        curve->SetInitialValue(initial_value);
        double duration_in_seconds = curve->Duration().InSecondsF();

        scoped_ptr<Animation> animation(
            Animation::Create(curve.Pass(), 1, 0, Animation::SCROLL_OFFSET));
        animation->set_is_impl_only(true);
        controller_impl->AddAnimation(animation.Pass());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_TRUE(controller_impl->HasActiveAnimation());
        EXPECT_EQ(initial_value, dummy_impl.scroll_offset());
        // Scroll offset animations should not generate property updates.
        const AnimationEvent* event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        TimeDelta duration = TimeDelta::FromMicroseconds(
            duration_in_seconds * base::Time::kMicrosecondsPerSecond);

        controller_impl->Animate(kInitialTickTime + duration / 2);
        controller_impl->UpdateState(true, events.get());
        EXPECT_VECTOR2DF_EQ(gfx::Vector2dF(200.f, 250.f),
            dummy_impl.scroll_offset());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);

        controller_impl->Animate(kInitialTickTime + duration);
        controller_impl->UpdateState(true, events.get());
        EXPECT_VECTOR2DF_EQ(target_value, dummy_impl.scroll_offset());
        EXPECT_FALSE(controller_impl->HasActiveAnimation());
        event = GetMostRecentPropertyUpdateEvent(events.get());
        EXPECT_FALSE(event);
    }

    TEST(LayerAnimationControllerTest, ScrollOffsetRemovalClearsScrollDelta)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        FakeLayerAnimationValueProvider dummy_provider_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        controller_impl->set_value_provider(&dummy_provider_impl);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        FakeLayerAnimationValueProvider dummy_provider;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        controller->set_value_provider(&dummy_provider);

        // First test the 1-argument version of RemoveAnimation.
        gfx::ScrollOffset target_value(300.f, 200.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value, EaseInOutTimingFunction::Create().Pass()));

        int animation_id = 1;
        scoped_ptr<Animation> animation(Animation::Create(
            curve.Pass(), animation_id, 0, Animation::SCROLL_OFFSET));
        animation->set_needs_synchronized_start_time(true);
        controller->AddAnimation(animation.Pass());
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());

        controller->RemoveAnimation(animation_id);
        EXPECT_TRUE(controller->scroll_offset_animation_was_interrupted());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_TRUE(controller_impl->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());

        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());

        // Now, test the 2-argument version of RemoveAnimation.
        curve = ScrollOffsetAnimationCurve::Create(
            target_value, EaseInOutTimingFunction::Create().Pass());
        animation = Animation::Create(curve.Pass(), animation_id, 0,
            Animation::SCROLL_OFFSET);
        animation->set_needs_synchronized_start_time(true);
        controller->AddAnimation(animation.Pass());
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());

        controller->RemoveAnimation(animation_id);
        EXPECT_TRUE(controller->scroll_offset_animation_was_interrupted());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_TRUE(controller_impl->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());

        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());

        // Check that removing non-scroll-offset animations does not cause
        // scroll_offset_animation_was_interrupted() to get set.
        animation_id = AddAnimatedTransformToController(controller.get(), 1.0, 1, 2);
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());

        controller->RemoveAnimation(animation_id);
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());

        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());

        animation_id = AddAnimatedFilterToController(controller.get(), 1.0, 0.1f, 0.2f);
        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());

        controller->RemoveAnimation(animation_id);
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());
        EXPECT_FALSE(controller->scroll_offset_animation_was_interrupted());

        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller_impl->scroll_offset_animation_was_interrupted());
    }

    class FakeAnimationDelegate : public AnimationDelegate {
    public:
        FakeAnimationDelegate()
            : started_(false)
            , finished_(false)
            , start_time_(base::TimeTicks())
        {
        }

        void NotifyAnimationStarted(TimeTicks monotonic_time,
            Animation::TargetProperty target_property,
            int group) override
        {
            started_ = true;
            start_time_ = monotonic_time;
        }

        void NotifyAnimationFinished(TimeTicks monotonic_time,
            Animation::TargetProperty target_property,
            int group) override
        {
            finished_ = true;
        }

        bool started() { return started_; }

        bool finished() { return finished_; }

        TimeTicks start_time() { return start_time_; }

    private:
        bool started_;
        bool finished_;
        TimeTicks start_time_;
    };

    // Tests that impl-only animations lead to start and finished notifications
    // on the impl thread controller's animation delegate.
    TEST(LayerAnimationControllerTest,
        NotificationsForImplOnlyAnimationsAreSentToImplThreadDelegate)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeAnimationDelegate delegate;
        controller_impl->set_layer_animation_delegate(&delegate);

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        to_add->set_is_impl_only(true);
        controller_impl->AddAnimation(to_add.Pass());

        EXPECT_FALSE(delegate.started());
        EXPECT_FALSE(delegate.finished());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());

        EXPECT_TRUE(delegate.started());
        EXPECT_FALSE(delegate.finished());

        events.reset(new AnimationEventsVector);
        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());

        EXPECT_TRUE(delegate.started());
        EXPECT_TRUE(delegate.finished());
    }

    // Tests that specified start times are sent to the main thread delegate
    TEST(LayerAnimationControllerTest,
        SpecifiedStartTimesAreSentToMainThreadDelegate)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        FakeAnimationDelegate delegate;
        controller->set_layer_animation_delegate(&delegate);

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0, 1, false);

        const TimeTicks start_time = TicksFromSecondsF(123);
        controller->GetAnimation(Animation::OPACITY)->set_start_time(start_time);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());

        AnimationEventsVector events;
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, &events);

        // Synchronize the start times.
        EXPECT_EQ(1u, events.size());
        controller->NotifyAnimationStarted(events[0]);

        // Validate start time on the main thread delegate.
        EXPECT_EQ(start_time, delegate.start_time());
    }

    class FakeLayerAnimationEventObserver : public LayerAnimationEventObserver {
    public:
        FakeLayerAnimationEventObserver()
            : start_time_(base::TimeTicks())
        {
        }

        void OnAnimationStarted(const AnimationEvent& event) override
        {
            start_time_ = event.monotonic_time;
        }

        TimeTicks start_time() { return start_time_; }

    private:
        TimeTicks start_time_;
    };

    // Tests that specified start times are sent to the event observers
    TEST(LayerAnimationControllerTest, SpecifiedStartTimesAreSentToEventObservers)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        FakeLayerAnimationEventObserver observer;
        controller->AddEventObserver(&observer);

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0, 1, false);

        const TimeTicks start_time = TicksFromSecondsF(123);
        controller->GetAnimation(Animation::OPACITY)->set_start_time(start_time);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());

        AnimationEventsVector events;
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, &events);

        // Synchronize the start times.
        EXPECT_EQ(1u, events.size());
        controller->NotifyAnimationStarted(events[0]);

        // Validate start time on the event observer.
        EXPECT_EQ(start_time, observer.start_time());
    }

    // Tests animations that are waiting for a synchronized start time do not
    // finish.
    TEST(LayerAnimationControllerTest,
        AnimationsWaitingForStartTimeDoNotFinishIfTheyOutwaitTheirFinish)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        to_add->set_needs_synchronized_start_time(true);

        // We should pause at the first keyframe indefinitely waiting for that
        // animation to start.
        controller->AddAnimation(to_add.Pass());
        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());

        // Send the synchronized start time.
        controller->NotifyAnimationStarted(
            AnimationEvent(AnimationEvent::STARTED, 0, 1, Animation::OPACITY,
                kInitialTickTime + TimeDelta::FromMilliseconds(2000)));
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(5000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(1.f, dummy.opacity());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    // Tests that two queued animations affecting the same property run in sequence.
    TEST(LayerAnimationControllerTest, TrivialQueuing)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        EXPECT_FALSE(controller->needs_to_start_animations_for_testing());

        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 1.f, 0.5f))
                .Pass(),
            2, Animation::OPACITY));

        EXPECT_TRUE(controller->needs_to_start_animations_for_testing());

        controller->Animate(kInitialTickTime);

        // The second animation still needs to be started.
        EXPECT_TRUE(controller->needs_to_start_animations_for_testing());

        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        EXPECT_TRUE(controller->needs_to_start_animations_for_testing());
        controller->UpdateState(true, events.get());
        EXPECT_FALSE(controller->needs_to_start_animations_for_testing());

        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(1.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(0.5f, dummy.opacity());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    // Tests interrupting a transition with another transition.
    TEST(LayerAnimationControllerTest, Interrupt)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);
        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 1.f, 0.5f))
                .Pass(),
            2, Animation::OPACITY));
        controller->AbortAnimations(Animation::OPACITY);
        controller->AddAnimation(to_add.Pass());

        // Since the previous animation was aborted, the new animation should start
        // right in this call to animate.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(1.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1500));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(0.5f, dummy.opacity());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    // Tests scheduling two animations to run together when only one property is
    // free.
    TEST(LayerAnimationControllerTest, ScheduleTogetherWhenAPropertyIsBlocked)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(1)).Pass(), 1,
            Animation::TRANSFORM));
        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(1)).Pass(), 2,
            Animation::TRANSFORM));
        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            2, Animation::OPACITY));

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_EQ(0.f, dummy.opacity());
        EXPECT_TRUE(controller->HasActiveAnimation());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, events.get());
        // Should not have started the float transition yet.
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        // The float animation should have started at time 1 and should be done.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(1.f, dummy.opacity());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    // Tests scheduling two animations to run together with different lengths and
    // another animation queued to start when the shorter animation finishes (should
    // wait for both to finish).
    TEST(LayerAnimationControllerTest, ScheduleTogetherWithAnAnimWaiting)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(2)).Pass(), 1,
            Animation::TRANSFORM));
        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 1.f, 0.5f))
                .Pass(),
            2, Animation::OPACITY));

        // Animations with id 1 should both start now.
        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        // The opacity animation should have finished at time 1, but the group
        // of animations with id 1 don't finish until time 2 because of the length
        // of the transform animation.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller->UpdateState(true, events.get());
        // Should not have started the float transition yet.
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(1.f, dummy.opacity());

        // The second opacity animation should start at time 2 and should be done by
        // time 3.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(3000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(0.5f, dummy.opacity());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    // Test that a looping animation loops and for the correct number of iterations.
    TEST(LayerAnimationControllerTest, TrivialLooping)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        to_add->set_iterations(3);
        controller->AddAnimation(to_add.Pass());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1250));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.25f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1750));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.75f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2250));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.25f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2750));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.75f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(3000));
        controller->UpdateState(true, events.get());
        EXPECT_FALSE(controller->HasActiveAnimation());
        EXPECT_EQ(1.f, dummy.opacity());

        // Just be extra sure.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(4000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(1.f, dummy.opacity());
    }

    // Test that an infinitely looping animation does indeed go until aborted.
    TEST(LayerAnimationControllerTest, InfiniteLooping)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        to_add->set_iterations(-1);
        controller->AddAnimation(to_add.Pass());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1250));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.25f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1750));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.75f, dummy.opacity());

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1073741824250));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.25f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1073741824750));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.75f, dummy.opacity());

        EXPECT_TRUE(controller->GetAnimation(Animation::OPACITY));
        controller->GetAnimation(Animation::OPACITY)
            ->SetRunState(Animation::ABORTED,
                kInitialTickTime + TimeDelta::FromMilliseconds(750));
        EXPECT_FALSE(controller->HasActiveAnimation());
        EXPECT_EQ(0.75f, dummy.opacity());
    }

    // Test that pausing and resuming work as expected.
    TEST(LayerAnimationControllerTest, PauseResume)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.5f, dummy.opacity());

        EXPECT_TRUE(controller->GetAnimation(Animation::OPACITY));
        controller->GetAnimation(Animation::OPACITY)
            ->SetRunState(Animation::PAUSED,
                kInitialTickTime + TimeDelta::FromMilliseconds(500));

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1024000));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.5f, dummy.opacity());

        EXPECT_TRUE(controller->GetAnimation(Animation::OPACITY));
        controller->GetAnimation(Animation::OPACITY)
            ->SetRunState(Animation::RUNNING,
                kInitialTickTime + TimeDelta::FromMilliseconds(1024000));
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1024250));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.75f, dummy.opacity());

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1024500));
        controller->UpdateState(true, events.get());
        EXPECT_FALSE(controller->HasActiveAnimation());
        EXPECT_EQ(1.f, dummy.opacity());
    }

    TEST(LayerAnimationControllerTest, AbortAGroupedAnimation)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        const int animation_id = 2;
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(1)).Pass(), 1, 1,
            Animation::TRANSFORM));
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(2.0, 0.f, 1.f)).Pass(),
            animation_id, 1, Animation::OPACITY));
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 1.f, 0.75f))
                .Pass(),
            3, 2, Animation::OPACITY));

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.5f, dummy.opacity());

        EXPECT_TRUE(controller->GetAnimationById(animation_id));
        controller->GetAnimationById(animation_id)
            ->SetRunState(Animation::ABORTED,
                kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(1.f, dummy.opacity());
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(!controller->HasActiveAnimation());
        EXPECT_EQ(0.75f, dummy.opacity());
    }

    TEST(LayerAnimationControllerTest, PushUpdatesWhenSynchronizedStartTimeNeeded)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        scoped_ptr<Animation> to_add(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(2.0, 0.f, 1.f)).Pass(),
            0, Animation::OPACITY));
        to_add->set_needs_synchronized_start_time(true);
        controller->AddAnimation(to_add.Pass());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_TRUE(controller->HasActiveAnimation());
        Animation* active_animation = controller->GetAnimation(Animation::OPACITY);
        EXPECT_TRUE(active_animation);
        EXPECT_TRUE(active_animation->needs_synchronized_start_time());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();

        active_animation = controller_impl->GetAnimation(Animation::OPACITY);
        EXPECT_TRUE(active_animation);
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            active_animation->run_state());
    }

    // Tests that skipping a call to UpdateState works as expected.
    TEST(LayerAnimationControllerTest, SkipUpdateState)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        scoped_ptr<Animation> first_animation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(1)).Pass(), 1,
            Animation::TRANSFORM));
        first_animation->set_is_controlling_instance_for_test(true);
        controller->AddAnimation(first_animation.Pass());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());

        scoped_ptr<Animation> second_animation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            2, Animation::OPACITY));
        second_animation->set_is_controlling_instance_for_test(true);
        controller->AddAnimation(second_animation.Pass());

        // Animate but don't UpdateState.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        events.reset(new AnimationEventsVector);
        controller->UpdateState(true, events.get());

        // Should have one STARTED event and one FINISHED event.
        EXPECT_EQ(2u, events->size());
        EXPECT_NE((*events)[0].type, (*events)[1].type);

        // The float transition should still be at its starting point.
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_EQ(0.f, dummy.opacity());

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(3000));
        controller->UpdateState(true, events.get());

        // The float tranisition should now be done.
        EXPECT_EQ(1.f, dummy.opacity());
        EXPECT_FALSE(controller->HasActiveAnimation());
    }

    // Tests that an animation controller with only a pending observer gets ticked
    // but doesn't progress animations past the STARTING state.
    TEST(LayerAnimationControllerTest, InactiveObserverGetsTicked)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy;
        FakeInactiveLayerAnimationValueObserver pending_dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));

        const int id = 1;
        controller->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.5f, 1.f))
                .Pass(),
            id, Animation::OPACITY));

        // Without an observer, the animation shouldn't progress to the STARTING
        // state.
        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, events.get());
        EXPECT_EQ(0u, events->size());
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller->GetAnimation(Animation::OPACITY)->run_state());

        controller->AddValueObserver(&pending_dummy);

        // With only a pending observer, the animation should progress to the
        // STARTING state and get ticked at its starting point, but should not
        // progress to RUNNING.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(0u, events->size());
        EXPECT_EQ(Animation::STARTING,
            controller->GetAnimation(Animation::OPACITY)->run_state());
        EXPECT_EQ(0.5f, pending_dummy.opacity());

        // Even when already in the STARTING state, the animation should stay
        // there, and shouldn't be ticked past its starting point.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(0u, events->size());
        EXPECT_EQ(Animation::STARTING,
            controller->GetAnimation(Animation::OPACITY)->run_state());
        EXPECT_EQ(0.5f, pending_dummy.opacity());

        controller->AddValueObserver(&dummy);

        // Now that an active observer has been added, the animation should still
        // initially tick at its starting point, but should now progress to RUNNING.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(3000));
        controller->UpdateState(true, events.get());
        EXPECT_EQ(1u, events->size());
        EXPECT_EQ(Animation::RUNNING,
            controller->GetAnimation(Animation::OPACITY)->run_state());
        EXPECT_EQ(0.5f, pending_dummy.opacity());
        EXPECT_EQ(0.5f, dummy.opacity());

        // The animation should now tick past its starting point.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(3500));
        EXPECT_NE(0.5f, pending_dummy.opacity());
        EXPECT_NE(0.5f, dummy.opacity());
    }

    TEST(LayerAnimationControllerTest, TransformAnimationBounds)
    {
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));

        scoped_ptr<KeyframedTransformAnimationCurve> curve1(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations1;
        curve1->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations1, nullptr));
        operations1.AppendTranslate(10.0, 15.0, 0.0);
        curve1->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations1, nullptr));

        scoped_ptr<Animation> animation(
            Animation::Create(curve1.Pass(), 1, 1, Animation::TRANSFORM));
        controller_impl->AddAnimation(animation.Pass());

        scoped_ptr<KeyframedTransformAnimationCurve> curve2(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations2;
        curve2->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations2, nullptr));
        operations2.AppendScale(2.0, 3.0, 4.0);
        curve2->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations2, nullptr));

        animation = Animation::Create(curve2.Pass(), 2, 2, Animation::TRANSFORM);
        controller_impl->AddAnimation(animation.Pass());

        gfx::BoxF box(1.f, 2.f, -1.f, 3.f, 4.f, 5.f);
        gfx::BoxF bounds;

        EXPECT_TRUE(controller_impl->TransformAnimationBoundsForBox(box, &bounds));
        EXPECT_EQ(gfx::BoxF(1.f, 2.f, -4.f, 13.f, 19.f, 20.f).ToString(),
            bounds.ToString());

        controller_impl->GetAnimationById(1)
            ->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));

        // Only the unfinished animation should affect the animated bounds.
        EXPECT_TRUE(controller_impl->TransformAnimationBoundsForBox(box, &bounds));
        EXPECT_EQ(gfx::BoxF(1.f, 2.f, -4.f, 7.f, 16.f, 20.f).ToString(),
            bounds.ToString());

        controller_impl->GetAnimationById(2)
            ->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));

        // There are no longer any running animations.
        EXPECT_FALSE(controller_impl->HasTransformAnimationThatInflatesBounds());

        // Add an animation whose bounds we don't yet support computing.
        scoped_ptr<KeyframedTransformAnimationCurve> curve3(
            KeyframedTransformAnimationCurve::Create());
        TransformOperations operations3;
        gfx::Transform transform3;
        transform3.Scale3d(1.0, 2.0, 3.0);
        curve3->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations3, nullptr));
        operations3.AppendMatrix(transform3);
        curve3->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations3, nullptr));
        animation = Animation::Create(curve3.Pass(), 3, 3, Animation::TRANSFORM);
        controller_impl->AddAnimation(animation.Pass());
        EXPECT_FALSE(controller_impl->TransformAnimationBoundsForBox(box, &bounds));
    }

    // Tests that AbortAnimations aborts all animations targeting the specified
    // property.
    TEST(LayerAnimationControllerTest, AbortAnimations)
    {
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        // Start with several animations, and allow some of them to reach the finished
        // state.
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(1.0)).Pass(), 1, 1,
            Animation::TRANSFORM));
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            2, 2, Animation::OPACITY));
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(1.0)).Pass(), 3, 3,
            Animation::TRANSFORM));
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(2.0)).Pass(), 4, 4,
            Animation::TRANSFORM));
        controller->AddAnimation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            5, 5, Animation::OPACITY));

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, nullptr);
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, nullptr);

        EXPECT_EQ(Animation::FINISHED, controller->GetAnimationById(1)->run_state());
        EXPECT_EQ(Animation::FINISHED, controller->GetAnimationById(2)->run_state());
        EXPECT_EQ(Animation::RUNNING, controller->GetAnimationById(3)->run_state());
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller->GetAnimationById(4)->run_state());
        EXPECT_EQ(Animation::RUNNING, controller->GetAnimationById(5)->run_state());

        controller->AbortAnimations(Animation::TRANSFORM);

        // Only un-finished TRANSFORM animations should have been aborted.
        EXPECT_EQ(Animation::FINISHED, controller->GetAnimationById(1)->run_state());
        EXPECT_EQ(Animation::FINISHED, controller->GetAnimationById(2)->run_state());
        EXPECT_EQ(Animation::ABORTED, controller->GetAnimationById(3)->run_state());
        EXPECT_EQ(Animation::ABORTED, controller->GetAnimationById(4)->run_state());
        EXPECT_EQ(Animation::RUNNING, controller->GetAnimationById(5)->run_state());
    }

    // An animation aborted on the main thread should get deleted on both threads.
    TEST(LayerAnimationControllerTest, MainThreadAbortedAnimationGetsDeleted)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        int animation_id = AddOpacityTransitionToController(controller.get(), 1.0, 0.f, 1.f, false);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));

        controller->AbortAnimations(Animation::OPACITY);
        EXPECT_EQ(Animation::ABORTED,
            controller->GetAnimation(Animation::OPACITY)->run_state());
        EXPECT_FALSE(dummy.animation_waiting_for_deletion());
        EXPECT_FALSE(dummy_impl.animation_waiting_for_deletion());

        controller->Animate(kInitialTickTime);
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(dummy.animation_waiting_for_deletion());
        EXPECT_EQ(Animation::WAITING_FOR_DELETION,
            controller->GetAnimation(Animation::OPACITY)->run_state());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->GetAnimationById(animation_id));
        EXPECT_FALSE(controller_impl->GetAnimationById(animation_id));
    }

    // An animation aborted on the impl thread should get deleted on both threads.
    TEST(LayerAnimationControllerTest, ImplThreadAbortedAnimationGetsDeleted)
    {
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        int animation_id = AddOpacityTransitionToController(controller.get(), 1.0, 0.f, 1.f, false);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));

        controller_impl->AbortAnimations(Animation::OPACITY);
        EXPECT_EQ(Animation::ABORTED,
            controller_impl->GetAnimation(Animation::OPACITY)->run_state());
        EXPECT_FALSE(dummy.animation_waiting_for_deletion());
        EXPECT_FALSE(dummy_impl.animation_waiting_for_deletion());

        AnimationEventsVector events;
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, &events);
        EXPECT_TRUE(dummy_impl.animation_waiting_for_deletion());
        EXPECT_EQ(1u, events.size());
        EXPECT_EQ(AnimationEvent::ABORTED, events[0].type);
        EXPECT_EQ(Animation::WAITING_FOR_DELETION,
            controller_impl->GetAnimation(Animation::OPACITY)->run_state());

        controller->NotifyAnimationAborted(events[0]);
        EXPECT_EQ(Animation::ABORTED,
            controller->GetAnimation(Animation::OPACITY)->run_state());

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(dummy.animation_waiting_for_deletion());
        EXPECT_EQ(Animation::WAITING_FOR_DELETION,
            controller->GetAnimation(Animation::OPACITY)->run_state());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller->GetAnimationById(animation_id));
        EXPECT_FALSE(controller_impl->GetAnimationById(animation_id));
    }

    // Ensure that we only generate FINISHED events for animations in a group
    // once all animations in that group are finished.
    TEST(LayerAnimationControllerTest, FinishedEventsForGroup)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);

        const int group_id = 1;

        // Add two animations with the same group id but different durations.
        scoped_ptr<Animation> first_animation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(2.0)).Pass(), 1,
            group_id, Animation::TRANSFORM));
        first_animation->set_is_controlling_instance_for_test(true);
        controller_impl->AddAnimation(first_animation.Pass());

        scoped_ptr<Animation> second_animation(Animation::Create(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            2, group_id, Animation::OPACITY));
        second_animation->set_is_controlling_instance_for_test(true);
        controller_impl->AddAnimation(second_animation.Pass());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());

        // Both animations should have started.
        EXPECT_EQ(2u, events->size());
        EXPECT_EQ(AnimationEvent::STARTED, (*events)[0].type);
        EXPECT_EQ(AnimationEvent::STARTED, (*events)[1].type);

        events.reset(new AnimationEventsVector);
        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());

        // The opacity animation should be finished, but should not have generated
        // a FINISHED event yet.
        EXPECT_EQ(0u, events->size());
        EXPECT_EQ(Animation::FINISHED,
            controller_impl->GetAnimationById(2)->run_state());
        EXPECT_EQ(Animation::RUNNING,
            controller_impl->GetAnimationById(1)->run_state());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller_impl->UpdateState(true, events.get());

        // Both animations should have generated FINISHED events.
        EXPECT_EQ(2u, events->size());
        EXPECT_EQ(AnimationEvent::FINISHED, (*events)[0].type);
        EXPECT_EQ(AnimationEvent::FINISHED, (*events)[1].type);
    }

    // Ensure that when a group has a mix of aborted and finished animations,
    // we generate a FINISHED event for the finished animation and an ABORTED
    // event for the aborted animation.
    TEST(LayerAnimationControllerTest, FinishedAndAbortedEventsForGroup)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);

        // Add two animations with the same group id.
        scoped_ptr<Animation> first_animation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeTransformTransition(1.0)).Pass(), 1,
            Animation::TRANSFORM));
        first_animation->set_is_controlling_instance_for_test(true);
        controller_impl->AddAnimation(first_animation.Pass());

        scoped_ptr<Animation> second_animation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        second_animation->set_is_controlling_instance_for_test(true);
        controller_impl->AddAnimation(second_animation.Pass());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());

        // Both animations should have started.
        EXPECT_EQ(2u, events->size());
        EXPECT_EQ(AnimationEvent::STARTED, (*events)[0].type);
        EXPECT_EQ(AnimationEvent::STARTED, (*events)[1].type);

        controller_impl->AbortAnimations(Animation::OPACITY);

        events.reset(new AnimationEventsVector);
        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());

        // We should have exactly 2 events: a FINISHED event for the tranform
        // animation, and an ABORTED event for the opacity animation.
        EXPECT_EQ(2u, events->size());
        EXPECT_EQ(AnimationEvent::FINISHED, (*events)[0].type);
        EXPECT_EQ(Animation::TRANSFORM, (*events)[0].target_property);
        EXPECT_EQ(AnimationEvent::ABORTED, (*events)[1].type);
        EXPECT_EQ(Animation::OPACITY, (*events)[1].target_property);
    }

    TEST(LayerAnimationControllerTest, HasAnimationThatAffectsScale)
    {
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));

        EXPECT_FALSE(controller_impl->HasAnimationThatAffectsScale());

        controller_impl->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));

        // Opacity animations don't affect scale.
        EXPECT_FALSE(controller_impl->HasAnimationThatAffectsScale());

        scoped_ptr<KeyframedTransformAnimationCurve> curve1(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations1;
        curve1->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations1, nullptr));
        operations1.AppendTranslate(10.0, 15.0, 0.0);
        curve1->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations1, nullptr));

        scoped_ptr<Animation> animation(
            Animation::Create(curve1.Pass(), 2, 2, Animation::TRANSFORM));
        controller_impl->AddAnimation(animation.Pass());

        // Translations don't affect scale.
        EXPECT_FALSE(controller_impl->HasAnimationThatAffectsScale());

        scoped_ptr<KeyframedTransformAnimationCurve> curve2(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations2;
        curve2->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations2, nullptr));
        operations2.AppendScale(2.0, 3.0, 4.0);
        curve2->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations2, nullptr));

        animation = Animation::Create(curve2.Pass(), 3, 3, Animation::TRANSFORM);
        controller_impl->AddAnimation(animation.Pass());

        EXPECT_TRUE(controller_impl->HasAnimationThatAffectsScale());

        controller_impl->GetAnimationById(3)
            ->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));

        // Only unfinished animations should be considered by
        // HasAnimationThatAffectsScale.
        EXPECT_FALSE(controller_impl->HasAnimationThatAffectsScale());
    }

    TEST(LayerAnimationControllerTest, HasOnlyTranslationTransforms)
    {
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));

        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::PENDING));

        controller_impl->AddAnimation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));

        // Opacity animations aren't non-translation transforms.
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::PENDING));

        scoped_ptr<KeyframedTransformAnimationCurve> curve1(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations1;
        curve1->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations1, nullptr));
        operations1.AppendTranslate(10.0, 15.0, 0.0);
        curve1->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations1, nullptr));

        scoped_ptr<Animation> animation(
            Animation::Create(curve1.Pass(), 2, 2, Animation::TRANSFORM));
        controller_impl->AddAnimation(animation.Pass());

        // The only transform animation we've added is a translation.
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::PENDING));

        scoped_ptr<KeyframedTransformAnimationCurve> curve2(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations2;
        curve2->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations2, nullptr));
        operations2.AppendScale(2.0, 3.0, 4.0);
        curve2->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations2, nullptr));

        animation = Animation::Create(curve2.Pass(), 3, 3, Animation::TRANSFORM);
        animation->set_affects_active_observers(false);
        controller_impl->AddAnimation(animation.Pass());

        // A scale animation is not a translation.
        EXPECT_FALSE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::PENDING));
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::ACTIVE));

        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::ACTIVE));

        controller_impl->GetAnimationById(3)->set_affects_pending_observers(false);
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::ACTIVE));

        controller_impl->GetAnimationById(3)
            ->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));

        // Only unfinished animations should be considered by
        // HasOnlyTranslationTransforms.
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::PENDING));
        EXPECT_TRUE(controller_impl->HasOnlyTranslationTransforms(
            LayerAnimationController::ObserverType::ACTIVE));
    }

    TEST(LayerAnimationControllerTest, AnimationStartScale)
    {
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        scoped_ptr<KeyframedTransformAnimationCurve> curve1(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations1;
        operations1.AppendScale(2.0, 3.0, 4.0);
        curve1->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations1, nullptr));
        TransformOperations operations2;
        curve1->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations2, nullptr));
        scoped_ptr<Animation> animation(
            Animation::Create(curve1.Pass(), 1, 1, Animation::TRANSFORM));
        animation->set_affects_active_observers(false);
        controller_impl->AddAnimation(animation.Pass());

        float start_scale = 0.f;
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::PENDING, &start_scale));
        EXPECT_EQ(4.f, start_scale);
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::ACTIVE, &start_scale));
        EXPECT_EQ(0.f, start_scale);

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::PENDING, &start_scale));
        EXPECT_EQ(4.f, start_scale);
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::ACTIVE, &start_scale));
        EXPECT_EQ(4.f, start_scale);

        scoped_ptr<KeyframedTransformAnimationCurve> curve2(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations3;
        curve2->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations3, nullptr));
        operations3.AppendScale(6.0, 5.0, 4.0);
        curve2->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations3, nullptr));

        controller_impl->RemoveAnimation(1);
        animation = Animation::Create(curve2.Pass(), 2, 2, Animation::TRANSFORM);

        // Reverse Direction
        animation->set_direction(Animation::DIRECTION_REVERSE);
        animation->set_affects_active_observers(false);
        controller_impl->AddAnimation(animation.Pass());

        scoped_ptr<KeyframedTransformAnimationCurve> curve3(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations4;
        operations4.AppendScale(5.0, 3.0, 1.0);
        curve3->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations4, nullptr));
        TransformOperations operations5;
        curve3->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations5, nullptr));

        animation = Animation::Create(curve3.Pass(), 3, 3, Animation::TRANSFORM);
        animation->set_affects_active_observers(false);
        controller_impl->AddAnimation(animation.Pass());

        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::PENDING, &start_scale));
        EXPECT_EQ(6.f, start_scale);
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::ACTIVE, &start_scale));
        EXPECT_EQ(0.f, start_scale);

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::PENDING, &start_scale));
        EXPECT_EQ(6.f, start_scale);
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::ACTIVE, &start_scale));
        EXPECT_EQ(6.f, start_scale);

        controller_impl->GetAnimationById(2)
            ->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));

        // Only unfinished animations should be considered by
        // AnimationStartScale.
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::PENDING, &start_scale));
        EXPECT_EQ(5.f, start_scale);
        EXPECT_TRUE(controller_impl->AnimationStartScale(
            LayerAnimationController::ObserverType::ACTIVE, &start_scale));
        EXPECT_EQ(5.f, start_scale);
    }

    TEST(LayerAnimationControllerTest, MaximumTargetScale)
    {
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));

        float max_scale = 0.f;
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(0.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(0.f, max_scale);

        scoped_ptr<KeyframedTransformAnimationCurve> curve1(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations1;
        curve1->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations1, nullptr));
        operations1.AppendScale(2.0, 3.0, 4.0);
        curve1->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations1, nullptr));

        scoped_ptr<Animation> animation(
            Animation::Create(curve1.Pass(), 1, 1, Animation::TRANSFORM));
        animation->set_affects_active_observers(false);
        controller_impl->AddAnimation(animation.Pass());

        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(4.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(0.f, max_scale);

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(4.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(4.f, max_scale);

        scoped_ptr<KeyframedTransformAnimationCurve> curve2(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations2;
        curve2->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations2, nullptr));
        operations2.AppendScale(6.0, 5.0, 4.0);
        curve2->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations2, nullptr));

        animation = Animation::Create(curve2.Pass(), 2, 2, Animation::TRANSFORM);
        animation->set_affects_active_observers(false);
        controller_impl->AddAnimation(animation.Pass());

        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(6.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(4.f, max_scale);

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(6.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(6.f, max_scale);

        scoped_ptr<KeyframedTransformAnimationCurve> curve3(
            KeyframedTransformAnimationCurve::Create());

        TransformOperations operations3;
        curve3->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations3, nullptr));
        operations3.AppendPerspective(6.0);
        curve3->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations3, nullptr));

        animation = Animation::Create(curve3.Pass(), 3, 3, Animation::TRANSFORM);
        animation->set_affects_active_observers(false);
        controller_impl->AddAnimation(animation.Pass());

        EXPECT_FALSE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(6.f, max_scale);

        controller_impl->ActivateAnimations();
        EXPECT_FALSE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_FALSE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));

        controller_impl->GetAnimationById(3)
            ->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));
        controller_impl->GetAnimationById(2)
            ->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));

        // Only unfinished animations should be considered by
        // MaximumTargetScale.
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(4.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(4.f, max_scale);
    }

    TEST(LayerAnimationControllerTest, MaximumTargetScaleWithDirection)
    {
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));

        scoped_ptr<KeyframedTransformAnimationCurve> curve1(
            KeyframedTransformAnimationCurve::Create());
        TransformOperations operations1;
        operations1.AppendScale(1.0, 2.0, 3.0);
        curve1->AddKeyframe(
            TransformKeyframe::Create(base::TimeDelta(), operations1, nullptr));
        TransformOperations operations2;
        operations2.AppendScale(4.0, 5.0, 6.0);
        curve1->AddKeyframe(TransformKeyframe::Create(
            base::TimeDelta::FromSecondsD(1.0), operations2, nullptr));

        scoped_ptr<Animation> animation_owned(
            Animation::Create(curve1.Pass(), 1, 1, Animation::TRANSFORM));
        Animation* animation = animation_owned.get();
        controller_impl->AddAnimation(animation_owned.Pass());

        float max_scale = 0.f;

        EXPECT_GT(animation->playback_rate(), 0.0);

        // NORMAL direction with positive playback rate.
        animation->set_direction(Animation::DIRECTION_NORMAL);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(6.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(6.f, max_scale);

        // ALTERNATE direction with positive playback rate.
        animation->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(6.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(6.f, max_scale);

        // REVERSE direction with positive playback rate.
        animation->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(3.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(3.f, max_scale);

        // ALTERNATE reverse direction.
        animation->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(3.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(3.f, max_scale);

        animation->set_playback_rate(-1.0);

        // NORMAL direction with negative playback rate.
        animation->set_direction(Animation::DIRECTION_NORMAL);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(3.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(3.f, max_scale);

        // ALTERNATE direction with negative playback rate.
        animation->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(3.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(3.f, max_scale);

        // REVERSE direction with negative playback rate.
        animation->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(6.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(6.f, max_scale);

        // ALTERNATE reverse direction with negative playback rate.
        animation->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::PENDING, &max_scale));
        EXPECT_EQ(6.f, max_scale);
        EXPECT_TRUE(controller_impl->MaximumTargetScale(
            LayerAnimationController::ObserverType::ACTIVE, &max_scale));
        EXPECT_EQ(6.f, max_scale);
    }

    TEST(LayerAnimationControllerTest, NewlyPushedAnimationWaitsForActivation)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        FakeInactiveLayerAnimationValueObserver pending_dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        controller_impl->AddValueObserver(&pending_dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        EXPECT_FALSE(controller->needs_to_start_animations_for_testing());
        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0.5f, 1.f, false);
        EXPECT_TRUE(controller->needs_to_start_animations_for_testing());

        EXPECT_FALSE(controller_impl->needs_to_start_animations_for_testing());
        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_TRUE(controller_impl->needs_to_start_animations_for_testing());

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_pending_observers());
        EXPECT_FALSE(controller_impl->GetAnimationById(animation_id)
                         ->affects_active_observers());

        controller_impl->Animate(kInitialTickTime);
        EXPECT_FALSE(controller_impl->needs_to_start_animations_for_testing());
        controller_impl->UpdateState(true, events.get());

        // Since the animation hasn't been activated, it should still be STARTING
        // rather than RUNNING.
        EXPECT_EQ(Animation::STARTING,
            controller_impl->GetAnimationById(animation_id)->run_state());

        // Since the animation hasn't been activated, only the pending observer
        // should have been ticked.
        EXPECT_EQ(0.5f, pending_dummy_impl.opacity());
        EXPECT_EQ(0.f, dummy_impl.opacity());

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_pending_observers());
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_active_observers());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());

        // Since the animation has been activated, it should have reached the
        // RUNNING state and the active observer should start to get ticked.
        EXPECT_EQ(Animation::RUNNING,
            controller_impl->GetAnimationById(animation_id)->run_state());
        EXPECT_EQ(0.5f, pending_dummy_impl.opacity());
        EXPECT_EQ(0.5f, dummy_impl.opacity());
    }

    TEST(LayerAnimationControllerTest, ActivationBetweenAnimateAndUpdateState)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        FakeInactiveLayerAnimationValueObserver pending_dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        controller_impl->AddValueObserver(&pending_dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0.5f, 1.f, true);

        controller->PushAnimationUpdatesTo(controller_impl.get());

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id));
        EXPECT_EQ(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            controller_impl->GetAnimationById(animation_id)->run_state());
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_pending_observers());
        EXPECT_FALSE(controller_impl->GetAnimationById(animation_id)
                         ->affects_active_observers());

        controller_impl->Animate(kInitialTickTime);

        // Since the animation hasn't been activated, only the pending observer
        // should have been ticked.
        EXPECT_EQ(0.5f, pending_dummy_impl.opacity());
        EXPECT_EQ(0.f, dummy_impl.opacity());

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_pending_observers());
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_active_observers());

        controller_impl->UpdateState(true, events.get());

        // Since the animation has been activated, it should have reached the
        // RUNNING state.
        EXPECT_EQ(Animation::RUNNING,
            controller_impl->GetAnimationById(animation_id)->run_state());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));

        // Both observers should have been ticked.
        EXPECT_EQ(0.75f, pending_dummy_impl.opacity());
        EXPECT_EQ(0.75f, dummy_impl.opacity());
    }

    TEST(LayerAnimationControllerTest,
        ObserverNotifiedWhenTransformIsPotentiallyAnimatingChanges)
    {
        AnimationEventsVector events;
        FakeLayerAnimationValueObserver active_dummy_impl;
        FakeInactiveLayerAnimationValueObserver pending_dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&active_dummy_impl);
        controller_impl->AddValueObserver(&pending_dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        EXPECT_FALSE(dummy.transform_is_animating());
        EXPECT_FALSE(pending_dummy_impl.transform_is_animating());
        EXPECT_FALSE(active_dummy_impl.transform_is_animating());

        // Case 1: An animation that's allowed to run until its finish point.
        AddAnimatedTransformToController(controller.get(), 1.0, 1, 1);
        EXPECT_TRUE(dummy.transform_is_animating());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_TRUE(pending_dummy_impl.transform_is_animating());
        EXPECT_FALSE(active_dummy_impl.transform_is_animating());

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(pending_dummy_impl.transform_is_animating());
        EXPECT_TRUE(active_dummy_impl.transform_is_animating());

        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, &events);

        controller->NotifyAnimationStarted(events[0]);
        events.clear();

        // Finish the animation.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller->UpdateState(true, nullptr);
        EXPECT_FALSE(dummy.transform_is_animating());

        controller->PushAnimationUpdatesTo(controller_impl.get());

        // controller_impl hasn't yet ticked at/past the end of the animation.
        EXPECT_TRUE(pending_dummy_impl.transform_is_animating());
        EXPECT_TRUE(active_dummy_impl.transform_is_animating());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, &events);
        EXPECT_FALSE(pending_dummy_impl.transform_is_animating());
        EXPECT_FALSE(active_dummy_impl.transform_is_animating());

        controller->NotifyAnimationFinished(events[0]);
        events.clear();

        // Case 2: An animation that's removed before it finishes.
        int animation_id = AddAnimatedTransformToController(controller.get(), 10.0, 2, 2);
        EXPECT_TRUE(dummy.transform_is_animating());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_TRUE(pending_dummy_impl.transform_is_animating());
        EXPECT_FALSE(active_dummy_impl.transform_is_animating());

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(pending_dummy_impl.transform_is_animating());
        EXPECT_TRUE(active_dummy_impl.transform_is_animating());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller_impl->UpdateState(true, &events);

        controller->NotifyAnimationStarted(events[0]);
        events.clear();

        controller->RemoveAnimation(animation_id);
        EXPECT_FALSE(dummy.transform_is_animating());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_FALSE(pending_dummy_impl.transform_is_animating());
        EXPECT_TRUE(active_dummy_impl.transform_is_animating());

        controller_impl->ActivateAnimations();
        EXPECT_FALSE(pending_dummy_impl.transform_is_animating());
        EXPECT_FALSE(active_dummy_impl.transform_is_animating());

        // Case 3: An animation that's aborted before it finishes.
        animation_id = AddAnimatedTransformToController(controller.get(), 10.0, 3, 3);
        EXPECT_TRUE(dummy.transform_is_animating());

        controller->PushAnimationUpdatesTo(controller_impl.get());
        EXPECT_TRUE(pending_dummy_impl.transform_is_animating());
        EXPECT_FALSE(active_dummy_impl.transform_is_animating());

        controller_impl->ActivateAnimations();
        EXPECT_TRUE(pending_dummy_impl.transform_is_animating());
        EXPECT_TRUE(active_dummy_impl.transform_is_animating());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(3000));
        controller_impl->UpdateState(true, &events);

        controller->NotifyAnimationStarted(events[0]);
        events.clear();

        controller_impl->AbortAnimations(Animation::TRANSFORM);
        EXPECT_FALSE(pending_dummy_impl.transform_is_animating());
        EXPECT_FALSE(active_dummy_impl.transform_is_animating());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(4000));
        controller_impl->UpdateState(true, &events);

        controller->NotifyAnimationAborted(events[0]);
        EXPECT_FALSE(dummy.transform_is_animating());
    }

    TEST(LayerAnimationControllerTest, ClippedOpacityValues)
    {
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        AddOpacityTransitionToController(controller.get(), 1, 1.f, 2.f, true);

        controller->Animate(kInitialTickTime);
        EXPECT_EQ(1.f, dummy.opacity());

        // Opacity values are clipped [0,1]
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        EXPECT_EQ(1.f, dummy.opacity());
    }

    TEST(LayerAnimationControllerTest, ClippedNegativeOpacityValues)
    {
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        AddOpacityTransitionToController(controller.get(), 1, 0.f, -2.f, true);

        controller->Animate(kInitialTickTime);
        EXPECT_EQ(0.f, dummy.opacity());

        // Opacity values are clipped [0,1]
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        EXPECT_EQ(0.f, dummy.opacity());
    }

    TEST(LayerAnimationControllerTest, PushedDeletedAnimationWaitsForActivation)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        FakeInactiveLayerAnimationValueObserver pending_dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        controller_impl->AddValueObserver(&pending_dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        int animation_id = AddOpacityTransitionToController(controller.get(), 1, 0.5f, 1.f, true);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());
        EXPECT_EQ(Animation::RUNNING,
            controller_impl->GetAnimationById(animation_id)->run_state());
        EXPECT_EQ(0.5f, pending_dummy_impl.opacity());
        EXPECT_EQ(0.5f, dummy_impl.opacity());

        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_pending_observers());
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_active_observers());

        // Delete the animation on the main-thread controller.
        controller->RemoveAnimation(
            controller->GetAnimation(Animation::OPACITY)->id());
        controller->PushAnimationUpdatesTo(controller_impl.get());

        // The animation should no longer affect pending observers.
        EXPECT_FALSE(controller_impl->GetAnimationById(animation_id)
                         ->affects_pending_observers());
        EXPECT_TRUE(controller_impl->GetAnimationById(animation_id)
                        ->affects_active_observers());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller_impl->UpdateState(true, events.get());

        // Only the active observer should have been ticked.
        EXPECT_EQ(0.5f, pending_dummy_impl.opacity());
        EXPECT_EQ(0.75f, dummy_impl.opacity());

        controller_impl->ActivateAnimations();

        // Activation should cause the animation to be deleted.
        EXPECT_FALSE(controller_impl->has_any_animation());
    }

    // Tests that an animation that affects only active observers won't block
    // an animation that affects only pending observers from starting.
    TEST(LayerAnimationControllerTest, StartAnimationsAffectingDifferentObservers)
    {
        scoped_ptr<AnimationEventsVector> events(
            make_scoped_ptr(new AnimationEventsVector));
        FakeLayerAnimationValueObserver dummy_impl;
        FakeInactiveLayerAnimationValueObserver pending_dummy_impl;
        scoped_refptr<LayerAnimationController> controller_impl(
            LayerAnimationController::Create(0));
        controller_impl->AddValueObserver(&dummy_impl);
        controller_impl->AddValueObserver(&pending_dummy_impl);
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        int first_animation_id = AddOpacityTransitionToController(controller.get(), 1, 0.f, 1.f, true);

        controller->PushAnimationUpdatesTo(controller_impl.get());
        controller_impl->ActivateAnimations();
        controller_impl->Animate(kInitialTickTime);
        controller_impl->UpdateState(true, events.get());

        // Remove the first animation from the main-thread controller, and add a
        // new animation affecting the same property.
        controller->RemoveAnimation(
            controller->GetAnimation(Animation::OPACITY)->id());
        int second_animation_id = AddOpacityTransitionToController(controller.get(), 1, 1.f, 0.5f, true);
        controller->PushAnimationUpdatesTo(controller_impl.get());

        // The original animation should only affect active observers, and the new
        // animation should only affect pending observers.
        EXPECT_FALSE(controller_impl->GetAnimationById(first_animation_id)
                         ->affects_pending_observers());
        EXPECT_TRUE(controller_impl->GetAnimationById(first_animation_id)
                        ->affects_active_observers());
        EXPECT_TRUE(controller_impl->GetAnimationById(second_animation_id)
                        ->affects_pending_observers());
        EXPECT_FALSE(controller_impl->GetAnimationById(second_animation_id)
                         ->affects_active_observers());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(500));
        controller_impl->UpdateState(true, events.get());

        // The original animation should still be running, and the new animation
        // should be starting.
        EXPECT_EQ(Animation::RUNNING,
            controller_impl->GetAnimationById(first_animation_id)->run_state());
        EXPECT_EQ(
            Animation::STARTING,
            controller_impl->GetAnimationById(second_animation_id)->run_state());

        // The active observer should have been ticked by the original animation,
        // and the pending observer should have been ticked by the new animation.
        EXPECT_EQ(1.f, pending_dummy_impl.opacity());
        EXPECT_EQ(0.5f, dummy_impl.opacity());

        controller_impl->ActivateAnimations();

        // The original animation should have been deleted, and the new animation
        // should now affect both observers.
        EXPECT_FALSE(controller_impl->GetAnimationById(first_animation_id));
        EXPECT_TRUE(controller_impl->GetAnimationById(second_animation_id)
                        ->affects_pending_observers());
        EXPECT_TRUE(controller_impl->GetAnimationById(second_animation_id)
                        ->affects_active_observers());

        controller_impl->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1000));
        controller_impl->UpdateState(true, events.get());

        // The new animation should be running, and the active observer should have
        // been ticked at the new animation's starting point.
        EXPECT_EQ(
            Animation::RUNNING,
            controller_impl->GetAnimationById(second_animation_id)->run_state());
        EXPECT_EQ(1.f, pending_dummy_impl.opacity());
        EXPECT_EQ(1.f, dummy_impl.opacity());
    }

    TEST(LayerAnimationControllerTest, TestIsCurrentlyAnimatingProperty)
    {
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        // Create an animation that initially affects only pending observers.
        scoped_ptr<Animation> animation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        animation->set_affects_active_observers(false);

        controller->AddAnimation(animation.Pass());
        controller->Animate(kInitialTickTime);
        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(controller->HasActiveAnimation());

        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::ACTIVE));

        controller->ActivateAnimations();

        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::ACTIVE));

        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(10));
        controller->UpdateState(true, nullptr);

        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::ACTIVE));

        EXPECT_EQ(0.f, dummy.opacity());

        // Tick past the end of the animation.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(1100));
        controller->UpdateState(true, nullptr);

        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::ACTIVE));

        EXPECT_EQ(1.f, dummy.opacity());
    }

    TEST(LayerAnimationControllerTest, TestIsAnimatingPropertyTimeOffsetFillMode)
    {
        FakeLayerAnimationValueObserver dummy;
        scoped_refptr<LayerAnimationController> controller(
            LayerAnimationController::Create(0));
        controller->AddValueObserver(&dummy);

        // Create an animation that initially affects only pending observers, and has
        // a start delay of 2 seconds.
        scoped_ptr<Animation> animation(CreateAnimation(
            scoped_ptr<AnimationCurve>(new FakeFloatTransition(1.0, 0.f, 1.f)).Pass(),
            1, Animation::OPACITY));
        animation->set_fill_mode(Animation::FILL_MODE_NONE);
        animation->set_time_offset(TimeDelta::FromMilliseconds(-2000));
        animation->set_affects_active_observers(false);

        controller->AddAnimation(animation.Pass());

        controller->Animate(kInitialTickTime);

        // Since the animation has a start delay, the observers it affects have a
        // potentially running transform animation but aren't currently animating
        // transform.
        EXPECT_TRUE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_FALSE(controller->IsPotentiallyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsPotentiallyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::ACTIVE));

        controller->ActivateAnimations();

        EXPECT_TRUE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_TRUE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_TRUE(controller->HasActiveAnimation());
        EXPECT_FALSE(controller->IsPotentiallyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsPotentiallyAnimatingProperty(
            Animation::FILTER, LayerAnimationController::ObserverType::ACTIVE));

        controller->UpdateState(true, nullptr);

        // Tick past the start delay.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(2000));
        controller->UpdateState(true, nullptr);
        EXPECT_TRUE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_TRUE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_TRUE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));

        // After the animaton finishes, the observers it affects have neither a
        // potentially running transform animation nor a currently running transform
        // animation.
        controller->Animate(kInitialTickTime + TimeDelta::FromMilliseconds(4000));
        controller->UpdateState(true, nullptr);
        EXPECT_FALSE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsPotentiallyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::PENDING));
        EXPECT_FALSE(controller->IsCurrentlyAnimatingProperty(
            Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE));
    }

} // namespace
} // namespace cc
