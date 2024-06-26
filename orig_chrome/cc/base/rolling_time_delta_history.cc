// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>

#include "cc/base/rolling_time_delta_history.h"

namespace cc {

RollingTimeDeltaHistory::RollingTimeDeltaHistory(size_t max_size)
    : max_size_(max_size)
{
}

RollingTimeDeltaHistory::~RollingTimeDeltaHistory() { }

void RollingTimeDeltaHistory::InsertSample(base::TimeDelta time)
{
    if (max_size_ == 0)
        return;

    if (sample_set_.size() == max_size_) {
        sample_set_.erase(chronological_sample_deque_.front());
        chronological_sample_deque_.pop_front();
    }

    TimeDeltaMultiset::iterator it = sample_set_.insert(time);
    chronological_sample_deque_.push_back(it);
}

void RollingTimeDeltaHistory::Clear()
{
    chronological_sample_deque_.clear();
    sample_set_.clear();
}

base::TimeDelta RollingTimeDeltaHistory::Percentile(double percent) const
{
    if (sample_set_.size() == 0)
        return base::TimeDelta();

    double fraction = percent / 100.0;

    if (fraction <= 0.0)
        return *(sample_set_.begin());

    if (fraction >= 1.0)
        return *(sample_set_.rbegin());

    size_t num_smaller_samples = static_cast<size_t>(std::ceil(fraction * sample_set_.size())) - 1;

    if (num_smaller_samples > sample_set_.size() / 2) {
        size_t num_larger_samples = sample_set_.size() - num_smaller_samples - 1;
        TimeDeltaMultiset::const_reverse_iterator it = sample_set_.rbegin();
        for (size_t i = 0; i < num_larger_samples; i++)
            it++;
        return *it;
    }

    TimeDeltaMultiset::const_iterator it = sample_set_.begin();
    for (size_t i = 0; i < num_smaller_samples; i++)
        it++;
    return *it;
}

} // namespace cc
