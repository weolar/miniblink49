// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCredentialManagerError_h
#define WebCredentialManagerError_h

namespace blink {

struct WebCredentialManagerError {
    // FIXME: This is a placeholder list of error conditions. We'll likely expand the
    // list as the API evolves.
    enum ErrorType {
        ErrorTypeDisabled = 0,
        ErrorTypePendingRequest,
        ErrorTypePasswordStoreUnavailable,
        ErrorTypeUnknown,
        ErrorTypeLast = ErrorTypeUnknown
    };

    // FIXME: We need this to be an object for the moment to make the WebCallbacks templates
    // happy. But, really, we probably just need the enum. We should fix that.
    explicit WebCredentialManagerError(ErrorType type) : errorType(type) { }

    ErrorType errorType;
};

} // namespace blink

#endif
