/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/fileapi/File.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/fileapi/FilePropertyBag.h"
#include "platform/FileMetadata.h"
#include "platform/MIMETypeRegistry.h"
#include "public/platform/Platform.h"
#include "public/platform/WebFileUtilities.h"
#include "wtf/CurrentTime.h"
#include "wtf/DateMath.h"

namespace blink {

static String getContentTypeFromFileName(const String& name, File::ContentTypeLookupPolicy policy)
{
    String type;
    int index = name.reverseFind('.');
    if (index != -1) {
        if (policy == File::WellKnownContentTypes)
            type = MIMETypeRegistry::getWellKnownMIMETypeForExtension(name.substring(index + 1));
        else {
            ASSERT(policy == File::AllContentTypes);
            type = MIMETypeRegistry::getMIMETypeForExtension(name.substring(index + 1));
        }
    }
    return type;
}

static PassOwnPtr<BlobData> createBlobDataForFileWithType(const String& path, const String& contentType)
{
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(contentType);
    blobData->appendFile(path);
    return blobData.release();
}

static PassOwnPtr<BlobData> createBlobDataForFile(const String& path, File::ContentTypeLookupPolicy policy)
{
    return createBlobDataForFileWithType(path, getContentTypeFromFileName(path, policy));
}

static PassOwnPtr<BlobData> createBlobDataForFileWithName(const String& path, const String& fileSystemName, File::ContentTypeLookupPolicy policy)
{
    return createBlobDataForFileWithType(path, getContentTypeFromFileName(fileSystemName, policy));
}

static PassOwnPtr<BlobData> createBlobDataForFileWithMetadata(const String& fileSystemName, const FileMetadata& metadata)
{
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(getContentTypeFromFileName(fileSystemName, File::WellKnownContentTypes));
    blobData->appendFile(metadata.platformPath, 0, metadata.length, metadata.modificationTime / msPerSecond);
    return blobData.release();
}

static PassOwnPtr<BlobData> createBlobDataForFileSystemURL(const KURL& fileSystemURL, const FileMetadata& metadata)
{
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(getContentTypeFromFileName(fileSystemURL.path(), File::WellKnownContentTypes));
    blobData->appendFileSystemURL(fileSystemURL, 0, metadata.length, metadata.modificationTime / msPerSecond);
    return blobData.release();
}

// static
File* File::create(const HeapVector<BlobOrStringOrArrayBufferViewOrArrayBuffer>& fileBits, const String& fileName, const FilePropertyBag& options, ExceptionState& exceptionState)
{
    ASSERT(options.hasType());
    if (!options.type().containsOnlyASCII()) {
        exceptionState.throwDOMException(SyntaxError, "The 'type' property must consist of ASCII characters.");
        return nullptr;
    }

    double lastModified;
    if (options.hasLastModified())
        lastModified = static_cast<double>(options.lastModified());
    else
        lastModified = currentTimeMS();

    ASSERT(options.hasEndings());
    bool normalizeLineEndingsToNative = options.endings() == "native";

    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(options.type().lower());
    populateBlobData(blobData.get(), fileBits, normalizeLineEndingsToNative);

    long long fileSize = blobData->length();
    return File::create(fileName, lastModified, BlobDataHandle::create(blobData.release(), fileSize));
}

File* File::createWithRelativePath(const String& path, const String& relativePath)
{
    File* file = new File(path, File::AllContentTypes, File::IsUserVisible);
    file->m_relativePath = relativePath;
    return file;
}

File::File(const String& path, ContentTypeLookupPolicy policy, UserVisibility userVisibility)
    : Blob(BlobDataHandle::create(createBlobDataForFile(path, policy), -1))
    , m_hasBackingFile(true)
    , m_userVisibility(userVisibility)
    , m_path(path)
    , m_name(Platform::current()->fileUtilities()->baseName(path))
    , m_snapshotSize(-1)
    , m_snapshotModificationTimeMS(invalidFileTime())
{
}

File::File(const String& path, const String& name, ContentTypeLookupPolicy policy, UserVisibility userVisibility)
    : Blob(BlobDataHandle::create(createBlobDataForFileWithName(path, name, policy), -1))
    , m_hasBackingFile(true)
    , m_userVisibility(userVisibility)
    , m_path(path)
    , m_name(name)
    , m_snapshotSize(-1)
    , m_snapshotModificationTimeMS(invalidFileTime())
{
}

File::File(const String& path, const String& name, const String& relativePath, UserVisibility userVisibility, bool hasSnaphotData, uint64_t size, double lastModified, PassRefPtr<BlobDataHandle> blobDataHandle)
    : Blob(blobDataHandle)
    , m_hasBackingFile(!path.isEmpty() || !relativePath.isEmpty())
    , m_userVisibility(userVisibility)
    , m_path(path)
    , m_name(name)
    , m_snapshotSize(hasSnaphotData ? static_cast<long long>(size) : -1)
    , m_snapshotModificationTimeMS(hasSnaphotData ? lastModified : invalidFileTime())
    , m_relativePath(relativePath)
{
}

File::File(const String& name, double modificationTimeMS, PassRefPtr<BlobDataHandle> blobDataHandle)
    : Blob(blobDataHandle)
    , m_hasBackingFile(false)
    , m_userVisibility(File::IsNotUserVisible)
    , m_name(name)
    , m_snapshotSize(Blob::size())
    , m_snapshotModificationTimeMS(modificationTimeMS)
{
}

File::File(const String& name, const FileMetadata& metadata, UserVisibility userVisibility)
    : Blob(BlobDataHandle::create(createBlobDataForFileWithMetadata(name, metadata), metadata.length))
    , m_hasBackingFile(true)
    , m_userVisibility(userVisibility)
    , m_path(metadata.platformPath)
    , m_name(name)
    , m_snapshotSize(metadata.length)
    , m_snapshotModificationTimeMS(metadata.modificationTime)
{
}

File::File(const KURL& fileSystemURL, const FileMetadata& metadata, UserVisibility userVisibility)
    : Blob(BlobDataHandle::create(createBlobDataForFileSystemURL(fileSystemURL, metadata), metadata.length))
    , m_hasBackingFile(false)
    , m_userVisibility(userVisibility)
    , m_name(decodeURLEscapeSequences(fileSystemURL.lastPathComponent()))
    , m_fileSystemURL(fileSystemURL)
    , m_snapshotSize(metadata.length)
    , m_snapshotModificationTimeMS(metadata.modificationTime)
{
}

File::File(const File& other)
    : Blob(other.blobDataHandle())
    , m_hasBackingFile(other.m_hasBackingFile)
    , m_userVisibility(other.m_userVisibility)
    , m_path(other.m_path)
    , m_name(other.m_name)
    , m_fileSystemURL(other.m_fileSystemURL)
    , m_snapshotSize(other.m_snapshotSize)
    , m_snapshotModificationTimeMS(other.m_snapshotModificationTimeMS)
    , m_relativePath(other.m_relativePath)
{
}

File* File::clone(const String& name) const
{
    File* file = new File(*this);
    if (!name.isNull())
        file->m_name = name;
    return file;
}

double File::lastModifiedMS() const
{
    if (hasValidSnapshotMetadata() && isValidFileTime(m_snapshotModificationTimeMS))
        return m_snapshotModificationTimeMS;

    double modificationTimeMS;
    if (hasBackingFile() && getFileModificationTime(m_path, modificationTimeMS) && isValidFileTime(modificationTimeMS))
        return modificationTimeMS;

    return currentTimeMS();
}

long long File::lastModified() const
{
    double modifiedDate = lastModifiedMS();

    // The getter should return the current time when the last modification time isn't known.
    if (!isValidFileTime(modifiedDate))
        modifiedDate = currentTimeMS();

    // lastModified returns a number, not a Date instance,
    // http://dev.w3.org/2006/webapi/FileAPI/#file-attrs
    return floor(modifiedDate);
}

double File::lastModifiedDate() const
{
    double modifiedDate = lastModifiedMS();

    // The getter should return the current time when the last modification time isn't known.
    if (!isValidFileTime(modifiedDate))
        modifiedDate = currentTimeMS();

    // lastModifiedDate returns a Date instance,
    // http://www.w3.org/TR/FileAPI/#dfn-lastModifiedDate
    return modifiedDate;
}

unsigned long long File::size() const
{
    if (hasValidSnapshotMetadata())
        return m_snapshotSize;

    // FIXME: JavaScript cannot represent sizes as large as unsigned long long, we need to
    // come up with an exception to throw if file size is not representable.
    long long size;
    if (!hasBackingFile() || !getFileSize(m_path, size))
        return 0;
    return static_cast<unsigned long long>(size);
}

Blob* File::slice(long long start, long long end, const String& contentType, ExceptionState& exceptionState) const
{
    if (hasBeenClosed()) {
        exceptionState.throwDOMException(InvalidStateError, "File has been closed.");
        return nullptr;
    }

    if (!m_hasBackingFile)
        return Blob::slice(start, end, contentType, exceptionState);

    // FIXME: This involves synchronous file operation. We need to figure out how to make it asynchronous.
    long long size;
    double modificationTimeMS;
    captureSnapshot(size, modificationTimeMS);
    clampSliceOffsets(size, start, end);

    long long length = end - start;
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(contentType);
    if (!m_fileSystemURL.isEmpty()) {
        blobData->appendFileSystemURL(m_fileSystemURL, start, length, modificationTimeMS / msPerSecond);
    } else {
        ASSERT(!m_path.isEmpty());
        blobData->appendFile(m_path, start, length, modificationTimeMS / msPerSecond);
    }
    return Blob::create(BlobDataHandle::create(blobData.release(), length));
}

void File::captureSnapshot(long long& snapshotSize, double& snapshotModificationTimeMS) const
{
    if (hasValidSnapshotMetadata()) {
        snapshotSize = m_snapshotSize;
        snapshotModificationTimeMS = m_snapshotModificationTimeMS;
        return;
    }

    // Obtains a snapshot of the file by capturing its current size and modification time. This is used when we slice a file for the first time.
    // If we fail to retrieve the size or modification time, probably due to that the file has been deleted, 0 size is returned.
    FileMetadata metadata;
    if (!hasBackingFile() || !getFileMetadata(m_path, metadata)) {
        snapshotSize = 0;
        snapshotModificationTimeMS = invalidFileTime();
        return;
    }

    snapshotSize = metadata.length;
    snapshotModificationTimeMS = metadata.modificationTime;
}

void File::close(ExecutionContext* executionContext, ExceptionState& exceptionState)
{
    if (hasBeenClosed()) {
        exceptionState.throwDOMException(InvalidStateError, "Blob has been closed.");
        return;
    }

    // Reset the File to its closed representation, an empty
    // Blob. The name isn't cleared, as it should still be
    // available.
    m_hasBackingFile = false;
    m_path = String();
    m_fileSystemURL = KURL();
    invalidateSnapshotMetadata();
    m_relativePath = String();
    Blob::close(executionContext, exceptionState);
}

void File::appendTo(BlobData& blobData) const
{
    if (!m_hasBackingFile) {
        Blob::appendTo(blobData);
        return;
    }

    // FIXME: This involves synchronous file operation. We need to figure out how to make it asynchronous.
    long long size;
    double modificationTimeMS;
    captureSnapshot(size, modificationTimeMS);
    if (!m_fileSystemURL.isEmpty()) {
        blobData.appendFileSystemURL(m_fileSystemURL, 0, size, modificationTimeMS / msPerSecond);
        return;
    }
    ASSERT(!m_path.isEmpty());
    blobData.appendFile(m_path, 0, size, modificationTimeMS / msPerSecond);
}

bool File::hasSameSource(const File& other) const
{
    if (m_hasBackingFile != other.m_hasBackingFile)
        return false;

    if (m_hasBackingFile)
        return m_path == other.m_path;

    if (m_fileSystemURL.isEmpty() != other.m_fileSystemURL.isEmpty())
        return false;

    if (!m_fileSystemURL.isEmpty())
        return m_fileSystemURL == other.m_fileSystemURL;

    return uuid() == other.uuid();
}

} // namespace blink
