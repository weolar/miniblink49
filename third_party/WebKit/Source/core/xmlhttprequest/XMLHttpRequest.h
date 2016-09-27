/*
 *  Copyright (C) 2003, 2006, 2008 Apple Inc. All rights reserved.
 *  Copyright (C) 2005, 2006 Alexey Proskuryakov <ap@nypop.com>
 *  Copyright (C) 2011 Google Inc. All rights reserved.
 *  Copyright (C) 2012 Intel Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef XMLHttpRequest_h
#define XMLHttpRequest_h

#include "bindings/core/v8/ScriptString.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/DocumentParserClient.h"
#include "core/loader/ThreadableLoaderClient.h"
#include "core/xmlhttprequest/XMLHttpRequestEventTarget.h"
#include "core/xmlhttprequest/XMLHttpRequestProgressEventThrottle.h"
#include "platform/heap/Handle.h"
#include "platform/network/FormData.h"
#include "platform/network/HTTPHeaderMap.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ArrayBufferOrArrayBufferViewOrBlobOrDocumentOrStringOrFormData;
class Blob;
class BlobDataHandle;
class DOMArrayBuffer;
class DOMArrayBufferView;
class DOMFormData;
class Document;
class DocumentParser;
class ExceptionState;
class ExecutionContext;
class ScriptState;
class SharedBuffer;
class Stream;
class TextResourceDecoder;
class ThreadableLoader;
class WebDataConsumerHandle;
class XMLHttpRequestUpload;

typedef int ExceptionCode;

class XMLHttpRequest final : public XMLHttpRequestEventTarget, private ThreadableLoaderClient, public DocumentParserClient, public ActiveDOMObject {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(XMLHttpRequest);
public:
    static XMLHttpRequest* create(ScriptState*);
    static XMLHttpRequest* create(ExecutionContext*);
    ~XMLHttpRequest() override;

    // These exact numeric values are important because JS expects them.
    enum State {
        UNSENT = 0,
        OPENED = 1,
        HEADERS_RECEIVED = 2,
        LOADING = 3,
        DONE = 4
    };

    enum ResponseTypeCode {
        ResponseTypeDefault,
        ResponseTypeText,
        ResponseTypeJSON,
        ResponseTypeDocument,
        ResponseTypeBlob,
        ResponseTypeArrayBuffer,
        ResponseTypeLegacyStream,
    };

    // ActiveDOMObject
    void contextDestroyed() override;
    ExecutionContext* executionContext() const override;
    bool hasPendingActivity() const override;
    void suspend() override;
    void resume() override;
    void stop() override;

    // XMLHttpRequestEventTarget
    const AtomicString& interfaceName() const override;

    // JavaScript attributes and methods
    const KURL& url() const { return m_url; }
    String statusText() const;
    int status() const;
    State readyState() const;
    bool withCredentials() const { return m_includeCredentials; }
    void setWithCredentials(bool, ExceptionState&);
    void open(const AtomicString& method, const String& url, ExceptionState&);
    void open(const AtomicString& method, const String& url, bool async, const String& username, const String& password, ExceptionState&);
    void open(const AtomicString& method, const KURL&, bool async, ExceptionState&);
    void send(const ArrayBufferOrArrayBufferViewOrBlobOrDocumentOrStringOrFormData&, ExceptionState&);
    void abort();
    void setRequestHeader(const AtomicString& name, const AtomicString& value, ExceptionState&);
    void overrideMimeType(const AtomicString& override, ExceptionState&);
    String getAllResponseHeaders() const;
    const AtomicString& getResponseHeader(const AtomicString&) const;
    ScriptString responseText(ExceptionState&);
    ScriptString responseJSONSource();
    Document* responseXML(ExceptionState&);
    Blob* responseBlob();
    DOMArrayBuffer* responseArrayBuffer();
    Stream* responseLegacyStream();
    unsigned timeout() const { return m_timeoutMilliseconds; }
    void setTimeout(unsigned timeout, ExceptionState&);
    ResponseTypeCode responseTypeCode() const { return m_responseTypeCode; }
    String responseType();
    void setResponseType(const String&, ExceptionState&);
    String responseURL();

    // For Inspector.
    void sendForInspectorXHRReplay(PassRefPtr<FormData>, ExceptionState&);

    XMLHttpRequestUpload* upload();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(readystatechange);

    // (Also) eagerly finalized so as to prevent access to the eagerly finalized
    // progress event throttle.
    EAGERLY_FINALIZE();
#if !ENABLE(OILPAN)
    DECLARE_EAGER_FINALIZATION_OPERATOR_NEW();
#endif
    DECLARE_VIRTUAL_TRACE();

private:
    class BlobLoader;
    XMLHttpRequest(ExecutionContext*, PassRefPtr<SecurityOrigin>);

    Document* document() const;
    SecurityOrigin* securityOrigin() const;

    void didSendData(unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override;
    void didReceiveResponse(unsigned long identifier, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
    void didReceiveData(const char* data, unsigned dataLength) override;
    // When responseType is set to "blob", didDownloadData() is called instead
    // of didReceiveData().
    void didDownloadData(int dataLength) override;
    void didFinishLoading(unsigned long identifier, double finishTime) override;
    void didFail(const ResourceError&) override;
    void didFailRedirectCheck() override;

    // BlobLoader notifications.
    void didFinishLoadingInternal();
    void didFinishLoadingFromBlob();
    void didFailLoadingFromBlob();

    PassRefPtr<BlobDataHandle> createBlobDataHandleFromResponse();

    // DocumentParserClient
    void notifyParserStopped() override;

    void endLoading();

    // Returns the MIME type part of m_mimeTypeOverride if present and
    // successfully parsed, or returns one of the "Content-Type" header value
    // of the received response.
    //
    // This method is named after the term "final MIME type" defined in the
    // spec but doesn't convert the result to ASCII lowercase as specified in
    // the spec. Must be lowered later or compared using case insensitive
    // comparison functions if required.
    AtomicString finalResponseMIMEType() const;
    // The same as finalResponseMIMEType() but fallbacks to "text/xml" if
    // finalResponseMIMEType() returns an empty string.
    AtomicString finalResponseMIMETypeWithFallback() const;
    bool responseIsXML() const;
    bool responseIsHTML() const;

    PassOwnPtr<TextResourceDecoder> createDecoder() const;

    void initResponseDocument();
    void parseDocumentChunk(const char* data, unsigned dataLength);

    bool areMethodAndURLValidForSend();

    bool initSend(ExceptionState&);
    void sendBytesData(const void*, size_t, ExceptionState&);
    void send(Document*, ExceptionState&);
    void send(const String&, ExceptionState&);
    void send(Blob*, ExceptionState&);
    void send(DOMFormData*, ExceptionState&);
    void send(DOMArrayBuffer*, ExceptionState&);
    void send(DOMArrayBufferView*, ExceptionState&);

    const AtomicString& getRequestHeader(const AtomicString& name) const;
    void setRequestHeaderInternal(const AtomicString& name, const AtomicString& value);

    void trackProgress(long long dataLength);
    // Changes m_state and dispatches a readyStateChange event if new m_state
    // value is different from last one.
    void changeState(State newState);
    void dispatchReadyStateChangeEvent();

    // Clears variables used only while the resource is being loaded.
    void clearVariablesForLoading();
    // Returns false iff reentry happened and a new load is started.
    //
    // This method may invoke V8 GC with m_loader unset. If you touch the
    // XMLHttpRequest instance after internalAbort() call, you must hold a
    // refcount on it to prevent it from destroyed.
    bool internalAbort();
    // Clears variables holding response header and body data.
    void clearResponse();
    void clearRequest();

    void createRequest(PassRefPtr<FormData>, ExceptionState&);

    // Dispatches a response ProgressEvent.
    void dispatchProgressEvent(const AtomicString&, long long, long long);
    // Dispatches a response ProgressEvent using values sampled from
    // m_receivedLength and m_response.
    void dispatchProgressEventFromSnapshot(const AtomicString&);

    // Handles didFail() call not caused by cancellation or timeout.
    void handleNetworkError();
    // Handles didFail() call for cancellations. For example, the
    // ResourceLoader handling the load notifies m_loader of an error
    // cancellation when the frame containing the XHR navigates away.
    void handleDidCancel();
    // Handles didFail() call for timeout.
    void handleDidTimeout();

    void handleRequestError(ExceptionCode, const AtomicString&, long long, long long);

    XMLHttpRequestProgressEventThrottle& progressEventThrottle();

    Member<XMLHttpRequestUpload> m_upload;

    KURL m_url;
    AtomicString m_method;
    HTTPHeaderMap m_requestHeaders;
    // Not converted to ASCII lowercase. Must be lowered later or compared
    // using case insensitive comparison functions if needed.
    AtomicString m_mimeTypeOverride;
    unsigned long m_timeoutMilliseconds;
    Member<Blob> m_responseBlob;
    Member<Stream> m_responseLegacyStream;

    RefPtr<ThreadableLoader> m_loader;
    State m_state;

    ResourceResponse m_response;
    String m_finalResponseCharset;

    OwnPtr<TextResourceDecoder> m_decoder;

    ScriptString m_responseText;
    RefPtrWillBeMember<Document> m_responseDocument;
    RefPtrWillBeMember<DocumentParser> m_responseDocumentParser;

    RefPtr<SharedBuffer> m_binaryResponseBuilder;
    long long m_lengthDownloadedToFile;

    RefPtr<DOMArrayBuffer> m_responseArrayBuffer;

    // Used for onprogress tracking
    long long m_receivedLength;

    // An exception to throw in synchronous mode. It's set when failure
    // notification is received from m_loader and thrown at the end of send() if
    // any.
    ExceptionCode m_exceptionCode;

    Member<XMLHttpRequestProgressEventThrottle> m_progressEventThrottle;

    // An enum corresponding to the allowed string values for the responseType attribute.
    ResponseTypeCode m_responseTypeCode;
    RefPtr<SecurityOrigin> m_securityOrigin;

    // This blob loader will be used if |m_downloadingToFile| is true and
    // |m_responseTypeCode| is NOT ResponseTypeBlob.
    Member<BlobLoader> m_blobLoader;

    // Positive if we are dispatching events.
    // This is an integer specifying the recursion level rather than a boolean
    // because in some cases we have recursive dispatching.
    int m_eventDispatchRecursionLevel;

    bool m_async;
    bool m_includeCredentials;
    // Used to skip m_responseDocument creation if it's done previously. We need
    // this separate flag since m_responseDocument can be 0 for some cases.
    bool m_parsedResponse;
    bool m_error;
    bool m_uploadEventsAllowed;
    bool m_uploadComplete;
    bool m_sameOriginRequest;
    // True iff the ongoing resource loading is using the downloadToFile
    // option.
    bool m_downloadingToFile;
    bool m_responseTextOverflow;
};

} // namespace blink

#endif // XMLHttpRequest_h
