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
#include "WTF.h"

#include "wtf/ArrayBufferContents.h"
#include "wtf/Assertions.h"
#include "wtf/FastMalloc.h"
#include "wtf/Partitions.h"

namespace WTF {

extern void initializeThreading();

bool s_initialized;
bool s_shutdown;

static void maxObservedSizeFunction(size_t sizeInMB)
{
//     const size_t supportedMaxSizeInMB = 4 * 1024;
//     if (sizeInMB >= supportedMaxSizeInMB)
//         sizeInMB = supportedMaxSizeInMB - 1;
// 
//     // Send a UseCounter only when we see the highest memory usage
//     // we've ever seen.
//     DEFINE_STATIC_LOCAL(EnumerationHistogram, committedSizeHistogram, ("PartitionAlloc.CommittedSize", supportedMaxSizeInMB));
//     committedSizeHistogram.count(sizeInMB);
}

void initialize(TimeFunction currentTimeFunction, TimeFunction monotonicallyIncreasingTimeFunction, TimeFunction systemTraceTimeFunction, HistogramEnumerationFunction histogramEnumerationFunction, AdjustAmountOfExternalAllocatedMemoryFunction adjustAmountOfExternalAllocatedMemoryFunction)
{
    // WTF, and Blink in general, cannot handle being re-initialized, even if shutdown first.
    // Make that explicit here.
    ASSERT(!s_initialized);
    ASSERT(!s_shutdown);
    s_initialized = true;
    setCurrentTimeFunction(currentTimeFunction);
    setMonotonicallyIncreasingTimeFunction(monotonicallyIncreasingTimeFunction);
    setSystemTraceTimeFunction(systemTraceTimeFunction);
    Partitions::initialize(maxObservedSizeFunction);
    //Partitions::setHistogramEnumeration(histogramEnumerationFunction);
    ArrayBufferContents::setAdjustAmoutOfExternalAllocatedMemoryFunction(adjustAmountOfExternalAllocatedMemoryFunction);
    initializeThreading();
}

void shutdown()
{
    ASSERT(s_initialized);
    ASSERT(!s_shutdown);
    s_shutdown = true;
    Partitions::shutdown();
}

bool isShutdown()
{
    return s_shutdown;
}

} // namespace WTF
