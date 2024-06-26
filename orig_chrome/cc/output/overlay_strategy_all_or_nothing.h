// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_OVERLAY_STRATEGY_ALL_OR_NOTHING_H_
#define CC_OUTPUT_OVERLAY_STRATEGY_ALL_OR_NOTHING_H_

#include "cc/output/overlay_processor.h"

namespace cc {

class OverlayCandidateValidator;

// This strategy attempts to promote all quads into overlays.
// If any quad can't be promoted, it returns false and no quads are promoted.
class CC_EXPORT OverlayStrategyAllOrNothing
    : public OverlayProcessor::Strategy {
public:
    explicit OverlayStrategyAllOrNothing(
        OverlayCandidateValidator* capability_checker);
    ~OverlayStrategyAllOrNothing() override;

    bool Attempt(RenderPassList* render_passes,
        OverlayCandidateList* candidate_list,
        float device_scale_factor) override;

private:
    OverlayCandidateValidator* capability_checker_; // Weak.

    DISALLOW_COPY_AND_ASSIGN(OverlayStrategyAllOrNothing);
};

} // namespace cc

#endif // CC_OUTPUT_OVERLAY_STRATEGY_ALL_OR_NOTHING_H_
