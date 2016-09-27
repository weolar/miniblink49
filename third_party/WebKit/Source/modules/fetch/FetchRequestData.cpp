// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/FetchRequestData.h"

#include "core/dom/ExecutionContext.h"
#include "core/fetch/ResourceLoaderOptions.h"
#include "core/loader/ThreadableLoader.h"
#include "modules/fetch/BodyStreamBuffer.h"
#include "modules/fetch/DataConsumerTee.h"
#include "modules/fetch/FetchBlobDataConsumerHandle.h"
#include "modules/fetch/FetchHeaderList.h"
#include "platform/network/ResourceRequest.h"
#include "public/platform/WebServiceWorkerRequest.h"
#include "public/platform/WebURLRequest.h"

namespace blink {

FetchRequestData* FetchRequestData::create()
{
    return new FetchRequestData();
}

FetchRequestData* FetchRequestData::create(ExecutionContext* executionContext, const WebServiceWorkerRequest& webRequest)
{
    FetchRequestData* request = FetchRequestData::create();
    request->m_url = webRequest.url();
    request->m_method = webRequest.method();
    for (HTTPHeaderMap::const_iterator it = webRequest.headers().begin(); it != webRequest.headers().end(); ++it)
        request->m_headerList->append(it->key, it->value);
    if (webRequest.blobDataHandle())
        request->setBuffer(BodyStreamBuffer::create(FetchBlobDataConsumerHandle::create(executionContext, webRequest.blobDataHandle())));
    request->setContext(webRequest.requestContext());
    request->m_referrer.setURL(webRequest.referrer());
    request->setMode(webRequest.mode());
    request->setCredentials(webRequest.credentialsMode());
    request->setMIMEType(request->m_headerList->extractMIMEType());
    return request;
}

FetchRequestData* FetchRequestData::clone(ExecutionContext* executionContext)
{
    FetchRequestData* request = FetchRequestData::create();
    request->m_url = m_url;
    request->m_method = m_method;
    request->m_headerList = m_headerList->clone();
    request->m_unsafeRequestFlag = m_unsafeRequestFlag;
    if (m_buffer) {
        OwnPtr<FetchDataConsumerHandle> dest1, dest2;
        DataConsumerTee::create(executionContext, m_buffer->releaseHandle(), &dest1, &dest2);
        m_buffer = BodyStreamBuffer::create(dest1.release());
        request->m_buffer = BodyStreamBuffer::create(dest2.release());
    }
    request->m_origin = m_origin;
    request->m_sameOriginDataURLFlag = m_sameOriginDataURLFlag;
    request->m_context = m_context;
    request->m_referrer = m_referrer;
    request->m_mode = m_mode;
    request->m_credentials = m_credentials;
    request->m_responseTainting = m_responseTainting;
    request->m_mimeType = m_mimeType;
    return request;
}

FetchRequestData* FetchRequestData::pass(ExecutionContext* executionContext)
{
    return clone(executionContext);
}

FetchRequestData::~FetchRequestData()
{
}

FetchRequestData::FetchRequestData()
    : m_method("GET")
    , m_headerList(FetchHeaderList::create())
    , m_unsafeRequestFlag(false)
    , m_context(WebURLRequest::RequestContextUnspecified)
    , m_sameOriginDataURLFlag(false)
    , m_mode(WebURLRequest::FetchRequestModeNoCORS)
    , m_credentials(WebURLRequest::FetchCredentialsModeOmit)
    , m_responseTainting(BasicTainting)
{
}

DEFINE_TRACE(FetchRequestData)
{
    visitor->trace(m_buffer);
    visitor->trace(m_headerList);
}

} // namespace blink
