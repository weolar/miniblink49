/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebContentDecryptionModuleSession_h
#define WebContentDecryptionModuleSession_h

#include "WebCommon.h"
#include "WebVector.h"
#include "public/platform/WebContentDecryptionModuleException.h"
#include "public/platform/WebContentDecryptionModuleResult.h"
#include "public/platform/WebEncryptedMediaTypes.h"

namespace blink {

class WebEncryptedMediaKeyInformation;
class WebString;
class WebURL;

class BLINK_PLATFORM_EXPORT WebContentDecryptionModuleSession {
public:
    class BLINK_PLATFORM_EXPORT Client {
    public:
        enum class MessageType {
            LicenseRequest,
            LicenseRenewal,
            LicenseRelease
        };

        virtual void message(MessageType, const unsigned char* message, size_t messageLength) = 0;
        virtual void close() = 0;

        // Called when the expiration time for the session changes.
        // |updatedExpiryTimeInMS| is specified as the number of milliseconds
        // since 01 January, 1970 UTC.
        virtual void expirationChanged(double updatedExpiryTimeInMS) = 0;

        // Called when the set of keys for this session changes or existing keys
        // change state. |hasAdditionalUsableKey| is set if a key is newly
        // usable (e.g. new key available, previously expired key has been
        // renewed, etc.) and the browser should attempt to resume playback
        // if necessary.
        virtual void keysStatusesChange(const WebVector<WebEncryptedMediaKeyInformation>&, bool hasAdditionalUsableKey) = 0;

    protected:
        virtual ~Client();
    };

    virtual ~WebContentDecryptionModuleSession();

    virtual void setClientInterface(Client*) = 0;
    virtual WebString sessionId() const = 0;

    virtual void initializeNewSession(WebEncryptedMediaInitDataType, const unsigned char* initData, size_t initDataLength, WebEncryptedMediaSessionType, WebContentDecryptionModuleResult) = 0;
    virtual void load(const WebString& sessionId, WebContentDecryptionModuleResult) = 0;
    virtual void update(const unsigned char* response, size_t responseLength, WebContentDecryptionModuleResult) = 0;
    virtual void close(WebContentDecryptionModuleResult) = 0;
    virtual void remove(WebContentDecryptionModuleResult) = 0;
};

} // namespace blink

#endif // WebContentDecryptionModuleSession_h
