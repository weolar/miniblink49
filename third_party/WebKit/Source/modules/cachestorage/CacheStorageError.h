// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CacheStorageError_h
#define CacheStorageError_h

#include "public/platform/WebServiceWorkerCacheError.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class DOMException;
class ScriptPromiseResolver;

class CacheStorageError {
    WTF_MAKE_NONCOPYABLE(CacheStorageError);
public:
    // For CallbackPromiseAdapter. Ownership of a given error is not
    // transferred.
    using WebType = WebServiceWorkerCacheError;
    static DOMException* take(ScriptPromiseResolver*, PassOwnPtr<WebType> webError);

    static DOMException* createException(WebType webError);
};

} // namespace blink

#endif // CacheStorageError_h
