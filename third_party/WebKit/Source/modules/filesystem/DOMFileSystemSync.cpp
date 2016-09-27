/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "modules/filesystem/DOMFileSystemSync.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/fileapi/File.h"
#include "core/fileapi/FileError.h"
#include "modules/filesystem/DOMFilePath.h"
#include "modules/filesystem/DirectoryEntrySync.h"
#include "modules/filesystem/ErrorCallback.h"
#include "modules/filesystem/FileEntrySync.h"
#include "modules/filesystem/FileSystemCallbacks.h"
#include "modules/filesystem/FileWriterBaseCallback.h"
#include "modules/filesystem/FileWriterSync.h"
#include "platform/FileMetadata.h"
#include "public/platform/WebFileSystem.h"
#include "public/platform/WebFileSystemCallbacks.h"

namespace blink {

class FileWriterBase;

DOMFileSystemSync* DOMFileSystemSync::create(DOMFileSystemBase* fileSystem)
{
    return new DOMFileSystemSync(fileSystem->m_context, fileSystem->name(), fileSystem->type(), fileSystem->rootURL());
}

DOMFileSystemSync::DOMFileSystemSync(ExecutionContext* context, const String& name, FileSystemType type, const KURL& rootURL)
    : DOMFileSystemBase(context, name, type, rootURL)
    , m_rootEntry(DirectoryEntrySync::create(this, DOMFilePath::root))
{
}

DOMFileSystemSync::~DOMFileSystemSync()
{
}

void DOMFileSystemSync::reportError(ErrorCallback* errorCallback, FileError* fileError)
{
    errorCallback->handleEvent(fileError);
}

DirectoryEntrySync* DOMFileSystemSync::root()
{
    return m_rootEntry.get();
}

namespace {

class CreateFileHelper final : public AsyncFileSystemCallbacks {
public:
    class CreateFileResult : public GarbageCollectedFinalized<CreateFileResult> {
      public:
        static CreateFileResult* create()
        {
            return new CreateFileResult();
        }

        bool m_failed;
        int m_code;
        Member<File> m_file;

        DEFINE_INLINE_TRACE()
        {
            visitor->trace(m_file);
        }

      private:
        CreateFileResult()
            : m_failed(false)
            , m_code(0)
        {
        }
    };

    static PassOwnPtr<AsyncFileSystemCallbacks> create(CreateFileResult* result, const String& name, const KURL& url, FileSystemType type)
    {
        return adoptPtr(static_cast<AsyncFileSystemCallbacks*>(new CreateFileHelper(result, name, url, type)));
    }

    void didFail(int code) override
    {
        m_result->m_failed = true;
        m_result->m_code = code;
    }

    ~CreateFileHelper() override
    {
    }

    void didCreateSnapshotFile(const FileMetadata& metadata, PassRefPtr<BlobDataHandle> snapshot) override
    {
        // We can't directly use the snapshot blob data handle because the content type on it hasn't been set.
        // The |snapshot| param is here to provide a a chain of custody thru thread bridging that is held onto until
        // *after* we've coined a File with a new handle that has the correct type set on it. This allows the
        // blob storage system to track when a temp file can and can't be safely deleted.

        m_result->m_file = DOMFileSystemBase::createFile(metadata, m_url, m_type, m_name);
    }

    bool shouldBlockUntilCompletion() const override
    {
        return true;
    }

private:
    CreateFileHelper(CreateFileResult* result, const String& name, const KURL& url, FileSystemType type)
        : m_result(result)
        , m_name(name)
        , m_url(url)
        , m_type(type)
    {
    }

    Persistent<CreateFileResult> m_result;
    String m_name;
    KURL m_url;
    FileSystemType m_type;
};

} // namespace

File* DOMFileSystemSync::createFile(const FileEntrySync* fileEntry, ExceptionState& exceptionState)
{
    KURL fileSystemURL = createFileSystemURL(fileEntry);
    CreateFileHelper::CreateFileResult* result(CreateFileHelper::CreateFileResult::create());
    fileSystem()->createSnapshotFileAndReadMetadata(fileSystemURL, CreateFileHelper::create(result, fileEntry->name(), fileSystemURL, type()));
    if (result->m_failed) {
        exceptionState.throwDOMException(result->m_code, "Could not create '" + fileEntry->name() + "'.");
        return nullptr;
    }
    return result->m_file.get();
}

namespace {

class ReceiveFileWriterCallback final : public FileWriterBaseCallback {
public:
    static ReceiveFileWriterCallback* create()
    {
        return new ReceiveFileWriterCallback();
    }

    void handleEvent(FileWriterBase*) override
    {
    }

private:
    ReceiveFileWriterCallback()
    {
    }
};

class LocalErrorCallback final : public ErrorCallback {
public:
    static LocalErrorCallback* create(FileError::ErrorCode& errorCode)
    {
        return new LocalErrorCallback(errorCode);
    }

    void handleEvent(FileError* error) override
    {
        ASSERT(error->code() != FileError::OK);
        m_errorCode = error->code();
    }

private:
    explicit LocalErrorCallback(FileError::ErrorCode& errorCode)
        : m_errorCode(errorCode)
    {
    }

    FileError::ErrorCode& m_errorCode;
};

}

FileWriterSync* DOMFileSystemSync::createWriter(const FileEntrySync* fileEntry, ExceptionState& exceptionState)
{
    ASSERT(fileEntry);

    FileWriterSync* fileWriter = FileWriterSync::create();
    ReceiveFileWriterCallback* successCallback = ReceiveFileWriterCallback::create();
    FileError::ErrorCode errorCode = FileError::OK;
    LocalErrorCallback* errorCallback = LocalErrorCallback::create(errorCode);

    OwnPtr<AsyncFileSystemCallbacks> callbacks = FileWriterBaseCallbacks::create(fileWriter, successCallback, errorCallback, m_context);
    callbacks->setShouldBlockUntilCompletion(true);

    fileSystem()->createFileWriter(createFileSystemURL(fileEntry), fileWriter, callbacks.release());
    if (errorCode != FileError::OK) {
        FileError::throwDOMException(exceptionState, errorCode);
        return 0;
    }
    return fileWriter;
}

DEFINE_TRACE(DOMFileSystemSync)
{
    DOMFileSystemBase::trace(visitor);
    visitor->trace(m_rootEntry);
}

}
