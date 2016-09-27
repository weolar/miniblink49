// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FetchRequestData_h
#define FetchRequestData_h

#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/Referrer.h"
#include "public/platform/WebServiceWorkerRequest.h"
#include "public/platform/WebURLRequest.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace blink {

class BodyStreamBuffer;
class ExecutionContext;
class FetchHeaderList;
class SecurityOrigin;
class WebServiceWorkerRequest;

class FetchRequestData final : public GarbageCollectedFinalized<FetchRequestData> {
    WTF_MAKE_NONCOPYABLE(FetchRequestData);
public:
    enum Tainting { BasicTainting, CORSTainting, OpaqueTainting };

    class Referrer final {
    public:
        Referrer() : m_type(ClientReferrer) { }
        bool isNoReferrer() const { return m_type == NoReferrer; }
        bool isClient() const { return m_type == ClientReferrer; }
        bool isURL() const { return m_type == URLReferrer; }
        void setNoReferrer()
        {
            m_referrer = blink::Referrer();
            m_type = NoReferrer;
        }
        void setClient()
        {
            m_referrer = blink::Referrer();
            m_type = ClientReferrer;
        }
        void setURL(const blink::Referrer& referrer)
        {
            m_referrer = referrer;
            m_type = URLReferrer;
        }
        blink::Referrer referrer() const { return m_referrer; }
    private:
        enum Type { NoReferrer, ClientReferrer, URLReferrer };
        Type m_type;
        blink::Referrer m_referrer;
    };

    static FetchRequestData* create();
    static FetchRequestData* create(ExecutionContext*, const WebServiceWorkerRequest&);
    // Call Request::refreshBody() after calling clone() or pass().
    FetchRequestData* clone(ExecutionContext*);
    FetchRequestData* pass(ExecutionContext*);
    ~FetchRequestData();

    void setMethod(AtomicString method) { m_method = method; }
    const AtomicString method() const { return m_method; }
    void setURL(const KURL& url) { m_url = url; }
    const KURL& url() const { return m_url; }
    bool unsafeRequestFlag() const { return m_unsafeRequestFlag; }
    void setUnsafeRequestFlag(bool flag) { m_unsafeRequestFlag = flag; }
    WebURLRequest::RequestContext context() const { return m_context; }
    void setContext(WebURLRequest::RequestContext context) { m_context = context; }
    PassRefPtr<SecurityOrigin> origin() { return m_origin; }
    void setOrigin(PassRefPtr<SecurityOrigin> origin) { m_origin = origin; }
    bool sameOriginDataURLFlag() { return m_sameOriginDataURLFlag; }
    void setSameOriginDataURLFlag(bool flag) { m_sameOriginDataURLFlag = flag; }
    const Referrer& referrer() const { return m_referrer; }
    Referrer* mutableReferrer() { return &m_referrer; }
    void setMode(WebURLRequest::FetchRequestMode mode) { m_mode = mode; }
    WebURLRequest::FetchRequestMode mode() const { return m_mode; }
    void setCredentials(WebURLRequest::FetchCredentialsMode credentials) { m_credentials = credentials; }
    WebURLRequest::FetchCredentialsMode credentials() const { return m_credentials; }
    void setResponseTainting(Tainting tainting) { m_responseTainting = tainting; }
    Tainting tainting() const { return m_responseTainting; }
    FetchHeaderList* headerList() const { return m_headerList.get(); }
    void setHeaderList(FetchHeaderList* headerList) { m_headerList = headerList; }
    BodyStreamBuffer* buffer() const { return m_buffer; }
    // Call Request::refreshBody() after calling setBuffer().
    void setBuffer(BodyStreamBuffer* buffer) { m_buffer = buffer; }
    String mimeType() const { return m_mimeType; }
    void setMIMEType(const String& type) { m_mimeType = type; }

    DECLARE_TRACE();

private:
    FetchRequestData();

    AtomicString m_method;
    KURL m_url;
    Member<FetchHeaderList> m_headerList;
    bool m_unsafeRequestFlag;
    // FIXME: Support m_skipServiceWorkerFlag;
    WebURLRequest::RequestContext m_context;
    RefPtr<SecurityOrigin> m_origin;
    // FIXME: Support m_forceOriginHeaderFlag;
    bool m_sameOriginDataURLFlag;
    Referrer m_referrer;
    // FIXME: Support m_authenticationFlag;
    // FIXME: Support m_synchronousFlag;
    WebURLRequest::FetchRequestMode m_mode;
    WebURLRequest::FetchCredentialsMode m_credentials;
    // FIXME: Support m_useURLCredentialsFlag;
    // FIXME: Support m_manualRedirectFlag;
    // FIXME: Support m_redirectCount;
    Tainting m_responseTainting;
    Member<BodyStreamBuffer> m_buffer;
    String m_mimeType;
};

} // namespace blink

#endif // FetchRequestData_h
