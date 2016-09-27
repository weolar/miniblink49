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

#ifndef PlatformSpeechSynthesizer_h
#define PlatformSpeechSynthesizer_h

#include "platform/PlatformExport.h"
#include "platform/heap/Handle.h"
#include "platform/speech/PlatformSpeechSynthesisVoice.h"
#include "wtf/Vector.h"

namespace blink {

enum SpeechBoundary {
    SpeechWordBoundary,
    SpeechSentenceBoundary
};

class PlatformSpeechSynthesisUtterance;
class WebSpeechSynthesizer;
class WebSpeechSynthesizerClientImpl;

class PLATFORM_EXPORT PlatformSpeechSynthesizerClient : public GarbageCollectedMixin {
public:
    virtual void didStartSpeaking(PlatformSpeechSynthesisUtterance*) = 0;
    virtual void didFinishSpeaking(PlatformSpeechSynthesisUtterance*) = 0;
    virtual void didPauseSpeaking(PlatformSpeechSynthesisUtterance*) = 0;
    virtual void didResumeSpeaking(PlatformSpeechSynthesisUtterance*) = 0;
    virtual void speakingErrorOccurred(PlatformSpeechSynthesisUtterance*) = 0;
    virtual void boundaryEventOccurred(PlatformSpeechSynthesisUtterance*, SpeechBoundary, unsigned charIndex) = 0;
    virtual void voicesDidChange() = 0;

protected:
    virtual ~PlatformSpeechSynthesizerClient() { }
};

class PLATFORM_EXPORT PlatformSpeechSynthesizer : public GarbageCollectedFinalized<PlatformSpeechSynthesizer> {
    WTF_MAKE_NONCOPYABLE(PlatformSpeechSynthesizer);
public:
    static PlatformSpeechSynthesizer* create(PlatformSpeechSynthesizerClient*);

    virtual ~PlatformSpeechSynthesizer();

    const HeapVector<Member<PlatformSpeechSynthesisVoice>>& voiceList() const { return m_voiceList; }
    virtual void speak(PlatformSpeechSynthesisUtterance*);
    virtual void pause();
    virtual void resume();
    virtual void cancel();

    PlatformSpeechSynthesizerClient* client() const { return m_speechSynthesizerClient; }

    void setVoiceList(HeapVector<Member<PlatformSpeechSynthesisVoice>>&);

    // Eager finalization is required to promptly release the owned WebSpeechSynthesizer.
    //
    // If not and delayed until lazily swept, m_webSpeechSynthesizerClient may end up
    // being lazily swept first (i.e., before this PlatformSpeechSynthesizer), leaving
    // m_webSpeechSynthesizer with a dangling pointer to a finalized object --
    // WebSpeechSynthesizer embedder implementations calling notification methods in the
    // other directions by way of m_webSpeechSynthesizerClient. Eagerly releasing
    // WebSpeechSynthesizer prevents such unsafe accesses.
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

protected:
    explicit PlatformSpeechSynthesizer(PlatformSpeechSynthesizerClient*);

    virtual void initializeVoiceList();

    HeapVector<Member<PlatformSpeechSynthesisVoice>> m_voiceList;

private:
    Member<PlatformSpeechSynthesizerClient> m_speechSynthesizerClient;

    OwnPtr<WebSpeechSynthesizer> m_webSpeechSynthesizer;
    Member<WebSpeechSynthesizerClientImpl> m_webSpeechSynthesizerClient;
};

} // namespace blink

#endif // PlatformSpeechSynthesizer_h
