/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
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

#ifndef WebAudioBus_h
#define WebAudioBus_h

#include "WebCommon.h"

#if INSIDE_BLINK
namespace WTF { template <typename T> class PassRefPtr; }
#endif

namespace blink {

class AudioBus;

// A container for multi-channel linear PCM audio data.
//
// WARNING: It is not safe to pass a WebAudioBus across threads!!!
//
class BLINK_PLATFORM_EXPORT WebAudioBus {
public:
    WebAudioBus() : m_private(0) { }
    ~WebAudioBus() { reset(); }

    // initialize() allocates memory of the given length for the given number of channels.
    void initialize(unsigned numberOfChannels, size_t length, double sampleRate);

    // resizeSmaller() can only be called after initialize() with a new length <= the initialization length.
    // The data stored in the bus will remain undisturbed.
    void resizeSmaller(size_t newLength);

    // reset() releases the memory allocated from initialize().
    void reset();

    unsigned numberOfChannels() const;
    size_t length() const;
    double sampleRate() const;

    float* channelData(unsigned channelIndex);

#if INSIDE_BLINK
    WTF::PassRefPtr<AudioBus> release();
#endif

private:
    // Disallow copy and assign.
    WebAudioBus(const WebAudioBus&);
    void operator=(const WebAudioBus&);

    AudioBus* m_private;
};

} // namespace blink

#endif // WebAudioBus_h
