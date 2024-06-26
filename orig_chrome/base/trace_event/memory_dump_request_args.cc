// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_dump_request_args.h"

#include "base/logging.h"

namespace base {
namespace trace_event {

    // static
    const char* MemoryDumpTypeToString(const MemoryDumpType& dump_type)
    {
        switch (dump_type) {
        case MemoryDumpType::TASK_BEGIN:
            return "task_begin";
        case MemoryDumpType::TASK_END:
            return "task_end";
        case MemoryDumpType::PERIODIC_INTERVAL:
            return "periodic_interval";
        case MemoryDumpType::EXPLICITLY_TRIGGERED:
            return "explicitly_triggered";
        }
        NOTREACHED();
        return "unknown";
    }

    const char* MemoryDumpLevelOfDetailToString(
        const MemoryDumpLevelOfDetail& level_of_detail)
    {
        switch (level_of_detail) {
        case MemoryDumpLevelOfDetail::LIGHT:
            return "light";
        case MemoryDumpLevelOfDetail::DETAILED:
            return "detailed";
        }
        NOTREACHED();
        return "unknown";
    }

    MemoryDumpLevelOfDetail StringToMemoryDumpLevelOfDetail(
        const std::string& str)
    {
        if (str == "light")
            return MemoryDumpLevelOfDetail::LIGHT;
        if (str == "detailed")
            return MemoryDumpLevelOfDetail::DETAILED;
        NOTREACHED();
        return MemoryDumpLevelOfDetail::LAST;
    }

} // namespace trace_event
} // namespace base
