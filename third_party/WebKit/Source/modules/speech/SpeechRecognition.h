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

#ifndef SpeechRecognition_h
#define SpeechRecognition_h

#include "core/dom/ActiveDOMObject.h"
#include "core/page/PageLifecycleObserver.h"
#include "modules/EventTargetModules.h"
#include "modules/ModulesExport.h"
#include "modules/speech/SpeechGrammarList.h"
#include "modules/speech/SpeechRecognitionResult.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebPrivatePtr.h"
#include "wtf/Compiler.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class MediaStreamTrack;
class SpeechRecognitionController;
class SpeechRecognitionError;

class MODULES_EXPORT SpeechRecognition final : public RefCountedGarbageCollectedEventTargetWithInlineData<SpeechRecognition>, public PageLifecycleObserver, public ActiveDOMObject {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(SpeechRecognition);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SpeechRecognition);
    DEFINE_WRAPPERTYPEINFO();
public:
    static SpeechRecognition* create(ExecutionContext*);
    ~SpeechRecognition() override;

    // SpeechRecognition.idl implemementation.
    // Attributes.
    SpeechGrammarList* grammars() { return m_grammars; }
    void setGrammars(SpeechGrammarList* grammars) { m_grammars = grammars; }
    String lang() { return m_lang; }
    void setLang(const String& lang) { m_lang = lang; }
    String serviceURI() { return m_serviceURI; }
    void setServiceURI(const String& serviceURI) { m_serviceURI = serviceURI; }
    bool continuous() { return m_continuous; }
    void setContinuous(bool continuous) { m_continuous = continuous; }
    bool interimResults() { return m_interimResults; }
    void setInterimResults(bool interimResults) { m_interimResults = interimResults; }
    unsigned maxAlternatives() { return m_maxAlternatives; }
    void setMaxAlternatives(unsigned maxAlternatives) { m_maxAlternatives = maxAlternatives; }
    MediaStreamTrack* audioTrack() { return m_audioTrack; }
    void setAudioTrack(MediaStreamTrack* audioTrack) { m_audioTrack = audioTrack; }

    // Callable by the user.
    void start(ExceptionState&);
    void stopFunction();
    void abort();

    // Called by the SpeechRecognitionClient.
    void didStartAudio();
    void didStartSound();
    void didStartSpeech();
    void didEndSpeech();
    void didEndSound();
    void didEndAudio();
    void didReceiveResults(const HeapVector<Member<SpeechRecognitionResult>>& newFinalResults, const HeapVector<Member<SpeechRecognitionResult>>& currentInterimResults);
    void didReceiveNoMatch(SpeechRecognitionResult*);
    void didReceiveError(PassRefPtrWillBeRawPtr<SpeechRecognitionError>);
    void didStart();
    void didEnd();

    // EventTarget.
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    // ActiveDOMObject.
    bool hasPendingActivity() const override;
    void stop() override;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(audiostart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(soundstart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(speechstart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(speechend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(soundend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(audioend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(result);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(nomatch);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(start);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(end);

    DECLARE_VIRTUAL_TRACE();

    // PageLifecycleObserver
    void contextDestroyed() override;

private:
    SpeechRecognition(Page*, ExecutionContext*);

    Member<SpeechGrammarList> m_grammars;
    Member<MediaStreamTrack> m_audioTrack;
    String m_lang;
    String m_serviceURI;
    bool m_continuous;
    bool m_interimResults;
    unsigned long m_maxAlternatives;

    RawPtrWillBeMember<SpeechRecognitionController> m_controller;
    bool m_stoppedByActiveDOMObject;
    bool m_started;
    bool m_stopping;
    HeapVector<Member<SpeechRecognitionResult>> m_finalResults;
};

} // namespace blink

#endif // SpeechRecognition_h
