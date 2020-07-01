// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/asar/AsarUtil.h"

#include <map>
#include <string>

#include "common/asar/Archive.h"
#include "base/files/file_path.h"
//#include "base/files/file_util.h"
//#include "base/lazy_instance.h"
//#include "base/stl_util.h"

namespace asar {

namespace {

// The global instance of ArchiveMap, will be destroyed on exit.
typedef std::map<base::FilePath, Archive*> ArchiveMap;
//static base::LazyInstance<ArchiveMap> g_archive_map = LAZY_INSTANCE_INITIALIZER;
static ArchiveMap* g_archive_map = nullptr;

const base::FilePath::CharType kAsarExtension[] = FILE_PATH_LITERAL(".asar");

}  // namespace

Archive* GetOrCreateAsarArchive(const base::FilePath& path) {
    if (!g_archive_map)
        g_archive_map = new ArchiveMap();
    ArchiveMap& archive_map = *g_archive_map;
    ArchiveMap::iterator it = archive_map.find(path);
    if (it == archive_map.end()) {
        Archive* archive(new Archive(path));
        if (!archive->Init()) {
            delete archive;
            return nullptr;
        }
        archive_map[path] = archive;
    }
    return archive_map[path];
}

bool GetAsarArchivePath(const base::FilePath& full_path,
    base::FilePath* asar_path,
    base::FilePath* relative_path) {
    base::FilePath iter = full_path;
    while (true) {
        base::FilePath dirname = iter.DirName();
        if (iter.MatchesExtension(kAsarExtension))
            break;
        else if (iter == dirname)
            return false;
        iter = dirname;
    }

    base::FilePath tail;
    if (!iter.AppendRelativePath(full_path, &tail))
        return false;

    *asar_path = iter;
    *relative_path = tail;
    return true;
}

bool ReadFileToString(const wchar_t* path, std::string* buffer) {
    HANDLE hFile = ::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return false;

    DWORD fileSizeHigh;
    const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

    DWORD numberOfBytesRead = 0;
    buffer->resize(bufferSize);
    BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
    ::CloseHandle(hFile);
    return !!b && 0 != numberOfBytesRead;
}

bool ReadFileToString(const base::FilePath& path, std::string* contents) {
    base::FilePath asar_path, relative_path;
    if (!GetAsarArchivePath(path, &asar_path, &relative_path))
        return ReadFileToString(path.AsUTF16Unsafe().c_str(), contents);

    Archive* archive = GetOrCreateAsarArchive(asar_path);
    if (!archive)
        return false;

    Archive::FileInfo info;
    if (!archive->GetFileInfo(relative_path, &info))
        return false;

    if (info.unpacked) {
        base::FilePath real_path;
        // For unpacked file it will return the real path instead of doing the copy.
        archive->CopyFileOut(relative_path, &real_path);
        return ReadFileToString(real_path, contents);
    }

    base::File src(asar_path, base::File::FLAG_OPEN | base::File::FLAG_READ);
    if (!src.IsValid())
        return false;

    contents->resize(info.size);
    return static_cast<int>(info.size) == src.Read(info.offset, const_cast<char*>(contents->data()), contents->size());
}

}  // namespace asar
