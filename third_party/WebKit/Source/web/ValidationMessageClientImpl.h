/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef ValidationMessageClientImpl_h
#define ValidationMessageClientImpl_h

#include "core/page/ValidationMessageClient.h"
#include "platform/Timer.h"
#include "platform/geometry/IntRect.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class FrameView;
class WebViewImpl;

class ValidationMessageClientImpl final : public NoBaseWillBeGarbageCollectedFinalized<ValidationMessageClientImpl>, public ValidationMessageClient {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ValidationMessageClientImpl);
public:
    static PassOwnPtrWillBeRawPtr<ValidationMessageClientImpl> create(WebViewImpl&);
    ~ValidationMessageClientImpl() override;

    DECLARE_VIRTUAL_TRACE();

private:
    ValidationMessageClientImpl(WebViewImpl&);
    void checkAnchorStatus(Timer<ValidationMessageClientImpl>*);
    FrameView* currentView();

    void showValidationMessage(const Element& anchor, const String& message, TextDirection messageDir, const String& subMessage, TextDirection subMessageDir) override;
    void hideValidationMessage(const Element& anchor) override;
    bool isValidationMessageVisible(const Element& anchor) override;
    void documentDetached(const Document&) override;
    void willBeDestroyed() override;

    WebViewImpl& m_webView;
    RawPtrWillBeMember<const Element> m_currentAnchor;
    String m_message;
    IntRect m_lastAnchorRectInScreen;
    float m_lastPageScaleFactor;
    double m_finishTime;
    Timer<ValidationMessageClientImpl> m_timer;
};

}

#endif
