// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/AnimationCurve.h"

#include "mc/animation/ScrollOffsetAnimationCurve.h"

namespace mc {

const ColorAnimationCurve* AnimationCurve::toColorAnimationCurve() const
{
    ASSERT(type() == AnimationCurve::COLOR);
    return static_cast<const ColorAnimationCurve*>(this);
}

AnimationCurve::CurveType ColorAnimationCurve::type() const
{
    return COLOR;
}

const FloatAnimationCurve* AnimationCurve::toFloatAnimationCurve() const
{
    ASSERT(type() == AnimationCurve::FLOAT);
    return static_cast<const FloatAnimationCurve*>(this);
}

AnimationCurve::CurveType FloatAnimationCurve::type() const
{
    return FLOAT;
}

const TransformAnimationCurve* AnimationCurve::toTransformAnimationCurve() const
{
    ASSERT(type() == AnimationCurve::TRANSFORM);
    return static_cast<const TransformAnimationCurve*>(this);
}

AnimationCurve::CurveType TransformAnimationCurve::type() const
{
    return TRANSFORM;
}

const FilterAnimationCurve* AnimationCurve::toFilterAnimationCurve() const
{
    ASSERT(type() == AnimationCurve::FILTER);
    return static_cast<const FilterAnimationCurve*>(this);
}

AnimationCurve::CurveType FilterAnimationCurve::type() const
{
    return FILTER;
}

const ScrollOffsetAnimationCurve* AnimationCurve::toScrollOffsetAnimationCurve() const
{
    ASSERT(type() == AnimationCurve::SCROLL_OFFSET);
    return static_cast<const ScrollOffsetAnimationCurve*>(this);
}

ScrollOffsetAnimationCurve* AnimationCurve::toScrollOffsetAnimationCurve()
{
    ASSERT(type() == AnimationCurve::SCROLL_OFFSET);
    return static_cast<ScrollOffsetAnimationCurve*>(this);
}

}  // namespace cc
