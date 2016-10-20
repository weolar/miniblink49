// Copyright (c) 2008 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/browser/CefStreamImpl.h"
#include <stdlib.h>
// #include "base/files/file_util.h"
// #include "base/logging.h"
// #include "base/threading/thread_restrictions.h"

// Static functions

CefRefPtr<CefStreamReader> CefStreamReader::CreateForFile(const CefString& fileName) {
    ASSERT(!fileName.empty());

//     // TODO(cef): Do not allow file IO on all threads (issue #1187).
//     base::ThreadRestrictions::ScopedAllowIO allow_io;
// 
//     CefRefPtr<CefStreamReader> reader;
//     FILE* file = base::OpenFile(base::FilePath(fileName), "rb");
//     if (file)
//         reader = new CefFileReader(file, true);
//     return reader;
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefStreamReader> CefStreamReader::CreateForData(void* data, size_t size) {
    ASSERT(data != NULL);
    ASSERT(size > 0);  // NOLINT(readability/check)
//     CefRefPtr<CefStreamReader> reader;
//     if (data && size > 0)
//         reader = new CefBytesReader(data, size, true);
//     return reader;
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefStreamReader> CefStreamReader::CreateForHandler(
    CefRefPtr<CefReadHandler> handler) {
//     ASSERT(handler.get());
//     CefRefPtr<CefStreamReader> reader;
//     if (handler.get())
//         reader = new CefHandlerReader(handler);
//     return reader;
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefStreamWriter> CefStreamWriter::CreateForFile(
    const CefString& fileName) {
    ASSERT(!fileName.empty());

    // TODO(cef): Do not allow file IO on all threads (issue #1187).
    //   base::ThreadRestrictions::ScopedAllowIO allow_io;
    // 
    //   CefRefPtr<CefStreamWriter> writer;
    //   FILE* file = base::OpenFile(base::FilePath(fileName), "wb");
    //   if (file)
    //     writer = new CefFileWriter(file, true);
    //   return writer;
    DebugBreak();
    return nullptr;

}

CefRefPtr<CefStreamWriter> CefStreamWriter::CreateForHandler(CefRefPtr<CefWriteHandler> handler) {
//     ASSERT(handler.get());
//     CefRefPtr<CefStreamWriter> writer;
//     if (handler.get())
//         writer = new CefHandlerWriter(handler);
//     return writer;
    DebugBreak();
    return nullptr;
}


// CefFileReader

CefFileReader::CefFileReader(FILE* file, bool close)
    : close_(close), file_(file) {
    DebugBreak();
}

CefFileReader::~CefFileReader() {
    base::AutoLock lock_scope(lock_);
//     if (close_)
//         base::CloseFile(file_);
    DebugBreak();
}

size_t CefFileReader::Read(void* ptr, size_t size, size_t n) {
    base::AutoLock lock_scope(lock_);
    return fread(ptr, size, n, file_);
}

int CefFileReader::Seek(int64 offset, int whence) {
//     base::AutoLock lock_scope(lock_);
// #if defined(OS_WIN)
//     return _fseeki64(file_, offset, whence);
// #else
//     return fseek(file_, offset, whence);
// #endif
    DebugBreak();
    return 0;
}

int64 CefFileReader::Tell() {
     base::AutoLock lock_scope(lock_);
// #if defined(OS_WIN)
//     return _ftelli64(file_);
// #else
//     return ftell(file_);
// #endif
     DebugBreak();
     return 0;
}

int CefFileReader::Eof() {
//     base::AutoLock lock_scope(lock_);
//     return feof(file_);
    DebugBreak();
    return 0;
}


// CefFileWriter

CefFileWriter::CefFileWriter(FILE* file, bool close)
    : file_(file),
    close_(close) {
    DebugBreak();
}

CefFileWriter::~CefFileWriter() {
    base::AutoLock lock_scope(lock_);
//     if (close_)
//         base::CloseFile(file_);
    DebugBreak();
}

size_t CefFileWriter::Write(const void* ptr, size_t size, size_t n) {
    base::AutoLock lock_scope(lock_);
    return (size_t)fwrite(ptr, size, n, file_);
}

int CefFileWriter::Seek(int64 offset, int whence) {
    base::AutoLock lock_scope(lock_);
    return fseek(file_, offset, whence);
}

int64 CefFileWriter::Tell() {
    base::AutoLock lock_scope(lock_);
    return ftell(file_);
}

int CefFileWriter::Flush() {
    base::AutoLock lock_scope(lock_);
    return fflush(file_);
}


// CefBytesReader

CefBytesReader::CefBytesReader(void* data, int64 datasize, bool copy)
    : data_(NULL),
    datasize_(0),
    copy_(false),
    offset_(0) {
    SetData(data, datasize, copy);
}

CefBytesReader::~CefBytesReader() {
    SetData(NULL, 0, false);
}

size_t CefBytesReader::Read(void* ptr, size_t size, size_t n) {
    base::AutoLock lock_scope(lock_);
    size_t s = (datasize_ - offset_) / size;
    size_t ret = (n < s ? n : s);
    memcpy(ptr, (reinterpret_cast<char*>(data_)) + offset_, ret * size);
    offset_ += ret * size;
    return ret;
}

int CefBytesReader::Seek(int64 offset, int whence) {
    int rv = -1L;
    base::AutoLock lock_scope(lock_);
    switch (whence) {
    case SEEK_CUR:
        if (offset_ + offset > datasize_ || offset_ + offset < 0)
            break;
        offset_ += offset;
        rv = 0;
        break;
    case SEEK_END: {
        int64 offset_abs = std::abs(offset);
        if (offset_abs > datasize_)
            break;
        offset_ = datasize_ - offset_abs;
        rv = 0;
        break;
    }
    case SEEK_SET:
        if (offset > datasize_ || offset < 0)
            break;
        offset_ = offset;
        rv = 0;
        break;
    }

    return rv;
}

int64 CefBytesReader::Tell() {
    base::AutoLock lock_scope(lock_);
    return offset_;
}

int CefBytesReader::Eof() {
    base::AutoLock lock_scope(lock_);
    return (offset_ >= datasize_);
}

void CefBytesReader::SetData(void* data, int64 datasize, bool copy) {
    base::AutoLock lock_scope(lock_);
    if (copy_)
        free(data_);

    copy_ = copy;
    offset_ = 0;
    datasize_ = datasize;

    if (copy) {
        data_ = malloc(datasize);
        ASSERT(data_ != NULL);
        if (data_)
            memcpy(data_, data, datasize);
    }
    else {
        data_ = data;
    }
}


// CefBytesWriter

CefBytesWriter::CefBytesWriter(size_t grow)
    : grow_(grow),
    datasize_(grow),
    offset_(0) {
    ASSERT(grow > 0);  // NOLINT(readability/check)
    data_ = malloc(grow);
    ASSERT(data_ != NULL);
}

CefBytesWriter::~CefBytesWriter() {
    base::AutoLock lock_scope(lock_);
    if (data_)
        free(data_);
}

size_t CefBytesWriter::Write(const void* ptr, size_t size, size_t n) {
    base::AutoLock lock_scope(lock_);
    size_t rv;
    if (offset_ + static_cast<int64>(size * n) >= datasize_ &&
        Grow(size * n) == 0) {
        rv = 0;
    }
    else {
        memcpy(reinterpret_cast<char*>(data_) + offset_, ptr, size * n);
        offset_ += size * n;
        rv = n;
    }

    return rv;
}

int CefBytesWriter::Seek(int64 offset, int whence) {
    int rv = -1L;
    base::AutoLock lock_scope(lock_);
    switch (whence) {
    case SEEK_CUR:
        if (offset_ + offset > datasize_ || offset_ + offset < 0)
            break;
        offset_ += offset;
        rv = 0;
        break;
    case SEEK_END: {
        int64 offset_abs = std::abs(offset);
        if (offset_abs > datasize_)
            break;
        offset_ = datasize_ - offset_abs;
        rv = 0;
        break;
    }
    case SEEK_SET:
        if (offset > datasize_ || offset < 0)
            break;
        offset_ = offset;
        rv = 0;
        break;
    }

    return rv;
}

int64 CefBytesWriter::Tell() {
    base::AutoLock lock_scope(lock_);
    return offset_;
}

int CefBytesWriter::Flush() {
    return 0;
}

std::string CefBytesWriter::GetDataString() {
    base::AutoLock lock_scope(lock_);
    std::string str(reinterpret_cast<char*>(data_), offset_);
    return str;
}

size_t CefBytesWriter::Grow(size_t size) {
    base::AutoLock lock_scope(lock_);
    size_t rv;
    size_t s = (size > grow_ ? size : grow_);
    void* tmp = realloc(data_, datasize_ + s);
    ASSERT(tmp != NULL);
    if (tmp) {
        data_ = tmp;
        datasize_ += s;
        rv = datasize_;
    }
    else {
        rv = 0;
    }

    return rv;
}
#endif
