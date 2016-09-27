/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011, 2012 Ericsson AB. All rights reserved.
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
#include "modules/mediastream/MediaStream.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "modules/mediastream/MediaStreamRegistry.h"
#include "modules/mediastream/MediaStreamTrackEvent.h"
#include "platform/mediastream/MediaStreamCenter.h"
#include "platform/mediastream/MediaStreamSource.h"

namespace blink {

static bool containsSource(MediaStreamTrackVector& trackVector, MediaStreamSource* source)
{
    for (size_t i = 0; i < trackVector.size(); ++i) {
        if (source->id() == trackVector[i]->component()->source()->id())
            return true;
    }
    return false;
}

static void processTrack(MediaStreamTrack* track, MediaStreamTrackVector& trackVector)
{
    if (track->ended())
        return;

    MediaStreamSource* source = track->component()->source();
    if (!containsSource(trackVector, source))
        trackVector.append(track);
}

MediaStream* MediaStream::create(ExecutionContext* context)
{
    MediaStreamTrackVector audioTracks;
    MediaStreamTrackVector videoTracks;

    return new MediaStream(context, audioTracks, videoTracks);
}

MediaStream* MediaStream::create(ExecutionContext* context, MediaStream* stream)
{
    ASSERT(stream);

    MediaStreamTrackVector audioTracks;
    MediaStreamTrackVector videoTracks;

    for (size_t i = 0; i < stream->m_audioTracks.size(); ++i)
        processTrack(stream->m_audioTracks[i].get(), audioTracks);

    for (size_t i = 0; i < stream->m_videoTracks.size(); ++i)
        processTrack(stream->m_videoTracks[i].get(), videoTracks);

    return new MediaStream(context, audioTracks, videoTracks);
}

MediaStream* MediaStream::create(ExecutionContext* context, const MediaStreamTrackVector& tracks)
{
    MediaStreamTrackVector audioTracks;
    MediaStreamTrackVector videoTracks;

    for (size_t i = 0; i < tracks.size(); ++i)
        processTrack(tracks[i].get(), tracks[i]->kind() == "audio" ? audioTracks : videoTracks);

    return new MediaStream(context, audioTracks, videoTracks);
}

MediaStream* MediaStream::create(ExecutionContext* context, PassRefPtr<MediaStreamDescriptor> streamDescriptor)
{
    return new MediaStream(context, streamDescriptor);
}

MediaStream::MediaStream(ExecutionContext* context, PassRefPtr<MediaStreamDescriptor> streamDescriptor)
    : ContextLifecycleObserver(context)
    , m_stopped(false)
    , m_descriptor(streamDescriptor)
    , m_scheduledEventTimer(this, &MediaStream::scheduledEventTimerFired)
{
    m_descriptor->setClient(this);

    size_t numberOfAudioTracks = m_descriptor->numberOfAudioComponents();
    m_audioTracks.reserveCapacity(numberOfAudioTracks);
    for (size_t i = 0; i < numberOfAudioTracks; i++) {
        MediaStreamTrack* newTrack = MediaStreamTrack::create(context, m_descriptor->audioComponent(i));
        newTrack->registerMediaStream(this);
        m_audioTracks.append(newTrack);
    }

    size_t numberOfVideoTracks = m_descriptor->numberOfVideoComponents();
    m_videoTracks.reserveCapacity(numberOfVideoTracks);
    for (size_t i = 0; i < numberOfVideoTracks; i++) {
        MediaStreamTrack* newTrack = MediaStreamTrack::create(context, m_descriptor->videoComponent(i));
        newTrack->registerMediaStream(this);
        m_videoTracks.append(newTrack);
    }

    if (emptyOrOnlyEndedTracks()) {
        m_descriptor->setActive(false);
    }
}

MediaStream::MediaStream(ExecutionContext* context, const MediaStreamTrackVector& audioTracks, const MediaStreamTrackVector& videoTracks)
    : ContextLifecycleObserver(context)
    , m_stopped(false)
    , m_scheduledEventTimer(this, &MediaStream::scheduledEventTimerFired)
{
    MediaStreamComponentVector audioComponents;
    MediaStreamComponentVector videoComponents;

    MediaStreamTrackVector::const_iterator iter;
    for (iter = audioTracks.begin(); iter != audioTracks.end(); ++iter) {
        (*iter)->registerMediaStream(this);
        audioComponents.append((*iter)->component());
    }
    for (iter = videoTracks.begin(); iter != videoTracks.end(); ++iter) {
        (*iter)->registerMediaStream(this);
        videoComponents.append((*iter)->component());
    }

    m_descriptor = MediaStreamDescriptor::create(audioComponents, videoComponents);
    m_descriptor->setClient(this);
    MediaStreamCenter::instance().didCreateMediaStream(m_descriptor.get());

    m_audioTracks = audioTracks;
    m_videoTracks = videoTracks;
    if (emptyOrOnlyEndedTracks()) {
        m_descriptor->setActive(false);
    }
}

MediaStream::~MediaStream()
{
    m_descriptor->setClient(0);
}

bool MediaStream::emptyOrOnlyEndedTracks()
{
    if (!m_audioTracks.size() && !m_videoTracks.size()) {
        return true;
    }
    for (MediaStreamTrackVector::iterator iter = m_audioTracks.begin(); iter != m_audioTracks.end(); ++iter) {
        if (!iter->get()->ended())
            return false;
    }
    for (MediaStreamTrackVector::iterator iter = m_videoTracks.begin(); iter != m_videoTracks.end(); ++iter) {
        if (!iter->get()->ended())
            return false;
    }
    return true;
}

bool MediaStream::ended() const
{
    return m_stopped || m_descriptor->ended();
}

MediaStreamTrackVector MediaStream::getTracks()
{
    MediaStreamTrackVector tracks;
    for (MediaStreamTrackVector::iterator iter = m_audioTracks.begin(); iter != m_audioTracks.end(); ++iter)
        tracks.append(iter->get());
    for (MediaStreamTrackVector::iterator iter = m_videoTracks.begin(); iter != m_videoTracks.end(); ++iter)
        tracks.append(iter->get());
    return tracks;
}

void MediaStream::addTrack(MediaStreamTrack* track, ExceptionState& exceptionState)
{
    if (!track) {
        exceptionState.throwDOMException(TypeMismatchError, "The MediaStreamTrack provided is invalid.");
        return;
    }

    if (getTrackById(track->id()))
        return;

    switch (track->component()->source()->type()) {
    case MediaStreamSource::TypeAudio:
        m_audioTracks.append(track);
        break;
    case MediaStreamSource::TypeVideo:
        m_videoTracks.append(track);
        break;
    }
    track->registerMediaStream(this);
    m_descriptor->addComponent(track->component());

    if (!active() && !track->ended()) {
        m_descriptor->setActive(true);
        scheduleDispatchEvent(Event::create(EventTypeNames::active));
    }

    MediaStreamCenter::instance().didAddMediaStreamTrack(m_descriptor.get(), track->component());
}

void MediaStream::removeTrack(MediaStreamTrack* track, ExceptionState& exceptionState)
{
    if (!track) {
        exceptionState.throwDOMException(TypeMismatchError, "The MediaStreamTrack provided is invalid.");
        return;
    }

    size_t pos = kNotFound;
    switch (track->component()->source()->type()) {
    case MediaStreamSource::TypeAudio:
        pos = m_audioTracks.find(track);
        if (pos != kNotFound)
            m_audioTracks.remove(pos);
        break;
    case MediaStreamSource::TypeVideo:
        pos = m_videoTracks.find(track);
        if (pos != kNotFound)
            m_videoTracks.remove(pos);
        break;
    }

    if (pos == kNotFound)
        return;
    track->unregisterMediaStream(this);
    m_descriptor->removeComponent(track->component());

    if (active() && emptyOrOnlyEndedTracks()) {
        m_descriptor->setActive(false);
        scheduleDispatchEvent(Event::create(EventTypeNames::inactive));
    }

    MediaStreamCenter::instance().didRemoveMediaStreamTrack(m_descriptor.get(), track->component());
}

MediaStreamTrack* MediaStream::getTrackById(String id)
{
    for (MediaStreamTrackVector::iterator iter = m_audioTracks.begin(); iter != m_audioTracks.end(); ++iter) {
        if ((*iter)->id() == id)
            return iter->get();
    }

    for (MediaStreamTrackVector::iterator iter = m_videoTracks.begin(); iter != m_videoTracks.end(); ++iter) {
        if ((*iter)->id() == id)
            return iter->get();
    }

    return 0;
}

MediaStream* MediaStream::clone(ExecutionContext* context)
{
    MediaStreamTrackVector tracks;
    for (MediaStreamTrackVector::iterator iter = m_audioTracks.begin(); iter != m_audioTracks.end(); ++iter)
        tracks.append((*iter)->clone(context));
    for (MediaStreamTrackVector::iterator iter = m_videoTracks.begin(); iter != m_videoTracks.end(); ++iter)
        tracks.append((*iter)->clone(context));
    return MediaStream::create(context, tracks);
}

void MediaStream::stop()
{
    if (ended())
        return;

    streamEnded();

    MediaStreamCenter::instance().didStopLocalMediaStream(descriptor());
}

void MediaStream::trackEnded()
{
    for (MediaStreamTrackVector::iterator iter = m_audioTracks.begin(); iter != m_audioTracks.end(); ++iter) {
        if (!(*iter)->ended())
            return;
    }

    for (MediaStreamTrackVector::iterator iter = m_videoTracks.begin(); iter != m_videoTracks.end(); ++iter) {
        if (!(*iter)->ended())
            return;
    }

    streamEnded();
}

void MediaStream::streamEnded()
{
    if (ended())
        return;

    if (active()) {
        m_descriptor->setActive(false);
        scheduleDispatchEvent(Event::create(EventTypeNames::inactive));
    }
    m_descriptor->setEnded();
    scheduleDispatchEvent(Event::create(EventTypeNames::ended));
}

void MediaStream::contextDestroyed()
{
    ContextLifecycleObserver::contextDestroyed();
    m_stopped = true;
}

const AtomicString& MediaStream::interfaceName() const
{
    return EventTargetNames::MediaStream;
}

ExecutionContext* MediaStream::executionContext() const
{
    return ContextLifecycleObserver::executionContext();
}

void MediaStream::addRemoteTrack(MediaStreamComponent* component)
{
    ASSERT(component);
    if (ended())
        return;

    MediaStreamTrack* track = MediaStreamTrack::create(executionContext(), component);
    switch (component->source()->type()) {
    case MediaStreamSource::TypeAudio:
        m_audioTracks.append(track);
        break;
    case MediaStreamSource::TypeVideo:
        m_videoTracks.append(track);
        break;
    }
    track->registerMediaStream(this);
    m_descriptor->addComponent(component);

    scheduleDispatchEvent(MediaStreamTrackEvent::create(EventTypeNames::addtrack, false, false, track));

    if (!active() && !track->ended()) {
        m_descriptor->setActive(true);
        scheduleDispatchEvent(Event::create(EventTypeNames::active));
    }
}

void MediaStream::removeRemoteTrack(MediaStreamComponent* component)
{
    ASSERT(component);
    if (m_stopped)
        return;

    MediaStreamTrackVector* tracks = 0;
    switch (component->source()->type()) {
    case MediaStreamSource::TypeAudio:
        tracks = &m_audioTracks;
        break;
    case MediaStreamSource::TypeVideo:
        tracks = &m_videoTracks;
        break;
    }

    size_t index = kNotFound;
    for (size_t i = 0; i < tracks->size(); ++i) {
        if ((*tracks)[i]->component() == component) {
            index = i;
            break;
        }
    }
    if (index == kNotFound)
        return;

    m_descriptor->removeComponent(component);

    MediaStreamTrack* track = (*tracks)[index];
    track->unregisterMediaStream(this);
    tracks->remove(index);
    scheduleDispatchEvent(MediaStreamTrackEvent::create(EventTypeNames::removetrack, false, false, track));

    if (active() && emptyOrOnlyEndedTracks()) {
        m_descriptor->setActive(false);
        scheduleDispatchEvent(Event::create(EventTypeNames::inactive));
    }
}

void MediaStream::scheduleDispatchEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    m_scheduledEvents.append(event);

    if (!m_scheduledEventTimer.isActive())
        m_scheduledEventTimer.startOneShot(0, FROM_HERE);
}

void MediaStream::scheduledEventTimerFired(Timer<MediaStream>*)
{
    if (m_stopped)
        return;

    WillBeHeapVector<RefPtrWillBeMember<Event>> events;
    events.swap(m_scheduledEvents);

    WillBeHeapVector<RefPtrWillBeMember<Event>>::iterator it = events.begin();
    for (; it != events.end(); ++it)
        dispatchEvent((*it).release());

    events.clear();
}

URLRegistry& MediaStream::registry() const
{
    return MediaStreamRegistry::registry();
}

DEFINE_TRACE(MediaStream)
{
    visitor->trace(m_audioTracks);
    visitor->trace(m_videoTracks);
    visitor->trace(m_scheduledEvents);
    RefCountedGarbageCollectedEventTargetWithInlineData<MediaStream>::trace(visitor);
    ContextLifecycleObserver::trace(visitor);
}

} // namespace blink
