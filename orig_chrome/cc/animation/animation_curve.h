// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_ANIMATION_CURVE_H_
#define CC_ANIMATION_ANIMATION_CURVE_H_

#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/output/filter_operations.h"
#include "ui/gfx/transform.h"

namespace gfx {
class BoxF;
}

namespace cc {

class ColorAnimationCurve;
class FilterAnimationCurve;
class FloatAnimationCurve;
class ScrollOffsetAnimationCurve;
class TransformAnimationCurve;
class TransformOperations;

// An animation curve is a function that returns a value given a time.
class CC_EXPORT AnimationCurve {
public:
    enum CurveType { COLOR,
        FLOAT,
        TRANSFORM,
        FILTER,
        SCROLL_OFFSET };

    virtual ~AnimationCurve() { }

    virtual base::TimeDelta Duration() const = 0;
    virtual CurveType Type() const = 0;
    virtual scoped_ptr<AnimationCurve> Clone() const = 0;

    const ColorAnimationCurve* ToColorAnimationCurve() const;
    const FloatAnimationCurve* ToFloatAnimationCurve() const;
    const TransformAnimationCurve* ToTransformAnimationCurve() const;
    const FilterAnimationCurve* ToFilterAnimationCurve() const;
    const ScrollOffsetAnimationCurve* ToScrollOffsetAnimationCurve() const;

    ScrollOffsetAnimationCurve* ToScrollOffsetAnimationCurve();
};

class CC_EXPORT ColorAnimationCurve : public AnimationCurve {
public:
    ~ColorAnimationCurve() override { }

    virtual SkColor GetValue(base::TimeDelta t) const = 0;

    // Partial Animation implementation.
    CurveType Type() const override;
};

class CC_EXPORT FloatAnimationCurve : public AnimationCurve {
public:
    ~FloatAnimationCurve() override { }

    virtual float GetValue(base::TimeDelta t) const = 0;

    // Partial Animation implementation.
    CurveType Type() const override;
};

class CC_EXPORT TransformAnimationCurve : public AnimationCurve {
public:
    ~TransformAnimationCurve() override { }

    virtual gfx::Transform GetValue(base::TimeDelta t) const = 0;

    // Sets |bounds| to be the bounding box for the region within which |box|
    // will move during this animation. If this region cannot be computed,
    // returns false.
    virtual bool AnimatedBoundsForBox(const gfx::BoxF& box,
        gfx::BoxF* bounds) const = 0;

    // Returns true if this animation affects scale.
    virtual bool AffectsScale() const = 0;

    // Returns true if this animation is a translation.
    virtual bool IsTranslation() const = 0;

    // Returns true if this animation preserves axis alignment.
    virtual bool PreservesAxisAlignment() const = 0;

    // Animation start scale
    virtual bool AnimationStartScale(bool forward_direction,
        float* start_scale) const = 0;

    // Set |max_scale| to the maximum scale along any dimension at the end of
    // intermediate animation target points (eg keyframe end points). When
    // |forward_direction| is true, the animation curve assumes it plays from
    // the first keyframe to the last, otherwise it assumes the opposite. Returns
    // false if the maximum scale cannot be computed.
    virtual bool MaximumTargetScale(bool forward_direction,
        float* max_scale) const = 0;

    // Partial Animation implementation.
    CurveType Type() const override;
};

class CC_EXPORT FilterAnimationCurve : public AnimationCurve {
public:
    ~FilterAnimationCurve() override { }

    virtual FilterOperations GetValue(base::TimeDelta t) const = 0;
    virtual bool HasFilterThatMovesPixels() const = 0;

    // Partial Animation implementation.
    CurveType Type() const override;
};

} // namespace cc

#endif // CC_ANIMATION_ANIMATION_CURVE_H_
