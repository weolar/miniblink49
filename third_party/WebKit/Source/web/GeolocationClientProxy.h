/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GeolocationClientProxy_h
#define GeolocationClientProxy_h

#include "modules/geolocation/GeolocationClient.h"
#include "platform/heap/Handle.h"
#include "public/web/WebGeolocationController.h"

namespace blink {

class GeolocationPosition;
class WebGeolocationClient;

class GeolocationClientProxy final : public GeolocationClient {
public:
    static PassOwnPtrWillBeRawPtr<GeolocationClientProxy> create(WebGeolocationClient* client)
    {
        return adoptPtrWillBeNoop(new GeolocationClientProxy(client));
    }

    ~GeolocationClientProxy() override;
    void setController(GeolocationController*);
    void startUpdating() override;
    void stopUpdating() override;
    void setEnableHighAccuracy(bool) override;
    GeolocationPosition* lastPosition() override;

    void requestPermission(Geolocation*) override;
    void cancelPermissionRequest(Geolocation*) override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit GeolocationClientProxy(WebGeolocationClient*);

    WebGeolocationClient* m_client;
    PersistentWillBeMember<GeolocationPosition> m_lastPosition;
};

} // namespace blink

#endif // GeolocationClientProxy_h
