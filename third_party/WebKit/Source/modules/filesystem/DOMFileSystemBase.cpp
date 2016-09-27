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
#include "modules/filesystem/DOMFileSystemBase.h"

#include "core/dom/ExecutionContext.h"
#include "core/fileapi/File.h"
#include "core/fileapi/FileError.h"
#include "core/html/VoidCallback.h"
#include "modules/filesystem/DOMFilePath.h"
#include "modules/filesystem/DirectoryEntry.h"
#include "modules/filesystem/DirectoryReaderBase.h"
#include "modules/filesystem/EntriesCallback.h"
#include "modules/filesystem/Entry.h"
#include "modules/filesystem/EntryBase.h"
#include "modules/filesystem/EntryCallback.h"
#include "modules/filesystem/ErrorCallback.h"
#include "modules/filesystem/FileSystemCallbacks.h"
#include "modules/filesystem/MetadataCallback.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebFileSystem.h"
#include "public/platform/WebFileSystemCallbacks.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/TextEncoding.h"
#include <url/url_util.h>

namespace blink {

const char DOMFileSystemBase::persistentPathPrefix[] = "persistent";
const char DOMFileSystemBase::temporaryPathPrefix[] = "temporary";
const char DOMFileSystemBase::isolatedPathPrefix[] = "isolated";
const char DOMFileSystemBase::externalPathPrefix[] = "external";

DOMFileSystemBase::DOMFileSystemBase(ExecutionContext* context, const String& name, FileSystemType type, const KURL& rootURL)
    : m_context(context)
    , m_name(name)
    , m_type(type)
    , m_filesystemRootURL(rootURL)
    , m_clonable(false)
{
}

DOMFileSystemBase::~DOMFileSystemBase()
{
}

DEFINE_TRACE(DOMFileSystemBase)
{
    visitor->trace(m_context);
}

WebFileSystem* DOMFileSystemBase::fileSystem() const
{
    Platform* platform = Platform::current();
    if (!platform)
        return nullptr;
    return platform->fileSystem();
}

SecurityOrigin* DOMFileSystemBase::securityOrigin() const
{
    return m_context->securityOrigin();
}

bool DOMFileSystemBase::isValidType(FileSystemType type)
{
    return type == FileSystemTypeTemporary || type == FileSystemTypePersistent || type == FileSystemTypeIsolated || type == FileSystemTypeExternal;
}

bool DOMFileSystemBase::crackFileSystemURL(const KURL& url, FileSystemType& type, String& filePath)
{
    if (!url.protocolIs("filesystem"))
        return false;

    if (!url.innerURL())
        return false;

    String typeString = url.innerURL()->path().substring(1);
    if (!pathPrefixToFileSystemType(typeString, type))
        return false;

    filePath = decodeURLEscapeSequences(url.path());
    return true;
}

KURL DOMFileSystemBase::createFileSystemRootURL(const String& origin, FileSystemType type)
{
    String typeString;
    if (type == FileSystemTypeTemporary)
        typeString = temporaryPathPrefix;
    else if (type == FileSystemTypePersistent)
        typeString = persistentPathPrefix;
    else if (type == FileSystemTypeExternal)
        typeString = externalPathPrefix;
    else
        return KURL();

    String result = "filesystem:" + origin + "/" + typeString + "/";
    return KURL(ParsedURLString, result);
}

bool DOMFileSystemBase::supportsToURL() const
{
    ASSERT(isValidType(m_type));
    return m_type != FileSystemTypeIsolated;
}

KURL DOMFileSystemBase::createFileSystemURL(const EntryBase* entry) const
{
    return createFileSystemURL(entry->fullPath());
}

KURL DOMFileSystemBase::createFileSystemURL(const String& fullPath) const
{
    ASSERT(DOMFilePath::isAbsolute(fullPath));

    if (type() == FileSystemTypeExternal) {
        // For external filesystem originString could be different from what we have in m_filesystemRootURL.
        StringBuilder result;
        result.appendLiteral("filesystem:");
        result.append(securityOrigin()->toString());
        result.append('/');
        result.append(externalPathPrefix);
        result.append(m_filesystemRootURL.path());
        // Remove the extra leading slash.
        result.append(encodeFilePathAsURIComponent(fullPath.substring(1)));
        return KURL(ParsedURLString, result.toString());
    }

    // For regular types we can just append the entry's fullPath to the m_filesystemRootURL that should look like 'filesystem:<origin>/<typePrefix>'.
    ASSERT(!m_filesystemRootURL.isEmpty());
    KURL url = m_filesystemRootURL;
    // Remove the extra leading slash.
    url.setPath(url.path() + encodeFilePathAsURIComponent(fullPath.substring(1)));
    return url;
}

bool DOMFileSystemBase::pathToAbsolutePath(FileSystemType type, const EntryBase* base, String path, String& absolutePath)
{
    ASSERT(base);

    if (!DOMFilePath::isAbsolute(path))
        path = DOMFilePath::append(base->fullPath(), path);
    absolutePath = DOMFilePath::removeExtraParentReferences(path);

    return (type != FileSystemTypeTemporary && type != FileSystemTypePersistent) || DOMFilePath::isValidPath(absolutePath);
}

bool DOMFileSystemBase::pathPrefixToFileSystemType(const String& pathPrefix, FileSystemType& type)
{
    if (pathPrefix == temporaryPathPrefix) {
        type = FileSystemTypeTemporary;
        return true;
    }

    if (pathPrefix == persistentPathPrefix) {
        type = FileSystemTypePersistent;
        return true;
    }

    if (pathPrefix == externalPathPrefix) {
        type = FileSystemTypeExternal;
        return true;
    }

    return false;
}

File* DOMFileSystemBase::createFile(const FileMetadata& metadata, const KURL& fileSystemURL, FileSystemType type, const String name)
{
    // For regular filesystem types (temporary or persistent), we should not cache file metadata as it could change File semantics.
    // For other filesystem types (which could be platform-specific ones), there's a chance that the files are on remote filesystem.
    // If the port has returned metadata just pass it to File constructor (so we may cache the metadata).
    // FIXME: We should use the snapshot metadata for all files.
    // https://www.w3.org/Bugs/Public/show_bug.cgi?id=17746
    if (type == FileSystemTypeTemporary || type == FileSystemTypePersistent)
        return File::createForFileSystemFile(metadata.platformPath, name);

    const File::UserVisibility userVisibility = (type == FileSystemTypeExternal) ? File::IsUserVisible : File::IsNotUserVisible;

    if (!metadata.platformPath.isEmpty()) {
        // If the platformPath in the returned metadata is given, we create a File object for the snapshot path.
        return File::createForFileSystemFile(name, metadata, userVisibility);
    } else {
        // Otherwise we create a File object for the fileSystemURL.
        return File::createForFileSystemFile(fileSystemURL, metadata, userVisibility);
    }
}

void DOMFileSystemBase::getMetadata(const EntryBase* entry, MetadataCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    OwnPtr<AsyncFileSystemCallbacks> callbacks(MetadataCallbacks::create(successCallback, errorCallback, m_context, this));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);
    fileSystem()->readMetadata(createFileSystemURL(entry), callbacks.release());
}

static bool verifyAndGetDestinationPathForCopyOrMove(const EntryBase* source, EntryBase* parent, const String& newName, String& destinationPath)
{
    ASSERT(source);

    if (!parent || !parent->isDirectory())
        return false;

    if (!newName.isEmpty() && !DOMFilePath::isValidName(newName))
        return false;

    const bool isSameFileSystem = (*source->filesystem() == *parent->filesystem());

    // It is an error to try to copy or move an entry inside itself at any depth if it is a directory.
    if (source->isDirectory() && isSameFileSystem && DOMFilePath::isParentOf(source->fullPath(), parent->fullPath()))
        return false;

    // It is an error to copy or move an entry into its parent if a name different from its current one isn't provided.
    if (isSameFileSystem && (newName.isEmpty() || source->name() == newName) && DOMFilePath::getDirectory(source->fullPath()) == parent->fullPath())
        return false;

    destinationPath = parent->fullPath();
    if (!newName.isEmpty())
        destinationPath = DOMFilePath::append(destinationPath, newName);
    else
        destinationPath = DOMFilePath::append(destinationPath, source->name());

    return true;
}

void DOMFileSystemBase::move(const EntryBase* source, EntryBase* parent, const String& newName, EntryCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    String destinationPath;
    if (!verifyAndGetDestinationPathForCopyOrMove(source, parent, newName, destinationPath)) {
        reportError(errorCallback, FileError::create(FileError::INVALID_MODIFICATION_ERR));
        return;
    }

    OwnPtr<AsyncFileSystemCallbacks> callbacks(EntryCallbacks::create(successCallback, errorCallback, m_context, parent->filesystem(), destinationPath, source->isDirectory()));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);

    fileSystem()->move(createFileSystemURL(source), parent->filesystem()->createFileSystemURL(destinationPath), callbacks.release());
}

void DOMFileSystemBase::copy(const EntryBase* source, EntryBase* parent, const String& newName, EntryCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    String destinationPath;
    if (!verifyAndGetDestinationPathForCopyOrMove(source, parent, newName, destinationPath)) {
        reportError(errorCallback, FileError::create(FileError::INVALID_MODIFICATION_ERR));
        return;
    }

    OwnPtr<AsyncFileSystemCallbacks> callbacks(EntryCallbacks::create(successCallback, errorCallback, m_context, parent->filesystem(), destinationPath, source->isDirectory()));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);

    fileSystem()->copy(createFileSystemURL(source), parent->filesystem()->createFileSystemURL(destinationPath), callbacks.release());
}

void DOMFileSystemBase::remove(const EntryBase* entry, VoidCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    ASSERT(entry);
    // We don't allow calling remove() on the root directory.
    if (entry->fullPath() == String(DOMFilePath::root)) {
        reportError(errorCallback, FileError::create(FileError::INVALID_MODIFICATION_ERR));
        return;
    }

    OwnPtr<AsyncFileSystemCallbacks> callbacks(VoidCallbacks::create(successCallback, errorCallback, m_context, this));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);

    fileSystem()->remove(createFileSystemURL(entry), callbacks.release());
}

void DOMFileSystemBase::removeRecursively(const EntryBase* entry, VoidCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    ASSERT(entry && entry->isDirectory());
    // We don't allow calling remove() on the root directory.
    if (entry->fullPath() == String(DOMFilePath::root)) {
        reportError(errorCallback, FileError::create(FileError::INVALID_MODIFICATION_ERR));
        return;
    }

    OwnPtr<AsyncFileSystemCallbacks> callbacks(VoidCallbacks::create(successCallback, errorCallback, m_context, this));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);

    fileSystem()->removeRecursively(createFileSystemURL(entry), callbacks.release());
}

void DOMFileSystemBase::getParent(const EntryBase* entry, EntryCallback* successCallback, ErrorCallback* errorCallback)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    ASSERT(entry);
    String path = DOMFilePath::getDirectory(entry->fullPath());

    fileSystem()->directoryExists(createFileSystemURL(path), EntryCallbacks::create(successCallback, errorCallback, m_context, this, path, true));
}

void DOMFileSystemBase::getFile(const EntryBase* entry, const String& path, const FileSystemFlags& flags, EntryCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    String absolutePath;
    if (!pathToAbsolutePath(m_type, entry, path, absolutePath)) {
        reportError(errorCallback, FileError::create(FileError::INVALID_MODIFICATION_ERR));
        return;
    }

    OwnPtr<AsyncFileSystemCallbacks> callbacks(EntryCallbacks::create(successCallback, errorCallback, m_context, this, absolutePath, false));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);

    if (flags.createFlag())
        fileSystem()->createFile(createFileSystemURL(absolutePath), flags.exclusive(), callbacks.release());
    else
        fileSystem()->fileExists(createFileSystemURL(absolutePath), callbacks.release());
}

void DOMFileSystemBase::getDirectory(const EntryBase* entry, const String& path, const FileSystemFlags& flags, EntryCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return;
    }

    String absolutePath;
    if (!pathToAbsolutePath(m_type, entry, path, absolutePath)) {
        reportError(errorCallback, FileError::create(FileError::INVALID_MODIFICATION_ERR));
        return;
    }

    OwnPtr<AsyncFileSystemCallbacks> callbacks(EntryCallbacks::create(successCallback, errorCallback, m_context, this, absolutePath, true));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);

    if (flags.createFlag())
        fileSystem()->createDirectory(createFileSystemURL(absolutePath), flags.exclusive(), callbacks.release());
    else
        fileSystem()->directoryExists(createFileSystemURL(absolutePath), callbacks.release());
}

int DOMFileSystemBase::readDirectory(DirectoryReaderBase* reader, const String& path, EntriesCallback* successCallback, ErrorCallback* errorCallback, SynchronousType synchronousType)
{
    if (!fileSystem()) {
        reportError(errorCallback, FileError::create(FileError::ABORT_ERR));
        return 0;
    }

    ASSERT(DOMFilePath::isAbsolute(path));

    OwnPtr<AsyncFileSystemCallbacks> callbacks(EntriesCallbacks::create(successCallback, errorCallback, m_context, reader, path));
    callbacks->setShouldBlockUntilCompletion(synchronousType == Synchronous);

    return fileSystem()->readDirectory(createFileSystemURL(path), callbacks.release());
}

bool DOMFileSystemBase::waitForAdditionalResult(int callbacksId)
{
    if (!fileSystem())
        return false;
    return fileSystem()->waitForAdditionalResult(callbacksId);
}

String DOMFileSystemBase::encodeFilePathAsURIComponent(const String& fullPath)
{
    CString utf8 = UTF8Encoding().encode(fullPath, WTF::URLEncodedEntitiesForUnencodables);

    url::RawCanonOutputT<char> buffer;
    int inputLength = utf8.length();
    if (buffer.length() < inputLength * 3)
        buffer.Resize(inputLength * 3);

    url::EncodeURIComponent(utf8.data(), inputLength, &buffer);
    String escaped(buffer.data(), buffer.length());
    // Unescape '/'; it's safe and much prettier.
    escaped.replace("%2F", "/");
    return escaped;
}

} // namespace blink
