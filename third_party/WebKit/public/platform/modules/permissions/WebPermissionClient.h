// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPermissionClient_h
#define WebPermissionClient_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/modules/permissions/WebPermissionStatus.h"
#include "public/platform/modules/permissions/WebPermissionType.h"

namespace blink {

using WebPermissionQueryCallback = WebCallbacks<WebPermissionStatus, void>;

class WebPermissionObserver;
class WebURL;

// This client is expected to do general permission handling. From simple
// permission query to requesting new permissions.
class WebPermissionClient {
public:
    // Query the permission status of a given origin for a specific permission.
    virtual void queryPermission(WebPermissionType, const WebURL&, WebPermissionQueryCallback*) { }

    // Listen for permission changes for a given origin and inform the observer
    // when they happen. The observer is not owned by the WebPermissionClient.
    // The client should expect stopListening() to be called when the observer
    // will be destroyed.
    virtual void startListening(WebPermissionType, const WebURL&, WebPermissionObserver*) { }

    // Stop listening to all the permission changes associated with the given
    // observer.
    virtual void stopListening(WebPermissionObserver*) { }

protected:
    virtual ~WebPermissionClient() { }
};

} // namespace blink

#endif // WebPermissionClient_h
