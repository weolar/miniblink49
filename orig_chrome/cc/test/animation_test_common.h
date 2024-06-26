// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_ANIMATION_TEST_COMMON_H_
#define CC_TEST_ANIMATION_TEST_COMMON_H_

#include "cc/animation/animation.h"
#include "cc/animation/animation_curve.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/animation/layer_animation_value_observer.h"
#include "cc/animation/layer_animation_value_provider.h"
#include "cc/output/filter_operations.h"
#include "cc/test/geometry_test_utils.h"

namespace cc {
class AnimationPlayer;
class LayerImpl;
class Layer;
}

namespace cc {

class FakeFloatAnimationCurve : public FloatAnimationCurve {
public:
    FakeFloatAnimationCurve();
    explicit FakeFloatAnimationCurve(double duration);
    ~FakeFloatAnimationCurve() override;

    base::TimeDelta Duration() const override;
    float GetValue(base::TimeDelta now) const override;
    scoped_ptr<AnimationCurve> Clone() const override;

private:
    base::TimeDelta duration_;
};

class FakeTransformTransition : public TransformAnimationCurve {
public:
    explicit FakeTransformTransition(double duration);
    ~FakeTransformTransition() override;

    base::TimeDelta Duration() const override;
    gfx::Transform GetValue(base::TimeDelta time) const override;
    bool AnimatedBoundsForBox(const gfx::BoxF& box,
        gfx::BoxF* bounds) const override;
    bool AffectsScale() const override;
    bool IsTranslation() const override;
    bool PreservesAxisAlignment() const override;
    bool AnimationStartScale(bool forward_direction,
        float* start_scale) const override;
    bool MaximumTargetScale(bool forward_direction,
        float* max_scale) const override;

    scoped_ptr<AnimationCurve> Clone() const override;

private:
    base::TimeDelta duration_;
};

class FakeFloatTransition : public FloatAnimationCurve {
public:
    FakeFloatTransition(double duration, float from, float to);
    ~FakeFloatTransition() override;

    base::TimeDelta Duration() const override;
    float GetValue(base::TimeDelta time) const override;

    scoped_ptr<AnimationCurve> Clone() const override;

private:
    base::TimeDelta duration_;
    float from_;
    float to_;
};

class FakeLayerAnimationValueObserver : public LayerAnimationValueObserver {
public:
    FakeLayerAnimationValueObserver();
    ~FakeLayerAnimationValueObserver() override;

    // LayerAnimationValueObserver implementation
    void OnFilterAnimated(const FilterOperations& filters) override;
    void OnOpacityAnimated(float opacity) override;
    void OnTransformAnimated(const gfx::Transform& transform) override;
    void OnScrollOffsetAnimated(const gfx::ScrollOffset& scroll_offset) override;
    void OnAnimationWaitingForDeletion() override;
    void OnTransformIsPotentiallyAnimatingChanged(bool is_animating) override;
    bool IsActive() const override;

    const FilterOperations& filters() const { return filters_; }
    float opacity() const { return opacity_; }
    const gfx::Transform& transform() const { return transform_; }
    gfx::ScrollOffset scroll_offset() { return scroll_offset_; }

    bool animation_waiting_for_deletion()
    {
        return animation_waiting_for_deletion_;
    }

    bool transform_is_animating() { return transform_is_animating_; }

private:
    FilterOperations filters_;
    float opacity_;
    gfx::Transform transform_;
    gfx::ScrollOffset scroll_offset_;
    bool animation_waiting_for_deletion_;
    bool transform_is_animating_;
};

class FakeInactiveLayerAnimationValueObserver
    : public FakeLayerAnimationValueObserver {
public:
    bool IsActive() const override;
};

class FakeLayerAnimationValueProvider : public LayerAnimationValueProvider {
public:
    gfx::ScrollOffset ScrollOffsetForAnimation() const override;

    void set_scroll_offset(const gfx::ScrollOffset& scroll_offset)
    {
        scroll_offset_ = scroll_offset;
    }

private:
    gfx::ScrollOffset scroll_offset_;
};

int AddOpacityTransitionToController(LayerAnimationController* controller,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function);

int AddAnimatedTransformToController(LayerAnimationController* controller,
    double duration,
    int delta_x,
    int delta_y);

int AddAnimatedFilterToController(LayerAnimationController* controller,
    double duration,
    float start_brightness,
    float end_brightness);

int AddOpacityTransitionToLayer(Layer* layer,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function);

int AddOpacityTransitionToLayer(LayerImpl* layer,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function);

int AddAnimatedTransformToLayer(Layer* layer,
    double duration,
    int delta_x,
    int delta_y);

int AddAnimatedTransformToLayer(LayerImpl* layer,
    double duration,
    int delta_x,
    int delta_y);

int AddAnimatedTransformToLayer(Layer* layer,
    double duration,
    TransformOperations start_operations,
    TransformOperations operations);

int AddAnimatedTransformToLayer(LayerImpl* layer,
    double duration,
    TransformOperations start_operations,
    TransformOperations operations);

int AddAnimatedFilterToLayer(Layer* layer,
    double duration,
    float start_brightness,
    float end_brightness);

int AddAnimatedFilterToLayer(LayerImpl* layer,
    double duration,
    float start_brightness,
    float end_brightness);

int AddAnimatedTransformToPlayer(AnimationPlayer* player,
    double duration,
    int delta_x,
    int delta_y);

int AddOpacityTransitionToPlayer(AnimationPlayer* player,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function);

int AddAnimatedFilterToPlayer(AnimationPlayer* player,
    double duration,
    float start_brightness,
    float end_brightness);

int AddOpacityStepsToController(LayerAnimationController* target,
    double duration,
    float start_opacity,
    float end_opacity,
    int num_steps);

} // namespace cc

#endif // CC_TEST_ANIMATION_TEST_COMMON_H_
