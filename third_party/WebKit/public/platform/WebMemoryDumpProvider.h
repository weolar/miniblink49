// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebMemoryDumpProvider_h
#define WebMemoryDumpProvider_h

#include "WebCommon.h"

namespace blink {

class WebProcessMemoryDump;

// Base interface to be part of the memory tracing infrastructure. Blink classes
// can implement this interface and register themselves (see
// Platform::registerMemoryDumpProvider()) to dump stats for their allocators.
class BLINK_PLATFORM_EXPORT WebMemoryDumpProvider {
public:
    virtual ~WebMemoryDumpProvider();

    // Called by the MemoryDumpManager when generating memory dumps. Embedders
    // are expected to populate the WebProcessMemoryDump* argument and return
    // true on success or false if anything went wrong and the dump should be
    // considered invalid.
    virtual bool onMemoryDump(WebProcessMemoryDump*) = 0;
};

} // namespace blink

#endif // WebMemoryDumpProvider_h
