/*
 * Copyright (C) 2010 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE, INC. ``AS IS'' AND ANY
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

#include "config.h"
#include "platform/weborigin/SchemeRegistry.h"

#include "wtf/ThreadSpecific.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

static Mutex& mutex()
{
    // The first call to this should be made before or during blink
    // initialization to avoid racy static local initialization.
    DEFINE_STATIC_LOCAL(Mutex, m, ());
    return m;
}

static void assertLockHeld()
{
#if ENABLE(ASSERT)
    ASSERT(mutex().locked());
#endif
}

static URLSchemesSet& localURLSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, localSchemes, ());

    if (localSchemes.isEmpty())
        localSchemes.add("file");

    return localSchemes;
}

static URLSchemesSet& displayIsolatedURLSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, displayIsolatedSchemes, ());
    return displayIsolatedSchemes;
}

static URLSchemesSet& mixedContentRestrictingSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, mixedContentRestrictingSchemes, ());

    if (mixedContentRestrictingSchemes.isEmpty())
        mixedContentRestrictingSchemes.add("https");

    return mixedContentRestrictingSchemes;
}

static URLSchemesSet& secureSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, secureSchemes, ());

    if (secureSchemes.isEmpty()) {
        secureSchemes.add("https");
        secureSchemes.add("about");
        secureSchemes.add("data");
        secureSchemes.add("wss");
    }

    return secureSchemes;
}

static URLSchemesSet& schemesWithUniqueOrigins()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, schemesWithUniqueOrigins, ());

    if (schemesWithUniqueOrigins.isEmpty()) {
        schemesWithUniqueOrigins.add("about");
        schemesWithUniqueOrigins.add("javascript");
        // This is a willful violation of HTML5.
        // See https://bugs.webkit.org/show_bug.cgi?id=11885
        schemesWithUniqueOrigins.add("data");
    }

    return schemesWithUniqueOrigins;
}

static URLSchemesSet& emptyDocumentSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, emptyDocumentSchemes, ());

    if (emptyDocumentSchemes.isEmpty())
        emptyDocumentSchemes.add("about");

    return emptyDocumentSchemes;
}

static HashSet<String>& schemesForbiddenFromDomainRelaxation()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(HashSet<String>, schemes, ());
    return schemes;
}

static URLSchemesSet& notAllowingJavascriptURLsSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, notAllowingJavascriptURLsSchemes, ());
    return notAllowingJavascriptURLsSchemes;
}

void SchemeRegistry::registerURLSchemeAsLocal(const String& scheme)
{
    MutexLocker locker(mutex());
    localURLSchemes().add(scheme);
}

const URLSchemesSet& SchemeRegistry::localSchemes()
{
    MutexLocker locker(mutex());
    return localURLSchemes();
}

static URLSchemesSet& CORSEnabledSchemes()
{
    // FIXME: http://bugs.webkit.org/show_bug.cgi?id=77160
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, CORSEnabledSchemes, ());

    if (CORSEnabledSchemes.isEmpty()) {
        CORSEnabledSchemes.add("http");
        CORSEnabledSchemes.add("https");
        CORSEnabledSchemes.add("data");
    }

    return CORSEnabledSchemes;
}

static URLSchemesSet& serviceWorkerSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesSet, serviceWorkerSchemes, ());

    if (serviceWorkerSchemes.isEmpty()) {
        // HTTP is required because http://localhost is considered secure.
        // Additional checks are performed to ensure that other http pages
        // are filtered out.
        serviceWorkerSchemes.add("http");
        serviceWorkerSchemes.add("https");
    }

    return serviceWorkerSchemes;
}

static URLSchemesMap<SchemeRegistry::PolicyAreas>& ContentSecurityPolicyBypassingSchemes()
{
    assertLockHeld();
    DEFINE_STATIC_LOCAL_NOASSERT(URLSchemesMap<SchemeRegistry::PolicyAreas>, schemes, ());
    return schemes;
}

bool SchemeRegistry::shouldTreatURLSchemeAsLocal(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return localURLSchemes().contains(scheme);
}

void SchemeRegistry::registerURLSchemeAsNoAccess(const String& scheme)
{
    MutexLocker locker(mutex());
    schemesWithUniqueOrigins().add(scheme);
}

bool SchemeRegistry::shouldTreatURLSchemeAsNoAccess(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return schemesWithUniqueOrigins().contains(scheme);
}

void SchemeRegistry::registerURLSchemeAsDisplayIsolated(const String& scheme)
{
    MutexLocker locker(mutex());
    displayIsolatedURLSchemes().add(scheme);
}

bool SchemeRegistry::shouldTreatURLSchemeAsDisplayIsolated(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return displayIsolatedURLSchemes().contains(scheme);
}

void SchemeRegistry::registerURLSchemeAsRestrictingMixedContent(const String& scheme)
{
    MutexLocker locker(mutex());
    mixedContentRestrictingSchemes().add(scheme);
}

bool SchemeRegistry::shouldTreatURLSchemeAsRestrictingMixedContent(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return mixedContentRestrictingSchemes().contains(scheme);
}

void SchemeRegistry::registerURLSchemeAsSecure(const String& scheme)
{
    MutexLocker locker(mutex());
    secureSchemes().add(scheme);
}

bool SchemeRegistry::shouldTreatURLSchemeAsSecure(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return secureSchemes().contains(scheme);
}

void SchemeRegistry::registerURLSchemeAsEmptyDocument(const String& scheme)
{
    MutexLocker locker(mutex());
    emptyDocumentSchemes().add(scheme);
}

bool SchemeRegistry::shouldLoadURLSchemeAsEmptyDocument(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return emptyDocumentSchemes().contains(scheme);
}

void SchemeRegistry::setDomainRelaxationForbiddenForURLScheme(bool forbidden, const String& scheme)
{
    if (scheme.isEmpty())
        return;

    MutexLocker locker(mutex());
    if (forbidden)
        schemesForbiddenFromDomainRelaxation().add(scheme);
    else
        schemesForbiddenFromDomainRelaxation().remove(scheme);
}

bool SchemeRegistry::isDomainRelaxationForbiddenForURLScheme(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return schemesForbiddenFromDomainRelaxation().contains(scheme);
}

bool SchemeRegistry::canDisplayOnlyIfCanRequest(const String& scheme)
{
    return equalIgnoringCase("blob", scheme) || equalIgnoringCase("filesystem", scheme);
}

void SchemeRegistry::registerURLSchemeAsNotAllowingJavascriptURLs(const String& scheme)
{
    MutexLocker locker(mutex());
    notAllowingJavascriptURLsSchemes().add(scheme);
}

bool SchemeRegistry::shouldTreatURLSchemeAsNotAllowingJavascriptURLs(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return notAllowingJavascriptURLsSchemes().contains(scheme);
}

void SchemeRegistry::registerURLSchemeAsCORSEnabled(const String& scheme)
{
    MutexLocker locker(mutex());
    CORSEnabledSchemes().add(scheme);
}

bool SchemeRegistry::shouldTreatURLSchemeAsCORSEnabled(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return CORSEnabledSchemes().contains(scheme);
}

String SchemeRegistry::listOfCORSEnabledURLSchemes()
{
    StringBuilder builder;
    bool addSeparator = false;
    URLSchemesSet schemes;
    {
        MutexLocker locker(mutex());
        schemes = CORSEnabledSchemes();
    }
    for (const auto& scheme : schemes) {
        if (addSeparator)
            builder.appendLiteral(", ");
        else
            addSeparator = true;

        builder.append(scheme);
    }
    return builder.toString();
}

bool SchemeRegistry::shouldTreatURLSchemeAsLegacy(const String& scheme)
{
    return equalIgnoringCase("ftp", scheme) || equalIgnoringCase("gopher", scheme);
}

void SchemeRegistry::registerURLSchemeAsAllowingServiceWorkers(const String& scheme)
{
    MutexLocker locker(mutex());
    serviceWorkerSchemes().add(scheme);
}

bool SchemeRegistry::shouldTreatURLSchemeAsAllowingServiceWorkers(const String& scheme)
{
    if (scheme.isEmpty())
        return false;
    MutexLocker locker(mutex());
    return serviceWorkerSchemes().contains(scheme);
}

void SchemeRegistry::registerURLSchemeAsBypassingContentSecurityPolicy(const String& scheme, PolicyAreas policyAreas)
{
    MutexLocker locker(mutex());
    ContentSecurityPolicyBypassingSchemes().add(scheme, policyAreas);
}

void SchemeRegistry::removeURLSchemeRegisteredAsBypassingContentSecurityPolicy(const String& scheme)
{
    MutexLocker locker(mutex());
    ContentSecurityPolicyBypassingSchemes().remove(scheme);
}

bool SchemeRegistry::schemeShouldBypassContentSecurityPolicy(const String& scheme, PolicyAreas policyAreas)
{
    ASSERT(policyAreas != PolicyAreaNone);
    if (scheme.isEmpty() || policyAreas == PolicyAreaNone)
        return false;

    // get() returns 0 (PolicyAreaNone) if there is no entry in the map.
    // Thus by default, schemes do not bypass CSP.
    MutexLocker locker(mutex());
    return (ContentSecurityPolicyBypassingSchemes().get(scheme) & policyAreas) == policyAreas;
}

} // namespace blink
