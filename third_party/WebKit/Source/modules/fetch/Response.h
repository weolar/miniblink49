// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Response_h
#define Response_h

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "modules/ModulesExport.h"
#include "modules/fetch/Body.h"
#include "modules/fetch/BodyStreamBuffer.h"
#include "modules/fetch/FetchResponseData.h"
#include "modules/fetch/Headers.h"
#include "platform/blob/BlobData.h"
#include "platform/heap/Handle.h"

namespace blink {

class Blob;
class DrainingBodyStreamBuffer;
class DOMArrayBuffer;
class ExceptionState;
class ResponseInit;
class WebServiceWorkerResponse;

typedef BlobOrArrayBufferOrArrayBufferViewOrFormDataOrUSVString BodyInit;

class MODULES_EXPORT Response final : public Body, public BodyStreamBuffer::DrainingStreamNotificationClient {
    DEFINE_WRAPPERTYPEINFO();
    USING_GARBAGE_COLLECTED_MIXIN(Response);
public:
    ~Response() override { }

    // From Response.idl:
    static Response* create(ExecutionContext*, ExceptionState&);
    static Response* create(ExecutionContext*, const BodyInit&, const Dictionary&, ExceptionState&);

    static Response* create(ExecutionContext*, Blob*, const ResponseInit&, ExceptionState&);
    static Response* create(ExecutionContext*, FetchResponseData*);
    static Response* create(ExecutionContext*, const WebServiceWorkerResponse&);

    static Response* createClone(const Response&);

    static Response* error(ExecutionContext*);
    static Response* redirect(ExecutionContext*, const String& url, unsigned short status, ExceptionState&);

    const FetchResponseData* response() const { return m_response; }

    // From Response.idl:
    String type() const;
    String url() const;
    unsigned short status() const;
    bool ok() const;
    String statusText() const;
    Headers* headers() const;

    // From Response.idl:
    Response* clone(ExceptionState&);

    // ActiveDOMObject
    bool hasPendingActivity() const override;

    // Does not call response.setBlobDataHandle().
    void populateWebServiceWorkerResponse(WebServiceWorkerResponse& /* response */);

    bool hasBody() const;

    String mimeType() const override;
    String internalMIMEType() const;

    // Do not call leakBuffer() on the returned buffer because
    // hasPendingActivity() assumes didFetchDataLoadFinishedFromDrainingStream()
    // will be called.
    PassOwnPtr<DrainingBodyStreamBuffer> createInternalDrainingStream();
    void didFetchDataLoadFinishedFromDrainingStream() override;

    // Only for tests (null checks and identity checks).
    void* bufferForTest() const;
    void* internalBufferForTest() const;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit Response(ExecutionContext*);
    Response(ExecutionContext*, FetchResponseData*);
    Response(ExecutionContext*, FetchResponseData*, Headers*);

    void refreshBody();

    const Member<FetchResponseData> m_response;
    const Member<Headers> m_headers;
    bool m_isInternalDrained;
};

} // namespace blink

#endif // Response_h
