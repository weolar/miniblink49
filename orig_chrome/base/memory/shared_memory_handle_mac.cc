// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/shared_memory_handle.h"

#include <mach/mach_vm.h>
#include <sys/mman.h>
#include <unistd.h>

#include "base/mac/mac_util.h"
#include "base/posix/eintr_wrapper.h"

namespace base {

static_assert(sizeof(SharedMemoryHandle::Type) <= sizeof(SharedMemoryHandle::TypeWireFormat),
    "Size of enum SharedMemoryHandle::Type exceeds size of type "
    "transmitted over wire.");

SharedMemoryHandle::SharedMemoryHandle()
    : type_(POSIX)
    , file_descriptor_()
{
}

SharedMemoryHandle::SharedMemoryHandle(
    const base::FileDescriptor& file_descriptor)
    : type_(POSIX)
    , file_descriptor_(file_descriptor)
{
}

SharedMemoryHandle::SharedMemoryHandle(int fd, bool auto_close)
    : type_(POSIX)
    , file_descriptor_(fd, auto_close)
{
}

SharedMemoryHandle::SharedMemoryHandle(mach_vm_size_t size)
{
    type_ = MACH;
    mach_port_t named_right;
    kern_return_t kr = mach_make_memory_entry_64(
        mach_task_self(),
        &size,
        0, // Address.
        MAP_MEM_NAMED_CREATE | VM_PROT_READ | VM_PROT_WRITE,
        &named_right,
        MACH_PORT_NULL); // Parent handle.
    if (kr != KERN_SUCCESS) {
        memory_object_ = MACH_PORT_NULL;
        return;
    }

    memory_object_ = named_right;
    size_ = size;
    pid_ = GetCurrentProcId();
    ownership_passes_to_ipc_ = false;
}

SharedMemoryHandle::SharedMemoryHandle(mach_port_t memory_object,
    mach_vm_size_t size,
    base::ProcessId pid)
    : type_(MACH)
    , memory_object_(memory_object)
    , size_(size)
    , pid_(pid)
    , ownership_passes_to_ipc_(false)
{
}

SharedMemoryHandle::SharedMemoryHandle(const SharedMemoryHandle& handle)
    : type_(handle.type_)
{
    CopyRelevantData(handle);
}

SharedMemoryHandle& SharedMemoryHandle::operator=(
    const SharedMemoryHandle& handle)
{
    if (this == &handle)
        return *this;

    type_ = handle.type_;
    CopyRelevantData(handle);
    return *this;
}

SharedMemoryHandle SharedMemoryHandle::Duplicate() const
{
    switch (type_) {
    case POSIX: {
        if (!IsValid())
            return SharedMemoryHandle();

        int duped_fd = HANDLE_EINTR(dup(file_descriptor_.fd));
        if (duped_fd < 0)
            return SharedMemoryHandle();
        return SharedMemoryHandle(duped_fd, true);
    }
    case MACH: {
        if (!IsValid())
            return SharedMemoryHandle(MACH_PORT_NULL, 0, 0);

        // Increment the ref count.
        kern_return_t kr = mach_port_mod_refs(mach_task_self(), memory_object_,
            MACH_PORT_RIGHT_SEND, 1);
        DCHECK_EQ(kr, KERN_SUCCESS);
        return SharedMemoryHandle(*this);
    }
    }
}

bool SharedMemoryHandle::operator==(const SharedMemoryHandle& handle) const
{
    if (!IsValid() && !handle.IsValid())
        return true;

    if (type_ != handle.type_)
        return false;

    switch (type_) {
    case POSIX:
        return file_descriptor_ == handle.file_descriptor_;
    case MACH:
        return memory_object_ == handle.memory_object_ && size_ == handle.size_ && pid_ == handle.pid_;
    }
}

bool SharedMemoryHandle::operator!=(const SharedMemoryHandle& handle) const
{
    return !(*this == handle);
}

SharedMemoryHandle::Type SharedMemoryHandle::GetType() const
{
    return type_;
}

bool SharedMemoryHandle::IsValid() const
{
    switch (type_) {
    case POSIX:
        return file_descriptor_.fd >= 0;
    case MACH:
        return memory_object_ != MACH_PORT_NULL;
    }
}

void SharedMemoryHandle::SetFileHandle(int fd, bool auto_close)
{
    DCHECK(!IsValid());
    file_descriptor_.fd = fd;
    file_descriptor_.auto_close = auto_close;
    type_ = POSIX;
}

const FileDescriptor SharedMemoryHandle::GetFileDescriptor() const
{
    DCHECK_EQ(type_, POSIX);
    return file_descriptor_;
}

mach_port_t SharedMemoryHandle::GetMemoryObject() const
{
    DCHECK_EQ(type_, MACH);
    return memory_object_;
}

bool SharedMemoryHandle::GetSize(size_t* size) const
{
    if (!IsValid())
        return false;

    switch (type_) {
    case SharedMemoryHandle::POSIX:
        struct stat st;
        if (fstat(file_descriptor_.fd, &st) != 0)
            return false;
        if (st.st_size < 0)
            return false;
        *size = st.st_size;
        return true;
    case SharedMemoryHandle::MACH:
        *size = size_;
        return true;
    }
}

bool SharedMemoryHandle::MapAt(off_t offset,
    size_t bytes,
    void** memory,
    bool read_only)
{
    DCHECK(IsValid());
    switch (type_) {
    case SharedMemoryHandle::POSIX:
        *memory = mmap(nullptr, bytes, PROT_READ | (read_only ? 0 : PROT_WRITE),
            MAP_SHARED, file_descriptor_.fd, offset);

        return *memory && *memory != reinterpret_cast<void*>(-1);
    case SharedMemoryHandle::MACH:
        // The flag VM_PROT_IS_MASK is only supported on OSX 10.7+.
        DCHECK(mac::IsOSLionOrLater());

        DCHECK_EQ(pid_, GetCurrentProcId());
        kern_return_t kr = mach_vm_map(
            mach_task_self(),
            reinterpret_cast<mach_vm_address_t*>(memory), // Output parameter
            bytes,
            0, // Alignment mask
            VM_FLAGS_ANYWHERE,
            memory_object_,
            offset,
            FALSE, // Copy
            VM_PROT_READ | (read_only ? 0 : VM_PROT_WRITE), // Current protection
            VM_PROT_WRITE | VM_PROT_READ | VM_PROT_IS_MASK, // Maximum protection
            VM_INHERIT_NONE);
        return kr == KERN_SUCCESS;
    }
}

void SharedMemoryHandle::Close() const
{
    if (!IsValid())
        return;

    switch (type_) {
    case POSIX:
        if (IGNORE_EINTR(close(file_descriptor_.fd)) < 0)
            DPLOG(ERROR) << "Error closing fd.";
        break;
    case MACH:
        kern_return_t kr = mach_port_deallocate(mach_task_self(), memory_object_);
        if (kr != KERN_SUCCESS)
            DPLOG(ERROR) << "Error deallocating mach port: " << kr;
        break;
    }
}

void SharedMemoryHandle::SetOwnershipPassesToIPC(bool ownership_passes)
{
    DCHECK_EQ(type_, MACH);
    ownership_passes_to_ipc_ = ownership_passes;
}

bool SharedMemoryHandle::OwnershipPassesToIPC() const
{
    DCHECK_EQ(type_, MACH);
    return ownership_passes_to_ipc_;
}

void SharedMemoryHandle::CopyRelevantData(const SharedMemoryHandle& handle)
{
    switch (type_) {
    case POSIX:
        file_descriptor_ = handle.file_descriptor_;
        break;
    case MACH:
        memory_object_ = handle.memory_object_;
        size_ = handle.size_;
        pid_ = handle.pid_;
        ownership_passes_to_ipc_ = handle.ownership_passes_to_ipc_;
        break;
    }
}

} // namespace base
