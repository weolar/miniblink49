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

#ifndef AudioBuffer_h
#define AudioBuffer_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMTypedArray.h"
#include "modules/ModulesExport.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class AudioBus;
class ExceptionState;

class MODULES_EXPORT AudioBuffer : public GarbageCollectedFinalized<AudioBuffer>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static AudioBuffer* create(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate);
    static AudioBuffer* create(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate, ExceptionState&);

    // Returns 0 if data is not a valid audio file.
    static AudioBuffer* createFromAudioFileData(const void* data, size_t dataSize, bool mixToMono, float sampleRate);

    static AudioBuffer* createFromAudioBus(AudioBus*);

    // Format
    size_t length() const { return m_length; }
    double duration() const { return length() / static_cast<double>(sampleRate()); }
    float sampleRate() const { return m_sampleRate; }

    // Channel data access
    unsigned numberOfChannels() const { return m_channels.size(); }
    PassRefPtr<DOMFloat32Array> getChannelData(unsigned channelIndex, ExceptionState&);
    DOMFloat32Array* getChannelData(unsigned channelIndex);
    void copyFromChannel(DOMFloat32Array*, long channelNumber, ExceptionState&);
    void copyFromChannel(DOMFloat32Array*, long channelNumber, unsigned long startInChannel, ExceptionState&);
    void copyToChannel(DOMFloat32Array*, long channelNumber, ExceptionState&);
    void copyToChannel(DOMFloat32Array*, long channelNumber, unsigned long startInChannel, ExceptionState&);

    void zero();

    DEFINE_INLINE_TRACE() { }

private:
    static PassRefPtr<DOMFloat32Array> createFloat32ArrayOrNull(size_t length);

protected:
    AudioBuffer(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate);
    explicit AudioBuffer(AudioBus*);
    bool createdSuccessfully(unsigned desiredNumberOfChannels) const;

    float m_sampleRate;
    size_t m_length;

    Vector<RefPtr<DOMFloat32Array>> m_channels;
};

} // namespace blink

#endif // AudioBuffer_h
