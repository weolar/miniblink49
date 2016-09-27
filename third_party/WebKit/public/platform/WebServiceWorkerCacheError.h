// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerCacheError_h
#define WebServiceWorkerCacheError_h

namespace blink {

enum WebServiceWorkerCacheError {
    WebServiceWorkerCacheErrorNotImplemented,
    WebServiceWorkerCacheErrorNotFound,
    WebServiceWorkerCacheErrorExists,
    WebServiceWorkerCacheErrorLast = WebServiceWorkerCacheErrorExists
};

} // namespace blink

#endif // WebServiceWorkerCacheError_h
