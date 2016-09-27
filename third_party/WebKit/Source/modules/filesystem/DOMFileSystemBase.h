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

#ifndef DOMFileSystemBase_h
#define DOMFileSystemBase_h

#include "modules/ModulesExport.h"
#include "modules/filesystem/FileSystemFlags.h"
#include "platform/FileSystemType.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/WTFString.h"

namespace blink {
class WebFileSystem;
}

namespace blink {

class DirectoryReaderBase;
class EntriesCallback;
class EntryBase;
class EntryCallback;
class ErrorCallback;
class File;
class FileError;
class FileMetadata;
class MetadataCallback;
class ExecutionContext;
class SecurityOrigin;
class VoidCallback;

// A common base class for DOMFileSystem and DOMFileSystemSync.
class MODULES_EXPORT DOMFileSystemBase : public GarbageCollectedFinalized<DOMFileSystemBase> {
public:
    enum SynchronousType {
        Synchronous,
        Asynchronous,
    };

    // Path prefixes that are used in the filesystem URLs (that can be obtained by toURL()).
    // http://www.w3.org/TR/file-system-api/#widl-Entry-toURL
    static const char persistentPathPrefix[];
    static const char temporaryPathPrefix[];
    static const char isolatedPathPrefix[];
    static const char externalPathPrefix[];

    virtual ~DOMFileSystemBase();

    // These are called when a new callback is created and resolved in
    // FileSystem API, so that subclasses can track the number of pending
    // callbacks if necessary.
    virtual void addPendingCallbacks() { }
    virtual void removePendingCallbacks() { }

    // Overridden by subclasses to handle sync vs async error-handling.
    virtual void reportError(ErrorCallback*, FileError*) = 0;

    const String& name() const { return m_name; }
    FileSystemType type() const { return m_type; }
    KURL rootURL() const { return m_filesystemRootURL; }
    WebFileSystem* fileSystem() const;
    SecurityOrigin* securityOrigin() const;

    // The clonable flag is used in the structured clone algorithm to test
    // whether the FileSystem API object is permitted to be cloned. It defaults
    // to false, and must be explicitly set by internal code permit cloning.
    void makeClonable() { m_clonable = true; }
    bool clonable() const { return m_clonable; }

    static bool isValidType(FileSystemType);
    static bool crackFileSystemURL(const KURL&, FileSystemType&, String& filePath);
    static KURL createFileSystemRootURL(const String& origin, FileSystemType);
    bool supportsToURL() const;
    KURL createFileSystemURL(const EntryBase*) const;
    KURL createFileSystemURL(const String& fullPath) const;
    static bool pathToAbsolutePath(FileSystemType, const EntryBase*, String path, String& absolutePath);
    static bool pathPrefixToFileSystemType(const String& pathPrefix, FileSystemType&);
    static File* createFile(const FileMetadata&, const KURL& fileSystemURL, FileSystemType, const String name);

    // Actual FileSystem API implementations. All the validity checks on virtual paths are done at this level.
    void getMetadata(const EntryBase*, MetadataCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    void move(const EntryBase* source, EntryBase* parent, const String& name, EntryCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    void copy(const EntryBase* source, EntryBase* parent, const String& name, EntryCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    void remove(const EntryBase*, VoidCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    void removeRecursively(const EntryBase*, VoidCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    void getParent(const EntryBase*, EntryCallback*, ErrorCallback*);
    void getFile(const EntryBase*, const String& path, const FileSystemFlags&, EntryCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    void getDirectory(const EntryBase*, const String& path, const FileSystemFlags&, EntryCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    int readDirectory(DirectoryReaderBase*, const String& path, EntriesCallback*, ErrorCallback*, SynchronousType = Asynchronous);
    bool waitForAdditionalResult(int callbacksId);

    DECLARE_VIRTUAL_TRACE();

protected:
    DOMFileSystemBase(ExecutionContext*, const String& name, FileSystemType, const KURL& rootURL);

    friend class DOMFileSystemBaseTest;
    friend class DOMFileSystemSync;

    RawPtrWillBeMember<ExecutionContext> m_context;
    String m_name;
    FileSystemType m_type;
    KURL m_filesystemRootURL;
    bool m_clonable;

private:
    // This does the same thing with encodeWithURLEscapeSequences defined in
    // KURL.h other than the unicode normalization (NFC).
    // See http://crbug.com/252551 for more details.
    static String encodeFilePathAsURIComponent(const String& fullPath);
};

inline bool operator==(const DOMFileSystemBase& a, const DOMFileSystemBase& b) { return a.name() == b.name() && a.type() == b.type() && a.rootURL() == b.rootURL(); }

} // namespace blink

#endif // DOMFileSystemBase_h
