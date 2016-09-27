/*
 * Copyright (C) 2011, 2012 Apple Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/track/TextTrackList.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/events/GenericEventQueue.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/track/InbandTextTrack.h"
#include "core/html/track/LoadableTextTrack.h"
#include "core/html/track/TextTrack.h"
#include "core/html/track/TrackEvent.h"

using namespace blink;

TextTrackList::TextTrackList(HTMLMediaElement* owner)
    : m_owner(owner)
    , m_asyncEventQueue(GenericEventQueue::create(this))
{
}

TextTrackList::~TextTrackList()
{
#if !ENABLE(OILPAN)
    ASSERT(!m_owner);

    // TextTrackList and m_asyncEventQueue always become unreachable
    // together. So TextTrackList and m_asyncEventQueue are destructed in the
    // same GC. We don't need to close it explicitly in Oilpan.
    m_asyncEventQueue->close();

    for (unsigned i = 0; i < length(); ++i) {
        item(i)->setTrackList(0);
    }
#endif
}

unsigned TextTrackList::length() const
{
    return m_addTrackTracks.size() + m_elementTracks.size() + m_inbandTracks.size();
}

int TextTrackList::getTrackIndex(TextTrack *textTrack)
{
    if (textTrack->trackType() == TextTrack::TrackElement)
        return static_cast<LoadableTextTrack*>(textTrack)->trackElementIndex();

    if (textTrack->trackType() == TextTrack::AddTrack)
        return m_elementTracks.size() + m_addTrackTracks.find(textTrack);

    if (textTrack->trackType() == TextTrack::InBand)
        return m_elementTracks.size() + m_addTrackTracks.size() + m_inbandTracks.find(textTrack);

    ASSERT_NOT_REACHED();

    return -1;
}

int TextTrackList::getTrackIndexRelativeToRenderedTracks(TextTrack *textTrack)
{
    // Calculate the "Let n be the number of text tracks whose text track mode is showing and that are in the media element's list of text tracks before track."
    int trackIndex = 0;

    for (size_t i = 0; i < m_elementTracks.size(); ++i) {
        if (!m_elementTracks[i]->isRendered())
            continue;

        if (m_elementTracks[i] == textTrack)
            return trackIndex;
        ++trackIndex;
    }

    for (size_t i = 0; i < m_addTrackTracks.size(); ++i) {
        if (!m_addTrackTracks[i]->isRendered())
            continue;

        if (m_addTrackTracks[i] == textTrack)
            return trackIndex;
        ++trackIndex;
    }

    for (size_t i = 0; i < m_inbandTracks.size(); ++i) {
        if (!m_inbandTracks[i]->isRendered())
            continue;

        if (m_inbandTracks[i] == textTrack)
            return trackIndex;
        ++trackIndex;
    }

    ASSERT_NOT_REACHED();

    return -1;
}

TextTrack* TextTrackList::item(unsigned index)
{
    // 4.8.10.12.1 Text track model
    // The text tracks are sorted as follows:
    // 1. The text tracks corresponding to track element children of the media element, in tree order.
    // 2. Any text tracks added using the addTextTrack() method, in the order they were added, oldest first.
    // 3. Any media-resource-specific text tracks (text tracks corresponding to data in the media
    // resource), in the order defined by the media resource's format specification.

    if (index < m_elementTracks.size())
        return m_elementTracks[index].get();

    index -= m_elementTracks.size();
    if (index < m_addTrackTracks.size())
        return m_addTrackTracks[index].get();

    index -= m_addTrackTracks.size();
    if (index < m_inbandTracks.size())
        return m_inbandTracks[index].get();

    return 0;
}

TextTrack* TextTrackList::getTrackById(const AtomicString& id)
{
    // 4.8.10.12.5 Text track API
    // The getTrackById(id) method must return the first TextTrack in the
    // TextTrackList object whose id IDL attribute would return a value equal
    // to the value of the id argument.
    for (unsigned i = 0; i < length(); ++i) {
        TextTrack* track = item(i);
        if (track->id() == id)
            return track;
    }

    // When no tracks match the given argument, the method must return null.
    return 0;
}

void TextTrackList::invalidateTrackIndexesAfterTrack(TextTrack* track)
{
    WillBeHeapVector<RefPtrWillBeMember<TextTrack>>* tracks = 0;

    if (track->trackType() == TextTrack::TrackElement) {
        tracks = &m_elementTracks;
        for (size_t i = 0; i < m_addTrackTracks.size(); ++i)
            m_addTrackTracks[i]->invalidateTrackIndex();
        for (size_t i = 0; i < m_inbandTracks.size(); ++i)
            m_inbandTracks[i]->invalidateTrackIndex();
    } else if (track->trackType() == TextTrack::AddTrack) {
        tracks = &m_addTrackTracks;
        for (size_t i = 0; i < m_inbandTracks.size(); ++i)
            m_inbandTracks[i]->invalidateTrackIndex();
    } else if (track->trackType() == TextTrack::InBand) {
        tracks = &m_inbandTracks;
    } else {
        ASSERT_NOT_REACHED();
    }

    size_t index = tracks->find(track);
    if (index == kNotFound)
        return;

    for (size_t i = index; i < tracks->size(); ++i)
        tracks->at(index)->invalidateTrackIndex();
}

void TextTrackList::append(PassRefPtrWillBeRawPtr<TextTrack> prpTrack)
{
    RefPtrWillBeRawPtr<TextTrack> track = prpTrack;

    if (track->trackType() == TextTrack::AddTrack) {
        m_addTrackTracks.append(track);
    } else if (track->trackType() == TextTrack::TrackElement) {
        // Insert tracks added for <track> element in tree order.
        size_t index = static_cast<LoadableTextTrack*>(track.get())->trackElementIndex();
        m_elementTracks.insert(index, track);
    } else if (track->trackType() == TextTrack::InBand) {
        m_inbandTracks.append(track);
    } else {
        ASSERT_NOT_REACHED();
    }

    invalidateTrackIndexesAfterTrack(track.get());

    ASSERT(!track->trackList());
    track->setTrackList(this);

    scheduleAddTrackEvent(track.release());
}

void TextTrackList::remove(TextTrack* track)
{
    WillBeHeapVector<RefPtrWillBeMember<TextTrack>>* tracks = 0;

    if (track->trackType() == TextTrack::TrackElement) {
        tracks = &m_elementTracks;
    } else if (track->trackType() == TextTrack::AddTrack) {
        tracks = &m_addTrackTracks;
    } else if (track->trackType() == TextTrack::InBand) {
        tracks = &m_inbandTracks;
    } else {
        ASSERT_NOT_REACHED();
    }

    size_t index = tracks->find(track);
    if (index == kNotFound)
        return;

    invalidateTrackIndexesAfterTrack(track);

    ASSERT(track->trackList() == this);
    track->setTrackList(0);

    tracks->remove(index);

    scheduleRemoveTrackEvent(track);
}

void TextTrackList::removeAllInbandTracks()
{
    for (unsigned i = 0; i < m_inbandTracks.size(); ++i) {
        m_inbandTracks[i]->setTrackList(0);
    }
    m_inbandTracks.clear();
}

bool TextTrackList::contains(TextTrack* track) const
{
    const WillBeHeapVector<RefPtrWillBeMember<TextTrack>>* tracks = 0;

    if (track->trackType() == TextTrack::TrackElement)
        tracks = &m_elementTracks;
    else if (track->trackType() == TextTrack::AddTrack)
        tracks = &m_addTrackTracks;
    else if (track->trackType() == TextTrack::InBand)
        tracks = &m_inbandTracks;
    else
        ASSERT_NOT_REACHED();

    return tracks->find(track) != kNotFound;
}

const AtomicString& TextTrackList::interfaceName() const
{
    return EventTargetNames::TextTrackList;
}

ExecutionContext* TextTrackList::executionContext() const
{
    return m_owner ? m_owner->executionContext() : 0;
}

#if !ENABLE(OILPAN)
void TextTrackList::clearOwner()
{
    m_owner = nullptr;
}
#endif

void TextTrackList::scheduleTrackEvent(const AtomicString& eventName, PassRefPtrWillBeRawPtr<TextTrack> track)
{
    m_asyncEventQueue->enqueueEvent(TrackEvent::create(eventName, track));
}

void TextTrackList::scheduleAddTrackEvent(PassRefPtrWillBeRawPtr<TextTrack> track)
{
    // 4.8.10.12.3 Sourcing out-of-band text tracks
    // 4.8.10.12.4 Text track API
    // ... then queue a task to fire an event with the name addtrack, that does not
    // bubble and is not cancelable, and that uses the TrackEvent interface, with
    // the track attribute initialized to the text track's TextTrack object, at
    // the media element's textTracks attribute's TextTrackList object.
    scheduleTrackEvent(EventTypeNames::addtrack, track);
}

void TextTrackList::scheduleChangeEvent()
{
    // 4.8.10.12.1 Text track model
    // Whenever a text track that is in a media element's list of text tracks
    // has its text track mode change value, the user agent must run the
    // following steps for the media element:
    // ...
    // Fire a simple event named change at the media element's textTracks
    // attribute's TextTrackList object.

    m_asyncEventQueue->enqueueEvent(Event::create(EventTypeNames::change));
}

void TextTrackList::scheduleRemoveTrackEvent(PassRefPtrWillBeRawPtr<TextTrack> track)
{
    // 4.8.10.12.3 Sourcing out-of-band text tracks
    // When a track element's parent element changes and the old parent was a
    // media element, then the user agent must remove the track element's
    // corresponding text track from the media element's list of text tracks,
    // and then queue a task to fire a trusted event with the name removetrack,
    // that does not bubble and is not cancelable, and that uses the TrackEvent
    // interface, with the track attribute initialized to the text track's
    // TextTrack object, at the media element's textTracks attribute's
    // TextTrackList object.
    scheduleTrackEvent(EventTypeNames::removetrack, track);
}

bool TextTrackList::hasShowingTracks()
{
    for (unsigned i = 0; i < length(); ++i) {
        if (item(i)->mode() == TextTrack::showingKeyword())
            return true;
    }
    return false;
}

HTMLMediaElement* TextTrackList::owner() const
{
    return m_owner;
}

DEFINE_TRACE(TextTrackList)
{
    visitor->trace(m_owner);
    visitor->trace(m_asyncEventQueue);
    visitor->trace(m_addTrackTracks);
    visitor->trace(m_elementTracks);
    visitor->trace(m_inbandTracks);
    EventTargetWithInlineData::trace(visitor);
}
