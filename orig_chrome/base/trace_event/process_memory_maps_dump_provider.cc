// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/process_memory_maps_dump_provider.h"

#include "base/files/scoped_file.h"
#include "base/format_macros.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/trace_event/process_memory_dump.h"
#include "base/trace_event/process_memory_maps.h"

namespace base {
namespace trace_event {

#if defined(OS_LINUX) || defined(OS_ANDROID) || defined(OS_NACL)
    // static
    FILE* ProcessMemoryMapsDumpProvider::proc_smaps_for_testing = nullptr;
#endif

    namespace {

        const uint32 kMaxLineSize = 4096;

        bool ParseSmapsHeader(const char* header_line,
            ProcessMemoryMaps::VMRegion* region)
        {
            // e.g., "00400000-00421000 r-xp 00000000 fc:01 1234  /foo.so\n"
            bool res = true; // Whether this region should be appended or skipped.
            uint64 end_addr = 0;
            char protection_flags[5] = { 0 };
            char mapped_file[kMaxLineSize];

            if (sscanf(header_line, "%" SCNx64 "-%" SCNx64 " %4c %*s %*s %*s%4095[^\n]\n",
                    &region->start_address, &end_addr, protection_flags,
                    mapped_file)
                != 4)
                return false;

            if (end_addr > region->start_address) {
                region->size_in_bytes = end_addr - region->start_address;
            } else {
                // This is not just paranoia, it can actually happen (See crbug.com/461237).
                region->size_in_bytes = 0;
                res = false;
            }

            region->protection_flags = 0;
            if (protection_flags[0] == 'r') {
                region->protection_flags |= ProcessMemoryMaps::VMRegion::kProtectionFlagsRead;
            }
            if (protection_flags[1] == 'w') {
                region->protection_flags |= ProcessMemoryMaps::VMRegion::kProtectionFlagsWrite;
            }
            if (protection_flags[2] == 'x') {
                region->protection_flags |= ProcessMemoryMaps::VMRegion::kProtectionFlagsExec;
            }

            region->mapped_file = mapped_file;
            TrimWhitespaceASCII(region->mapped_file, TRIM_ALL, &region->mapped_file);

            return res;
        }

        uint64 ReadCounterBytes(char* counter_line)
        {
            uint64 counter_value = 0;
            int res = sscanf(counter_line, "%*s %" SCNu64 " kB", &counter_value);
            DCHECK_EQ(1, res);
            return counter_value * 1024;
        }

        uint32 ParseSmapsCounter(char* counter_line,
            ProcessMemoryMaps::VMRegion* region)
        {
            // A smaps counter lines looks as follows: "RSS:  0 Kb\n"
            uint32 res = 1;
            char counter_name[20];
            int did_read = sscanf(counter_line, "%19[^\n ]", counter_name);
            DCHECK_EQ(1, did_read);

            if (strcmp(counter_name, "Pss:") == 0) {
                region->byte_stats_proportional_resident = ReadCounterBytes(counter_line);
            } else if (strcmp(counter_name, "Private_Dirty:") == 0) {
                region->byte_stats_private_dirty_resident = ReadCounterBytes(counter_line);
            } else if (strcmp(counter_name, "Private_Clean:") == 0) {
                region->byte_stats_private_clean_resident = ReadCounterBytes(counter_line);
            } else if (strcmp(counter_name, "Shared_Dirty:") == 0) {
                region->byte_stats_shared_dirty_resident = ReadCounterBytes(counter_line);
            } else if (strcmp(counter_name, "Shared_Clean:") == 0) {
                region->byte_stats_shared_clean_resident = ReadCounterBytes(counter_line);
            } else if (strcmp(counter_name, "Swap:") == 0) {
                region->byte_stats_swapped = ReadCounterBytes(counter_line);
            } else {
                res = 0;
            }

            return res;
        }

        uint32 ReadLinuxProcSmapsFile(FILE* smaps_file, ProcessMemoryMaps* pmm)
        {
            if (!smaps_file)
                return 0;

            fseek(smaps_file, 0, SEEK_SET);

            char line[kMaxLineSize];
            const uint32 kNumExpectedCountersPerRegion = 6;
            uint32 counters_parsed_for_current_region = 0;
            uint32 num_valid_regions = 0;
            ProcessMemoryMaps::VMRegion region;
            bool should_add_current_region = false;
            for (;;) {
                line[0] = '\0';
                if (fgets(line, kMaxLineSize, smaps_file) == nullptr)
                    break;
                DCHECK_GT(strlen(line), 0u);
                if (isxdigit(line[0]) && !isupper(line[0])) {
                    region = ProcessMemoryMaps::VMRegion();
                    counters_parsed_for_current_region = 0;
                    should_add_current_region = ParseSmapsHeader(line, &region);
                } else {
                    counters_parsed_for_current_region += ParseSmapsCounter(line, &region);
                    DCHECK_LE(counters_parsed_for_current_region,
                        kNumExpectedCountersPerRegion);
                    if (counters_parsed_for_current_region == kNumExpectedCountersPerRegion) {
                        if (should_add_current_region) {
                            pmm->AddVMRegion(region);
                            ++num_valid_regions;
                            should_add_current_region = false;
                        }
                    }
                }
            }
            return num_valid_regions;
        }

    } // namespace

    // static
    ProcessMemoryMapsDumpProvider* ProcessMemoryMapsDumpProvider::GetInstance()
    {
        return Singleton<ProcessMemoryMapsDumpProvider,
            LeakySingletonTraits<ProcessMemoryMapsDumpProvider>>::get();
    }

    ProcessMemoryMapsDumpProvider::ProcessMemoryMapsDumpProvider()
    {
    }

    ProcessMemoryMapsDumpProvider::~ProcessMemoryMapsDumpProvider()
    {
    }

    // Called at trace dump point time. Creates a snapshot of the memory maps for
    // the current process.
    bool ProcessMemoryMapsDumpProvider::OnMemoryDump(const MemoryDumpArgs& args,
        ProcessMemoryDump* pmd)
    {
        // Snapshot of memory maps is not taken for light dump requests.
        //   if (args.level_of_detail == MemoryDumpLevelOfDetail::LIGHT)
        //     return true;
        //
        //   uint32 res = 0;
        //
        //   if (UNLIKELY(proc_smaps_for_testing)) {
        //     res = ReadLinuxProcSmapsFile(proc_smaps_for_testing, pmd->process_mmaps());
        //   } else {
        //     ScopedFILE smaps_file(fopen("/proc/self/smaps", "r"));
        //     res = ReadLinuxProcSmapsFile(smaps_file.get(), pmd->process_mmaps());
        //   }
        //
        //   if (res > 0) {
        //     pmd->set_has_process_mmaps();
        //     return true;
        //   }
        DebugBreak();
        return false;
    }

} // namespace trace_event
} // namespace base
