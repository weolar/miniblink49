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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#ifndef AudioParam_h
#define AudioParam_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMTypedArray.h"
#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioParamTimeline.h"
#include "modules/webaudio/AudioSummingJunction.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"
#include <sys/types.h>

namespace blink {

class AudioNodeOutput;

// AudioParamHandler is an actual implementation of web-exposed AudioParam
// interface. Each of AudioParam object creates and owns an AudioParamHandler,
// and it is responsible for all of AudioParam tasks. An AudioParamHandler
// object is owned by the originator AudioParam object, and some audio
// processing classes have additional references. An AudioParamHandler can
// outlive the owner AudioParam, and it never dies before the owner AudioParam
// dies.
class AudioParamHandler final : public ThreadSafeRefCounted<AudioParamHandler>, public AudioSummingJunction {
public:
    static const double DefaultSmoothingConstant;
    static const double SnapThreshold;

    static PassRefPtr<AudioParamHandler> create(AudioContext& context, double defaultValue)
    {
        return adoptRef(new AudioParamHandler(context, defaultValue));
    }
    DECLARE_TRACE();
    // This should be used only in audio rendering thread.
    AudioContext* context() const;

    // AudioSummingJunction
    void didUpdate() override { }

    AudioParamTimeline& timeline() { return m_timeline; }

    // Intrinsic value.
    float value();
    void setValue(float);

    // Final value for k-rate parameters, otherwise use calculateSampleAccurateValues() for a-rate.
    // Must be called in the audio thread.
    float finalValue();

    float defaultValue() const { return static_cast<float>(m_defaultValue); }

    // Value smoothing:

    // When a new value is set with setValue(), in our internal use of the parameter we don't immediately jump to it.
    // Instead we smoothly approach this value to avoid glitching.
    float smoothedValue();

    // Smoothly exponentially approaches to (de-zippers) the desired value.
    // Returns true if smoothed value has already snapped exactly to value.
    bool smooth();

    void resetSmoothedValue() { m_smoothedValue = m_value; }

    bool hasSampleAccurateValues() { return m_timeline.hasValues() || numberOfRenderingConnections(); }

    // Calculates numberOfValues parameter values starting at the context's current time.
    // Must be called in the context's render thread.
    void calculateSampleAccurateValues(float* values, unsigned numberOfValues);

    // Connect an audio-rate signal to control this parameter.
    void connect(AudioNodeOutput&);
    void disconnect(AudioNodeOutput&);

private:
    AudioParamHandler(AudioContext& context, double defaultValue)
        : AudioSummingJunction(context.deferredTaskHandler())
        , m_value(defaultValue)
        , m_defaultValue(defaultValue)
        , m_smoothedValue(defaultValue)
        , m_context(context) { }

    // sampleAccurate corresponds to a-rate (audio rate) vs. k-rate in the Web Audio specification.
    void calculateFinalValues(float* values, unsigned numberOfValues, bool sampleAccurate);
    void calculateTimelineValues(float* values, unsigned numberOfValues);

    double m_value;
    double m_defaultValue;

    // Smoothing (de-zippering)
    double m_smoothedValue;

    AudioParamTimeline m_timeline;

    // We can't make this Persistent because of a reference cycle. It's safe to
    // access this field only when we're rendering audio.
    AudioContext& m_context;
};

// AudioParam class represents web-exposed AudioParam interface.
class AudioParam final : public GarbageCollectedFinalized<AudioParam>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static AudioParam* create(AudioContext&, double defaultValue);
    DECLARE_TRACE();
    // |handler| always returns a valid object.
    AudioParamHandler& handler() const { return *m_handler; }
    // |context| always returns a valid object.
    AudioContext* context() const { return m_context; }

    float value() const;
    void setValue(float);
    float defaultValue() const;
    void setValueAtTime(float value, double time, ExceptionState&);
    void linearRampToValueAtTime(float value, double time, ExceptionState&);
    void exponentialRampToValueAtTime(float value, double time, ExceptionState&);
    void setTargetAtTime(float target, double time, double timeConstant, ExceptionState&);
    void setValueCurveAtTime(DOMFloat32Array* curve, double time, double duration, ExceptionState&);
    void cancelScheduledValues(double startTime, ExceptionState&);

private:
    AudioParam(AudioContext&, double defaultValue);

    RefPtr<AudioParamHandler> m_handler;
    Member<AudioContext> m_context;
};

} // namespace blink

#endif // AudioParam_h
