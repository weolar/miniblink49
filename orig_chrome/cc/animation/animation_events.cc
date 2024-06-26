// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_events.h"

namespace cc {

AnimationEvent::AnimationEvent(AnimationEvent::Type type,
    int layer_id,
    int group_id,
    Animation::TargetProperty target_property,
    base::TimeTicks monotonic_time)
    : type(type)
    , layer_id(layer_id)
    , group_id(group_id)
    , target_property(target_property)
    , monotonic_time(monotonic_time)
    , is_impl_only(false)
    , opacity(0.f)
{
}

} // namespace cc
