// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PushSubscriptionCallbacks_h
#define PushSubscriptionCallbacks_h

#include "platform/heap/Handle.h"
#include "public/platform/WebCallbacks.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class ServiceWorkerRegistration;
class ScriptPromiseResolver;
struct WebPushError;
struct WebPushSubscription;

// PushSubscriptionCallbacks is an implementation of WebPushSubscriptionCallbacks
// that will resolve the underlying promise depending on the result passed to
// the callback. It takes a ServiceWorkerRegistration in its constructor and
// will pass it to the PushSubscription.
class PushSubscriptionCallbacks final : public WebCallbacks<WebPushSubscription, WebPushError> {
    WTF_MAKE_NONCOPYABLE(PushSubscriptionCallbacks);
public:
    PushSubscriptionCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver>, ServiceWorkerRegistration*);
    ~PushSubscriptionCallbacks() override;

    void onSuccess(WebPushSubscription*) override;
    void onError(WebPushError*) override;

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
    Persistent<ServiceWorkerRegistration> m_serviceWorkerRegistration;
};

} // namespace blink

#endif // PushSubscriptionCallbacks_h
