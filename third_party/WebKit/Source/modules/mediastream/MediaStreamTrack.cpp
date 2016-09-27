/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
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
#include "modules/mediastream/MediaStreamTrack.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/Event.h"
#include "modules/mediastream/MediaStream.h"
#include "modules/mediastream/MediaStreamTrackSourcesCallback.h"
#include "modules/mediastream/MediaStreamTrackSourcesRequestImpl.h"
#include "modules/mediastream/UserMediaController.h"
#include "platform/mediastream/MediaStreamCenter.h"
#include "platform/mediastream/MediaStreamComponent.h"
#include "public/platform/WebSourceInfo.h"

namespace blink {

MediaStreamTrack* MediaStreamTrack::create(ExecutionContext* context, MediaStreamComponent* component)
{
    MediaStreamTrack* track = new MediaStreamTrack(context, component);
    track->suspendIfNeeded();
    return track;
}

MediaStreamTrack::MediaStreamTrack(ExecutionContext* context, MediaStreamComponent* component)
    : ActiveDOMObject(context)
    , m_readyState(MediaStreamSource::ReadyStateLive)
    , m_isIteratingRegisteredMediaStreams(false)
    , m_stopped(false)
    , m_component(component)
{
    m_component->source()->addObserver(this);
}

MediaStreamTrack::~MediaStreamTrack()
{
    m_component->source()->removeObserver(this);
}

String MediaStreamTrack::kind() const
{
    DEFINE_STATIC_LOCAL(String, audioKind, ("audio"));
    DEFINE_STATIC_LOCAL(String, videoKind, ("video"));

    switch (m_component->source()->type()) {
    case MediaStreamSource::TypeAudio:
        return audioKind;
    case MediaStreamSource::TypeVideo:
        return videoKind;
    }

    ASSERT_NOT_REACHED();
    return audioKind;
}

String MediaStreamTrack::id() const
{
    return m_component->id();
}

String MediaStreamTrack::label() const
{
    return m_component->source()->name();
}

bool MediaStreamTrack::enabled() const
{
    return m_component->enabled();
}

void MediaStreamTrack::setEnabled(bool enabled)
{
    if (enabled == m_component->enabled())
        return;

    m_component->setEnabled(enabled);

    if (!ended())
        MediaStreamCenter::instance().didSetMediaStreamTrackEnabled(m_component.get());
}

bool MediaStreamTrack::muted() const
{
    return m_component->muted();
}

bool MediaStreamTrack::remote() const
{
    return m_component->source()->remote();
}

bool MediaStreamTrack::readonly() const
{
    return m_component->source()->readonly();
}

String MediaStreamTrack::readyState() const
{
    if (ended())
        return "ended";

    switch (m_readyState) {
    case MediaStreamSource::ReadyStateLive:
        return "live";
    case MediaStreamSource::ReadyStateMuted:
        return "muted";
    case MediaStreamSource::ReadyStateEnded:
        return "ended";
    }

    ASSERT_NOT_REACHED();
    return String();
}

void MediaStreamTrack::getSources(ExecutionContext* context, MediaStreamTrackSourcesCallback* callback, ExceptionState& exceptionState)
{
    LocalFrame* frame = toDocument(context)->frame();
    UserMediaController* userMedia = UserMediaController::from(frame);
    if (!userMedia) {
        exceptionState.throwDOMException(NotSupportedError, "No sources controller available; is this a detached window?");
        return;
    }
    MediaStreamTrackSourcesRequest* request = MediaStreamTrackSourcesRequestImpl::create(*context, callback);
    userMedia->requestSources(request);
}

void MediaStreamTrack::stopTrack(ExceptionState& exceptionState)
{
    if (ended())
        return;

    m_readyState = MediaStreamSource::ReadyStateEnded;
    MediaStreamCenter::instance().didStopMediaStreamTrack(component());
    dispatchEvent(Event::create(EventTypeNames::ended));
    propagateTrackEnded();
}

MediaStreamTrack* MediaStreamTrack::clone(ExecutionContext* context)
{
    RefPtr<MediaStreamComponent> clonedComponent = MediaStreamComponent::create(component()->source());
    MediaStreamTrack* clonedTrack = MediaStreamTrack::create(context, clonedComponent.get());
    MediaStreamCenter::instance().didCreateMediaStreamTrack(clonedComponent.get());
    return clonedTrack;
}

bool MediaStreamTrack::ended() const
{
    return m_stopped || (m_readyState == MediaStreamSource::ReadyStateEnded);
}

void MediaStreamTrack::sourceChangedState()
{
    if (ended())
        return;

    m_readyState = m_component->source()->readyState();
    switch (m_readyState) {
    case MediaStreamSource::ReadyStateLive:
        m_component->setMuted(false);
        dispatchEvent(Event::create(EventTypeNames::unmute));
        break;
    case MediaStreamSource::ReadyStateMuted:
        m_component->setMuted(true);
        dispatchEvent(Event::create(EventTypeNames::mute));
        break;
    case MediaStreamSource::ReadyStateEnded:
        dispatchEvent(Event::create(EventTypeNames::ended));
        propagateTrackEnded();
        break;
    }
}

void MediaStreamTrack::propagateTrackEnded()
{
    RELEASE_ASSERT(!m_isIteratingRegisteredMediaStreams);
    m_isIteratingRegisteredMediaStreams = true;
    for (HeapHashSet<Member<MediaStream>>::iterator iter = m_registeredMediaStreams.begin(); iter != m_registeredMediaStreams.end(); ++iter)
        (*iter)->trackEnded();
    m_isIteratingRegisteredMediaStreams = false;
}

MediaStreamComponent* MediaStreamTrack::component()
{
    return m_component.get();
}

void MediaStreamTrack::stop()
{
    m_stopped = true;
}

PassOwnPtr<AudioSourceProvider> MediaStreamTrack::createWebAudioSource()
{
    return MediaStreamCenter::instance().createWebAudioSourceFromMediaStreamTrack(component());
}

void MediaStreamTrack::registerMediaStream(MediaStream* mediaStream)
{
    RELEASE_ASSERT(!m_isIteratingRegisteredMediaStreams);
    RELEASE_ASSERT(!m_registeredMediaStreams.contains(mediaStream));
    m_registeredMediaStreams.add(mediaStream);
}

void MediaStreamTrack::unregisterMediaStream(MediaStream* mediaStream)
{
    RELEASE_ASSERT(!m_isIteratingRegisteredMediaStreams);
    HeapHashSet<Member<MediaStream>>::iterator iter = m_registeredMediaStreams.find(mediaStream);
    RELEASE_ASSERT(iter != m_registeredMediaStreams.end());
    m_registeredMediaStreams.remove(iter);
}

const AtomicString& MediaStreamTrack::interfaceName() const
{
    return EventTargetNames::MediaStreamTrack;
}

ExecutionContext* MediaStreamTrack::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

DEFINE_TRACE(MediaStreamTrack)
{
    visitor->trace(m_registeredMediaStreams);
    RefCountedGarbageCollectedEventTargetWithInlineData<MediaStreamTrack>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
