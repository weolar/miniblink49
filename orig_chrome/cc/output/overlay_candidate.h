// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_OVERLAY_CANDIDATE_H_
#define CC_OUTPUT_OVERLAY_CANDIDATE_H_

#include <vector>

#include "cc/base/cc_export.h"
#include "cc/resources/resource_format.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/overlay_transform.h"
#include "ui/gfx/transform.h"

namespace gfx {
class Rect;
}

namespace cc {

class DrawQuad;
class IOSurfaceDrawQuad;
class StreamVideoDrawQuad;
class TextureDrawQuad;

class CC_EXPORT OverlayCandidate {
public:
    // Returns true and fills in |candidate| if |draw_quad| is of a known quad
    // type and contains an overlayable resource.
    static bool FromDrawQuad(const DrawQuad* quad, OverlayCandidate* candidate);
    OverlayCandidate();
    ~OverlayCandidate();

    // Transformation to apply to layer during composition.
    gfx::OverlayTransform transform;
    // Format of the buffer to composite.
    ResourceFormat format;
    // Size of the resource, in pixels.
    gfx::Size resource_size_in_pixels;
    // Rect on the display to position the overlay to. Implementer must convert
    // to integer coordinates if setting |overlay_handled| to true.
    gfx::RectF display_rect;
    // Crop within the buffer to be placed inside |display_rect|.
    gfx::RectF uv_rect;
    // Quad geometry rect after applying the quad_transform().
    gfx::Rect quad_rect_in_target_space;
    // Clip rect in the target content space after composition.
    gfx::Rect clip_rect;
    // If the quad is clipped after composition.
    bool is_clipped;
    // True if the texture for this overlay should be the same one used by the
    // output surface's main overlay.
    bool use_output_surface_for_resource;
    // Texture resource to present in an overlay.
    unsigned resource_id;
    // Stacking order of the overlay plane relative to the main surface,
    // which is 0. Signed to allow for "underlays".
    int plane_z_order;

    // To be modified by the implementer if this candidate can go into
    // an overlay.
    bool overlay_handled;

private:
    static bool FromTextureQuad(const TextureDrawQuad* quad,
        OverlayCandidate* candidate);
    static bool FromStreamVideoQuad(const StreamVideoDrawQuad* quad,
        OverlayCandidate* candidate);
    static bool FromIOSurfaceQuad(const IOSurfaceDrawQuad* quad,
        OverlayCandidate* candidate);
};

typedef std::vector<OverlayCandidate> OverlayCandidateList;

} // namespace cc

#endif // CC_OUTPUT_OVERLAY_CANDIDATE_H_
