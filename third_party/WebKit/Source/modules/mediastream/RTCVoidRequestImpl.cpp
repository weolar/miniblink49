/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Google Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#include "config.h"
#include "modules/mediastream/RTCVoidRequestImpl.h"

#include "core/html/VoidCallback.h"
#include "modules/mediastream/RTCErrorCallback.h"
#include "modules/mediastream/RTCPeerConnection.h"

namespace blink {

RTCVoidRequestImpl* RTCVoidRequestImpl::create(ExecutionContext* context, RTCPeerConnection* requester, VoidCallback* successCallback, RTCErrorCallback* errorCallback)
{
    RTCVoidRequestImpl* request = new RTCVoidRequestImpl(context, requester, successCallback, errorCallback);
    request->suspendIfNeeded();
    return request;
}

RTCVoidRequestImpl::RTCVoidRequestImpl(ExecutionContext* context, RTCPeerConnection* requester, VoidCallback* successCallback, RTCErrorCallback* errorCallback)
    : ActiveDOMObject(context)
    , m_successCallback(successCallback)
    , m_errorCallback(errorCallback)
    , m_requester(requester)
{
    ASSERT(m_requester);
}

RTCVoidRequestImpl::~RTCVoidRequestImpl()
{
}

void RTCVoidRequestImpl::requestSucceeded()
{
    bool shouldFireCallback = m_requester ? m_requester->shouldFireDefaultCallbacks() : false;
    if (shouldFireCallback && m_successCallback)
        m_successCallback->handleEvent();

    clear();
}

void RTCVoidRequestImpl::requestFailed(const String& error)
{
    bool shouldFireCallback = m_requester ? m_requester->shouldFireDefaultCallbacks() : false;
    if (shouldFireCallback && m_errorCallback.get())
        m_errorCallback->handleEvent(error);

    clear();
}

void RTCVoidRequestImpl::stop()
{
    clear();
}

void RTCVoidRequestImpl::clear()
{
    m_successCallback.clear();
    m_errorCallback.clear();
    m_requester.clear();
}

DEFINE_TRACE(RTCVoidRequestImpl)
{
    visitor->trace(m_successCallback);
    visitor->trace(m_errorCallback);
    visitor->trace(m_requester);
    RTCVoidRequest::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
