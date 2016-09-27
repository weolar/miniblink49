// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebServiceWorkerResponse.h"

#include "platform/blob/BlobData.h"
#include "platform/network/HTTPHeaderMap.h"
#include "public/platform/WebHTTPHeaderVisitor.h"
#include "wtf/HashMap.h"

namespace blink {

class WebServiceWorkerResponsePrivate : public RefCounted<WebServiceWorkerResponsePrivate> {
public:
    WebServiceWorkerResponsePrivate()
        : status(0)
        , responseType(WebServiceWorkerResponseTypeDefault)
        , error(WebServiceWorkerResponseErrorUnknown)
    {
    }
    WebURL url;
    unsigned short status;
    WebString statusText;
    WebServiceWorkerResponseType responseType;
    HTTPHeaderMap headers;
    RefPtr<BlobDataHandle> blobDataHandle;
    WebURL streamURL;
    WebServiceWorkerResponseError error;
};

WebServiceWorkerResponse::WebServiceWorkerResponse()
    : m_private(adoptRef(new WebServiceWorkerResponsePrivate))
{
}

void WebServiceWorkerResponse::reset()
{
    m_private.reset();
}

void WebServiceWorkerResponse::assign(const WebServiceWorkerResponse& other)
{
    m_private = other.m_private;
}

void WebServiceWorkerResponse::setURL(const WebURL& url)
{
    m_private->url = url;
}

WebURL WebServiceWorkerResponse::url() const
{
    return m_private->url;
}

void WebServiceWorkerResponse::setStatus(unsigned short status)
{
    m_private->status = status;
}

unsigned short WebServiceWorkerResponse::status() const
{
    return m_private->status;
}

void WebServiceWorkerResponse::setStatusText(const WebString& statusText)
{
    m_private->statusText = statusText;
}

WebString WebServiceWorkerResponse::statusText() const
{
    return m_private->statusText;
}

void WebServiceWorkerResponse::setResponseType(WebServiceWorkerResponseType responseType)
{
    m_private->responseType = responseType;
}

WebServiceWorkerResponseType WebServiceWorkerResponse::responseType() const
{
    return m_private->responseType;
}

void WebServiceWorkerResponse::setHeader(const WebString& key, const WebString& value)
{
    m_private->headers.set(key, value);
}

void WebServiceWorkerResponse::appendHeader(const WebString& key, const WebString& value)
{
    HTTPHeaderMap::AddResult addResult = m_private->headers.add(key, value);
    if (!addResult.isNewEntry)
        addResult.storedValue->value = addResult.storedValue->value + ", " + String(value);
}

WebVector<WebString> WebServiceWorkerResponse::getHeaderKeys() const
{
    Vector<String> keys;
    for (HTTPHeaderMap::const_iterator it = m_private->headers.begin(), end = m_private->headers.end(); it != end; ++it)
        keys.append(it->key);

    return keys;
}

WebString WebServiceWorkerResponse::getHeader(const WebString& key) const
{
    return m_private->headers.get(key);
}

void WebServiceWorkerResponse::visitHTTPHeaderFields(WebHTTPHeaderVisitor* headerVisitor) const
{
    for (HTTPHeaderMap::const_iterator i = m_private->headers.begin(), end = m_private->headers.end(); i != end; ++i)
        headerVisitor->visitHeader(i->key, i->value);
}

void WebServiceWorkerResponse::setBlob(const WebString& uuid, uint64_t size)
{
    m_private->blobDataHandle = BlobDataHandle::create(uuid, String(), size);
}

WebString WebServiceWorkerResponse::blobUUID() const
{
    if (!m_private->blobDataHandle)
        return WebString();
    return m_private->blobDataHandle->uuid();
}

uint64_t WebServiceWorkerResponse::blobSize() const
{
    if (!m_private->blobDataHandle)
        return 0;
    return m_private->blobDataHandle->size();
}

const HTTPHeaderMap& WebServiceWorkerResponse::headers() const
{
    return m_private->headers;
}

void WebServiceWorkerResponse::setBlobDataHandle(PassRefPtr<BlobDataHandle> blobDataHandle)
{
    m_private->blobDataHandle = blobDataHandle;
}

PassRefPtr<BlobDataHandle> WebServiceWorkerResponse::blobDataHandle() const
{
    return m_private->blobDataHandle;
}

void WebServiceWorkerResponse::setStreamURL(const WebURL& url)
{
    m_private->streamURL = url;
}

WebURL WebServiceWorkerResponse::streamURL() const
{
    return m_private->streamURL;
}

void WebServiceWorkerResponse::setError(WebServiceWorkerResponseError error)
{
    m_private->error = error;
}

WebServiceWorkerResponseError WebServiceWorkerResponse::error() const
{
    return m_private->error;
}

} // namespace blink
