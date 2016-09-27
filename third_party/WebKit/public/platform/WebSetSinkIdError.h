// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSetSinkIdError_h
#define WebSetSinkIdError_h

#include "public/platform/WebString.h"

namespace blink {

struct WebSetSinkIdError {
    enum ErrorType {
        ErrorTypeNotFound = 1,
        ErrorTypeSecurity,
        ErrorTypeAbort,
        ErrorTypeNotSupported,
        ErrorTypeLast = ErrorTypeNotSupported
    };

    WebSetSinkIdError(ErrorType errorType, const WebString& message)
        : errorType(errorType)
        , message(message)
    {
    }

    ErrorType errorType;
    WebString message;
};

} // namespace blink

#endif // WebSetSinkIdError_h
