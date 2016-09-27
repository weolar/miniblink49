// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Body_h
#define Body_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/DOMArrayBuffer.h"
#include "modules/ModulesExport.h"
#include "modules/fetch/FetchDataConsumerHandle.h"
#include "modules/fetch/FetchDataLoader.h"
#include "platform/blob/BlobData.h"
#include "platform/heap/Handle.h"
#include "wtf/RefPtr.h"

namespace blink {

class BodyStreamBuffer;
class DrainingBodyStreamBuffer;
class BodyStreamSource;
class DOMException;
class ReadableByteStream;
class ScriptState;

class MODULES_EXPORT Body
    : public GarbageCollectedFinalized<Body>
    , public ScriptWrappable
    , public ActiveDOMObject
    , public FetchDataLoader::Client {
    DEFINE_WRAPPERTYPEINFO();
    USING_GARBAGE_COLLECTED_MIXIN(Body);
public:
    class ReadableStreamSource;
    enum ResponseType {
        ResponseUnknown,
        ResponseAsArrayBuffer,
        ResponseAsBlob,
        ResponseAsFormData,
        ResponseAsJSON,
        ResponseAsText
    };
    enum LockBodyOption {
        LockBodyOptionNone,
        // Setting "body passed" flag in addition to acquiring a lock.
        PassBody,
    };
    explicit Body(ExecutionContext*);
    ~Body() override { }

    ScriptPromise arrayBuffer(ScriptState*);
    ScriptPromise blob(ScriptState*);
    ScriptPromise formData(ScriptState*);
    ScriptPromise json(ScriptState*);
    ScriptPromise text(ScriptState*);
    ReadableByteStream* body();

    bool bodyUsed() const;
    void lockBody(LockBodyOption = LockBodyOptionNone);

    // Creates a DrainingBodyStreamBuffer to access body data.
    // Returns nullptr if underlying BodyStreamBuffer is null.
    PassOwnPtr<DrainingBodyStreamBuffer> createDrainingStream();

    // ActiveDOMObject override.
    bool hasPendingActivity() const override;

    DECLARE_VIRTUAL_TRACE();

protected:
    // Sets |m_stream| to a newly created stream from |buffer|.
    // |buffer| can be null.
    // This is called when the underlying buffer is set/modified.
    // TODO(hiroshige): Merge FetchRequest/ResponseData::buffer() and
    // integrate Body::setBody(), Request/Response::refreshBody(),
    // FetchRequestData::setBuffer() and
    // FetchResponseData::replaceBodyStreamBuffer().
    void setBody(BodyStreamBuffer* /* buffer */);

private:
    ScriptPromise readAsync(ScriptState*, ResponseType);
    void resolveWithEmptyDataSynchronously();
    void readAsyncFromDrainingBodyStreamBuffer(PassOwnPtr<DrainingBodyStreamBuffer>, const String& mimeType);
    void resolveJSON(const String&);

    // FetchDataLoader::Client functions.
    void didFetchDataLoadFailed() override;
    void didFetchDataLoadedBlobHandle(PassRefPtr<BlobDataHandle>) override;
    void didFetchDataLoadedArrayBuffer(PassRefPtr<DOMArrayBuffer>) override;
    void didFetchDataLoadedString(const String&) override;

    virtual String mimeType() const = 0;

    bool m_bodyUsed;
    ResponseType m_responseType;
    RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;
    Member<ReadableStreamSource> m_streamSource;
    Member<ReadableByteStream> m_stream;
};

} // namespace blink

#endif // Body_h
