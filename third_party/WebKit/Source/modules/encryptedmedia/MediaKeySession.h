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

#ifndef MediaKeySession_h
#define MediaKeySession_h

#include "bindings/core/v8/ScriptPromiseProperty.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/DOMArrayPiece.h"
#include "modules/EventTargetModules.h"
#include "modules/encryptedmedia/MediaKeyStatusMap.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebContentDecryptionModuleSession.h"
#include "public/platform/WebEncryptedMediaTypes.h"

namespace blink {

class DOMException;
class GenericEventQueue;
class MediaKeys;

// References are held by JS only. However, even if all JS references are
// dropped, it won't be garbage collected until close event received or
// MediaKeys goes away (as determined by a WeakMember reference). This allows
// the CDM to continue to fire events for this session, as long as the session
// is open.
//
// WeakMember<MediaKeys> is used instead of having MediaKeys and MediaKeySession
// keep references to each other, and then having to inform the other object
// when it gets destroyed. When the Oilpan garbage collector determines that
// only WeakMember<> references remain to the MediaKeys object, the MediaKeys
// object will be finalized and the WeakMember<> references will be cleared
// out(zeroed) by the garbage collector.
//
// Because this object controls the lifetime of the WebContentDecryptionModuleSession,
// it may outlive any JavaScript references as long as the MediaKeys object is alive.
// The WebContentDecryptionModuleSession has the same lifetime as this object.
class MediaKeySession final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<MediaKeySession>
    , public ActiveDOMObject
    , private WebContentDecryptionModuleSession::Client {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(MediaKeySession);
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MediaKeySession);
public:
    static MediaKeySession* create(ScriptState*, MediaKeys*, WebEncryptedMediaSessionType);
    ~MediaKeySession() override;

    String sessionId() const;
    double expiration() const { return m_expiration; }
    ScriptPromise closed(ScriptState*);
    MediaKeyStatusMap* keyStatuses();

    ScriptPromise generateRequest(ScriptState*, const String& initDataType, const DOMArrayPiece& initData);
    ScriptPromise load(ScriptState*, const String& sessionId);

    ScriptPromise update(ScriptState*, const DOMArrayPiece& response);
    ScriptPromise close(ScriptState*);
    ScriptPromise remove(ScriptState*);

    // EventTarget
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    // ActiveDOMObject
    bool hasPendingActivity() const override;
    void stop() override;

    // Oilpan: eagerly release owned m_session, which in turn
    // drops the client reference back to this MediaKeySession object.
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

private:
    class PendingAction;
    friend class NewSessionResultPromise;
    friend class LoadSessionResultPromise;

    MediaKeySession(ScriptState*, MediaKeys*, WebEncryptedMediaSessionType);

    void actionTimerFired(Timer<MediaKeySession>*);

    // WebContentDecryptionModuleSession::Client
    void message(MessageType, const unsigned char* message, size_t messageLength) override;
    void close() override;
    void expirationChanged(double updatedExpiryTimeInMS) override;
    void keysStatusesChange(const WebVector<WebEncryptedMediaKeyInformation>&, bool hasAdditionalUsableKey) override;

    // Called by NewSessionResult when the new session has been created.
    void finishGenerateRequest();

    // Called by LoadSessionResult when the session has been loaded.
    void finishLoad();

    OwnPtrWillBeMember<GenericEventQueue> m_asyncEventQueue;
    OwnPtr<WebContentDecryptionModuleSession> m_session;

    // Used to determine if MediaKeys is still active.
    WeakMember<MediaKeys> m_mediaKeys;

    // Session properties.
    WebEncryptedMediaSessionType m_sessionType;
    double m_expiration;
    Member<MediaKeyStatusMap> m_keyStatusesMap;

    // Session states.
    bool m_isUninitialized;
    bool m_isCallable;
    bool m_isClosed; // Is the CDM finished with this session?

    // Keep track of the closed promise.
    typedef ScriptPromiseProperty<Member<MediaKeySession>, ToV8UndefinedGenerator, Member<DOMException>> ClosedPromise;
    Member<ClosedPromise> m_closedPromise;

    HeapDeque<Member<PendingAction>> m_pendingActions;
    Timer<MediaKeySession> m_actionTimer;
};

} // namespace blink

#endif // MediaKeySession_h
