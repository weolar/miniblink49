// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/process_memory_maps_dump_provider.h"

#include "base/files/file_util.h"
#include "base/trace_event/process_memory_dump.h"
#include "base/trace_event/process_memory_maps.h"
#include "base/trace_event/trace_event_argument.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace trace_event {

    namespace {
        const char kTestSmaps1[] = "00400000-004be000 r-xp 00000000 fc:01 1234              /file/1\n"
                                   "Size:                760 kB\n"
                                   "Rss:                 296 kB\n"
                                   "Pss:                 162 kB\n"
                                   "Shared_Clean:        228 kB\n"
                                   "Shared_Dirty:          0 kB\n"
                                   "Private_Clean:         0 kB\n"
                                   "Private_Dirty:        68 kB\n"
                                   "Referenced:          296 kB\n"
                                   "Anonymous:            68 kB\n"
                                   "AnonHugePages:         0 kB\n"
                                   "Swap:                  4 kB\n"
                                   "KernelPageSize:        4 kB\n"
                                   "MMUPageSize:           4 kB\n"
                                   "Locked:                0 kB\n"
                                   "VmFlags: rd ex mr mw me dw sd\n"
                                   "ff000000-ff800000 -w-p 00001080 fc:01 0            /file/name with space\n"
                                   "Size:                  0 kB\n"
                                   "Rss:                 192 kB\n"
                                   "Pss:                 128 kB\n"
                                   "Shared_Clean:        120 kB\n"
                                   "Shared_Dirty:          4 kB\n"
                                   "Private_Clean:        60 kB\n"
                                   "Private_Dirty:         8 kB\n"
                                   "Referenced:          296 kB\n"
                                   "Anonymous:             0 kB\n"
                                   "AnonHugePages:         0 kB\n"
                                   "Swap:                  0 kB\n"
                                   "KernelPageSize:        4 kB\n"
                                   "MMUPageSize:           4 kB\n"
                                   "Locked:                0 kB\n"
                                   "VmFlags: rd ex mr mw me dw sd";

        const char kTestSmaps2[] =
            // An invalid region, with zero size and overlapping with the last one
            // (See crbug.com/461237).
            "7fe7ce79c000-7fe7ce79c000 ---p 00000000 00:00 0 \n"
            "Size:                  4 kB\n"
            "Rss:                   0 kB\n"
            "Pss:                   0 kB\n"
            "Shared_Clean:          0 kB\n"
            "Shared_Dirty:          0 kB\n"
            "Private_Clean:         0 kB\n"
            "Private_Dirty:         0 kB\n"
            "Referenced:            0 kB\n"
            "Anonymous:             0 kB\n"
            "AnonHugePages:         0 kB\n"
            "Swap:                  0 kB\n"
            "KernelPageSize:        4 kB\n"
            "MMUPageSize:           4 kB\n"
            "Locked:                0 kB\n"
            "VmFlags: rd ex mr mw me dw sd\n"
            // A invalid region with its range going backwards.
            "00400000-00200000 ---p 00000000 00:00 0 \n"
            "Size:                  4 kB\n"
            "Rss:                   0 kB\n"
            "Pss:                   0 kB\n"
            "Shared_Clean:          0 kB\n"
            "Shared_Dirty:          0 kB\n"
            "Private_Clean:         0 kB\n"
            "Private_Dirty:         0 kB\n"
            "Referenced:            0 kB\n"
            "Anonymous:             0 kB\n"
            "AnonHugePages:         0 kB\n"
            "Swap:                  0 kB\n"
            "KernelPageSize:        4 kB\n"
            "MMUPageSize:           4 kB\n"
            "Locked:                0 kB\n"
            "VmFlags: rd ex mr mw me dw sd\n"
            // A good anonymous region at the end.
            "7fe7ce79c000-7fe7ce7a8000 ---p 00000000 00:00 0 \n"
            "Size:                 48 kB\n"
            "Rss:                  40 kB\n"
            "Pss:                  32 kB\n"
            "Shared_Clean:         16 kB\n"
            "Shared_Dirty:         12 kB\n"
            "Private_Clean:         8 kB\n"
            "Private_Dirty:         4 kB\n"
            "Referenced:           40 kB\n"
            "Anonymous:            16 kB\n"
            "AnonHugePages:         0 kB\n"
            "Swap:                  0 kB\n"
            "KernelPageSize:        4 kB\n"
            "MMUPageSize:           4 kB\n"
            "Locked:                0 kB\n"
            "VmFlags: rd wr mr mw me ac sd\n";

        void CreateAndSetSmapsFileForTesting(const char* smaps_string,
            ScopedFILE& file)
        {
            FilePath temp_path;
            FILE* temp_file = CreateAndOpenTemporaryFile(&temp_path);
            file.reset(temp_file);
            ASSERT_TRUE(temp_file);

            ASSERT_TRUE(base::WriteFileDescriptor(fileno(temp_file), smaps_string,
                strlen(smaps_string)));
        }

    } // namespace

    TEST(ProcessMemoryMapsDumpProviderTest, ParseProcSmaps)
    {
        const uint32 kProtR = ProcessMemoryMaps::VMRegion::kProtectionFlagsRead;
        const uint32 kProtW = ProcessMemoryMaps::VMRegion::kProtectionFlagsWrite;
        const uint32 kProtX = ProcessMemoryMaps::VMRegion::kProtectionFlagsExec;
        const MemoryDumpArgs dump_args = { MemoryDumpLevelOfDetail::DETAILED };

        auto pmmdp = ProcessMemoryMapsDumpProvider::GetInstance();

        // Emulate an empty /proc/self/smaps.
        ProcessMemoryDump pmd_invalid(nullptr /* session_state */);
        ScopedFILE empty_file(OpenFile(FilePath("/dev/null"), "r"));
        ASSERT_TRUE(empty_file.get());
        ProcessMemoryMapsDumpProvider::proc_smaps_for_testing = empty_file.get();
        pmmdp->OnMemoryDump(dump_args, &pmd_invalid);
        ASSERT_FALSE(pmd_invalid.has_process_mmaps());

        // Parse the 1st smaps file.
        ProcessMemoryDump pmd_1(nullptr /* session_state */);
        ScopedFILE temp_file1;
        CreateAndSetSmapsFileForTesting(kTestSmaps1, temp_file1);
        ProcessMemoryMapsDumpProvider::proc_smaps_for_testing = temp_file1.get();
        pmmdp->OnMemoryDump(dump_args, &pmd_1);
        ASSERT_TRUE(pmd_1.has_process_mmaps());
        const auto& regions_1 = pmd_1.process_mmaps()->vm_regions();
        ASSERT_EQ(2UL, regions_1.size());

        EXPECT_EQ(0x00400000UL, regions_1[0].start_address);
        EXPECT_EQ(0x004be000UL - 0x00400000UL, regions_1[0].size_in_bytes);
        EXPECT_EQ(kProtR | kProtX, regions_1[0].protection_flags);
        EXPECT_EQ("/file/1", regions_1[0].mapped_file);
        EXPECT_EQ(162 * 1024UL, regions_1[0].byte_stats_proportional_resident);
        EXPECT_EQ(228 * 1024UL, regions_1[0].byte_stats_shared_clean_resident);
        EXPECT_EQ(0UL, regions_1[0].byte_stats_shared_dirty_resident);
        EXPECT_EQ(0UL, regions_1[0].byte_stats_private_clean_resident);
        EXPECT_EQ(68 * 1024UL, regions_1[0].byte_stats_private_dirty_resident);
        EXPECT_EQ(4 * 1024UL, regions_1[0].byte_stats_swapped);

        EXPECT_EQ(0xff000000UL, regions_1[1].start_address);
        EXPECT_EQ(0xff800000UL - 0xff000000UL, regions_1[1].size_in_bytes);
        EXPECT_EQ(kProtW, regions_1[1].protection_flags);
        EXPECT_EQ("/file/name with space", regions_1[1].mapped_file);
        EXPECT_EQ(128 * 1024UL, regions_1[1].byte_stats_proportional_resident);
        EXPECT_EQ(120 * 1024UL, regions_1[1].byte_stats_shared_clean_resident);
        EXPECT_EQ(4 * 1024UL, regions_1[1].byte_stats_shared_dirty_resident);
        EXPECT_EQ(60 * 1024UL, regions_1[1].byte_stats_private_clean_resident);
        EXPECT_EQ(8 * 1024UL, regions_1[1].byte_stats_private_dirty_resident);
        EXPECT_EQ(0 * 1024UL, regions_1[1].byte_stats_swapped);

        // Parse the 2nd smaps file.
        ProcessMemoryDump pmd_2(nullptr /* session_state */);
        ScopedFILE temp_file2;
        CreateAndSetSmapsFileForTesting(kTestSmaps2, temp_file2);
        ProcessMemoryMapsDumpProvider::proc_smaps_for_testing = temp_file2.get();
        pmmdp->OnMemoryDump(dump_args, &pmd_2);
        ASSERT_TRUE(pmd_2.has_process_mmaps());
        const auto& regions_2 = pmd_2.process_mmaps()->vm_regions();
        ASSERT_EQ(1UL, regions_2.size());
        EXPECT_EQ(0x7fe7ce79c000UL, regions_2[0].start_address);
        EXPECT_EQ(0x7fe7ce7a8000UL - 0x7fe7ce79c000UL, regions_2[0].size_in_bytes);
        EXPECT_EQ(0U, regions_2[0].protection_flags);
        EXPECT_EQ("", regions_2[0].mapped_file);
        EXPECT_EQ(32 * 1024UL, regions_2[0].byte_stats_proportional_resident);
        EXPECT_EQ(16 * 1024UL, regions_2[0].byte_stats_shared_clean_resident);
        EXPECT_EQ(12 * 1024UL, regions_2[0].byte_stats_shared_dirty_resident);
        EXPECT_EQ(8 * 1024UL, regions_2[0].byte_stats_private_clean_resident);
        EXPECT_EQ(4 * 1024UL, regions_2[0].byte_stats_private_dirty_resident);
        EXPECT_EQ(0 * 1024UL, regions_2[0].byte_stats_swapped);
    }

} // namespace trace_event
} // namespace base
