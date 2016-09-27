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

#ifndef SpeechRecognitionResult_h
#define SpeechRecognitionResult_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/ModulesExport.h"
#include "modules/speech/SpeechRecognitionAlternative.h"
#include "platform/heap/Handle.h"

namespace blink {

class MODULES_EXPORT SpeechRecognitionResult final : public GarbageCollected<SpeechRecognitionResult>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static SpeechRecognitionResult* create(const HeapVector<Member<SpeechRecognitionAlternative>>&, bool final);

    unsigned length() { return m_alternatives.size(); }
    SpeechRecognitionAlternative* item(unsigned index);
    bool isFinal() { return m_final; }

    DECLARE_TRACE();

private:
    SpeechRecognitionResult(const HeapVector<Member<SpeechRecognitionAlternative>>&, bool final);

    bool m_final;
    HeapVector<Member<SpeechRecognitionAlternative>> m_alternatives;
};

} // namespace blink

#endif // SpeechRecognitionResult_h
