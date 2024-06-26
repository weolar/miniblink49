// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/overlay_strategy_all_or_nothing.h"

#include "cc/output/overlay_candidate_validator.h"
#include "cc/quads/draw_quad.h"

namespace cc {

OverlayStrategyAllOrNothing::OverlayStrategyAllOrNothing(
    OverlayCandidateValidator* capability_checker)
    : capability_checker_(capability_checker)
{
}

OverlayStrategyAllOrNothing::~OverlayStrategyAllOrNothing() { }

bool OverlayStrategyAllOrNothing::Attempt(RenderPassList* render_passes,
    OverlayCandidateList* candidates,
    float device_scale_factor)
{
    QuadList& quad_list = render_passes->back()->quad_list;
    OverlayCandidateList new_candidates;
    int next_z_order = -1;

    for (const DrawQuad* quad : quad_list) {
        OverlayCandidate candidate;
        if (!OverlayCandidate::FromDrawQuad(quad, &candidate))
            return false;
        candidate.plane_z_order = next_z_order--;
        new_candidates.push_back(candidate);
    }

    capability_checker_->CheckOverlaySupport(&new_candidates);
    for (const OverlayCandidate& candidate : new_candidates) {
        if (!candidate.overlay_handled)
            return false;
    }

    quad_list.clear();
    candidates->swap(new_candidates);
    return true;
}

} // namespace cc
