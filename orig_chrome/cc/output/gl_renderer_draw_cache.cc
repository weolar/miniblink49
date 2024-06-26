// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/gl_renderer_draw_cache.h"

namespace cc {

TexturedQuadDrawCache::TexturedQuadDrawCache()
    : program_id(-1)
    , resource_id(-1)
    , needs_blending(false)
    , nearest_neighbor(false)
    , background_color(0)
    , uv_xform_location(-1)
    , background_color_location(-1)
    , vertex_opacity_location(-1)
    , matrix_location(-1)
    , sampler_location(-1)
{
}

TexturedQuadDrawCache::~TexturedQuadDrawCache() { }

} // namespace cc
