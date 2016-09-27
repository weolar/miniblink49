/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Google, Inc. ("Google") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/weborigin/SecurityPolicy.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/OriginAccessEntry.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/MainThread.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Threading.h"
#include "wtf/text/StringHash.h"

namespace blink {

using OriginAccessWhiteList = Vector<OriginAccessEntry>;
using OriginAccessMap = HashMap<String, OwnPtr<OriginAccessWhiteList>>;
using OriginSet = HashSet<String>;

static OriginAccessMap& originAccessMap()
{
    DEFINE_STATIC_LOCAL(OriginAccessMap, originAccessMap, ());
    return originAccessMap;
}

static OriginSet& trustworthyOriginSet()
{
    DEFINE_STATIC_LOCAL(OriginSet, trustworthyOriginSet, ());
    return trustworthyOriginSet;
}

void SecurityPolicy::init()
{
    originAccessMap();
    trustworthyOriginSet();
}

bool SecurityPolicy::shouldHideReferrer(const KURL& url, const String& referrer)
{
    bool referrerIsSecureURL = protocolIs(referrer, "https");
    bool referrerIsWebURL = referrerIsSecureURL || protocolIs(referrer, "http");

    if (!referrerIsWebURL)
        return true;

    if (!referrerIsSecureURL)
        return false;

    bool URLIsSecureURL = url.protocolIs("https");

    return !URLIsSecureURL;
}

Referrer SecurityPolicy::generateReferrer(ReferrerPolicy referrerPolicy, const KURL& url, const String& referrer)
{
    if (referrer.isEmpty())
        return Referrer(String(), referrerPolicy);

    if (!(protocolIs(referrer, "https") || protocolIs(referrer, "http")))
        return Referrer(String(), referrerPolicy);

    if (SecurityOrigin::shouldUseInnerURL(url))
        return Referrer(String(), referrerPolicy);

    switch (referrerPolicy) {
    case ReferrerPolicyNever:
        return Referrer(String(), referrerPolicy);
    case ReferrerPolicyAlways:
        return Referrer(referrer, referrerPolicy);
    case ReferrerPolicyOrigin: {
        String origin = SecurityOrigin::createFromString(referrer)->toString();
        // A security origin is not a canonical URL as it lacks a path. Add /
        // to turn it into a canonical URL we can use as referrer.
        return Referrer(origin + "/", referrerPolicy);
    }
    case ReferrerPolicyOriginWhenCrossOrigin: {
        RefPtr<SecurityOrigin> referrerOrigin = SecurityOrigin::createFromString(referrer);
        RefPtr<SecurityOrigin> urlOrigin = SecurityOrigin::create(url);
        if (!urlOrigin->isSameSchemeHostPort(referrerOrigin.get())) {
            String origin = referrerOrigin->toString();
            return Referrer(origin + "/", referrerPolicy);
        }
        break;
    }
    case ReferrerPolicyDefault: {
        // If the flag is enabled, and we're dealing with a cross-origin request, strip it.
        // Otherwise fallthrough to NoReferrerWhenDowngrade behavior.
        RefPtr<SecurityOrigin> referrerOrigin = SecurityOrigin::createFromString(referrer);
        RefPtr<SecurityOrigin> urlOrigin = SecurityOrigin::create(url);
        if (RuntimeEnabledFeatures::reducedReferrerGranularityEnabled() && !urlOrigin->isSameSchemeHostPort(referrerOrigin.get())) {
            String origin = referrerOrigin->toString();
            return Referrer(shouldHideReferrer(url, referrer) ? String() : origin + "/", referrerPolicy);
        }
        break;
    }
    case ReferrerPolicyNoReferrerWhenDowngrade:
        break;
    }

    return Referrer(shouldHideReferrer(url, referrer) ? String() : referrer, referrerPolicy);
}

void SecurityPolicy::addOriginTrustworthyWhiteList(PassRefPtr<SecurityOrigin> origin)
{
    // Must be called before we start other threads.
    ASSERT(WTF::isBeforeThreadCreated());
    if (origin->isUnique())
        return;
    trustworthyOriginSet().add(origin->toRawString());
}

bool SecurityPolicy::isOriginWhiteListedTrustworthy(const SecurityOrigin& origin)
{
    if (origin.isUnique())
        return false;
    return trustworthyOriginSet().contains(origin.toRawString());
}

bool SecurityPolicy::isAccessWhiteListed(const SecurityOrigin* activeOrigin, const SecurityOrigin* targetOrigin)
{
    if (OriginAccessWhiteList* list = originAccessMap().get(activeOrigin->toString())) {
        for (size_t i = 0; i < list->size();  ++i) {
            if (list->at(i).matchesOrigin(*targetOrigin) != OriginAccessEntry::DoesNotMatchOrigin)
                return true;
        }
    }
    return false;
}

bool SecurityPolicy::isAccessToURLWhiteListed(const SecurityOrigin* activeOrigin, const KURL& url)
{
    RefPtr<SecurityOrigin> targetOrigin = SecurityOrigin::create(url);
    return isAccessWhiteListed(activeOrigin, targetOrigin.get());
}

void SecurityPolicy::addOriginAccessWhitelistEntry(const SecurityOrigin& sourceOrigin, const String& destinationProtocol, const String& destinationDomain, bool allowDestinationSubdomains)
{
    ASSERT(isMainThread());
    ASSERT(!sourceOrigin.isUnique());
    if (sourceOrigin.isUnique())
        return;

    String sourceString = sourceOrigin.toString();
    OriginAccessMap::AddResult result = originAccessMap().add(sourceString, nullptr);
    if (result.isNewEntry)
        result.storedValue->value = adoptPtr(new OriginAccessWhiteList);

    OriginAccessWhiteList* list = result.storedValue->value.get();
    list->append(OriginAccessEntry(destinationProtocol, destinationDomain, allowDestinationSubdomains ? OriginAccessEntry::AllowSubdomains : OriginAccessEntry::DisallowSubdomains));
}

void SecurityPolicy::removeOriginAccessWhitelistEntry(const SecurityOrigin& sourceOrigin, const String& destinationProtocol, const String& destinationDomain, bool allowDestinationSubdomains)
{
    ASSERT(isMainThread());
    ASSERT(!sourceOrigin.isUnique());
    if (sourceOrigin.isUnique())
        return;

    String sourceString = sourceOrigin.toString();
    OriginAccessMap& map = originAccessMap();
    OriginAccessMap::iterator it = map.find(sourceString);
    if (it == map.end())
        return;

    OriginAccessWhiteList* list = it->value.get();
    size_t index = list->find(OriginAccessEntry(destinationProtocol, destinationDomain, allowDestinationSubdomains ? OriginAccessEntry::AllowSubdomains : OriginAccessEntry::DisallowSubdomains));

    if (index == kNotFound)
        return;

    list->remove(index);

    if (list->isEmpty())
        map.remove(it);
}

void SecurityPolicy::resetOriginAccessWhitelists()
{
    ASSERT(isMainThread());
    originAccessMap().clear();
}

} // namespace blink
