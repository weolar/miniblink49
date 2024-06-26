// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_dump_session_state.h"

namespace base {
namespace trace_event {

    MemoryDumpSessionState::MemoryDumpSessionState(
        const scoped_refptr<StackFrameDeduplicator>& stack_frame_deduplicator)
        : stack_frame_deduplicator_(stack_frame_deduplicator)
    {
    }

    MemoryDumpSessionState::~MemoryDumpSessionState()
    {
    }

} // namespace trace_event
} // namespace base
