/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/speech/DOMWindowSpeechSynthesis.h"

#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "wtf/PassRefPtr.h"

namespace blink {

DOMWindowSpeechSynthesis::DOMWindowSpeechSynthesis(LocalDOMWindow& window)
    : DOMWindowProperty(window.frame())
{
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(DOMWindowSpeechSynthesis);

const char* DOMWindowSpeechSynthesis::supplementName()
{
    return "DOMWindowSpeechSynthesis";
}

// static
DOMWindowSpeechSynthesis& DOMWindowSpeechSynthesis::from(LocalDOMWindow& window)
{
    DOMWindowSpeechSynthesis* supplement = static_cast<DOMWindowSpeechSynthesis*>(WillBeHeapSupplement<LocalDOMWindow>::from(window, supplementName()));
    if (!supplement) {
        supplement = new DOMWindowSpeechSynthesis(window);
        provideTo(window, supplementName(), adoptPtrWillBeNoop(supplement));
    }
    return *supplement;
}

// static
SpeechSynthesis* DOMWindowSpeechSynthesis::speechSynthesis(DOMWindow& window)
{
    return DOMWindowSpeechSynthesis::from(toLocalDOMWindow(window)).speechSynthesis();
}

SpeechSynthesis* DOMWindowSpeechSynthesis::speechSynthesis()
{
    if (!m_speechSynthesis && frame())
        m_speechSynthesis = SpeechSynthesis::create(frame()->domWindow()->executionContext());
    return m_speechSynthesis;
}

DEFINE_TRACE(DOMWindowSpeechSynthesis)
{
    visitor->trace(m_speechSynthesis);
    WillBeHeapSupplement<LocalDOMWindow>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

} // namespace blink
