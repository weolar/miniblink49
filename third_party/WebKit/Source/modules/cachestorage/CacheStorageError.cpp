// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/cachestorage/CacheStorageError.h"

#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/cachestorage/Cache.h"
#include "public/platform/WebServiceWorkerCacheError.h"

namespace blink {

DOMException* CacheStorageError::take(ScriptPromiseResolver*, PassOwnPtr<WebType> webError)
{
    return CacheStorageError::createException(*webError);
}

DOMException* CacheStorageError::createException(WebType webError)
{
    switch (webError) {
    case WebServiceWorkerCacheErrorNotImplemented:
        return DOMException::create(NotSupportedError, "Method is not implemented.");
    case WebServiceWorkerCacheErrorNotFound:
        return DOMException::create(NotFoundError, "Entry was not found.");
    case WebServiceWorkerCacheErrorExists:
        return DOMException::create(InvalidAccessError, "Entry already exists.");
    default:
        ASSERT_NOT_REACHED();
        return DOMException::create(NotSupportedError, "Unknown error.");
    }
}

} // namespace blink
