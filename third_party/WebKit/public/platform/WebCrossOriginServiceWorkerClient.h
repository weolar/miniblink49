// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCrossOriginServiceWorkerClient_h
#define WebCrossOriginServiceWorkerClient_h

#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/WebURL.h"

namespace blink {

struct WebCrossOriginServiceWorkerClient {
    WebURL targetURL;
    WebURL origin;
    int clientID;

    WebCrossOriginServiceWorkerClient()
        : clientID(-1)
    {
    }
};

} // namespace blink

#endif // WebCrossOriginServiceWorkerClient_h
