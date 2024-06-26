// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SCHEDULER_SCHEDULER_SETTINGS_H_
#define CC_SCHEDULER_SCHEDULER_SETTINGS_H_

#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "base/values.h"
#include "cc/base/cc_export.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
}
}

namespace cc {

class CC_EXPORT SchedulerSettings {
public:
    SchedulerSettings();
    ~SchedulerSettings();

    bool use_external_begin_frame_source;
    bool main_frame_while_swap_throttled_enabled;
    bool main_frame_before_activation_enabled;
    bool commit_to_active_tree;
    bool timeout_and_draw_when_animation_checkerboards;
    bool using_synchronous_renderer_compositor;
    bool throttle_frame_production;

    int maximum_number_of_failed_draws_before_draw_is_forced;
    base::TimeDelta background_frame_interval;

    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsValue() const;
};

} // namespace cc

#endif // CC_SCHEDULER_SCHEDULER_SETTINGS_H_
