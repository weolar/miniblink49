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

#include "config.h"
#include "core/timing/MemoryInfo.h"

#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/CurrentTime.h"
#include "wtf/MainThread.h"
#include "wtf/MathExtras.h"
#include "wtf/ThreadSpecific.h"
#include <limits>
#include <v8.h>

namespace blink {

static const double TwentyMinutesInSeconds = 20 * 60;

static void getHeapSize(HeapInfo& info)
{
    v8::HeapStatistics heapStatistics;
    v8::Isolate::GetCurrent()->GetHeapStatistics(&heapStatistics);
    info.usedJSHeapSize = heapStatistics.used_heap_size();
    info.totalJSHeapSize = heapStatistics.total_physical_size();
    info.jsHeapSizeLimit = heapStatistics.heap_size_limit();
}

class HeapSizeCache {
    WTF_MAKE_NONCOPYABLE(HeapSizeCache); WTF_MAKE_FAST_ALLOCATED(HeapSizeCache);
public:
    HeapSizeCache()
        : m_lastUpdateTime(monotonicallyIncreasingTime() - TwentyMinutesInSeconds)
    {
    }

    void getCachedHeapSize(HeapInfo& info)
    {
        maybeUpdate();
        info = m_info;
    }

    static HeapSizeCache& forCurrentThread()
    {
        AtomicallyInitializedStaticReference(ThreadSpecific<HeapSizeCache>, heapSizeCache, new ThreadSpecific<HeapSizeCache>);
        return *heapSizeCache;
    }

private:
    void maybeUpdate()
    {
        // We rate-limit queries to once every twenty minutes to make it more difficult
        // for attackers to compare memory usage before and after some event.
        double now = monotonicallyIncreasingTime();
        if (now - m_lastUpdateTime >= TwentyMinutesInSeconds) {
            update();
            m_lastUpdateTime = now;
        }
    }

    void update()
    {
        getHeapSize(m_info);
        m_info.usedJSHeapSize = quantizeMemorySize(m_info.usedJSHeapSize);
        m_info.totalJSHeapSize = quantizeMemorySize(m_info.totalJSHeapSize);
        m_info.jsHeapSizeLimit = quantizeMemorySize(m_info.jsHeapSizeLimit);
    }

    double m_lastUpdateTime;

    HeapInfo m_info;
};

// We quantize the sizes to make it more difficult for an attacker to see precise
// impact of operations on memory. The values are used for performance tuning,
// and hence don't need to be as refined when the value is large, so we threshold
// at a list of exponentially separated buckets.
size_t quantizeMemorySize(size_t size)
{
    const int numberOfBuckets = 100;
    DEFINE_STATIC_LOCAL(Vector<size_t>, bucketSizeList, ());

    if (bucketSizeList.isEmpty()) {
        bucketSizeList.resize(numberOfBuckets);

        float sizeOfNextBucket = 10000000.0; // First bucket size is roughly 10M.
        const float largestBucketSize = 4000000000.0; // Roughly 4GB.
        // We scale with the Nth root of the ratio, so that we use all the bucktes.
        const float scalingFactor = exp(log(largestBucketSize / sizeOfNextBucket) / numberOfBuckets);

        size_t nextPowerOfTen = static_cast<size_t>(pow(10, floor(log10(sizeOfNextBucket)) + 1) + 0.5);
        size_t granularity = nextPowerOfTen / 1000; // We want 3 signficant digits.

        for (int i = 0; i < numberOfBuckets; ++i) {
            size_t currentBucketSize = static_cast<size_t>(sizeOfNextBucket);
            bucketSizeList[i] = currentBucketSize - (currentBucketSize % granularity);

            sizeOfNextBucket *= scalingFactor;
            if (sizeOfNextBucket >= nextPowerOfTen) {
                if (std::numeric_limits<size_t>::max() / 10 <= nextPowerOfTen) {
                    nextPowerOfTen = std::numeric_limits<size_t>::max();
                } else {
                    nextPowerOfTen *= 10;
                    granularity *= 10;
                }
            }

            // Watch out for overflow, if the range is too large for size_t.
            if (i > 0 && bucketSizeList[i] < bucketSizeList[i - 1])
                bucketSizeList[i] = std::numeric_limits<size_t>::max();
        }
    }

    for (int i = 0; i < numberOfBuckets; ++i) {
        if (size <= bucketSizeList[i])
            return bucketSizeList[i];
    }

    return bucketSizeList[numberOfBuckets - 1];
}

MemoryInfo::MemoryInfo()
{
    if (RuntimeEnabledFeatures::preciseMemoryInfoEnabled())
        getHeapSize(m_info);
    else
        HeapSizeCache::forCurrentThread().getCachedHeapSize(m_info);
}

} // namespace blink
