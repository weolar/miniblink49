// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/events/ApplicationCacheErrorEvent.h"

namespace blink {

static const String& errorReasonToString(WebApplicationCacheHost::ErrorReason reason)
{
    DEFINE_STATIC_LOCAL(String, errorManifest, ("manifest"));
    DEFINE_STATIC_LOCAL(String, errorSignature, ("signature"));
    DEFINE_STATIC_LOCAL(String, errorResource, ("resource"));
    DEFINE_STATIC_LOCAL(String, errorChanged, ("changed"));
    DEFINE_STATIC_LOCAL(String, errorAbort, ("abort"));
    DEFINE_STATIC_LOCAL(String, errorQuota, ("quota"));
    DEFINE_STATIC_LOCAL(String, errorPolicy, ("policy"));
    DEFINE_STATIC_LOCAL(String, errorUnknown, ("unknown"));

    switch (reason) {
    case WebApplicationCacheHost::ManifestError:
        return errorManifest;
    case WebApplicationCacheHost::SignatureError:
        return errorSignature;
    case WebApplicationCacheHost::ResourceError:
        return errorResource;
    case WebApplicationCacheHost::ChangedError:
        return errorChanged;
    case WebApplicationCacheHost::AbortError:
        return errorAbort;
    case WebApplicationCacheHost::QuotaError:
        return errorQuota;
    case WebApplicationCacheHost::PolicyError:
        return errorPolicy;
    case WebApplicationCacheHost::UnknownError:
        return errorUnknown;
    }
    ASSERT_NOT_REACHED();
    return emptyString();
}

ApplicationCacheErrorEvent::ApplicationCacheErrorEvent()
{
}

ApplicationCacheErrorEvent::ApplicationCacheErrorEvent(WebApplicationCacheHost::ErrorReason reason, const String& url, int status, const String& message)
    : Event(EventTypeNames::error, false, false)
    , m_reason(errorReasonToString(reason))
    , m_url(url)
    , m_status(status)
    , m_message(message)
{
}

ApplicationCacheErrorEvent::ApplicationCacheErrorEvent(const AtomicString& eventType, const ApplicationCacheErrorEventInit& initializer)
    : Event(eventType, initializer)
    , m_status(0)
{
    if (initializer.hasReason())
        m_reason = initializer.reason();
    if (initializer.hasURL())
        m_url = initializer.url();
    if (initializer.hasStatus())
        m_status = initializer.status();
    if (initializer.hasMessage())
        m_message = initializer.message();
}

ApplicationCacheErrorEvent::~ApplicationCacheErrorEvent()
{
}

DEFINE_TRACE(ApplicationCacheErrorEvent)
{
    Event::trace(visitor);
}

} // namespace blink
