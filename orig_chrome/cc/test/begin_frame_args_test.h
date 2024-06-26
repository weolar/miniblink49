// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_BEGIN_FRAME_ARGS_TEST_H_
#define CC_TEST_BEGIN_FRAME_ARGS_TEST_H_

#include <iosfwd>

#include "base/test/simple_test_tick_clock.h"
#include "base/time/time.h"
#include "cc/output/begin_frame_args.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

// Functions for quickly creating BeginFrameArgs
BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location);
BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    base::TimeTicks frame_time);
BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    int64 frame_time,
    int64 deadline,
    int64 interval);
BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    int64 frame_time,
    int64 deadline,
    int64 interval,
    BeginFrameArgs::BeginFrameArgsType type);
BeginFrameArgs CreateExpiredBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location);

// Creates a BeginFrameArgs using the fake Now value stored on the
// OrderSimpleTaskRunner.
BeginFrameArgs CreateBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    base::SimpleTestTickClock* now_src);
BeginFrameArgs CreateExpiredBeginFrameArgsForTesting(
    BeginFrameArgs::CreationLocation location,
    base::SimpleTestTickClock* now_src);

// gtest helpers -- these *must* be in the same namespace as the types they
// operate on.

// Allow "EXPECT_EQ(args1, args2);"
// We don't define operator!= because EXPECT_NE(args1, args2) isn't all that
// sensible.
bool operator==(const BeginFrameArgs& lhs, const BeginFrameArgs& rhs);

// Allow gtest to pretty print begin frame args.
::std::ostream& operator<<(::std::ostream& os, const BeginFrameArgs& args);
void PrintTo(const BeginFrameArgs& args, ::std::ostream* os);

} // namespace cc

#endif // CC_TEST_BEGIN_FRAME_ARGS_TEST_H_
