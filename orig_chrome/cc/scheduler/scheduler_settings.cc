// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/scheduler/scheduler_settings.h"

#include "base/trace_event/trace_event_argument.h"

namespace cc {

SchedulerSettings::SchedulerSettings()
    : use_external_begin_frame_source(false)
    , main_frame_while_swap_throttled_enabled(false)
    , main_frame_before_activation_enabled(false)
    , commit_to_active_tree(false)
    , timeout_and_draw_when_animation_checkerboards(true)
    , using_synchronous_renderer_compositor(false)
    , throttle_frame_production(true)
    , maximum_number_of_failed_draws_before_draw_is_forced(3)
    , background_frame_interval(base::TimeDelta::FromSeconds(1))
{
}

SchedulerSettings::~SchedulerSettings() { }

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
SchedulerSettings::AsValue() const
{
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();
    state->SetBoolean("use_external_begin_frame_source",
        use_external_begin_frame_source);
    state->SetBoolean("main_frame_while_swap_throttled_enabled",
        main_frame_while_swap_throttled_enabled);
    state->SetBoolean("main_frame_before_activation_enabled",
        main_frame_before_activation_enabled);
    state->SetBoolean("commit_to_active_tree", commit_to_active_tree);
    state->SetBoolean("timeout_and_draw_when_animation_checkerboards",
        timeout_and_draw_when_animation_checkerboards);
    state->SetInteger("maximum_number_of_failed_draws_before_draw_is_forced",
        maximum_number_of_failed_draws_before_draw_is_forced);
    state->SetBoolean("using_synchronous_renderer_compositor",
        using_synchronous_renderer_compositor);
    state->SetBoolean("throttle_frame_production", throttle_frame_production);
    state->SetInteger("background_frame_interval",
        background_frame_interval.InMicroseconds());
    return state;
}

} // namespace cc
