// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_TRACED_VALUE_H_
#define CC_DEBUG_TRACED_VALUE_H_

namespace base {
namespace trace_event {
    class TracedValue;
}
}

namespace cc {

class TracedValue {
public:
    static void AppendIDRef(const void* id,
        base::trace_event::TracedValue* array);
    static void SetIDRef(const void* id,
        base::trace_event::TracedValue* dict,
        const char* name);
    static void MakeDictIntoImplicitSnapshot(base::trace_event::TracedValue* dict,
        const char* object_name,
        const void* id);
    static void MakeDictIntoImplicitSnapshotWithCategory(
        const char* category,
        base::trace_event::TracedValue* dict,
        const char* object_name,
        const void* id);
    static void MakeDictIntoImplicitSnapshotWithCategory(
        const char* category,
        base::trace_event::TracedValue* dict,
        const char* object_base_type_name,
        const char* object_name,
        const void* id);
};

} // namespace cc

#endif // CC_DEBUG_TRACED_VALUE_H_
