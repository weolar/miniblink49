// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SCHEDULER_COMMIT_EARLYOUT_REASON_H_
#define CC_SCHEDULER_COMMIT_EARLYOUT_REASON_H_

#include "base/logging.h"

namespace cc {

enum class CommitEarlyOutReason {
    ABORTED_OUTPUT_SURFACE_LOST,
    ABORTED_NOT_VISIBLE,
    ABORTED_DEFERRED_COMMIT,
    FINISHED_NO_UPDATES,
};

inline const char* CommitEarlyOutReasonToString(CommitEarlyOutReason reason)
{
    switch (reason) {
    case CommitEarlyOutReason::ABORTED_OUTPUT_SURFACE_LOST:
        return "CommitEarlyOutReason::ABORTED_OUTPUT_SURFACE_LOST";
    case CommitEarlyOutReason::ABORTED_NOT_VISIBLE:
        return "CommitEarlyOutReason::ABORTED_NOT_VISIBLE";
    case CommitEarlyOutReason::ABORTED_DEFERRED_COMMIT:
        return "CommitEarlyOutReason::ABORTED_DEFERRED_COMMIT";
    case CommitEarlyOutReason::FINISHED_NO_UPDATES:
        return "CommitEarlyOutReason::FINISHED_NO_UPDATES";
    }
    NOTREACHED();
    return "???";
}

inline bool CommitEarlyOutHandledCommit(CommitEarlyOutReason reason)
{
    return reason == CommitEarlyOutReason::FINISHED_NO_UPDATES;
}

} // namespace cc

#endif // CC_SCHEDULER_COMMIT_EARLYOUT_REASON_H_
