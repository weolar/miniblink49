// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/process_memory_totals.h"

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event_argument.h"

namespace base {
namespace trace_event {

    ProcessMemoryTotals::ProcessMemoryTotals()
        : resident_set_bytes_(0)
        , peak_resident_set_bytes_(0)
        , is_peak_rss_resetable_(false)
    {
    }

    ProcessMemoryTotals::~ProcessMemoryTotals() { }

    void ProcessMemoryTotals::AsValueInto(TracedValue* value) const
    {
        value->SetString("resident_set_bytes",
            StringPrintf("%" PRIx64, resident_set_bytes_));
        if (peak_resident_set_bytes_ > 0) {
            value->SetString("peak_resident_set_bytes",
                StringPrintf("%" PRIx64, peak_resident_set_bytes_));
            value->SetBoolean("is_peak_rss_resetable", is_peak_rss_resetable_);
        }

        for (const auto it : extra_fields_) {
            value->SetString(it.first, StringPrintf("%" PRIx64, it.second));
        }
    }

    void ProcessMemoryTotals::Clear()
    {
        resident_set_bytes_ = 0;
    }

    void ProcessMemoryTotals::SetExtraFieldInBytes(const char* name,
        uint64_t value)
    {
        DCHECK_EQ(0u, extra_fields_.count(name));
        extra_fields_[name] = value;
    }

} // namespace trace_event
} // namespace base
