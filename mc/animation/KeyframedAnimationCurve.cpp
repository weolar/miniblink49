// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>

#include "mc/animation/KeyframedAnimationCurve.h"
#include "mc/base/TimeUtil.h"
#include "mc/base/Tween.h"
#include "mc/base/FloatBox.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint3D.h"

namespace mc {

namespace {

template <class KeyframeType>
void insertKeyframe(WTF::PassOwnPtr<KeyframeType> keyframe, ScopedPtrVector<KeyframeType>* keyframes) {
    // Usually, the keyframes will be added in order, so this loop would be
    // unnecessary and we should skip it if possible.
    if (!keyframes->empty() && keyframe->getTime() < keyframes->back()->getTime()) {
        for (size_t i = 0; i < keyframes->size(); ++i) {
            if (keyframe->getTime() < keyframes->at(i)->getTime()) {
                keyframes->insert(keyframes->begin() + i, keyframe.leakPtr());
                return;
            }
        }
    }

    keyframes->push_back(keyframe.leakPtr());
}

template <typename KeyframeType>
base::TimeDelta transformedAnimationTime(
    const ScopedPtrVector<KeyframeType>& keyframes,
    TimingFunction* timingFunction,
    base::TimeDelta time
    ) 
{
    if (timingFunction) {
        base::TimeDelta startTime = keyframes.front()->getTime();
        base::TimeDelta duration = keyframes.back()->getTime() - keyframes.front()->getTime();
        double progress = TimeUtil::divide(time - startTime, duration);

        time = TimeUtil::scale(duration, timingFunction->getValue(progress)) + startTime;
    }

    return time;
}

template <typename KeyframeType>
size_t getActiveKeyframe(const ScopedPtrVector<KeyframeType>& keyframes, base::TimeDelta time)
{
    ASSERT(keyframes.size() >= 2ul);
    size_t i = 0;
    for (; i < keyframes.size() - 2; ++i) {  // Last keyframe is never active.
        if (time < keyframes[i + 1]->getTime())
            break;
    }

    return i;
}

template <typename KeyframeType>
double transformedKeyframeProgress(const ScopedPtrVector<KeyframeType>& keyframes, base::TimeDelta time, size_t i) 
{
    double progress = TimeUtil::divide(time - keyframes[i]->getTime(), keyframes[i + 1]->getTime() - keyframes[i]->getTime());

    if (keyframes[i]->getTimingFunction()) {
        progress = keyframes[i]->getTimingFunction()->getValue(progress);
    }

    return progress;
}

}  // namespace

Keyframe::Keyframe(base::TimeDelta time, WTF::PassOwnPtr<TimingFunction> timingFunction)
    : m_time(time)
    , m_timingFunction(timingFunction) 
{
}

Keyframe::~Keyframe() {}

base::TimeDelta Keyframe::getTime() const
{
    return m_time;
}

WTF::PassOwnPtr<ColorKeyframe> ColorKeyframe::create(base::TimeDelta time, SkColor value, WTF::PassOwnPtr<TimingFunction> timingFunction) 
{
    return adoptPtr(new ColorKeyframe(time, value, timingFunction));
}

ColorKeyframe::ColorKeyframe(base::TimeDelta time, SkColor value,  WTF::PassOwnPtr<TimingFunction> timingFunction)
    : Keyframe(time, timingFunction), m_value(value) {
}

ColorKeyframe::~ColorKeyframe() {}

SkColor ColorKeyframe::value() const 
{
    return m_value;
}

WTF::PassOwnPtr<ColorKeyframe> ColorKeyframe::clone() const 
{
    WTF::PassOwnPtr<TimingFunction> func(getTimingFunction() ? getTimingFunction()->clone() : nullptr);
    return ColorKeyframe::create(getTime(), value(), func);
}

WTF::PassOwnPtr<FloatKeyframe> FloatKeyframe::create(base::TimeDelta time, float value, WTF::PassOwnPtr<TimingFunction> timingFunction)
{
    return adoptPtr(new FloatKeyframe(time, value, timingFunction));
}

FloatKeyframe::FloatKeyframe(base::TimeDelta time, float value, WTF::PassOwnPtr<TimingFunction> timingFunction)
    : Keyframe(time, timingFunction), m_value(value)
{
}

FloatKeyframe::~FloatKeyframe() {}

float FloatKeyframe::value() const 
{
    return m_value;
}

WTF::PassOwnPtr<FloatKeyframe> FloatKeyframe::clone() const 
{
    WTF::PassOwnPtr<TimingFunction> func(getTimingFunction() ? getTimingFunction()->clone() : nullptr);
    return FloatKeyframe::create(getTime(), value(), func);
}

WTF::PassOwnPtr<TransformKeyframe> TransformKeyframe::create(base::TimeDelta time, const TransformOperations& value,  WTF::PassOwnPtr<TimingFunction> timingFunction)
{
    return adoptPtr(new TransformKeyframe(time, value, timingFunction));
}

TransformKeyframe::TransformKeyframe(base::TimeDelta time, const TransformOperations& value, WTF::PassOwnPtr<TimingFunction> timingFunction)
    : Keyframe(time, timingFunction), m_value(value) {
}

TransformKeyframe::~TransformKeyframe() {}

const TransformOperations& TransformKeyframe::value() const 
{
    return m_value;
}

WTF::PassOwnPtr<TransformKeyframe> TransformKeyframe::clone() const 
{
    WTF::PassOwnPtr<TimingFunction> func(getTimingFunction() ? getTimingFunction()->clone() : nullptr);
    return TransformKeyframe::create(getTime(), value(), func);
}

WTF::PassOwnPtr<FilterKeyframe> FilterKeyframe::create(base::TimeDelta time, const FilterOperationsWrap& value, WTF::PassOwnPtr<TimingFunction> timingFunction)
{
    return adoptPtr(new FilterKeyframe(time, value, timingFunction));
}

FilterKeyframe::FilterKeyframe(base::TimeDelta time, const FilterOperationsWrap& value, WTF::PassOwnPtr<TimingFunction> timingFunction)
    : Keyframe(time, timingFunction), m_value(new FilterOperationsWrap(value)) {
}

FilterKeyframe::~FilterKeyframe() {}

const FilterOperationsWrap& FilterKeyframe::value() const
{
    return *m_value;
}

WTF::PassOwnPtr<FilterKeyframe> FilterKeyframe::clone() const
{
    WTF::PassOwnPtr<TimingFunction> func(getTimingFunction() ? getTimingFunction()->clone() : nullptr);
    return FilterKeyframe::create(getTime(), value(), func);
}

WTF::PassOwnPtr<KeyframedColorAnimationCurve> KeyframedColorAnimationCurve::create()
{
    return adoptPtr(new KeyframedColorAnimationCurve);
}

KeyframedColorAnimationCurve::KeyframedColorAnimationCurve() {}

KeyframedColorAnimationCurve::~KeyframedColorAnimationCurve() {}

void KeyframedColorAnimationCurve::addKeyframe(WTF::PassOwnPtr<ColorKeyframe> keyframe) 
{
    insertKeyframe(keyframe, &m_keyframes);
}

base::TimeDelta KeyframedColorAnimationCurve::duration() const
{
    return m_keyframes.back()->getTime() - m_keyframes.front()->getTime();
}

WTF::PassOwnPtr<AnimationCurve> KeyframedColorAnimationCurve::clone() const 
{
    WTF::PassOwnPtr<KeyframedColorAnimationCurve> toReturn(KeyframedColorAnimationCurve::create());
    for (size_t i = 0; i < m_keyframes.size(); ++i)
        toReturn->addKeyframe(m_keyframes[i]->clone());

    if (m_timingFunction)
        toReturn->setTimingFunction(m_timingFunction->clone());

    return toReturn;
}

SkColor KeyframedColorAnimationCurve::getValue(base::TimeDelta t) const 
{
    if (t <= m_keyframes.front()->getTime())
        return m_keyframes.front()->value();

    if (t >= m_keyframes.back()->getTime())
        return m_keyframes.back()->value();

    t = transformedAnimationTime(m_keyframes, m_timingFunction.get(), t);
    size_t i = getActiveKeyframe(m_keyframes, t);
    double progress = transformedKeyframeProgress(m_keyframes, t, i);

    return Tween::colorValueBetween(progress, m_keyframes[i]->value(), m_keyframes[i + 1]->value());
}

// KeyframedFloatAnimationCurve

WTF::PassOwnPtr<KeyframedFloatAnimationCurve> KeyframedFloatAnimationCurve::create()
{
    return adoptPtr(new KeyframedFloatAnimationCurve);
}

KeyframedFloatAnimationCurve::KeyframedFloatAnimationCurve() {}

KeyframedFloatAnimationCurve::~KeyframedFloatAnimationCurve() {}

void KeyframedFloatAnimationCurve::addKeyframe(WTF::PassOwnPtr<FloatKeyframe> keyframe) 
{
    insertKeyframe(keyframe, &m_keyframes);
}

base::TimeDelta KeyframedFloatAnimationCurve::duration() const 
{
    return m_keyframes.back()->getTime() - m_keyframes.front()->getTime();
}

WTF::PassOwnPtr<AnimationCurve> KeyframedFloatAnimationCurve::clone() const
{
    WTF::PassOwnPtr<KeyframedFloatAnimationCurve> toReturn (KeyframedFloatAnimationCurve::create());
    for (size_t i = 0; i < m_keyframes.size(); ++i)
        toReturn->addKeyframe(m_keyframes[i]->clone());

    if (m_timingFunction)
        toReturn->setTimingFunction(m_timingFunction->clone());

    return toReturn;
}

float KeyframedFloatAnimationCurve::getValue(base::TimeDelta t) const
{
    if (t <= m_keyframes.front()->getTime())
        return m_keyframes.front()->value();

    if (t >= m_keyframes.back()->getTime())
        return m_keyframes.back()->value();

    t = transformedAnimationTime(m_keyframes, m_timingFunction.get(), t);
    size_t i = getActiveKeyframe(m_keyframes, t);
    double progress = transformedKeyframeProgress(m_keyframes, t, i);

    return m_keyframes[i]->value() + (m_keyframes[i + 1]->value() - m_keyframes[i]->value()) * progress;
}

WTF::PassOwnPtr<KeyframedTransformAnimationCurve> KeyframedTransformAnimationCurve::create()
{
    return adoptPtr(new KeyframedTransformAnimationCurve);
}

KeyframedTransformAnimationCurve::KeyframedTransformAnimationCurve() {}

KeyframedTransformAnimationCurve::~KeyframedTransformAnimationCurve() {}

void KeyframedTransformAnimationCurve::addKeyframe(WTF::PassOwnPtr<TransformKeyframe> keyframe)
{
    insertKeyframe(keyframe, &m_keyframes);
}

base::TimeDelta KeyframedTransformAnimationCurve::duration() const 
{
    return m_keyframes.back()->getTime() - m_keyframes.front()->getTime();
}

WTF::PassOwnPtr<AnimationCurve> KeyframedTransformAnimationCurve::clone() const 
{
    WTF::PassOwnPtr<KeyframedTransformAnimationCurve> toReturn(KeyframedTransformAnimationCurve::create());
    for (size_t i = 0; i < m_keyframes.size(); ++i)
        toReturn->addKeyframe(m_keyframes[i]->clone());

    if (m_timingFunction)
        toReturn->setTimingFunction(m_timingFunction->clone());

    return toReturn;
}

SkMatrix44 KeyframedTransformAnimationCurve::getValue(base::TimeDelta t) const
{
    if (t <= m_keyframes.front()->getTime())
        return m_keyframes.front()->value().apply();

    if (t >= m_keyframes.back()->getTime())
        return m_keyframes.back()->value().apply();

    t = transformedAnimationTime(m_keyframes, m_timingFunction.get(), t);
    size_t i = getActiveKeyframe(m_keyframes, t);
    double progress = transformedKeyframeProgress(m_keyframes, t, i);

    return m_keyframes[i + 1]->value().blend(m_keyframes[i]->value(), progress);
}

bool KeyframedTransformAnimationCurve::animatedBoundsForBox(const FloatBox& box, FloatBox* bounds) const
{
//     ASSERT(m_keyframes.size() >= 2ul);
//     *bounds = gfx::BoxF();
//     for (size_t i = 0; i < m_keyframes.size() - 1; ++i) {
//         gfx::BoxF bounds_for_step;
//         float min_progress = 0.0;
//         float max_progress = 1.0;
//         if (m_keyframes[i]->getTimingFunction())
//             m_keyframes[i]->getTimingFunction()->Range(&min_progress, &max_progress);
//         if (!m_keyframes[i + 1]->value().BlendedBoundsForBox(box,
//             m_keyframes[i]->value(),
//             min_progress,
//             max_progress,
//             &bounds_for_step))
//             return false;
//         bounds->Union(bounds_for_step);
//     }
    DebugBreak();
    return true;
}

bool KeyframedTransformAnimationCurve::affectsScale() const
{
    for (size_t i = 0; i < m_keyframes.size(); ++i) {
        if (m_keyframes[i]->value().affectsScale())
            return true;
    }
    return false;
}

bool KeyframedTransformAnimationCurve::preservesAxisAlignment() const
{
    for (size_t i = 0; i < m_keyframes.size(); ++i) {
        if (!m_keyframes[i]->value().preservesAxisAlignment())
            return false;
    }
    return true;
}

bool KeyframedTransformAnimationCurve::isTranslation() const
{
    for (size_t i = 0; i < m_keyframes.size(); ++i) {
        if (!m_keyframes[i]->value().isTranslation() &&
            !m_keyframes[i]->value().isIdentity())
            return false;
    }
    return true;
}

bool KeyframedTransformAnimationCurve::animationStartScale(bool forwardDirection, float* startScale) const 
{
    ASSERT(m_keyframes.size() >= 2ul);
    *startScale = 0.f;
    size_t start_location = 0;
    if (!forwardDirection) {
        start_location = m_keyframes.size() - 1;
    }

    blink::FloatPoint3D initialTargetScale;
    if (!m_keyframes[start_location]->value().scaleComponent(&initialTargetScale))
        return false;
    float start_scale_for_segment =
        fmax(std::abs(initialTargetScale.x()),
            fmax(std::abs(initialTargetScale.y()),
                std::abs(initialTargetScale.z())));
    *startScale = start_scale_for_segment;
    return true;
}

bool KeyframedTransformAnimationCurve::maximumTargetScale(bool forwardDirection, float* maxScale) const 
{
    ASSERT(m_keyframes.size() >= 2ul);
    *maxScale = 0.f;

    // If |forwardDirection| is true, then skip the first frame, otherwise
    // skip the last frame, since that is the original position in the animation.
    size_t start = 1;
    size_t end = m_keyframes.size();
    if (!forwardDirection) {
        --start;
        --end;
    }

    for (size_t i = start; i < end; ++i) {
        blink::FloatPoint3D targetScaleForSegment;
        if (!m_keyframes[i]->value().scaleComponent(&targetScaleForSegment))
            return false;
        float max_scale_for_segment = fmax(std::abs(targetScaleForSegment.x()),
                fmax(std::abs(targetScaleForSegment.y()), std::abs(targetScaleForSegment.z()))
            );
        *maxScale = fmax(*maxScale, max_scale_for_segment);
    }
    return true;
}

WTF::PassOwnPtr<KeyframedFilterAnimationCurve> KeyframedFilterAnimationCurve::create() 
{
    return adoptPtr(new KeyframedFilterAnimationCurve);
}

KeyframedFilterAnimationCurve::KeyframedFilterAnimationCurve() {}

KeyframedFilterAnimationCurve::~KeyframedFilterAnimationCurve() {}

void KeyframedFilterAnimationCurve::addKeyframe(WTF::PassOwnPtr<FilterKeyframe> keyframe) 
{
    insertKeyframe(keyframe, &m_keyframes);
}

base::TimeDelta KeyframedFilterAnimationCurve::duration() const 
{
    return m_keyframes.back()->getTime() - m_keyframes.front()->getTime();
}

WTF::PassOwnPtr<AnimationCurve> KeyframedFilterAnimationCurve::clone() const 
{
    WTF::PassOwnPtr<KeyframedFilterAnimationCurve> toReturn(KeyframedFilterAnimationCurve::create());
    for (size_t i = 0; i < m_keyframes.size(); ++i)
        toReturn->addKeyframe(m_keyframes[i]->clone());

    if (m_timingFunction)
        toReturn->setTimingFunction(m_timingFunction->clone());

    return toReturn;
}

FilterOperationsWrap KeyframedFilterAnimationCurve::getValue(base::TimeDelta t) const
{
    if (t <= m_keyframes.front()->getTime())
        return m_keyframes.front()->value();

    if (t >= m_keyframes.back()->getTime())
        return m_keyframes.back()->value();

    t = transformedAnimationTime(m_keyframes, m_timingFunction.get(), t);
    size_t i = getActiveKeyframe(m_keyframes, t);
    double progress = transformedKeyframeProgress(m_keyframes, t, i);

    return doFilterOperationsBlend(m_keyframes[i + 1]->value(), m_keyframes[i]->value(), progress);
}

bool KeyframedFilterAnimationCurve::hasFilterThatMovesPixels() const
{
    for (size_t i = 0; i < m_keyframes.size(); ++i) {
        if (m_keyframes[i]->value().ops.hasFilterThatMovesPixels()) {
            return true;
        }
    }
    return false;
}

}  // namespace mc
