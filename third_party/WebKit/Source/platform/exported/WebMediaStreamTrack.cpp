/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "public/platform/WebMediaStreamTrack.h"

#include "platform/mediastream/MediaStreamComponent.h"
#include "platform/mediastream/MediaStreamSource.h"
#include "public/platform/WebAudioSourceProvider.h"
#include "public/platform/WebMediaStream.h"
#include "public/platform/WebMediaStreamSource.h"
#include "public/platform/WebString.h"

namespace blink {

namespace {

class ExtraDataContainer : public MediaStreamComponent::ExtraData {
public:
    explicit ExtraDataContainer(PassOwnPtr<WebMediaStreamTrack::ExtraData> extraData) : m_extraData(extraData) { }

    WebMediaStreamTrack::ExtraData* extraData() { return m_extraData.get(); }

private:
    OwnPtr<WebMediaStreamTrack::ExtraData> m_extraData;
};

} // namespace

WebMediaStreamTrack WebMediaStreamTrack::ExtraData::owner()
{
    ASSERT(m_owner);
    return WebMediaStreamTrack(m_owner);
}

void WebMediaStreamTrack::ExtraData::setOwner(MediaStreamComponent* owner)
{
    ASSERT(!m_owner);
    m_owner = owner;
}

WebMediaStreamTrack::WebMediaStreamTrack(PassRefPtr<MediaStreamComponent> mediaStreamComponent)
    : m_private(mediaStreamComponent)
{
}

WebMediaStreamTrack::WebMediaStreamTrack(MediaStreamComponent* mediaStreamComponent)
    : m_private(mediaStreamComponent)
{
}

WebMediaStreamTrack& WebMediaStreamTrack::operator=(MediaStreamComponent* mediaStreamComponent)
{
    m_private = mediaStreamComponent;
    return *this;
}

void WebMediaStreamTrack::initialize(const WebMediaStreamSource& source)
{
    m_private = MediaStreamComponent::create(source);
}

void WebMediaStreamTrack::initialize(const WebString& id, const WebMediaStreamSource& source)
{
    m_private = MediaStreamComponent::create(id, source);
}

void WebMediaStreamTrack::reset()
{
    m_private.reset();
}

WebMediaStreamTrack::operator PassRefPtr<MediaStreamComponent>() const
{
    return m_private.get();
}

WebMediaStreamTrack::operator MediaStreamComponent*() const
{
    return m_private.get();
}

bool WebMediaStreamTrack::isEnabled() const
{
    ASSERT(!m_private.isNull());
    return m_private->enabled();
}

bool WebMediaStreamTrack::isMuted() const
{
    ASSERT(!m_private.isNull());
    return m_private->muted();
}

WebString WebMediaStreamTrack::id() const
{
    ASSERT(!m_private.isNull());
    return m_private->id();
}

WebMediaStreamSource WebMediaStreamTrack::source() const
{
    ASSERT(!m_private.isNull());
    return WebMediaStreamSource(m_private->source());
}

WebMediaStreamTrack::ExtraData* WebMediaStreamTrack::extraData() const
{
    MediaStreamComponent::ExtraData* data = m_private->extraData();
    if (!data)
        return 0;
    return static_cast<ExtraDataContainer*>(data)->extraData();
}

void WebMediaStreamTrack::setExtraData(ExtraData* extraData)
{
    ASSERT(!m_private.isNull());

    if (extraData)
        extraData->setOwner(m_private.get());

    m_private->setExtraData(adoptPtr(new ExtraDataContainer(adoptPtr(extraData))));
}

void WebMediaStreamTrack::setSourceProvider(WebAudioSourceProvider* provider)
{
#if ENABLE(WEB_AUDIO)
    ASSERT(!m_private.isNull());
    m_private->setSourceProvider(provider);
#endif // ENABLE(WEB_AUDIO)
}

void WebMediaStreamTrack::assign(const WebMediaStreamTrack& other)
{
    m_private = other.m_private;
}

} // namespace blink
