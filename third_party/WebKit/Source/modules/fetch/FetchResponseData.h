// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FetchResponseData_h
#define FetchResponseData_h

#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/WebServiceWorkerRequest.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class BodyStreamBuffer;
class ExecutionContext;
class FetchHeaderList;
class WebServiceWorkerResponse;

class MODULES_EXPORT FetchResponseData final : public GarbageCollectedFinalized<FetchResponseData> {
    WTF_MAKE_NONCOPYABLE(FetchResponseData);
public:
    // "A response has an associated type which is one of basic, CORS, default,
    // error, and opaque. Unless stated otherwise, it is default."
    enum Type { BasicType, CORSType, DefaultType, ErrorType, OpaqueType };
    // "A response can have an associated termination reason which is one of
    // end-user abort, fatal, and timeout."
    enum TerminationReason { EndUserAbortTermination, FatalTermination, TimeoutTermination };

    static FetchResponseData* create();
    static FetchResponseData* createNetworkErrorResponse();
    static FetchResponseData* createWithBuffer(BodyStreamBuffer*);

    FetchResponseData* createBasicFilteredResponse();
    FetchResponseData* createCORSFilteredResponse();
    FetchResponseData* createOpaqueFilteredResponse();

    FetchResponseData* clone(ExecutionContext*);

    Type type() const { return m_type; }
    const KURL& url() const { return m_url; }
    unsigned short status() const { return m_status; }
    AtomicString statusMessage() const { return m_statusMessage; }
    FetchHeaderList* headerList() const { return m_headerList.get(); }
    BodyStreamBuffer* buffer() const { return m_buffer; }
    String mimeType() const;
    BodyStreamBuffer* internalBuffer() const;
    String internalMIMEType() const;

    void setURL(const KURL& url) { m_url = url; }
    void setStatus(unsigned short status) { m_status = status; }
    void setStatusMessage(AtomicString statusMessage) { m_statusMessage = statusMessage; }
    void setMIMEType(const String& type) { m_mimeType = type; }

    // If the type is Default, replaces |m_buffer|.
    // If the type is Basic or CORS, replaces |m_buffer| and
    // |m_internalResponse->m_buffer|.
    // If the type is Error or Opaque, does nothing.
    void replaceBodyStreamBuffer(BodyStreamBuffer*);

    // Does not call response.setBlobDataHandle().
    void populateWebServiceWorkerResponse(WebServiceWorkerResponse& /* response */);

    DECLARE_TRACE();

private:
    FetchResponseData(Type, unsigned short, AtomicString);

    Type m_type;
    OwnPtr<TerminationReason> m_terminationReason;
    KURL m_url;
    unsigned short m_status;
    AtomicString m_statusMessage;
    Member<FetchHeaderList> m_headerList;
    Member<FetchResponseData> m_internalResponse;
    Member<BodyStreamBuffer> m_buffer;
    String m_mimeType;
};

} // namespace blink

#endif // FetchResponseData_h
