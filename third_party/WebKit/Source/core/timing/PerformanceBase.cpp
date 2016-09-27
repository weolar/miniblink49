/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2012 Intel Inc. All rights reserved.
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
#include "core/timing/PerformanceBase.h"

#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/timing/PerformanceCompositeTiming.h"
#include "core/timing/PerformanceRenderTiming.h"
#include "core/timing/PerformanceResourceTiming.h"
#include "core/timing/PerformanceUserTiming.h"
#include "platform/network/ResourceTimingInfo.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/CurrentTime.h"

namespace blink {

static const size_t defaultResourceTimingBufferSize = 150;
static const size_t defaultFrameTimingBufferSize = 150;

PerformanceBase::PerformanceBase(double timeOrigin)
    : m_frameTimingBufferSize(defaultFrameTimingBufferSize)
    , m_resourceTimingBufferSize(defaultResourceTimingBufferSize)
    , m_timeOrigin(timeOrigin)
    , m_userTiming(nullptr)
{
}

PerformanceBase::~PerformanceBase()
{
}

const AtomicString& PerformanceBase::interfaceName() const
{
    return EventTargetNames::Performance;
}

PerformanceTiming* PerformanceBase::timing() const
{
    return nullptr;
}

PerformanceEntryVector PerformanceBase::getEntries() const
{
    PerformanceEntryVector entries;

    entries.appendVector(m_resourceTimingBuffer);
    entries.appendVector(m_frameTimingBuffer);

    if (m_userTiming) {
        entries.appendVector(m_userTiming->getMarks());
        entries.appendVector(m_userTiming->getMeasures());
    }

    std::sort(entries.begin(), entries.end(), PerformanceEntry::startTimeCompareLessThan);
    return entries;
}

PerformanceEntryVector PerformanceBase::getEntriesByType(const String& entryType)
{
    PerformanceEntryVector entries;

    if (equalIgnoringCase(entryType, "resource")) {
        for (const auto& resource : m_resourceTimingBuffer)
            entries.append(resource);
    }

    if (equalIgnoringCase(entryType, "composite")
        || equalIgnoringCase(entryType, "render")) {
        for (const auto& frame : m_frameTimingBuffer) {
            if (equalIgnoringCase(entryType, frame->entryType())) {
                entries.append(frame);
            }
        }
    }

    if (m_userTiming) {
        if (equalIgnoringCase(entryType, "mark"))
            entries.appendVector(m_userTiming->getMarks());
        else if (equalIgnoringCase(entryType, "measure"))
            entries.appendVector(m_userTiming->getMeasures());
    }

    std::sort(entries.begin(), entries.end(), PerformanceEntry::startTimeCompareLessThan);
    return entries;
}

PerformanceEntryVector PerformanceBase::getEntriesByName(const String& name, const String& entryType)
{
    PerformanceEntryVector entries;

    if (entryType.isNull() || equalIgnoringCase(entryType, "resource")) {
        for (const auto& resource : m_resourceTimingBuffer) {
            if (resource->name() == name)
                entries.append(resource);
        }
    }

    if (entryType.isNull() || equalIgnoringCase(entryType, "composite")
        || equalIgnoringCase(entryType, "render")) {
        for (const auto& frame : m_frameTimingBuffer) {
            if (frame->name() == name && (entryType.isNull()
                || equalIgnoringCase(entryType, frame->entryType()))) {
                entries.append(frame);
            }
        }
    }

    if (m_userTiming) {
        if (entryType.isNull() || equalIgnoringCase(entryType, "mark"))
            entries.appendVector(m_userTiming->getMarks(name));
        if (entryType.isNull() || equalIgnoringCase(entryType, "measure"))
            entries.appendVector(m_userTiming->getMeasures(name));
    }

    std::sort(entries.begin(), entries.end(), PerformanceEntry::startTimeCompareLessThan);
    return entries;
}

void PerformanceBase::webkitClearResourceTimings()
{
    m_resourceTimingBuffer.clear();
}

void PerformanceBase::webkitSetResourceTimingBufferSize(unsigned size)
{
    m_resourceTimingBufferSize = size;
    if (isResourceTimingBufferFull())
        dispatchEvent(Event::create(EventTypeNames::webkitresourcetimingbufferfull));
}

void PerformanceBase::clearFrameTimings()
{
    m_frameTimingBuffer.clear();
}

void PerformanceBase::setFrameTimingBufferSize(unsigned size)
{
    m_frameTimingBufferSize = size;
    if (isFrameTimingBufferFull())
        dispatchEvent(Event::create(EventTypeNames::frametimingbufferfull));
}

static bool passesTimingAllowCheck(const ResourceResponse& response, const SecurityOrigin& initiatorSecurityOrigin, const AtomicString& originalTimingAllowOrigin)
{
    AtomicallyInitializedStaticReference(AtomicString, timingAllowOrigin, new AtomicString("timing-allow-origin"));

    RefPtr<SecurityOrigin> resourceOrigin = SecurityOrigin::create(response.url());
    if (resourceOrigin->isSameSchemeHostPort(&initiatorSecurityOrigin))
        return true;

    const AtomicString& timingAllowOriginString = originalTimingAllowOrigin.isEmpty() ? response.httpHeaderField(timingAllowOrigin) : originalTimingAllowOrigin;
    if (timingAllowOriginString.isEmpty() || equalIgnoringCase(timingAllowOriginString, "null"))
        return false;

    if (timingAllowOriginString == starAtom)
        return true;

    const String& securityOrigin = initiatorSecurityOrigin.toString();
    Vector<String> timingAllowOrigins;
    timingAllowOriginString.string().split(' ', timingAllowOrigins);
    for (const String& allowOrigin : timingAllowOrigins) {
        if (allowOrigin == securityOrigin)
            return true;
    }

    return false;
}

static bool allowsTimingRedirect(const Vector<ResourceResponse>& redirectChain, const ResourceResponse& finalResponse, const SecurityOrigin& initiatorSecurityOrigin)
{
    if (!passesTimingAllowCheck(finalResponse, initiatorSecurityOrigin, emptyAtom))
        return false;

    for (const ResourceResponse& response : redirectChain) {
        if (!passesTimingAllowCheck(response, initiatorSecurityOrigin, emptyAtom))
            return false;
    }

    return true;
}

void PerformanceBase::addResourceTiming(const ResourceTimingInfo& info)
{
    if (isResourceTimingBufferFull())
        return;
    SecurityOrigin* securityOrigin = nullptr;
    if (ExecutionContext* context = executionContext())
        securityOrigin = context->securityOrigin();
    if (!securityOrigin)
        return;

    const ResourceResponse& finalResponse = info.finalResponse();
    bool allowTimingDetails = passesTimingAllowCheck(finalResponse, *securityOrigin, info.originalTimingAllowOrigin());
    double startTime = info.initialTime();

    if (info.redirectChain().isEmpty()) {
        PerformanceEntry* entry = PerformanceResourceTiming::create(info, timeOrigin(), startTime, allowTimingDetails);
        addResourceTimingBuffer(entry);
        return;
    }

    const Vector<ResourceResponse>& redirectChain = info.redirectChain();
    bool allowRedirectDetails = allowsTimingRedirect(redirectChain, finalResponse, *securityOrigin);

    if (!allowRedirectDetails) {
        ResourceLoadTiming* finalTiming = finalResponse.resourceLoadTiming();
        ASSERT(finalTiming);
        if (finalTiming)
            startTime = finalTiming->requestTime();
    }

    ResourceLoadTiming* lastRedirectTiming = redirectChain.last().resourceLoadTiming();
    ASSERT(lastRedirectTiming);
    double lastRedirectEndTime = lastRedirectTiming->receiveHeadersEnd();

    PerformanceEntry* entry = PerformanceResourceTiming::create(info, timeOrigin(), startTime, lastRedirectEndTime, allowTimingDetails, allowRedirectDetails);
    addResourceTimingBuffer(entry);
}

void PerformanceBase::addResourceTimingBuffer(PerformanceEntry* entry)
{
    m_resourceTimingBuffer.append(entry);

    if (isResourceTimingBufferFull())
        dispatchEvent(Event::create(EventTypeNames::webkitresourcetimingbufferfull));
}

bool PerformanceBase::isResourceTimingBufferFull()
{
    return m_resourceTimingBuffer.size() >= m_resourceTimingBufferSize;
}

void PerformanceBase::addRenderTiming(Document* initiatorDocument, unsigned sourceFrame, double startTime, double finishTime)
{
    if (isFrameTimingBufferFull())
        return;

    PerformanceEntry* entry = PerformanceRenderTiming::create(initiatorDocument, sourceFrame, startTime, finishTime);
    addFrameTimingBuffer(entry);
}

void PerformanceBase::addCompositeTiming(Document* initiatorDocument, unsigned sourceFrame, double startTime)
{
    if (isFrameTimingBufferFull())
        return;

    PerformanceEntry* entry = PerformanceCompositeTiming::create(initiatorDocument, sourceFrame, startTime);
    addFrameTimingBuffer(entry);
}

void PerformanceBase::addFrameTimingBuffer(PerformanceEntry* entry)
{
    m_frameTimingBuffer.append(entry);

    if (isFrameTimingBufferFull())
        dispatchEvent(Event::create(EventTypeNames::frametimingbufferfull));
}

bool PerformanceBase::isFrameTimingBufferFull()
{
    return m_frameTimingBuffer.size() >= m_frameTimingBufferSize;
}

void PerformanceBase::mark(const String& markName, ExceptionState& exceptionState)
{
    if (!m_userTiming)
        m_userTiming = UserTiming::create(this);
    m_userTiming->mark(markName, exceptionState);
}

void PerformanceBase::clearMarks(const String& markName)
{
    if (!m_userTiming)
        m_userTiming = UserTiming::create(this);
    m_userTiming->clearMarks(markName);
}

void PerformanceBase::measure(const String& measureName, const String& startMark, const String& endMark, ExceptionState& exceptionState)
{
    if (!m_userTiming)
        m_userTiming = UserTiming::create(this);
    m_userTiming->measure(measureName, startMark, endMark, exceptionState);
}

void PerformanceBase::clearMeasures(const String& measureName)
{
    if (!m_userTiming)
        m_userTiming = UserTiming::create(this);
    m_userTiming->clearMeasures(measureName);
}

double PerformanceBase::now() const
{
    double nowSeconds = monotonicallyIncreasingTime() - m_timeOrigin;
    const double resolutionSeconds = 0.000005;
    return 1000.0 * floor(nowSeconds / resolutionSeconds) * resolutionSeconds;
}

DEFINE_TRACE(PerformanceBase)
{
    visitor->trace(m_frameTimingBuffer);
    visitor->trace(m_resourceTimingBuffer);
    visitor->trace(m_userTiming);
    EventTargetWithInlineData::trace(visitor);
}

} // namespace blink
