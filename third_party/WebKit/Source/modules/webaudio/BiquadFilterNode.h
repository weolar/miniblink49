/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
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

#ifndef BiquadFilterNode_h
#define BiquadFilterNode_h

#include "core/dom/DOMTypedArray.h"
#include "modules/webaudio/AudioNode.h"
#include "modules/webaudio/BiquadProcessor.h"

namespace blink {

class AudioParam;

class BiquadFilterNode final : public AudioNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    // These must be defined as in the .idl file and must match those in the BiquadProcessor class.
    enum {
        LOWPASS = 0,
        HIGHPASS = 1,
        BANDPASS = 2,
        LOWSHELF = 3,
        HIGHSHELF = 4,
        PEAKING = 5,
        NOTCH = 6,
        ALLPASS = 7
    };

    static BiquadFilterNode* create(AudioContext& context, float sampleRate)
    {
        return new BiquadFilterNode(context, sampleRate);
    }
    DECLARE_VIRTUAL_TRACE();

    String type() const;
    void setType(const String&);

    AudioParam* frequency() { return m_frequency; }
    AudioParam* q() { return m_q; }
    AudioParam* gain() { return m_gain; }
    AudioParam* detune() { return m_detune; }

    // Get the magnitude and phase response of the filter at the given
    // set of frequencies (in Hz). The phase response is in radians.
    void getFrequencyResponse(const DOMFloat32Array* frequencyHz, DOMFloat32Array* magResponse, DOMFloat32Array* phaseResponse);

private:
    BiquadFilterNode(AudioContext&, float sampleRate);

    BiquadProcessor* biquadProcessor() const;
    bool setType(unsigned); // Returns true on success.

    Member<AudioParam> m_frequency;
    Member<AudioParam> m_q;
    Member<AudioParam> m_gain;
    Member<AudioParam> m_detune;
};

} // namespace blink

#endif // BiquadFilterNode_h
