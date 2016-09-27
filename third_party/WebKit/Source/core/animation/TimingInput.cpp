// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/TimingInput.h"

#include "core/animation/AnimationInputHelpers.h"
#include "core/animation/KeyframeEffectOptions.h"

namespace blink {

void TimingInput::setStartDelay(Timing& timing, double startDelay)
{
    if (std::isfinite(startDelay))
        timing.startDelay = startDelay / 1000;
    else
        timing.startDelay = Timing::defaults().startDelay;
}

void TimingInput::setEndDelay(Timing& timing, double endDelay)
{
    if (std::isfinite(endDelay))
        timing.endDelay = endDelay / 1000;
    else
        timing.endDelay = Timing::defaults().endDelay;
}

void TimingInput::setFillMode(Timing& timing, const String& fillMode)
{
    if (fillMode == "none") {
        timing.fillMode = Timing::FillModeNone;
    } else if (fillMode == "backwards") {
        timing.fillMode = Timing::FillModeBackwards;
    } else if (fillMode == "both") {
        timing.fillMode = Timing::FillModeBoth;
    } else if (fillMode == "forwards") {
        timing.fillMode = Timing::FillModeForwards;
    } else {
        timing.fillMode = Timing::defaults().fillMode;
    }
}

void TimingInput::setIterationStart(Timing& timing, double iterationStart)
{
    if (std::isfinite(iterationStart))
        timing.iterationStart = std::max<double>(iterationStart, 0);
    else
        timing.iterationStart = Timing::defaults().iterationStart;
}

void TimingInput::setIterationCount(Timing& timing, double iterationCount)
{
    if (!std::isnan(iterationCount))
        timing.iterationCount = std::max<double>(iterationCount, 0);
    else
        timing.iterationCount = Timing::defaults().iterationCount;
}

void TimingInput::setIterationDuration(Timing& timing, double iterationDuration)
{
    if (!std::isnan(iterationDuration) && iterationDuration >= 0)
        timing.iterationDuration = iterationDuration / 1000;
    else
        timing.iterationDuration = Timing::defaults().iterationDuration;
}

void TimingInput::setPlaybackRate(Timing& timing, double playbackRate)
{
    if (std::isfinite(playbackRate))
        timing.playbackRate = playbackRate;
    else
        timing.playbackRate = Timing::defaults().playbackRate;
}

void TimingInput::setPlaybackDirection(Timing& timing, const String& direction)
{
    if (direction == "reverse") {
        timing.direction = Timing::PlaybackDirectionReverse;
    } else if (direction == "alternate") {
        timing.direction = Timing::PlaybackDirectionAlternate;
    } else if (direction == "alternate-reverse") {
        timing.direction = Timing::PlaybackDirectionAlternateReverse;
    } else {
        timing.direction = Timing::defaults().direction;
    }
}

void TimingInput::setTimingFunction(Timing& timing, const String& timingFunctionString)
{
    if (RefPtr<TimingFunction> timingFunction = AnimationInputHelpers::parseTimingFunction(timingFunctionString))
        timing.timingFunction = timingFunction;
    else
        timing.timingFunction = Timing::defaults().timingFunction;
}

Timing TimingInput::convert(const KeyframeEffectOptions& timingInput)
{
    Timing result;

    setStartDelay(result, timingInput.delay());
    setEndDelay(result, timingInput.endDelay());
    setFillMode(result, timingInput.fill());
    setIterationStart(result, timingInput.iterationStart());
    setIterationCount(result, timingInput.iterations());
    if (timingInput.duration().isUnrestrictedDouble())
        setIterationDuration(result, timingInput.duration().getAsUnrestrictedDouble());
    else
        setIterationDuration(result, -1);
    setPlaybackRate(result, timingInput.playbackRate());
    setPlaybackDirection(result, timingInput.direction());
    setTimingFunction(result, timingInput.easing());

    result.assertValid();

    return result;
}

Timing TimingInput::convert(double duration)
{
    Timing result;
    setIterationDuration(result, duration);
    return result;
}

} // namespace blink
