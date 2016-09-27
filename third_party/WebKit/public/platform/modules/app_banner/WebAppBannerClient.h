// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebAppBannerClient_h
#define WebAppBannerClient_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/modules/app_banner/WebAppBannerPromptResult.h"

namespace blink {

using WebAppBannerCallbacks = WebCallbacks<WebAppBannerPromptResult, void>;

class WebAppBannerClient {
public:
    virtual ~WebAppBannerClient() { }

    // Ownership of the callbacks is transferred to the client.
    virtual void registerBannerCallbacks(int requestId, WebAppBannerCallbacks*) = 0;

    // Tell the client that the event is being redispatched.
    virtual void showAppBanner(int requestId) { }
};

} // namespace blink

#endif // WebAppBannerClient_h
