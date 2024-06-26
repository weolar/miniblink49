// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_OVERLAY_STRATEGY_COMMON_H_
#define CC_OUTPUT_OVERLAY_STRATEGY_COMMON_H_

#include "cc/base/cc_export.h"
#include "cc/output/overlay_candidate.h"
#include "cc/output/overlay_processor.h"

namespace cc {
class IOSurfaceDrawQuad;
class OverlayCandidate;
class OverlayCandidateValidator;
class StreamVideoDrawQuad;
class TextureDrawQuad;

enum OverlayResult {
    DID_NOT_CREATE_OVERLAY,
    CREATED_OVERLAY_STOP_LOOKING,
    CREATED_OVERLAY_KEEP_LOOKING,
};

class CC_EXPORT OverlayStrategyCommonDelegate {
public:
    virtual ~OverlayStrategyCommonDelegate() { }

    // Check if |candidate| can be promoted into an overlay. If so, add it to
    // |candidate_list| and update the quads in |render_passes_in_draw_order|
    // as necessary. When returning CREATED_OVERLAY_KEEP_LOOKING, |iter| is
    // updated to point to the next quad to evaluate.
    virtual OverlayResult TryOverlay(
        OverlayCandidateValidator* capability_checker,
        RenderPassList* render_passes_in_draw_order,
        OverlayCandidateList* candidate_list,
        const OverlayCandidate& candidate,
        QuadList::Iterator* iter,
        float device_scale_factor)
        = 0;
};

class CC_EXPORT OverlayStrategyCommon : public OverlayProcessor::Strategy {
public:
    explicit OverlayStrategyCommon(OverlayCandidateValidator* capability_checker,
        OverlayStrategyCommonDelegate* delegate);
    ~OverlayStrategyCommon() override;

    bool Attempt(RenderPassList* render_passes_in_draw_order,
        OverlayCandidateList* candidate_list,
        float device_scale_factor) override;

    // Returns true if |draw_quad| will not block quads underneath from becoming
    // an overlay.
    static bool IsInvisibleQuad(const DrawQuad* draw_quad);

private:
    OverlayCandidateValidator* capability_checker_;
    scoped_ptr<OverlayStrategyCommonDelegate> delegate_;

    DISALLOW_COPY_AND_ASSIGN(OverlayStrategyCommon);
};
} // namespace cc

#endif // CC_OUTPUT_OVERLAY_STRATEGY_COMMON_H_
