// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPushProvider_h
#define WebPushProvider_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/modules/push_messaging/WebPushPermissionStatus.h"

namespace blink {

class WebServiceWorkerRegistration;
struct WebPushError;
struct WebPushSubscription;
struct WebPushSubscriptionOptions;

using WebPushSubscriptionCallbacks = WebCallbacks<WebPushSubscription, WebPushError>;
using WebPushPermissionStatusCallbacks = WebCallbacks<WebPushPermissionStatus, WebPushError>;
using WebPushUnsubscribeCallbacks = WebCallbacks<bool, WebPushError>;

class WebPushProvider {
public:
    virtual ~WebPushProvider() { }

    // Takes ownership of the WebPushSubscriptionCallbacks.
    // Does not take ownership of the WebServiceWorkerRegistration.
    virtual void subscribe(WebServiceWorkerRegistration*, const WebPushSubscriptionOptions&, WebPushSubscriptionCallbacks*) = 0;

    // Takes ownership of the WebPushSubscriptionCallbacks.
    // Does not take ownership of the WebServiceWorkerRegistration.
    virtual void getSubscription(WebServiceWorkerRegistration*, WebPushSubscriptionCallbacks*) = 0;

    // Takes ownership of the WebPushPermissionStatusCallbacks.
    // Does not take ownership of the WebServiceWorkerRegistration.
    virtual void getPermissionStatus(WebServiceWorkerRegistration*, const WebPushSubscriptionOptions&, WebPushPermissionStatusCallbacks*) = 0;

    // Takes ownership if the WebPushUnsubscribeCallbacks.
    // Does not take ownership of the WebServiceWorkerRegistration.
    virtual void unsubscribe(WebServiceWorkerRegistration*, WebPushUnsubscribeCallbacks*) = 0;
};

} // namespace blink

#endif // WebPushProvider_h
