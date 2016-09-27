/*
 * Copyright (C) 2008, 2009 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/loader/appcache/ApplicationCache.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/events/EventListener.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoader.h"

namespace blink {

ApplicationCache::ApplicationCache(LocalFrame* frame)
    : DOMWindowProperty(frame)
{
    ApplicationCacheHost* cacheHost = applicationCacheHost();
    if (cacheHost)
        cacheHost->setApplicationCache(this);
}

DEFINE_TRACE(ApplicationCache)
{
    EventTargetWithInlineData::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

void ApplicationCache::willDestroyGlobalObjectInFrame()
{
    if (ApplicationCacheHost* cacheHost = applicationCacheHost())
        cacheHost->setApplicationCache(0);
    DOMWindowProperty::willDestroyGlobalObjectInFrame();
}

ApplicationCacheHost* ApplicationCache::applicationCacheHost() const
{
    if (!m_frame || !m_frame->loader().documentLoader())
        return 0;
    return m_frame->loader().documentLoader()->applicationCacheHost();
}

unsigned short ApplicationCache::status() const
{
    ApplicationCacheHost* cacheHost = applicationCacheHost();
    if (!cacheHost)
        return ApplicationCacheHost::UNCACHED;
    return cacheHost->status();
}

void ApplicationCache::update(ExceptionState& exceptionState)
{
    ApplicationCacheHost* cacheHost = applicationCacheHost();
    if (!cacheHost || !cacheHost->update())
        exceptionState.throwDOMException(InvalidStateError, "there is no application cache to update.");
}

void ApplicationCache::swapCache(ExceptionState& exceptionState)
{
    ApplicationCacheHost* cacheHost = applicationCacheHost();
    if (!cacheHost || !cacheHost->swapCache())
        exceptionState.throwDOMException(InvalidStateError, "there is no newer application cache to swap to.");
}

void ApplicationCache::abort()
{
    ApplicationCacheHost* cacheHost = applicationCacheHost();
    if (cacheHost)
        cacheHost->abort();
}

const AtomicString& ApplicationCache::interfaceName() const
{
    return EventTargetNames::ApplicationCache;
}

ExecutionContext* ApplicationCache::executionContext() const
{
    if (m_frame)
        return m_frame->document();
    return 0;
}

const AtomicString& ApplicationCache::toEventType(ApplicationCacheHost::EventID id)
{
    switch (id) {
    case ApplicationCacheHost::CHECKING_EVENT:
        return EventTypeNames::checking;
    case ApplicationCacheHost::ERROR_EVENT:
        return EventTypeNames::error;
    case ApplicationCacheHost::NOUPDATE_EVENT:
        return EventTypeNames::noupdate;
    case ApplicationCacheHost::DOWNLOADING_EVENT:
        return EventTypeNames::downloading;
    case ApplicationCacheHost::PROGRESS_EVENT:
        return EventTypeNames::progress;
    case ApplicationCacheHost::UPDATEREADY_EVENT:
        return EventTypeNames::updateready;
    case ApplicationCacheHost::CACHED_EVENT:
        return EventTypeNames::cached;
    case ApplicationCacheHost::OBSOLETE_EVENT:
        return EventTypeNames::obsolete;
    }
    ASSERT_NOT_REACHED();
    return EventTypeNames::error;
}

} // namespace blink
