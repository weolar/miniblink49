// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/background_sync/SyncError.h"

#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "wtf/OwnPtr.h"

namespace blink {

DOMException* SyncError::take(ScriptPromiseResolver*, WebType* webErrorRaw)
{
    OwnPtr<WebType> webError = adoptPtr(webErrorRaw);
    switch (webError->errorType) {
    case WebSyncError::ErrorTypeAbort:
        return DOMException::create(AbortError, webError->message);
    case WebSyncError::ErrorTypeNoPermission:
        return DOMException::create(InvalidAccessError, webError->message);
    case WebSyncError::ErrorTypeNotFound:
        return DOMException::create(NotFoundError, webError->message);
    case WebSyncError::ErrorTypeUnknown:
        return DOMException::create(UnknownError, webError->message);
    }
    ASSERT_NOT_REACHED();
    return DOMException::create(UnknownError);
}

void SyncError::dispose(WebType* webErrorRaw)
{
    delete webErrorRaw;
}

} // namespace blink
