// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/scheduler/delay_based_time_source.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "base/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"

namespace cc {

namespace {

    // kDoubleTickDivisor prevents ticks from running within the specified
    // fraction of an interval.  This helps account for jitter in the timebase as
    // well as quick timer reactivation.
    static const int kDoubleTickDivisor = 2;

    // kIntervalChangeThreshold is the fraction of the interval that will trigger an
    // immediate interval change.  kPhaseChangeThreshold is the fraction of the
    // interval that will trigger an immediate phase change.  If the changes are
    // within the thresholds, the change will take place on the next tick.  If
    // either change is outside the thresholds, the next tick will be canceled and
    // reissued immediately.
    static const double kIntervalChangeThreshold = 0.25;
    static const double kPhaseChangeThreshold = 0.25;

} // namespace

// The following methods correspond to the DelayBasedTimeSource that uses
// the base::TimeTicks::Now as the timebase.
DelayBasedTimeSource::DelayBasedTimeSource(
    base::TimeDelta interval,
    base::SingleThreadTaskRunner* task_runner)
    : client_(nullptr)
    , active_(false)
    , timebase_(base::TimeTicks())
    , interval_(interval)
    , last_tick_time_(base::TimeTicks() - interval)
    , next_tick_time_(base::TimeTicks())
    , task_runner_(task_runner)
    , weak_factory_(this)
{
    DCHECK_GT(interval, base::TimeDelta());
}

DelayBasedTimeSource::~DelayBasedTimeSource() { }

base::TimeTicks DelayBasedTimeSource::SetActive(bool active)
{
    TRACE_EVENT1("cc", "DelayBasedTimeSource::SetActive", "active", active);

    if (active == active_)
        return base::TimeTicks();

    active_ = active;

    if (!active_) {
        next_tick_time_ = base::TimeTicks();
        tick_closure_.Cancel();
        return base::TimeTicks();
    }

    ResetTickTask(Now());

    // Determine if there was a tick that was missed while not active.
    base::TimeTicks last_tick_time_if_always_active = next_tick_time_ - interval_;
    base::TimeTicks last_tick_time_threshold = last_tick_time_ + interval_ / kDoubleTickDivisor;
    if (last_tick_time_if_always_active > last_tick_time_threshold) {
        last_tick_time_ = last_tick_time_if_always_active;
        return last_tick_time_;
    }

    return base::TimeTicks();
}

base::TimeDelta DelayBasedTimeSource::Interval() const
{
    return interval_;
}

bool DelayBasedTimeSource::Active() const { return active_; }

base::TimeTicks DelayBasedTimeSource::LastTickTime() const
{
    return last_tick_time_;
}

base::TimeTicks DelayBasedTimeSource::NextTickTime() const
{
    return next_tick_time_;
}

void DelayBasedTimeSource::OnTimerTick()
{
    DCHECK(active_);

    last_tick_time_ = next_tick_time_;

    PostNextTickTask(Now());

    // Fire the tick.
    if (client_)
        client_->OnTimerTick();
}

void DelayBasedTimeSource::PostNextTickTask(base::TimeTicks now)
{
    next_tick_time_ = NextTickTarget(now);
    DCHECK(next_tick_time_ >= now);
    // Post another task *before* the tick and update state
    base::TimeDelta delay = next_tick_time_ - now;
    task_runner_->PostDelayedTask(FROM_HERE, tick_closure_.callback(), delay);

    //---
//     static int64_t s_lastTime = 0;
//     static int64_t s_last_tick_time = 0;
//     static int s_tick = 0;
//     int64_t t1 = Now().ToInternalValue();
//     int t2 = ::GetTickCount();
//     char* output = (char*)malloc(0x100);
//     const char* show = "PostNextTickTask: tick:%d, now time det:%I64d, last_tick_time det:%I64d, delay:%I64d, tid:%d\n";
//     sprintf_s(output, 0x99, show, t2 - s_tick, t1 - s_lastTime, last_tick_time_.ToInternalValue() - s_last_tick_time, delay.ToInternalValue(), ::GetCurrentThreadId());
//     OutputDebugStringA(output);
//     free(output);
//     s_tick = t2;
//     s_lastTime = t1;
//     s_last_tick_time = last_tick_time_.ToInternalValue();
    //---
}

void DelayBasedTimeSource::SetClient(DelayBasedTimeSourceClient* client)
{
    client_ = client;
}

void DelayBasedTimeSource::SetTimebaseAndInterval(base::TimeTicks timebase,
    base::TimeDelta interval)
{
    DCHECK_GT(interval, base::TimeDelta());

    // If the change in interval is larger than the change threshold,
    // request an immediate reset.
    double interval_delta = std::abs((interval - interval_).InSecondsF());
    // Comparing with next_tick_time_ is the right thing to do because we want to
    // know if we want to cancel the existing tick task and schedule a new one.
    // Also next_tick_time_ = timebase_ mod interval_.
    double timebase_delta = std::abs((timebase - next_tick_time_).InSecondsF());

    interval_ = interval;
    timebase_ = timebase;

    // If we aren't active, there's no need to reset the timer.
    if (!active_)
        return;

    double interval_change = interval_delta / interval.InSecondsF();
    if (interval_change > kIntervalChangeThreshold) {
        TRACE_EVENT_INSTANT0("cc", "DelayBasedTimeSource::IntervalChanged",
            TRACE_EVENT_SCOPE_THREAD);
        ResetTickTask(Now());
        return;
    }

    // If the change in phase is greater than the change threshold in either
    // direction, request an immediate reset. This logic might result in a false
    // negative if there is a simultaneous small change in the interval and the
    // fmod just happens to return something near zero. Assuming the timebase
    // is very recent though, which it should be, we'll still be ok because the
    // old clock and new clock just happen to line up.
    double phase_change = fmod(timebase_delta, interval.InSecondsF()) / interval.InSecondsF();
    if (phase_change > kPhaseChangeThreshold && phase_change < (1.0 - kPhaseChangeThreshold)) {
        TRACE_EVENT_INSTANT0("cc", "DelayBasedTimeSource::PhaseChanged",
            TRACE_EVENT_SCOPE_THREAD);
        ResetTickTask(Now());
        return;
    }
}

base::TimeTicks DelayBasedTimeSource::Now() const
{
    return base::TimeTicks::Now();
}

// This code tries to achieve an average tick rate as close to interval_ as
// possible.  To do this, it has to deal with a few basic issues:
//   1. PostDelayedTask can delay only at a millisecond granularity. So, 16.666
//   has to posted as 16 or 17.
//   2. A delayed task may come back a bit late (a few ms), or really late
//   (frames later)
//
// The basic idea with this scheduler here is to keep track of where we *want*
// to run in tick_target_. We update this with the exact interval.
//
// Then, when we post our task, we take the floor of (tick_target_ and Now()).
// If we started at now=0, and 60FPs (all times in milliseconds):
//      now=0    target=16.667   PostDelayedTask(16)
//
// When our callback runs, we figure out how far off we were from that goal.
// Because of the flooring operation, and assuming our timer runs exactly when
// it should, this yields:
//      now=16   target=16.667
//
// Since we can't post a 0.667 ms task to get to now=16, we just treat this as a
// tick. Then, we update target to be 33.333. We now post another task based on
// the difference between our target and now:
//      now=16   tick_target=16.667  new_target=33.333   -->
//          PostDelayedTask(floor(33.333 - 16)) --> PostDelayedTask(17)
//
// Over time, with no late tasks, this leads to us posting tasks like this:
//      now=0    tick_target=0       new_target=16.667   -->
//          tick(), PostDelayedTask(16)
//      now=16   tick_target=16.667  new_target=33.333   -->
//          tick(), PostDelayedTask(17)
//      now=33   tick_target=33.333  new_target=50.000   -->
//          tick(), PostDelayedTask(17)
//      now=50   tick_target=50.000  new_target=66.667   -->
//          tick(), PostDelayedTask(16)
//
// We treat delays in tasks differently depending on the amount of delay we
// encounter. Suppose we posted a task with a target=16.667:
//   Case 1: late but not unrecoverably-so
//      now=18 tick_target=16.667
//
//   Case 2: so late we obviously missed the tick
//      now=25.0 tick_target=16.667
//
// We treat the first case as a tick anyway, and assume the delay was unusual.
// Thus, we compute the new_target based on the old timebase:
//      now=18   tick_target=16.667  new_target=33.333   -->
//          tick(), PostDelayedTask(floor(33.333-18)) --> PostDelayedTask(15)
// This brings us back to 18+15 = 33, which was where we would have been if the
// task hadn't been late.
//
// For the really late delay, we we move to the next logical tick. The timebase
// is not reset.
//      now=37   tick_target=16.667  new_target=50.000  -->
//          tick(), PostDelayedTask(floor(50.000-37)) --> PostDelayedTask(13)
base::TimeTicks DelayBasedTimeSource::NextTickTarget(base::TimeTicks now) const
{
    base::TimeTicks next_tick_target = now.SnappedToNextTick(timebase_, interval_);
    DCHECK(now <= next_tick_target)
        << "now = " << now.ToInternalValue()
        << "; new_tick_target = " << next_tick_target.ToInternalValue()
        << "; new_interval = " << interval_.InMicroseconds()
        << "; new_timbase = " << timebase_.ToInternalValue();

    // Avoid double ticks when:
    // 1) Turning off the timer and turning it right back on.
    // 2) Jittery data is passed to SetTimebaseAndInterval().
    if (next_tick_target - last_tick_time_ <= interval_ / kDoubleTickDivisor)
        next_tick_target += interval_;

    return next_tick_target;
}

void DelayBasedTimeSource::ResetTickTask(base::TimeTicks now)
{
    tick_closure_.Reset(base::Bind(&DelayBasedTimeSource::OnTimerTick, weak_factory_.GetWeakPtr()));
    PostNextTickTask(now);
}

std::string DelayBasedTimeSource::TypeString() const
{
    return "DelayBasedTimeSource";
}

void DelayBasedTimeSource::AsValueInto(base::trace_event::TracedValue* state) const
{
    state->SetString("type", TypeString());
    state->SetDouble("last_tick_time_us", LastTickTime().ToInternalValue());
    state->SetDouble("next_tick_time_us", NextTickTime().ToInternalValue());
    state->SetDouble("interval_us", interval_.InMicroseconds());
    state->SetDouble("timebase_us", timebase_.ToInternalValue());
    state->SetBoolean("active", active_);
}

} // namespace cc
