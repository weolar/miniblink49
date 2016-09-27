// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerEventResult_h
#define WebServiceWorkerEventResult_h

namespace blink {

// Indicates how the service worker handled an event.
enum WebServiceWorkerEventResult {
    // The event dispatch completed with no rejections.
    WebServiceWorkerEventResultCompleted = 0,
    // The service worker associated the event with a promise that was rejected
    // (e.g., the promise passed to waitUntil for an install event was rejected).
    WebServiceWorkerEventResultRejected,
    WebServiceWorkerEventResultLast = WebServiceWorkerEventResultRejected
};

} // namespace blink

#endif // WebServiceWorkerEventResult_h
