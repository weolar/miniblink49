/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
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
 * 3. Neither the name of Ericsson nor the names of its contributors
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
#include "platform/mediastream/MediaStreamCenter.h"

#include "platform/mediastream/MediaStreamDescriptor.h"
#include "platform/mediastream/MediaStreamTrackSourcesRequest.h"
#include "platform/mediastream/MediaStreamWebAudioSource.h"
#include "public/platform/Platform.h"
#include "public/platform/WebAudioSourceProvider.h"
#include "public/platform/WebMediaStream.h"
#include "public/platform/WebMediaStreamCenter.h"
#include "public/platform/WebMediaStreamTrack.h"
#include "wtf/Assertions.h"
#include "wtf/MainThread.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

MediaStreamCenter& MediaStreamCenter::instance()
{
    ASSERT(isMainThread());
    DEFINE_STATIC_LOCAL(MediaStreamCenter, center, ());
    return center;
}

MediaStreamCenter::MediaStreamCenter()
    : m_private(adoptPtr(Platform::current()->createMediaStreamCenter(this)))
{
}

MediaStreamCenter::~MediaStreamCenter()
{
}

void MediaStreamCenter::didSetMediaStreamTrackEnabled(MediaStreamComponent* component)
{
    if (m_private) {
        if (component->enabled()) {
            m_private->didEnableMediaStreamTrack(component);
        } else {
            m_private->didDisableMediaStreamTrack(component);
        }
    }
}

bool MediaStreamCenter::didAddMediaStreamTrack(MediaStreamDescriptor* stream, MediaStreamComponent* component)
{
    return m_private && m_private->didAddMediaStreamTrack(stream, component);
}

bool MediaStreamCenter::didRemoveMediaStreamTrack(MediaStreamDescriptor* stream, MediaStreamComponent* component)
{
    return m_private && m_private->didRemoveMediaStreamTrack(stream, component);
}

void MediaStreamCenter::didStopLocalMediaStream(MediaStreamDescriptor* stream)
{
    if (m_private)
        m_private->didStopLocalMediaStream(stream);
}

bool MediaStreamCenter::didStopMediaStreamTrack(MediaStreamComponent* track)
{
    return m_private && m_private->didStopMediaStreamTrack(track);
}

void MediaStreamCenter::didCreateMediaStreamAndTracks(MediaStreamDescriptor* stream)
{
    if (!m_private)
        return;

    for (size_t i = 0; i < stream->numberOfAudioComponents(); ++i)
        didCreateMediaStreamTrack(stream->audioComponent(i));

    for (size_t i = 0; i < stream->numberOfVideoComponents(); ++i)
        didCreateMediaStreamTrack(stream->videoComponent(i));

    WebMediaStream webStream(stream);
    m_private->didCreateMediaStream(webStream);
}

void MediaStreamCenter::didCreateMediaStream(MediaStreamDescriptor* stream)
{
    if (m_private) {
        WebMediaStream webStream(stream);
        m_private->didCreateMediaStream(webStream);
    }
}

void MediaStreamCenter::didCreateMediaStreamTrack(MediaStreamComponent* track)
{
    if (m_private)
        m_private->didCreateMediaStreamTrack(track);
}

PassOwnPtr<AudioSourceProvider> MediaStreamCenter::createWebAudioSourceFromMediaStreamTrack(MediaStreamComponent* track)
{
    ASSERT_UNUSED(track, track);
#if ENABLE(WEB_AUDIO)
    if (m_private)
        return MediaStreamWebAudioSource::create(adoptPtr(m_private->createWebAudioSourceFromMediaStreamTrack(track)));
#endif

    return nullptr;
}

void MediaStreamCenter::stopLocalMediaStream(const WebMediaStream& webStream)
{
    MediaStreamDescriptor* stream = webStream;
    MediaStreamDescriptorClient* client = stream->client();
    if (client)
        client->streamEnded();
    else
        stream->setEnded();
}

} // namespace blink
