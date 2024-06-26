// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_MEMORY_DUMP_SESSION_STATE_H_
#define BASE_TRACE_EVENT_MEMORY_DUMP_SESSION_STATE_H_

#include <string>

#include "base/base_export.h"
#include "base/memory/ref_counted.h"
#include "base/trace_event/memory_profiler_allocation_context.h"

namespace base {
namespace trace_event {

    class StackFrameDeduplicator;

    // Container for state variables that should be shared across all the memory
    // dumps in a tracing session.
    class BASE_EXPORT MemoryDumpSessionState
        : public RefCountedThreadSafe<MemoryDumpSessionState> {
    public:
        MemoryDumpSessionState(
            const scoped_refptr<StackFrameDeduplicator>& stack_frame_deduplicator);

        // Returns the stack frame deduplicator that should be used by memory dump
        // providers when doing a heap dump.
        StackFrameDeduplicator* stack_frame_deduplicator()
        {
            return stack_frame_deduplicator_.get();
        }

    private:
        friend class RefCountedThreadSafe<MemoryDumpSessionState>;
        ~MemoryDumpSessionState();

        // Deduplicates backtraces in heap dumps so they can be written once when the
        // trace is finalized.
        scoped_refptr<StackFrameDeduplicator> stack_frame_deduplicator_;
    };

} // namespace trace_event
} // namespace base

#endif // BASE_TRACE_EVENT_MEMORY_DUMP_SESSION_STATE_H_
