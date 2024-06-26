// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/draw_properties.h"

namespace cc {

DrawProperties::DrawProperties()
    : opacity(0.f)
    , screen_space_transform_is_animating(false)
    , can_use_lcd_text(false)
    , is_clipped(false)
    , render_target(nullptr)
    , num_unclipped_descendants(0)
    , has_child_with_a_scroll_parent(false)
    , last_drawn_render_surface_layer_list_id(0)
    , maximum_animation_contents_scale(0.f)
    , starting_animation_contents_scale(0.f)
{
}

DrawProperties::~DrawProperties() { }

} // namespace cc
