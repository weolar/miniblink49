// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/overlay_strategy_common.h"

#include "cc/quads/solid_color_draw_quad.h"

namespace cc {

OverlayStrategyCommon::OverlayStrategyCommon(
    OverlayCandidateValidator* capability_checker,
    OverlayStrategyCommonDelegate* delegate)
    : capability_checker_(capability_checker)
    , delegate_(delegate)
{
}

OverlayStrategyCommon::~OverlayStrategyCommon()
{
}

bool OverlayStrategyCommon::Attempt(RenderPassList* render_passes_in_draw_order,
    OverlayCandidateList* candidate_list,
    float device_scale_factor)
{
    if (!capability_checker_)
        return false;
    RenderPass* root_render_pass = render_passes_in_draw_order->back();
    DCHECK(root_render_pass);

    bool created_overlay = false;
    QuadList& quad_list = root_render_pass->quad_list;
    for (auto it = quad_list.begin(); it != quad_list.end();) {
        OverlayCandidate candidate;
        if (!OverlayCandidate::FromDrawQuad(*it, &candidate)) {
            ++it;
            continue;
        }

        OverlayResult result = delegate_->TryOverlay(
            capability_checker_, render_passes_in_draw_order, candidate_list,
            candidate, &it, device_scale_factor);
        switch (result) {
        case DID_NOT_CREATE_OVERLAY:
            ++it;
            break;
        case CREATED_OVERLAY_STOP_LOOKING:
            return true;
        case CREATED_OVERLAY_KEEP_LOOKING:
            created_overlay = true;
            break;
        }
    }

    return created_overlay;
}

// static
bool OverlayStrategyCommon::IsInvisibleQuad(const DrawQuad* draw_quad)
{
    if (draw_quad->material == DrawQuad::SOLID_COLOR) {
        const SolidColorDrawQuad* solid_quad = SolidColorDrawQuad::MaterialCast(draw_quad);
        SkColor color = solid_quad->color;
        float opacity = solid_quad->shared_quad_state->opacity;
        float alpha = (SkColorGetA(color) * (1.0f / 255.0f)) * opacity;
        return solid_quad->ShouldDrawWithBlending() && alpha < std::numeric_limits<float>::epsilon();
    }
    return false;
}

} // namespace cc
