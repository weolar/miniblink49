// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_ANIMATION_CURVE_COMMON_H_
#define CC_BLINK_WEB_ANIMATION_CURVE_COMMON_H_

#include "base/memory/scoped_ptr.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimationCurve.h"

namespace cc {
class TimingFunction;
}

namespace cc_blink {
scoped_ptr<cc::TimingFunction> CreateTimingFunction(
    blink::WebCompositorAnimationCurve::TimingFunctionType);
} // namespace cc_blink

#endif // CC_BLINK_WEB_ANIMATION_CURVE_COMMON_H_
