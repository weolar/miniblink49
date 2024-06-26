// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_animation_curve_common.h"

#include "cc/animation/timing_function.h"

namespace cc_blink {

scoped_ptr<cc::TimingFunction> CreateTimingFunction(
    blink::WebCompositorAnimationCurve::TimingFunctionType type)
{
    switch (type) {
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEase:
        return cc::EaseTimingFunction::Create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEaseIn:
        return cc::EaseInTimingFunction::Create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEaseOut:
        return cc::EaseOutTimingFunction::Create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeEaseInOut:
        return cc::EaseInOutTimingFunction::Create();
    case blink::WebCompositorAnimationCurve::TimingFunctionTypeLinear:
        return nullptr;
    }
    return nullptr;
}

} // namespace cc_blink
