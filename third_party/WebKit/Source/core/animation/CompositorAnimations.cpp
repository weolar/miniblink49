/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/animation/CompositorAnimations.h"

#include "core/animation/AnimationEffect.h"
#include "core/animation/AnimationTranslationUtil.h"
#include "core/animation/CompositorAnimationsImpl.h"
#include "core/animation/ElementAnimations.h"
#include "core/animation/animatable/AnimatableDouble.h"
#include "core/animation/animatable/AnimatableFilterOperations.h"
#include "core/animation/animatable/AnimatableTransform.h"
#include "core/animation/animatable/AnimatableValue.h"
#include "core/layout/LayoutBoxModelObject.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/geometry/FloatBox.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositorAnimation.h"
#include "public/platform/WebCompositorAnimationPlayer.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebFilterAnimationCurve.h"
#include "public/platform/WebFilterKeyframe.h"
#include "public/platform/WebFloatAnimationCurve.h"
#include "public/platform/WebFloatKeyframe.h"
#include "public/platform/WebTransformAnimationCurve.h"
#include "public/platform/WebTransformKeyframe.h"

#include <algorithm>
#include <cmath>

namespace blink {

namespace {

void getKeyframeValuesForProperty(const KeyframeEffectModelBase* effect, PropertyHandle property, double scale, PropertySpecificKeyframeVector& values)
{
    ASSERT(values.isEmpty());

    for (const auto& keyframe : effect->getPropertySpecificKeyframes(property)) {
        double offset = keyframe->offset() * scale;
        values.append(keyframe->cloneWithOffset(offset));
    }
}

bool considerAnimationAsIncompatible(const Animation& animation, const Animation& animationToAdd)
{
    if (&animation == &animationToAdd)
        return false;

    switch (animation.playStateInternal()) {
    case Animation::Idle:
        return false;
    case Animation::Pending:
    case Animation::Running:
        return true;
    case Animation::Paused:
    case Animation::Finished:
        return Animation::hasLowerPriority(&animationToAdd, &animation);
    default:
        ASSERT_NOT_REACHED();
        return true;
    }
}

bool isTransformRelatedCSSProperty(const PropertyHandle property)
{
    return property.isCSSProperty()
        && (property.cssProperty() == CSSPropertyRotate
        || property.cssProperty() == CSSPropertyScale
        || property.cssProperty() == CSSPropertyTransform
        || property.cssProperty() == CSSPropertyTranslate);
}


bool isTransformRelatedAnimation(const Element& targetElement, const Animation* animation)
{
    return animation->affects(targetElement, CSSPropertyTransform)
        || animation->affects(targetElement, CSSPropertyRotate)
        || animation->affects(targetElement, CSSPropertyScale)
        || animation->affects(targetElement, CSSPropertyTranslate);
}

bool hasIncompatibleAnimations(const Element& targetElement, const Animation& animationToAdd, const EffectModel& effectToAdd)
{
    const bool affectsOpacity = effectToAdd.affects(PropertyHandle(CSSPropertyOpacity));
    const bool affectsTransform = effectToAdd.isTransformRelatedEffect();
    const bool affectsFilter = effectToAdd.affects(PropertyHandle(CSSPropertyWebkitFilter));

    if (!targetElement.hasAnimations())
        return false;

    ElementAnimations* elementAnimations = targetElement.elementAnimations();
    ASSERT(elementAnimations);

    for (const auto& entry : elementAnimations->animations()) {
        const Animation* attachedAnimation = entry.key;
        if (!considerAnimationAsIncompatible(*attachedAnimation, animationToAdd))
            continue;

        if ((affectsOpacity && attachedAnimation->affects(targetElement, CSSPropertyOpacity))
            || (affectsTransform && isTransformRelatedAnimation(targetElement, attachedAnimation))
            || (affectsFilter && attachedAnimation->affects(targetElement, CSSPropertyWebkitFilter)))
            return true;
    }

    return false;
}

}

bool CompositorAnimations::isCompositableProperty(CSSPropertyID property)
{
    for (CSSPropertyID id : compositableProperties) {
        if (property == id)
            return true;
    }
    return false;
}

const CSSPropertyID CompositorAnimations::compositableProperties[6] = {
    CSSPropertyOpacity,
    CSSPropertyRotate,
    CSSPropertyScale,
    CSSPropertyTransform,
    CSSPropertyTranslate,
    CSSPropertyWebkitFilter
};

bool CompositorAnimations::getAnimatedBoundingBox(FloatBox& box, const EffectModel& effect, double minValue, double maxValue) const
{
    const KeyframeEffectModelBase& keyframeEffect = toKeyframeEffectModelBase(effect);

    PropertyHandleSet properties = keyframeEffect.properties();

    if (properties.isEmpty())
        return true;

    minValue = std::min(minValue, 0.0);
    maxValue = std::max(maxValue, 1.0);

    for (const auto& property : properties) {
        if (!property.isCSSProperty())
            continue;

        // TODO: Add the ability to get expanded bounds for filters as well.
        if (!isTransformRelatedCSSProperty(property))
            continue;

        const PropertySpecificKeyframeVector& frames = keyframeEffect.getPropertySpecificKeyframes(property);
        if (frames.isEmpty() || frames.size() < 2)
            continue;

        FloatBox originalBox(box);

        for (size_t j = 0; j < frames.size() - 1; ++j) {
            const AnimatableTransform* startTransform = toAnimatableTransform(frames[j]->getAnimatableValue().get());
            const AnimatableTransform* endTransform = toAnimatableTransform(frames[j+1]->getAnimatableValue().get());
            if (!startTransform || !endTransform)
                return false;

            // TODO: Add support for inflating modes other than Replace.
            if (frames[j]->composite() != EffectModel::CompositeReplace)
                return false;

            const TimingFunction& timing = frames[j]->easing();
            double min = 0;
            double max = 1;
            if (j == 0) {
                float frameLength = frames[j+1]->offset();
                if (frameLength > 0) {
                    min = minValue / frameLength;
                }
            }

            if (j == frames.size() - 2) {
                float frameLength = frames[j+1]->offset() - frames[j]->offset();
                if (frameLength > 0) {
                    max = 1 + (maxValue - 1) / frameLength;
                }
            }

            FloatBox bounds;
            timing.range(&min, &max);
            if (!endTransform->transformOperations().blendedBoundsForBox(originalBox, startTransform->transformOperations(), min, max, &bounds))
                return false;
            box.expandTo(bounds);
        }
    }
    return true;
}

// -----------------------------------------------------------------------
// CompositorAnimations public API
// -----------------------------------------------------------------------

bool CompositorAnimations::isCandidateForAnimationOnCompositor(const Timing& timing, const Element& targetElement, const Animation* animationToAdd, const EffectModel& effect, double animationPlaybackRate)
{
    return false; //weolar

    const KeyframeEffectModelBase& keyframeEffect = toKeyframeEffectModelBase(effect);

    PropertyHandleSet properties = keyframeEffect.properties();
    if (properties.isEmpty())
        return false;

    unsigned transformPropertyCount = 0;
    for (const auto& property : properties) {
        if (!property.isCSSProperty())
            return false;

        if (isTransformRelatedCSSProperty(property))
            transformPropertyCount++;

        const PropertySpecificKeyframeVector& keyframes = keyframeEffect.getPropertySpecificKeyframes(property);
        ASSERT(keyframes.size() >= 2);
        for (const auto& keyframe : keyframes) {
            // FIXME: Determine candidacy based on the CSSValue instead of a snapshot AnimatableValue.
            bool isNeutralKeyframe = keyframe->isCSSPropertySpecificKeyframe() && !toCSSPropertySpecificKeyframe(keyframe.get())->value() && keyframe->composite() == EffectModel::CompositeAdd;
            if ((keyframe->composite() != EffectModel::CompositeReplace && !isNeutralKeyframe) || !keyframe->getAnimatableValue())
                return false;

            switch (property.cssProperty()) {
            case CSSPropertyOpacity:
                break;
            case CSSPropertyRotate:
            case CSSPropertyScale:
            case CSSPropertyTranslate:
            case CSSPropertyTransform:
                if (toAnimatableTransform(keyframe->getAnimatableValue().get())->transformOperations().dependsOnBoxSize())
                    return false;
                break;
            case CSSPropertyWebkitFilter: {
                const FilterOperations& operations = toAnimatableFilterOperations(keyframe->getAnimatableValue().get())->operations();
                if (operations.hasFilterThatMovesPixels())
                    return false;
                break;
            }
            default:
                // any other types are not allowed to run on compositor.
                return false;
            }
        }
    }

    // TODO: Support multiple transform property animations on the compositor
    if (transformPropertyCount > 1)
        return false;

    if (animationToAdd && hasIncompatibleAnimations(targetElement, *animationToAdd, effect))
        return false;

    CompositorAnimationsImpl::CompositorTiming out;
    if (!CompositorAnimationsImpl::convertTimingForCompositor(timing, 0, out, animationPlaybackRate))
        return false;

    return true;
}

void CompositorAnimations::cancelIncompatibleAnimationsOnCompositor(const Element& targetElement, const Animation& animationToAdd, const EffectModel& effectToAdd)
{
    const bool affectsOpacity = effectToAdd.affects(PropertyHandle(CSSPropertyOpacity));
    const bool affectsTransform =  effectToAdd.isTransformRelatedEffect();
    const bool affectsFilter = effectToAdd.affects(PropertyHandle(CSSPropertyWebkitFilter));

    if (!targetElement.hasAnimations())
        return;

    ElementAnimations* elementAnimations = targetElement.elementAnimations();
    ASSERT(elementAnimations);

    for (const auto& entry : elementAnimations->animations()) {
        Animation* attachedAnimation = entry.key;
        if (!considerAnimationAsIncompatible(*attachedAnimation, animationToAdd))
            continue;

        if ((affectsOpacity && attachedAnimation->affects(targetElement, CSSPropertyOpacity))
            || (affectsTransform && isTransformRelatedAnimation(targetElement, attachedAnimation))
            || (affectsFilter && attachedAnimation->affects(targetElement, CSSPropertyWebkitFilter)))
            attachedAnimation->cancelAnimationOnCompositor();
    }
}

bool CompositorAnimations::canStartAnimationOnCompositor(const Element& element)
{
    return element.layoutObject() && element.layoutObject()->compositingState() == PaintsIntoOwnBacking;
}

bool CompositorAnimations::startAnimationOnCompositor(const Element& element, int group, double startTime, double timeOffset, const Timing& timing, const Animation& animation, const EffectModel& effect, Vector<int>& startedAnimationIds, double animationPlaybackRate)
{
    ASSERT(startedAnimationIds.isEmpty());
    ASSERT(isCandidateForAnimationOnCompositor(timing, element, &animation, effect, animationPlaybackRate));
    ASSERT(canStartAnimationOnCompositor(element));

    const KeyframeEffectModelBase& keyframeEffect = toKeyframeEffectModelBase(effect);

    DeprecatedPaintLayer* layer = toLayoutBoxModelObject(element.layoutObject())->layer();
    ASSERT(layer);

    Vector<OwnPtr<WebCompositorAnimation>> animations;
    CompositorAnimationsImpl::getAnimationOnCompositor(timing, group, startTime, timeOffset, keyframeEffect, animations, animationPlaybackRate);
    ASSERT(!animations.isEmpty());
    for (auto& compositorAnimation : animations) {
        int id = compositorAnimation->id();
        if (RuntimeEnabledFeatures::compositorAnimationTimelinesEnabled()) {
            WebCompositorAnimationPlayer* compositorPlayer = animation.compositorPlayer();
            ASSERT(compositorPlayer);
            compositorPlayer->addAnimation(compositorAnimation.leakPtr());
        } else if (!layer->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()->addAnimation(compositorAnimation.release())) {
            // FIXME: We should know ahead of time whether these animations can be started.
            for (int startedAnimationId : startedAnimationIds)
                cancelAnimationOnCompositor(element, animation, startedAnimationId);
            startedAnimationIds.clear();
            return false;
        }
        startedAnimationIds.append(id);
    }
    ASSERT(!startedAnimationIds.isEmpty());
    return true;
}

void CompositorAnimations::cancelAnimationOnCompositor(const Element& element, const Animation& animation, int id)
{
    if (!canStartAnimationOnCompositor(element)) {
        // When an element is being detached, we cancel any associated
        // Animations for CSS animations. But by the time we get
        // here the mapping will have been removed.
        // FIXME: Defer remove/pause operations until after the
        // compositing update.
        return;
    }
    if (RuntimeEnabledFeatures::compositorAnimationTimelinesEnabled()) {
        WebCompositorAnimationPlayer* compositorPlayer = animation.compositorPlayer();
        ASSERT(compositorPlayer);
        compositorPlayer->removeAnimation(id);
    } else {
        toLayoutBoxModelObject(element.layoutObject())->layer()->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()->removeAnimation(id);
    }
}

void CompositorAnimations::pauseAnimationForTestingOnCompositor(const Element& element, const Animation& animation, int id, double pauseTime)
{
    // FIXME: canStartAnimationOnCompositor queries compositingState, which is not necessarily up to date.
    // https://code.google.com/p/chromium/issues/detail?id=339847
    DisableCompositingQueryAsserts disabler;

    if (!canStartAnimationOnCompositor(element)) {
        ASSERT_NOT_REACHED();
        return;
    }
    if (RuntimeEnabledFeatures::compositorAnimationTimelinesEnabled()) {
        WebCompositorAnimationPlayer* compositorPlayer = animation.compositorPlayer();
        ASSERT(compositorPlayer);
        compositorPlayer->pauseAnimation(id, pauseTime);
    } else {
        toLayoutBoxModelObject(element.layoutObject())->layer()->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()->pauseAnimation(id, pauseTime);
    }
}

bool CompositorAnimations::canAttachCompositedLayers(const Element& element, const Animation& animation)
{
    if (!RuntimeEnabledFeatures::compositorAnimationTimelinesEnabled())
        return false;

    if (!animation.compositorPlayer())
        return false;

    if (!element.layoutObject() || !element.layoutObject()->isBoxModelObject())
        return false;

    DeprecatedPaintLayer* layer = toLayoutBoxModelObject(element.layoutObject())->layer();

    if (!layer || !layer->isAllowedToQueryCompositingState()
        || !layer->compositedDeprecatedPaintLayerMapping()
        || !layer->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer())
        return false;

    if (!layer->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()->platformLayer())
        return false;

    return true;
}

void CompositorAnimations::attachCompositedLayers(const Element& element, const Animation& animation)
{
    ASSERT(element.layoutObject());

    DeprecatedPaintLayer* layer = toLayoutBoxModelObject(element.layoutObject())->layer();
    ASSERT(layer);

    WebCompositorAnimationPlayer* compositorPlayer = animation.compositorPlayer();
    ASSERT(compositorPlayer);

    ASSERT(layer->compositedDeprecatedPaintLayerMapping());
    compositorPlayer->attachLayer(layer->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()->platformLayer());
}

// -----------------------------------------------------------------------
// CompositorAnimationsImpl
// -----------------------------------------------------------------------

bool CompositorAnimationsImpl::convertTimingForCompositor(const Timing& timing, double timeOffset, CompositorTiming& out, double animationPlaybackRate)
{
    timing.assertValid();

    // FIXME: Compositor does not know anything about endDelay.
    if (timing.endDelay != 0)
        return false;

    if (std::isnan(timing.iterationDuration) || !timing.iterationCount || !timing.iterationDuration)
        return false;

    if (!std::isfinite(timing.iterationCount)) {
        out.adjustedIterationCount = -1;
    } else {
        out.adjustedIterationCount = timing.iterationCount;
    }

    out.scaledDuration = timing.iterationDuration;
    out.direction = timing.direction;
    // Compositor's time offset is positive for seeking into the animation.
    out.scaledTimeOffset = -timing.startDelay / animationPlaybackRate + timeOffset;
    out.playbackRate = timing.playbackRate * animationPlaybackRate;
    out.fillMode = timing.fillMode == Timing::FillModeAuto ? Timing::FillModeNone : timing.fillMode;
    out.iterationStart = timing.iterationStart;
    out.assertValid();
    return true;
}

namespace {

void getCubicBezierTimingFunctionParameters(const TimingFunction& timingFunction, bool& outCustom,
    WebCompositorAnimationCurve::TimingFunctionType& outEaseSubType,
    double& outX1, double& outY1, double& outX2, double& outY2)
{
    const CubicBezierTimingFunction& cubic = toCubicBezierTimingFunction(timingFunction);
    outCustom = false;

    switch (cubic.subType()) {
    case CubicBezierTimingFunction::Ease:
        outEaseSubType = WebCompositorAnimationCurve::TimingFunctionTypeEase;
        break;
    case CubicBezierTimingFunction::EaseIn:
        outEaseSubType = WebCompositorAnimationCurve::TimingFunctionTypeEaseIn;
        break;
    case CubicBezierTimingFunction::EaseOut:
        outEaseSubType = WebCompositorAnimationCurve::TimingFunctionTypeEaseOut;
        break;
    case CubicBezierTimingFunction::EaseInOut:
        outEaseSubType = WebCompositorAnimationCurve::TimingFunctionTypeEaseInOut;
        break;
    case CubicBezierTimingFunction::Custom:
        outCustom = true;
        outX1 = cubic.x1();
        outY1 = cubic.y1();
        outX2 = cubic.x2();
        outY2 = cubic.y2();
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

void getStepsTimingFunctionParameters(const TimingFunction& timingFunction, int& outSteps, float& outStepsStartOffset)
{
    const StepsTimingFunction& steps = toStepsTimingFunction(timingFunction);

    outSteps = steps.numberOfSteps();
    switch (steps.stepAtPosition()) {
    case StepsTimingFunction::Start:
        outStepsStartOffset = 1;
        break;
    case StepsTimingFunction::Middle:
        outStepsStartOffset = 0.5;
        break;
    case StepsTimingFunction::End:
        outStepsStartOffset = 0;
        break;
    default:
        ASSERT_NOT_REACHED();
        outStepsStartOffset = 0;
        break;
    }
}

template<typename PlatformAnimationCurveType, typename PlatformAnimationKeyframeType>
void addKeyframeWithTimingFunction(PlatformAnimationCurveType& curve, const PlatformAnimationKeyframeType& keyframe, const TimingFunction* timingFunction)
{
    if (!timingFunction) {
        curve.add(keyframe);
        return;
    }

    switch (timingFunction->type()) {
    case TimingFunction::LinearFunction:
        curve.add(keyframe, WebCompositorAnimationCurve::TimingFunctionTypeLinear);
        break;

    case TimingFunction::CubicBezierFunction: {
        bool custom;
        WebCompositorAnimationCurve::TimingFunctionType easeSubType;
        double x1, y1;
        double x2, y2;
        getCubicBezierTimingFunctionParameters(*timingFunction, custom, easeSubType, x1, y1, x2, y2);

        if (custom)
            curve.add(keyframe, x1, y1, x2, y2);
        else
            curve.add(keyframe, easeSubType);
        break;
    }

    case TimingFunction::StepsFunction: {
        int steps;
        float stepsStartOffset;
        getStepsTimingFunctionParameters(*timingFunction, steps, stepsStartOffset);

        curve.add(keyframe, steps, stepsStartOffset);
        break;
    }

    default:
        ASSERT_NOT_REACHED();
    }
}

template <typename PlatformAnimationCurveType>
void setTimingFunctionOnCurve(PlatformAnimationCurveType& curve, TimingFunction* timingFunction)
{
    if (!timingFunction) {
        curve.setLinearTimingFunction();
        return;
    }

    switch (timingFunction->type()) {
    case TimingFunction::LinearFunction:
        curve.setLinearTimingFunction();
        break;

    case TimingFunction::CubicBezierFunction: {
        bool custom;
        WebCompositorAnimationCurve::TimingFunctionType easeSubType;
        double x1, y1;
        double x2, y2;
        getCubicBezierTimingFunctionParameters(*timingFunction, custom, easeSubType, x1, y1, x2, y2);

        if (custom)
            curve.setCubicBezierTimingFunction(x1, y1, x2, y2);
        else
            curve.setCubicBezierTimingFunction(easeSubType);
        break;
    }

    case TimingFunction::StepsFunction: {
        int steps;
        float stepsStartOffset;
        getStepsTimingFunctionParameters(*timingFunction, steps, stepsStartOffset);

        curve.setStepsTimingFunction(steps, stepsStartOffset);
        break;
    }

    default:
        ASSERT_NOT_REACHED();
    }
}

} // namespace anoymous

void CompositorAnimationsImpl::addKeyframesToCurve(WebCompositorAnimationCurve& curve, const PropertySpecificKeyframeVector& keyframes, const Timing& timing)
{
    auto* lastKeyframe = keyframes.last().get();
    for (const auto& keyframe : keyframes) {
        const TimingFunction* keyframeTimingFunction = 0;
        if (keyframe != lastKeyframe) { // Ignore timing function of last frame.
            keyframeTimingFunction = &keyframe->easing();
        }

        // FIXME: This relies on StringKeyframes being eagerly evaluated, which will
        // not happen eventually. Instead we should extract the CSSValue here
        // and convert using another set of toAnimatableXXXOperations functions.
        const AnimatableValue* value = keyframe->getAnimatableValue().get();

        switch (curve.type()) {
        case WebCompositorAnimationCurve::AnimationCurveTypeFilter: {
            OwnPtr<WebFilterOperations> ops = adoptPtr(Platform::current()->compositorSupport()->createFilterOperations());
            toWebFilterOperations(toAnimatableFilterOperations(value)->operations(), ops.get());

            WebFilterKeyframe filterKeyframe(keyframe->offset(), ops.release());
            WebFilterAnimationCurve* filterCurve = static_cast<WebFilterAnimationCurve*>(&curve);
            addKeyframeWithTimingFunction(*filterCurve, filterKeyframe, keyframeTimingFunction);
            break;
        }
        case WebCompositorAnimationCurve::AnimationCurveTypeFloat: {
            WebFloatKeyframe floatKeyframe(keyframe->offset(), toAnimatableDouble(value)->toDouble());
            WebFloatAnimationCurve* floatCurve = static_cast<WebFloatAnimationCurve*>(&curve);
            addKeyframeWithTimingFunction(*floatCurve, floatKeyframe, keyframeTimingFunction);
            break;
        }
        case WebCompositorAnimationCurve::AnimationCurveTypeTransform: {
            OwnPtr<WebTransformOperations> ops = adoptPtr(Platform::current()->compositorSupport()->createTransformOperations());
            toWebTransformOperations(toAnimatableTransform(value)->transformOperations(), ops.get());

            WebTransformKeyframe transformKeyframe(keyframe->offset(), ops.release());
            WebTransformAnimationCurve* transformCurve = static_cast<WebTransformAnimationCurve*>(&curve);
            addKeyframeWithTimingFunction(*transformCurve, transformKeyframe, keyframeTimingFunction);
            break;
        }
        default:
            ASSERT_NOT_REACHED();
        }
    }
}

void CompositorAnimationsImpl::getAnimationOnCompositor(const Timing& timing, int group, double startTime, double timeOffset, const KeyframeEffectModelBase& effect, Vector<OwnPtr<WebCompositorAnimation>>& animations, double animationPlaybackRate)
{
    ASSERT(animations.isEmpty());
    CompositorTiming compositorTiming;
    bool timingValid = convertTimingForCompositor(timing, timeOffset, compositorTiming, animationPlaybackRate);
    ASSERT_UNUSED(timingValid, timingValid);

    PropertyHandleSet properties = effect.properties();
    ASSERT(!properties.isEmpty());
    for (const auto& property : properties) {
        PropertySpecificKeyframeVector values;
        getKeyframeValuesForProperty(&effect, property, compositorTiming.scaledDuration, values);

        WebCompositorAnimation::TargetProperty targetProperty;
        OwnPtr<WebCompositorAnimationCurve> curve;
        switch (property.cssProperty()) {
        case CSSPropertyOpacity: {
            targetProperty = WebCompositorAnimation::TargetPropertyOpacity;

            WebFloatAnimationCurve* floatCurve = Platform::current()->compositorSupport()->createFloatAnimationCurve();
            addKeyframesToCurve(*floatCurve, values, timing);
            setTimingFunctionOnCurve(*floatCurve, timing.timingFunction.get());
            curve = adoptPtr(floatCurve);
            break;
        }
        case CSSPropertyWebkitFilter: {
            targetProperty = WebCompositorAnimation::TargetPropertyFilter;
            WebFilterAnimationCurve* filterCurve = Platform::current()->compositorSupport()->createFilterAnimationCurve();
            addKeyframesToCurve(*filterCurve, values, timing);
            setTimingFunctionOnCurve(*filterCurve, timing.timingFunction.get());
            curve = adoptPtr(filterCurve);
            break;
        }
        case CSSPropertyRotate:
        case CSSPropertyScale:
        case CSSPropertyTranslate:
        case CSSPropertyTransform: {
            targetProperty = WebCompositorAnimation::TargetPropertyTransform;
            WebTransformAnimationCurve* transformCurve = Platform::current()->compositorSupport()->createTransformAnimationCurve();
            addKeyframesToCurve(*transformCurve, values, timing);
            setTimingFunctionOnCurve(*transformCurve, timing.timingFunction.get());
            curve = adoptPtr(transformCurve);
            break;
        }
        default:
            ASSERT_NOT_REACHED();
            continue;
        }
        ASSERT(curve.get());

        OwnPtr<WebCompositorAnimation> animation = adoptPtr(Platform::current()->compositorSupport()->createAnimation(*curve, targetProperty, group, 0));

        if (!std::isnan(startTime))
            animation->setStartTime(startTime);

        animation->setIterations(compositorTiming.adjustedIterationCount);
        animation->setIterationStart(compositorTiming.iterationStart);
        animation->setTimeOffset(compositorTiming.scaledTimeOffset);

        switch (compositorTiming.direction) {
        case Timing::PlaybackDirectionNormal:
            animation->setDirection(WebCompositorAnimation::DirectionNormal);
            break;
        case Timing::PlaybackDirectionReverse:
            animation->setDirection(WebCompositorAnimation::DirectionReverse);
            break;
        case Timing::PlaybackDirectionAlternate:
            animation->setDirection(WebCompositorAnimation::DirectionAlternate);
            break;
        case Timing::PlaybackDirectionAlternateReverse:
            animation->setDirection(WebCompositorAnimation::DirectionAlternateReverse);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        animation->setPlaybackRate(compositorTiming.playbackRate);

        switch (compositorTiming.fillMode) {
        case Timing::FillModeNone:
            animation->setFillMode(WebCompositorAnimation::FillModeNone);
            break;
        case Timing::FillModeForwards:
            animation->setFillMode(WebCompositorAnimation::FillModeForwards);
            break;
        case Timing::FillModeBackwards:
            animation->setFillMode(WebCompositorAnimation::FillModeBackwards);
            break;
        case Timing::FillModeBoth:
            animation->setFillMode(WebCompositorAnimation::FillModeBoth);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        animations.append(animation.release());
    }
    ASSERT(!animations.isEmpty());
}

} // namespace blink
