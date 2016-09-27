// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSyncClient_h
#define WebSyncClient_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/modules/background_sync/WebSyncError.h"

namespace blink {

class WebServiceWorkerRegistration;
struct WebSyncRegistration;

using WebSyncRegistrationCallbacks = WebCallbacks<WebSyncRegistration, WebSyncError>;

class WebSyncClient {
public:
    virtual ~WebSyncClient() { }

    // Ownership of the WebServiceWorkerRegistration is not transferred.
    // Ownership of the callbacks is transferred to the client.
    virtual void registerSync(WebServiceWorkerRegistration*, WebSyncRegistrationCallbacks*) = 0;
};

} // namespace blink

#endif // WebSyncClient_h
