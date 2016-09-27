/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef SpeechRecognitionClientProxy_h
#define SpeechRecognitionClientProxy_h

#include "modules/speech/SpeechRecognitionClient.h"
#include "public/web/WebSpeechRecognizerClient.h"
#include "wtf/Compiler.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class MediaStreamTrack;
class WebSpeechRecognizer;
class WebString;

class SpeechRecognitionClientProxy final : public SpeechRecognitionClient, public WebSpeechRecognizerClient {
public:
    ~SpeechRecognitionClientProxy() override;

    // Constructing a proxy object with a 0 WebSpeechRecognizer is safe in
    // itself, but attempting to call start/stop/abort on it will crash.
    static PassOwnPtr<SpeechRecognitionClientProxy> create(WebSpeechRecognizer*);

    // SpeechRecognitionClient:
    void start(SpeechRecognition*, const SpeechGrammarList*, const String& lang, const String& serviceURI, bool continuous, bool interimResults, unsigned long maxAlternatives, MediaStreamTrack*) override;
    void stop(SpeechRecognition*) override;
    void abort(SpeechRecognition*) override;

    // WebSpeechRecognizerClient:
    void didStartAudio(const WebSpeechRecognitionHandle&) override;
    void didStartSound(const WebSpeechRecognitionHandle&) override;
    void didEndSound(const WebSpeechRecognitionHandle&) override;
    void didEndAudio(const WebSpeechRecognitionHandle&) override;
    void didReceiveResults(const WebSpeechRecognitionHandle&, const WebVector<WebSpeechRecognitionResult>& newFinalResults, const WebVector<WebSpeechRecognitionResult>& currentInterimResults) override;
    void didReceiveNoMatch(const WebSpeechRecognitionHandle&, const WebSpeechRecognitionResult&) override;
    void didReceiveError(const WebSpeechRecognitionHandle&, const WebString& message, WebSpeechRecognizerClient::ErrorCode) override;
    void didStart(const WebSpeechRecognitionHandle&) override;
    void didEnd(const WebSpeechRecognitionHandle&) override;

private:
    SpeechRecognitionClientProxy(WebSpeechRecognizer*);

    WebSpeechRecognizer* m_recognizer;
};

} // namespace blink

#endif // SpeechRecognitionClientProxy_h
