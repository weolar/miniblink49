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
#include "modules/filesystem/DOMFileSystem.h"

#include "modules/filesystem/DOMFilePath.h"
#include "modules/filesystem/DirectoryEntry.h"
#include "modules/filesystem/ErrorCallback.h"
#include "modules/filesystem/FileCallback.h"
#include "modules/filesystem/FileEntry.h"
#include "modules/filesystem/FileSystemCallbacks.h"
#include "modules/filesystem/FileWriter.h"
#include "modules/filesystem/FileWriterBaseCallback.h"
#include "modules/filesystem/FileWriterCallback.h"
#include "modules/filesystem/MetadataCallback.h"
#include "platform/FileMetadata.h"
#include "platform/weborigin/DatabaseIdentifier.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/WebFileSystem.h"
#include "public/platform/WebFileSystemCallbacks.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

// static
DOMFileSystem* DOMFileSystem::create(ExecutionContext* context, const String& name, FileSystemType type, const KURL& rootURL)
{
    DOMFileSystem* fileSystem(new DOMFileSystem(context, name, type, rootURL));
    fileSystem->suspendIfNeeded();
    return fileSystem;
}

DOMFileSystem* DOMFileSystem::createIsolatedFileSystem(ExecutionContext* context, const String& filesystemId)
{
    if (filesystemId.isEmpty())
        return 0;

    StringBuilder filesystemName;
    filesystemName.append(createDatabaseIdentifierFromSecurityOrigin(context->securityOrigin()));
    filesystemName.appendLiteral(":Isolated_");
    filesystemName.append(filesystemId);

    // The rootURL created here is going to be attached to each filesystem request and
    // is to be validated each time the request is being handled.
    StringBuilder rootURL;
    rootURL.appendLiteral("filesystem:");
    rootURL.append(context->securityOrigin()->toString());
    rootURL.append('/');
    rootURL.append(isolatedPathPrefix);
    rootURL.append('/');
    rootURL.append(filesystemId);
    rootURL.append('/');

    return DOMFileSystem::create(context, filesystemName.toString(), FileSystemTypeIsolated, KURL(ParsedURLString, rootURL.toString()));
}

DOMFileSystem::DOMFileSystem(ExecutionContext* context, const String& name, FileSystemType type, const KURL& rootURL)
    : DOMFileSystemBase(context, name, type, rootURL)
    , ActiveDOMObject(context)
    , m_numberOfPendingCallbacks(0)
    , m_rootEntry(DirectoryEntry::create(this, DOMFilePath::root))
{
}

DirectoryEntry* DOMFileSystem::root() const
{
    return m_rootEntry.get();
}

void DOMFileSystem::addPendingCallbacks()
{
    ++m_numberOfPendingCallbacks;
}

void DOMFileSystem::removePendingCallbacks()
{
    ASSERT(m_numberOfPendingCallbacks > 0);
    --m_numberOfPendingCallbacks;
}

bool DOMFileSystem::hasPendingActivity() const
{
    ASSERT(m_numberOfPendingCallbacks >= 0);
    return m_numberOfPendingCallbacks;
}

void DOMFileSystem::reportError(ErrorCallback* errorCallback, FileError* fileError)
{
    scheduleCallback(errorCallback, fileError);
}

namespace {

class ConvertToFileWriterCallback : public FileWriterBaseCallback {
public:
    static ConvertToFileWriterCallback* create(FileWriterCallback* callback)
    {
        return new ConvertToFileWriterCallback(callback);
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_callback);
        FileWriterBaseCallback::trace(visitor);
    }

    void handleEvent(FileWriterBase* fileWriterBase)
    {
        m_callback->handleEvent(static_cast<FileWriter*>(fileWriterBase));
    }
private:
    explicit ConvertToFileWriterCallback(FileWriterCallback* callback)
        : m_callback(callback)
    {
    }
    Member<FileWriterCallback> m_callback;
};

}

void DOMFileSystem::createWriter(const FileEntry* fileEntry, FileWriterCallback* successCallback, ErrorCallback* errorCallback)
{
    ASSERT(fileEntry);

    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    FileWriter* fileWriter = FileWriter::create(executionContext());
    FileWriterBaseCallback* conversionCallback = ConvertToFileWriterCallback::create(successCallback);
    OwnPtr<AsyncFileSystemCallbacks> callbacks = FileWriterBaseCallbacks::create(fileWriter, conversionCallback, errorCallback, m_context);
    fileSystem()->createFileWriter(createFileSystemURL(fileEntry), fileWriter, callbacks.release());
}

void DOMFileSystem::createFile(const FileEntry* fileEntry, FileCallback* successCallback, ErrorCallback* errorCallback)
{
    KURL fileSystemURL = createFileSystemURL(fileEntry);
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    fileSystem()->createSnapshotFileAndReadMetadata(fileSystemURL, SnapshotFileCallback::create(this, fileEntry->name(), fileSystemURL, successCallback, errorCallback, m_context));
}

DEFINE_TRACE(DOMFileSystem)
{
    DOMFileSystemBase::trace(visitor);
    ActiveDOMObject::trace(visitor);
    visitor->trace(m_rootEntry);
}

} // namespace blink
