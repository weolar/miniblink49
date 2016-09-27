// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerRegistration_h
#define WebServiceWorkerRegistration_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/WebURL.h"

namespace blink {

class WebServiceWorkerProvider;
class WebServiceWorkerRegistrationProxy;
struct WebServiceWorkerError;

class WebServiceWorkerRegistration {
public:
    virtual ~WebServiceWorkerRegistration() { }

    using WebServiceWorkerUnregistrationCallbacks = WebCallbacks<bool, WebServiceWorkerError>;

    virtual void setProxy(WebServiceWorkerRegistrationProxy*) { }
    virtual WebServiceWorkerRegistrationProxy* proxy() { return nullptr; }
    virtual void proxyStopped() { }

    virtual WebURL scope() const { return WebURL(); }
    virtual void update(WebServiceWorkerProvider*) { }
    virtual void unregister(WebServiceWorkerProvider*, WebServiceWorkerUnregistrationCallbacks*) { }
};

} // namespace blink

#endif // WebServiceWorkerRegistration_h
