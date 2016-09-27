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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MediaKeys_h
#define MediaKeys_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/DOMArrayPiece.h"
#include "platform/Timer.h"
#include "public/platform/WebContentDecryptionModule.h"
#include "public/platform/WebEncryptedMediaTypes.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"
#include "wtf/Forward.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class HTMLMediaElement;
class MediaKeySession;
class ScriptState;
class WebContentDecryptionModule;

// References are held by JS and HTMLMediaElement.
// The WebContentDecryptionModule has the same lifetime as this object.
class MediaKeys : public GarbageCollectedFinalized<MediaKeys>, public ActiveDOMObject, public ScriptWrappable {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MediaKeys);
    DEFINE_WRAPPERTYPEINFO();
public:
    static MediaKeys* create(ExecutionContext*, const WebVector<WebEncryptedMediaSessionType>& supportedSessionTypes, PassOwnPtr<WebContentDecryptionModule>);
    ~MediaKeys() override;

    MediaKeySession* createSession(ScriptState*, const String& sessionTypeString, ExceptionState&);

    ScriptPromise setServerCertificate(ScriptState*, const DOMArrayPiece& serverCertificate);

    // Indicates that the provided HTMLMediaElement wants to use this object.
    // Returns true if no other HTMLMediaElement currently references this
    // object, false otherwise. Will take a weak reference to HTMLMediaElement.
    bool setMediaElement(HTMLMediaElement*);
    // Indicates that no HTMLMediaElement is currently using this object.
    void clearMediaElement();

    WebContentDecryptionModule* contentDecryptionModule();

    DECLARE_VIRTUAL_TRACE();

    // ActiveDOMObject implementation.
    // FIXME: This class could derive from ContextLifecycleObserver
    // again once hasPendingActivity() is moved to ScriptWrappable
    // (http://crbug.com/483722).
    void contextDestroyed() override;
    bool hasPendingActivity() const override;
    void stop() override;

private:
    MediaKeys(ExecutionContext*, const WebVector<WebEncryptedMediaSessionType>& supportedSessionTypes, PassOwnPtr<WebContentDecryptionModule>);
    class PendingAction;

    bool sessionTypeSupported(WebEncryptedMediaSessionType);
    void timerFired(Timer<MediaKeys>*);

    const WebVector<WebEncryptedMediaSessionType> m_supportedSessionTypes;
    OwnPtr<WebContentDecryptionModule> m_cdm;

    // Keep track of the HTMLMediaElement that references this object. Keeping
    // a WeakMember so that HTMLMediaElement's lifetime isn't dependent on
    // this object.
    RawPtrWillBeWeakMember<HTMLMediaElement> m_mediaElement;

    HeapDeque<Member<PendingAction>> m_pendingActions;
    Timer<MediaKeys> m_timer;
};

} // namespace blink

#endif // MediaKeys_h
