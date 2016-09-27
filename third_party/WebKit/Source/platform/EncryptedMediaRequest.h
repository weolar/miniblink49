// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EncryptedMediaRequest_h
#define EncryptedMediaRequest_h

#include "platform/heap/Handle.h"

namespace blink {

class SecurityOrigin;
class WebContentDecryptionModuleAccess;
struct WebMediaKeySystemConfiguration;
class WebString;
template <typename T> class WebVector;

class EncryptedMediaRequest : public GarbageCollectedFinalized<EncryptedMediaRequest> {
public:
    virtual ~EncryptedMediaRequest() { }

    virtual WebString keySystem() const = 0;
    virtual const WebVector<WebMediaKeySystemConfiguration>& supportedConfigurations() const = 0;

    virtual SecurityOrigin* securityOrigin() const = 0;

    virtual void requestSucceeded(WebContentDecryptionModuleAccess*) = 0;
    virtual void requestNotSupported(const WebString& errorMessage) = 0;

    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

} // namespace blink

#endif // EncryptedMediaRequest_h
