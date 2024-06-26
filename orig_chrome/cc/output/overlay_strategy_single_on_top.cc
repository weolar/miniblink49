// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/overlay_strategy_single_on_top.h"

#include <limits>

#include "cc/base/math_util.h"
#include "cc/output/overlay_candidate_validator.h"
#include "cc/quads/draw_quad.h"

namespace cc {

OverlayStrategySingleOnTop::~OverlayStrategySingleOnTop() { }

OverlayResult OverlayStrategySingleOnTop::TryOverlay(
    OverlayCandidateValidator* capability_checker,
    RenderPassList* render_passes_in_draw_order,
    OverlayCandidateList* candidate_list,
    const OverlayCandidate& candidate,
    QuadList::Iterator* candidate_iterator,
    float device_scale_factor)
{
    RenderPass* root_render_pass = render_passes_in_draw_order->back();
    QuadList& quad_list = root_render_pass->quad_list;
    // Check that no prior quads overlap it.
    for (auto overlap_iter = quad_list.cbegin();
         overlap_iter != *candidate_iterator; ++overlap_iter) {
        gfx::RectF overlap_rect = MathUtil::MapClippedRect(
            overlap_iter->shared_quad_state->quad_to_target_transform,
            gfx::RectF(overlap_iter->rect));
        if (candidate.display_rect.Intersects(overlap_rect) && !OverlayStrategyCommon::IsInvisibleQuad(*overlap_iter))
            return DID_NOT_CREATE_OVERLAY;
    }

    // Add the overlay.
    OverlayCandidateList new_candidate_list = *candidate_list;
    new_candidate_list.push_back(candidate);
    new_candidate_list.back().plane_z_order = 1;

    // Check for support.
    capability_checker->CheckOverlaySupport(&new_candidate_list);

    // If the candidate can be handled by an overlay, create a pass for it.
    if (new_candidate_list.back().overlay_handled) {
        quad_list.EraseAndInvalidateAllPointers(*candidate_iterator);
        candidate_list->swap(new_candidate_list);
        return CREATED_OVERLAY_STOP_LOOKING;
    }

    return DID_NOT_CREATE_OVERLAY;
}

} // namespace cc
