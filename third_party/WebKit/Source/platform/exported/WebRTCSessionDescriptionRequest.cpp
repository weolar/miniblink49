/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#include "public/platform/WebRTCSessionDescriptionRequest.h"

#include "platform/mediastream/RTCSessionDescriptionRequest.h"
#include "public/platform/WebRTCSessionDescription.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

namespace {

class ExtraDataContainer : public RTCSessionDescriptionRequest::ExtraData {
public:
    ExtraDataContainer(PassOwnPtr<WebRTCSessionDescriptionRequest::ExtraData> extraData) : m_extraData(extraData) { }

    WebRTCSessionDescriptionRequest::ExtraData* extraData() { return m_extraData.get(); }

private:
    OwnPtr<WebRTCSessionDescriptionRequest::ExtraData> m_extraData;
};

} // namespace

WebRTCSessionDescriptionRequest::WebRTCSessionDescriptionRequest(RTCSessionDescriptionRequest* constraints)
    : m_private(constraints)
{
}

void WebRTCSessionDescriptionRequest::assign(const WebRTCSessionDescriptionRequest& other)
{
    m_private = other.m_private;
}

void WebRTCSessionDescriptionRequest::reset()
{
    m_private.reset();
}

void WebRTCSessionDescriptionRequest::requestSucceeded(const WebRTCSessionDescription& sessionDescription) const
{
    ASSERT(m_private.get());
    m_private->requestSucceeded(sessionDescription);
}

void WebRTCSessionDescriptionRequest::requestFailed(const WebString& error) const
{
    ASSERT(m_private.get());
    m_private->requestFailed(error);
}

WebRTCSessionDescriptionRequest::ExtraData* WebRTCSessionDescriptionRequest::extraData() const
{
    RTCSessionDescriptionRequest::ExtraData* data = m_private->extraData();
    if (!data)
        return 0;
    return static_cast<ExtraDataContainer*>(data)->extraData();
}

void WebRTCSessionDescriptionRequest::setExtraData(ExtraData* extraData)
{
    m_private->setExtraData(adoptPtr(new ExtraDataContainer(adoptPtr(extraData))));
}

} // namespace blink

