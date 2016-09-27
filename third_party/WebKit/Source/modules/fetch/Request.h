// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Request_h
#define Request_h

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "modules/ModulesExport.h"
#include "modules/fetch/Body.h"
#include "modules/fetch/FetchRequestData.h"
#include "modules/fetch/Headers.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/WebURLRequest.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class BodyStreamBuffer;
class RequestInit;
class WebServiceWorkerRequest;

typedef RequestOrUSVString RequestInfo;

class MODULES_EXPORT Request final : public Body {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~Request() override { }

    // From Request.idl:
    static Request* create(ScriptState*, const RequestInfo&, const Dictionary&, ExceptionState&);

    static Request* create(ScriptState*, const String&, ExceptionState&);
    static Request* create(ScriptState*, const String&, const Dictionary&, ExceptionState&);
    static Request* create(ScriptState*, Request*, ExceptionState&);
    static Request* create(ScriptState*, Request*, const Dictionary&, ExceptionState&);
    static Request* create(ExecutionContext*, FetchRequestData*);
    static Request* create(ExecutionContext*, const WebServiceWorkerRequest&);

    const FetchRequestData* request() { return m_request; }

    // From Request.idl:
    String method() const;
    KURL url() const;
    Headers* headers() const { return m_headers; }
    String context() const;
    String referrer() const;
    String mode() const;
    String credentials() const;

    // From Request.idl:
    Request* clone(ExceptionState&);

    FetchRequestData* passRequestData();

    void populateWebServiceWorkerRequest(WebServiceWorkerRequest&) const;

    bool hasBody() const { return m_request->buffer(); }

    DECLARE_VIRTUAL_TRACE();

private:
    Request(ExecutionContext*, FetchRequestData*);
    Request(ExecutionContext*, const WebServiceWorkerRequest&);
    Request(ExecutionContext*, FetchRequestData*, Headers*);

    void setBuffer(BodyStreamBuffer*);
    void refreshBody();

    static Request* createRequestWithRequestOrString(ScriptState*, Request*, const String&, const RequestInit&, ExceptionState&);
    void clearHeaderList();

    String mimeType() const override;

    const Member<FetchRequestData> m_request;
    const Member<Headers> m_headers;
};

} // namespace blink

#endif // Request_h
