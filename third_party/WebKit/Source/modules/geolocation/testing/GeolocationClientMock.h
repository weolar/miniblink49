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

#ifndef GeolocationClientMock_h
#define GeolocationClientMock_h

#include "modules/geolocation/GeolocationClient.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "wtf/HashSet.h"
#include "wtf/text/WTFString.h"

namespace blink {

class GeolocationController;
class GeolocationPosition;

// Provides a mock object for the geolocation client.
class GeolocationClientMock : public GeolocationClient {
public:
    GeolocationClientMock();
    ~GeolocationClientMock() override;

    void setPosition(GeolocationPosition*);
    void setPositionUnavailableError(const String& errorMessage);
    void setPermission(bool allowed);
    int numberOfPendingPermissionRequests() const;

    // GeolocationClient
    void startUpdating() override;
    void stopUpdating() override;
    void setEnableHighAccuracy(bool) override;
    GeolocationPosition* lastPosition() override;
    void requestPermission(Geolocation*) override;
    void cancelPermissionRequest(Geolocation*) override;
    void controllerForTestAdded(GeolocationController*) override;
    void controllerForTestRemoved(GeolocationController*) override;

    DECLARE_VIRTUAL_TRACE();

private:
    void asyncUpdateController();
    void controllerTimerFired(Timer<GeolocationClientMock>*);

    void asyncUpdatePermission();
    void permissionTimerFired(Timer<GeolocationClientMock>*);

    void clearError();

    typedef WillBeHeapHashSet<RawPtrWillBeWeakMember<GeolocationController>> GeolocationControllers;
    GeolocationControllers m_controllers;

    PersistentWillBeMember<GeolocationPosition> m_lastPosition;
    bool m_hasError;
    String m_errorMessage;
    Timer<GeolocationClientMock> m_controllerTimer;
    Timer<GeolocationClientMock> m_permissionTimer;
    bool m_isActive;

    enum PermissionState {
        PermissionStateUnset,
        PermissionStateAllowed,
        PermissionStateDenied
    };

    PermissionState m_permissionState;

    typedef PersistentHeapHashSetWillBeHeapHashSet<Member<Geolocation>> GeolocationSet;
    GeolocationSet m_pendingPermissions;
};

}

#endif
