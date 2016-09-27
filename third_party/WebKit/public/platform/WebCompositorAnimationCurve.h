// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCompositorAnimationCurve_h
#define WebCompositorAnimationCurve_h

namespace blink {

class WebCompositorAnimationCurve {
public:
    virtual ~WebCompositorAnimationCurve() { }

    enum TimingFunctionType {
        TimingFunctionTypeEase,
        TimingFunctionTypeEaseIn,
        TimingFunctionTypeEaseOut,
        TimingFunctionTypeEaseInOut,
        TimingFunctionTypeLinear
    };

    enum AnimationCurveType {
        AnimationCurveTypeFilter,
        AnimationCurveTypeFloat,
        AnimationCurveTypeScrollOffset,
        AnimationCurveTypeTransform,
    };

    virtual AnimationCurveType type() const = 0;
};

} // namespace blink

#endif // WebCompositorAnimationCurve_h
