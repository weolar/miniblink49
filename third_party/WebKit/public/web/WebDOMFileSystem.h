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
#ifndef WebDOMFileSystem_h
#define WebDOMFileSystem_h

#include "../platform/WebCommon.h"
#include "../platform/WebFileSystem.h"
#include "../platform/WebPrivatePtr.h"
#include "../platform/WebString.h"
#include "../platform/WebURL.h"
#include "WebFrame.h"

#if BLINK_IMPLEMENTATION
#include "platform/heap/Handle.h"
#endif

namespace v8 {
class Isolate;
class Object;
class Value;
template <class T> class Local;
}

namespace blink {

class DOMFileSystem;

class WebDOMFileSystem {
public:
    enum SerializableType {
        SerializableTypeSerializable,
        SerializableTypeNotSerializable,
    };
    enum EntryType {
        EntryTypeFile,
        EntryTypeDirectory,
    };

    ~WebDOMFileSystem() { reset(); }

    WebDOMFileSystem() { }
    WebDOMFileSystem(const WebDOMFileSystem& d) { assign(d); }
    WebDOMFileSystem& operator=(const WebDOMFileSystem& d)
    {
        assign(d);
        return *this;
    }

    BLINK_EXPORT static WebDOMFileSystem fromV8Value(v8::Local<v8::Value>);
    // Create file system URL from the given entry.
    BLINK_EXPORT static WebURL createFileSystemURL(v8::Local<v8::Value> entry);

    // FIXME: Deprecate the last argument when all filesystems become
    // serializable.
    BLINK_EXPORT static WebDOMFileSystem create(
        WebLocalFrame*,
        WebFileSystemType,
        const WebString& name,
        const WebURL& rootURL,
        SerializableType = SerializableTypeNotSerializable);

    BLINK_EXPORT void reset();
    BLINK_EXPORT void assign(const WebDOMFileSystem&);

    BLINK_EXPORT WebString name() const;
    BLINK_EXPORT WebFileSystem::Type type() const;
    BLINK_EXPORT WebURL rootURL() const;

    BLINK_EXPORT v8::Local<v8::Value> toV8Value(v8::Local<v8::Object> creationContext, v8::Isolate*);
    BLINK_EXPORT v8::Local<v8::Value> createV8Entry(
        const WebString& path,
        EntryType,
        v8::Local<v8::Object> creationContext, v8::Isolate*);

    bool isNull() const { return m_private.isNull(); }

#if BLINK_IMPLEMENTATION
    WebDOMFileSystem(DOMFileSystem*);
    WebDOMFileSystem& operator=(DOMFileSystem*);
#endif

private:
    WebPrivatePtr<DOMFileSystem> m_private;
};

} // namespace blink

#endif // WebDOMFileSystem_h
