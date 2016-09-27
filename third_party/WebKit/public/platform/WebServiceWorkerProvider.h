/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebServiceWorkerProvider_h
#define WebServiceWorkerProvider_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/WebServiceWorkerRegistration.h"
#include "public/platform/WebVector.h"

namespace blink {

class WebURL;
class WebServiceWorker;
class WebServiceWorkerProviderClient;
struct WebServiceWorkerError;

// Created on the main thread, and may be passed to another script context
// thread (e.g. worker thread) later. All methods of this class must be called
// on the single script context thread.
class WebServiceWorkerProvider {
public:
    // Called when a client wants to start listening to the service worker
    // events. Must be cleared before the client becomes invalid.
    virtual void setClient(WebServiceWorkerProviderClient*) { }

    // The WebServiceWorkerRegistration and WebServiceWorkerError ownership are
    // passed to the WebServiceWorkerRegistrationCallbacks implementation.
    typedef WebCallbacks<WebServiceWorkerRegistration, WebServiceWorkerError> WebServiceWorkerRegistrationCallbacks;
    typedef WebCallbacks<WebServiceWorkerRegistration, WebServiceWorkerError> WebServiceWorkerGetRegistrationCallbacks;
    typedef WebCallbacks<WebVector<WebServiceWorkerRegistration*>, WebServiceWorkerError> WebServiceWorkerGetRegistrationsCallbacks;
    typedef WebCallbacks<WebServiceWorkerRegistration, void> WebServiceWorkerGetRegistrationForReadyCallbacks;

    virtual void registerServiceWorker(const WebURL& pattern, const WebURL& scriptUrl, WebServiceWorkerRegistrationCallbacks*) { }
    virtual void getRegistration(const WebURL& documentURL, WebServiceWorkerGetRegistrationCallbacks*) { }
    virtual void getRegistrations(WebServiceWorkerGetRegistrationsCallbacks*) { }
    virtual void getRegistrationForReady(WebServiceWorkerGetRegistrationForReadyCallbacks*) { }

    virtual ~WebServiceWorkerProvider() { }
};

} // namespace blink

#endif
