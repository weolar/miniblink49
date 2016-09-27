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

#ifndef WebBlobRegistry_h
#define WebBlobRegistry_h

#include "WebCommon.h"
#include "WebThreadSafeData.h"

namespace blink {

class WebBlobData;
class WebString;
class WebURL;

// Acts as singleton facade for all Blob interactions ouside of blink.  This
// includes blob:
// * creation,
// * reference counting,
// * publishing, and
// * streaming.
class WebBlobRegistry {
public:
    // Builder class for creating blobs. The blob is built on calling the
    // build() method, where IPCs are sent to the browser.
    // Preconditions:
    // * Not meant to be used on multiple threads.
    // * Must not be kept alive longer than creator WebBlobRegistry (shouldn't
    //   be an issue because of the singleton nature of the WebBlobRegistry)
    // * append.* methods are invalid after build() is called.
    class Builder {
    public:
        virtual ~Builder() { }
        virtual void appendData(const WebThreadSafeData&) = 0;
        virtual void appendFile(const WebString& path, uint64_t offset, uint64_t length, double expectedModificationTime) = 0;
        // Calling this method ensures the given blob lives for the creation of
        // the new blob.
        virtual void appendBlob(const WebString& uuid, uint64_t offset, uint64_t length) = 0;
        virtual void appendFileSystemURL(const WebURL&, uint64_t offset, uint64_t length, double expectedModificationTime) = 0;

        // Builds the blob. All calls to append* are invalid after calling this
        // method.
        virtual void build() = 0;
    };

    virtual ~WebBlobRegistry() { }

    // TODO(dmurph): Deprecate and migrate to createBuilder
    virtual void registerBlobData(const WebString& uuid, const WebBlobData&) { }

    // Caller takes ownership of the Builder. The blob is finalized (and sent to
    // the browser) on calling build() on the Builder object.
    virtual Builder* createBuilder(const WebString& uuid, const WebString& contentType) { BLINK_ASSERT_NOT_REACHED(); return nullptr; }

    virtual void addBlobDataRef(const WebString& uuid) { }
    virtual void removeBlobDataRef(const WebString& uuid) { }
    virtual void registerPublicBlobURL(const WebURL&, const WebString& uuid) { }
    virtual void revokePublicBlobURL(const WebURL&) { }

    // Registers a stream URL referring to a stream with the specified media
    // type.
    virtual void registerStreamURL(const WebURL&, const WebString&) { BLINK_ASSERT_NOT_REACHED(); }

    // Registers a stream URL referring to the stream identified by the
    // specified srcURL.
    virtual void registerStreamURL(const WebURL&, const WebURL& srcURL) { BLINK_ASSERT_NOT_REACHED(); }

    // Add data to the stream referred by the URL.
    virtual void addDataToStream(const WebURL&, const char* data, size_t length) { BLINK_ASSERT_NOT_REACHED(); }

    // Flush contents buffered in the stream to the corresponding reader.
    virtual void flushStream(const WebURL&) { BLINK_ASSERT_NOT_REACHED(); }

    // Tell the registry that construction of this stream has completed
    // successfully and so it won't receive any more data.
    virtual void finalizeStream(const WebURL&) { BLINK_ASSERT_NOT_REACHED(); }

    // Tell the registry that construction of this stream has been aborted and
    // so it won't receive any more data.
    virtual void abortStream(const WebURL&) { BLINK_ASSERT_NOT_REACHED(); }

    // Unregisters a stream referred by the URL.
    virtual void unregisterStreamURL(const WebURL&) { BLINK_ASSERT_NOT_REACHED(); }
};

} // namespace blink

#endif // WebBlobRegistry_h
