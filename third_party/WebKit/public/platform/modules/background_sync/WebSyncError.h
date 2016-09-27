// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSyncError_h
#define WebSyncError_h

#include "public/platform/WebString.h"

namespace blink {

struct WebSyncError {
    enum ErrorType {
        ErrorTypeAbort = 0,
        ErrorTypeNoPermission,
        ErrorTypeNotFound,
        ErrorTypeUnknown,
        ErrorTypeLast = ErrorTypeUnknown
    };

    WebSyncError(ErrorType errorType, const WebString& message)
        : errorType(errorType)
        , message(message)
    {
    }

    ErrorType errorType;
    WebString message;
};

} // namespace blink

#endif // WebSyncError_h
