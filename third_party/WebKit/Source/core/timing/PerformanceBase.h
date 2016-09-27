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

#ifndef PerformanceBase_h
#define PerformanceBase_h

#include "core/CoreExport.h"
#include "core/events/EventTarget.h"
#include "core/timing/PerformanceEntry.h"
#include "platform/heap/Handle.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;
class ExceptionState;
class PerformanceTiming;
class ResourceTimingInfo;
class UserTiming;

using PerformanceEntryVector = HeapVector<Member<PerformanceEntry>>;

class CORE_EXPORT PerformanceBase : public RefCountedGarbageCollectedEventTargetWithInlineData<PerformanceBase> {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(PerformanceBase);
public:
    virtual ~PerformanceBase();

    virtual const AtomicString& interfaceName() const override;

    virtual PerformanceTiming* timing() const;
    double now() const;

    double timeOrigin() const { return m_timeOrigin; }

    PerformanceEntryVector getEntries() const;
    PerformanceEntryVector getEntriesByType(const String& entryType);
    PerformanceEntryVector getEntriesByName(const String& name, const String& entryType);

    void webkitClearResourceTimings();
    void webkitSetResourceTimingBufferSize(unsigned);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(webkitresourcetimingbufferfull);

    void clearFrameTimings();
    void setFrameTimingBufferSize(unsigned);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(frametimingbufferfull);

    void addResourceTiming(const ResourceTimingInfo&);

    void addRenderTiming(Document*, unsigned, double, double);

    void addCompositeTiming(Document*, unsigned, double);

    void mark(const String& markName, ExceptionState&);
    void clearMarks(const String& markName);

    void measure(const String& measureName, const String& startMark, const String& endMark, ExceptionState&);
    void clearMeasures(const String& measureName);

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit PerformanceBase(double timeOrigin);
    bool isResourceTimingBufferFull();
    void addResourceTimingBuffer(PerformanceEntry*);

    bool isFrameTimingBufferFull();
    void addFrameTimingBuffer(PerformanceEntry*);

    PerformanceEntryVector m_frameTimingBuffer;
    unsigned m_frameTimingBufferSize;
    PerformanceEntryVector m_resourceTimingBuffer;
    unsigned m_resourceTimingBufferSize;
    double m_timeOrigin;

    Member<UserTiming> m_userTiming;
};

} // namespace blink

#endif // PerformanceBase_h
