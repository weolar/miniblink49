// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebAnimationCurveCommon.h"

#include "mc/animation/TimingFunction.h"

namespace mc_blink {

WTF::PassOwnPtr<mc::TimingFunction> createTimingFunction(blink::WebCompositorAnimationCurve::TimingFunctionType type)
{
    switch (type) {
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEase:
        return mc::EaseTimingFunction::create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEaseIn:
        return mc::EaseInTimingFunction::create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEaseOut:
        return mc::EaseOutTimingFunction::create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEaseInOut:
        return mc::EaseInOutTimingFunction::create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeLinear:
        return nullptr;
    }
    return nullptr;
}

}  // namespace cc_blink
