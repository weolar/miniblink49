// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerClientQueryOptions_h
#define WebServiceWorkerClientQueryOptions_h

#include "public/platform/WebServiceWorkerClientType.h"

namespace blink {

struct WebServiceWorkerClientQueryOptions {
    WebServiceWorkerClientQueryOptions()
        : clientType(WebServiceWorkerClientTypeWindow)
        , includeUncontrolled(false)
    {
    }

    WebServiceWorkerClientType clientType;
    bool includeUncontrolled;
};

} // namespace blink

#endif // WebServiceWorkerClientQueryOptions_h
