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

#include "config.h"
#include "modules/encryptedmedia/MediaKeySession.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "core/events/Event.h"
#include "core/events/GenericEventQueue.h"
#include "core/html/MediaKeyError.h"
#include "modules/encryptedmedia/ContentDecryptionModuleResultPromise.h"
#include "modules/encryptedmedia/EncryptedMediaUtils.h"
#include "modules/encryptedmedia/MediaKeyMessageEvent.h"
#include "modules/encryptedmedia/MediaKeys.h"
#include "modules/encryptedmedia/SimpleContentDecryptionModuleResultPromise.h"
#include "platform/ContentDecryptionModuleResult.h"
#include "platform/ContentType.h"
#include "platform/Logging.h"
#include "platform/Timer.h"
#include "public/platform/WebContentDecryptionModule.h"
#include "public/platform/WebContentDecryptionModuleException.h"
#include "public/platform/WebContentDecryptionModuleSession.h"
#include "public/platform/WebEncryptedMediaKeyInformation.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "wtf/ASCIICType.h"
#include <cmath>
#include <limits>

namespace {

// Minimum and maximum length for session ids.
enum {
    MinSessionIdLength = 1,
    MaxSessionIdLength = 512
};

} // namespace

namespace blink {

// Checks that |sessionId| looks correct and returns whether all checks pass.
static bool isValidSessionId(const String& sessionId)
{
    if ((sessionId.length() < MinSessionIdLength) || (sessionId.length() > MaxSessionIdLength))
        return false;

    if (!sessionId.containsOnlyASCII())
        return false;

    // Check that the sessionId only contains alphanumeric characters.
    for (unsigned i = 0; i < sessionId.length(); ++i) {
        if (!isASCIIAlphanumeric(sessionId[i]))
            return false;
    }

    return true;
}

static String ConvertKeyStatusToString(const WebEncryptedMediaKeyInformation::KeyStatus status)
{
    switch (status) {
    case WebEncryptedMediaKeyInformation::KeyStatus::Usable:
        return "usable";
    case WebEncryptedMediaKeyInformation::KeyStatus::Expired:
        return "expired";
    case WebEncryptedMediaKeyInformation::KeyStatus::OutputDownscaled:
        return "output-downscaled";
    case WebEncryptedMediaKeyInformation::KeyStatus::OutputNotAllowed:
        return "output-not-allowed";
    case WebEncryptedMediaKeyInformation::KeyStatus::StatusPending:
        return "status-pending";
    case WebEncryptedMediaKeyInformation::KeyStatus::InternalError:
        return "internal-error";
    }

    ASSERT_NOT_REACHED();
    return "internal-error";
}

static ScriptPromise CreateRejectedPromiseNotCallable(ScriptState* scriptState)
{
    return ScriptPromise::rejectWithDOMException(
        scriptState, DOMException::create(InvalidStateError, "The session is not callable."));
}

static ScriptPromise CreateRejectedPromiseAlreadyInitialized(ScriptState* scriptState)
{
    return ScriptPromise::rejectWithDOMException(
        scriptState, DOMException::create(InvalidStateError, "The session is already initialized."));
}

// A class holding a pending action.
class MediaKeySession::PendingAction : public GarbageCollectedFinalized<MediaKeySession::PendingAction> {
public:
    enum Type {
        GenerateRequest,
        Load,
        Update,
        Close,
        Remove
    };

    Type type() const { return m_type; }

    const Persistent<ContentDecryptionModuleResult> result() const
    {
        return m_result;
    }

    const PassRefPtr<DOMArrayBuffer> data() const
    {
        ASSERT(m_type == GenerateRequest || m_type == Update);
        return m_data;
    }

    WebEncryptedMediaInitDataType initDataType() const
    {
        ASSERT(m_type == GenerateRequest);
        return m_initDataType;
    }

    const String& sessionId() const
    {
        ASSERT(m_type == Load);
        return m_stringData;
    }

    static PendingAction* CreatePendingGenerateRequest(ContentDecryptionModuleResult* result, WebEncryptedMediaInitDataType initDataType, PassRefPtr<DOMArrayBuffer> initData)
    {
        ASSERT(result);
        ASSERT(initData);
        return new PendingAction(GenerateRequest, result, initDataType, initData, String());
    }

    static PendingAction* CreatePendingLoadRequest(ContentDecryptionModuleResult* result, const String& sessionId)
    {
        ASSERT(result);
        return new PendingAction(Load, result, WebEncryptedMediaInitDataType::Unknown, PassRefPtr<DOMArrayBuffer>(), sessionId);
    }

    static PendingAction* CreatePendingUpdate(ContentDecryptionModuleResult* result, PassRefPtr<DOMArrayBuffer> data)
    {
        ASSERT(result);
        ASSERT(data);
        return new PendingAction(Update, result, WebEncryptedMediaInitDataType::Unknown, data, String());
    }

    static PendingAction* CreatePendingClose(ContentDecryptionModuleResult* result)
    {
        ASSERT(result);
        return new PendingAction(Close, result, WebEncryptedMediaInitDataType::Unknown, PassRefPtr<DOMArrayBuffer>(), String());
    }

    static PendingAction* CreatePendingRemove(ContentDecryptionModuleResult* result)
    {
        ASSERT(result);
        return new PendingAction(Remove, result, WebEncryptedMediaInitDataType::Unknown, PassRefPtr<DOMArrayBuffer>(), String());
    }

    ~PendingAction()
    {
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_result);
    }

private:
    PendingAction(Type type, ContentDecryptionModuleResult* result, WebEncryptedMediaInitDataType initDataType, PassRefPtr<DOMArrayBuffer> data, const String& stringData)
        : m_type(type)
        , m_result(result)
        , m_initDataType(initDataType)
        , m_data(data)
        , m_stringData(stringData)
    {
    }

    const Type m_type;
    const Member<ContentDecryptionModuleResult> m_result;
    const WebEncryptedMediaInitDataType m_initDataType;
    const RefPtr<DOMArrayBuffer> m_data;
    const String m_stringData;
};

// This class wraps the promise resolver used when initializing a new session
// and is passed to Chromium to fullfill the promise. This implementation of
// completeWithSession() will resolve the promise with void, while
// completeWithError() will reject the promise with an exception. complete()
// is not expected to be called, and will reject the promise.
class NewSessionResultPromise : public ContentDecryptionModuleResultPromise {
public:
    NewSessionResultPromise(ScriptState* scriptState, MediaKeySession* session)
        : ContentDecryptionModuleResultPromise(scriptState)
        , m_session(session)
    {
    }

    ~NewSessionResultPromise() override
    {
    }

    // ContentDecryptionModuleResult implementation.
    void completeWithSession(WebContentDecryptionModuleResult::SessionStatus status) override
    {
        if (status != WebContentDecryptionModuleResult::NewSession) {
            ASSERT_NOT_REACHED();
            reject(InvalidStateError, "Unexpected completion.");
        }

        m_session->finishGenerateRequest();
        resolve();
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_session);
        ContentDecryptionModuleResultPromise::trace(visitor);
    }

private:
    Member<MediaKeySession> m_session;
};

// This class wraps the promise resolver used when loading a session
// and is passed to Chromium to fullfill the promise. This implementation of
// completeWithSession() will resolve the promise with true/false, while
// completeWithError() will reject the promise with an exception. complete()
// is not expected to be called, and will reject the promise.
class LoadSessionResultPromise : public ContentDecryptionModuleResultPromise {
public:
    LoadSessionResultPromise(ScriptState* scriptState, MediaKeySession* session)
        : ContentDecryptionModuleResultPromise(scriptState)
        , m_session(session)
    {
    }

    ~LoadSessionResultPromise() override
    {
    }

    // ContentDecryptionModuleResult implementation.
    void completeWithSession(WebContentDecryptionModuleResult::SessionStatus status) override
    {
        switch (status) {
        case WebContentDecryptionModuleResult::NewSession:
            m_session->finishLoad();
            resolve(true);
            return;

        case WebContentDecryptionModuleResult::SessionNotFound:
            resolve(false);
            return;

        case WebContentDecryptionModuleResult::SessionAlreadyExists:
            ASSERT_NOT_REACHED();
            reject(InvalidStateError, "Unexpected completion.");
            return;
        }

        ASSERT_NOT_REACHED();
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_session);
        ContentDecryptionModuleResultPromise::trace(visitor);
    }

private:
    Member<MediaKeySession> m_session;
};

MediaKeySession* MediaKeySession::create(ScriptState* scriptState, MediaKeys* mediaKeys, WebEncryptedMediaSessionType sessionType)
{
    RefPtrWillBeRawPtr<MediaKeySession> session = new MediaKeySession(scriptState, mediaKeys, sessionType);
    session->suspendIfNeeded();
    return session.get();
}

MediaKeySession::MediaKeySession(ScriptState* scriptState, MediaKeys* mediaKeys, WebEncryptedMediaSessionType sessionType)
    : ActiveDOMObject(scriptState->executionContext())
    , m_asyncEventQueue(GenericEventQueue::create(this))
    , m_mediaKeys(mediaKeys)
    , m_sessionType(sessionType)
    , m_expiration(std::numeric_limits<double>::quiet_NaN())
    , m_keyStatusesMap(new MediaKeyStatusMap())
    , m_isUninitialized(true)
    , m_isCallable(false)
    , m_isClosed(false)
    , m_closedPromise(new ClosedPromise(scriptState->executionContext(), this, ClosedPromise::Closed))
    , m_actionTimer(this, &MediaKeySession::actionTimerFired)
{
    WTF_LOG(Media, "MediaKeySession(%p)::MediaKeySession", this);

    // Create the matching Chromium object. It will not be usable until
    // initializeNewSession() is called in response to the user calling
    // generateRequest().
    WebContentDecryptionModule* cdm = mediaKeys->contentDecryptionModule();
    m_session = adoptPtr(cdm->createSession());
    m_session->setClientInterface(this);

    // From https://w3c.github.io/encrypted-media/#createSession:
    // MediaKeys::createSession(), step 3.
    // 3.1 Let the sessionId attribute be the empty string.
    ASSERT(sessionId().isEmpty());

    // 3.2 Let the expiration attribute be NaN.
    ASSERT(std::isnan(m_expiration));

    // 3.3 Let the closed attribute be a new promise.
    ASSERT(!closed(scriptState).isUndefinedOrNull());

    // 3.4 Let the keyStatuses attribute be empty.
    ASSERT(m_keyStatusesMap->size() == 0);

    // 3.5 Let the session type be sessionType.
    ASSERT(m_sessionType != WebEncryptedMediaSessionType::Unknown);

    // 3.6 Let uninitialized be true.
    ASSERT(m_isUninitialized);

    // 3.7 Let callable be false.
    ASSERT(!m_isCallable);

    // 3.8 Let the use distinctive identifier value be this object's
    // use distinctive identifier.
    // FIXME: Implement this (http://crbug.com/448922).

    // 3.9 Let the cdm implementation value be this object's cdm implementation.
    // 3.10 Let the cdm instance value be this object's cdm instance.
}

MediaKeySession::~MediaKeySession()
{
    WTF_LOG(Media, "MediaKeySession(%p)::~MediaKeySession", this);
    m_session.clear();
#if !ENABLE(OILPAN)
    // MediaKeySession and m_asyncEventQueue always become unreachable
    // together. So MediaKeySession and m_asyncEventQueue are destructed in the
    // same GC. We don't need to call cancelAllEvents explicitly in Oilpan.
    m_asyncEventQueue->cancelAllEvents();
#endif
}

String MediaKeySession::sessionId() const
{
    return m_session->sessionId();
}

ScriptPromise MediaKeySession::closed(ScriptState* scriptState)
{
    return m_closedPromise->promise(scriptState->world());
}

MediaKeyStatusMap* MediaKeySession::keyStatuses()
{
    return m_keyStatusesMap;
}

ScriptPromise MediaKeySession::generateRequest(ScriptState* scriptState, const String& initDataTypeString, const DOMArrayPiece& initData)
{
    WTF_LOG(Media, "MediaKeySession(%p)::generateRequest %s", this, initDataTypeString.ascii().data());

    // From https://w3c.github.io/encrypted-media/#generateRequest:
    // Generates a request based on the initData. When this method is invoked,
    // the user agent must run the following steps:

    // 1. If this object's uninitialized value is false, return a promise
    //    rejected with a new DOMException whose name is "InvalidStateError".
    if (!m_isUninitialized)
        return CreateRejectedPromiseAlreadyInitialized(scriptState);

    // 2. Let this object's uninitialized be false.
    m_isUninitialized = false;

    // 3. If initDataType is an empty string, return a promise rejected with a
    //    new DOMException whose name is "InvalidAccessError".
    if (initDataTypeString.isEmpty()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(InvalidAccessError, "The initDataType parameter is empty."));
    }

    // 4. If initData is an empty array, return a promise rejected with a new
    //    DOMException whose name is"InvalidAccessError".
    if (!initData.byteLength()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(InvalidAccessError, "The initData parameter is empty."));
    }

    // 5. If the Key System implementation represented by this object's cdm
    //    implementation value does not support initDataType as an
    //    Initialization Data Type, return a promise rejected with a new
    //    DOMException whose name is NotSupportedError. String comparison
    //    is case-sensitive.
    //    (blink side doesn't know what the CDM supports, so the proper check
    //     will be done on the Chromium side. However, we can verify that
    //     |initDataType| is one of the registered values.)
    WebEncryptedMediaInitDataType initDataType = EncryptedMediaUtils::convertToInitDataType(initDataTypeString);
    if (initDataType == WebEncryptedMediaInitDataType::Unknown) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(NotSupportedError, "The initialization data type '" + initDataTypeString + "' is not supported."));
    }

    // 6. Let init data be a copy of the contents of the initData parameter.
    RefPtr<DOMArrayBuffer> initDataBuffer = DOMArrayBuffer::create(initData.data(), initData.byteLength());

    // 7. Let session type be this object's session type.
    //    (Done in constructor.)

    // 8. Let promise be a new promise.
    NewSessionResultPromise* result = new NewSessionResultPromise(scriptState, this);
    ScriptPromise promise = result->promise();

    // 9. Run the following steps asynchronously (documented in
    //    actionTimerFired())
    m_pendingActions.append(PendingAction::CreatePendingGenerateRequest(result, initDataType, initDataBuffer.release()));
    ASSERT(!m_actionTimer.isActive());
    m_actionTimer.startOneShot(0, FROM_HERE);

    // 10. Return promise.
    return promise;
}

ScriptPromise MediaKeySession::load(ScriptState* scriptState, const String& sessionId)
{
    WTF_LOG(Media, "MediaKeySession(%p)::load %s", this, sessionId.ascii().data());

    // From https://w3c.github.io/encrypted-media/#load:
    // Loads the data stored for the specified session into this object. When
    // this method is invoked, the user agent must run the following steps:

    // 1. If this object's uninitialized value is false, return a promise
    //    rejected with a new DOMException whose name is "InvalidStateError".
    if (!m_isUninitialized)
        return CreateRejectedPromiseAlreadyInitialized(scriptState);

    // 2. Let this object's uninitialized be false.
    m_isUninitialized = false;

    // 3. If sessionId is an empty string, return a promise rejected with a
    //    new DOMException whose name is "InvalidAccessError".
    if (sessionId.isEmpty()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(InvalidAccessError, "The sessionId parameter is empty."));
    }

    // 4. If this object's session type is not "persistent-license" or
    //    "persistent-release-message", return a promise rejected with a
    //    new DOMException whose name is InvalidAccessError.
    if (m_sessionType != WebEncryptedMediaSessionType::PersistentLicense && m_sessionType != WebEncryptedMediaSessionType::PersistentReleaseMessage) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(InvalidAccessError, "The session type is not persistent."));
    }

    // 5. If the Key System implementation represented by this object's cdm
    //    implementation value does not support loading previous sessions,
    //    return a promise rejected with a new DOMException whose name is
    //    NotSupportedError.
    // FIXME: Implement this (http://crbug.com/448922).

    // 6. Let origin be the origin of this object's Document.
    //    (Available as executionContext()->securityOrigin() anytime.)

    // 7. Let promise be a new promise.
    LoadSessionResultPromise* result = new LoadSessionResultPromise(scriptState, this);
    ScriptPromise promise = result->promise();

    // 8. Run the following steps asynchronously (documented in
    //    actionTimerFired())
    m_pendingActions.append(PendingAction::CreatePendingLoadRequest(result, sessionId));
    ASSERT(!m_actionTimer.isActive());
    m_actionTimer.startOneShot(0, FROM_HERE);

    // 9. Return promise.
    return promise;
}

ScriptPromise MediaKeySession::update(ScriptState* scriptState, const DOMArrayPiece& response)
{
    WTF_LOG(Media, "MediaKeySession(%p)::update", this);
    ASSERT(!m_isClosed);

    // From https://w3c.github.io/encrypted-media/#update:
    // Provides messages, including licenses, to the CDM. When this method is
    // invoked, the user agent must run the following steps:

    // 1. If this object's callable value is false, return a promise rejected
    //    with a new DOMException whose name is InvalidStateError.
    if (!m_isCallable)
        return CreateRejectedPromiseNotCallable(scriptState);

    // 2. If response is an empty array, return a promise rejected with a
    //    new DOMException whose name is InvalidAccessError.
    if (!response.byteLength()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(InvalidAccessError, "The response parameter is empty."));
    }

    // 3. Let response copy be a copy of the contents of the response parameter.
    RefPtr<DOMArrayBuffer> responseCopy = DOMArrayBuffer::create(response.data(), response.byteLength());

    // 4. Let promise be a new promise.
    SimpleContentDecryptionModuleResultPromise* result = new SimpleContentDecryptionModuleResultPromise(scriptState);
    ScriptPromise promise = result->promise();

    // 5. Run the following steps asynchronously (documented in
    //    actionTimerFired())
    m_pendingActions.append(PendingAction::CreatePendingUpdate(result, responseCopy.release()));
    if (!m_actionTimer.isActive())
        m_actionTimer.startOneShot(0, FROM_HERE);

    // 6. Return promise.
    return promise;
}

ScriptPromise MediaKeySession::close(ScriptState* scriptState)
{
    WTF_LOG(Media, "MediaKeySession(%p)::close", this);

    // From https://w3c.github.io/encrypted-media/#close:
    // Indicates that the application no longer needs the session and the CDM
    // should release any resources associated with this object and close it.
    // When this method is invoked, the user agent must run the following steps:

    // 1. If this object's callable value is false, return a promise rejected
    //    with a new DOMException whose name is "InvalidStateError".
    if (!m_isCallable)
        return CreateRejectedPromiseNotCallable(scriptState);

    // 2. If the Session Close algorithm has been run on this object,
    //    return a resolved promise.
    if (m_isClosed)
        return ScriptPromise::cast(scriptState, ScriptValue());

    // 3. Let promise be a new promise.
    SimpleContentDecryptionModuleResultPromise* result = new SimpleContentDecryptionModuleResultPromise(scriptState);
    ScriptPromise promise = result->promise();

    // 4. Run the following steps asynchronously (documented in
    //    actionTimerFired()).
    m_pendingActions.append(PendingAction::CreatePendingClose(result));
    if (!m_actionTimer.isActive())
        m_actionTimer.startOneShot(0, FROM_HERE);

    // 5. Return promise.
    return promise;
}

ScriptPromise MediaKeySession::remove(ScriptState* scriptState)
{
    WTF_LOG(Media, "MediaKeySession(%p)::remove", this);

    // From https://w3c.github.io/encrypted-media/#remove:
    // Removes stored session data associated with this object. When this
    // method is invoked, the user agent must run the following steps:

    // 1. If this object's callable value is false, return a promise rejected
    //    with a new DOMException whose name is "InvalidStateError".
    if (!m_isCallable)
        return CreateRejectedPromiseNotCallable(scriptState);

    // 2. If this object's session type is not "persistent-license" or
    //    "persistent-release-message", return a promise rejected with a
    //    new DOMException whose name is InvalidAccessError.
    if (m_sessionType != WebEncryptedMediaSessionType::PersistentLicense && m_sessionType != WebEncryptedMediaSessionType::PersistentReleaseMessage) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(InvalidAccessError, "The session type is not persistent."));
    }

    // 3. If the Session Close algorithm has been run on this object, return a
    //    promise rejected with a new DOMException whose name is
    //    "InvalidStateError".
    if (m_isClosed) {
        return ScriptPromise::rejectWithDOMException(
            scriptState, DOMException::create(InvalidStateError, "The session is already closed."));
    }

    // 4. Let promise be a new promise.
    SimpleContentDecryptionModuleResultPromise* result = new SimpleContentDecryptionModuleResultPromise(scriptState);
    ScriptPromise promise = result->promise();

    // 5. Run the following steps asynchronously (documented in
    //    actionTimerFired()).
    m_pendingActions.append(PendingAction::CreatePendingRemove(result));
    if (!m_actionTimer.isActive())
        m_actionTimer.startOneShot(0, FROM_HERE);

    // 6. Return promise.
    return promise;
}

void MediaKeySession::actionTimerFired(Timer<MediaKeySession>*)
{
    ASSERT(m_pendingActions.size());

    // Resolving promises now run synchronously and may result in additional
    // actions getting added to the queue. As a result, swap the queue to
    // a local copy to avoid problems if this happens.
    HeapDeque<Member<PendingAction>> pendingActions;
    pendingActions.swap(m_pendingActions);

    while (!pendingActions.isEmpty()) {
        PendingAction* action = pendingActions.takeFirst();

        switch (action->type()) {
        case PendingAction::GenerateRequest:
            // NOTE: Continue step 9 of MediaKeySession::generateRequest().
            WTF_LOG(Media, "MediaKeySession(%p)::actionTimerFired: GenerateRequest", this);

            // initializeNewSession() in Chromium will execute steps 9.1 to 9.7.
            m_session->initializeNewSession(action->initDataType(), static_cast<unsigned char*>(action->data()->data()), action->data()->byteLength(), m_sessionType, action->result()->result());

            // Remaining steps (from 9.8) executed in finishGenerateRequest(),
            // called when |result| is resolved.
            break;

        case PendingAction::Load:
            // NOTE: Continue step 8 of MediaKeySession::load().
            WTF_LOG(Media, "MediaKeySession(%p)::actionTimerFired: Load", this);

            // 8.1 Let sanitized session ID be a validated and/or sanitized
            //     version of sessionId. The user agent should thoroughly
            //     validate the sessionId value before passing it to the CDM.
            //     At a minimum, this should include checking that the length
            //     and value (e.g. alphanumeric) are reasonable.
            // 8.2 If the previous step failed, reject promise with a new
            //     DOMException whose name is "InvalidAccessError".
            if (!isValidSessionId(action->sessionId())) {
                action->result()->completeWithError(WebContentDecryptionModuleExceptionInvalidAccessError, 0, "Invalid sessionId");
                return;
            }

            // 8.3 If there is an unclosed session in the object's Document
            //     whose sessionId attribute is sanitized session ID, reject
            //     promise with a new DOMException whose name is
            //     QuotaExceededError. In other words, do not create a session
            //     if a non-closed session, regardless of type, already exists
            //     for this sanitized session ID in this browsing context.
            //     (Done in the CDM.)

            // 8.4 Let expiration time be NaN.
            //     (Done in the constructor.)
            ASSERT(std::isnan(m_expiration));

            // load() in Chromium will execute steps 8.5 through 8.8.
            m_session->load(action->sessionId(), action->result()->result());

            // Remaining steps (from 8.9) executed in finishLoad(), called
            // when |result| is resolved.
            break;

        case PendingAction::Update:
            // NOTE: Continue step 5 of MediaKeySession::update().
            WTF_LOG(Media, "MediaKeySession(%p)::actionTimerFired: Update", this);

            // update() in Chromium will execute steps 5.1 through 5.8.
            m_session->update(static_cast<unsigned char*>(action->data()->data()), action->data()->byteLength(), action->result()->result());

            // Last step (5.9 Resolve promise) will be done when |result| is
            // resolved.
            break;

        case PendingAction::Close:
            // NOTE: Continue step 4 of MediaKeySession::close().
            WTF_LOG(Media, "MediaKeySession(%p)::actionTimerFired: Close", this);

            // close() in Chromium will execute steps 4.1 through 4.2.
            m_session->close(action->result()->result());

            // Last step (4.3 Resolve promise) will be done when |result| is
            // resolved.
            break;

        case PendingAction::Remove:
            // NOTE: Continue step 5 of MediaKeySession::remove().
            WTF_LOG(Media, "MediaKeySession(%p)::actionTimerFired: Remove", this);

            // remove() in Chromium will execute steps 5.1 through 5.3.
            m_session->remove(action->result()->result());

            // Last step (5.3.3 Resolve promise) will be done when |result| is
            // resolved.
            break;
        }
    }
}

void MediaKeySession::finishGenerateRequest()
{
    // 9.8 If any of the preceding steps failed, reject promise with a
    //     new DOMException whose name is the appropriate error name.
    //     (Done by CDM calling result.completeWithError() as appropriate.)

    // 9.9 Set the sessionId attribute to session id.
    ASSERT(!sessionId().isEmpty());

    // 9.10 Let this object's callable be true.
    m_isCallable = true;

    // 9.11 Run the queue a "message" event algorithm on the session,
    //      providing "license-request" and message.
    //     (Done by the CDM.)

    // 9.12 Resolve promise.
    //      (Done by NewSessionResultPromise.)
}

void MediaKeySession::finishLoad()
{
    // 8.9 If any of the preceding steps failed, reject promise with a new
    //     DOMException whose name is the appropriate error name.
    //     (Done by CDM calling result.completeWithError() as appropriate.)

    // 8.10 Set the sessionId attribute to sanitized session ID.
    ASSERT(!sessionId().isEmpty());

    // 8.11 Let this object's callable be true.
    m_isCallable = true;

    // 8.12 If the loaded session contains information about any keys (there
    //      are known keys), run the update key statuses algorithm on the
    //      session, providing each key's key ID along with the appropriate
    //      MediaKeyStatus. Should additional processing be necessary to
    //      determine with certainty the status of a key, use the non-"usable"
    //      MediaKeyStatus value that corresponds to the reason for the
    //      additional processing. Once the additional processing for one or
    //      more keys has completed, run the update key statuses algorithm
    //      again if any of the statuses has changed.
    //      (Done by the CDM.)

    // 8.13 Run the Update Expiration algorithm on the session,
    //      providing expiration time.
    //      (Done by the CDM.)

    // 8.14 If message is not null, run the queue a "message" event algorithm
    //      on the session, providing message type and message.
    //      (Done by the CDM.)

    // 8.15 Resolve promise with true.
    //      (Done by LoadSessionResultPromise.)
}

// Queue a task to fire a simple event named keymessage at the new object.
void MediaKeySession::message(MessageType messageType, const unsigned char* message, size_t messageLength)
{
    WTF_LOG(Media, "MediaKeySession(%p)::message", this);

    // Verify that 'message' not fired before session initialization is complete.
    ASSERT(m_isCallable);

    // From https://w3c.github.io/encrypted-media/#queue-message:
    // The following steps are run:
    // 1. Let the session be the specified MediaKeySession object.
    // 2. Queue a task to fire a simple event named message at the session.
    //    The event is of type MediaKeyMessageEvent and has:
    //    -> messageType = the specified message type
    //    -> message = the specified message

    MediaKeyMessageEventInit init;
    switch (messageType) {
    case WebContentDecryptionModuleSession::Client::MessageType::LicenseRequest:
        init.setMessageType("license-request");
        break;
    case WebContentDecryptionModuleSession::Client::MessageType::LicenseRenewal:
        init.setMessageType("license-renewal");
        break;
    case WebContentDecryptionModuleSession::Client::MessageType::LicenseRelease:
        init.setMessageType("license-release");
        break;
    }
    init.setMessage(DOMArrayBuffer::create(static_cast<const void*>(message), messageLength));

    RefPtrWillBeRawPtr<MediaKeyMessageEvent> event = MediaKeyMessageEvent::create(EventTypeNames::message, init);
    event->setTarget(this);
    m_asyncEventQueue->enqueueEvent(event.release());
}

void MediaKeySession::close()
{
    WTF_LOG(Media, "MediaKeySession(%p)::close", this);

    // From https://w3c.github.io/encrypted-media/#session-close:
    // The following steps are run:
    // 1. Let the session be the associated MediaKeySession object.
    // 2. Let promise be the closed attribute of the session.
    // 3. Resolve promise.
    m_closedPromise->resolve(ToV8UndefinedGenerator());

    // Once closed, the session can no longer be the target of events from
    // the CDM so this object can be garbage collected.
    m_isClosed = true;
}

void MediaKeySession::expirationChanged(double updatedExpiryTimeInMS)
{
    WTF_LOG(Media, "MediaKeySession(%p)::expirationChanged %f", this, updatedExpiryTimeInMS);

    // From https://w3c.github.io/encrypted-media/#update-expiration:
    // The following steps are run:
    // 1. Let the session be the associated MediaKeySession object.
    // 2. Let expiration time be NaN.
    double expirationTime = std::numeric_limits<double>::quiet_NaN();

    // 3. If the new expiration time is not NaN, let expiration time be the
    //    new expiration time in milliseconds since 01 January 1970 UTC.
    //    (Note that Chromium actually passes 0 to indicate no expiry.)
    // FIXME: Get Chromium to pass NaN.
    if (!std::isnan(updatedExpiryTimeInMS) && updatedExpiryTimeInMS != 0.0)
        expirationTime = updatedExpiryTimeInMS;

    // 4. Set the session's expiration attribute to expiration time.
    m_expiration = expirationTime;
}

void MediaKeySession::keysStatusesChange(const WebVector<WebEncryptedMediaKeyInformation>& keys, bool hasAdditionalUsableKey)
{
    WTF_LOG(Media, "MediaKeySession(%p)::keysStatusesChange with %zu keys and usable key: %d", this, keys.size(), hasAdditionalUsableKey);

    // From https://w3c.github.io/encrypted-media/#update-key-statuses:
    // The following steps are run:
    // 1. Let the session be the associated MediaKeySession object.
    // 2. Let the input statuses be the sequence of pairs key ID and
    //    associated MediaKeyStatus pairs.
    // 3. Let the statuses be session's keyStatuses attribute.

    // 4. Run the following steps to replace the contents of statuses:
    // 4.1 Empty statuses.
    m_keyStatusesMap->clear();

    // 4.2 For each pair in input statuses.
    for (size_t i = 0; i < keys.size(); ++i) {
        // 4.2.1 Let pair be the pair.
        const auto& key = keys[i];
        // 4.2.2 Insert an entry for pair's key ID into statuses with the
        //       value of pair's MediaKeyStatus value.
        m_keyStatusesMap->addEntry(key.id(), ConvertKeyStatusToString(key.status()));
    }

    // 5. Queue a task to fire a simple event named keystatuseschange
    //    at the session.
    RefPtrWillBeRawPtr<Event> event = Event::create(EventTypeNames::keystatuseschange);
    event->setTarget(this);
    m_asyncEventQueue->enqueueEvent(event.release());

    // 6. Queue a task to run the attempt to resume playback if necessary
    //    algorithm on each of the media element(s) whose mediaKeys attribute
    //    is the MediaKeys object that created the session. The user agent
    //    may choose to skip this step if it knows resuming will fail.
    // FIXME: Attempt to resume playback if |hasAdditionalUsableKey| is true.
    // http://crbug.com/413413
}

const AtomicString& MediaKeySession::interfaceName() const
{
    return EventTargetNames::MediaKeySession;
}

ExecutionContext* MediaKeySession::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

bool MediaKeySession::hasPendingActivity() const
{
    // Remain around if there are pending events or MediaKeys is still around
    // and we're not closed.
    WTF_LOG(Media, "MediaKeySession(%p)::hasPendingActivity %s%s%s%s", this,
        ActiveDOMObject::hasPendingActivity() ? " ActiveDOMObject::hasPendingActivity()" : "",
        !m_pendingActions.isEmpty() ? " !m_pendingActions.isEmpty()" : "",
        m_asyncEventQueue->hasPendingEvents() ? " m_asyncEventQueue->hasPendingEvents()" : "",
        (m_mediaKeys && !m_isClosed) ? " m_mediaKeys && !m_isClosed" : "");

    return ActiveDOMObject::hasPendingActivity()
        || !m_pendingActions.isEmpty()
        || m_asyncEventQueue->hasPendingEvents()
        || (m_mediaKeys && !m_isClosed);
}

void MediaKeySession::stop()
{
    // Stop the CDM from firing any more events for this session.
    m_session.clear();
    m_isClosed = true;

    if (m_actionTimer.isActive())
        m_actionTimer.stop();
    m_pendingActions.clear();
    m_asyncEventQueue->close();
}

DEFINE_TRACE(MediaKeySession)
{
    visitor->trace(m_asyncEventQueue);
    visitor->trace(m_pendingActions);
    visitor->trace(m_mediaKeys);
    visitor->trace(m_keyStatusesMap);
    visitor->trace(m_closedPromise);
    RefCountedGarbageCollectedEventTargetWithInlineData<MediaKeySession>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
