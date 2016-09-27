// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerResponseError_h
#define WebServiceWorkerResponseError_h

namespace blink {

// This enum is used in UMA histograms, so don't change the order or remove
// entries.
enum WebServiceWorkerResponseError {
    WebServiceWorkerResponseErrorUnknown,
    WebServiceWorkerResponseErrorPromiseRejected,
    WebServiceWorkerResponseErrorDefaultPrevented,
    WebServiceWorkerResponseErrorNoV8Instance,
    WebServiceWorkerResponseErrorResponseTypeError,
    WebServiceWorkerResponseErrorResponseTypeOpaque,
    WebServiceWorkerResponseErrorResponseTypeNotBasicOrDefault,
    WebServiceWorkerResponseErrorBodyUsed,
    WebServiceWorkerResponseErrorLast = WebServiceWorkerResponseErrorBodyUsed
};

} // namespace blink

#endif // WebServiceWorkerResponseError_h
