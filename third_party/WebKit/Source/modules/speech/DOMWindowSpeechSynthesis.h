/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#ifndef DOMWindowSpeechSynthesis_h
#define DOMWindowSpeechSynthesis_h

#include "core/frame/DOMWindowProperty.h"
#include "modules/ModulesExport.h"
#include "modules/speech/SpeechSynthesis.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class DOMWindow;

class MODULES_EXPORT DOMWindowSpeechSynthesis final : public NoBaseWillBeGarbageCollected<DOMWindowSpeechSynthesis>, public WillBeHeapSupplement<LocalDOMWindow>, public DOMWindowProperty {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMWindowSpeechSynthesis);
    DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(DOMWindowSpeechSynthesis);
public:
    static SpeechSynthesis* speechSynthesis(DOMWindow&);
    static DOMWindowSpeechSynthesis& from(LocalDOMWindow&);

    DECLARE_TRACE();

private:
    explicit DOMWindowSpeechSynthesis(LocalDOMWindow&);

    SpeechSynthesis* speechSynthesis();
    static const char* supplementName();

    PersistentWillBeMember<SpeechSynthesis> m_speechSynthesis;
};

} // namespace blink

#endif // DOMWindowSpeechSynthesis_h
