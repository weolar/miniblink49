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

#ifndef WebStorageQuotaCallbacks_h
#define WebStorageQuotaCallbacks_h

#include "WebCommon.h"
#include "WebPrivatePtr.h"
#include "WebStorageQuotaError.h"

namespace blink {

class StorageQuotaCallbacks;

class WebStorageQuotaCallbacks {
public:
    ~WebStorageQuotaCallbacks() { reset(); }
    WebStorageQuotaCallbacks() { }
    WebStorageQuotaCallbacks(const WebStorageQuotaCallbacks& c) { assign(c); }
    WebStorageQuotaCallbacks& operator=(const WebStorageQuotaCallbacks& c)
    {
        assign(c);
        return *this;
    }

    BLINK_PLATFORM_EXPORT void reset();
    BLINK_PLATFORM_EXPORT void assign(const WebStorageQuotaCallbacks&);

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebStorageQuotaCallbacks(StorageQuotaCallbacks*);
#endif

    // Callback for WebFrameClient::queryStorageUsageAndQuota.
    BLINK_PLATFORM_EXPORT void didQueryStorageUsageAndQuota(unsigned long long usageInBytes, unsigned long long quotaInBytes);

    // Callback for WebFrameClient::requestStorageQuota.
    // This may return a smaller amount of quota than the requested.
    BLINK_PLATFORM_EXPORT void didGrantStorageQuota(unsigned long long usageInBytes, unsigned long long grantedQuotaInBytes);

    BLINK_PLATFORM_EXPORT void didFail(WebStorageQuotaError);

private:
    WebPrivatePtr<StorageQuotaCallbacks> m_private;
};

} // namespace blink

#endif // WebStorageQuotaCallbacks_h
