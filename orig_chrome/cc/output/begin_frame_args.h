// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_BEGIN_FRAME_ARGS_H_
#define CC_OUTPUT_BEGIN_FRAME_ARGS_H_

#include "base/location.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "base/values.h"
#include "cc/base/cc_export.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
    class TracedValue;
}
}

/**
 * In debug builds we trace the creation origin of BeginFrameArgs objects. We
 * reuse the tracked_objects::Location system to do that.
 *
 * However, in release builds we don't want this as it doubles the size of the
 * BeginFrameArgs object. As well it adds a number of largish strings to the
 * binary. Despite the argument being unused, most compilers are unable to
 * optimise it away even when unused. Instead we use the BEGINFRAME_FROM_HERE
 * macro to prevent the data even getting referenced.
 */
#ifdef NDEBUG
#define BEGINFRAME_FROM_HERE nullptr
#else
#define BEGINFRAME_FROM_HERE FROM_HERE
#endif

namespace cc {

struct CC_EXPORT BeginFrameArgs {
    enum BeginFrameArgsType {
        INVALID,
        NORMAL,
        MISSED,
        // Not a real type, but used by the IPC system. Should always remain the
        // *last* value in this enum.
        BEGIN_FRAME_ARGS_TYPE_MAX,
    };
    static const char* TypeToString(BeginFrameArgsType type);

    // Creates an invalid set of values.
    BeginFrameArgs();

#ifdef NDEBUG
    typedef const void* CreationLocation;
#else
    typedef const tracked_objects::Location& CreationLocation;
    tracked_objects::Location created_from;
#endif

    // You should be able to find all instances where a BeginFrame has been
    // created by searching for "BeginFrameArgs::Create".
    // The location argument should **always** be BEGINFRAME_FROM_HERE macro.
    static BeginFrameArgs Create(CreationLocation location,
        base::TimeTicks frame_time,
        base::TimeTicks deadline,
        base::TimeDelta interval,
        BeginFrameArgsType type);

    // This is the default delta that will be used to adjust the deadline when
    // proper draw-time estimations are not yet available.
    static base::TimeDelta DefaultEstimatedParentDrawTime();

    // This is the default interval to use to avoid sprinkling the code with
    // magic numbers.
    static base::TimeDelta DefaultInterval();

    bool IsValid() const { return interval >= base::TimeDelta(); }

    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsValue() const;
    void AsValueInto(base::trace_event::TracedValue* dict) const;

    base::TimeTicks frame_time;
    base::TimeTicks deadline;
    base::TimeDelta interval;
    BeginFrameArgsType type;
    bool on_critical_path;

private:
    BeginFrameArgs(base::TimeTicks frame_time,
        base::TimeTicks deadline,
        base::TimeDelta interval,
        BeginFrameArgsType type);
};

} // namespace cc

#endif // CC_OUTPUT_BEGIN_FRAME_ARGS_H_
