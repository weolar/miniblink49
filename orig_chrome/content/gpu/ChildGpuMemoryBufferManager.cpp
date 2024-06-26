// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/gpu/ChildGpuMemoryBufferManager.h"
#include "base/atomic_sequence_num.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/gpu_memory_buffer.h"

namespace content {
namespace {

    void DeletedGpuMemoryBuffer( //ThreadSafeSender* sender,
        gfx::GpuMemoryBufferId id,
        const gpu::SyncToken& sync_token)
    {
        //   TRACE_EVENT0("renderer",
        //                "ChildGpuMemoryBufferManager::DeletedGpuMemoryBuffer");
        //   sender->Send(new ChildProcessHostMsg_DeletedGpuMemoryBuffer(id, sync_token));
    }

    // Global atomic to generate gpu memory buffer unique IDs.
    base::StaticAtomicSequenceNumber g_next_generic_shared_memory_id;

    gfx::GenericSharedMemoryId GetNextGenericSharedMemoryId()
    {
        return gfx::GenericSharedMemoryId(g_next_generic_shared_memory_id.GetNext());
    }

} // namespace

class GFX_EXPORT GpuMemoryBufferImpl : public gfx::GpuMemoryBuffer {
public:
    static scoped_ptr<GpuMemoryBufferImpl> Create(
        gfx::GpuMemoryBufferId id,
        const gfx::Size& size,
        gfx::BufferFormat format)
    {
        size_t buffer_size = 0u;
        if (!gfx::BufferSizeForBufferFormatChecked(size, format, &buffer_size))
            return nullptr;

        scoped_ptr<base::SharedMemory> shared_memory(new base::SharedMemory());
        if (!shared_memory->CreateAndMapAnonymous(buffer_size))
            return nullptr;

        return make_scoped_ptr(new GpuMemoryBufferImpl(
            id, size, format, shared_memory.Pass(), 0,
            gfx::RowSizeForBufferFormat(size.width(), format, 0)));
    }

    virtual ~GpuMemoryBufferImpl()
    {
    }

    // Maps each plane of the buffer into the client's address space so it can be
    // written to by the CPU. This call may block, for instance if the GPU needs
    // to finish accessing the buffer or if CPU caches need to be synchronized.
    // Returns false on failure.
    virtual bool Map() override
    {
        DCHECK(!mapped_);

        // Map the buffer first time Map() is called then keep it mapped for the
        // lifetime of the buffer. This avoids mapping the buffer unless necessary.
        if (!shared_memory_->memory()) {
            DCHECK_EQ(static_cast<size_t>(stride_), gfx::RowSizeForBufferFormat(size_.width(), format_, 0));
            size_t buffer_size = gfx::BufferSizeForBufferFormat(size_, format_);
            // Note: offset_ != 0 is not common use-case. To keep it simple we
            // map offset + buffer_size here but this can be avoided using MapAt().
            size_t map_size = offset_ + buffer_size;
            if (!shared_memory_->Map(map_size))
                DebugBreak();
        }
        mapped_ = true;
        return true;
    }

    // Returns a pointer to the memory address of a plane. Buffer must have been
    // successfully mapped using a call to Map() before calling this function.
    virtual void* memory(size_t plane) override
    {
        DCHECK(mapped_);
        DCHECK_LT(plane, gfx::NumberOfPlanesForBufferFormat(format_));
        return reinterpret_cast<uint8_t*>(shared_memory_->memory()) + offset_ + gfx::BufferOffsetForBufferFormat(size_, format_, plane);
    }

    // Unmaps the buffer. It's illegal to use any pointer returned by memory()
    // after this has been called.
    virtual void Unmap() override
    {
        DCHECK(mapped_);
        mapped_ = false;
    }

    // Returns true if the buffer is currently being read from by the system's
    // window server, and should not be written to.
    // TODO(ccameron): This is specific to the Mac OS WindowServer process, and
    // should be merged with synchronization mechanisms from other platforms, if
    // possible.
    virtual bool IsInUseByMacOSWindowServer() const override
    {
        return false;
    }

    // Returns the size for the buffer.
    virtual gfx::Size GetSize() const
    {
        return size_;
    }

    // Returns the format for the buffer.
    virtual gfx::BufferFormat GetFormat() const override
    {
        return format_;
    }

    // Fills the stride in bytes for each plane of the buffer. The stride of
    // plane K is stored at index K-1 of the |stride| array.
    virtual int stride(size_t plane) const override
    {
        DCHECK_LT(plane, gfx::NumberOfPlanesForBufferFormat(format_));
        return gfx::RowSizeForBufferFormat(size_.width(), format_, plane);
    }

    // Returns a unique identifier associated with buffer.
    virtual gfx::GpuMemoryBufferId GetId() const override
    {
        return id_;
    }

    // Returns a platform specific handle for this buffer.
    virtual gfx::GpuMemoryBufferHandle GetHandle() const override
    {
        gfx::GpuMemoryBufferHandle handle;
        handle.type = gfx::SHARED_MEMORY_BUFFER;
        handle.id = id_;
        handle.offset = offset_;
        handle.stride = stride_;
        handle.handle = shared_memory_->handle();
        return handle;
    }

    // Type-checking downcast routine.
    virtual ClientBuffer AsClientBuffer() override
    {
        return reinterpret_cast<ClientBuffer>(this);
    }

    static scoped_ptr<GpuMemoryBufferImpl> createFromHandle(
        const gfx::GpuMemoryBufferHandle& handle,
        const gfx::Size& size,
        gfx::BufferFormat format,
        gfx::BufferUsage usage)
    {
        if (!base::SharedMemory::IsHandleValid(handle.handle))
            return nullptr;

        return make_scoped_ptr(new GpuMemoryBufferImpl(
            handle.id, size, format,
            make_scoped_ptr(new base::SharedMemory(handle.handle, false)),
            handle.offset, handle.stride));
    }

private:
    GpuMemoryBufferImpl(
        gfx::GpuMemoryBufferId id,
        const gfx::Size& size,
        gfx::BufferFormat format,
        scoped_ptr<base::SharedMemory> shared_memory,
        size_t offset,
        int stride)
        : id_(id)
        , size_(size)
        , format_(format)
        , offset_(offset)
        , stride_(stride)
        , shared_memory_(shared_memory.Pass())
        , mapped_(false)
    {
    }

    const gfx::GpuMemoryBufferId id_;
    gfx::Size size_;
    size_t offset_;
    gfx::BufferFormat format_;
    int stride_;
    scoped_ptr<base::SharedMemory> shared_memory_;
    bool mapped_;
};

ChildGpuMemoryBufferManager::ChildGpuMemoryBufferManager()
{
}

ChildGpuMemoryBufferManager::~ChildGpuMemoryBufferManager()
{
}

gfx::GpuMemoryBufferHandle allocateForChildProcess(
    gfx::GpuMemoryBufferId id,
    const gfx::Size& size,
    gfx::BufferFormat format,
    base::ProcessHandle child_process)
{
    size_t buffer_size = 0u;
    if (!gfx::BufferSizeForBufferFormatChecked(size, format, &buffer_size))
        return gfx::GpuMemoryBufferHandle();

    base::SharedMemory shared_memory;
    if (!shared_memory.CreateAnonymous(buffer_size))
        return gfx::GpuMemoryBufferHandle();

    gfx::GpuMemoryBufferHandle handle;
    handle.type = gfx::SHARED_MEMORY_BUFFER;
    handle.id = id;
    handle.offset = 0;
    handle.stride = static_cast<int32_t>(gfx::RowSizeForBufferFormat(size.width(), format, 0));
    shared_memory.GiveToProcess(child_process, &handle.handle);
    return handle;
}

scoped_ptr<gfx::GpuMemoryBuffer> ChildGpuMemoryBufferManager::AllocateGpuMemoryBuffer(
    const gfx::Size& size,
    gfx::BufferFormat format,
    gfx::BufferUsage usage)
{
    gfx::GpuMemoryBufferHandle handle;
    base::ProcessHandle childProcess = ::GetCurrentProcess();
    handle = allocateForChildProcess(content::GetNextGenericSharedMemoryId(), size, format, childProcess);
    if (handle.is_null())
        return nullptr;

    scoped_ptr<GpuMemoryBufferImpl> buffer(GpuMemoryBufferImpl::createFromHandle(handle, size, format, usage));
    if (!buffer)
        return nullptr;
    return buffer.Pass();
}

scoped_ptr<gfx::GpuMemoryBuffer>
ChildGpuMemoryBufferManager::CreateGpuMemoryBufferFromHandle(const gfx::GpuMemoryBufferHandle& handle, const gfx::Size& size, gfx::BufferFormat format)
{
    NOTIMPLEMENTED();
    DebugBreak();
    return nullptr;
}

gfx::GpuMemoryBuffer* ChildGpuMemoryBufferManager::GpuMemoryBufferFromClientBuffer(ClientBuffer buffer)
{
    return reinterpret_cast<GpuMemoryBufferImpl*>(buffer);
}

void ChildGpuMemoryBufferManager::SetDestructionSyncToken(gfx::GpuMemoryBuffer* buffer, const gpu::SyncToken& sync_token)
{
    //static_cast<GpuMemoryBufferImpl*>(buffer)->set_destruction_sync_token(sync_token);
    DebugBreak();
}

} // namespace content
