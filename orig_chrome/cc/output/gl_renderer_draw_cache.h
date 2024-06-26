// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_GL_RENDERER_DRAW_CACHE_H_
#define CC_OUTPUT_GL_RENDERER_DRAW_CACHE_H_

#include <vector>

#include "base/basictypes.h"
#include "third_party/skia/include/core/SkColor.h"

namespace cc {

// Collects 4 floats at a time for easy upload to GL.
struct Float4 {
    float data[4];
};

// Collects 16 floats at a time for easy upload to GL.
struct Float16 {
    float data[16];
};

// A cache for storing textured quads to be drawn.  Stores the minimum required
// data to tell if two back to back draws only differ in their transform. Quads
// that only differ by transform may be coalesced into a single draw call.
struct TexturedQuadDrawCache {
    TexturedQuadDrawCache();
    ~TexturedQuadDrawCache();

    // Values tracked to determine if textured quads may be coalesced.
    int program_id;
    int resource_id;
    bool needs_blending;
    bool nearest_neighbor;
    SkColor background_color;

    // Information about the program binding that is required to draw.
    int uv_xform_location;
    int background_color_location;
    int vertex_opacity_location;
    int matrix_location;
    int sampler_location;

    // A cache for the coalesced quad data.
    std::vector<Float4> uv_xform_data;
    std::vector<float> vertex_opacity_data;
    std::vector<Float16> matrix_data;

private:
    DISALLOW_COPY_AND_ASSIGN(TexturedQuadDrawCache);
};

} // namespace cc

#endif // CC_OUTPUT_GL_RENDERER_DRAW_CACHE_H_
