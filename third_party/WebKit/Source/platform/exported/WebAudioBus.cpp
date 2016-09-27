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

#include "config.h"
#include "public/platform/WebAudioBus.h"

#if ENABLE(WEB_AUDIO)
#include "platform/audio/AudioBus.h"
#else
#include "wtf/ThreadSafeRefCounted.h"

namespace blink {
class AudioBus : public ThreadSafeRefCounted<AudioBus> {
};
} // namespace blink
#endif

#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class WebAudioBusPrivate : public AudioBus {
};

void WebAudioBus::initialize(unsigned numberOfChannels, size_t length, double sampleRate)
{
#if ENABLE(WEB_AUDIO)
    RefPtr<AudioBus> audioBus = AudioBus::create(numberOfChannels, length);
    audioBus->setSampleRate(sampleRate);

    if (m_private)
        (static_cast<AudioBus*>(m_private))->deref();

    audioBus->ref();
    m_private = static_cast<WebAudioBusPrivate*>(audioBus.get());
#else
    ASSERT_NOT_REACHED();
#endif
}

void WebAudioBus::resizeSmaller(size_t newLength)
{
#if ENABLE(WEB_AUDIO)
    ASSERT(m_private);
    if (m_private) {
        ASSERT(newLength <= length());
        m_private->resizeSmaller(newLength);
    }
#else
    ASSERT_NOT_REACHED();
#endif
}

void WebAudioBus::reset()
{
#if ENABLE(WEB_AUDIO)
    if (m_private) {
        (static_cast<AudioBus*>(m_private))->deref();
        m_private = 0;
    }
#else
    ASSERT_NOT_REACHED();
#endif
}

unsigned WebAudioBus::numberOfChannels() const
{
#if ENABLE(WEB_AUDIO)
    if (!m_private)
        return 0;
    return m_private->numberOfChannels();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

size_t WebAudioBus::length() const
{
#if ENABLE(WEB_AUDIO)
    if (!m_private)
        return 0;
    return m_private->length();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

double WebAudioBus::sampleRate() const
{
#if ENABLE(WEB_AUDIO)
    if (!m_private)
        return 0;
    return m_private->sampleRate();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

float* WebAudioBus::channelData(unsigned channelIndex)
{
#if ENABLE(WEB_AUDIO)
    if (!m_private)
        return 0;
    ASSERT(channelIndex < numberOfChannels());
    return m_private->channel(channelIndex)->mutableData();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

PassRefPtr<AudioBus> WebAudioBus::release()
{
#if ENABLE(WEB_AUDIO)
    RefPtr<AudioBus> audioBus(adoptRef(static_cast<AudioBus*>(m_private)));
    m_private = 0;
    return audioBus;
#else
    ASSERT_NOT_REACHED();
    return nullptr;
#endif
}

} // namespace blink
