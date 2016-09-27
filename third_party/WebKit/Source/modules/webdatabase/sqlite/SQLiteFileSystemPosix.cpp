/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/webdatabase/sqlite/SQLiteFileSystem.h"

#include <sqlite3.h>
#include "public/platform/Platform.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

// Defined in Chromium's codebase in third_party/sqlite/src/os_unix.c
extern "C" {
void chromium_sqlite3_initialize_unix_sqlite3_file(sqlite3_file* file);
int chromium_sqlite3_fill_in_unix_sqlite3_file(sqlite3_vfs* vfs, int fd, int dirfd, sqlite3_file* file, const char* fileName, int noLock);
int chromium_sqlite3_get_reusable_file_handle(sqlite3_file* file, const char* fileName, int flags, int* fd);
void chromium_sqlite3_update_reusable_file_handle(sqlite3_file* file, int fd, int flags);
void chromium_sqlite3_destroy_reusable_file_handle(sqlite3_file* file);
}

namespace blink {

// Chromium's Posix implementation of SQLite VFS
namespace {

struct chromiumVfsFile {
    sqlite3_io_methods* pMethods;
    sqlite3_file* wrappedFile;
    char* wrappedFileName;
};

int chromiumClose(sqlite3_file* sqliteFile)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    int r = chromiumFile->wrappedFile->pMethods->xClose(chromiumFile->wrappedFile);
    sqlite3_free(chromiumFile->wrappedFileName);
    sqlite3_free(chromiumFile->wrappedFile);
    memset(chromiumFile, 0, sizeof(*chromiumFile));
    return r;
}

int chromiumRead(sqlite3_file* sqliteFile, void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xRead(chromiumFile->wrappedFile, pBuf, iAmt, iOfst);
}

int chromiumWrite(sqlite3_file* sqliteFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xWrite(chromiumFile->wrappedFile, pBuf, iAmt, iOfst);
}

int chromiumTruncate(sqlite3_file* sqliteFile, sqlite3_int64 size)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);

    // The OSX and Linux sandboxes block ftruncate(), proxy to the browser
    // process.
    if (Platform::current()->databaseSetFileSize(String(chromiumFile->wrappedFileName), size))
        return SQLITE_OK;
    return SQLITE_IOERR_TRUNCATE;
}

int chromiumSync(sqlite3_file* sqliteFile, int flags)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xSync(chromiumFile->wrappedFile, flags);
}

int chromiumFileSize(sqlite3_file* sqliteFile, sqlite3_int64* pSize)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xFileSize(chromiumFile->wrappedFile, pSize);
}

int chromiumLock(sqlite3_file* sqliteFile, int eFileLock)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xLock(chromiumFile->wrappedFile, eFileLock);
}

int chromiumUnlock(sqlite3_file* sqliteFile, int eFileLock)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xUnlock(chromiumFile->wrappedFile, eFileLock);
}

int chromiumCheckReservedLock(sqlite3_file* sqliteFile, int* pResOut)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xCheckReservedLock(chromiumFile->wrappedFile, pResOut);
}

int chromiumFileControl(sqlite3_file* sqliteFile, int op, void* pArg)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xFileControl(chromiumFile->wrappedFile, op, pArg);
}

int chromiumSectorSize(sqlite3_file* sqliteFile)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xSectorSize(chromiumFile->wrappedFile);
}

int chromiumDeviceCharacteristics(sqlite3_file* sqliteFile)
{
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(sqliteFile);
    return chromiumFile->wrappedFile->pMethods->xDeviceCharacteristics(chromiumFile->wrappedFile);
}

// Opens a file.
//
// vfs - pointer to the sqlite3_vfs object.
// fileName - the name of the file.
// id - the structure that will manipulate the newly opened file.
// desiredFlags - the desired open mode flags.
// usedFlags - the actual open mode flags that were used.
int chromiumOpenInternal(sqlite3_vfs* vfs, const char* fileName, sqlite3_file* id, int desiredFlags, int* usedFlags)
{
    chromium_sqlite3_initialize_unix_sqlite3_file(id);
    int fd = -1;
    int result = chromium_sqlite3_get_reusable_file_handle(id, fileName, desiredFlags, &fd);
    if (result != SQLITE_OK)
        return result;

    if (fd < 0) {
        fd = Platform::current()->databaseOpenFile(String(fileName), desiredFlags);
        if ((fd < 0) && (desiredFlags & SQLITE_OPEN_READWRITE)) {
            int newFlags = (desiredFlags & ~(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)) | SQLITE_OPEN_READONLY;
            fd = Platform::current()->databaseOpenFile(String(fileName), newFlags);
        }
    }
    if (fd < 0) {
        chromium_sqlite3_destroy_reusable_file_handle(id);
        return SQLITE_CANTOPEN;
    }

    if (usedFlags)
        *usedFlags = desiredFlags;
    chromium_sqlite3_update_reusable_file_handle(id, fd, desiredFlags);

    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);

    // The mask 0x00007F00 gives us the 7 bits that determine the type of the file SQLite is trying to open.
    int fileType = desiredFlags & 0x00007F00;
    int noLock = (fileType != SQLITE_OPEN_MAIN_DB);
    sqlite3_vfs* wrappedVfs = static_cast<sqlite3_vfs*>(vfs->pAppData);
    result = chromium_sqlite3_fill_in_unix_sqlite3_file(wrappedVfs, fd, -1, id, fileName, noLock);
    if (result != SQLITE_OK)
        chromium_sqlite3_destroy_reusable_file_handle(id);
    return result;
}

int chromiumOpen(sqlite3_vfs* vfs, const char* fileName, sqlite3_file* id, int desiredFlags, int* usedFlags)
{
    sqlite3_vfs* wrappedVfs = static_cast<sqlite3_vfs*>(vfs->pAppData);
    sqlite3_file* wrappedFile = static_cast<sqlite3_file*>(sqlite3_malloc(wrappedVfs->szOsFile));
    if (!wrappedFile)
        return SQLITE_NOMEM;

    // Make a local copy of the file name.  SQLite's os_unix.c appears to be written to allow caching the pointer passed
    // in to this function, but that seems brittle.
    char* wrappedFileName = sqlite3_mprintf("%s", fileName);
    if (!wrappedFileName) {
        sqlite3_free(wrappedFile);
        return SQLITE_NOMEM;
    }

    // SQLite's unixOpen() makes assumptions about the structure of |fileName|.  Our local copy may not answer those
    // assumptions correctly.
    int rc = chromiumOpenInternal(vfs, fileName, wrappedFile, desiredFlags, usedFlags);
    if (rc != SQLITE_OK) {
        sqlite3_free(wrappedFileName);
        sqlite3_free(wrappedFile);
        return rc;
    }

    static sqlite3_io_methods chromiumIoMethods = {
        1,
        chromiumClose,
        chromiumRead,
        chromiumWrite,
        chromiumTruncate,
        chromiumSync,
        chromiumFileSize,
        chromiumLock,
        chromiumUnlock,
        chromiumCheckReservedLock,
        chromiumFileControl,
        chromiumSectorSize,
        chromiumDeviceCharacteristics,
        // Methods above are valid for version 1.
    };
    chromiumVfsFile* chromiumFile = reinterpret_cast<chromiumVfsFile*>(id);
    chromiumFile->pMethods = &chromiumIoMethods;
    chromiumFile->wrappedFile = wrappedFile;
    chromiumFile->wrappedFileName = wrappedFileName;
    return SQLITE_OK;
}

// Deletes the given file.
//
// vfs - pointer to the sqlite3_vfs object.
// fileName - the name of the file.
// syncDir - determines if the directory to which this file belongs
//           should be synched after the file is deleted.
int chromiumDelete(sqlite3_vfs*, const char* fileName, int syncDir)
{
    return Platform::current()->databaseDeleteFile(String(fileName), syncDir);
}

// Check the existance and status of the given file.
//
// vfs - pointer to the sqlite3_vfs object.
// fileName - the name of the file.
// flag - the type of test to make on this file.
// res - the result.
int chromiumAccess(sqlite3_vfs*, const char* fileName, int flag, int* res)
{
    int attr = static_cast<int>(Platform::current()->databaseGetFileAttributes(String(fileName)));
    if (attr < 0) {
        *res = 0;
        return SQLITE_OK;
    }

    switch (flag) {
    case SQLITE_ACCESS_EXISTS:
        *res = 1;   // if the file doesn't exist, attr < 0
        break;
    case SQLITE_ACCESS_READWRITE:
        *res = (attr & W_OK) && (attr & R_OK);
        break;
    case SQLITE_ACCESS_READ:
        *res = (attr & R_OK);
        break;
    default:
        return SQLITE_ERROR;
    }

    return SQLITE_OK;
}

// Turns a relative pathname into a full pathname.
//
// vfs - pointer to the sqlite3_vfs object.
// relativePath - the relative path.
// bufSize - the size of the output buffer in bytes.
// absolutePath - the output buffer where the absolute path will be stored.
int chromiumFullPathname(sqlite3_vfs* vfs, const char* relativePath, int bufSize, char* absolutePath)
{
    // The renderer process doesn't need to know the absolute path of the file
    sqlite3_snprintf(bufSize, absolutePath, "%s", relativePath);
    return SQLITE_OK;
}

// Do not allow loading libraries in the renderer.
void* chromiumDlOpen(sqlite3_vfs*, const char*)
{
    return 0;
}

void chromiumDlError(sqlite3_vfs*, int bufSize, char* errorBuffer)
{
    sqlite3_snprintf(bufSize, errorBuffer, "Dynamic loading not supported");
}

void(*chromiumDlSym(sqlite3_vfs*, void*, const char*))(void)
{
    return 0;
}

void chromiumDlClose(sqlite3_vfs*, void*)
{
}

int chromiumRandomness(sqlite3_vfs* vfs, int bufSize, char* buffer)
{
    sqlite3_vfs* wrappedVfs = static_cast<sqlite3_vfs*>(vfs->pAppData);
    return wrappedVfs->xRandomness(wrappedVfs, bufSize, buffer);
}

int chromiumSleep(sqlite3_vfs* vfs, int microseconds)
{
    sqlite3_vfs* wrappedVfs = static_cast<sqlite3_vfs*>(vfs->pAppData);
    return wrappedVfs->xSleep(wrappedVfs, microseconds);
}

int chromiumCurrentTime(sqlite3_vfs* vfs, double* prNow)
{
    sqlite3_vfs* wrappedVfs = static_cast<sqlite3_vfs*>(vfs->pAppData);
    return wrappedVfs->xCurrentTime(wrappedVfs, prNow);
}

int chromiumGetLastError(sqlite3_vfs* vfs, int e, char* s)
{
    sqlite3_vfs* wrappedVfs = static_cast<sqlite3_vfs*>(vfs->pAppData);
    return wrappedVfs->xGetLastError(wrappedVfs, e, s);
}

} // namespace

void SQLiteFileSystem::registerSQLiteVFS()
{
    sqlite3_vfs* wrappedVfs = sqlite3_vfs_find("unix");
    static sqlite3_vfs chromium_vfs = {
        1,
        sizeof(chromiumVfsFile),
        wrappedVfs->mxPathname,
        0,
        "chromium_vfs",
        wrappedVfs,
        chromiumOpen,
        chromiumDelete,
        chromiumAccess,
        chromiumFullPathname,
        chromiumDlOpen,
        chromiumDlError,
        chromiumDlSym,
        chromiumDlClose,
        chromiumRandomness,
        chromiumSleep,
        chromiumCurrentTime,
        chromiumGetLastError
    };
    sqlite3_vfs_register(&chromium_vfs, 0);
}

} // namespace blink
