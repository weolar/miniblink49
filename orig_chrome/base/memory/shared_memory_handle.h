// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MEMORY_SHARED_MEMORY_HANDLE_H_
#define BASE_MEMORY_SHARED_MEMORY_HANDLE_H_

#include "build/build_config.h"

#if defined(OS_WIN)
#include "base/process/process_handle.h"
#include <windows.h>
#elif defined(OS_MACOSX) && !defined(OS_IOS)
#include "base/base_export.h"
#include "base/file_descriptor_posix.h"
#include "base/macros.h"
#include "base/process/process_handle.h"
#include <mach/mach.h>
#include <sys/types.h>
#elif defined(OS_POSIX)
#include "base/file_descriptor_posix.h"
#include <sys/types.h>
#endif

namespace base {

class Pickle;

// SharedMemoryHandle is a platform specific type which represents
// the underlying OS handle to a shared memory segment.
#if defined(OS_POSIX) && !(defined(OS_MACOSX) && !defined(OS_IOS))
typedef FileDescriptor SharedMemoryHandle;
#elif defined(OS_WIN)
class BASE_EXPORT SharedMemoryHandle {
public:
    // The default constructor returns an invalid SharedMemoryHandle.
    SharedMemoryHandle();
    SharedMemoryHandle(HANDLE h, base::ProcessId pid);

    // Standard copy constructor. The new instance shares the underlying OS
    // primitives.
    SharedMemoryHandle(const SharedMemoryHandle& handle);

    // Standard assignment operator. The updated instance shares the underlying
    // OS primitives.
    SharedMemoryHandle& operator=(const SharedMemoryHandle& handle);

    // Comparison operators.
    bool operator==(const SharedMemoryHandle& handle) const;
    bool operator!=(const SharedMemoryHandle& handle) const;

    // Closes the underlying OS resources.
    void Close() const;

    // Whether the underlying OS primitive is valid.
    bool IsValid() const;

    // Whether |pid_| is the same as the current process's id.
    bool BelongsToCurrentProcess() const;

    // Whether handle_ needs to be duplicated into the destination process when
    // an instance of this class is passed over a Chrome IPC channel.
    bool NeedsBrokering() const;

    HANDLE GetHandle() const;
    base::ProcessId GetPID() const;

private:
    HANDLE handle_;

    // The process in which |handle_| is valid and can be used. If |handle_| is
    // invalid, this will be kNullProcessId.
    base::ProcessId pid_;
};
#else
class BASE_EXPORT SharedMemoryHandle {
public:
    // The values of these enums must not change, as they are used by the
    // histogram OSX.SharedMemory.Mechanism.
    enum Type {
        // The SharedMemoryHandle is backed by a POSIX fd.
        POSIX,
        // The SharedMemoryHandle is backed by the Mach primitive "memory object".
        MACH,
    };
    static const int TypeMax = 2;

    // The format that should be used to transmit |Type| over the wire.
    typedef int TypeWireFormat;

    // The default constructor returns an invalid SharedMemoryHandle.
    SharedMemoryHandle();

    // Constructs a SharedMemoryHandle backed by the components of a
    // FileDescriptor. The newly created instance has the same ownership semantics
    // as base::FileDescriptor. This typically means that the SharedMemoryHandle
    // takes ownership of the |fd| if |auto_close| is true. Unfortunately, it's
    // common for existing code to make shallow copies of SharedMemoryHandle, and
    // the one that is finally passed into a base::SharedMemory is the one that
    // "consumes" the fd.
    explicit SharedMemoryHandle(const base::FileDescriptor& file_descriptor);
    SharedMemoryHandle(int fd, bool auto_close);

    // Makes a Mach-based SharedMemoryHandle of the given size. On error,
    // subsequent calls to IsValid() return false.
    explicit SharedMemoryHandle(mach_vm_size_t size);

    // Makes a Mach-based SharedMemoryHandle from |memory_object|, a named entry
    // in the task with process id |pid|. The memory region has size |size|.
    SharedMemoryHandle(mach_port_t memory_object,
        mach_vm_size_t size,
        base::ProcessId pid);

    // Standard copy constructor. The new instance shares the underlying OS
    // primitives.
    SharedMemoryHandle(const SharedMemoryHandle& handle);

    // Standard assignment operator. The updated instance shares the underlying
    // OS primitives.
    SharedMemoryHandle& operator=(const SharedMemoryHandle& handle);

    // Duplicates the underlying OS resources.
    SharedMemoryHandle Duplicate() const;

    // Comparison operators.
    bool operator==(const SharedMemoryHandle& handle) const;
    bool operator!=(const SharedMemoryHandle& handle) const;

    // Returns the type.
    Type GetType() const;

    // Whether the underlying OS primitive is valid. Once the SharedMemoryHandle
    // is backed by a valid OS primitive, it becomes immutable.
    bool IsValid() const;

    // Sets the POSIX fd backing the SharedMemoryHandle. Requires that the
    // SharedMemoryHandle be backed by a POSIX fd.
    void SetFileHandle(int fd, bool auto_close);

    // This method assumes that the SharedMemoryHandle is backed by a POSIX fd.
    // This is eventually no longer going to be true, so please avoid adding new
    // uses of this method.
    const FileDescriptor GetFileDescriptor() const;

    // Exposed so that the SharedMemoryHandle can be transported between
    // processes.
    mach_port_t GetMemoryObject() const;

    // Returns false on a failure to determine the size. On success, populates the
    // output variable |size|.
    bool GetSize(size_t* size) const;

    // The SharedMemoryHandle must be valid.
    // Returns whether the SharedMemoryHandle was successfully mapped into memory.
    // On success, |memory| is an output variable that contains the start of the
    // mapped memory.
    bool MapAt(off_t offset, size_t bytes, void** memory, bool read_only);

    // Closes the underlying OS primitive.
    void Close() const;

    void SetOwnershipPassesToIPC(bool ownership_passes);
    bool OwnershipPassesToIPC() const;

private:
    // Shared code between copy constructor and operator=.
    void CopyRelevantData(const SharedMemoryHandle& handle);

    Type type_;

    // Each instance of a SharedMemoryHandle is backed either by a POSIX fd or a
    // mach port. |type_| determines the backing member.
    union {
        FileDescriptor file_descriptor_;

        struct {
            mach_port_t memory_object_;

            // The size of the shared memory region when |type_| is MACH. Only
            // relevant if |memory_object_| is not |MACH_PORT_NULL|.
            mach_vm_size_t size_;

            // The pid of the process in which |memory_object_| is usable. Only
            // relevant if |memory_object_| is not |MACH_PORT_NULL|.
            base::ProcessId pid_;

            // Whether passing this object as a parameter to an IPC message passes
            // ownership of |memory_object_| to the IPC stack. This is meant to mimic
            // the behavior of the |auto_close| parameter of FileDescriptor.
            // Defaults to |false|.
            bool ownership_passes_to_ipc_;
        };
    };
};
#endif

} // namespace base

#endif // BASE_MEMORY_SHARED_MEMORY_HANDLE_H_
