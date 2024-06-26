// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationCurve_h
#define mc_animation_AnimationCurve_h

#include "base/time/time.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "mc/animation/FilterOperationsWrap.h"
#include "base/time/time.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

namespace gfx {
class BoxF;
class Transform {

};
}

namespace mc {

class ColorAnimationCurve;
class FilterAnimationCurve;
class FloatAnimationCurve;
class ScrollOffsetAnimationCurve;
class TransformAnimationCurve;
class TransformOperations;
class FloatBox;

// An animation curve is a function that returns a value given a time.
class AnimationCurve {
public:
    enum CurveType {
        COLOR, FLOAT, TRANSFORM, FILTER, SCROLL_OFFSET
    };

    virtual ~AnimationCurve() {}

    virtual base::TimeDelta duration() const = 0;
    virtual CurveType type() const = 0;
    virtual WTF::PassOwnPtr<AnimationCurve> clone() const = 0;

    const ColorAnimationCurve* toColorAnimationCurve() const;
    const FloatAnimationCurve* toFloatAnimationCurve() const;
    const TransformAnimationCurve* toTransformAnimationCurve() const;
    const FilterAnimationCurve* toFilterAnimationCurve() const;
    const ScrollOffsetAnimationCurve* toScrollOffsetAnimationCurve() const;

    ScrollOffsetAnimationCurve* toScrollOffsetAnimationCurve();
};

class ColorAnimationCurve : public AnimationCurve {
public:
    ~ColorAnimationCurve() override {}

    virtual SkColor getValue(base::TimeDelta t) const = 0;

    // Partial Animation implementation.
    CurveType type() const override;
};

class FloatAnimationCurve : public AnimationCurve {
public:
    ~FloatAnimationCurve() override {}

    virtual float getValue(base::TimeDelta t) const = 0;

    // Partial Animation implementation.
    CurveType type() const override;
};

class TransformAnimationCurve : public AnimationCurve {
public:
    ~TransformAnimationCurve() override {}

    virtual SkMatrix44 getValue(base::TimeDelta t) const = 0;

    // Sets |bounds| to be the bounding box for the region within which |box|
    // will move during this animation. If this region cannot be computed,
    // returns false.
    virtual bool animatedBoundsForBox(const FloatBox& box, FloatBox* bounds) const = 0;

    // Returns true if this animation affects scale.
    virtual bool affectsScale() const = 0;

    // Returns true if this animation is a translation.
    virtual bool isTranslation() const = 0;

    // Returns true if this animation preserves axis alignment.
    virtual bool preservesAxisAlignment() const = 0;

    // Animation start scale
    virtual bool animationStartScale(bool forward_direction, float* start_scale) const = 0;

    // Set |max_scale| to the maximum scale along any dimension at the end of
    // intermediate animation target points (eg keyframe end points). When
    // |forward_direction| is true, the animation curve assumes it plays from
    // the first keyframe to the last, otherwise it assumes the opposite. Returns
    // false if the maximum scale cannot be computed.
    virtual bool maximumTargetScale(bool forward_direction,
        float* max_scale) const = 0;

    // Partial Animation implementation.
    CurveType type() const override;
};

class FilterAnimationCurve : public AnimationCurve {
public:
    ~FilterAnimationCurve() override {}

    virtual FilterOperationsWrap getValue(base::TimeDelta t) const = 0;
    virtual bool hasFilterThatMovesPixels() const = 0;

    // Partial Animation implementation.
    CurveType type() const override;
};

}  // namespace mc

#endif  // mc_animation_AnimationCurve_h
