// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/AnimationObj.h"

#include <cmath>

#include "mc/animation/AnimationCurve.h"
#include "mc/base/TimeUtil.h"

namespace {

// This should match the RunState enum.
static const char* const s_runStateNames[] = {
    "WAITING_FOR_TARGET_AVAILABILITY",
    "WAITING_FOR_DELETION",
    "STARTING",
    "RUNNING",
    "PAUSED",
    "FINISHED",
    "ABORTED"
};

static_assert(static_cast<int>(mc::AnimationObj::LAST_RUN_STATE) + 1 == sizeof(s_runStateNames) / sizeof(char*),
    "RunStateEnumSize should equal the number of elements in s_runStateNames");

// This should match the TargetProperty enum.
static const char* const s_targetPropertyNames[] = {
    "TRANSFORM",
    "OPACITY",
    "FILTER",
    "SCROLL_OFFSET",
    "BACKGROUND_COLOR"
};

static_assert(static_cast<int>(mc::AnimationObj::LAST_TARGET_PROPERTY) + 1 == sizeof(s_targetPropertyNames) / sizeof(char*),
    "TargetPropertyEnumSize should equal the number of elements in s_targetPropertyNames");

}  // namespace

namespace mc {

WTF::PassOwnPtr<AnimationObj> AnimationObj::create(WTF::PassOwnPtr<AnimationCurve> curve, int animationId, int groupId, TargetProperty targetProperty)
{
    return adoptPtr(new AnimationObj(curve, animationId, groupId, targetProperty));
}

AnimationObj::AnimationObj(WTF::PassOwnPtr<AnimationCurve> curve, int animationId, int groupId, TargetProperty targetProperty)
    : m_curve(curve)
    , m_id(animationId)
    , m_group(groupId)
    , m_targetProperty(targetProperty)
    , m_runState(WAITING_FOR_TARGET_AVAILABILITY)
    , m_iterations(1)
    , m_iterationStart(0)
    , m_direction(DIRECTION_NORMAL)
    , m_playbackRate(1)
    , m_fillMode(FILL_MODE_BOTH)
    , m_needsSynchronizedStartTime(false)
    , m_receivedFinishedEvent(false)
    , m_suspended(false)
    , m_isControllingInstance(false)
    , m_isImplOnly(false)
    , m_affectsActiveObservers(true)
    , m_affectsPendingObservers(true)
{
}

AnimationObj::~AnimationObj()
{
    if (m_runState == RUNNING || m_runState == PAUSED)
        setRunState(ABORTED, base::TimeTicks());
}

void AnimationObj::setRunState(RunState runState, base::TimeTicks monotonicTime)
{
    if (m_suspended)
        return;

//     char nameBuffer[256];
//     base::snprintf(nameBuffer, sizeof(nameBuffer), "%s-%d", s_targetPropertyNames[m_targetProperty], m_group);

    bool isWaitingToStart = m_runState == WAITING_FOR_TARGET_AVAILABILITY || m_runState == STARTING;

//     if (m_isControllingInstance && isWaitingToStart && runState == RUNNING) {
//         TRACE_EVENT_ASYNC_BEGIN1("cc", "AnimationObj", this, "Name", TRACE_STR_COPY(nameBuffer));
//     }

    bool was_finished = isFinished();

    const char* oldRunStateName = s_runStateNames[m_runState];

    if (runState == RUNNING && m_runState == PAUSED)
        m_totalPausedTime += (monotonicTime - m_pauseTime);
    else if (runState == PAUSED)
        m_pauseTime = monotonicTime;
    m_runState = runState;

    const char* new_run_state_name = s_runStateNames[runState];

//     if (m_isControllingInstance && !was_finished && is_finished())
//         TRACE_EVENT_ASYNC_END0("cc", "AnimationObj", this);
// 
//     char state_buffer[256];
//     base::snprintf(state_buffer, sizeof(state_buffer), "%s->%s", old_run_state_name, new_run_state_name);
// 
//     TRACE_EVENT_INSTANT2("cc", "LayerAnimationController::setRunState", TRACE_EVENT_SCOPE_THREAD,
//         "Name", TRACE_STR_COPY(nameBuffer),
//         "State", TRACE_STR_COPY(state_buffer));
}

void AnimationObj::Suspend(base::TimeTicks monotonicTime)
{
    setRunState(PAUSED, monotonicTime);
    m_suspended = true;
}

void AnimationObj::Resume(base::TimeTicks monotonicTime)
{
    m_suspended = false;
    setRunState(RUNNING, monotonicTime);
}

bool AnimationObj::isFinishedAt(base::TimeTicks monotonicTime) const
{
    if (isFinished())
        return true;

    if (m_needsSynchronizedStartTime)
        return false;

    if (m_playbackRate == 0)
        return false;

    return m_runState == RUNNING && m_iterations >= 0 && 
        TimeUtil::scale(m_curve->duration(), m_iterations / std::abs(m_playbackRate)) <= (monotonicTime + m_timeOffset - m_startTime - m_totalPausedTime);
}

bool AnimationObj::inEffect(base::TimeTicks monotonicTime) const
{
    return convertToActiveTime(monotonicTime) >= base::TimeDelta() || (m_fillMode == FILL_MODE_BOTH || m_fillMode == FILL_MODE_BACKWARDS);
}

base::TimeDelta AnimationObj::convertToActiveTime(base::TimeTicks monotonicTime) const
{
    base::TimeTicks trimmed = monotonicTime + m_timeOffset;

    // If we're paused, time is 'stuck' at the pause time.
    if (m_runState == PAUSED)
        trimmed = m_pauseTime;

    // Returned time should always be relative to the start time and should
    // subtract all time spent paused.
    trimmed -= (m_startTime - base::TimeTicks()) + m_totalPausedTime;

    // If we're just starting or we're waiting on receiving a start time,
    // time is 'stuck' at the initial state.
    if ((m_runState == STARTING && !hasSetStartTime()) || needsSynchronizedStartTime())
        trimmed = base::TimeTicks() + m_timeOffset;

    return (trimmed - base::TimeTicks());
}

base::TimeDelta AnimationObj::trimTimeToCurrentIteration(base::TimeTicks monotonicTime) const
{
    // Check for valid parameters
    ASSERT(m_playbackRate);
    ASSERT(m_iterationStart >= 0);

    base::TimeDelta activeTime = convertToActiveTime(monotonicTime);
    base::TimeDelta startOffset = TimeUtil::scale(m_curve->duration(), m_iterationStart);

    // Return start offset if we are before the start of the animation
    if (activeTime < base::TimeDelta())
        return startOffset;
    // Always return zero if we have no iterations.
    if (!m_iterations)
        return base::TimeDelta();

    // Don't attempt to trim if we have no duration.
    if (m_curve->duration() <= base::TimeDelta())
        return base::TimeDelta();

    base::TimeDelta repeatedDuration = TimeUtil::scale(m_curve->duration(), m_iterations);
    base::TimeDelta activeDuration = TimeUtil::scale(repeatedDuration, 1.0 / std::abs(m_playbackRate));

    // Check if we are past active duration
    if (m_iterations > 0 && activeTime >= activeDuration)
        activeTime = activeDuration;

    // Calculate the scaled active time
    base::TimeDelta scaledActiveTime;
    if (m_playbackRate < 0)
        scaledActiveTime = TimeUtil::scale((activeTime - activeDuration), m_playbackRate) + startOffset;
    else
        scaledActiveTime = TimeUtil::scale(activeTime, m_playbackRate) + startOffset;

    // Calculate the iteration time
    base::TimeDelta iterationTime;
    if (scaledActiveTime - startOffset == repeatedDuration && fmod(m_iterations + m_iterationStart, 1) == 0)
        iterationTime = m_curve->duration();
    else
        iterationTime = TimeUtil::mod(scaledActiveTime, m_curve->duration());

    // Calculate the current iteration
    int iteration;
    if (scaledActiveTime <= base::TimeDelta())
        iteration = 0;
    else if (iterationTime == m_curve->duration())
        iteration = ceil(m_iterationStart + m_iterations - 1);
    else
        iteration = static_cast<int>(scaledActiveTime / m_curve->duration());

    // Check if we are running the animation in reverse direction for the current iteration
    bool reverse =
        (m_direction == DIRECTION_REVERSE) ||
        (m_direction == DIRECTION_ALTERNATE && iteration % 2 == 1) ||
        (m_direction == DIRECTION_ALTERNATE_REVERSE && iteration % 2 == 0);

    // If we are running the animation in reverse direction, reverse the result
    if (reverse)
        iterationTime = m_curve->duration() - iterationTime;

    return iterationTime;
}

WTF::PassOwnPtr<AnimationObj> AnimationObj::cloneAndInitialize(RunState initial_run_state) const
{
    WTF::PassOwnPtr<AnimationObj> toReturn = adoptPtr(new AnimationObj(m_curve->clone(), m_id, m_group, m_targetProperty));
    toReturn->m_runState = initial_run_state;
    toReturn->m_iterations = m_iterations;
    toReturn->m_iterationStart = m_iterationStart;
    toReturn->m_startTime = m_startTime;
    toReturn->m_pauseTime = m_pauseTime;
    toReturn->m_totalPausedTime = m_totalPausedTime;
    toReturn->m_timeOffset = m_timeOffset;
    toReturn->m_direction = m_direction;
    toReturn->m_playbackRate = m_playbackRate;
    toReturn->m_fillMode = m_fillMode;
    ASSERT(!toReturn->m_isControllingInstance);
    toReturn->m_isControllingInstance = true;

    return toReturn;
}

void AnimationObj::pushPropertiesTo(AnimationObj* other) const 
{
    // Currently, we only push changes due to pausing and resuming animations on
    // the main thread.
    if (m_runState == AnimationObj::PAUSED || other->m_runState == AnimationObj::PAUSED) {
        other->m_runState = m_runState;
        other->m_pauseTime = m_pauseTime;
        other->m_totalPausedTime = m_totalPausedTime;
    }
}

}  // namespace cc
