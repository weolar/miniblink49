/*
 * Copyright (C) 2012 Google, Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
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
 */

#include "config.h"
#include "core/fetch/FetchRequest.h"

#include "core/fetch/CrossOriginAccessControl.h"
#include "core/fetch/ResourceFetcher.h"

namespace blink {

FetchRequest::FetchRequest(const ResourceRequest& resourceRequest, const AtomicString& initiator, const String& charset, ResourceLoadPriority priority)
    : m_resourceRequest(resourceRequest)
    , m_charset(charset)
    , m_options(ResourceFetcher::defaultResourceOptions())
    , m_priority(priority)
    , m_forPreload(false)
    , m_defer(NoDefer)
    , m_originRestriction(UseDefaultOriginRestrictionForType)
{
    m_options.initiatorInfo.name = initiator;
}

FetchRequest::FetchRequest(const ResourceRequest& resourceRequest, const AtomicString& initiator, const ResourceLoaderOptions& options)
    : m_resourceRequest(resourceRequest)
    , m_options(options)
    , m_priority(ResourceLoadPriorityUnresolved)
    , m_forPreload(false)
    , m_defer(NoDefer)
    , m_originRestriction(UseDefaultOriginRestrictionForType)
{
    m_options.initiatorInfo.name = initiator;
}

FetchRequest::FetchRequest(const ResourceRequest& resourceRequest, const FetchInitiatorInfo& initiator)
    : m_resourceRequest(resourceRequest)
    , m_options(ResourceFetcher::defaultResourceOptions())
    , m_priority(ResourceLoadPriorityUnresolved)
    , m_forPreload(false)
    , m_defer(NoDefer)
    , m_originRestriction(UseDefaultOriginRestrictionForType)
{
    m_options.initiatorInfo = initiator;
}

FetchRequest::~FetchRequest()
{
}

void FetchRequest::setCrossOriginAccessControl(SecurityOrigin* origin, StoredCredentials allowCredentials, CredentialRequest requested)
{
    ASSERT(requested == ClientDidNotRequestCredentials || allowCredentials == AllowStoredCredentials);
    m_resourceRequest.setFetchRequestMode(WebURLRequest::FetchRequestModeCORS);
    updateRequestForAccessControl(m_resourceRequest, origin, allowCredentials);
    m_options.allowCredentials = allowCredentials;
    m_options.corsEnabled = IsCORSEnabled;
    m_options.securityOrigin = origin;
    m_options.credentialsRequested = requested;
}

void FetchRequest::setCrossOriginAccessControl(SecurityOrigin* origin, StoredCredentials allowCredentials)
{
    setCrossOriginAccessControl(origin, allowCredentials, allowCredentials == AllowStoredCredentials ? ClientRequestedCredentials : ClientDidNotRequestCredentials);
}

void FetchRequest::setCrossOriginAccessControl(SecurityOrigin* origin, const AtomicString& crossOriginMode)
{
    setCrossOriginAccessControl(origin, equalIgnoringCase(crossOriginMode, "use-credentials") ? AllowStoredCredentials : DoNotAllowStoredCredentials);
}

void FetchRequest::setResourceWidth(ResourceWidth resourceWidth)
{
    if (resourceWidth.isSet) {
        m_resourceWidth.width = resourceWidth.width;
        m_resourceWidth.isSet = true;
    }
}

} // namespace blink
