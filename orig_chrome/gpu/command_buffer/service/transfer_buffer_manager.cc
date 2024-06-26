// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/transfer_buffer_manager.h"

#include <limits>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/process/process_handle.h"
#include "base/strings/stringprintf.h"
#include "base/thread_task_runner_handle.h"
// #include "base/trace_event/memory_allocator_dump.h"
// #include "base/trace_event/memory_dump_manager.h"
// #include "base/trace_event/process_memory_dump.h"
#include "base/trace_event/trace_event.h"
#include "gpu/command_buffer/common/cmd_buffer_common.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/memory_tracking.h"

using ::base::SharedMemory;

namespace gpu {

TransferBufferManagerInterface::~TransferBufferManagerInterface()
{
}

TransferBufferManager::TransferBufferManager(
    gles2::MemoryTracker* memory_tracker)
    : shared_memory_bytes_allocated_(0)
    , memory_tracker_(memory_tracker)
{
}

TransferBufferManager::~TransferBufferManager()
{
    while (!registered_buffers_.empty()) {
        BufferMap::iterator it = registered_buffers_.begin();
        DCHECK(shared_memory_bytes_allocated_ >= it->second->size());
        shared_memory_bytes_allocated_ -= it->second->size();
        registered_buffers_.erase(it);
    }
    DCHECK(!shared_memory_bytes_allocated_);

    //   base::trace_event::MemoryDumpManager::GetInstance()->UnregisterDumpProvider(
    //       this);
}

bool TransferBufferManager::Initialize()
{
    // When created from InProcessCommandBuffer, we won't have a |memory_tracker_|
    // so don't register a dump provider.
    //   if (memory_tracker_) {
    //     base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider(
    //         this, "gpu::TransferBufferManager",
    //         base::ThreadTaskRunnerHandle::Get());
    //   }
    return true;
}

bool TransferBufferManager::RegisterTransferBuffer(
    int32 id,
    scoped_ptr<BufferBacking> buffer_backing)
{
    if (id <= 0) {
        DVLOG(0) << "Cannot register transfer buffer with non-positive ID.";
        return false;
    }

    // Fail if the ID is in use.
    if (registered_buffers_.find(id) != registered_buffers_.end()) {
        DVLOG(0) << "Buffer ID already in use.";
        return false;
    }

    // Register the shared memory with the ID.
    scoped_refptr<Buffer> buffer(new gpu::Buffer(buffer_backing.Pass()));

    // Check buffer alignment is sane.
    DCHECK(!(reinterpret_cast<uintptr_t>(buffer->memory()) & (kCommandBufferEntrySize - 1)));

    shared_memory_bytes_allocated_ += buffer->size();

    registered_buffers_[id] = buffer;

    return true;
}

void TransferBufferManager::DestroyTransferBuffer(int32 id)
{
    BufferMap::iterator it = registered_buffers_.find(id);
    if (it == registered_buffers_.end()) {
        DVLOG(0) << "Transfer buffer ID was not registered.";
        return;
    }

    DCHECK(shared_memory_bytes_allocated_ >= it->second->size());
    shared_memory_bytes_allocated_ -= it->second->size();

    registered_buffers_.erase(it);
}

scoped_refptr<Buffer> TransferBufferManager::GetTransferBuffer(int32 id)
{
    if (id == 0)
        return NULL;

    BufferMap::iterator it = registered_buffers_.find(id);
    if (it == registered_buffers_.end())
        return NULL;

    return it->second;
}

bool TransferBufferManager::OnMemoryDump(
    const base::trace_event::MemoryDumpArgs& args,
    base::trace_event::ProcessMemoryDump* pmd)
{
    //   for (const auto& buffer_entry : registered_buffers_) {
    //     int32 buffer_id = buffer_entry.first;
    //     const Buffer* buffer = buffer_entry.second.get();
    //     std::string dump_name =
    //         base::StringPrintf("gpu/transfer_memory/client_%d/buffer_%d",
    //                            memory_tracker_->ClientId(), buffer_id);
    //     base::trace_event::MemoryAllocatorDump* dump =
    //         pmd->CreateAllocatorDump(dump_name);
    //     dump->AddScalar(base::trace_event::MemoryAllocatorDump::kNameSize,
    //                     base::trace_event::MemoryAllocatorDump::kUnitsBytes,
    //                     buffer->size());
    //     auto guid =
    //         GetBufferGUIDForTracing(memory_tracker_->ClientTracingId(), buffer_id);
    //     pmd->CreateSharedGlobalAllocatorDump(guid);
    //     pmd->AddOwnershipEdge(dump->guid(), guid);
    //   }

    return true;
}

} // namespace gpu
