// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/scheduler/begin_frame_source.h"

#include "base/auto_reset.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/scheduler/delay_based_time_source.h"
#include "cc/scheduler/scheduler.h"

#ifdef NDEBUG
#define DEBUG_FRAMES(...)
#else
#define DEBUG_FRAMES(name, arg1_name, arg1_val, arg2_name, arg2_val)     \
    TRACE_EVENT2(TRACE_DISABLED_BY_DEFAULT("cc.debug.scheduler.frames"), \
        name,                                                            \
        arg1_name,                                                       \
        arg1_val,                                                        \
        arg2_name,                                                       \
        arg2_val);
#endif

namespace cc {

// BeginFrameObserverBase -----------------------------------------------
BeginFrameObserverBase::BeginFrameObserverBase()
    : last_begin_frame_args_()
    , dropped_begin_frame_args_(0)
{
}

const BeginFrameArgs BeginFrameObserverBase::LastUsedBeginFrameArgs() const
{
    return last_begin_frame_args_;
}
void BeginFrameObserverBase::OnBeginFrame(const BeginFrameArgs& args)
{
    DEBUG_FRAMES("BeginFrameObserverBase::OnBeginFrame",
        "last args",
        last_begin_frame_args_.AsValue(),
        "new args",
        args.AsValue());
    DCHECK(args.IsValid());
    DCHECK(args.frame_time >= last_begin_frame_args_.frame_time);
    bool used = OnBeginFrameDerivedImpl(args);
    if (used) {
        last_begin_frame_args_ = args;
    } else {
        ++dropped_begin_frame_args_;
    }
}

void BeginFrameObserverBase::AsValueInto(
    base::trace_event::TracedValue* dict) const
{
    dict->BeginDictionary("last_begin_frame_args_");
    last_begin_frame_args_.AsValueInto(dict);
    dict->EndDictionary();
    dict->SetInteger("dropped_begin_frame_args_", dropped_begin_frame_args_);
}

// BeginFrameSourceBase ------------------------------------------------------
BeginFrameSourceBase::BeginFrameSourceBase()
    : observer_(NULL)
    , needs_begin_frames_(false)
    , inside_as_value_into_(false)
{
    DCHECK(!observer_);
    DCHECK_EQ(inside_as_value_into_, false);
}

bool BeginFrameSourceBase::NeedsBeginFrames() const
{
    return needs_begin_frames_;
}

void BeginFrameSourceBase::SetNeedsBeginFrames(bool needs_begin_frames)
{
    DEBUG_FRAMES("BeginFrameSourceBase::SetNeedsBeginFrames",
        "current state",
        needs_begin_frames_,
        "new state",
        needs_begin_frames);
    if (needs_begin_frames_ != needs_begin_frames) {
        needs_begin_frames_ = needs_begin_frames;
        OnNeedsBeginFramesChange(needs_begin_frames);
    }
}

void BeginFrameSourceBase::AddObserver(BeginFrameObserver* obs)
{
    DEBUG_FRAMES("BeginFrameSourceBase::AddObserver",
        "current observer",
        observer_,
        "to add observer",
        obs);
    DCHECK(!observer_);
    observer_ = obs;
}

void BeginFrameSourceBase::RemoveObserver(BeginFrameObserver* obs)
{
    DEBUG_FRAMES("BeginFrameSourceBase::RemoveObserver",
        "current observer",
        observer_,
        "to remove observer",
        obs);
    DCHECK_EQ(observer_, obs);
    observer_ = NULL;
}

void BeginFrameSourceBase::CallOnBeginFrame(const BeginFrameArgs& args)
{
    DEBUG_FRAMES("BeginFrameSourceBase::CallOnBeginFrame",
        "current observer",
        observer_,
        "args",
        args.AsValue());
    if (observer_) {
        return observer_->OnBeginFrame(args);
    }
}

// Tracing support
void BeginFrameSourceBase::AsValueInto(
    base::trace_event::TracedValue* dict) const
{
    // As the observer might try to trace the source, prevent an infinte loop
    // from occuring.
    if (inside_as_value_into_) {
        dict->SetString("observer", "<loop detected>");
        return;
    }

    if (observer_) {
        base::AutoReset<bool> prevent_loops(
            const_cast<bool*>(&inside_as_value_into_), true);
        dict->BeginDictionary("observer");
        observer_->AsValueInto(dict);
        dict->EndDictionary();
    } else {
        dict->SetString("observer", "NULL");
    }
    dict->SetBoolean("needs_begin_frames", NeedsBeginFrames());
}

// BackToBackBeginFrameSource --------------------------------------------
scoped_ptr<BackToBackBeginFrameSource> BackToBackBeginFrameSource::Create(
    base::SingleThreadTaskRunner* task_runner)
{
    return make_scoped_ptr(new BackToBackBeginFrameSource(task_runner));
}

BackToBackBeginFrameSource::BackToBackBeginFrameSource(
    base::SingleThreadTaskRunner* task_runner)
    : BeginFrameSourceBase()
    , task_runner_(task_runner)
    , send_begin_frame_posted_(false)
    , weak_factory_(this)
{
    DCHECK(task_runner);
    DCHECK_EQ(needs_begin_frames_, false);
    DCHECK_EQ(send_begin_frame_posted_, false);
}

BackToBackBeginFrameSource::~BackToBackBeginFrameSource()
{
}

base::TimeTicks BackToBackBeginFrameSource::Now()
{
    return base::TimeTicks::Now();
}

void BackToBackBeginFrameSource::OnNeedsBeginFramesChange(
    bool needs_begin_frames)
{
    if (!needs_begin_frames)
        return;

    if (send_begin_frame_posted_)
        return;

    send_begin_frame_posted_ = true;
    task_runner_->PostTask(FROM_HERE,
        base::Bind(&BackToBackBeginFrameSource::BeginFrame,
            weak_factory_.GetWeakPtr()));
}

void BackToBackBeginFrameSource::BeginFrame()
{
    send_begin_frame_posted_ = false;

    if (!needs_begin_frames_)
        return;

    base::TimeTicks now = Now();
    BeginFrameArgs args = BeginFrameArgs::Create(
        BEGINFRAME_FROM_HERE, now, now + BeginFrameArgs::DefaultInterval(),
        BeginFrameArgs::DefaultInterval(), BeginFrameArgs::NORMAL);
    CallOnBeginFrame(args);
}

// BeginFrameSource support

void BackToBackBeginFrameSource::DidFinishFrame(size_t remaining_frames)
{
    if (remaining_frames == 0) {
        OnNeedsBeginFramesChange(NeedsBeginFrames());
    }
}

// Tracing support
void BackToBackBeginFrameSource::AsValueInto(
    base::trace_event::TracedValue* dict) const
{
    dict->SetString("type", "BackToBackBeginFrameSource");
    BeginFrameSourceBase::AsValueInto(dict);
    dict->SetBoolean("send_begin_frame_posted_", send_begin_frame_posted_);
}

// SyntheticBeginFrameSource ---------------------------------------------
scoped_ptr<SyntheticBeginFrameSource> SyntheticBeginFrameSource::Create(
    base::SingleThreadTaskRunner* task_runner,
    base::TimeDelta initial_vsync_interval)
{
    scoped_ptr<DelayBasedTimeSource> time_source = DelayBasedTimeSource::Create(initial_vsync_interval, task_runner);
    return make_scoped_ptr(new SyntheticBeginFrameSource(time_source.Pass()));
}

SyntheticBeginFrameSource::SyntheticBeginFrameSource(
    scoped_ptr<DelayBasedTimeSource> time_source)
    : BeginFrameSourceBase()
    , time_source_(time_source.Pass())
{
    time_source_->SetActive(false);
    time_source_->SetClient(this);
}

SyntheticBeginFrameSource::~SyntheticBeginFrameSource()
{
    time_source_->SetActive(false);
}

void SyntheticBeginFrameSource::OnUpdateVSyncParameters(
    base::TimeTicks new_vsync_timebase,
    base::TimeDelta new_vsync_interval)
{
    time_source_->SetTimebaseAndInterval(new_vsync_timebase, new_vsync_interval);
}

BeginFrameArgs SyntheticBeginFrameSource::CreateBeginFrameArgs(
    base::TimeTicks frame_time,
    BeginFrameArgs::BeginFrameArgsType type)
{
    base::TimeTicks deadline = time_source_->NextTickTime();
    return BeginFrameArgs::Create(BEGINFRAME_FROM_HERE, frame_time, deadline,
        time_source_->Interval(), type);
}

// DelayBasedTimeSourceClient support
void SyntheticBeginFrameSource::OnTimerTick()
{
    CallOnBeginFrame(CreateBeginFrameArgs(time_source_->LastTickTime(),
        BeginFrameArgs::NORMAL));
}

// BeginFrameSourceBase support
void SyntheticBeginFrameSource::OnNeedsBeginFramesChange(
    bool needs_begin_frames)
{
    base::TimeTicks missed_tick_time = time_source_->SetActive(needs_begin_frames);
    if (!missed_tick_time.is_null()) {
        DCHECK(needs_begin_frames);
        CallOnBeginFrame(
            CreateBeginFrameArgs(missed_tick_time, BeginFrameArgs::MISSED));
    }
}

// Tracing support
void SyntheticBeginFrameSource::AsValueInto(
    base::trace_event::TracedValue* dict) const
{
    dict->SetString("type", "SyntheticBeginFrameSource");
    BeginFrameSourceBase::AsValueInto(dict);

    dict->BeginDictionary("time_source");
    time_source_->AsValueInto(dict);
    dict->EndDictionary();
}

// BeginFrameSourceMultiplexer -------------------------------------------
scoped_ptr<BeginFrameSourceMultiplexer> BeginFrameSourceMultiplexer::Create()
{
    return make_scoped_ptr(new BeginFrameSourceMultiplexer());
}

BeginFrameSourceMultiplexer::BeginFrameSourceMultiplexer()
    : BeginFrameSourceBase()
    , minimum_interval_(base::TimeDelta())
    , active_source_(NULL)
    , source_list_()
{
}

BeginFrameSourceMultiplexer::BeginFrameSourceMultiplexer(
    base::TimeDelta minimum_interval)
    : BeginFrameSourceBase()
    , minimum_interval_(minimum_interval)
    , active_source_(NULL)
    , source_list_()
{
}

BeginFrameSourceMultiplexer::~BeginFrameSourceMultiplexer()
{
    if (active_source_) {
        active_source_->SetNeedsBeginFrames(false);
        active_source_->RemoveObserver(this);
    }
}

void BeginFrameSourceMultiplexer::SetMinimumInterval(
    base::TimeDelta new_minimum_interval)
{
    DEBUG_FRAMES("BeginFrameSourceMultiplexer::SetMinimumInterval",
        "current minimum (us)",
        minimum_interval_.InMicroseconds(),
        "new minimum (us)",
        new_minimum_interval.InMicroseconds());
    DCHECK_GE(new_minimum_interval.ToInternalValue(), 0);
    minimum_interval_ = new_minimum_interval;
}

void BeginFrameSourceMultiplexer::AddSource(BeginFrameSource* new_source)
{
    DEBUG_FRAMES("BeginFrameSourceMultiplexer::AddSource", "current active",
        active_source_, "source to be added", new_source);
    DCHECK(new_source);
    DCHECK(!HasSource(new_source));

    source_list_.insert(new_source);

    // If there is no active source, set the new one as the active one.
    if (!active_source_)
        SetActiveSource(new_source);
}

void BeginFrameSourceMultiplexer::RemoveSource(
    BeginFrameSource* existing_source)
{
    DEBUG_FRAMES("BeginFrameSourceMultiplexer::RemoveSource", "current active",
        active_source_, "source to be removed", existing_source);
    DCHECK(existing_source);
    DCHECK(HasSource(existing_source));
    DCHECK_NE(existing_source, active_source_);
    source_list_.erase(existing_source);
}

void BeginFrameSourceMultiplexer::SetActiveSource(
    BeginFrameSource* new_source)
{
    DEBUG_FRAMES("BeginFrameSourceMultiplexer::SetActiveSource",
        "current active",
        active_source_,
        "to become active",
        new_source);

    DCHECK(HasSource(new_source) || new_source == NULL);

    bool needs_begin_frames = NeedsBeginFrames();
    if (active_source_) {
        if (needs_begin_frames)
            SetNeedsBeginFrames(false);

        // Technically we shouldn't need to remove observation, but this prevents
        // the case where SetNeedsBeginFrames message gets to the source after a
        // message has already been sent.
        active_source_->RemoveObserver(this);
        active_source_ = NULL;
    }
    DCHECK(!active_source_);
    active_source_ = new_source;

    if (active_source_) {
        active_source_->AddObserver(this);

        if (needs_begin_frames) {
            SetNeedsBeginFrames(true);
        }
    }
}

const BeginFrameSource* BeginFrameSourceMultiplexer::ActiveSource()
{
    return active_source_;
}

// BeginFrameObserver support
void BeginFrameSourceMultiplexer::OnBeginFrame(const BeginFrameArgs& args)
{
    if (!IsIncreasing(args)) {
        DEBUG_FRAMES("BeginFrameSourceMultiplexer::OnBeginFrame",
            "action",
            "discarding",
            "new args",
            args.AsValue());
        return;
    }
    DEBUG_FRAMES("BeginFrameSourceMultiplexer::OnBeginFrame",
        "action",
        "using",
        "new args",
        args.AsValue());
    CallOnBeginFrame(args);
}

const BeginFrameArgs BeginFrameSourceMultiplexer::LastUsedBeginFrameArgs()
    const
{
    if (observer_)
        return observer_->LastUsedBeginFrameArgs();
    else
        return BeginFrameArgs();
}

// BeginFrameSource support
void BeginFrameSourceMultiplexer::OnNeedsBeginFramesChange(
    bool needs_begin_frames)
{
    DEBUG_FRAMES("BeginFrameSourceMultiplexer::OnNeedsBeginFramesChange",
        "active_source", active_source_, "needs_begin_frames",
        needs_begin_frames);
    if (active_source_) {
        active_source_->SetNeedsBeginFrames(needs_begin_frames);
    } else {
        DCHECK(!needs_begin_frames);
    }
}

void BeginFrameSourceMultiplexer::DidFinishFrame(size_t remaining_frames)
{
    DEBUG_FRAMES("BeginFrameSourceMultiplexer::DidFinishFrame",
        "active_source",
        active_source_,
        "remaining_frames",
        remaining_frames);
    if (active_source_) {
        active_source_->DidFinishFrame(remaining_frames);
    }
}

// Tracing support
void BeginFrameSourceMultiplexer::AsValueInto(
    base::trace_event::TracedValue* dict) const
{
    dict->SetString("type", "BeginFrameSourceMultiplexer");

    dict->SetInteger("minimum_interval_us", minimum_interval_.InMicroseconds());
    if (observer_) {
        dict->BeginDictionary("last_begin_frame_args");
        observer_->LastUsedBeginFrameArgs().AsValueInto(dict);
        dict->EndDictionary();
    }

    if (active_source_) {
        dict->BeginDictionary("active_source");
        active_source_->AsValueInto(dict);
        dict->EndDictionary();
    } else {
        dict->SetString("active_source", "NULL");
    }

    dict->BeginArray("sources");
    for (std::set<BeginFrameSource*>::const_iterator it = source_list_.begin();
         it != source_list_.end();
         ++it) {
        dict->BeginDictionary();
        (*it)->AsValueInto(dict);
        dict->EndDictionary();
    }
    dict->EndArray();
}

// protected methods
bool BeginFrameSourceMultiplexer::HasSource(BeginFrameSource* source)
{
    return (source_list_.find(source) != source_list_.end());
}

bool BeginFrameSourceMultiplexer::IsIncreasing(const BeginFrameArgs& args)
{
    DCHECK(args.IsValid());
    if (!observer_)
        return false;

    // If the last begin frame is invalid, then any new begin frame is valid.
    if (!observer_->LastUsedBeginFrameArgs().IsValid())
        return true;

    // Only allow new args have a *strictly bigger* frame_time value and statisfy
    // minimum interval requirement.
    return (args.frame_time >= observer_->LastUsedBeginFrameArgs().frame_time + minimum_interval_);
}

} // namespace cc
