// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAPTURE_FEEDBACK_SIGNAL_ACCUMULATOR_H_
#define MEDIA_CAPTURE_FEEDBACK_SIGNAL_ACCUMULATOR_H_

#include "base/time/time.h"
#include "media/base/media_export.h"

namespace media {

// Utility class for maintaining an exponentially-decaying average of feedback
// signal values whose updates occur at undetermined, possibly irregular time
// intervals.
//
// Feedback signals can be made by multiple sources.  Meaning, there can be
// several values provided for the same timestamp.  In this case, the greatest
// value is retained and used to re-compute the average.  Therefore, the values
// provided to this class' methods should be appropriately translated with this
// in mind.  For example, an "fraction available" metric should be translated
// into a "fraction utilized" one.
//
// Usage note: Reset() must be called at least once before the first call to
// Update().
class MEDIA_EXPORT FeedbackSignalAccumulator {
public:
    // |half_life| is the amount of time that must pass between two data points to
    // move the accumulated average value halfway in-between.  Example: If
    // |half_life| is one second, then calling Reset(0.0, t=0s) and then
    // Update(1.0, t=1s) will result in an accumulated average value of 0.5.
    explicit FeedbackSignalAccumulator(base::TimeDelta half_life);

    // Erase all memory of historical values, re-starting with the given
    // |starting_value|.
    void Reset(double starting_value, base::TimeTicks timestamp);
    base::TimeTicks reset_time() const { return reset_time_; }

    // Apply the given |value|, which was observed at the given |timestamp|, to
    // the accumulated average.  If the timestamp is in chronological order, the
    // update succeeds and this method returns true.  Otherwise the update has no
    // effect and false is returned.  If there are two or more updates at the same
    // |timestamp|, only the one with the greatest value will be accounted for
    // (see class comments for elaboration).
    bool Update(double value, base::TimeTicks timestamp);
    base::TimeTicks update_time() const { return update_time_; }

    // Returns the current accumulated average value.
    double current() const { return average_; }

private:
    // In conjunction with the |update_time_| and |prior_update_time_|, this is
    // used to compute the weight of the current update value versus the prior
    // accumulated average.
    const base::TimeDelta half_life_;

    base::TimeTicks reset_time_; // |timestamp| passed in last call to Reset().
    double average_; // Current accumulated average.
    double update_value_; // Latest |value| accepted by Update().
    base::TimeTicks update_time_; // Latest |timestamp| accepted by Update().
    double prior_average_; // Accumulated average before last call to Update().
    base::TimeTicks prior_update_time_; // |timestamp| in prior call to Update().
};

} // namespace media

#endif // MEDIA_CAPTURE_FEEDBACK_SIGNAL_ACCUMULATOR_H_
