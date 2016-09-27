/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2012 Apple Inc. All Rights Reserved.
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
#include "GeolocationClientMock.h"

#include "modules/geolocation/GeolocationController.h"
#include "modules/geolocation/GeolocationError.h"
#include "modules/geolocation/GeolocationPosition.h"

namespace blink {

GeolocationClientMock::GeolocationClientMock()
    : m_hasError(false)
    , m_controllerTimer(this, &GeolocationClientMock::controllerTimerFired)
    , m_permissionTimer(this, &GeolocationClientMock::permissionTimerFired)
    , m_isActive(false)
    , m_permissionState(PermissionStateUnset)
{
}

GeolocationClientMock::~GeolocationClientMock()
{
    ASSERT(!m_isActive);
}

void GeolocationClientMock::setPosition(GeolocationPosition* position)
{
    m_lastPosition = position;
    clearError();
    asyncUpdateController();
}

void GeolocationClientMock::setPositionUnavailableError(const String& errorMessage)
{
    m_hasError = true;
    m_errorMessage = errorMessage;
    m_lastPosition = nullptr;
    asyncUpdateController();
}

void GeolocationClientMock::setPermission(bool allowed)
{
    m_permissionState = allowed ? PermissionStateAllowed : PermissionStateDenied;
    asyncUpdatePermission();
}

int GeolocationClientMock::numberOfPendingPermissionRequests() const
{
    return m_pendingPermissions.size();
}

void GeolocationClientMock::requestPermission(Geolocation* geolocation)
{
    m_pendingPermissions.add(geolocation);
    if (m_permissionState != PermissionStateUnset)
        asyncUpdatePermission();
}

void GeolocationClientMock::cancelPermissionRequest(Geolocation* geolocation)
{
    // Called from Geolocation::disconnectFrame() in response to LocalFrame destruction.
    m_pendingPermissions.remove(geolocation);
    if (m_pendingPermissions.isEmpty() && m_permissionTimer.isActive())
        m_permissionTimer.stop();
}

void GeolocationClientMock::controllerForTestAdded(GeolocationController* controller)
{
    m_controllers.add(controller);
}

void GeolocationClientMock::controllerForTestRemoved(GeolocationController* controller)
{
    m_controllers.remove(controller);
}

void GeolocationClientMock::asyncUpdatePermission()
{
    ASSERT(m_permissionState != PermissionStateUnset);
    if (!m_permissionTimer.isActive())
        m_permissionTimer.startOneShot(0, FROM_HERE);
}

void GeolocationClientMock::permissionTimerFired(Timer<GeolocationClientMock>* timer)
{
    ASSERT_UNUSED(timer, timer == &m_permissionTimer);
    ASSERT(m_permissionState != PermissionStateUnset);
    bool allowed = m_permissionState == PermissionStateAllowed;
    GeolocationSet::iterator end = m_pendingPermissions.end();

    // Once permission has been set (or denied) on a Geolocation object, there can be
    // no further requests for permission to the mock. Consequently the callbacks
    // which fire synchronously from Geolocation::setIsAllowed() cannot reentrantly modify
    // m_pendingPermissions.
    for (GeolocationSet::iterator it = m_pendingPermissions.begin(); it != end; ++it)
        (*it)->setIsAllowed(allowed);
    m_pendingPermissions.clear();
}

void GeolocationClientMock::startUpdating()
{
    ASSERT(!m_isActive);
    m_isActive = true;
    asyncUpdateController();
}

void GeolocationClientMock::stopUpdating()
{
    ASSERT(m_isActive);
    m_isActive = false;
    m_controllerTimer.stop();
}

void GeolocationClientMock::setEnableHighAccuracy(bool)
{
    // FIXME: We need to add some tests regarding "high accuracy" mode.
    // See https://bugs.webkit.org/show_bug.cgi?id=49438
}

GeolocationPosition* GeolocationClientMock::lastPosition()
{
    return m_lastPosition.get();
}

void GeolocationClientMock::asyncUpdateController()
{
    if (m_isActive && !m_controllerTimer.isActive())
        m_controllerTimer.startOneShot(0, FROM_HERE);
}

void GeolocationClientMock::controllerTimerFired(Timer<GeolocationClientMock>* timer)
{
    ASSERT_UNUSED(timer, timer == &m_controllerTimer);

    // Make a copy of the set of controllers since it might be modified while iterating.
    GeolocationControllers controllers = m_controllers;
    if (m_lastPosition.get()) {
        ASSERT(!m_hasError);
        for (GeolocationControllers::iterator it = controllers.begin(); it != controllers.end(); ++it)
            (*it)->positionChanged(m_lastPosition.get());
    } else if (m_hasError) {
        for (GeolocationControllers::iterator it = controllers.begin(); it != controllers.end(); ++it)
            (*it)->errorOccurred(GeolocationError::create(GeolocationError::PositionUnavailable, m_errorMessage));
    }
}

void GeolocationClientMock::clearError()
{
    m_hasError = false;
    m_errorMessage = String();
}

DEFINE_TRACE(GeolocationClientMock)
{
#if ENABLE(OILPAN)
    visitor->trace(m_controllers);
#endif
    visitor->trace(m_lastPosition);
    visitor->trace(m_pendingPermissions);
    GeolocationClient::trace(visitor);
}

} // namespace blink
