/*
 * Copyright (C) 2012, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OscillatorNode_h
#define OscillatorNode_h

#include "modules/webaudio/AudioParam.h"
#include "modules/webaudio/AudioScheduledSourceNode.h"
#include "platform/audio/AudioBus.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Threading.h"

namespace blink {

class AudioContext;
class ExceptionState;
class PeriodicWave;

// OscillatorNode is an audio generator of periodic waveforms.

class OscillatorHandler final : public AudioScheduledSourceHandler {
public:
    // The waveform type.
    // These must be defined as in the .idl file.
    enum {
        SINE = 0,
        SQUARE = 1,
        SAWTOOTH = 2,
        TRIANGLE = 3,
        CUSTOM = 4
    };

    static PassRefPtr<OscillatorHandler> create(AudioNode&, float sampleRate, AudioParamHandler& frequency, AudioParamHandler& detune);
    ~OscillatorHandler() override;

    // AudioHandler
    void process(size_t framesToProcess) override;

    String type() const;
    void setType(const String&, ExceptionState&);

    void setPeriodicWave(PeriodicWave*);

private:
    OscillatorHandler(AudioNode&, float sampleRate, AudioParamHandler& frequency, AudioParamHandler& detune);
    bool setType(unsigned); // Returns true on success.

    // Returns true if there are sample-accurate timeline parameter changes.
    bool calculateSampleAccuratePhaseIncrements(size_t framesToProcess);

    bool propagatesSilence() const override;

    // One of the waveform types defined in the enum.
    unsigned short m_type;

    // Frequency value in Hertz.
    RefPtr<AudioParamHandler> m_frequency;

    // Detune value (deviating from the frequency) in Cents.
    RefPtr<AudioParamHandler> m_detune;

    bool m_firstRender;

    // m_virtualReadIndex is a sample-frame index into our buffer representing the current playback position.
    // Since it's floating-point, it has sub-sample accuracy.
    double m_virtualReadIndex;

    // This synchronizes process().
    mutable Mutex m_processLock;

    // Stores sample-accurate values calculated according to frequency and detune.
    AudioFloatArray m_phaseIncrements;
    AudioFloatArray m_detuneValues;

    // This Persistent doesn't make a reference cycle including the owner
    // OscillatorNode.
    Persistent<PeriodicWave> m_periodicWave;
};

class OscillatorNode final : public AudioScheduledSourceNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    static OscillatorNode* create(AudioContext&, float sampleRate);
    DECLARE_VIRTUAL_TRACE();

    String type() const;
    void setType(const String&, ExceptionState&);
    AudioParam* frequency();
    AudioParam* detune();
    void setPeriodicWave(PeriodicWave*);

private:
    OscillatorNode(AudioContext&, float sampleRate);
    OscillatorHandler& oscillatorHandler() const;

    Member<AudioParam> m_frequency;
    Member<AudioParam> m_detune;
};

} // namespace blink

#endif // OscillatorNode_h
