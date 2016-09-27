// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebContentDecryptionModuleException_h
#define WebContentDecryptionModuleException_h

namespace blink {

enum WebContentDecryptionModuleException {
    WebContentDecryptionModuleExceptionNotSupportedError,
    WebContentDecryptionModuleExceptionInvalidStateError,
    WebContentDecryptionModuleExceptionInvalidAccessError,
    WebContentDecryptionModuleExceptionQuotaExceededError,
    WebContentDecryptionModuleExceptionUnknownError,
    WebContentDecryptionModuleExceptionClientError,
    WebContentDecryptionModuleExceptionOutputError,
};

} // namespace blink

#endif // WebContentDecryptionModuleException_h
