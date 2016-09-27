/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "public/platform/WebFileSystemCallbacks.h"

#include "platform/AsyncFileSystemCallbacks.h"
#include "platform/FileMetadata.h"
#include "public/platform/WebFileInfo.h"
#include "public/platform/WebFileSystem.h"
#include "public/platform/WebFileSystemEntry.h"
#include "public/platform/WebFileWriter.h"
#include "public/platform/WebString.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class WebFileSystemCallbacksPrivate : public RefCounted<WebFileSystemCallbacksPrivate> {
public:
    static PassRefPtr<WebFileSystemCallbacksPrivate> create(const PassOwnPtr<AsyncFileSystemCallbacks>& callbacks)
    {
        return adoptRef(new WebFileSystemCallbacksPrivate(callbacks));
    }

    AsyncFileSystemCallbacks* callbacks() { return m_callbacks.get(); }

private:
    WebFileSystemCallbacksPrivate(const PassOwnPtr<AsyncFileSystemCallbacks>& callbacks) : m_callbacks(callbacks) { }
    OwnPtr<AsyncFileSystemCallbacks> m_callbacks;
};

WebFileSystemCallbacks::WebFileSystemCallbacks(const PassOwnPtr<AsyncFileSystemCallbacks>& callbacks)
{
    m_private = WebFileSystemCallbacksPrivate::create(callbacks);
}

void WebFileSystemCallbacks::reset()
{
    m_private.reset();
}

void WebFileSystemCallbacks::assign(const WebFileSystemCallbacks& other)
{
    m_private = other.m_private;
}

void WebFileSystemCallbacks::didSucceed()
{
    ASSERT(!m_private.isNull());
    m_private->callbacks()->didSucceed();
    m_private.reset();
}

void WebFileSystemCallbacks::didReadMetadata(const WebFileInfo& webFileInfo)
{
    ASSERT(!m_private.isNull());
    FileMetadata fileMetadata;
    fileMetadata.modificationTime = webFileInfo.modificationTime;
    fileMetadata.length = webFileInfo.length;
    fileMetadata.type = static_cast<FileMetadata::Type>(webFileInfo.type);
    fileMetadata.platformPath = webFileInfo.platformPath;
    m_private->callbacks()->didReadMetadata(fileMetadata);
    m_private.reset();
}

void WebFileSystemCallbacks::didCreateSnapshotFile(const WebFileInfo& webFileInfo)
{
    ASSERT(!m_private.isNull());
    // It's important to create a BlobDataHandle that refers to the platform file path prior
    // to return from this method so the underlying file will not be deleted.
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->appendFile(webFileInfo.platformPath);
    RefPtr<BlobDataHandle> snapshotBlob = BlobDataHandle::create(blobData.release(), webFileInfo.length);

    FileMetadata fileMetadata;
    fileMetadata.modificationTime = webFileInfo.modificationTime;
    fileMetadata.length = webFileInfo.length;
    fileMetadata.type = static_cast<FileMetadata::Type>(webFileInfo.type);
    fileMetadata.platformPath = webFileInfo.platformPath;
    m_private->callbacks()->didCreateSnapshotFile(fileMetadata, snapshotBlob);
    m_private.reset();
}

void WebFileSystemCallbacks::didReadDirectory(const WebVector<WebFileSystemEntry>& entries, bool hasMore)
{
    ASSERT(!m_private.isNull());
    for (size_t i = 0; i < entries.size(); ++i)
        m_private->callbacks()->didReadDirectoryEntry(entries[i].name, entries[i].isDirectory);
    m_private->callbacks()->didReadDirectoryEntries(hasMore);
    m_private.reset();
}

void WebFileSystemCallbacks::didOpenFileSystem(const WebString& name, const WebURL& rootURL)
{
    ASSERT(!m_private.isNull());
    m_private->callbacks()->didOpenFileSystem(name, rootURL);
    m_private.reset();
}

void WebFileSystemCallbacks::didResolveURL(const WebString& name, const WebURL& rootURL, WebFileSystemType type, const WebString& filePath, bool isDirectory)
{
    ASSERT(!m_private.isNull());
    m_private->callbacks()->didResolveURL(name, rootURL, static_cast<FileSystemType>(type), filePath, isDirectory);
    m_private.reset();
}

void WebFileSystemCallbacks::didCreateFileWriter(WebFileWriter* webFileWriter, long long length)
{
    ASSERT(!m_private.isNull());
    m_private->callbacks()->didCreateFileWriter(adoptPtr(webFileWriter), length);
    m_private.reset();
}

void WebFileSystemCallbacks::didFail(WebFileError error)
{
    ASSERT(!m_private.isNull());
    m_private->callbacks()->didFail(error);
    m_private.reset();
}

bool WebFileSystemCallbacks::shouldBlockUntilCompletion() const
{
    ASSERT(!m_private.isNull());
    return m_private->callbacks()->shouldBlockUntilCompletion();
}

} // namespace blink
