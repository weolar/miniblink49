// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/platform_file.h"

#include <io.h>

#include "base/files/file_path.h"
#include "base/logging.h"
//#include "base/metrics/sparse_histogram.h"
//#include "base/threading/thread_restrictions.h"

namespace base {

bool ClosePlatformFile(PlatformFile file) {
    //base::ThreadRestrictions::AssertIOAllowed();
    return (CloseHandle(file) != 0);
}

int64 SeekPlatformFile(PlatformFile file,
    PlatformFileWhence whence,
    int64 offset) {
    //base::ThreadRestrictions::AssertIOAllowed();
    if (file == kInvalidPlatformFileValue || offset < 0)
        return -1;

    LARGE_INTEGER distance, res;
    distance.QuadPart = offset;
    DWORD move_method = static_cast<DWORD>(whence);
    if (!SetFilePointerEx(file, distance, &res, move_method))
        return -1;
    return res.QuadPart;
}

int ReadPlatformFile(PlatformFile file, int64 offset, char* data, int size) {
    //base::ThreadRestrictions::AssertIOAllowed();
    if (file == kInvalidPlatformFileValue || size < 0)
        return -1;

    LARGE_INTEGER offset_li;
    offset_li.QuadPart = offset;

    OVERLAPPED overlapped = { 0 };
    overlapped.Offset = offset_li.LowPart;
    overlapped.OffsetHigh = offset_li.HighPart;

    DWORD bytes_read;
    if (::ReadFile(file, data, size, &bytes_read, &overlapped) != 0)
        return bytes_read;
    if (ERROR_HANDLE_EOF == GetLastError())
        return 0;

    return -1;
}

int ReadPlatformFileAtCurrentPos(PlatformFile file, char* data, int size) {
    //base::ThreadRestrictions::AssertIOAllowed();
    if (file == kInvalidPlatformFileValue || size < 0)
        return -1;

    DWORD bytes_read;
    if (::ReadFile(file, data, size, &bytes_read, NULL) != 0)
        return bytes_read;
    if (ERROR_HANDLE_EOF == GetLastError())
        return 0;

    return -1;
}

int ReadPlatformFileNoBestEffort(PlatformFile file, int64 offset, char* data,
    int size) {
    return ReadPlatformFile(file, offset, data, size);
}

int ReadPlatformFileCurPosNoBestEffort(PlatformFile file,
    char* data, int size) {
    return ReadPlatformFileAtCurrentPos(file, data, size);
}

int WritePlatformFile(PlatformFile file, int64 offset,
    const char* data, int size) {
    //base::ThreadRestrictions::AssertIOAllowed();
    if (file == kInvalidPlatformFileValue)
        return -1;

    LARGE_INTEGER offset_li;
    offset_li.QuadPart = offset;

    OVERLAPPED overlapped = { 0 };
    overlapped.Offset = offset_li.LowPart;
    overlapped.OffsetHigh = offset_li.HighPart;

    DWORD bytes_written;
    if (::WriteFile(file, data, size, &bytes_written, &overlapped) != 0)
        return bytes_written;

    return -1;
}

int WritePlatformFileAtCurrentPos(PlatformFile file, const char* data,
    int size) {
    return WritePlatformFile(file, 0, data, size);
}

int WritePlatformFileCurPosNoBestEffort(PlatformFile file,
    const char* data, int size) {
    return WritePlatformFile(file, 0, data, size);
}

bool TruncatePlatformFile(PlatformFile file, int64 length) {
    //base::ThreadRestrictions::AssertIOAllowed();
    if (file == kInvalidPlatformFileValue)
        return false;

    // Get the current file pointer.
    LARGE_INTEGER file_pointer;
    LARGE_INTEGER zero;
    zero.QuadPart = 0;
    if (::SetFilePointerEx(file, zero, &file_pointer, FILE_CURRENT) == 0)
        return false;

    LARGE_INTEGER length_li;
    length_li.QuadPart = length;
    // If length > file size, SetFilePointerEx() should extend the file
    // with zeroes on all Windows standard file systems (NTFS, FATxx).
    if (!::SetFilePointerEx(file, length_li, NULL, FILE_BEGIN))
        return false;

    // Set the new file length and move the file pointer to its old position.
    // This is consistent with ftruncate()'s behavior, even when the file
    // pointer points to a location beyond the end of the file.
    return ((::SetEndOfFile(file) != 0) &&
        (::SetFilePointerEx(file, file_pointer, NULL, FILE_BEGIN) != 0));
}

bool FlushPlatformFile(PlatformFile file) {
    //base::ThreadRestrictions::AssertIOAllowed();
    return ((file != kInvalidPlatformFileValue) && ::FlushFileBuffers(file));
}

bool TouchPlatformFile(PlatformFile file, const base::Time& last_access_time,
    const base::Time& last_modified_time) {
    //base::ThreadRestrictions::AssertIOAllowed();
    if (file == kInvalidPlatformFileValue)
        return false;

    FILETIME last_access_filetime = last_access_time.ToFileTime();
    FILETIME last_modified_filetime = last_modified_time.ToFileTime();
    return (::SetFileTime(file, NULL, &last_access_filetime,
        &last_modified_filetime) != 0);
}

bool GetPlatformFileInfo(PlatformFile file, PlatformFileInfo* info) {
    //base::ThreadRestrictions::AssertIOAllowed();
    if (!info)
        return false;

    BY_HANDLE_FILE_INFORMATION file_info;
    if (GetFileInformationByHandle(file, &file_info) == 0)
        return false;

    LARGE_INTEGER size;
    size.HighPart = file_info.nFileSizeHigh;
    size.LowPart = file_info.nFileSizeLow;
    info->size = size.QuadPart;
    info->is_directory =
        (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    info->is_symbolic_link = false; // Windows doesn't have symbolic links.
    info->last_modified = base::Time::FromFileTime(file_info.ftLastWriteTime);
    info->last_accessed = base::Time::FromFileTime(file_info.ftLastAccessTime);
    info->creation_time = base::Time::FromFileTime(file_info.ftCreationTime);
    return true;
}

PlatformFileError LockPlatformFile(PlatformFile file) {
    BOOL result = LockFile(file, 0, 0, MAXDWORD, MAXDWORD);
    if (!result)
        return LastErrorToPlatformFileError(GetLastError());
    return PLATFORM_FILE_OK;
}

PlatformFileError UnlockPlatformFile(PlatformFile file) {
    BOOL result = UnlockFile(file, 0, 0, MAXDWORD, MAXDWORD);
    if (!result)
        return LastErrorToPlatformFileError(GetLastError());
    return PLATFORM_FILE_OK;
}

PlatformFileError LastErrorToPlatformFileError(DWORD last_error) {
    switch (last_error) {
    case ERROR_SHARING_VIOLATION:
        return PLATFORM_FILE_ERROR_IN_USE;
    case ERROR_FILE_EXISTS:
        return PLATFORM_FILE_ERROR_EXISTS;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return PLATFORM_FILE_ERROR_NOT_FOUND;
    case ERROR_ACCESS_DENIED:
        return PLATFORM_FILE_ERROR_ACCESS_DENIED;
    case ERROR_TOO_MANY_OPEN_FILES:
        return PLATFORM_FILE_ERROR_TOO_MANY_OPENED;
    case ERROR_OUTOFMEMORY:
    case ERROR_NOT_ENOUGH_MEMORY:
        return PLATFORM_FILE_ERROR_NO_MEMORY;
    case ERROR_HANDLE_DISK_FULL:
    case ERROR_DISK_FULL:
    //case ERROR_DISK_RESOURCES_EXHAUSTED:
        return PLATFORM_FILE_ERROR_NO_SPACE;
    case ERROR_USER_MAPPED_FILE:
        return PLATFORM_FILE_ERROR_INVALID_OPERATION;
    case ERROR_NOT_READY:
    case ERROR_SECTOR_NOT_FOUND:
    case ERROR_DEV_NOT_EXIST:
    case ERROR_IO_DEVICE:
    case ERROR_FILE_CORRUPT:
    case ERROR_DISK_CORRUPT:
        return PLATFORM_FILE_ERROR_IO;
    default:
        //UMA_HISTOGRAM_SPARSE_SLOWLY("PlatformFile.UnknownErrors.Windows", last_error);
        return PLATFORM_FILE_ERROR_FAILED;
    }
}

}  // namespace base
