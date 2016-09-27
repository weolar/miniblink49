/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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

#include "modules/geolocation/GeolocationController.h"

#include "core/page/Page.h"
#include "modules/geolocation/GeolocationClient.h"
#include "modules/geolocation/GeolocationError.h"
#include "modules/geolocation/GeolocationPosition.h"

namespace blink {

GeolocationController::GeolocationController(LocalFrame& frame, GeolocationClient* client)
    : PageLifecycleObserver(frame.page())
    , m_client(client)
    , m_hasClientForTest(false)
    , m_isClientUpdating(false)
{
    if (!frame.isMainFrame() && frame.page()->mainFrame()->isLocalFrame()) {
        // internals.setGeolocationClientMock is per page.
        GeolocationController* mainController = GeolocationController::from(frame.page()->deprecatedLocalMainFrame());
        if (mainController->hasClientForTest())
            setClientForTest(mainController->client());
    }
}

void GeolocationController::startUpdatingIfNeeded()
{
    if (m_isClientUpdating)
        return;
    m_isClientUpdating = true;
    m_client->startUpdating();
}

void GeolocationController::stopUpdatingIfNeeded()
{
    if (!m_isClientUpdating)
        return;
    m_isClientUpdating = false;
    m_client->stopUpdating();
}

GeolocationController::~GeolocationController()
{
    ASSERT(m_observers.isEmpty());
#if !ENABLE(OILPAN)
    if (m_hasClientForTest) {
        m_client->controllerForTestRemoved(this);
        m_hasClientForTest = false;
    }
#endif
}

PassOwnPtrWillBeRawPtr<GeolocationController> GeolocationController::create(LocalFrame& frame, GeolocationClient* client)
{
    return adoptPtrWillBeNoop(new GeolocationController(frame, client));
}

void GeolocationController::addObserver(Geolocation* observer, bool enableHighAccuracy)
{
    // This may be called multiple times with the same observer, though removeObserver()
    // is called only once with each.
    bool wasEmpty = m_observers.isEmpty();
    m_observers.add(observer);
    if (enableHighAccuracy)
        m_highAccuracyObservers.add(observer);

    if (m_client) {
        if (enableHighAccuracy)
            m_client->setEnableHighAccuracy(true);
        if (wasEmpty && page() && page()->visibilityState() == PageVisibilityStateVisible)
            startUpdatingIfNeeded();
    }
}

void GeolocationController::removeObserver(Geolocation* observer)
{
    if (!m_observers.contains(observer))
        return;

    m_observers.remove(observer);
    m_highAccuracyObservers.remove(observer);

    if (m_client) {
        if (m_observers.isEmpty())
            stopUpdatingIfNeeded();
        else if (m_highAccuracyObservers.isEmpty())
            m_client->setEnableHighAccuracy(false);
    }
}

void GeolocationController::requestPermission(Geolocation* geolocation)
{
    if (m_client)
        m_client->requestPermission(geolocation);
}

void GeolocationController::cancelPermissionRequest(Geolocation* geolocation)
{
    if (m_client)
        m_client->cancelPermissionRequest(geolocation);
}

void GeolocationController::positionChanged(GeolocationPosition* position)
{
    if (!position) {
        errorOccurred(GeolocationError::create(GeolocationError::PositionUnavailable, "PositionUnavailable"));
        return;
    }
    m_lastPosition = position;
    HeapVector<Member<Geolocation>> observersVector;
    copyToVector(m_observers, observersVector);
    for (size_t i = 0; i < observersVector.size(); ++i)
        observersVector[i]->positionChanged();
}

void GeolocationController::errorOccurred(GeolocationError* error)
{
    HeapVector<Member<Geolocation>> observersVector;
    copyToVector(m_observers, observersVector);
    for (size_t i = 0; i < observersVector.size(); ++i)
        observersVector[i]->setError(error);
}

GeolocationPosition* GeolocationController::lastPosition()
{
    if (m_lastPosition.get())
        return m_lastPosition.get();

    if (!m_client)
        return 0;

    return m_client->lastPosition();
}

void GeolocationController::setClientForTest(GeolocationClient* client)
{
    if (m_hasClientForTest)
        m_client->controllerForTestRemoved(this);
    m_client = client;
    m_hasClientForTest = true;

    client->controllerForTestAdded(this);
}

void GeolocationController::pageVisibilityChanged()
{
    if (m_observers.isEmpty() || !m_client)
        return;

    if (page() && page()->visibilityState() == PageVisibilityStateVisible)
        startUpdatingIfNeeded();
    else
        stopUpdatingIfNeeded();
}

const char* GeolocationController::supplementName()
{
    return "GeolocationController";
}

DEFINE_TRACE(GeolocationController)
{
    visitor->trace(m_client);
    visitor->trace(m_lastPosition);
    visitor->trace(m_observers);
    visitor->trace(m_highAccuracyObservers);
    WillBeHeapSupplement<LocalFrame>::trace(visitor);
    PageLifecycleObserver::trace(visitor);
}

void provideGeolocationTo(LocalFrame& frame, GeolocationClient* client)
{
    WillBeHeapSupplement<LocalFrame>::provideTo(frame, GeolocationController::supplementName(), GeolocationController::create(frame, client));
}

} // namespace blink
