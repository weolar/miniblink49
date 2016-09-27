// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerRequest_h
#define WebServiceWorkerRequest_h

#include "WebCommon.h"
#include "public/platform/WebPrivatePtr.h"
#include "public/platform/WebReferrerPolicy.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLRequest.h"

#if INSIDE_BLINK
#include "platform/network/HTTPHeaderMap.h"
#include "platform/weborigin/Referrer.h"
#include "wtf/Forward.h"
#include "wtf/text/StringHash.h"
#include <utility>
#endif

namespace blink {

class BlobDataHandle;
class WebHTTPHeaderVisitor;
class WebServiceWorkerRequestPrivate;

// Represents a request for a web resource.
class BLINK_PLATFORM_EXPORT WebServiceWorkerRequest {
public:
    ~WebServiceWorkerRequest() { reset(); }
    WebServiceWorkerRequest();
    WebServiceWorkerRequest(const WebServiceWorkerRequest& other) { assign(other); }
    WebServiceWorkerRequest& operator=(const WebServiceWorkerRequest& other)
    {
        assign(other);
        return *this;
    }

    void reset();
    void assign(const WebServiceWorkerRequest&);

    void setURL(const WebURL&);
    WebURL url() const;

    void setMethod(const WebString&);
    WebString method() const;

    void setHeader(const WebString& key, const WebString& value);

    // If the key already exists, the value is appended to the existing value
    // with a comma delimiter between them.
    void appendHeader(const WebString& key, const WebString& value);

    void visitHTTPHeaderFields(WebHTTPHeaderVisitor*) const;

    void setBlob(const WebString& uuid, long long size);

    void setReferrer(const WebString&, WebReferrerPolicy);
    WebURL referrerUrl() const;
    WebReferrerPolicy referrerPolicy() const;

    void setMode(WebURLRequest::FetchRequestMode);
    WebURLRequest::FetchRequestMode mode() const;

    void setCredentialsMode(WebURLRequest::FetchCredentialsMode);
    WebURLRequest::FetchCredentialsMode credentialsMode() const;

    void setRequestContext(WebURLRequest::RequestContext);
    WebURLRequest::RequestContext requestContext() const;

    void setFrameType(WebURLRequest::FrameType);
    WebURLRequest::FrameType frameType() const;

    void setIsReload(bool);
    bool isReload() const;

#if INSIDE_BLINK
    const HTTPHeaderMap& headers() const;
    PassRefPtr<BlobDataHandle> blobDataHandle() const;
    const Referrer& referrer() const;
#endif

private:
    WebPrivatePtr<WebServiceWorkerRequestPrivate> m_private;
};

} // namespace blink

#endif // WebServiceWorkerRequest_h
