/*
 * Copyright (C) 2013 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/speech/PlatformSpeechSynthesizer.h"

#include "platform/exported/WebSpeechSynthesizerClientImpl.h"
#include "platform/speech/PlatformSpeechSynthesisUtterance.h"
#include "public/platform/Platform.h"
#include "public/platform/WebSpeechSynthesisUtterance.h"
#include "public/platform/WebSpeechSynthesizer.h"
#include "public/platform/WebSpeechSynthesizerClient.h"
#include "wtf/RetainPtr.h"

namespace blink {

PlatformSpeechSynthesizer* PlatformSpeechSynthesizer::create(PlatformSpeechSynthesizerClient* client)
{
    PlatformSpeechSynthesizer* synthesizer = new PlatformSpeechSynthesizer(client);
    synthesizer->initializeVoiceList();
    return synthesizer;
}

PlatformSpeechSynthesizer::PlatformSpeechSynthesizer(PlatformSpeechSynthesizerClient* client)
    : m_speechSynthesizerClient(client)
{
    m_webSpeechSynthesizerClient = new WebSpeechSynthesizerClientImpl(this, client);
    m_webSpeechSynthesizer = adoptPtr(Platform::current()->createSpeechSynthesizer(m_webSpeechSynthesizerClient));
}

PlatformSpeechSynthesizer::~PlatformSpeechSynthesizer()
{
}

void PlatformSpeechSynthesizer::speak(PlatformSpeechSynthesisUtterance* utterance)
{
    if (m_webSpeechSynthesizer && m_webSpeechSynthesizerClient)
        m_webSpeechSynthesizer->speak(WebSpeechSynthesisUtterance(utterance));
}

void PlatformSpeechSynthesizer::pause()
{
    if (m_webSpeechSynthesizer.get())
        m_webSpeechSynthesizer->pause();
}

void PlatformSpeechSynthesizer::resume()
{
    if (m_webSpeechSynthesizer.get())
        m_webSpeechSynthesizer->resume();
}

void PlatformSpeechSynthesizer::cancel()
{
    if (m_webSpeechSynthesizer.get())
        m_webSpeechSynthesizer->cancel();
}

void PlatformSpeechSynthesizer::setVoiceList(HeapVector<Member<PlatformSpeechSynthesisVoice>>& voices)
{
    m_voiceList = voices;
}

void PlatformSpeechSynthesizer::initializeVoiceList()
{
    if (m_webSpeechSynthesizer.get())
        m_webSpeechSynthesizer->updateVoiceList();
}

DEFINE_TRACE(PlatformSpeechSynthesizer)
{
    visitor->trace(m_speechSynthesizerClient);
    visitor->trace(m_voiceList);
    visitor->trace(m_webSpeechSynthesizerClient);
}

} // namespace blink
