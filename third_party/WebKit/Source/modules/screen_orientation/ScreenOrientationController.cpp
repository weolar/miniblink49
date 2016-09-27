// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/screen_orientation/ScreenOrientationController.h"

#include "core/events/Event.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "modules/screen_orientation/ScreenOrientation.h"
#include "modules/screen_orientation/ScreenOrientationDispatcher.h"
#include "platform/LayoutTestSupport.h"
#include "public/platform/WebScreenInfo.h"
#include "public/platform/modules/screen_orientation/WebScreenOrientationClient.h"

namespace blink {

ScreenOrientationController::~ScreenOrientationController()
{
}

void ScreenOrientationController::provideTo(LocalFrame& frame, WebScreenOrientationClient* client)
{
    ASSERT(RuntimeEnabledFeatures::screenOrientationEnabled());

    ScreenOrientationController* controller = new ScreenOrientationController(frame, client);
    WillBeHeapSupplement<LocalFrame>::provideTo(frame, supplementName(), adoptPtrWillBeNoop(controller));
}

ScreenOrientationController* ScreenOrientationController::from(LocalFrame& frame)
{
    return static_cast<ScreenOrientationController*>(WillBeHeapSupplement<LocalFrame>::from(frame, supplementName()));
}

ScreenOrientationController::ScreenOrientationController(LocalFrame& frame, WebScreenOrientationClient* client)
    : LocalFrameLifecycleObserver(&frame)
    , PlatformEventController(frame.page())
    , m_client(client)
    , m_dispatchEventTimer(this, &ScreenOrientationController::dispatchEventTimerFired)
    , m_override(false)
    , m_overrideType(WebScreenOrientationUndefined)
    , m_overrideAngle(0)
{
}

const char* ScreenOrientationController::supplementName()
{
    return "ScreenOrientationController";
}

// Compute the screen orientation using the orientation angle and the screen width / height.
WebScreenOrientationType ScreenOrientationController::computeOrientation(const IntRect& rect, uint16_t rotation)
{
    // Bypass orientation detection in layout tests to get consistent results.
    // FIXME: The screen dimension should be fixed when running the layout tests to avoid such
    // issues.
    if (LayoutTestSupport::isRunningLayoutTest())
        return WebScreenOrientationPortraitPrimary;

    bool isTallDisplay = rotation % 180 ? rect.height() < rect.width() : rect.height() > rect.width();
    switch (rotation) {
    case 0:
        return isTallDisplay ? WebScreenOrientationPortraitPrimary : WebScreenOrientationLandscapePrimary;
    case 90:
        return isTallDisplay ? WebScreenOrientationLandscapePrimary : WebScreenOrientationPortraitSecondary;
    case 180:
        return isTallDisplay ? WebScreenOrientationPortraitSecondary : WebScreenOrientationLandscapeSecondary;
    case 270:
        return isTallDisplay ? WebScreenOrientationLandscapeSecondary : WebScreenOrientationPortraitPrimary;
    default:
        ASSERT_NOT_REACHED();
        return WebScreenOrientationPortraitPrimary;
    }
}

unsigned short ScreenOrientationController::effectiveAngle(ChromeClient& chromeClient)
{
    return m_override ? m_overrideAngle : chromeClient.screenInfo().orientationAngle;
}

WebScreenOrientationType ScreenOrientationController::effectiveType(ChromeClient& chromeClient)
{
    return m_override ? m_overrideType : chromeClient.screenInfo().orientationType;
}

void ScreenOrientationController::updateOrientation()
{
    ASSERT(m_orientation);
    ASSERT(frame());
    ASSERT(frame()->host());

    ChromeClient& chromeClient = frame()->host()->chromeClient();
    WebScreenOrientationType orientationType = effectiveType(chromeClient);
    if (orientationType == WebScreenOrientationUndefined) {
        // The embedder could not provide us with an orientation, deduce it ourselves.
        orientationType = computeOrientation(chromeClient.screenInfo().rect, effectiveAngle(chromeClient));
    }
    ASSERT(orientationType != WebScreenOrientationUndefined);

    m_orientation->setType(orientationType);
    m_orientation->setAngle(effectiveAngle(chromeClient));
}

bool ScreenOrientationController::isActiveAndVisible() const
{
    return m_orientation && frame() && page() && page()->visibilityState() == PageVisibilityStateVisible;
}

void ScreenOrientationController::pageVisibilityChanged()
{
    notifyDispatcher();

    if (!isActiveAndVisible())
        return;

    // The orientation type and angle are tied in a way that if the angle has
    // changed, the type must have changed.
    unsigned short currentAngle = effectiveAngle(frame()->host()->chromeClient());

    // FIXME: sendOrientationChangeEvent() currently send an event all the
    // children of the frame, so it should only be called on the frame on
    // top of the tree. We would need the embedder to call
    // sendOrientationChangeEvent on every WebFrame part of a WebView to be
    // able to remove this.
    if (frame() == frame()->localFrameRoot() && m_orientation->angle() != currentAngle)
        notifyOrientationChanged();
}

void ScreenOrientationController::notifyOrientationChanged()
{
    ASSERT(RuntimeEnabledFeatures::screenOrientationEnabled());

    if (!isActiveAndVisible())
        return;

    updateOrientation();

    // Keep track of the frames that need to be notified before notifying the
    // current frame as it will prevent side effects from the change event
    // handlers.
    WillBeHeapVector<RefPtrWillBeMember<LocalFrame>> childFrames;
    for (Frame* child = frame()->tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame())
            childFrames.append(toLocalFrame(child));
    }

    // Notify current orientation object.
    if (!m_dispatchEventTimer.isActive())
        m_dispatchEventTimer.startOneShot(0, FROM_HERE);

    // ... and child frames, if they have a ScreenOrientationController.
    for (size_t i = 0; i < childFrames.size(); ++i) {
        if (ScreenOrientationController* controller = ScreenOrientationController::from(*childFrames[i]))
            controller->notifyOrientationChanged();
    }
}

void ScreenOrientationController::setOrientation(ScreenOrientation* orientation)
{
    m_orientation = orientation;
    if (m_orientation)
        updateOrientation();
    notifyDispatcher();
}

void ScreenOrientationController::lock(WebScreenOrientationLockType orientation, WebLockOrientationCallback* callback)
{
    // When detached, the client is no longer valid.
    if (!m_client)
        return;
    m_client->lockOrientation(orientation, callback);
}

void ScreenOrientationController::unlock()
{
    // When detached, the client is no longer valid.
    if (!m_client)
        return;
    m_client->unlockOrientation();
}

void ScreenOrientationController::setOverride(WebScreenOrientationType type, unsigned short angle)
{
    m_override = true;
    m_overrideType = type;
    m_overrideAngle = angle;
    notifyOrientationChanged();
}

void ScreenOrientationController::clearOverride()
{
    m_override = false;
    notifyOrientationChanged();
}

void ScreenOrientationController::dispatchEventTimerFired(Timer<ScreenOrientationController>*)
{
    if (!m_orientation)
        return;
    m_orientation->dispatchEvent(Event::create(EventTypeNames::change));
}

void ScreenOrientationController::didUpdateData()
{
    // Do nothing.
}

void ScreenOrientationController::registerWithDispatcher()
{
    ScreenOrientationDispatcher::instance().addController(this);
}

void ScreenOrientationController::unregisterWithDispatcher()
{
    ScreenOrientationDispatcher::instance().removeController(this);
}

bool ScreenOrientationController::hasLastData()
{
    return true;
}

void ScreenOrientationController::willDetachFrameHost()
{
    m_client = nullptr;
}

void ScreenOrientationController::notifyDispatcher()
{
    if (m_orientation && page()->visibilityState() == PageVisibilityStateVisible)
        startUpdating();
    else
        stopUpdating();
}

DEFINE_TRACE(ScreenOrientationController)
{
    visitor->trace(m_orientation);
    LocalFrameLifecycleObserver::trace(visitor);
    WillBeHeapSupplement<LocalFrame>::trace(visitor);
    PlatformEventController::trace(visitor);
}

} // namespace blink
