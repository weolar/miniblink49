/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "modules/notifications/Notification.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "core/dom/ScopedWindowFocusAllowedIndicator.h"
#include "core/events/Event.h"
#include "core/frame/UseCounter.h"
#include "modules/notifications/NotificationOptions.h"
#include "modules/notifications/NotificationPermissionClient.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/UserGestureIndicator.h"
#include "public/platform/Platform.h"
#include "public/platform/WebSerializedOrigin.h"
#include "public/platform/WebString.h"
#include "public/platform/modules/notifications/WebNotificationData.h"
#include "public/platform/modules/notifications/WebNotificationManager.h"

namespace blink {
namespace {

const int64_t kInvalidPersistentId = -1;

WebNotificationManager* notificationManager()
{
    return Platform::current()->notificationManager();
}

} // namespace

Notification* Notification::create(ExecutionContext* context, const String& title, const NotificationOptions& options, ExceptionState& exceptionState)
{
    // The Web Notification constructor may be disabled through a runtime feature. The
    // behavior of the constructor is changing, but not completely agreed upon yet.
    if (!RuntimeEnabledFeatures::notificationConstructorEnabled()) {
        exceptionState.throwTypeError("Illegal constructor. Use ServiceWorkerRegistration.showNotification() instead.");
        return nullptr;
    }

    // The Web Notification constructor may not be used in Service Worker contexts.
    if (context->isServiceWorkerGlobalScope()) {
        exceptionState.throwTypeError("Illegal constructor.");
        return nullptr;
    }

    // If options's silent is true, and options's vibrate is present, throw a TypeError exception.
    if (options.hasVibrate() && options.silent()) {
        exceptionState.throwTypeError("Silent notifications must not specify vibration patterns.");
        return nullptr;
    }

    RefPtr<SerializedScriptValue> data;
    if (options.hasData()) {
        data = SerializedScriptValueFactory::instance().create(options.data().isolate(), options.data(), nullptr, exceptionState);
        if (exceptionState.hadException())
            return nullptr;
    }

    Notification* notification = new Notification(title, context);

    notification->setBody(options.body());
    notification->setTag(options.tag());
    notification->setLang(options.lang());
    notification->setDir(options.dir());
    notification->setVibrate(NavigatorVibration::sanitizeVibrationPattern(options.vibrate()));
    notification->setSilent(options.silent());
    notification->setSerializedData(data.release());
    if (options.hasIcon()) {
        KURL iconUrl = options.icon().isEmpty() ? KURL() : context->completeURL(options.icon());
        if (!iconUrl.isEmpty() && iconUrl.isValid())
            notification->setIconUrl(iconUrl);
    }

    String insecureOriginMessage;
    UseCounter::Feature feature = context->isPrivilegedContext(insecureOriginMessage)
        ? UseCounter::NotificationSecureOrigin
        : UseCounter::NotificationInsecureOrigin;
    UseCounter::count(context, feature);

    notification->scheduleShow();
    notification->suspendIfNeeded();
    return notification;
}

Notification* Notification::create(ExecutionContext* context, int64_t persistentId, const WebNotificationData& data)
{
    Notification* notification = new Notification(data.title, context);

    notification->setPersistentId(persistentId);
    notification->setDir(data.direction == WebNotificationData::DirectionLeftToRight ? "ltr" : "rtl");
    notification->setLang(data.lang);
    notification->setBody(data.body);
    notification->setTag(data.tag);
    notification->setSilent(data.silent);

    if (!data.icon.isEmpty())
        notification->setIconUrl(data.icon);

    if (!data.vibrate.isEmpty()) {
        NavigatorVibration::VibrationPattern pattern;
        pattern.appendRange(data.vibrate.begin(), data.vibrate.end());
        notification->setVibrate(pattern);
    }

    const WebVector<char>& dataBytes = data.data;
    if (!dataBytes.isEmpty()) {
        notification->setSerializedData(SerializedScriptValueFactory::instance().createFromWireBytes(dataBytes.data(), dataBytes.size()));
        notification->serializedData()->registerMemoryAllocatedWithCurrentScriptContext();
    }

    notification->setState(NotificationStateShowing);
    notification->suspendIfNeeded();
    return notification;
}

Notification::Notification(const String& title, ExecutionContext* context)
    : ActiveDOMObject(context)
    , m_title(title)
    , m_dir("auto")
    , m_silent(false)
    , m_persistentId(kInvalidPersistentId)
    , m_state(NotificationStateIdle)
    , m_asyncRunner(this, &Notification::show)
{
    ASSERT(notificationManager());
}

Notification::~Notification()
{
}

void Notification::scheduleShow()
{
    ASSERT(m_state == NotificationStateIdle);
    ASSERT(!m_asyncRunner.isActive());

    m_asyncRunner.runAsync();
}

void Notification::show()
{
    ASSERT(m_state == NotificationStateIdle);
    if (Notification::checkPermission(executionContext()) != WebNotificationPermissionAllowed) {
        dispatchErrorEvent();
        return;
    }

    SecurityOrigin* origin = executionContext()->securityOrigin();
    ASSERT(origin);

    // FIXME: Do CSP checks on the associated notification icon.
    WebNotificationData::Direction dir = m_dir == "rtl" ? WebNotificationData::DirectionRightToLeft : WebNotificationData::DirectionLeftToRight;

    // The lifetime and availability of non-persistent notifications is tied to the page
    // they were created by, and thus the data doesn't have to be known to the embedder.
    Vector<char> emptyDataWireBytes;

    WebNotificationData notificationData(m_title, dir, m_lang, m_body, m_tag, m_iconUrl, m_vibrate, m_silent, emptyDataWireBytes);
    notificationManager()->show(WebSerializedOrigin(*origin), notificationData, this);

    m_state = NotificationStateShowing;
}

void Notification::close()
{
    if (m_state != NotificationStateShowing)
        return;

    if (m_persistentId == kInvalidPersistentId) {
        // Fire the close event asynchronously.
        executionContext()->postTask(FROM_HERE, createSameThreadTask(&Notification::dispatchCloseEvent, this));

        m_state = NotificationStateClosing;
        notificationManager()->close(this);
    } else {
        m_state = NotificationStateClosed;

        SecurityOrigin* origin = executionContext()->securityOrigin();
        ASSERT(origin);

        notificationManager()->closePersistent(WebSerializedOrigin(*origin), m_persistentId);
    }
}

void Notification::dispatchShowEvent()
{
    dispatchEvent(Event::create(EventTypeNames::show));
}

void Notification::dispatchClickEvent()
{
    UserGestureIndicator gestureIndicator(DefinitelyProcessingNewUserGesture);
    ScopedWindowFocusAllowedIndicator windowFocusAllowed(executionContext());
    dispatchEvent(Event::create(EventTypeNames::click));
}

void Notification::dispatchErrorEvent()
{
    dispatchEvent(Event::create(EventTypeNames::error));
}

void Notification::dispatchCloseEvent()
{
    // The notification will be showing when the user initiated the close, or it will be
    // closing if the developer initiated the close.
    if (m_state != NotificationStateShowing && m_state != NotificationStateClosing)
        return;

    m_state = NotificationStateClosed;
    dispatchEvent(Event::create(EventTypeNames::close));
}

NavigatorVibration::VibrationPattern Notification::vibrate(bool& isNull) const
{
    isNull = m_vibrate.isEmpty();
    return m_vibrate;
}

TextDirection Notification::direction() const
{
    // FIXME: Resolve dir()=="auto" against the document.
    return dir() == "rtl" ? RTL : LTR;
}

String Notification::permissionString(WebNotificationPermission permission)
{
    switch (permission) {
    case WebNotificationPermissionAllowed:
        return "granted";
    case WebNotificationPermissionDenied:
        return "denied";
    case WebNotificationPermissionDefault:
        return "default";
    }

    ASSERT_NOT_REACHED();
    return "denied";
}

String Notification::permission(ExecutionContext* context)
{
    return permissionString(checkPermission(context));
}

WebNotificationPermission Notification::checkPermission(ExecutionContext* context)
{
    SecurityOrigin* origin = context->securityOrigin();
    ASSERT(origin);

    return notificationManager()->checkPermission(WebSerializedOrigin(*origin));
}

void Notification::requestPermission(ExecutionContext* context, NotificationPermissionCallback* callback)
{
    // FIXME: Assert that this code-path will only be reached for Document environments
    // when Blink supports [Exposed] annotations on class members in IDL definitions.
    if (NotificationPermissionClient* permissionClient = NotificationPermissionClient::from(context))
        permissionClient->requestPermission(context, callback);
}

bool Notification::dispatchEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    ASSERT(executionContext()->isContextThread());
    return EventTarget::dispatchEvent(event);
}

const AtomicString& Notification::interfaceName() const
{
    return EventTargetNames::Notification;
}

void Notification::stop()
{
    notificationManager()->notifyDelegateDestroyed(this);

    m_state = NotificationStateClosed;

    m_asyncRunner.stop();
}

bool Notification::hasPendingActivity() const
{
    return m_state == NotificationStateShowing || m_asyncRunner.isActive();
}

ScriptValue Notification::data(ScriptState* scriptState) const
{
    if (!m_serializedData)
        return ScriptValue::createNull(scriptState);

    return ScriptValue(scriptState, m_serializedData->deserialize(scriptState->isolate()));
}

DEFINE_TRACE(Notification)
{
    RefCountedGarbageCollectedEventTargetWithInlineData<Notification>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
