// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebAnimationCurveCommon_h
#define mc_blink_WebAnimationCurveCommon_h

#include "third_party/WebKit/public/platform/WebCompositorAnimationCurve.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {
class TimingFunction;
}

namespace mc_blink {

WTF::PassOwnPtr<mc::TimingFunction> createTimingFunction(blink::WebCompositorAnimationCurve::TimingFunctionType type);

}  // namespace mc_blink

#endif  // mc_blink_WebAnimationCurveCommon_h
