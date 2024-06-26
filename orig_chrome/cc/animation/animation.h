// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_ANIMATION_H_
#define CC_ANIMATION_ANIMATION_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"

namespace cc {

class AnimationCurve;

// An Animation contains all the state required to play an AnimationCurve.
// Specifically, the affected property, the run state (paused, finished, etc.),
// loop count, last pause time, and the total time spent paused.
class CC_EXPORT Animation {
public:
    // Animations begin in the 'WAITING_FOR_TARGET_AVAILABILITY' state. An
    // Animation waiting for target availibility will run as soon as its target
    // property is free (and all the animations animating with it are also able to
    // run). When this time arrives, the controller will move the animation into
    // the STARTING state, and then into the RUNNING state. RUNNING animations may
    // toggle between RUNNING and PAUSED, and may be stopped by moving into either
    // the ABORTED or FINISHED states. A FINISHED animation was allowed to run to
    // completion, but an ABORTED animation was not.
    enum RunState {
        WAITING_FOR_TARGET_AVAILABILITY = 0,
        WAITING_FOR_DELETION,
        STARTING,
        RUNNING,
        PAUSED,
        FINISHED,
        ABORTED,
        // This sentinel must be last.
        LAST_RUN_STATE = ABORTED
    };

    enum TargetProperty {
        TRANSFORM = 0,
        OPACITY,
        FILTER,
        SCROLL_OFFSET,
        BACKGROUND_COLOR,
        // This sentinel must be last.
        LAST_TARGET_PROPERTY = BACKGROUND_COLOR
    };

    enum Direction {
        DIRECTION_NORMAL,
        DIRECTION_REVERSE,
        DIRECTION_ALTERNATE,
        DIRECTION_ALTERNATE_REVERSE
    };

    enum FillMode {
        FILL_MODE_NONE,
        FILL_MODE_FORWARDS,
        FILL_MODE_BACKWARDS,
        FILL_MODE_BOTH
    };

    static scoped_ptr<Animation> Create(scoped_ptr<AnimationCurve> curve,
        int animation_id,
        int group_id,
        TargetProperty target_property);

    virtual ~Animation();

    int id() const { return id_; }
    int group() const { return group_; }
    TargetProperty target_property() const { return target_property_; }

    RunState run_state() const { return run_state_; }
    void SetRunState(RunState run_state, base::TimeTicks monotonic_time);

    // This is the number of times that the animation will play. If this
    // value is zero the animation will not play. If it is negative, then
    // the animation will loop indefinitely.
    double iterations() const { return iterations_; }
    void set_iterations(double n) { iterations_ = n; }

    double iteration_start() const { return iteration_start_; }
    void set_iteration_start(double iteration_start)
    {
        iteration_start_ = iteration_start;
    }

    base::TimeTicks start_time() const { return start_time_; }

    void set_start_time(base::TimeTicks monotonic_time)
    {
        start_time_ = monotonic_time;
    }
    bool has_set_start_time() const { return !start_time_.is_null(); }

    base::TimeDelta time_offset() const { return time_offset_; }
    void set_time_offset(base::TimeDelta monotonic_time)
    {
        time_offset_ = monotonic_time;
    }

    void Suspend(base::TimeTicks monotonic_time);
    void Resume(base::TimeTicks monotonic_time);

    Direction direction() { return direction_; }
    void set_direction(Direction direction) { direction_ = direction; }

    FillMode fill_mode() { return fill_mode_; }
    void set_fill_mode(FillMode fill_mode) { fill_mode_ = fill_mode; }

    double playback_rate() { return playback_rate_; }
    void set_playback_rate(double playback_rate)
    {
        playback_rate_ = playback_rate;
    }

    bool IsFinishedAt(base::TimeTicks monotonic_time) const;
    bool is_finished() const
    {
        return run_state_ == FINISHED || run_state_ == ABORTED || run_state_ == WAITING_FOR_DELETION;
    }

    bool InEffect(base::TimeTicks monotonic_time) const;

    AnimationCurve* curve() { return curve_.get(); }
    const AnimationCurve* curve() const { return curve_.get(); }

    // If this is true, even if the animation is running, it will not be tickable
    // until it is given a start time. This is true for animations running on the
    // main thread.
    bool needs_synchronized_start_time() const
    {
        return needs_synchronized_start_time_;
    }
    void set_needs_synchronized_start_time(bool needs_synchronized_start_time)
    {
        needs_synchronized_start_time_ = needs_synchronized_start_time;
    }

    // This is true for animations running on the main thread when the FINISHED
    // event sent by the corresponding impl animation has been received.
    bool received_finished_event() const
    {
        return received_finished_event_;
    }
    void set_received_finished_event(bool received_finished_event)
    {
        received_finished_event_ = received_finished_event;
    }

    // Takes the given absolute time, and using the start time and the number
    // of iterations, returns the relative time in the current iteration.
    base::TimeDelta TrimTimeToCurrentIteration(
        base::TimeTicks monotonic_time) const;

    scoped_ptr<Animation> CloneAndInitialize(RunState initial_run_state) const;

    void set_is_controlling_instance_for_test(bool is_controlling_instance)
    {
        is_controlling_instance_ = is_controlling_instance;
    }
    bool is_controlling_instance() const { return is_controlling_instance_; }

    void PushPropertiesTo(Animation* other) const;

    void set_is_impl_only(bool is_impl_only) { is_impl_only_ = is_impl_only; }
    bool is_impl_only() const { return is_impl_only_; }

    void set_affects_active_observers(bool affects_active_observers)
    {
        affects_active_observers_ = affects_active_observers;
    }
    bool affects_active_observers() const { return affects_active_observers_; }

    void set_affects_pending_observers(bool affects_pending_observers)
    {
        affects_pending_observers_ = affects_pending_observers;
    }
    bool affects_pending_observers() const { return affects_pending_observers_; }

private:
    Animation(scoped_ptr<AnimationCurve> curve,
        int animation_id,
        int group_id,
        TargetProperty target_property);

    base::TimeDelta ConvertToActiveTime(base::TimeTicks monotonic_time) const;

    scoped_ptr<AnimationCurve> curve_;

    // IDs must be unique.
    int id_;

    // Animations that must be run together are called 'grouped' and have the same
    // group id. Grouped animations are guaranteed to start at the same time and
    // no other animations may animate any of the group's target properties until
    // all animations in the group have finished animating.
    int group_;

    TargetProperty target_property_;
    RunState run_state_;
    double iterations_;
    double iteration_start_;
    base::TimeTicks start_time_;
    Direction direction_;
    double playback_rate_;
    FillMode fill_mode_;

    // The time offset effectively pushes the start of the animation back in time.
    // This is used for resuming paused animations -- an animation is added with a
    // non-zero time offset, causing the animation to skip ahead to the desired
    // point in time.
    base::TimeDelta time_offset_;

    bool needs_synchronized_start_time_;
    bool received_finished_event_;

    // When an animation is suspended, it behaves as if it is paused and it also
    // ignores all run state changes until it is resumed. This is used for testing
    // purposes.
    bool suspended_;

    // These are used in TrimTimeToCurrentIteration to account for time
    // spent while paused. This is not included in AnimationState since it
    // there is absolutely no need for clients of this controller to know
    // about these values.
    base::TimeTicks pause_time_;
    base::TimeDelta total_paused_time_;

    // Animations lead dual lives. An active animation will be conceptually owned
    // by two controllers, one on the impl thread and one on the main. In reality,
    // there will be two separate Animation instances for the same animation. They
    // will have the same group id and the same target property (these two values
    // uniquely identify an animation). The instance on the impl thread is the
    // instance that ultimately controls the values of the animating layer and so
    // we will refer to it as the 'controlling instance'.
    bool is_controlling_instance_;

    bool is_impl_only_;

    // When pushed from a main-thread controller to a compositor-thread
    // controller, an animation will initially only affect pending observers
    // (corresponding to layers in the pending tree). Animations that only
    // affect pending observers are able to reach the STARTING state and tick
    // pending observers, but cannot proceed any further and do not tick active
    // observers. After activation, such animations affect both kinds of observers
    // and are able to proceed past the STARTING state. When the removal of
    // an animation is pushed from a main-thread controller to a
    // compositor-thread controller, this initially only makes the animation
    // stop affecting pending observers. After activation, such animations no
    // longer affect any observers, and are deleted.
    bool affects_active_observers_;
    bool affects_pending_observers_;

    DISALLOW_COPY_AND_ASSIGN(Animation);
};

} // namespace cc

#endif // CC_ANIMATION_ANIMATION_H_
