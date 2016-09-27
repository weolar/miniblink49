/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef MemoryInfo_h
#define MemoryInfo_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "platform/heap/Handle.h"

namespace blink {

struct HeapInfo {
    HeapInfo()
        : usedJSHeapSize(0)
        , totalJSHeapSize(0)
        , jsHeapSizeLimit(0)
    {
    }

    size_t usedJSHeapSize;
    size_t totalJSHeapSize;
    size_t jsHeapSizeLimit;
};

class CORE_EXPORT MemoryInfo final : public GarbageCollected<MemoryInfo>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static MemoryInfo* create()
    {
        return new MemoryInfo();
    }

    size_t totalJSHeapSize() const { return m_info.totalJSHeapSize; }
    size_t usedJSHeapSize() const { return m_info.usedJSHeapSize; }
    size_t jsHeapSizeLimit() const { return m_info.jsHeapSizeLimit; }

    DEFINE_INLINE_TRACE() { }

private:
    MemoryInfo();

    HeapInfo m_info;
};

CORE_EXPORT size_t quantizeMemorySize(size_t);

} // namespace blink

#endif // MemoryInfo_h
