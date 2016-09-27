// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebNotificationManager_h
#define WebNotificationManager_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"
#include "public/platform/modules/notifications/WebNotificationData.h"
#include "public/platform/modules/notifications/WebNotificationPermission.h"
#include <stdint.h>

namespace blink {

class WebNotificationDelegate;
class WebSerializedOrigin;
class WebServiceWorkerRegistration;

// Structure representing the info associated with a persistent notification.
struct WebPersistentNotificationInfo {
    int64_t persistentId = 0;
    WebNotificationData data;
};

using WebNotificationGetCallbacks = WebCallbacks<WebVector<WebPersistentNotificationInfo>, void>;
using WebNotificationShowCallbacks = WebCallbacks<void, void>;

// Provides the services to show platform notifications to the user.
class WebNotificationManager {
public:
    virtual ~WebNotificationManager() { }

    // Shows a page notification on the user's system. These notifications will have their
    // events delivered to the delegate specified in this call.
    virtual void show(const WebSerializedOrigin&, const WebNotificationData&, WebNotificationDelegate*) = 0;

    // Shows a persistent notification on the user's system. These notifications will have
    // their events delivered to a Service Worker rather than the object's delegate. Will
    // take ownership of the WebNotificationShowCallbacks object.
    virtual void showPersistent(const WebSerializedOrigin&, const WebNotificationData&, WebServiceWorkerRegistration*, WebNotificationShowCallbacks*) = 0;

    // Asynchronously gets the persistent notifications belonging to the Service Worker Registration.
    // If |filterTag| is not an empty string, only the notification with the given tag will be
    // considered. Will take ownership of the WebNotificationGetCallbacks object.
    virtual void getNotifications(const WebString& filterTag, WebServiceWorkerRegistration*, WebNotificationGetCallbacks*) = 0;

    // Closes a notification previously shown, and removes it if being shown.
    virtual void close(WebNotificationDelegate*) = 0;

    // Closes a persistent notification identified by its persistent notification Id.
    virtual void closePersistent(const WebSerializedOrigin&, int64_t persistentNotificationId) = 0;

    // Indicates that the delegate object is being destroyed, and must no longer
    // be used by the embedder to dispatch events.
    virtual void notifyDelegateDestroyed(WebNotificationDelegate*) = 0;

    // Synchronously checks the permission level for the given origin.
    virtual WebNotificationPermission checkPermission(const WebSerializedOrigin&) = 0;
};

} // namespace blink

#endif // WebNotificationManager_h
