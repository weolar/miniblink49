// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gesture_detection/gesture_configuration.h"

#include "base/command_line.h"
#include "base/macros.h"
#include "base/memory/singleton.h"
#include "ui/events/event_switches.h"

namespace ui {
namespace {

    class GestureConfigurationAura : public GestureConfiguration {
    public:
        ~GestureConfigurationAura() override
        {
        }

        static GestureConfigurationAura* GetInstance()
        {
            return base::Singleton<GestureConfigurationAura>::get();
        }

    private:
        GestureConfigurationAura()
            : GestureConfiguration()
        {
            set_double_tap_enabled(false);
            set_double_tap_timeout_in_ms(semi_long_press_time_in_ms());
            set_gesture_begin_end_types_enabled(true);
            set_min_gesture_bounds_length(default_radius());
            set_min_pinch_update_span_delta(
                base::CommandLine::ForCurrentProcess()->HasSwitch(
                    switches::kCompensateForUnstablePinchZoom)
                    ? 5
                    : 0);
            set_min_scaling_touch_major(default_radius() * 2);
            set_velocity_tracker_strategy(VelocityTracker::Strategy::LSQ2_RESTRICTED);
            set_span_slop(max_touch_move_in_pixels_for_click() * 2);
            set_swipe_enabled(true);
            set_two_finger_tap_enabled(true);
            set_fling_touchpad_tap_suppression_enabled(true);
            set_fling_touchscreen_tap_suppression_enabled(true);
        }

        friend struct base::DefaultSingletonTraits<GestureConfigurationAura>;
        DISALLOW_COPY_AND_ASSIGN(GestureConfigurationAura);
    };

} // namespace

// Create a GestureConfigurationAura singleton instance when using aura.
GestureConfiguration* GestureConfiguration::GetPlatformSpecificInstance()
{
    return GestureConfigurationAura::GetInstance();
}

} // namespace ui
