// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_profiler_heap_dump_writer.h"

#include <algorithm>
#include <iterator>

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/memory_profiler_allocation_register.h"
#include "base/trace_event/trace_event_argument.h"

namespace base {
namespace trace_event {

    using TypeId = AllocationContext::TypeId;

    namespace {

        template <typename T>
        bool PairSizeGt(const std::pair<T, size_t>& lhs,
            const std::pair<T, size_t>& rhs)
        {
            return lhs.second > rhs.second;
        }

        template <typename T>
        std::vector<std::pair<T, size_t>> SortDescending(
            const hash_map<T, size_t>& grouped)
        {
            std::vector<std::pair<T, size_t>> sorted;
            std::copy(grouped.begin(), grouped.end(), std::back_inserter(sorted));
            std::sort(sorted.begin(), sorted.end(), PairSizeGt<T>);
            return sorted;
        }

    } // namespace

    HeapDumpWriter::HeapDumpWriter(StackFrameDeduplicator* stack_frame_deduplicator)
        : traced_value_(new TracedValue())
        , stack_frame_deduplicator_(stack_frame_deduplicator)
    {
    }

    HeapDumpWriter::~HeapDumpWriter() { }

    void HeapDumpWriter::InsertAllocation(const AllocationContext& context,
        size_t size)
    {
        bytes_by_context_[context] += size;
    }

    scoped_refptr<TracedValue> HeapDumpWriter::WriteHeapDump()
    {
        // Group by backtrace and by type ID, and compute the total heap size while
        // iterating anyway.
        size_t total_size = 0;
        hash_map<Backtrace, size_t> bytes_by_backtrace;
        hash_map<TypeId, size_t> bytes_by_type;

        for (auto context_size : bytes_by_context_) {
            total_size += context_size.second;
            bytes_by_backtrace[context_size.first.backtrace] += context_size.second;
            bytes_by_type[context_size.first.type_id] += context_size.second;
        }

        // Sort the backtraces and type IDs by size.
        auto sorted_bytes_by_backtrace = SortDescending(bytes_by_backtrace);
        auto sorted_bytes_by_type = SortDescending(bytes_by_type);

        traced_value_->BeginArray("entries");

        // The global size, no column specified.
        {
            traced_value_->BeginDictionary();
            WriteSize(total_size);
            traced_value_->EndDictionary();
        }

        // Entries with the size per backtrace.
        for (const auto& entry : sorted_bytes_by_backtrace) {
            traced_value_->BeginDictionary();
            // Insert a forward reference to the backtrace that will be written to the
            // |stackFrames| dictionary later on.
            WriteStackFrameIndex(stack_frame_deduplicator_->Insert(entry.first));
            WriteSize(entry.second);
            traced_value_->EndDictionary();
        }

        // Entries with the size per type.
        for (const auto& entry : sorted_bytes_by_type) {
            traced_value_->BeginDictionary();
            WriteTypeId(entry.first);
            WriteSize(entry.second);
            traced_value_->EndDictionary();
        }

        traced_value_->EndArray(); // "entries"

        return traced_value_;
    }

    void HeapDumpWriter::WriteStackFrameIndex(int index)
    {
        if (index == -1) {
            // An empty backtrace (which will have index -1) is represented by the empty
            // string, because there is no leaf frame to reference in |stackFrames|.
            traced_value_->SetString("bt", "");
        } else {
            // Format index of the leaf frame as a string, because |stackFrames| is a
            // dictionary, not an array.
            SStringPrintf(&buffer_, "%i", index);
            traced_value_->SetString("bt", buffer_);
        }
    }

    void HeapDumpWriter::WriteTypeId(TypeId type_id)
    {
        if (type_id == 0) {
            // Type ID 0 represents "unknown type". Instead of writing it as "0" which
            // could be mistaken for an actual type ID, an unknown type is represented
            // by the empty string.
            traced_value_->SetString("type", "");
        } else {
            // Format the type ID as a string.
            SStringPrintf(&buffer_, "%" PRIu16, type_id);
            traced_value_->SetString("type", buffer_);
        }
    }

    void HeapDumpWriter::WriteSize(size_t size)
    {
        // Format size as hexadecimal string into |buffer_|.
        SStringPrintf(&buffer_, "%" PRIx64, static_cast<uint64_t>(size));
        traced_value_->SetString("size", buffer_);
    }

} // namespace trace_event
} // namespace base
