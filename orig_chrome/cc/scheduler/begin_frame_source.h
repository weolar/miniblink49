// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SCHEDULER_BEGIN_FRAME_SOURCE_H_
#define CC_SCHEDULER_BEGIN_FRAME_SOURCE_H_

#include <set>
#include <string>

#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "cc/output/begin_frame_args.h"
#include "cc/scheduler/delay_based_time_source.h"

namespace cc {

// (Pure) Interface for observing BeginFrame messages from BeginFrameSource
// objects.
class CC_EXPORT BeginFrameObserver {
public:
    virtual ~BeginFrameObserver() { }

    // The |args| given to OnBeginFrame is guaranteed to have
    // |args|.IsValid()==true and have |args|.frame_time
    // field be strictly greater than the previous call.
    //
    // Side effects: This function can (and most of the time *will*) change the
    // return value of the LastUsedBeginFrameArgs method. See the documentation
    // on that method for more information.
    virtual void OnBeginFrame(const BeginFrameArgs& args) = 0;

    // Returns the last BeginFrameArgs used by the observer. This method's return
    // value is affected by the OnBeginFrame method!
    //
    //  - Before the first call of OnBeginFrame, this method should return a
    //    BeginFrameArgs on which IsValid() returns false.
    //
    //  - If the |args| passed to OnBeginFrame is (or *will be*) used, then
    //    LastUsedBeginFrameArgs return value should become the |args| given to
    //    OnBeginFrame.
    //
    //  - If the |args| passed to OnBeginFrame is dropped, then
    //    LastUsedBeginFrameArgs return value should *not* change.
    //
    // These requirements are designed to allow chaining and nesting of
    // BeginFrameObservers which filter the incoming BeginFrame messages while
    // preventing "double dropping" and other bad side effects.
    virtual const BeginFrameArgs LastUsedBeginFrameArgs() const = 0;

    // Tracing support
    virtual void AsValueInto(base::trace_event::TracedValue* dict) const = 0;
};

// Simple base class which implements a BeginFrameObserver which checks the
// incoming values meet the BeginFrameObserver requirements and implements the
// required LastUsedBeginFrameArgs behaviour.
//
// Users of this class should;
//  - Implement the OnBeginFrameDerivedImpl function.
//  - Recommended (but not required) to call
//    BeginFrameObserverBase::OnValueInto in their overridden OnValueInto
//    function.
class CC_EXPORT BeginFrameObserverBase : public BeginFrameObserver {
public:
    BeginFrameObserverBase();

    // BeginFrameObserver

    // Traces |args| and DCHECK |args| satisfies pre-conditions then calls
    // OnBeginFrameDerivedImpl and updates the last_begin_frame_args_ value on
    // true.
    void OnBeginFrame(const BeginFrameArgs& args) override;
    const BeginFrameArgs LastUsedBeginFrameArgs() const override;

    // Outputs last_begin_frame_args_
    void AsValueInto(base::trace_event::TracedValue* dict) const override;

protected:
    // Subclasses should override this method!
    // Return true if the given argument is (or will be) used.
    virtual bool OnBeginFrameDerivedImpl(const BeginFrameArgs& args) = 0;

    BeginFrameArgs last_begin_frame_args_;
    int64_t dropped_begin_frame_args_;

private:
    DISALLOW_COPY_AND_ASSIGN(BeginFrameObserverBase);
};

// Interface for a class which produces BeginFrame calls to a
// BeginFrameObserver.
//
// BeginFrame calls *normally* occur just after a vsync interrupt when input
// processing has been finished and provide information about the time values
// of the vsync times. *However*, these values can be heavily modified or even
// plain made up (when no vsync signal is available or vsync throttling is
// turned off). See the BeginFrameObserver for information about the guarantees
// all BeginFrameSources *must* provide.
class CC_EXPORT BeginFrameSource {
public:
    virtual ~BeginFrameSource() { }

    // SetNeedsBeginFrames is the on/off "switch" for the BeginFrameSource. When
    // set to false no more BeginFrame messages should be sent to observer.
    virtual bool NeedsBeginFrames() const = 0;
    virtual void SetNeedsBeginFrames(bool needs_begin_frames) = 0;

    // DidFinishFrame provides back pressure to a frame source about frame
    // processing (rather than toggling SetNeedsBeginFrames every frame). It is
    // used by systems like the BackToBackFrameSource to make sure only one frame
    // is pending at a time.
    virtual void DidFinishFrame(size_t remaining_frames) = 0;

    // Add/Remove an observer from the source.
    // *At the moment* only a single observer can be added to the source, however
    // in the future this may be extended to allow multiple observers.
    // If making this change, please use base::ObserverList to do so.
    virtual void AddObserver(BeginFrameObserver* obs) = 0;
    virtual void RemoveObserver(BeginFrameObserver* obs) = 0;

    // Tells the Source that client is ready to handle BeginFrames messages.
    virtual void SetClientReady() = 0;

    // Tracing support - Recommend (but not required) to call this implementation
    // in any override.
    virtual void AsValueInto(base::trace_event::TracedValue* dict) const = 0;
};

// Simple base class which implements a BeginFrameSource.
// Implementation classes should:
//  - Implement the pure virtual (Set)NeedsBeginFrames methods from
//    BeginFrameSource.
//  - Use the CallOnBeginFrame method to call to the observer(s).
//  - Recommended (but not required) to call BeginFrameSourceBase::AsValueInto
//    in their own AsValueInto implementation.
class CC_EXPORT BeginFrameSourceBase : public BeginFrameSource {
public:
    ~BeginFrameSourceBase() override { }

    // BeginFrameSource
    bool NeedsBeginFrames() const final;
    void SetNeedsBeginFrames(bool needs_begin_frames) final;
    void DidFinishFrame(size_t remaining_frames) override { }
    void AddObserver(BeginFrameObserver* obs) final;
    void RemoveObserver(BeginFrameObserver* obs) final;
    void SetClientReady() override { }

    // Tracing support - Recommend (but not required) to call this implementation
    // in any override.
    void AsValueInto(base::trace_event::TracedValue* dict) const override;

protected:
    BeginFrameSourceBase();

    // These methods should be used by subclasses to make the call to the
    // observers.
    void CallOnBeginFrame(const BeginFrameArgs& args);

    // This method should be overridden if you want to change some behaviour on
    // needs_begin_frames change.
    virtual void OnNeedsBeginFramesChange(bool needs_begin_frames) { }

    BeginFrameObserver* observer_;
    bool needs_begin_frames_;

private:
    bool inside_as_value_into_;

    DISALLOW_COPY_AND_ASSIGN(BeginFrameSourceBase);
};

// A frame source which calls BeginFrame (at the next possible time) as soon as
// remaining frames reaches zero.
class CC_EXPORT BackToBackBeginFrameSource : public BeginFrameSourceBase {
public:
    static scoped_ptr<BackToBackBeginFrameSource> Create(
        base::SingleThreadTaskRunner* task_runner);
    ~BackToBackBeginFrameSource() override;

    // BeginFrameSource
    void DidFinishFrame(size_t remaining_frames) override;

    // Tracing
    void AsValueInto(base::trace_event::TracedValue* dict) const override;

protected:
    explicit BackToBackBeginFrameSource(
        base::SingleThreadTaskRunner* task_runner);
    virtual base::TimeTicks Now(); // Now overridable for testing

    base::SingleThreadTaskRunner* task_runner_;

    bool send_begin_frame_posted_;

    // BeginFrameSourceBase
    void OnNeedsBeginFramesChange(bool needs_begin_frames) override;

    void BeginFrame();

private:
    base::WeakPtrFactory<BackToBackBeginFrameSource> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(BackToBackBeginFrameSource);
};

// A frame source which is locked to an external parameters provides from a
// vsync source and generates BeginFrameArgs for it.
class CC_EXPORT SyntheticBeginFrameSource : public BeginFrameSourceBase,
                                            public DelayBasedTimeSourceClient {
public:
    static scoped_ptr<SyntheticBeginFrameSource> Create(
        base::SingleThreadTaskRunner* task_runner,
        base::TimeDelta initial_vsync_interval);
    ~SyntheticBeginFrameSource() override;

    void OnUpdateVSyncParameters(base::TimeTicks new_vsync_timebase,
        base::TimeDelta new_vsync_interval);

    // Tracing
    void AsValueInto(base::trace_event::TracedValue* dict) const override;

    // DelayBasedTimeSourceClient
    void OnTimerTick() override;

protected:
    explicit SyntheticBeginFrameSource(
        scoped_ptr<DelayBasedTimeSource> time_source);

    BeginFrameArgs CreateBeginFrameArgs(base::TimeTicks frame_time,
        BeginFrameArgs::BeginFrameArgsType type);

    // BeginFrameSourceBase
    void OnNeedsBeginFramesChange(bool needs_begin_frames) override;

    scoped_ptr<DelayBasedTimeSource> time_source_;

private:
    DISALLOW_COPY_AND_ASSIGN(SyntheticBeginFrameSource);
};

// A "virtual" frame source which lets you switch between multiple other frame
// sources while making sure the BeginFrameArgs stays increasing (possibly
// enforcing minimum boundry between BeginFrameArgs messages).
class CC_EXPORT BeginFrameSourceMultiplexer : public BeginFrameSourceBase,
                                              public BeginFrameObserver {
public:
    static scoped_ptr<BeginFrameSourceMultiplexer> Create();
    ~BeginFrameSourceMultiplexer() override;

    void SetMinimumInterval(base::TimeDelta new_minimum_interval);

    void AddSource(BeginFrameSource* new_source);
    void RemoveSource(BeginFrameSource* existing_source);
    void SetActiveSource(BeginFrameSource* new_source);
    const BeginFrameSource* ActiveSource();

    // BeginFrameObserver
    // The mux is an BeginFrameObserver as it needs to proxy the OnBeginFrame
    // calls to preserve the monotonicity of the BeginFrameArgs when switching
    // sources.
    void OnBeginFrame(const BeginFrameArgs& args) override;
    const BeginFrameArgs LastUsedBeginFrameArgs() const override;

    // BeginFrameSource
    void DidFinishFrame(size_t remaining_frames) override;

    // BeginFrameSourceBase
    void OnNeedsBeginFramesChange(bool needs_begin_frames) override;

    // Tracing
    void AsValueInto(base::trace_event::TracedValue* dict) const override;

protected:
    BeginFrameSourceMultiplexer();
    explicit BeginFrameSourceMultiplexer(base::TimeDelta minimum_interval);

    bool HasSource(BeginFrameSource* source);
    bool IsIncreasing(const BeginFrameArgs& args);

    base::TimeDelta minimum_interval_;

    BeginFrameSource* active_source_;
    std::set<BeginFrameSource*> source_list_;

private:
    DISALLOW_COPY_AND_ASSIGN(BeginFrameSourceMultiplexer);
};

} // namespace cc

#endif // CC_SCHEDULER_BEGIN_FRAME_SOURCE_H_
