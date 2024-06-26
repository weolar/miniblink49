// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_OVERLAY_STRATEGY_UNDERLAY_H_
#define CC_OUTPUT_OVERLAY_STRATEGY_UNDERLAY_H_

#include "cc/output/overlay_strategy_common.h"

namespace cc {
class StreamVideoDrawQuad;
class TextureDrawQuad;

// The underlay strategy looks for a video quad without regard to quads above
// it. The video is "underlaid" through a black transparent quad substituted
// for the video quad. The overlay content can then be blended in by the
// hardware under the the scene. This is only valid for overlay contents that
// are fully opaque.
class CC_EXPORT OverlayStrategyUnderlay : public OverlayStrategyCommonDelegate {
public:
    OverlayStrategyUnderlay() { }
    ~OverlayStrategyUnderlay() override;

    OverlayResult TryOverlay(OverlayCandidateValidator* capability_checker,
        RenderPassList* render_passes_in_draw_order,
        OverlayCandidateList* candidate_list,
        const OverlayCandidate& candidate,
        QuadList::Iterator* candidate_iterator,
        float device_scale_factor) override;

private:
    DISALLOW_COPY_AND_ASSIGN(OverlayStrategyUnderlay);
};

} // namespace cc

#endif // CC_OUTPUT_OVERLAY_STRATEGY_UNDERLAY_H_
