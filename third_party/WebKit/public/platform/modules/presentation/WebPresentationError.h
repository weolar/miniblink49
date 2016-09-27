// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPresentationError_h
#define WebPresentationError_h

#include "public/platform/WebString.h"

namespace blink {

struct WebPresentationError {
    enum ErrorType {
        ErrorTypeNoAvailableScreens = 0,
        ErrorTypeSessionRequestCancelled,
        ErrorTypeNoPresentationFound,
        ErrorTypeAvailabilityNotSupported,
        ErrorTypeUnknown,
        ErrorTypeLast = ErrorTypeUnknown
    };

    WebPresentationError(ErrorType errorType, const WebString& message)
        : errorType(errorType)
        , message(message)
    {
    }

    ErrorType errorType;
    WebString message;
};

} // namespace blink

#endif // WebPresentationError_h
