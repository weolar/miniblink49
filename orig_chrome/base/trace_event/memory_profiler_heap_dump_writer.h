// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_MEMORY_PROFILER_HEAP_DUMP_WRITER_H_
#define BASE_TRACE_EVENT_MEMORY_PROFILER_HEAP_DUMP_WRITER_H_

#include <string>

#include "base/base_export.h"
#include "base/containers/hash_tables.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/trace_event/memory_profiler_allocation_context.h"

namespace base {
namespace trace_event {

    class AllocationRegister;
    class TracedValue;

    // Helper class to dump a snapshot of an |AllocationRegister| or other heap
    // bookkeeping structure into a |TracedValue|. This class is intended to be
    // used as a one-shot local instance on the stack. To write heap dumps, call
    // |InsertAllocation| for every captured allocation, then call |WriteHeapDump|
    // to do the processing and generate a heap dump value for the trace log.
    class BASE_EXPORT HeapDumpWriter {
    public:
        // The |StackFrameDeduplicator| is not owned. The heap dump writer assumes
        // exclusive access to it during the lifetime of the dump writer.
        HeapDumpWriter(StackFrameDeduplicator* stack_frame_deduplicator);
        ~HeapDumpWriter();

        // Inserts information from which the heap dump will be generated. This method
        // does minimal processing, so it can be called when a lock is held.
        void InsertAllocation(const AllocationContext& context, size_t size);

        // Aggregates allocations and writes an "entries" array to a traced value. See
        // https://goo.gl/jYN4Zn for a description of the format.
        scoped_refptr<TracedValue> WriteHeapDump();

    private:
        // Writes a "bt" key that references a stack frame in the |stackFrames|
        // dictionary.
        void WriteStackFrameIndex(int index);

        // Writes a "type" key with the stringified type ID.
        void WriteTypeId(AllocationContext::TypeId type_id);

        // Writes a "size" key with value |size| as a hexidecimal string to the traced
        // value.
        void WriteSize(size_t size);

        // The value that this heap dumper writes to.
        const scoped_refptr<TracedValue> traced_value_;

        // Helper for generating the |stackFrames| dictionary. Not owned, must outlive
        // this heap dump writer instance.
        StackFrameDeduplicator* const stack_frame_deduplicator_;

        // A map of allocation context to the number of bytes allocated for that
        // context.
        hash_map<AllocationContext, size_t> bytes_by_context_;

        // Buffer for converting integers into strings, that is re-used throughout the
        // dump.
        std::string buffer_;

        DISALLOW_COPY_AND_ASSIGN(HeapDumpWriter);
    };

} // namespace trace_event
} // namespace base

#endif // BASE_TRACE_EVENT_MEMORY_PROFILER_HEAP_DUMP_WRITER_H_
