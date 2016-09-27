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

#include "config.h"
#include "core/fetch/FetchContext.h"


namespace blink {

FetchContext& FetchContext::nullInstance()
{
    DEFINE_STATIC_LOCAL(Persistent<FetchContext>, instance, (new FetchContext));
    return *instance;
}

void FetchContext::dispatchDidChangeResourcePriority(unsigned long, ResourceLoadPriority, int)
{
}

void FetchContext::addAdditionalRequestHeaders(ResourceRequest&, FetchResourceType)
{
}

void FetchContext::setFirstPartyForCookies(ResourceRequest&)
{
}

CachePolicy FetchContext::cachePolicy() const
{
    return CachePolicyVerify;
}

ResourceRequestCachePolicy FetchContext::resourceRequestCachePolicy(const ResourceRequest&, Resource::Type) const
{
    return UseProtocolCachePolicy;
}

void FetchContext::dispatchWillSendRequest(unsigned long, ResourceRequest&, const ResourceResponse&, const FetchInitiatorInfo&)
{
}

void FetchContext::dispatchDidLoadResourceFromMemoryCache(const ResourceRequest&, const ResourceResponse&)
{
}

void FetchContext::dispatchDidReceiveResponse(unsigned long, const ResourceResponse&, ResourceLoader*)
{
}

void FetchContext::dispatchDidReceiveData(unsigned long, const char*, int, int)
{
}

void FetchContext::dispatchDidDownloadData(unsigned long, int, int)
{
}

void FetchContext::dispatchDidFinishLoading(unsigned long, double, int64_t)
{
}

void FetchContext::dispatchDidFail(unsigned long, const ResourceError&, bool)
{
}

void FetchContext::sendRemainingDelegateMessages(unsigned long, const ResourceResponse&, int)
{
}

void FetchContext::dispatchWillRequestResource(FetchRequest*)
{
}

void FetchContext::willStartLoadingResource(ResourceRequest&)
{
}

void FetchContext::didLoadResource()
{
}

void FetchContext::addResourceTiming(const ResourceTimingInfo&)
{
}

void FetchContext::sendImagePing(const KURL&)
{
}

void FetchContext::addConsoleMessage(const String&) const
{
}

void FetchContext::upgradeInsecureRequest(FetchRequest&)
{
}

void FetchContext::addClientHintsIfNecessary(FetchRequest&)
{
}

void FetchContext::addCSPHeaderIfNecessary(Resource::Type, FetchRequest&)
{
}

} // namespace blink
