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

#include "modules/speech/testing/PlatformSpeechSynthesizerMock.h"

#include "platform/speech/PlatformSpeechSynthesisUtterance.h"

namespace blink {

PlatformSpeechSynthesizerMock* PlatformSpeechSynthesizerMock::create(PlatformSpeechSynthesizerClient* client)
{
    PlatformSpeechSynthesizerMock* synthesizer = new PlatformSpeechSynthesizerMock(client);
    synthesizer->initializeVoiceList();
    client->voicesDidChange();
    return synthesizer;
}

PlatformSpeechSynthesizerMock::PlatformSpeechSynthesizerMock(PlatformSpeechSynthesizerClient* client)
    : PlatformSpeechSynthesizer(client)
    , m_speakingErrorOccurredTimer(this, &PlatformSpeechSynthesizerMock::speakingErrorOccurred)
    , m_speakingFinishedTimer(this, &PlatformSpeechSynthesizerMock::speakingFinished)
{
}

PlatformSpeechSynthesizerMock::~PlatformSpeechSynthesizerMock()
{
}

void PlatformSpeechSynthesizerMock::speakingErrorOccurred(Timer<PlatformSpeechSynthesizerMock>*)
{
    ASSERT(m_currentUtterance);

    client()->speakingErrorOccurred(m_currentUtterance);
    speakNext();
}

void PlatformSpeechSynthesizerMock::speakingFinished(Timer<PlatformSpeechSynthesizerMock>*)
{
    ASSERT(m_currentUtterance);
    client()->didFinishSpeaking(m_currentUtterance);
    speakNext();
}

void PlatformSpeechSynthesizerMock::speakNext()
{
    if (m_speakingErrorOccurredTimer.isActive())
        return;

    if (m_queuedUtterances.isEmpty()) {
        m_currentUtterance = nullptr;
        return;
    }
    m_currentUtterance = m_queuedUtterances.takeFirst();
    speakNow();
}

void PlatformSpeechSynthesizerMock::initializeVoiceList()
{
    m_voiceList.clear();
    m_voiceList.append(PlatformSpeechSynthesisVoice::create(String("mock.voice.bruce"), String("bruce"), String("en-US"), true, true));
    m_voiceList.append(PlatformSpeechSynthesisVoice::create(String("mock.voice.clark"), String("clark"), String("en-US"), true, false));
    m_voiceList.append(PlatformSpeechSynthesisVoice::create(String("mock.voice.logan"), String("logan"), String("fr-CA"), true, true));
}

void PlatformSpeechSynthesizerMock::speak(PlatformSpeechSynthesisUtterance* utterance)
{
    if (!m_currentUtterance) {
        m_currentUtterance = utterance;
        speakNow();
        return;
    }
    m_queuedUtterances.append(utterance);
}

void PlatformSpeechSynthesizerMock::speakNow()
{
    ASSERT(m_currentUtterance);
    client()->didStartSpeaking(m_currentUtterance);

    // Fire a fake word and then sentence boundary event.
    client()->boundaryEventOccurred(m_currentUtterance, SpeechWordBoundary, 0);
    client()->boundaryEventOccurred(m_currentUtterance, SpeechSentenceBoundary, m_currentUtterance->text().length());

    // Give the fake speech job some time so that pause and other functions have time to be called.
    m_speakingFinishedTimer.startOneShot(.1, FROM_HERE);
}

void PlatformSpeechSynthesizerMock::cancel()
{
    if (!m_currentUtterance)
        return;

    // Per spec, removes all queued utterances.
    m_queuedUtterances.clear();

    m_speakingFinishedTimer.stop();
    m_speakingErrorOccurredTimer.startOneShot(.1, FROM_HERE);
}

void PlatformSpeechSynthesizerMock::pause()
{
    client()->didPauseSpeaking(m_currentUtterance);
}

void PlatformSpeechSynthesizerMock::resume()
{
    client()->didResumeSpeaking(m_currentUtterance);
}

DEFINE_TRACE(PlatformSpeechSynthesizerMock)
{
    visitor->trace(m_currentUtterance);
    visitor->trace(m_queuedUtterances);
    PlatformSpeechSynthesizer::trace(visitor);
}

} // namespace blink
