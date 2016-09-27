/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/AudioParam.h"

#include "modules/webaudio/AudioNode.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/FloatConversion.h"
#include "platform/audio/AudioUtilities.h"
#include "wtf/MathExtras.h"

namespace blink {

const double AudioParamHandler::DefaultSmoothingConstant = 0.05;
const double AudioParamHandler::SnapThreshold = 0.001;

AudioContext* AudioParamHandler::context() const
{
    // TODO(tkent): We can remove this dangerous function by removing
    // AudioContext dependency from AudioParamTimeline.
    ASSERT_WITH_SECURITY_IMPLICATION(deferredTaskHandler().isAudioThread());
    return &m_context;
}

float AudioParamHandler::value()
{
    // Update value for timeline.
    if (deferredTaskHandler().isAudioThread()) {
        bool hasValue;
        float timelineValue = m_timeline.valueForContextTime(context(), narrowPrecisionToFloat(m_value), hasValue);

        if (hasValue)
            m_value = timelineValue;
    }

    return narrowPrecisionToFloat(m_value);
}

void AudioParamHandler::setValue(float value)
{
    m_value = value;
}

float AudioParamHandler::smoothedValue()
{
    return narrowPrecisionToFloat(m_smoothedValue);
}

bool AudioParamHandler::smooth()
{
    // If values have been explicitly scheduled on the timeline, then use the exact value.
    // Smoothing effectively is performed by the timeline.
    bool useTimelineValue = false;
    if (context())
        m_value = m_timeline.valueForContextTime(context(), narrowPrecisionToFloat(m_value), useTimelineValue);

    if (m_smoothedValue == m_value) {
        // Smoothed value has already approached and snapped to value.
        return true;
    }

    if (useTimelineValue) {
        m_smoothedValue = m_value;
    } else {
        // Dezipper - exponential approach.
        m_smoothedValue += (m_value - m_smoothedValue) * DefaultSmoothingConstant;

        // If we get close enough then snap to actual value.
        // FIXME: the threshold needs to be adjustable depending on range - but
        // this is OK general purpose value.
        if (fabs(m_smoothedValue - m_value) < SnapThreshold)
            m_smoothedValue = m_value;
    }

    return false;
}

float AudioParamHandler::finalValue()
{
    float value = m_value;
    calculateFinalValues(&value, 1, false);
    return value;
}

void AudioParamHandler::calculateSampleAccurateValues(float* values, unsigned numberOfValues)
{
    bool isSafe = deferredTaskHandler().isAudioThread() && values && numberOfValues;
    ASSERT(isSafe);
    if (!isSafe)
        return;

    calculateFinalValues(values, numberOfValues, true);
}

void AudioParamHandler::calculateFinalValues(float* values, unsigned numberOfValues, bool sampleAccurate)
{
    bool isGood = deferredTaskHandler().isAudioThread() && values && numberOfValues;
    ASSERT(isGood);
    if (!isGood)
        return;

    // The calculated result will be the "intrinsic" value summed with all audio-rate connections.

    if (sampleAccurate) {
        // Calculate sample-accurate (a-rate) intrinsic values.
        calculateTimelineValues(values, numberOfValues);
    } else {
        // Calculate control-rate (k-rate) intrinsic value.
        bool hasValue;
        float timelineValue = m_timeline.valueForContextTime(context(), narrowPrecisionToFloat(m_value), hasValue);

        if (hasValue)
            m_value = timelineValue;

        values[0] = narrowPrecisionToFloat(m_value);
    }

    // Now sum all of the audio-rate connections together (unity-gain summing junction).
    // Note that connections would normally be mono, but we mix down to mono if necessary.
    RefPtr<AudioBus> summingBus = AudioBus::create(1, numberOfValues, false);
    summingBus->setChannelMemory(0, values, numberOfValues);

    for (unsigned i = 0; i < numberOfRenderingConnections(); ++i) {
        AudioNodeOutput* output = renderingOutput(i);
        ASSERT(output);

        // Render audio from this output.
        AudioBus* connectionBus = output->pull(0, AudioHandler::ProcessingSizeInFrames);

        // Sum, with unity-gain.
        summingBus->sumFrom(*connectionBus);
    }
}

void AudioParamHandler::calculateTimelineValues(float* values, unsigned numberOfValues)
{
    // Calculate values for this render quantum.  Normally numberOfValues will
    // equal to AudioHandler::ProcessingSizeInFrames (the render quantum size).
    double sampleRate = context()->sampleRate();
    double startTime = context()->currentTime();
    double endTime = startTime + numberOfValues / sampleRate;

    // Note we're running control rate at the sample-rate.
    // Pass in the current value as default value.
    m_value = m_timeline.valuesForTimeRange(startTime, endTime, narrowPrecisionToFloat(m_value), values, numberOfValues, sampleRate, sampleRate);
}

void AudioParamHandler::connect(AudioNodeOutput& output)
{
    ASSERT(deferredTaskHandler().isGraphOwner());

    if (m_outputs.contains(&output))
        return;

    output.addParam(*this);
    m_outputs.add(&output);
    changedOutputs();
}

void AudioParamHandler::disconnect(AudioNodeOutput& output)
{
    ASSERT(deferredTaskHandler().isGraphOwner());

    if (m_outputs.contains(&output)) {
        m_outputs.remove(&output);
        changedOutputs();
        output.removeParam(*this);
    }
}

// ----------------------------------------------------------------

AudioParam::AudioParam(AudioContext& context, double defaultValue)
    : m_handler(AudioParamHandler::create(context, defaultValue))
    , m_context(context)
{
}

AudioParam* AudioParam::create(AudioContext& context, double defaultValue)
{
    return new AudioParam(context, defaultValue);
}

DEFINE_TRACE(AudioParam)
{
    visitor->trace(m_context);
}

float AudioParam::value() const
{
    return handler().value();
}

void AudioParam::setValue(float value)
{
    handler().setValue(value);
}

float AudioParam::defaultValue() const
{
    return handler().defaultValue();
}

void AudioParam::setValueAtTime(float value, double time, ExceptionState& exceptionState)
{
    handler().timeline().setValueAtTime(value, time, exceptionState);
}

void AudioParam::linearRampToValueAtTime(float value, double time, ExceptionState& exceptionState)
{
    handler().timeline().linearRampToValueAtTime(value, time, exceptionState);
}

void AudioParam::exponentialRampToValueAtTime(float value, double time, ExceptionState& exceptionState)
{
    handler().timeline().exponentialRampToValueAtTime(value, time, exceptionState);
}

void AudioParam::setTargetAtTime(float target, double time, double timeConstant, ExceptionState& exceptionState)
{
    handler().timeline().setTargetAtTime(target, time, timeConstant, exceptionState);
}

void AudioParam::setValueCurveAtTime(DOMFloat32Array* curve, double time, double duration, ExceptionState& exceptionState)
{
    handler().timeline().setValueCurveAtTime(curve, time, duration, exceptionState);
}

void AudioParam::cancelScheduledValues(double startTime, ExceptionState& exceptionState)
{
    handler().timeline().cancelScheduledValues(startTime, exceptionState);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
