// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/shared_memory_handle.h"

#include "base/logging.h"

namespace base {

SharedMemoryHandle::SharedMemoryHandle()
    : handle_(nullptr)
    , pid_(kNullProcessId)
{
}

SharedMemoryHandle::SharedMemoryHandle(HANDLE h, base::ProcessId pid)
    : handle_(h)
    , pid_(pid)
{
}

SharedMemoryHandle::SharedMemoryHandle(const SharedMemoryHandle& handle)
    : handle_(handle.handle_)
    , pid_(handle.pid_)
{
}

SharedMemoryHandle& SharedMemoryHandle::operator=(
    const SharedMemoryHandle& handle)
{
    if (this == &handle)
        return *this;

    handle_ = handle.handle_;
    pid_ = handle.pid_;
    return *this;
}

bool SharedMemoryHandle::operator==(const SharedMemoryHandle& handle) const
{
    // Invalid handles are always equal.
    if (!IsValid() && !handle.IsValid())
        return true;

    return handle_ == handle.handle_ && pid_ == handle.pid_;
}

bool SharedMemoryHandle::operator!=(const SharedMemoryHandle& handle) const
{
    return !(*this == handle);
}

void SharedMemoryHandle::Close() const
{
    DCHECK(handle_ != nullptr);
    DCHECK(BelongsToCurrentProcess());
    ::CloseHandle(handle_);
}

bool SharedMemoryHandle::IsValid() const
{
    return handle_ != nullptr;
}

bool SharedMemoryHandle::BelongsToCurrentProcess() const
{
    return pid_ == base::GetCurrentProcId();
}

bool SharedMemoryHandle::NeedsBrokering() const
{
    return false;
}

HANDLE SharedMemoryHandle::GetHandle() const
{
    return handle_;
}

base::ProcessId SharedMemoryHandle::GetPID() const
{
    return pid_;
}

} // namespace base
