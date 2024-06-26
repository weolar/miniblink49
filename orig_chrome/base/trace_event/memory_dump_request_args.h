// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_MEMORY_DUMP_REQUEST_ARGS_H_
#define BASE_TRACE_EVENT_MEMORY_DUMP_REQUEST_ARGS_H_

// This file defines the types and structs used to issue memory dump requests.
// These are also used in the IPCs for coordinating inter-process memory dumps.

#include <stdint.h>
#include <string>

#include "base/base_export.h"
#include "base/callback.h"

namespace base {
namespace trace_event {

    // Captures the reason why a memory dump is being requested. This is to allow
    // selective enabling of dumps, filtering and post-processing.
    enum class MemoryDumpType {
        TASK_BEGIN, // Dumping memory at the beginning of a message-loop task.
        TASK_END, // Dumping memory at the ending of a message-loop task.
        PERIODIC_INTERVAL, // Dumping memory at periodic intervals.
        EXPLICITLY_TRIGGERED, // Non maskable dump request.
        LAST = EXPLICITLY_TRIGGERED // For IPC macros.
    };

    // Tells the MemoryDumpProvider(s) how much detailed their dumps should be.
    // MemoryDumpProvider instances must guarantee that level of detail does not
    // affect the total size reported in the root node, but only the granularity of
    // the child MemoryAllocatorDump(s).
    enum class MemoryDumpLevelOfDetail {
        LIGHT, // Few entries, typically a fixed number, per dump.
        DETAILED, // Unrestricted amount of entries per dump.
        LAST = DETAILED // For IPC Macros.
    };

    // Initial request arguments for a global memory dump. (see
    // MemoryDumpManager::RequestGlobalMemoryDump()).
    struct BASE_EXPORT MemoryDumpRequestArgs {
        // Globally unique identifier. In multi-process dumps, all processes issue a
        // local dump with the same guid. This allows the trace importers to
        // reconstruct the global dump.
        uint64_t dump_guid;

        MemoryDumpType dump_type;
        MemoryDumpLevelOfDetail level_of_detail;
    };

    using MemoryDumpCallback = Callback<void(uint64_t dump_guid, bool success)>;

    BASE_EXPORT const char* MemoryDumpTypeToString(const MemoryDumpType& dump_type);

    BASE_EXPORT const char* MemoryDumpLevelOfDetailToString(
        const MemoryDumpLevelOfDetail& level_of_detail);

    BASE_EXPORT MemoryDumpLevelOfDetail
    StringToMemoryDumpLevelOfDetail(const std::string& str);

} // namespace trace_event
} // namespace base

#endif // BASE_TRACE_EVENT_MEMORY_DUMP_REQUEST_ARGS_H_
