// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/input/top_controls_manager.h"

#include <algorithm>

#include "base/logging.h"
#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/timing_function.h"
#include "cc/input/top_controls_manager_client.h"
#include "cc/output/begin_frame_args.h"
#include "cc/trees/layer_tree_impl.h"
#include "ui/gfx/geometry/vector2d_f.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {
    // These constants were chosen empirically for their visually pleasant behavior.
    // Contact tedchoc@chromium.org for questions about changing these values.
    const int64 kShowHideMaxDurationMs = 200;
}

// static
scoped_ptr<TopControlsManager> TopControlsManager::Create(
    TopControlsManagerClient* client,
    float top_controls_show_threshold,
    float top_controls_hide_threshold)
{
    return make_scoped_ptr(new TopControlsManager(client,
        top_controls_show_threshold,
        top_controls_hide_threshold));
}

TopControlsManager::TopControlsManager(TopControlsManagerClient* client,
    float top_controls_show_threshold,
    float top_controls_hide_threshold)
    : client_(client)
    , animation_direction_(NO_ANIMATION)
    , permitted_state_(BOTH)
    , accumulated_scroll_delta_(0.f)
    , baseline_content_offset_(0.f)
    , top_controls_show_threshold_(top_controls_hide_threshold)
    , top_controls_hide_threshold_(top_controls_show_threshold)
    , pinch_gesture_active_(false)
{
    CHECK(client_);
}

TopControlsManager::~TopControlsManager()
{
}

float TopControlsManager::ControlsTopOffset() const
{
    return ContentTopOffset() - TopControlsHeight();
}

float TopControlsManager::ContentTopOffset() const
{
    return TopControlsShownRatio() * TopControlsHeight();
}

float TopControlsManager::TopControlsShownRatio() const
{
    return client_->CurrentTopControlsShownRatio();
}

float TopControlsManager::TopControlsHeight() const
{
    return client_->TopControlsHeight();
}

void TopControlsManager::UpdateTopControlsState(TopControlsState constraints,
    TopControlsState current,
    bool animate)
{
    DCHECK(!(constraints == SHOWN && current == HIDDEN));
    DCHECK(!(constraints == HIDDEN && current == SHOWN));

    permitted_state_ = constraints;

    // Don't do anything if it doesn't matter which state the controls are in.
    if (constraints == BOTH && current == BOTH)
        return;

    // Don't do anything if there is no change in offset.
    float final_shown_ratio = 1.f;
    if (constraints == HIDDEN || current == HIDDEN)
        final_shown_ratio = 0.f;
    if (final_shown_ratio == TopControlsShownRatio())
        return;

    if (animate) {
        SetupAnimation(final_shown_ratio ? SHOWING_CONTROLS : HIDING_CONTROLS);
    } else {
        ResetAnimations();
        client_->SetCurrentTopControlsShownRatio(final_shown_ratio);
    }
}

void TopControlsManager::ScrollBegin()
{
    DCHECK(!pinch_gesture_active_);
    ResetAnimations();
    ResetBaseline();
}

gfx::Vector2dF TopControlsManager::ScrollBy(
    const gfx::Vector2dF& pending_delta)
{
    if (!TopControlsHeight())
        return pending_delta;

    if (pinch_gesture_active_)
        return pending_delta;

    if (permitted_state_ == SHOWN && pending_delta.y() > 0)
        return pending_delta;
    else if (permitted_state_ == HIDDEN && pending_delta.y() < 0)
        return pending_delta;

    accumulated_scroll_delta_ += pending_delta.y();

    float old_offset = ContentTopOffset();
    client_->SetCurrentTopControlsShownRatio(
        (baseline_content_offset_ - accumulated_scroll_delta_) / TopControlsHeight());

    // If the controls are fully visible, treat the current position as the
    // new baseline even if the gesture didn't end.
    if (TopControlsShownRatio() == 1.f)
        ResetBaseline();

    ResetAnimations();

    gfx::Vector2dF applied_delta(0.f, old_offset - ContentTopOffset());
    return pending_delta - applied_delta;
}

void TopControlsManager::ScrollEnd()
{
    DCHECK(!pinch_gesture_active_);
    StartAnimationIfNecessary();
}

void TopControlsManager::PinchBegin()
{
    DCHECK(!pinch_gesture_active_);
    pinch_gesture_active_ = true;
    StartAnimationIfNecessary();
}

void TopControlsManager::PinchEnd()
{
    DCHECK(pinch_gesture_active_);
    // Pinch{Begin,End} will always occur within the scope of Scroll{Begin,End},
    // so return to a state expected by the remaining scroll sequence.
    pinch_gesture_active_ = false;
    ScrollBegin();
}

void TopControlsManager::MainThreadHasStoppedFlinging()
{
    StartAnimationIfNecessary();
}

gfx::Vector2dF TopControlsManager::Animate(base::TimeTicks monotonic_time)
{
    if (!top_controls_animation_ || !client_->HaveRootScrollLayer())
        return gfx::Vector2dF();

    base::TimeDelta time = monotonic_time - base::TimeTicks();

    float old_offset = ContentTopOffset();
    client_->SetCurrentTopControlsShownRatio(
        top_controls_animation_->GetValue(time));

    if (IsAnimationCompleteAtTime(monotonic_time))
        ResetAnimations();

    gfx::Vector2dF scroll_delta(0.f, ContentTopOffset() - old_offset);
    return scroll_delta;
}

void TopControlsManager::ResetAnimations()
{
    top_controls_animation_ = nullptr;
    animation_direction_ = NO_ANIMATION;
}

void TopControlsManager::SetupAnimation(AnimationDirection direction)
{
    DCHECK_NE(NO_ANIMATION, direction);
    DCHECK_IMPLIES(direction == HIDING_CONTROLS, TopControlsShownRatio() > 0.f);
    DCHECK_IMPLIES(direction == SHOWING_CONTROLS, TopControlsShownRatio() < 1.f);

    if (top_controls_animation_ && animation_direction_ == direction)
        return;

    if (!TopControlsHeight()) {
        client_->SetCurrentTopControlsShownRatio(
            direction == HIDING_CONTROLS ? 0.f : 1.f);
        return;
    }

    top_controls_animation_ = KeyframedFloatAnimationCurve::Create();
    base::TimeDelta start_time = base::TimeTicks::Now() - base::TimeTicks();
    top_controls_animation_->AddKeyframe(
        FloatKeyframe::Create(start_time, TopControlsShownRatio(), nullptr));
    float max_ending_ratio = (direction == SHOWING_CONTROLS ? 1 : -1);
    top_controls_animation_->AddKeyframe(FloatKeyframe::Create(
        start_time + base::TimeDelta::FromMilliseconds(kShowHideMaxDurationMs),
        TopControlsShownRatio() + max_ending_ratio,
        EaseTimingFunction::Create()));
    animation_direction_ = direction;
    client_->DidChangeTopControlsPosition();
}

void TopControlsManager::StartAnimationIfNecessary()
{
    if (TopControlsShownRatio() == 0.f || TopControlsShownRatio() == 1.f)
        return;

    if (TopControlsShownRatio() >= 1.f - top_controls_hide_threshold_) {
        // If we're showing so much that the hide threshold won't trigger, show.
        SetupAnimation(SHOWING_CONTROLS);
    } else if (TopControlsShownRatio() <= top_controls_show_threshold_) {
        // If we're showing so little that the show threshold won't trigger, hide.
        SetupAnimation(HIDING_CONTROLS);
    } else {
        // If we could be either showing or hiding, we determine which one to
        // do based on whether or not the total scroll delta was moving up or
        // down.
        SetupAnimation(accumulated_scroll_delta_ <= 0.f ? SHOWING_CONTROLS
                                                        : HIDING_CONTROLS);
    }
}

bool TopControlsManager::IsAnimationCompleteAtTime(base::TimeTicks time)
{
    if (!top_controls_animation_)
        return true;

    base::TimeDelta animation_time = time - base::TimeTicks();
    float new_ratio = top_controls_animation_->GetValue(animation_time);

    if ((animation_direction_ == SHOWING_CONTROLS && new_ratio >= 1.f) || (animation_direction_ == HIDING_CONTROLS && new_ratio <= 0.f)) {
        return true;
    }
    return false;
}

void TopControlsManager::ResetBaseline()
{
    accumulated_scroll_delta_ = 0.f;
    baseline_content_offset_ = ContentTopOffset();
}

} // namespace cc
