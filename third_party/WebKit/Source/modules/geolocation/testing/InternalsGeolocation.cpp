/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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
#include "InternalsGeolocation.h"

#include "core/dom/Document.h"
#include "core/page/Page.h"
#include "core/testing/Internals.h"
#include "modules/geolocation/GeolocationController.h"
#include "modules/geolocation/GeolocationError.h"
#include "modules/geolocation/GeolocationPosition.h"
#include "modules/geolocation/testing/GeolocationClientMock.h"
#include "wtf/CurrentTime.h"

namespace blink {

void InternalsGeolocation::setGeolocationClientMock(Internals&, Document* document)
{
    ASSERT(document);
    if (!document->frame())
        return;

    GeolocationClientMock* client = new GeolocationClientMock();
    for (Frame* childFrame = document->page()->mainFrame(); childFrame; childFrame = childFrame->tree().traverseNext()) {
        if (childFrame->isLocalFrame())
            GeolocationController::from(toLocalFrame(childFrame))->setClientForTest(client);
    }
}

void InternalsGeolocation::setGeolocationPosition(Internals&, Document* document, double latitude, double longitude, double accuracy)
{
    ASSERT(document);
    if (!document->frame())
        return;

    GeolocationClientMock* client = geolocationClient(document);
    if (!client)
        return;
    client->setPosition(GeolocationPosition::create(currentTime(), latitude, longitude, accuracy));
}

void InternalsGeolocation::setGeolocationPositionUnavailableError(Internals&, Document* document, const String& message)
{
    ASSERT(document);
    if (!document->frame())
        return;

    GeolocationClientMock* client = geolocationClient(document);
    if (!client)
        return;
    client->setPositionUnavailableError(message);
}

void InternalsGeolocation::setGeolocationPermission(Internals&, Document* document, bool allowed)
{
    ASSERT(document);
    if (!document->frame())
        return;

    GeolocationClientMock* client = geolocationClient(document);
    if (!client)
        return;
    client->setPermission(allowed);
}

int InternalsGeolocation::numberOfPendingGeolocationPermissionRequests(Internals&, Document* document)
{
    ASSERT(document);
    if (!document->frame())
        return -1;

    GeolocationClientMock* client = geolocationClient(document);
    if (!client)
        return -1;
    return client->numberOfPendingPermissionRequests();
}

GeolocationClientMock* InternalsGeolocation::geolocationClient(Document* document)
{
    ASSERT(document);
    if (!document->frame())
        return nullptr;

    GeolocationController* controller = GeolocationController::from(document->frame());
    if (!controller->hasClientForTest())
        return nullptr;
    return static_cast<GeolocationClientMock*>(controller->client());
}

} // namespace blink
