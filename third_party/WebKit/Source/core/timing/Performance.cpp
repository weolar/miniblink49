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
#include "core/timing/Performance.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/DocumentLoader.h"
#include "core/timing/PerformanceTiming.h"

namespace blink {

Performance::Performance(LocalFrame* frame)
    : PerformanceBase(frame && frame->host() ? frame->document()->loader()->timing().referenceMonotonicTime() : 0.0)
    , DOMWindowProperty(frame)
{
}

Performance::~Performance()
{
}

ExecutionContext* Performance::executionContext() const
{
    if (!frame())
        return nullptr;
    return frame()->document();
}

MemoryInfo* Performance::memory()
{
    return MemoryInfo::create();
}

PerformanceNavigation* Performance::navigation() const
{
    if (!m_navigation)
        m_navigation = PerformanceNavigation::create(m_frame);

    return m_navigation.get();
}

PerformanceTiming* Performance::timing() const
{
    if (!m_timing)
        m_timing = PerformanceTiming::create(m_frame);

    return m_timing.get();
}

DEFINE_TRACE(Performance)
{
    visitor->trace(m_navigation);
    visitor->trace(m_timing);
    DOMWindowProperty::trace(visitor);
    PerformanceBase::trace(visitor);
}

} // namespace blink
