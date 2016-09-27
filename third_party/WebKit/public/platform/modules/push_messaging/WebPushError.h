// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPushError_h
#define WebPushError_h

#include "public/platform/WebString.h"

namespace blink {

struct WebPushError {
    enum ErrorType {
        ErrorTypeAbort = 0,
        ErrorTypeNetwork,
        ErrorTypeNotFound,
        ErrorTypeNotSupported,
        ErrorTypeUnknown,
        ErrorTypeLast = ErrorTypeUnknown
    };

    WebPushError(ErrorType errorType, const WebString& message)
        : errorType(errorType)
        , message(message)
    {
    }

    ErrorType errorType;
    WebString message;
};

} // namespace blink

#endif // WebPushError_h
