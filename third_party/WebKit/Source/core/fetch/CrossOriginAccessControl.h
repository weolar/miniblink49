/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef CrossOriginAccessControl_h
#define CrossOriginAccessControl_h

#include "core/CoreExport.h"
#include "core/fetch/ResourceLoaderOptions.h"
#include "platform/network/ResourceRequest.h"
#include "wtf/Forward.h"
#include "wtf/HashSet.h"

namespace blink {

typedef HashSet<String, CaseFoldingHash> HTTPHeaderSet;

class Resource;
struct ResourceLoaderOptions;
class ResourceRequest;
class ResourceResponse;
class SecurityOrigin;

class CrossOriginAccessControl {
public:
    // Given the new request URL, returns true if
    // - the URL has a CORS supported scheme and
    // - the URL does not contain the userinfo production.
    static bool isLegalRedirectLocation(const KURL&, String& errorDescription);
    static bool handleRedirect(SecurityOrigin*, ResourceRequest&, const ResourceResponse&, StoredCredentials, ResourceLoaderOptions&, String&);
};

CORE_EXPORT bool isOnAccessControlResponseHeaderWhitelist(const String&);

void updateRequestForAccessControl(ResourceRequest&, SecurityOrigin*, StoredCredentials);
ResourceRequest createAccessControlPreflightRequest(const ResourceRequest&, SecurityOrigin*);

bool passesAccessControlCheck(const ResourceResponse&, StoredCredentials, SecurityOrigin*, String& errorDescription);
bool passesPreflightStatusCheck(const ResourceResponse&, String& errorDescription);
CORE_EXPORT void parseAccessControlExposeHeadersAllowList(const String& headerValue, HTTPHeaderSet&);

} // namespace blink

#endif // CrossOriginAccessControl_h
