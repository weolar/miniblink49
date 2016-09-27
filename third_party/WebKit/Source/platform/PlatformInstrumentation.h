/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef PlatformInstrumentation_h
#define PlatformInstrumentation_h

#include "platform/PlatformExport.h"
#include "platform/TraceEvent.h"
#include "wtf/MainThread.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PLATFORM_EXPORT PlatformInstrumentation {
public:
    class LazyPixelRefTracker: TraceEvent::TraceScopedTrackableObject<void*> {
    public:
        LazyPixelRefTracker(void* instance)
            : TraceEvent::TraceScopedTrackableObject<void*>(CategoryName, LazyPixelRef, instance)
        {
        }
    };

    static const char ImageDecodeEvent[];
    static const char ImageResizeEvent[];
    static const char DrawLazyPixelRefEvent[];
    static const char DecodeLazyPixelRefEvent[];

    static const char ImageTypeArgument[];
    static const char CachedArgument[];

    static const char LazyPixelRef[];

    static void willDecodeImage(const String& imageType);
    static void didDecodeImage();
    static void didDrawLazyPixelRef(unsigned long long lazyPixelRefId);
    static void willDecodeLazyPixelRef(unsigned long long lazyPixelRefId);
    static void didDecodeLazyPixelRef();

private:
    static const char CategoryName[];
};

inline void PlatformInstrumentation::willDecodeImage(const String& imageType)
{
    TRACE_EVENT_BEGIN1(CategoryName, ImageDecodeEvent, ImageTypeArgument, imageType.ascii());
}

inline void PlatformInstrumentation::didDecodeImage()
{
    TRACE_EVENT_END0(CategoryName, ImageDecodeEvent);
}

inline void PlatformInstrumentation::didDrawLazyPixelRef(unsigned long long lazyPixelRefId)
{
    TRACE_EVENT_INSTANT1(CategoryName, DrawLazyPixelRefEvent, TRACE_EVENT_SCOPE_THREAD, LazyPixelRef, lazyPixelRefId);
}

inline void PlatformInstrumentation::willDecodeLazyPixelRef(unsigned long long lazyPixelRefId)
{
    TRACE_EVENT_BEGIN1(CategoryName, DecodeLazyPixelRefEvent, LazyPixelRef, lazyPixelRefId);
}

inline void PlatformInstrumentation::didDecodeLazyPixelRef()
{
    TRACE_EVENT_END0(CategoryName, DecodeLazyPixelRefEvent);
}

} // namespace blink

#endif // PlatformInstrumentation_h
