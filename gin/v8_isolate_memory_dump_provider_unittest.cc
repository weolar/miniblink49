// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/v8_isolate_memory_dump_provider.h"

#include "base/trace_event/process_memory_dump.h"
#include "gin/public/isolate_holder.h"
#include "gin/test/v8_test.h"

namespace gin {

typedef V8Test V8MemoryDumpProviderTest;

// Checks if the dump provider runs without crashing and dumps root objects.
TEST_F(V8MemoryDumpProviderTest, DumpStatistics) {
  // Sets the track objects flag for dumping object statistics. Since this is
  // not set before V8::InitializePlatform the sizes will not be accurate, but
  // this serves the purpose of this test.
  const char track_objects_flag[] = "--track-gc-object-stats";
  v8::V8::SetFlagsFromString(track_objects_flag,
                             static_cast<int>(strlen(track_objects_flag)));

  scoped_ptr<base::trace_event::ProcessMemoryDump> process_memory_dump(
      new base::trace_event::ProcessMemoryDump(nullptr));
  instance_->isolate_memory_dump_provider_for_testing()->OnMemoryDump(
      process_memory_dump.get());
  const base::trace_event::ProcessMemoryDump::AllocatorDumpsMap&
      allocator_dumps = process_memory_dump->allocator_dumps();

  bool did_dump_isolate_stats = false;
  bool did_dump_space_stats = false;
  bool did_dump_objects_stats = false;
  for (const auto& it : allocator_dumps) {
    const std::string& dump_name = it.first;
    if (dump_name.find("v8/isolate") != std::string::npos) {
      did_dump_isolate_stats = true;
    }
    if (dump_name.find("heap_spaces") != std::string::npos) {
      did_dump_space_stats = true;
    } else if (dump_name.find("heap_objects") != std::string::npos) {
      did_dump_objects_stats = true;
    }
  }

  ASSERT_TRUE(did_dump_isolate_stats);
  ASSERT_TRUE(did_dump_space_stats);
  ASSERT_TRUE(did_dump_objects_stats);
}

}  // namespace gin
