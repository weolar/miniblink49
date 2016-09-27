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

#ifndef WebFileSystemCallbacks_h
#define WebFileSystemCallbacks_h

#include "WebCommon.h"
#include "WebFileError.h"
#include "WebFileSystemEntry.h"
#include "WebFileSystemType.h"
#include "WebPrivatePtr.h"
#include "WebVector.h"

namespace WTF { template <typename T> class PassOwnPtr; }

namespace blink {

class AsyncFileSystemCallbacks;
class WebFileWriter;
class WebString;
class WebURL;
class WebFileSystemCallbacksPrivate;
struct WebFileInfo;

class WebFileSystemCallbacks {
public:
    ~WebFileSystemCallbacks() { reset(); }
    WebFileSystemCallbacks() { }
    WebFileSystemCallbacks(const WebFileSystemCallbacks& c) { assign(c); }
    WebFileSystemCallbacks& operator=(const WebFileSystemCallbacks& c)
    {
        assign(c);
        return *this;
    }

    BLINK_PLATFORM_EXPORT void reset();
    BLINK_PLATFORM_EXPORT void assign(const WebFileSystemCallbacks&);

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebFileSystemCallbacks(const WTF::PassOwnPtr<AsyncFileSystemCallbacks>&);
#endif

    // Callback for WebFileSystem's various operations that don't require
    // return values.
    BLINK_PLATFORM_EXPORT void didSucceed();

    // Callback for WebFileSystem::readMetadata. Called with the file metadata
    // for the requested path.
    BLINK_PLATFORM_EXPORT void didReadMetadata(const WebFileInfo&);

    // Callback for WebFileSystem::createSnapshot. The metadata also includes the
    // platform file path.
    BLINK_PLATFORM_EXPORT void didCreateSnapshotFile(const WebFileInfo&);

    // Callback for WebFileSystem::readDirectory. Called with a vector of
    // file entries in the requested directory. This callback might be called
    // multiple times if the directory has many entries. |hasMore| must be
    // true when there are more entries.
    BLINK_PLATFORM_EXPORT void didReadDirectory(const WebVector<WebFileSystemEntry>&, bool hasMore);

    // Callback for WebFileSystem::openFileSystem. Called with a name and
    // root URL for the FileSystem when the request is accepted.
    BLINK_PLATFORM_EXPORT void didOpenFileSystem(const WebString& name, const WebURL& rootURL);

    // Callback for WebFileSystem::resolveURL. Called with a name, root URL and
    // file path for the FileSystem when the request is accepted. |isDirectory|
    // must be true when an entry to be resolved is a directory.
    BLINK_PLATFORM_EXPORT void didResolveURL(const WebString& name, const WebURL& rootURL, WebFileSystemType, const WebString& filePath, bool isDirectory);

    // Callback for WebFileSystem::createFileWriter. Called with an instance
    // of WebFileWriter and the target file length. The writer's ownership
    // is transferred to the callback.
    BLINK_PLATFORM_EXPORT void didCreateFileWriter(WebFileWriter*, long long length);

    // Called with an error code when a requested operation hasn't been
    // completed.
    BLINK_PLATFORM_EXPORT void didFail(WebFileError);

    // Returns true if the caller expects to be blocked until the request
    // is fullfilled.
    BLINK_PLATFORM_EXPORT bool shouldBlockUntilCompletion() const;

private:
    WebPrivatePtr<WebFileSystemCallbacksPrivate> m_private;
};

} // namespace blink

#endif
