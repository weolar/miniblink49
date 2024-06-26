// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_TOUCH_NOISE_FILTER_H_
#define UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_TOUCH_NOISE_FILTER_H_

#include <bitset>
#include <vector>

#include "base/time/time.h"
#include "ui/events/ozone/evdev/touch_evdev_types.h"

namespace ui {

class TouchNoiseFilter {
public:
    virtual ~TouchNoiseFilter() { }
    virtual void Filter(const std::vector<InProgressTouchEvdev>& touches,
        base::TimeDelta time,
        std::bitset<kNumTouchEvdevSlots>* slots_with_noise)
        = 0;
};

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_TOUCH_NOISE_FILTER_H_
