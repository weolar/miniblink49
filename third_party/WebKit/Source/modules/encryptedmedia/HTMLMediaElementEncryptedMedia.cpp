// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/encryptedmedia/HTMLMediaElementEncryptedMedia.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8Binding.h"
#include "core/dom/DOMException.h"
#include "core/dom/DOMTypedArray.h"
#include "core/dom/ExceptionCode.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/MediaKeyError.h"
#include "core/html/MediaKeyEvent.h"
#include "modules/encryptedmedia/ContentDecryptionModuleResultPromise.h"
#include "modules/encryptedmedia/EncryptedMediaUtils.h"
#include "modules/encryptedmedia/MediaEncryptedEvent.h"
#include "modules/encryptedmedia/MediaKeys.h"
#include "platform/ContentDecryptionModuleResult.h"
#include "platform/Logging.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/Functional.h"

namespace blink {

static void throwExceptionIfMediaKeyExceptionOccurred(const String& keySystem, const String& sessionId, WebMediaPlayer::MediaKeyException exception, ExceptionState& exceptionState)
{
    switch (exception) {
    case WebMediaPlayer::MediaKeyExceptionNoError:
        return;
    case WebMediaPlayer::MediaKeyExceptionInvalidPlayerState:
        exceptionState.throwDOMException(InvalidStateError, "The player is in an invalid state.");
        return;
    case WebMediaPlayer::MediaKeyExceptionKeySystemNotSupported:
        exceptionState.throwDOMException(NotSupportedError, "The key system provided ('" + keySystem +"') is not supported.");
        return;
    case WebMediaPlayer::MediaKeyExceptionInvalidAccess:
        exceptionState.throwDOMException(InvalidAccessError, "The session ID provided ('" + sessionId + "') is invalid.");
        return;
    }

    ASSERT_NOT_REACHED();
    return;
}

// This class allows MediaKeys to be set asynchronously.
class SetMediaKeysHandler : public ScriptPromiseResolver {
    WTF_MAKE_NONCOPYABLE(SetMediaKeysHandler);

public:
    static ScriptPromise create(ScriptState*, HTMLMediaElement&, MediaKeys*);
    ~SetMediaKeysHandler() override;

    DECLARE_VIRTUAL_TRACE();

private:
    SetMediaKeysHandler(ScriptState*, HTMLMediaElement&, MediaKeys*);
    void timerFired(Timer<SetMediaKeysHandler>*);

    void clearExistingMediaKeys();
    void setNewMediaKeys();

    void finish();
    void fail(ExceptionCode, const String& errorMessage);

    void clearFailed(ExceptionCode, const String& errorMessage);
    void setFailed(ExceptionCode, const String& errorMessage);

    // Keep media element alive until promise is fulfilled
    RefPtrWillBeMember<HTMLMediaElement> m_element;
    PersistentWillBeMember<MediaKeys> m_newMediaKeys;
    bool m_tookOwnership;
    Timer<SetMediaKeysHandler> m_timer;
};

typedef Function<void()> SuccessCallback;
typedef Function<void(ExceptionCode, const String&)> FailureCallback;

// Represents the result used when setContentDecryptionModule() is called.
// Calls |success| if result is resolved, |failure| is result is rejected.
class SetContentDecryptionModuleResult final : public ContentDecryptionModuleResult {
public:
    SetContentDecryptionModuleResult(PassOwnPtr<SuccessCallback> success, PassOwnPtr<FailureCallback> failure)
        : m_successCallback(success)
        , m_failureCallback(failure)
    {
    }

    // ContentDecryptionModuleResult implementation.
    void complete() override
    {
        (*m_successCallback)();
    }

    void completeWithContentDecryptionModule(WebContentDecryptionModule*) override
    {
        ASSERT_NOT_REACHED();
        (*m_failureCallback)(InvalidStateError, "Unexpected completion.");
    }

    void completeWithSession(WebContentDecryptionModuleResult::SessionStatus status) override
    {
        ASSERT_NOT_REACHED();
        (*m_failureCallback)(InvalidStateError, "Unexpected completion.");
    }

    void completeWithError(WebContentDecryptionModuleException code, unsigned long systemCode, const WebString& message) override
    {
        // Non-zero |systemCode| is appended to the |message|. If the |message|
        // is empty, we'll report "Rejected with system code (systemCode)".
        String errorString = message;
        if (systemCode != 0) {
            if (errorString.isEmpty())
                errorString.append("Rejected with system code");
            errorString.append(" (" + String::number(systemCode) + ")");
        }
        (*m_failureCallback)(WebCdmExceptionToExceptionCode(code), errorString);
    }

private:
    OwnPtr<SuccessCallback> m_successCallback;
    OwnPtr<FailureCallback> m_failureCallback;
};

ScriptPromise SetMediaKeysHandler::create(ScriptState* scriptState, HTMLMediaElement& element, MediaKeys* mediaKeys)
{
    RefPtrWillBeRawPtr<SetMediaKeysHandler> handler = adoptRefWillBeNoop(new SetMediaKeysHandler(scriptState, element, mediaKeys));
    handler->suspendIfNeeded();
    handler->keepAliveWhilePending();
    return handler->promise();
}

SetMediaKeysHandler::SetMediaKeysHandler(ScriptState* scriptState, HTMLMediaElement& element, MediaKeys* mediaKeys)
    : ScriptPromiseResolver(scriptState)
    , m_element(element)
    , m_newMediaKeys(mediaKeys)
    , m_tookOwnership(false)
    , m_timer(this, &SetMediaKeysHandler::timerFired)
{
    WTF_LOG(Media, "SetMediaKeysHandler::SetMediaKeysHandler");

    // 3. Run the remaining steps asynchronously.
    m_timer.startOneShot(0, FROM_HERE);
}

SetMediaKeysHandler::~SetMediaKeysHandler()
{
}

void SetMediaKeysHandler::timerFired(Timer<SetMediaKeysHandler>*)
{
    clearExistingMediaKeys();
}

void SetMediaKeysHandler::clearExistingMediaKeys()
{
    WTF_LOG(Media, "SetMediaKeysHandler::clearExistingMediaKeys");
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(*m_element);

    // 3.1 If mediaKeys is not null, it is already in use by another media
    //     element, and the user agent is unable to use it with this element,
    //     reject promise with a new DOMException whose name is
    //     "QuotaExceededError".
    if (m_newMediaKeys) {
        if (!m_newMediaKeys->setMediaElement(m_element.get())) {
            fail(QuotaExceededError, "The MediaKeys object is already in use by another media element.");
            return;
        }
        // Note that |m_newMediaKeys| is considered owned by |m_element|, so
        // it needs to be reset if any of these steps fail.
        m_tookOwnership = true;
    }

    // 3.2 If the mediaKeys attribute is not null, run the following steps:
    if (thisElement.m_mediaKeys) {
        WebMediaPlayer* mediaPlayer = m_element->webMediaPlayer();
        if (mediaPlayer) {
            // 3.2.1 If the user agent or CDM do not support removing the
            //       association, return a promise rejected with a new
            //       DOMException whose name is "NotSupportedError".
            // 3.2.2 If the association cannot currently be removed (i.e.
            //       during playback), return a promise rejected with a new
            //       DOMException whose name is "InvalidStateError".
            // 3.2.3 Stop using the CDM instance represented by the mediaKeys
            //       attribute to decrypt media data and remove the association
            //       with the media element.
            // (All 3 steps handled as needed in Chromium.)
            OwnPtr<SuccessCallback> successCallback = bind(&SetMediaKeysHandler::setNewMediaKeys, this);
            OwnPtr<FailureCallback> failureCallback = bind<ExceptionCode, const String&>(&SetMediaKeysHandler::clearFailed, this);
            ContentDecryptionModuleResult* result = new SetContentDecryptionModuleResult(successCallback.release(), failureCallback.release());
            mediaPlayer->setContentDecryptionModule(nullptr, result->result());

            // Don't do anything more until |result| is resolved (or rejected).
            return;
        }
    }

    // MediaKeys not currently set or no player connected, so continue on.
    setNewMediaKeys();
}

void SetMediaKeysHandler::setNewMediaKeys()
{
    WTF_LOG(Media, "SetMediaKeysHandler::setNewMediaKeys");

    // 3.3 If mediaKeys is not null, run the following steps:
    if (m_newMediaKeys) {
        // 3.3.1 Associate the CDM instance represented by mediaKeys with the
        //       media element for decrypting media data.
        // 3.3.2 If the preceding step failed, run the following steps:
        //       (done in setFailed()).
        // 3.3.3 Run the Attempt to Resume Playback If Necessary algorithm on
        //       the media element. The user agent may choose to skip this
        //       step if it knows resuming will fail (i.e. mediaKeys has no
        //       sessions).
        //       (Handled in Chromium).
        if (m_element->webMediaPlayer()) {
            OwnPtr<SuccessCallback> successCallback = bind(&SetMediaKeysHandler::finish, this);
            OwnPtr<FailureCallback> failureCallback = bind<ExceptionCode, const String&>(&SetMediaKeysHandler::setFailed, this);
            ContentDecryptionModuleResult* result = new SetContentDecryptionModuleResult(successCallback.release(), failureCallback.release());
            m_element->webMediaPlayer()->setContentDecryptionModule(m_newMediaKeys->contentDecryptionModule(), result->result());

            // Don't do anything more until |result| is resolved (or rejected).
            return;
        }
    }

    // MediaKeys doesn't need to be set on the player, so continue on.
    finish();
}

void SetMediaKeysHandler::finish()
{
    WTF_LOG(Media, "SetMediaKeysHandler::finish");
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(*m_element);

    // 3.4 Set the mediaKeys attribute to mediaKeys.
    if (thisElement.m_mediaKeys)
        thisElement.m_mediaKeys->clearMediaElement();
    thisElement.m_mediaKeys = m_newMediaKeys;

    // 3.5 Resolve promise with undefined.
    resolve();
}

void SetMediaKeysHandler::fail(ExceptionCode code, const String& errorMessage)
{
    // Reset ownership of |m_newMediaKeys|.
    if (m_tookOwnership)
        m_newMediaKeys->clearMediaElement();

    // Reject promise with an appropriate error.
    reject(DOMException::create(code, errorMessage));
}

void SetMediaKeysHandler::clearFailed(ExceptionCode code, const String& errorMessage)
{
    WTF_LOG(Media, "SetMediaKeysHandler::clearFailed (%d, %s)", code, errorMessage.ascii().data());

    // 3.2.4 If the preceding step failed (in setContentDecryptionModule()
    //       called from clearExistingMediaKeys()), reject promise with a new
    //       DOMException whose name is the appropriate error name and that
    //       has an appropriate message.
    fail(code, errorMessage);
}

void SetMediaKeysHandler::setFailed(ExceptionCode code, const String& errorMessage)
{
    WTF_LOG(Media, "SetMediaKeysHandler::setFailed (%d, %s)", code, errorMessage.ascii().data());
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(*m_element);

    // 3.3.2 If the preceding step failed (in setContentDecryptionModule()
    //       called from setNewMediaKeys()), run the following steps:
    // 3.3.2.1 Set the mediaKeys attribute to null.
    thisElement.m_mediaKeys.clear();

    // 3.3.2.2 Reject promise with a new DOMException whose name is the
    //         appropriate error name and that has an appropriate message.
    fail(code, errorMessage);
}

DEFINE_TRACE(SetMediaKeysHandler)
{
    visitor->trace(m_element);
    visitor->trace(m_newMediaKeys);
    ScriptPromiseResolver::trace(visitor);
}

HTMLMediaElementEncryptedMedia::HTMLMediaElementEncryptedMedia()
    : m_emeMode(EmeModeNotSelected)
    , m_isWaitingForKey(false)
{
}

#if !ENABLE(OILPAN)
HTMLMediaElementEncryptedMedia::~HTMLMediaElementEncryptedMedia()
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::~HTMLMediaElementEncryptedMedia");
    if (m_mediaKeys)
        m_mediaKeys->clearMediaElement();
}
#endif

const char* HTMLMediaElementEncryptedMedia::supplementName()
{
    return "HTMLMediaElementEncryptedMedia";
}

HTMLMediaElementEncryptedMedia& HTMLMediaElementEncryptedMedia::from(HTMLMediaElement& element)
{
    HTMLMediaElementEncryptedMedia* supplement = static_cast<HTMLMediaElementEncryptedMedia*>(WillBeHeapSupplement<HTMLMediaElement>::from(element, supplementName()));
    if (!supplement) {
        supplement = new HTMLMediaElementEncryptedMedia();
        provideTo(element, supplementName(), adoptPtrWillBeNoop(supplement));
    }
    return *supplement;
}

bool HTMLMediaElementEncryptedMedia::setEmeMode(EmeMode emeMode)
{
    if (m_emeMode != EmeModeNotSelected && m_emeMode != emeMode)
        return false;

    m_emeMode = emeMode;
    return true;
}

WebContentDecryptionModule* HTMLMediaElementEncryptedMedia::contentDecryptionModule()
{
    return m_mediaKeys ? m_mediaKeys->contentDecryptionModule() : 0;
}

MediaKeys* HTMLMediaElementEncryptedMedia::mediaKeys(HTMLMediaElement& element)
{
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(element);
    return thisElement.m_mediaKeys.get();
}

ScriptPromise HTMLMediaElementEncryptedMedia::setMediaKeys(ScriptState* scriptState, HTMLMediaElement& element, MediaKeys* mediaKeys)
{
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(element);
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::setMediaKeys current(%p), new(%p)", thisElement.m_mediaKeys.get(), mediaKeys);

    if (!thisElement.setEmeMode(EmeModeUnprefixed))
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(InvalidStateError, "Mixed use of EME prefixed and unprefixed API not allowed."));

    // 1. If mediaKeys and the mediaKeys attribute are the same object, return
    //    a promise resolved with undefined.
    if (thisElement.m_mediaKeys == mediaKeys)
        return ScriptPromise::cast(scriptState, v8::Undefined(scriptState->isolate()));

    // 2. Let promise be a new promise. Remaining steps done in handler.
    return SetMediaKeysHandler::create(scriptState, element, mediaKeys);
}

// Create a MediaEncryptedEvent for WD EME.
static PassRefPtrWillBeRawPtr<Event> createEncryptedEvent(WebEncryptedMediaInitDataType initDataType, const unsigned char* initData, unsigned initDataLength)
{
    MediaEncryptedEventInit initializer;
    initializer.setInitDataType(EncryptedMediaUtils::convertFromInitDataType(initDataType));
    initializer.setInitData(DOMArrayBuffer::create(initData, initDataLength));
    initializer.setBubbles(false);
    initializer.setCancelable(false);

    return MediaEncryptedEvent::create(EventTypeNames::encrypted, initializer);
}

// Create a 'needkey' MediaKeyEvent for v0.1b EME.
static PassRefPtrWillBeRawPtr<Event> createWebkitNeedKeyEvent(const unsigned char* initData, unsigned initDataLength)
{
    MediaKeyEventInit webkitInitializer;
    webkitInitializer.setInitData(DOMUint8Array::create(initData, initDataLength));

    return MediaKeyEvent::create(EventTypeNames::webkitneedkey, webkitInitializer);
}

void HTMLMediaElementEncryptedMedia::webkitGenerateKeyRequest(HTMLMediaElement& element, const String& keySystem, PassRefPtr<DOMUint8Array> initData, ExceptionState& exceptionState)
{
    HTMLMediaElementEncryptedMedia::from(element).generateKeyRequest(element.webMediaPlayer(), keySystem, initData, exceptionState);
}

void HTMLMediaElementEncryptedMedia::generateKeyRequest(WebMediaPlayer* webMediaPlayer, const String& keySystem, PassRefPtr<DOMUint8Array> initData, ExceptionState& exceptionState)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::webkitGenerateKeyRequest");

    if (!setEmeMode(EmeModePrefixed)) {
        exceptionState.throwDOMException(InvalidStateError, "Mixed use of EME prefixed and unprefixed API not allowed.");
        return;
    }

    if (keySystem.isEmpty()) {
        exceptionState.throwDOMException(SyntaxError, "The key system provided is empty.");
        return;
    }

    if (!webMediaPlayer) {
        exceptionState.throwDOMException(InvalidStateError, "No media has been loaded.");
        return;
    }

    const unsigned char* initDataPointer = 0;
    unsigned initDataLength = 0;
    if (initData) {
        initDataPointer = initData->data();
        initDataLength = initData->length();
    }

    WebMediaPlayer::MediaKeyException result = webMediaPlayer->generateKeyRequest(keySystem, initDataPointer, initDataLength);
    throwExceptionIfMediaKeyExceptionOccurred(keySystem, String(), result, exceptionState);
}

void HTMLMediaElementEncryptedMedia::webkitGenerateKeyRequest(HTMLMediaElement& mediaElement, const String& keySystem, ExceptionState& exceptionState)
{
    webkitGenerateKeyRequest(mediaElement, keySystem, DOMUint8Array::create(0), exceptionState);
}

void HTMLMediaElementEncryptedMedia::webkitAddKey(HTMLMediaElement& element, const String& keySystem, PassRefPtr<DOMUint8Array> key, PassRefPtr<DOMUint8Array> initData, const String& sessionId, ExceptionState& exceptionState)
{
    HTMLMediaElementEncryptedMedia::from(element).addKey(element.webMediaPlayer(), keySystem, key, initData, sessionId, exceptionState);
}

void HTMLMediaElementEncryptedMedia::addKey(WebMediaPlayer* webMediaPlayer, const String& keySystem, PassRefPtr<DOMUint8Array> key, PassRefPtr<DOMUint8Array> initData, const String& sessionId, ExceptionState& exceptionState)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::webkitAddKey");

    if (!setEmeMode(EmeModePrefixed)) {
        exceptionState.throwDOMException(InvalidStateError, "Mixed use of EME prefixed and unprefixed API not allowed.");
        return;
    }

    if (keySystem.isEmpty()) {
        exceptionState.throwDOMException(SyntaxError, "The key system provided is empty.");
        return;
    }

    if (!key) {
        exceptionState.throwDOMException(SyntaxError, "The key provided is invalid.");
        return;
    }

    if (!key->length()) {
        exceptionState.throwDOMException(TypeMismatchError, "The key provided is invalid.");
        return;
    }

    if (!webMediaPlayer) {
        exceptionState.throwDOMException(InvalidStateError, "No media has been loaded.");
        return;
    }

    const unsigned char* initDataPointer = 0;
    unsigned initDataLength = 0;
    if (initData) {
        initDataPointer = initData->data();
        initDataLength = initData->length();
    }

    WebMediaPlayer::MediaKeyException result = webMediaPlayer->addKey(keySystem, key->data(), key->length(), initDataPointer, initDataLength, sessionId);
    throwExceptionIfMediaKeyExceptionOccurred(keySystem, sessionId, result, exceptionState);
}

void HTMLMediaElementEncryptedMedia::webkitAddKey(HTMLMediaElement& mediaElement, const String& keySystem, PassRefPtr<DOMUint8Array> key, ExceptionState& exceptionState)
{
    webkitAddKey(mediaElement, keySystem, key, DOMUint8Array::create(0), String(), exceptionState);
}

void HTMLMediaElementEncryptedMedia::webkitCancelKeyRequest(HTMLMediaElement& element, const String& keySystem, const String& sessionId, ExceptionState& exceptionState)
{
    HTMLMediaElementEncryptedMedia::from(element).cancelKeyRequest(element.webMediaPlayer(), keySystem, sessionId, exceptionState);
}

void HTMLMediaElementEncryptedMedia::cancelKeyRequest(WebMediaPlayer* webMediaPlayer, const String& keySystem, const String& sessionId, ExceptionState& exceptionState)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::webkitCancelKeyRequest");

    if (!setEmeMode(EmeModePrefixed)) {
        exceptionState.throwDOMException(InvalidStateError, "Mixed use of EME prefixed and unprefixed API not allowed.");
        return;
    }

    if (keySystem.isEmpty()) {
        exceptionState.throwDOMException(SyntaxError, "The key system provided is empty.");
        return;
    }

    if (!webMediaPlayer) {
        exceptionState.throwDOMException(InvalidStateError, "No media has been loaded.");
        return;
    }

    WebMediaPlayer::MediaKeyException result = webMediaPlayer->cancelKeyRequest(keySystem, sessionId);
    throwExceptionIfMediaKeyExceptionOccurred(keySystem, sessionId, result, exceptionState);
}

void HTMLMediaElementEncryptedMedia::keyAdded(HTMLMediaElement& element, const String& keySystem, const String& sessionId)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::mediaPlayerKeyAdded");

    MediaKeyEventInit initializer;
    initializer.setKeySystem(keySystem);
    initializer.setSessionId(sessionId);

    RefPtrWillBeRawPtr<Event> event = MediaKeyEvent::create(EventTypeNames::webkitkeyadded, initializer);
    event->setTarget(&element);
    element.scheduleEvent(event.release());
}

void HTMLMediaElementEncryptedMedia::keyError(HTMLMediaElement& element, const String& keySystem, const String& sessionId, WebMediaPlayerClient::MediaKeyErrorCode errorCode, unsigned short systemCode)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::mediaPlayerKeyError: sessionID=%s, errorCode=%d, systemCode=%d", sessionId.utf8().data(), errorCode, systemCode);

    MediaKeyError::Code mediaKeyErrorCode = MediaKeyError::MEDIA_KEYERR_UNKNOWN;
    switch (errorCode) {
    case WebMediaPlayerClient::MediaKeyErrorCodeUnknown:
        mediaKeyErrorCode = MediaKeyError::MEDIA_KEYERR_UNKNOWN;
        break;
    case WebMediaPlayerClient::MediaKeyErrorCodeClient:
        mediaKeyErrorCode = MediaKeyError::MEDIA_KEYERR_CLIENT;
        break;
    case WebMediaPlayerClient::MediaKeyErrorCodeService:
        mediaKeyErrorCode = MediaKeyError::MEDIA_KEYERR_SERVICE;
        break;
    case WebMediaPlayerClient::MediaKeyErrorCodeOutput:
        mediaKeyErrorCode = MediaKeyError::MEDIA_KEYERR_OUTPUT;
        break;
    case WebMediaPlayerClient::MediaKeyErrorCodeHardwareChange:
        mediaKeyErrorCode = MediaKeyError::MEDIA_KEYERR_HARDWARECHANGE;
        break;
    case WebMediaPlayerClient::MediaKeyErrorCodeDomain:
        mediaKeyErrorCode = MediaKeyError::MEDIA_KEYERR_DOMAIN;
        break;
    }

    MediaKeyEventInit initializer;
    initializer.setKeySystem(keySystem);
    initializer.setSessionId(sessionId);
    initializer.setErrorCode(MediaKeyError::create(mediaKeyErrorCode));
    initializer.setSystemCode(systemCode);

    RefPtrWillBeRawPtr<Event> event = MediaKeyEvent::create(EventTypeNames::webkitkeyerror, initializer);
    event->setTarget(&element);
    element.scheduleEvent(event.release());
}

void HTMLMediaElementEncryptedMedia::keyMessage(HTMLMediaElement& element, const String& keySystem, const String& sessionId, const unsigned char* message, unsigned messageLength, const WebURL& defaultURL)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::mediaPlayerKeyMessage: sessionID=%s", sessionId.utf8().data());

    MediaKeyEventInit initializer;
    initializer.setKeySystem(keySystem);
    initializer.setSessionId(sessionId);
    initializer.setMessage(DOMUint8Array::create(message, messageLength));
    initializer.setDefaultURL(KURL(defaultURL));

    RefPtrWillBeRawPtr<Event> event = MediaKeyEvent::create(EventTypeNames::webkitkeymessage, initializer);
    event->setTarget(&element);
    element.scheduleEvent(event.release());
}

void HTMLMediaElementEncryptedMedia::encrypted(HTMLMediaElement& element, WebEncryptedMediaInitDataType initDataType, const unsigned char* initData, unsigned initDataLength)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::encrypted");

    if (RuntimeEnabledFeatures::encryptedMediaEnabled()) {
        // Send event for WD EME.
        RefPtrWillBeRawPtr<Event> event;
        if (element.isMediaDataCORSSameOrigin(element.executionContext()->securityOrigin())) {
            event = createEncryptedEvent(initDataType, initData, initDataLength);
        } else {
            // Current page is not allowed to see content from the media file,
            // so don't return the initData. However, they still get an event.
            event = createEncryptedEvent(WebEncryptedMediaInitDataType::Unknown, nullptr, 0);
        }

        event->setTarget(&element);
        element.scheduleEvent(event.release());
    }

    if (RuntimeEnabledFeatures::prefixedEncryptedMediaEnabled()) {
        // Send event for v0.1b EME.
        RefPtrWillBeRawPtr<Event> event = createWebkitNeedKeyEvent(initData, initDataLength);
        event->setTarget(&element);
        element.scheduleEvent(event.release());
    }
}

void HTMLMediaElementEncryptedMedia::didBlockPlaybackWaitingForKey(HTMLMediaElement& element)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::didBlockPlaybackWaitingForKey");

    // From https://w3c.github.io/encrypted-media/#queue-waitingforkey:
    // It should only be called when the HTMLMediaElement object is potentially
    // playing and its readyState is equal to HAVE_FUTURE_DATA or greater.
    // FIXME: Is this really required?

    // 1. Let the media element be the specified HTMLMediaElement object.
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(element);

    // 2. If the media element's waiting for key value is false, queue a task
    //    to fire a simple event named waitingforkey at the media element.
    if (!thisElement.m_isWaitingForKey) {
        RefPtrWillBeRawPtr<Event> event = Event::create(EventTypeNames::waitingforkey);
        event->setTarget(&element);
        element.scheduleEvent(event.release());
    }

    // 3. Set the media element's waiting for key value to true.
    thisElement.m_isWaitingForKey = true;

    // 4. Suspend playback.
    //    (Already done on the Chromium side by the decryptors.)
}

void HTMLMediaElementEncryptedMedia::didResumePlaybackBlockedForKey(HTMLMediaElement& element)
{
    WTF_LOG(Media, "HTMLMediaElementEncryptedMedia::didResumePlaybackBlockedForKey");

    // Logic is on the Chromium side to attempt to resume playback when a new
    // key is available. However, |m_isWaitingForKey| needs to be cleared so
    // that a later waitingForKey() call can generate the event.
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(element);
    thisElement.m_isWaitingForKey = false;
}

WebContentDecryptionModule* HTMLMediaElementEncryptedMedia::contentDecryptionModule(HTMLMediaElement& element)
{
    HTMLMediaElementEncryptedMedia& thisElement = HTMLMediaElementEncryptedMedia::from(element);
    return thisElement.contentDecryptionModule();
}

DEFINE_TRACE(HTMLMediaElementEncryptedMedia)
{
    visitor->trace(m_mediaKeys);
    WillBeHeapSupplement<HTMLMediaElement>::trace(visitor);
}

} // namespace blink
