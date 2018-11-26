// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/asar/ScopedTemporaryFile.h"

//#include "base/files/file_util.h"
//#include "base/threading/thread_restrictions.h"
#include "base/files/file.h"
#include "base/files/file_path.h"

#include <windows.h>
#include <vector>

namespace {

// bool CopyDirectory(const FilePath& from_path, const FilePath& to_path, bool recursive) {
//     //base::ThreadRestrictions::AssertIOAllowed();
// 
//     if (recursive)
//         return ShellCopy(from_path, to_path, true);
// 
//     // The following code assumes that from path is a directory.
//     DCHECK(DirectoryExists(from_path));
// 
//     // Instead of creating a new directory, we copy the old one to include the
//     // security information of the folder as part of the copy.
//     if (!PathExists(to_path)) {
//         // Except that Vista fails to do that, and instead do a recursive copy if
//         // the target directory doesn't exist.
//         if (base::win::GetVersion() >= base::win::VERSION_VISTA)
//             CreateDirectory(to_path);
//         else
//             ShellCopy(from_path, to_path, false);
//     }
// 
//     FilePath directory = from_path.Append(L"*.*");
//     return ShellCopy(directory, to_path, false);
// }
// 
// bool CopyAndDeleteDirectory(const FilePath& from_path,
//     const FilePath& to_path) {
//     //ThreadRestrictions::AssertIOAllowed();
//     if (CopyDirectory(from_path, to_path, true)) {
//         if (DeleteFile(from_path, true))
//             return true;
// 
//         // Like Move, this function is not transactional, so we just
//         // leave the copied bits behind if deleting from_path fails.
//         // If to_path exists previously then we have already overwritten
//         // it by now, we don't get better off by deleting the new bits.
//     }
//     return false;
// }

bool MoveUnsafe(const base::FilePath& from_path, const base::FilePath& to_path) {
    //ThreadRestrictions::AssertIOAllowed();

    // NOTE: I suspect we could support longer paths, but that would involve
    // analyzing all our usage of files.
    if (from_path.value().length() >= MAX_PATH ||
        to_path.value().length() >= MAX_PATH) {
        return false;
    }
    if (::MoveFileEx(from_path.value().c_str(), to_path.value().c_str(),
        MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) != 0)
        return true;

    // Keep the last error value from MoveFileEx around in case the below
    // fails.
    bool ret = false;
    DWORD last_error = ::GetLastError();

//     if (::DirectoryExists(from_path)) {
//         // MoveFileEx fails if moving directory across volumes. We will simulate
//         // the move by using Copy and Delete. Ideally we could check whether
//         // from_path and to_path are indeed in different volumes.
//         ret = CopyAndDeleteDirectory(from_path, to_path);
//     }

    if (!ret) {
        // Leave a clue about what went wrong so that it can be (at least) picked
        // up by a PLOG entry.
        ::SetLastError(last_error);
    }

    return ret;
}

// bool CopyFile(const FilePath& from_path, const FilePath& to_path) {
//     if (from_path.ReferencesParent() || to_path.ReferencesParent())
//         return false;
//     return CopyFileUnsafe(from_path, to_path);
// }

bool GetTempDir(base::FilePath* path) {
    //base::ThreadRestrictions::AssertIOAllowed();

    wchar_t temp_path[MAX_PATH + 1];
    DWORD path_len = ::GetTempPath(MAX_PATH, temp_path);
    if (path_len >= MAX_PATH || path_len <= 0)
        return false;
    // TODO(evanm): the old behavior of this function was to always strip the
    // trailing slash.  We duplicate this here, but it shouldn't be necessary
    // when everyone is using the appropriate base::FilePath APIs.
    *path = base::FilePath(temp_path).StripTrailingSeparators();
    return true;
}

bool CreateTemporaryFileInDir(const base::FilePath& dir,
    base::FilePath* temp_file) {
    //base::ThreadRestrictions::AssertIOAllowed();

    wchar_t temp_name[MAX_PATH + 1];

    if (!GetTempFileName(dir.value().c_str(), L"", 0, temp_name)) {
        //DPLOG(WARNING) << "Failed to get temporary file name in " << dir.value();
        return false;
    }

    DWORD path_len = GetLongPathName(temp_name, temp_name, MAX_PATH);
    if (path_len > MAX_PATH + 1 || path_len == 0) {
        //DPLOG(WARNING) << "Failed to get long path name for " << temp_name;
        return false;
    }

    std::wstring temp_file_str;
    temp_file_str.assign(temp_name, path_len);
    *temp_file = base::FilePath(temp_file_str);
    return true;
}

bool CreateTemporaryFile(base::FilePath* path) {
    //base::ThreadRestrictions::AssertIOAllowed();

    base::FilePath temp_file;

    if (!GetTempDir(path))
        return false;

    if (CreateTemporaryFileInDir(*path, &temp_file)) {
        *path = temp_file;
        return true;
    }

    return false;
}

bool Move(const base::FilePath& from_path, const base::FilePath& to_path) {
    if (from_path.ReferencesParent() || to_path.ReferencesParent())
        return false;
    return MoveUnsafe(from_path, to_path);
}

}

namespace asar {

ScopedTemporaryFile::ScopedTemporaryFile() {
}

ScopedTemporaryFile::~ScopedTemporaryFile() {
  if (!path_.empty()) {
    //base::ThreadRestrictions::ScopedAllowIO allow_io;
    // On Windows it is very likely the file is already in use (because it is
    // mostly used for Node native modules), so deleting it now will halt the
    // program.
#if defined(OS_WIN)
    //base::DeleteFileAfterReboot(path_);
#else
    base::DeleteFile(path_, false);
#endif
  }
}

bool ScopedTemporaryFile::Init(const base::FilePath::StringType& ext) {
  if (!path_.empty())
    return true;

  //base::ThreadRestrictions::ScopedAllowIO allow_io;
  if (!CreateTemporaryFile(&path_))
    return false;

#if defined(OS_WIN)
  // Keep the original extension.
  if (!ext.empty()) {
    base::FilePath new_path = path_.AddExtension(ext);
    if (!Move(path_, new_path))
      return false;
    path_ = new_path;
  }
#endif

  return true;
}

bool ScopedTemporaryFile::InitFromFile(base::File* src,
                                       const base::FilePath::StringType& ext,
                                       uint64_t offset, uint64_t size) {
  if (!src->IsValid())
    return false;

  if (!Init(ext))
    return false;

  std::vector<char> buf((size_t)size);
  int len = src->Read(offset, &buf[0], buf.size());
  if (len != static_cast<int>(size))
    return false;

  base::File dest(path_, base::File::FLAG_OPEN | base::File::FLAG_WRITE);
  if (!dest.IsValid())
    return false;

  return dest.WriteAtCurrentPos(&buf[0], buf.size()) == static_cast<int>(size);
}

}  // namespace asar
