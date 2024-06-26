// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/trace_event_etw_export_win.h"
#include "windows.h"

namespace base {
namespace trace_event {

    // static
    void TraceEventETWExport::AddEvent(
        char phase,
        const unsigned char* category_group_enabled,
        const char* name,
        unsigned long long id,
        int num_args,
        const char** arg_names,
        const unsigned char* arg_types,
        const unsigned long long* arg_values,
        const scoped_refptr<ConvertableToTraceFormat>* convertable_values)
    {
        // DebugBreak();
        *(int*)1 = 0;
    }

    // static
    void TraceEventETWExport::AddCompleteEndEvent(const char* name)
    {
        auto* instance = GetInstance();
        //DebugBreak();
        *(int*)1 = 0;
    }

    // static
    TraceEventETWExport* TraceEventETWExport::GetInstance()
    {
        DebugBreak();
        return nullptr;
    }

    // static
    bool TraceEventETWExport::IsCategoryGroupEnabled(const char* category_group_name)
    {
        return false;
    }

} // trace_event
} // base
