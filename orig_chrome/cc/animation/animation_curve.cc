// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_curve.h"

#include "base/logging.h"
#include "cc/animation/scroll_offset_animation_curve.h"

namespace cc {

const ColorAnimationCurve* AnimationCurve::ToColorAnimationCurve() const
{
    DCHECK(Type() == AnimationCurve::COLOR);
    return static_cast<const ColorAnimationCurve*>(this);
}

AnimationCurve::CurveType ColorAnimationCurve::Type() const
{
    return COLOR;
}

const FloatAnimationCurve* AnimationCurve::ToFloatAnimationCurve() const
{
    DCHECK(Type() == AnimationCurve::FLOAT);
    return static_cast<const FloatAnimationCurve*>(this);
}

AnimationCurve::CurveType FloatAnimationCurve::Type() const
{
    return FLOAT;
}

const TransformAnimationCurve* AnimationCurve::ToTransformAnimationCurve()
    const
{
    DCHECK(Type() == AnimationCurve::TRANSFORM);
    return static_cast<const TransformAnimationCurve*>(this);
}

AnimationCurve::CurveType TransformAnimationCurve::Type() const
{
    return TRANSFORM;
}

const FilterAnimationCurve* AnimationCurve::ToFilterAnimationCurve() const
{
    DCHECK(Type() == AnimationCurve::FILTER);
    return static_cast<const FilterAnimationCurve*>(this);
}

AnimationCurve::CurveType FilterAnimationCurve::Type() const
{
    return FILTER;
}

const ScrollOffsetAnimationCurve* AnimationCurve::ToScrollOffsetAnimationCurve()
    const
{
    DCHECK(Type() == AnimationCurve::SCROLL_OFFSET);
    return static_cast<const ScrollOffsetAnimationCurve*>(this);
}

ScrollOffsetAnimationCurve* AnimationCurve::ToScrollOffsetAnimationCurve()
{
    DCHECK(Type() == AnimationCurve::SCROLL_OFFSET);
    return static_cast<ScrollOffsetAnimationCurve*>(this);
}

} // namespace cc
