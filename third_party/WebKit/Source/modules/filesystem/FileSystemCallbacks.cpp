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
#include "modules/filesystem/FileSystemCallbacks.h"

#include "core/dom/ExecutionContext.h"
#include "core/fileapi/File.h"
#include "core/fileapi/FileError.h"
#include "core/html/VoidCallback.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "modules/filesystem/DOMFilePath.h"
#include "modules/filesystem/DOMFileSystem.h"
#include "modules/filesystem/DOMFileSystemBase.h"
#include "modules/filesystem/DirectoryEntry.h"
#include "modules/filesystem/DirectoryReader.h"
#include "modules/filesystem/Entry.h"
#include "modules/filesystem/EntryCallback.h"
#include "modules/filesystem/ErrorCallback.h"
#include "modules/filesystem/FileCallback.h"
#include "modules/filesystem/FileEntry.h"
#include "modules/filesystem/FileSystemCallback.h"
#include "modules/filesystem/FileWriterBase.h"
#include "modules/filesystem/FileWriterBaseCallback.h"
#include "modules/filesystem/Metadata.h"
#include "modules/filesystem/MetadataCallback.h"
#include "platform/FileMetadata.h"
#include "public/platform/WebFileWriter.h"

namespace blink {

FileSystemCallbacksBase::FileSystemCallbacksBase(ErrorCallback* errorCallback, DOMFileSystemBase* fileSystem, ExecutionContext* context)
    : m_errorCallback(errorCallback)
    , m_fileSystem(fileSystem)
    , m_executionContext(context)
    , m_asyncOperationId(0)
{
    if (m_fileSystem)
        m_fileSystem->addPendingCallbacks();
    if (m_executionContext)
        m_asyncOperationId = InspectorInstrumentation::traceAsyncOperationStarting(m_executionContext.get(), "FileSystem");
}

FileSystemCallbacksBase::~FileSystemCallbacksBase()
{
    if (m_fileSystem)
        m_fileSystem->removePendingCallbacks();
    if (m_asyncOperationId && m_executionContext)
        InspectorInstrumentation::traceAsyncOperationCompleted(m_executionContext.get(), m_asyncOperationId);
}

void FileSystemCallbacksBase::didFail(int code)
{
    if (m_errorCallback)
        handleEventOrScheduleCallback(m_errorCallback.release(), FileError::create(static_cast<FileError::ErrorCode>(code)));
}

bool FileSystemCallbacksBase::shouldScheduleCallback() const
{
    return !shouldBlockUntilCompletion() && m_executionContext && m_executionContext->activeDOMObjectsAreSuspended();
}

template <typename CB, typename CBArg>
void FileSystemCallbacksBase::handleEventOrScheduleCallback(RawPtr<CB> callback, CBArg* arg)
{
    ASSERT(callback);
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncOperationCompletedCallbackStarting(m_executionContext.get(), m_asyncOperationId);
    if (shouldScheduleCallback())
        DOMFileSystem::scheduleCallback(m_executionContext.get(), callback.get(), arg);
    else if (callback)
        callback->handleEvent(arg);
    m_executionContext.clear();
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

template <typename CB>
void FileSystemCallbacksBase::handleEventOrScheduleCallback(RawPtr<CB> callback)
{
    ASSERT(callback);
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncOperationCompletedCallbackStarting(m_executionContext.get(), m_asyncOperationId);
    if (shouldScheduleCallback())
        DOMFileSystem::scheduleCallback(m_executionContext.get(), callback.get());
    else if (callback)
        callback->handleEvent();
    m_executionContext.clear();
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

// EntryCallbacks -------------------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> EntryCallbacks::create(EntryCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DOMFileSystemBase* fileSystem, const String& expectedPath, bool isDirectory)
{
    return adoptPtr(new EntryCallbacks(successCallback, errorCallback, context, fileSystem, expectedPath, isDirectory));
}

EntryCallbacks::EntryCallbacks(EntryCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DOMFileSystemBase* fileSystem, const String& expectedPath, bool isDirectory)
    : FileSystemCallbacksBase(errorCallback, fileSystem, context)
    , m_successCallback(successCallback)
    , m_expectedPath(expectedPath)
    , m_isDirectory(isDirectory)
{
}

void EntryCallbacks::didSucceed()
{
    if (m_successCallback) {
        if (m_isDirectory)
            handleEventOrScheduleCallback(m_successCallback.release(), DirectoryEntry::create(m_fileSystem, m_expectedPath));
        else
            handleEventOrScheduleCallback(m_successCallback.release(), FileEntry::create(m_fileSystem, m_expectedPath));
    }
}

// EntriesCallbacks -----------------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> EntriesCallbacks::create(EntriesCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DirectoryReaderBase* directoryReader, const String& basePath)
{
    return adoptPtr(new EntriesCallbacks(successCallback, errorCallback, context, directoryReader, basePath));
}

EntriesCallbacks::EntriesCallbacks(EntriesCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DirectoryReaderBase* directoryReader, const String& basePath)
    : FileSystemCallbacksBase(errorCallback, directoryReader->filesystem(), context)
    , m_successCallback(successCallback)
    , m_directoryReader(directoryReader)
    , m_basePath(basePath)
{
    ASSERT(m_directoryReader);
}

void EntriesCallbacks::didReadDirectoryEntry(const String& name, bool isDirectory)
{
    if (isDirectory)
        m_entries.append(DirectoryEntry::create(m_directoryReader->filesystem(), DOMFilePath::append(m_basePath, name)));
    else
        m_entries.append(FileEntry::create(m_directoryReader->filesystem(), DOMFilePath::append(m_basePath, name)));
}

void EntriesCallbacks::didReadDirectoryEntries(bool hasMore)
{
    m_directoryReader->setHasMoreEntries(hasMore);
    EntryHeapVector entries;
    entries.swap(m_entries);
    // FIXME: delay the callback iff shouldScheduleCallback() is true.
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_executionContext.get(), m_asyncOperationId);
    if (m_successCallback)
        m_successCallback->handleEvent(entries);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
    if (!hasMore)
        InspectorInstrumentation::traceAsyncOperationCompleted(m_executionContext.get(), m_asyncOperationId);
}

// FileSystemCallbacks --------------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> FileSystemCallbacks::create(FileSystemCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, FileSystemType type)
{
    return adoptPtr(new FileSystemCallbacks(successCallback, errorCallback, context, type));
}

FileSystemCallbacks::FileSystemCallbacks(FileSystemCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, FileSystemType type)
    : FileSystemCallbacksBase(errorCallback, nullptr, context)
    , m_successCallback(successCallback)
    , m_type(type)
{
}

void FileSystemCallbacks::didOpenFileSystem(const String& name, const KURL& rootURL)
{
    if (m_successCallback)
        handleEventOrScheduleCallback(m_successCallback.release(), DOMFileSystem::create(m_executionContext.get(), name, m_type, rootURL));
}

// ResolveURICallbacks --------------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> ResolveURICallbacks::create(EntryCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context)
{
    return adoptPtr(new ResolveURICallbacks(successCallback, errorCallback, context));
}

ResolveURICallbacks::ResolveURICallbacks(EntryCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context)
    : FileSystemCallbacksBase(errorCallback, nullptr, context)
    , m_successCallback(successCallback)
{
}

void ResolveURICallbacks::didResolveURL(const String& name, const KURL& rootURL, FileSystemType type, const String& filePath, bool isDirectory)
{
    DOMFileSystem* filesystem = DOMFileSystem::create(m_executionContext.get(), name, type, rootURL);
    DirectoryEntry* root = filesystem->root();

    String absolutePath;
    if (!DOMFileSystemBase::pathToAbsolutePath(type, root, filePath, absolutePath)) {
        handleEventOrScheduleCallback(m_errorCallback.release(), FileError::create(FileError::INVALID_MODIFICATION_ERR));
        return;
    }

    if (isDirectory)
        handleEventOrScheduleCallback(m_successCallback.release(), DirectoryEntry::create(filesystem, absolutePath));
    else
        handleEventOrScheduleCallback(m_successCallback.release(), FileEntry::create(filesystem, absolutePath));
}

// MetadataCallbacks ----------------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> MetadataCallbacks::create(MetadataCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DOMFileSystemBase* fileSystem)
{
    return adoptPtr(new MetadataCallbacks(successCallback, errorCallback, context, fileSystem));
}

MetadataCallbacks::MetadataCallbacks(MetadataCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DOMFileSystemBase* fileSystem)
    : FileSystemCallbacksBase(errorCallback, fileSystem, context)
    , m_successCallback(successCallback)
{
}

void MetadataCallbacks::didReadMetadata(const FileMetadata& metadata)
{
    if (m_successCallback)
        handleEventOrScheduleCallback(m_successCallback.release(), Metadata::create(metadata));
}

// FileWriterBaseCallbacks ----------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> FileWriterBaseCallbacks::create(FileWriterBase* fileWriter, FileWriterBaseCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context)
{
    return adoptPtr(new FileWriterBaseCallbacks(fileWriter, successCallback, errorCallback, context));
}

FileWriterBaseCallbacks::FileWriterBaseCallbacks(FileWriterBase* fileWriter, FileWriterBaseCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context)
    : FileSystemCallbacksBase(errorCallback, nullptr, context)
    , m_fileWriter(fileWriter)
    , m_successCallback(successCallback)
{
}

void FileWriterBaseCallbacks::didCreateFileWriter(PassOwnPtr<WebFileWriter> fileWriter, long long length)
{
    m_fileWriter->initialize(fileWriter, length);
    if (m_successCallback)
        handleEventOrScheduleCallback(m_successCallback.release(), m_fileWriter.release().get());
}

// SnapshotFileCallback -------------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> SnapshotFileCallback::create(DOMFileSystemBase* filesystem, const String& name, const KURL& url, FileCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context)
{
    return adoptPtr(new SnapshotFileCallback(filesystem, name, url, successCallback, errorCallback, context));
}

SnapshotFileCallback::SnapshotFileCallback(DOMFileSystemBase* filesystem, const String& name, const KURL& url, FileCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context)
    : FileSystemCallbacksBase(errorCallback, filesystem, context)
    , m_name(name)
    , m_url(url)
    , m_successCallback(successCallback)
{
}

void SnapshotFileCallback::didCreateSnapshotFile(const FileMetadata& metadata, PassRefPtr<BlobDataHandle> snapshot)
{
    if (!m_successCallback)
        return;

    // We can't directly use the snapshot blob data handle because the content type on it hasn't been set.
    // The |snapshot| param is here to provide a a chain of custody thru thread bridging that is held onto until
    // *after* we've coined a File with a new handle that has the correct type set on it. This allows the
    // blob storage system to track when a temp file can and can't be safely deleted.

    handleEventOrScheduleCallback(m_successCallback.release(), DOMFileSystemBase::createFile(metadata, m_url, m_fileSystem->type(), m_name));
}

// VoidCallbacks --------------------------------------------------------------

PassOwnPtr<AsyncFileSystemCallbacks> VoidCallbacks::create(VoidCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DOMFileSystemBase* fileSystem)
{
    return adoptPtr(new VoidCallbacks(successCallback, errorCallback, context, fileSystem));
}

VoidCallbacks::VoidCallbacks(VoidCallback* successCallback, ErrorCallback* errorCallback, ExecutionContext* context, DOMFileSystemBase* fileSystem)
    : FileSystemCallbacksBase(errorCallback, fileSystem, context)
    , m_successCallback(successCallback)
{
}

void VoidCallbacks::didSucceed()
{
    if (m_successCallback)
        handleEventOrScheduleCallback(m_successCallback.release());
}

} // namespace blink
