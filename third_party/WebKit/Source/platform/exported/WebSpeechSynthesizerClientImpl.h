/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebSpeechSynthesizerClientImpl_h
#define WebSpeechSynthesizerClientImpl_h

#include "platform/heap/Handle.h"
#include "platform/speech/PlatformSpeechSynthesizer.h"
#include "public/platform/WebSpeechSynthesisUtterance.h"
#include "public/platform/WebSpeechSynthesisVoice.h"
#include "public/platform/WebSpeechSynthesizerClient.h"

namespace blink {

class PlatformSpeechSynthesizer;
class PlatformSpeechSynthesizerClient;

class WebSpeechSynthesizerClientImpl final : public GarbageCollectedFinalized<WebSpeechSynthesizerClientImpl>, public WebSpeechSynthesizerClient {
public:
    WebSpeechSynthesizerClientImpl(PlatformSpeechSynthesizer*, PlatformSpeechSynthesizerClient*);
    virtual ~WebSpeechSynthesizerClientImpl();

    virtual void setVoiceList(const WebVector<WebSpeechSynthesisVoice>& voices);
    virtual void didStartSpeaking(const WebSpeechSynthesisUtterance&);
    virtual void didFinishSpeaking(const WebSpeechSynthesisUtterance&);
    virtual void didPauseSpeaking(const WebSpeechSynthesisUtterance&);
    virtual void didResumeSpeaking(const WebSpeechSynthesisUtterance&);
    virtual void speakingErrorOccurred(const WebSpeechSynthesisUtterance&);
    virtual void wordBoundaryEventOccurred(const WebSpeechSynthesisUtterance&, unsigned charIndex);
    virtual void sentenceBoundaryEventOccurred(const WebSpeechSynthesisUtterance&, unsigned charIndex);

    DECLARE_TRACE();

private:
    Member<PlatformSpeechSynthesizer> m_synthesizer;
    Member<PlatformSpeechSynthesizerClient> m_client;
};

} // namespace blink

#endif // WebSpeechSynthesizerClientImpl_h
