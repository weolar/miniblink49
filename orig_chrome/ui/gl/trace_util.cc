// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/trace_util.h"

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"

namespace gfx {

base::trace_event::MemoryAllocatorDumpGuid GetGLTextureClientGUIDForTracing(
    uint64_t share_group_guid,
    uint32_t texture_id)
{
    return base::trace_event::MemoryAllocatorDumpGuid(
        base::StringPrintf("gl-texture-client-x-process/%" PRIx64 "/%d",
            share_group_guid, texture_id));
}

base::trace_event::MemoryAllocatorDumpGuid GetGLTextureServiceGUIDForTracing(
    uint64_t share_group_guid,
    uint32_t texture_id)
{
    return base::trace_event::MemoryAllocatorDumpGuid(
        base::StringPrintf("gl-texture-service-x-process/%" PRIx64 "/%d",
            share_group_guid, texture_id));
}

base::trace_event::MemoryAllocatorDumpGuid GetGLBufferGUIDForTracing(
    uint64_t share_group_guid,
    uint32_t buffer_id)
{
    return base::trace_event::MemoryAllocatorDumpGuid(base::StringPrintf(
        "gl-buffer-x-process/%" PRIx64 "/%d", share_group_guid, buffer_id));
}

base::trace_event::MemoryAllocatorDumpGuid GetGLRenderbufferGUIDForTracing(
    uint64_t share_group_guid,
    uint32_t renderbuffer_id)
{
    return base::trace_event::MemoryAllocatorDumpGuid(
        base::StringPrintf("gl-renderbuffer-x-process/%" PRIx64 "/%d",
            share_group_guid, renderbuffer_id));
}

} // namespace ui
