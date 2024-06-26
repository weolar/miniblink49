// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/begin_frame_args_test.h"

#include "base/time/time.h"
#include "cc/output/begin_frame_args.h"

namespace cc {

BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location)
{
    return CreateBeginFrameArgsForTesting(location, base::TimeTicks::Now());
}

BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    base::TimeTicks frame_time)
{
    return BeginFrameArgs::Create(
        location, frame_time,
        frame_time + BeginFrameArgs::DefaultInterval() - BeginFrameArgs::DefaultEstimatedParentDrawTime(),
        BeginFrameArgs::DefaultInterval(), BeginFrameArgs::NORMAL);
}

BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    int64 frame_time,
    int64 deadline,
    int64 interval)
{
    return BeginFrameArgs::Create(
        location, base::TimeTicks::FromInternalValue(frame_time),
        base::TimeTicks::FromInternalValue(deadline),
        base::TimeDelta::FromInternalValue(interval), BeginFrameArgs::NORMAL);
}

BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    int64 frame_time,
    int64 deadline,
    int64 interval,
    BeginFrameArgs::BeginFrameArgsType type)
{
    return BeginFrameArgs::Create(
        location, base::TimeTicks::FromInternalValue(frame_time),
        base::TimeTicks::FromInternalValue(deadline),
        base::TimeDelta::FromInternalValue(interval), type);
}

BeginFrameArgs CreateExpiredBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location)
{
    base::TimeTicks now = base::TimeTicks::Now();
    return BeginFrameArgs::Create(
        location, now, now - BeginFrameArgs::DefaultInterval(),
        BeginFrameArgs::DefaultInterval(), BeginFrameArgs::NORMAL);
}

BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    base::SimpleTestTickClock* now_src)
{
    base::TimeTicks now = now_src->NowTicks();
    return BeginFrameArgs::Create(
        location, now, now + BeginFrameArgs::DefaultInterval() - BeginFrameArgs::DefaultEstimatedParentDrawTime(),
        BeginFrameArgs::DefaultInterval(), BeginFrameArgs::NORMAL);
}

BeginFrameArgs CreateExpiredBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    base::SimpleTestTickClock* now_src)
{
    base::TimeTicks now = now_src->NowTicks();
    return BeginFrameArgs::Create(
        location, now, now - BeginFrameArgs::DefaultInterval(),
        BeginFrameArgs::DefaultInterval(), BeginFrameArgs::NORMAL);
}

bool operator==(const BeginFrameArgs& lhs, const BeginFrameArgs& rhs)
{
    return (lhs.type == rhs.type) && (lhs.frame_time == rhs.frame_time) && (lhs.deadline == rhs.deadline) && (lhs.interval == rhs.interval);
}

::std::ostream& operator<<(::std::ostream& os, const BeginFrameArgs& args)
{
    PrintTo(args, &os);
    return os;
}

void PrintTo(const BeginFrameArgs& args, ::std::ostream* os)
{
    *os << "BeginFrameArgs(" << BeginFrameArgs::TypeToString(args.type) << ", "
        << args.frame_time.ToInternalValue() << ", "
        << args.deadline.ToInternalValue() << ", "
        << args.interval.InMicroseconds() << "us)";
}

} // namespace cc
