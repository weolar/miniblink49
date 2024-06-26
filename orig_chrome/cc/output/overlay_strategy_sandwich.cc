// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/overlay_strategy_sandwich.h"

#include "cc/base/math_util.h"
#include "cc/base/region.h"
#include "cc/output/overlay_candidate_validator.h"
#include "cc/quads/draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"

namespace cc {

namespace {

    void ClipDisplayAndUVRects(gfx::Rect* display_rect,
        gfx::RectF* uv_rect,
        const gfx::Rect& clip_rect)
    {
        gfx::Rect display_cropped_rect = gfx::IntersectRects(*display_rect, clip_rect);

        gfx::RectF uv_cropped_rect = gfx::RectF(display_cropped_rect);
        uv_cropped_rect -= gfx::Vector2dF(display_rect->x(), display_rect->y());
        uv_cropped_rect.Scale(uv_rect->width() / display_rect->width(),
            uv_rect->height() / display_rect->height());
        uv_cropped_rect += gfx::Vector2dF(uv_rect->x(), uv_rect->y());

        *display_rect = display_cropped_rect;
        *uv_rect = uv_cropped_rect;
    }

} // namespace

OverlayStrategySandwich::~OverlayStrategySandwich() { }

OverlayResult OverlayStrategySandwich::TryOverlay(
    OverlayCandidateValidator* capability_checker,
    RenderPassList* render_passes_in_draw_order,
    OverlayCandidateList* candidate_list,
    const OverlayCandidate& candidate,
    QuadList::Iterator* candidate_iter_in_quad_list,
    float device_scale_factor)
{
    RenderPass* root_render_pass = render_passes_in_draw_order->back();
    QuadList& quad_list = root_render_pass->quad_list;
    gfx::Rect pixel_bounds = root_render_pass->output_rect;

    const DrawQuad* candidate_quad = **candidate_iter_in_quad_list;
    const gfx::Transform& candidate_transform = candidate_quad->shared_quad_state->quad_to_target_transform;
    gfx::Transform candidate_inverse_transform;
    if (!candidate_transform.GetInverse(&candidate_inverse_transform))
        return DID_NOT_CREATE_OVERLAY;

    // Compute the candidate's rect in display space (pixels on the screen).
    gfx::Rect candidate_pixel_rect = candidate.quad_rect_in_target_space;
    gfx::RectF candidate_uv_rect = candidate.uv_rect;
    if (candidate.is_clipped && !candidate.clip_rect.Contains(candidate_pixel_rect)) {
        ClipDisplayAndUVRects(&candidate_pixel_rect, &candidate_uv_rect,
            candidate.clip_rect);
    }

    // Don't allow overlapping overlays for now.
    for (const OverlayCandidate& other_candidate : *candidate_list) {
        if (other_candidate.display_rect.Intersects(candidate.display_rect) && other_candidate.plane_z_order == 1) {
            return DID_NOT_CREATE_OVERLAY;
        }
    }

    // Iterate through the quads in front of |candidate|, and compute the region
    // of |candidate| that is covered.
    Region pixel_covered_region;
    for (auto overlap_iter = quad_list.cbegin();
         overlap_iter != *candidate_iter_in_quad_list; ++overlap_iter) {
        if (OverlayStrategyCommon::IsInvisibleQuad(*overlap_iter))
            continue;
        // Compute the quad's bounds in display space.
        gfx::Rect pixel_covered_rect = MathUtil::MapEnclosingClippedRect(
            overlap_iter->shared_quad_state->quad_to_target_transform,
            overlap_iter->rect);

        // Include the intersection of that quad with the candidate's quad in the
        // covered region.
        pixel_covered_rect.Intersect(candidate_pixel_rect);
        pixel_covered_region.Union(pixel_covered_rect);
    }

    // Add the candidate's overlay.
    DCHECK(candidate.resource_id);
    OverlayCandidateList new_candidate_list = *candidate_list;
    new_candidate_list.push_back(candidate);
    OverlayCandidate& new_candidate = new_candidate_list.back();
    new_candidate.plane_z_order = 1;
    new_candidate.display_rect = gfx::RectF(candidate_pixel_rect);
    new_candidate.quad_rect_in_target_space = candidate_pixel_rect;
    new_candidate.uv_rect = candidate_uv_rect;

    // Add an overlay of the primary surface for any part of the candidate's
    // quad that was covered.
    std::vector<gfx::Rect> pixel_covered_rects;
    for (Region::Iterator it(pixel_covered_region); it.has_rect(); it.next())
        pixel_covered_rects.push_back(it.rect());
    for (const gfx::Rect& pixel_covered_rect : pixel_covered_rects) {
        OverlayCandidate main_image_on_top;
        main_image_on_top.display_rect = gfx::RectF(pixel_covered_rect);
        main_image_on_top.uv_rect = gfx::RectF(pixel_covered_rect);
        main_image_on_top.uv_rect.Scale(1.f / pixel_bounds.width(),
            1.f / pixel_bounds.height());
        main_image_on_top.plane_z_order = 2;
        main_image_on_top.transform = gfx::OVERLAY_TRANSFORM_NONE;
        main_image_on_top.use_output_surface_for_resource = true;
        new_candidate_list.push_back(main_image_on_top);
    }

    // Check for support.
    capability_checker->CheckOverlaySupport(&new_candidate_list);
    for (const OverlayCandidate& candidate : new_candidate_list) {
        if (!candidate.overlay_handled)
            return DID_NOT_CREATE_OVERLAY;
    }

    // Remove the quad for the overlay quad. Replace it with a transparent quad
    // if we're putting a new overlay on top.
    if (pixel_covered_rects.empty()) {
        *candidate_iter_in_quad_list = quad_list.EraseAndInvalidateAllPointers(*candidate_iter_in_quad_list);
    } else {
        // Cache the information from the candidate quad that we'll need to
        // construct the solid color quads.
        const SharedQuadState* candidate_shared_quad_state = candidate_quad->shared_quad_state;
        const gfx::Rect candidate_rect = candidate_quad->rect;

        // Reserve space in the quad list for the transparent quads.
        quad_list.ReplaceExistingElement<SolidColorDrawQuad>(
            *candidate_iter_in_quad_list);
        *candidate_iter_in_quad_list = quad_list.InsertBeforeAndInvalidateAllPointers<SolidColorDrawQuad>(
            *candidate_iter_in_quad_list, pixel_covered_rects.size() - 1);

        // Cover the region with transparent quads.
        for (const gfx::Rect& pixel_covered_rect : pixel_covered_rects) {
            gfx::Rect quad_space_covered_rect = MathUtil::MapEnclosingClippedRect(
                candidate_inverse_transform, pixel_covered_rect);
            quad_space_covered_rect.Intersect(candidate_rect);

            SolidColorDrawQuad* transparent_quad = static_cast<SolidColorDrawQuad*>(**candidate_iter_in_quad_list);
            transparent_quad->SetAll(candidate_shared_quad_state,
                quad_space_covered_rect, quad_space_covered_rect,
                quad_space_covered_rect, false,
                SK_ColorTRANSPARENT, true);
            ++(*candidate_iter_in_quad_list);
        }
    }

    candidate_list->swap(new_candidate_list);
    return CREATED_OVERLAY_KEEP_LOOKING;
}

} // namespace cc
