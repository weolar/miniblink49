// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebMemoryAllocatorDump_h
#define WebMemoryAllocatorDump_h

#include "WebCommon.h"
#include "WebString.h"

namespace blink {
typedef uint64_t WebMemoryAllocatorDumpGuid;

// A container which holds all the attributes of a particular dump for a given
// allocator.
class BLINK_PLATFORM_EXPORT WebMemoryAllocatorDump {
public:
    virtual ~WebMemoryAllocatorDump();

    // Adds a scalar attribute to the dump.
    // Arguments:
    //   name: name of the attribute. Typical names, emitted by most allocators
    //       dump providers are: "size" and "objects_count".
    //   units: the units for the attribute. Gives a hint to the trace-viewer UI
    //       about the semantics of the attribute.
    //       Currently supported values are "bytes" and "objects".
    //   value: the value of the attribute.
    virtual void AddScalar(const char* name, const char* units, uint64_t value) { }
    virtual void AddScalarF(const char* name, const char* units, double value) { }
    virtual void AddString(const char* name, const char* units, const WebString& value) { }

    // |guid| is an optional global dump identifier, unique across all processes
    // within the scope of a global dump. It is only required when using the
    // graph APIs (see AddOwnershipEdge) to express retention / suballocation or
    // cross process sharing. See crbug.com/492102 for design docs.
    // Subsequent MemoryAllocatorDump(s) with the same |absolute_name| are
    // expected to have the same guid.
    virtual WebMemoryAllocatorDumpGuid guid() const
    {
        BLINK_ASSERT_NOT_REACHED();
        return 0;
    }
};

} // namespace blink

#endif // WebMemoryAllocatorDump_h
