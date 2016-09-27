// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCredentialManagerClient_h
#define WebCredentialManagerClient_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/WebCredentialManagerError.h"
#include "public/platform/WebVector.h"

namespace blink {

class WebCredential;
class WebURL;

// WebCredentialManagerClient is an interface which allows an embedder to
// implement 'navigator.credential.*' calls which are defined in the
// 'credentialmanager' module.
class WebCredentialManagerClient {
public:
    typedef WebCallbacks<WebCredential, WebCredentialManagerError> RequestCallbacks;
    typedef WebCallbacks<void, WebCredentialManagerError> NotificationCallbacks;

    // Ownership of the callback is transferred to the callee for each of
    // the following methods.
    virtual void dispatchFailedSignIn(const WebCredential&, NotificationCallbacks*) { }
    virtual void dispatchSignedIn(const WebCredential&, NotificationCallbacks*) { }
    virtual void dispatchRequireUserMediation(NotificationCallbacks*) { }
    virtual void dispatchRequest(bool zeroClickOnly, const WebVector<WebURL>& federations, RequestCallbacks*) { }
};

} // namespace blink

#endif // WebCredentialManager_h
