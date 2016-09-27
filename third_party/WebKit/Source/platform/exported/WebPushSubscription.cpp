// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/modules/push_messaging/WebPushSubscription.h"

#include "platform/weborigin/KURL.h"
#include "wtf/text/WTFString.h"

namespace blink {

WebPushSubscription::WebPushSubscription(const WebString& endpointWithoutSubscriptionId, const WebString& subscriptionId)
{
    String concatenatedEndpoint = endpointWithoutSubscriptionId;
    concatenatedEndpoint.append("/");
    concatenatedEndpoint.append(subscriptionId);

    endpoint = KURL(ParsedURLString, concatenatedEndpoint);
}

} // namespace blink
