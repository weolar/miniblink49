// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebEncryptedMediaRequest_h
#define WebEncryptedMediaRequest_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebPrivatePtr.h"
#include "public/platform/WebString.h"

namespace blink {

class EncryptedMediaRequest;
class WebContentDecryptionModuleAccess;
struct WebMediaKeySystemConfiguration;
class WebSecurityOrigin;
template <typename T> class WebVector;

class WebEncryptedMediaRequest {
public:
    BLINK_PLATFORM_EXPORT WebEncryptedMediaRequest(const WebEncryptedMediaRequest&);
    BLINK_PLATFORM_EXPORT ~WebEncryptedMediaRequest();

    BLINK_PLATFORM_EXPORT WebString keySystem() const;
    BLINK_PLATFORM_EXPORT const WebVector<WebMediaKeySystemConfiguration>& supportedConfigurations() const;

    BLINK_PLATFORM_EXPORT WebSecurityOrigin securityOrigin() const;

    BLINK_PLATFORM_EXPORT void requestSucceeded(WebContentDecryptionModuleAccess*);
    BLINK_PLATFORM_EXPORT void requestNotSupported(const WebString& errorMessage);

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT explicit WebEncryptedMediaRequest(EncryptedMediaRequest*);
#endif

private:
    void assign(const WebEncryptedMediaRequest&);
    void reset();

    WebPrivatePtr<EncryptedMediaRequest> m_private;
};

} // namespace blink

#endif // WebEncryptedMediaRequest_h
