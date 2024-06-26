// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationObj_h
#define mc_animation_AnimationObj_h

#include "base/time/time.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {

class AnimationCurve;

// An Animation contains all the state required to play an AnimationCurve.
// Specifically, the affected property, the run state (paused, finished, etc.),
// loop count, last pause time, and the total time spent paused.
class AnimationObj {
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

    static WTF::PassOwnPtr<AnimationObj> create(
        WTF::PassOwnPtr<AnimationCurve> curve,
        int animationId,
        int groupId,
        TargetProperty targetProperty
        );

    virtual ~AnimationObj();

    int id() const 
    {
        return m_id;
    }

    int group() const 
    {
        return m_group;
    }

    TargetProperty getTargetProperty() const 
    {
        return m_targetProperty;
    }

    RunState runState() const { return m_runState; }
    void setRunState(RunState runState, base::TimeTicks monotonicTime);

    // This is the number of times that the animation will play. If this
    // value is zero the animation will not play. If it is negative, then
    // the animation will loop indefinitely.
    double iterations() const 
    {
        return m_iterations;
    }
    void setIterations(double n) 
    {
        m_iterations = n;
    }

    double iterationStart() const 
    {
        return m_iterationStart;
    }

    void setIterationStart(double iteration_start)
    {
        m_iterationStart = iteration_start;
    }

    base::TimeTicks startTime() const
    {
        return m_startTime;
    }

    void setStartTime(base::TimeTicks monotonicTime)
    {
        m_startTime = monotonicTime;
    }

    bool hasSetStartTime() const
    {
        return !m_startTime.is_null();
    }

    base::TimeDelta timeOffset() const 
    {
        return m_timeOffset;
    }
    void setTimeOffset(base::TimeDelta monotonicTime) 
    {
        m_timeOffset = monotonicTime;
    }

    void Suspend(base::TimeTicks monotonicTime);
    void Resume(base::TimeTicks monotonicTime);

    Direction direction() 
    {
        return m_direction;
    }
    void setDirection(Direction direction) 
    {
        m_direction = direction;
    }

    FillMode fillMode() 
    {
        return m_fillMode;
    }
    void setFillMode(FillMode fill_mode) 
    {
        m_fillMode = fill_mode;
    }

    double playbackRate() 
    {
        return m_playbackRate;
    }
    void setPlaybackRate(double playback_rate) 
    {
        m_playbackRate = playback_rate;
    }

    bool isFinishedAt(base::TimeTicks monotonicTime) const;
    bool isFinished() const
    {
        return m_runState == FINISHED || m_runState == ABORTED || m_runState == WAITING_FOR_DELETION;
    }

    bool inEffect(base::TimeTicks monotonicTime) const;

    AnimationCurve* curve() 
    {
        return m_curve.get();
    }
    const AnimationCurve* curve() const 
    {
        return m_curve.get();
    }

    // If this is true, even if the animation is running, it will not be tickable
    // until it is given a start time. This is true for animations running on the
    // main thread.
    bool needsSynchronizedStartTime() const 
    {
        return m_needsSynchronizedStartTime;
    }
    void setNeedsSynchronizedStartTime(bool needs_synchronized_start_time)
    {
        m_needsSynchronizedStartTime = needs_synchronized_start_time;
    }

    // This is true for animations running on the main thread when the FINISHED
    // event sent by the corresponding impl animation has been received.
    bool receivedFinishedEvent() const 
    {
        return m_receivedFinishedEvent;
    }
    void setReceivedFinishedEvent(bool received_finished_event) 
    {
        m_receivedFinishedEvent = received_finished_event;
    }

    // Takes the given absolute time, and using the start time and the number
    // of iterations, returns the relative time in the current iteration.
    base::TimeDelta trimTimeToCurrentIteration(base::TimeTicks monotonicTime) const;

    WTF::PassOwnPtr<AnimationObj> cloneAndInitialize(RunState initial_run_state) const;

    void setIsControllingInstanceForTest(bool is_controlling_instance) 
    {
        m_isControllingInstance = is_controlling_instance;
    }
    bool isControllingInstance() const 
    {
        return m_isControllingInstance;
    }

    void pushPropertiesTo(AnimationObj* other) const;

    void setIsImplOnly(bool is_impl_only) 
    {
        m_isImplOnly = is_impl_only;
    }
    bool isImplOnly() const 
    {
        return m_isImplOnly;
    }

    void setAffectsActiveObservers(bool affects_active_observers) 
    {
        m_affectsActiveObservers = affects_active_observers;
    }
    bool affectsActiveObservers() const 
    {
        return m_affectsActiveObservers;
    }

    void setAffectsPendingObservers(bool affects_pending_observers) 
    {
        m_affectsPendingObservers = affects_pending_observers;
    }
    bool affectsPendingObservers() const 
    {
        return m_affectsPendingObservers;
    }

private:
    AnimationObj(WTF::PassOwnPtr<AnimationCurve> curve, int animationId, int groupId, TargetProperty targetProperty);

    base::TimeDelta convertToActiveTime(base::TimeTicks monotonicTime) const;

    WTF::OwnPtr<AnimationCurve> m_curve;

    // IDs must be unique.
    int m_id;

    // Animations that must be run together are called 'grouped' and have the same
    // group id. Grouped animations are guaranteed to start at the same time and
    // no other animations may animate any of the group's target properties until
    // all animations in the group have finished animating.
    int m_group;

    TargetProperty m_targetProperty;
    RunState m_runState;
    double m_iterations;
    double m_iterationStart;
    base::TimeTicks m_startTime;
    Direction m_direction;
    double m_playbackRate;
    FillMode m_fillMode;

    // The time offset effectively pushes the start of the animation back in time.
    // This is used for resuming paused animations -- an animation is added with a
    // non-zero time offset, causing the animation to skip ahead to the desired
    // point in time.
    base::TimeDelta m_timeOffset;

    bool m_needsSynchronizedStartTime;
    bool m_receivedFinishedEvent;

    // When an animation is suspended, it behaves as if it is paused and it also
    // ignores all run state changes until it is resumed. This is used for testing
    // purposes.
    bool m_suspended;

    // These are used in TrimTimeToCurrentIteration to account for time
    // spent while paused. This is not included in AnimationState since it
    // there is absolutely no need for clients of this controller to know
    // about these values.
    base::TimeTicks m_pauseTime;
    base::TimeDelta m_totalPausedTime;

    // Animations lead dual lives. An active animation will be conceptually owned
    // by two controllers, one on the impl thread and one on the main. In reality,
    // there will be two separate Animation instances for the same animation. They
    // will have the same group id and the same target property (these two values
    // uniquely identify an animation). The instance on the impl thread is the
    // instance that ultimately controls the values of the animating layer and so
    // we will refer to it as the 'controlling instance'.
    bool m_isControllingInstance;

    bool m_isImplOnly;

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
    bool m_affectsActiveObservers;
    bool m_affectsPendingObservers;
};

}  // namespace mc

#endif  // mc_animation_AnimationObj_h
