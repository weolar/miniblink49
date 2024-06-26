// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebAnimationImpl.h"

#include "mc/animation/AnimationObj.h"
#include "mc/animation/AnimationCurve.h"
#include "mc/animation/AnimationIdProvider.h"
#include "mc/blink/WebFilterAnimationCurveImpl.h"
#include "mc/blink/WebFloatAnimationCurveImpl.h"
#include "mc/blink/WebScrollOffsetAnimationCurveImpl.h"
#include "mc/blink/WebTransformAnimationCurveImpl.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimationCurve.h"

using mc::AnimationObj;
using mc::AnimationIdProvider;

using blink::WebCompositorAnimation;
using blink::WebCompositorAnimationCurve;

namespace mc_blink {

WebCompositorAnimationImpl::WebCompositorAnimationImpl(const WebCompositorAnimationCurve& web_curve, TargetProperty target_property, int animation_id, int group_id) 
{
    if (!animation_id)
        animation_id = AnimationIdProvider::nextAnimationId();
    if (!group_id)
        group_id = AnimationIdProvider::nextGroupId();

    WebCompositorAnimationCurve::AnimationCurveType curveType = web_curve.type();
    WTF::OwnPtr<mc::AnimationCurve> curve;
    switch (curveType) {
        case WebCompositorAnimationCurve::AnimationCurveTypeFloat: {
            const WebFloatAnimationCurveImpl* floatCurveImpl = static_cast<const WebFloatAnimationCurveImpl*>(&web_curve);
            curve = floatCurveImpl->cloneToAnimationCurve();
            break;
        }
        case WebCompositorAnimationCurve::AnimationCurveTypeTransform: {
            const WebTransformAnimationCurveImpl* transformCurveImpl = static_cast<const WebTransformAnimationCurveImpl*>(&web_curve);
            curve = transformCurveImpl->cloneToAnimationCurve();
            break;
        }
        case WebCompositorAnimationCurve::AnimationCurveTypeFilter: {
            const WebFilterAnimationCurveImpl* filterCurveImpl = static_cast<const WebFilterAnimationCurveImpl*>(&web_curve);
            curve = filterCurveImpl->cloneToAnimationCurve();
            break;
        }
        case WebCompositorAnimationCurve::AnimationCurveTypeScrollOffset: {
    //         const WebScrollOffsetAnimationCurveImpl* scroll_curve_impl =
    //             static_cast<const WebScrollOffsetAnimationCurveImpl*>(&web_curve);
    //         curve = scroll_curve_impl->cloneToAnimationCurve();
            DebugBreak();
            break;
        }
    }
    m_animation = AnimationObj::create(curve.release(), animation_id, group_id, static_cast<mc::AnimationObj::TargetProperty>(target_property));
}

WebCompositorAnimationImpl::~WebCompositorAnimationImpl() 
{
}

int WebCompositorAnimationImpl::id() 
{
    return m_animation->id();
}

int WebCompositorAnimationImpl::group()
{
    return m_animation->group();
}

blink::WebCompositorAnimation::TargetProperty WebCompositorAnimationImpl::targetProperty() const
{
    return static_cast<WebCompositorAnimationImpl::TargetProperty>(m_animation->getTargetProperty());
}

double WebCompositorAnimationImpl::iterations() const 
{
    return m_animation->iterations();
}

void WebCompositorAnimationImpl::setIterations(double n) 
{
    m_animation->setIterations(n);
}

double WebCompositorAnimationImpl::iterationStart() const 
{
    return m_animation->iterationStart();
}

void WebCompositorAnimationImpl::setIterationStart(double iteration_start)
{
    m_animation->setIterationStart(iteration_start);
}

double WebCompositorAnimationImpl::startTime() const 
{
    return (m_animation->startTime() - base::TimeTicks()).InSecondsF();
}

void WebCompositorAnimationImpl::setStartTime(double monotonic_time) 
{
    m_animation->setStartTime(base::TimeTicks::FromInternalValue(monotonic_time * base::Time::kMicrosecondsPerSecond));
}

double WebCompositorAnimationImpl::timeOffset() const 
{
    return m_animation->timeOffset().InSecondsF();
}

void WebCompositorAnimationImpl::setTimeOffset(double monotonic_time)
{
    m_animation->setTimeOffset(base::TimeDelta::FromSecondsD(monotonic_time));
}

blink::WebCompositorAnimation::Direction WebCompositorAnimationImpl::direction() const
{
    switch (m_animation->direction()) {
    case mc::AnimationObj::DIRECTION_NORMAL:
        return DirectionNormal;
    case mc::AnimationObj::DIRECTION_REVERSE:
        return DirectionReverse;
    case mc::AnimationObj::DIRECTION_ALTERNATE:
        return DirectionAlternate;
    case mc::AnimationObj::DIRECTION_ALTERNATE_REVERSE:
        return DirectionAlternateReverse;
    default:
        DebugBreak();
    }
    return DirectionNormal;
}

void WebCompositorAnimationImpl::setDirection(Direction direction) 
{
    switch (direction) {
    case DirectionNormal:
        m_animation->setDirection(mc::AnimationObj::DIRECTION_NORMAL);
        break;
    case DirectionReverse:
        m_animation->setDirection(mc::AnimationObj::DIRECTION_REVERSE);
        break;
    case DirectionAlternate:
        m_animation->setDirection(mc::AnimationObj::DIRECTION_ALTERNATE);
        break;
    case DirectionAlternateReverse:
        m_animation->setDirection(mc::AnimationObj::DIRECTION_ALTERNATE_REVERSE);
        break;
    }
}

double WebCompositorAnimationImpl::playbackRate() const 
{
    return m_animation->playbackRate();
}

void WebCompositorAnimationImpl::setPlaybackRate(double playback_rate) 
{
    m_animation->setPlaybackRate(playback_rate);
}

blink::WebCompositorAnimation::FillMode WebCompositorAnimationImpl::fillMode() const 
{
    switch (m_animation->fillMode()) {
    case mc::AnimationObj::FILL_MODE_NONE:
        return FillModeNone;
    case mc::AnimationObj::FILL_MODE_FORWARDS:
        return FillModeForwards;
    case mc::AnimationObj::FILL_MODE_BACKWARDS:
        return FillModeBackwards;
    case mc::AnimationObj::FILL_MODE_BOTH:
        return FillModeBoth;
    default:
        DebugBreak();
    }
    return FillModeNone;
}

void WebCompositorAnimationImpl::setFillMode(FillMode fill_mode) {
    switch (fill_mode) {
    case FillModeNone:
        m_animation->setFillMode(mc::AnimationObj::FILL_MODE_NONE);
        break;
    case FillModeForwards:
        m_animation->setFillMode(mc::AnimationObj::FILL_MODE_FORWARDS);
        break;
    case FillModeBackwards:
        m_animation->setFillMode(mc::AnimationObj::FILL_MODE_BACKWARDS);
        break;
    case FillModeBoth:
        m_animation->setFillMode(mc::AnimationObj::FILL_MODE_BOTH);
        break;
    }
}

PassOwnPtr<mc::AnimationObj> WebCompositorAnimationImpl::passAnimation()
{
    m_animation->setNeedsSynchronizedStartTime(true);
    return m_animation.release();
}

}  // namespace mc_blink
