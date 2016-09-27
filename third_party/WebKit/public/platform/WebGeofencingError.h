// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGeofencingError_h
#define WebGeofencingError_h

#include "WebString.h"

namespace blink {

struct WebGeofencingError {
    enum ErrorType {
        ErrorTypeAbort = 0,
        ErrorTypeUnknown,
        ErrorTypeLast = ErrorTypeUnknown
    };

    WebGeofencingError(ErrorType errorType, const WebString& message)
        : errorType(errorType)
        , message(message)
    {
    }

    ErrorType errorType;
    WebString message;
};

} // namespace blink

#endif // WebGeofencingError_h
