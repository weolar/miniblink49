// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_MEMORY_DUMP_PROVIDER_H_
#define BASE_TRACE_EVENT_MEMORY_DUMP_PROVIDER_H_

#include "base/base_export.h"
#include "base/macros.h"
#include "base/process/process_handle.h"
#include "base/trace_event/memory_dump_request_args.h"

namespace base {
namespace trace_event {

    class ProcessMemoryDump;

    // Args passed to OnMemoryDump(). This is to avoid rewriting all the subclasses
    // in the codebase when extending the MemoryDumpProvider API.
    struct MemoryDumpArgs {
        MemoryDumpLevelOfDetail level_of_detail;
    };

    // The contract interface that memory dump providers must implement.
    class BASE_EXPORT MemoryDumpProvider {
    public:
        // Optional arguments for MemoryDumpManager::RegisterDumpProvider().
        struct Options {
            Options()
                : target_pid(kNullProcessId)
            {
            }
            explicit Options(ProcessId target_pid)
                : target_pid(target_pid)
            {
            }

            // If the dump provider generates dumps on behalf of another process,
            // |target_process| contains the pid of that process.
            // The default value is kNullProcessId, which means that the dump provider
            // generates dumps for the current process.
            ProcessId target_pid;
        };

        // Called by the MemoryDumpManager when generating memory dumps.
        // The |args| specify if the embedder should generate light/heavy dumps on
        // dump requests. The embedder should return true if the |pmd| was
        // successfully populated, false if something went wrong and the dump should
        // be considered invalid.
        // (Note, the MemoryDumpManager has a fail-safe logic which will disable the
        // MemoryDumpProvider for the entire trace session if it fails consistently).
        virtual bool OnMemoryDump(const MemoryDumpArgs& args,
            ProcessMemoryDump* pmd)
            = 0;

        // Called by the MemoryDumpManager when an allocator should start or stop
        // collecting extensive allocation data, if supported.
        virtual void OnHeapProfilingEnabled(bool enabled) { }

    protected:
        MemoryDumpProvider() { }
        virtual ~MemoryDumpProvider() { }

        DISALLOW_COPY_AND_ASSIGN(MemoryDumpProvider);
    };

} // namespace trace_event
} // namespace base

#endif // BASE_TRACE_EVENT_MEMORY_DUMP_PROVIDER_H_
