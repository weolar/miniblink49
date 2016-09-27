// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPushSubscription_h
#define WebPushSubscription_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"

namespace blink {

struct WebPushSubscription {
    explicit WebPushSubscription(const WebURL& endpoint)
        : endpoint(endpoint)
    {
    }

    // TODO(peter): Remove this constructor when the embedder switched over to the one above.
    BLINK_PLATFORM_EXPORT WebPushSubscription(const WebString& endpointWithoutSubscriptionId, const WebString& subscriptionId);

    WebURL endpoint;
};

} // namespace blink

#endif // WebPushSubscription_h
