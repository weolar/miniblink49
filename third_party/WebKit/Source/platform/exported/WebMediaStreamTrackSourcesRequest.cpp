/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "public/platform/WebMediaStreamTrackSourcesRequest.h"

#include "platform/mediastream/MediaStreamTrackSourcesRequest.h"
#include "public/platform/WebSourceInfo.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

class ExtraDataContainer : public MediaStreamTrackSourcesRequest::ExtraData {
public:
    ExtraDataContainer(PassOwnPtr<WebMediaStreamTrackSourcesRequest::ExtraData> extraData) : m_extraData(extraData) { }

    WebMediaStreamTrackSourcesRequest::ExtraData* extraData() { return m_extraData.get(); }

private:
    OwnPtr<WebMediaStreamTrackSourcesRequest::ExtraData> m_extraData;
};

} // namespace

WebMediaStreamTrackSourcesRequest::WebMediaStreamTrackSourcesRequest(MediaStreamTrackSourcesRequest* request)
    : m_private(request)
{
}

void WebMediaStreamTrackSourcesRequest::assign(const WebMediaStreamTrackSourcesRequest& other)
{
    m_private = other.m_private;
}

void WebMediaStreamTrackSourcesRequest::reset()
{
    m_private.reset();
}

WebString WebMediaStreamTrackSourcesRequest::origin() const
{
    ASSERT(m_private.get());
    return m_private->origin();
}

void WebMediaStreamTrackSourcesRequest::requestSucceeded(const WebVector<WebSourceInfo>& sourceInfos) const
{
    ASSERT(m_private.get());
    m_private->requestSucceeded(sourceInfos);
}

WebMediaStreamTrackSourcesRequest::ExtraData* WebMediaStreamTrackSourcesRequest::extraData() const
{
    MediaStreamTrackSourcesRequest::ExtraData* data = m_private->extraData();
    if (!data)
        return 0;
    return static_cast<ExtraDataContainer*>(data)->extraData();
}

void WebMediaStreamTrackSourcesRequest::setExtraData(ExtraData* extraData)
{
    m_private->setExtraData(adoptPtr(new ExtraDataContainer(adoptPtr(extraData))));
}

} // namespace blink

