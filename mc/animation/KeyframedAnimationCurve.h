// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_KeyframedAnimationCurve_h
#define mc_animation_KeyframedAnimationCurve_h

#include "mc/animation/AnimationCurve.h"
#include "mc/animation/TimingFunction.h"
#include "mc/animation/TransformOperations.h"
#include "mc/base/ScopedPtrVector.h"
#include "base/time/time.h"

namespace mc {

class FloatBox;

class Keyframe {
public:
    base::TimeDelta getTime() const;
    const TimingFunction* getTimingFunction() const
    {
        return m_timingFunction.get();
    }

protected:
    Keyframe(base::TimeDelta time, WTF::PassOwnPtr<TimingFunction> timingFunction);
    virtual ~Keyframe();

private:
    base::TimeDelta m_time;
    WTF::OwnPtr<TimingFunction> m_timingFunction;
};

class ColorKeyframe : public Keyframe {
public:
    static WTF::PassOwnPtr<ColorKeyframe> create(base::TimeDelta time, SkColor value, WTF::PassOwnPtr<TimingFunction> timingFunction);
    ~ColorKeyframe() override;

    SkColor value() const;

    WTF::PassOwnPtr<ColorKeyframe> clone() const;

private:
    ColorKeyframe(base::TimeDelta time, SkColor value, WTF::PassOwnPtr<TimingFunction> timingFunction);

    SkColor m_value;
};

class FloatKeyframe : public Keyframe {
public:
    static WTF::PassOwnPtr<FloatKeyframe> create(base::TimeDelta time, float value, WTF::PassOwnPtr<TimingFunction> timingFunction);
    ~FloatKeyframe() override;

    float value() const;

    WTF::PassOwnPtr<FloatKeyframe> clone() const;

private:
    FloatKeyframe(base::TimeDelta time, float value, WTF::PassOwnPtr<TimingFunction> timingFunction);

    float m_value;
};

class TransformKeyframe : public Keyframe {
public:
    static WTF::PassOwnPtr<TransformKeyframe> create(base::TimeDelta time, const TransformOperations& value, WTF::PassOwnPtr<TimingFunction> timingFunction);
    ~TransformKeyframe() override;

    const TransformOperations& value() const;

    WTF::PassOwnPtr<TransformKeyframe> clone() const;

private:
    TransformKeyframe(base::TimeDelta time,  const TransformOperations& value,  WTF::PassOwnPtr<TimingFunction> timingFunction);

    TransformOperations m_value;
};

class FilterKeyframe : public Keyframe {
public:
    static WTF::PassOwnPtr<FilterKeyframe> create(base::TimeDelta time, const FilterOperationsWrap& value, WTF::PassOwnPtr<TimingFunction> timingFunction);
    ~FilterKeyframe() override;

    const FilterOperationsWrap& value() const;

    WTF::PassOwnPtr<FilterKeyframe> clone() const;

private:
    FilterKeyframe(base::TimeDelta time, const FilterOperationsWrap& value, WTF::PassOwnPtr<TimingFunction> timingFunction);

    blink::Persistent<FilterOperationsWrap> m_value;
};

class KeyframedColorAnimationCurve : public ColorAnimationCurve {
public:
    // It is required that the keyframes be sorted by time.
    static WTF::PassOwnPtr<KeyframedColorAnimationCurve> create();

    ~KeyframedColorAnimationCurve() override;

    void addKeyframe(WTF::PassOwnPtr<ColorKeyframe> keyframe);
    void setTimingFunction(WTF::PassOwnPtr<TimingFunction> timingFunction) 
    {
        m_timingFunction = timingFunction;
    }

    // AnimationCurve implementation
    base::TimeDelta duration() const override;
    WTF::PassOwnPtr<AnimationCurve> clone() const override;

    // BackgrounColorAnimationCurve implementation
    SkColor getValue(base::TimeDelta t) const override;

private:
    KeyframedColorAnimationCurve();

    // Always sorted in order of increasing time. No two keyframes have the
    // same time.
    ScopedPtrVector<ColorKeyframe> m_keyframes;
    WTF::OwnPtr<TimingFunction> m_timingFunction;
};

class KeyframedFloatAnimationCurve : public FloatAnimationCurve {
public:
    // It is required that the keyframes be sorted by time.
    static WTF::PassOwnPtr<KeyframedFloatAnimationCurve> create();

    ~KeyframedFloatAnimationCurve() override;

    void addKeyframe(WTF::PassOwnPtr<FloatKeyframe> keyframe);
    void setTimingFunction(WTF::PassOwnPtr<TimingFunction> timingFunction)
    {
        m_timingFunction = timingFunction;
    }

    // AnimationCurve implementation
    base::TimeDelta duration() const override;
    WTF::PassOwnPtr<AnimationCurve> clone() const override;

    // FloatAnimationCurve implementation
    float getValue(base::TimeDelta t) const override;

private:
    KeyframedFloatAnimationCurve();

    // Always sorted in order of increasing time. No two keyframes have the
    // same time.
    ScopedPtrVector<FloatKeyframe> m_keyframes;
    WTF::OwnPtr<TimingFunction> m_timingFunction;
};

class KeyframedTransformAnimationCurve : public TransformAnimationCurve {
public:
    // It is required that the keyframes be sorted by time.
    static WTF::PassOwnPtr<KeyframedTransformAnimationCurve> create();

    ~KeyframedTransformAnimationCurve() override;

    void addKeyframe(WTF::PassOwnPtr<TransformKeyframe> keyframe);
    void setTimingFunction(WTF::PassOwnPtr<TimingFunction> timingFunction) {
        m_timingFunction = timingFunction;
    }

    // AnimationCurve implementation
    base::TimeDelta duration() const override;
    WTF::PassOwnPtr<AnimationCurve> clone() const override;

    // TransformAnimationCurve implementation
    SkMatrix44 getValue(base::TimeDelta t) const override;
    bool animatedBoundsForBox(const FloatBox& box, FloatBox* bounds) const override;
    bool affectsScale() const override;
    bool preservesAxisAlignment() const override;
    bool isTranslation() const override;
    bool animationStartScale(bool forwardDirection, float* startScale) const override;
    bool maximumTargetScale(bool forwardDirection, float* maxScale) const override;

private:
    KeyframedTransformAnimationCurve();

    // Always sorted in order of increasing time. No two keyframes have the
    // same time.
    ScopedPtrVector<TransformKeyframe> m_keyframes;
    WTF::OwnPtr<TimingFunction> m_timingFunction;
};

class KeyframedFilterAnimationCurve : public FilterAnimationCurve {
public:
    // It is required that the keyframes be sorted by time.
    static WTF::PassOwnPtr<KeyframedFilterAnimationCurve> create();

    ~KeyframedFilterAnimationCurve() override;

    void addKeyframe(WTF::PassOwnPtr<FilterKeyframe> keyframe);
    void setTimingFunction(WTF::PassOwnPtr<TimingFunction> timingFunction)
    {
        m_timingFunction = timingFunction;
    }

    // AnimationCurve implementation
    base::TimeDelta duration() const override;
    WTF::PassOwnPtr<AnimationCurve> clone() const override;

    // FilterAnimationCurve implementation
    FilterOperationsWrap getValue(base::TimeDelta t) const override;
    bool hasFilterThatMovesPixels() const override;

private:
    KeyframedFilterAnimationCurve();

    // Always sorted in order of increasing time. No two keyframes have the
    // same time.
    ScopedPtrVector<FilterKeyframe> m_keyframes;
    WTF::OwnPtr<TimingFunction> m_timingFunction;
};

}  // namespace mc

#endif  // mc_animation_KeyframedAnimationCurve.h
