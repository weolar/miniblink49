/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 * Copyright (C) 2011, 2012, 2013 Apple Inc.  All rights reserved.
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
#include "core/html/track/TextTrack.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/ExceptionCode.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/track/CueTimeline.h"
#include "core/html/track/TextTrackCueList.h"
#include "core/html/track/TextTrackList.h"
#include "core/html/track/vtt/VTTRegion.h"
#include "core/html/track/vtt/VTTRegionList.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

static const int invalidTrackIndex = -1;

const AtomicString& TextTrack::subtitlesKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("subtitles", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& TextTrack::captionsKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, captions, ("captions", AtomicString::ConstructFromLiteral));
    return captions;
}

const AtomicString& TextTrack::descriptionsKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, descriptions, ("descriptions", AtomicString::ConstructFromLiteral));
    return descriptions;
}

const AtomicString& TextTrack::chaptersKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, chapters, ("chapters", AtomicString::ConstructFromLiteral));
    return chapters;
}

const AtomicString& TextTrack::metadataKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, metadata, ("metadata", AtomicString::ConstructFromLiteral));
    return metadata;
}

const AtomicString& TextTrack::disabledKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, open, ("disabled", AtomicString::ConstructFromLiteral));
    return open;
}

const AtomicString& TextTrack::hiddenKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, closed, ("hidden", AtomicString::ConstructFromLiteral));
    return closed;
}

const AtomicString& TextTrack::showingKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, ended, ("showing", AtomicString::ConstructFromLiteral));
    return ended;
}

TextTrack::TextTrack(const AtomicString& kind, const AtomicString& label, const AtomicString& language, const AtomicString& id, TextTrackType type)
    : TrackBase(TrackBase::TextTrack, label, language, id)
    , m_cues(nullptr)
    , m_regions(nullptr)
    , m_trackList(nullptr)
    , m_mode(disabledKeyword())
    , m_trackType(type)
    , m_readinessState(NotLoaded)
    , m_trackIndex(invalidTrackIndex)
    , m_renderedTrackIndex(invalidTrackIndex)
    , m_hasBeenConfigured(false)
{
    setKind(kind);
}

TextTrack::~TextTrack()
{
#if !ENABLE(OILPAN)
    ASSERT(!m_trackList);

    if (m_cues) {
        for (size_t i = 0; i < m_cues->length(); ++i)
            m_cues->item(i)->setTrack(0);
    }
    if (m_regions) {
        for (size_t i = 0; i < m_regions->length(); ++i)
            m_regions->item(i)->setTrack(0);
    }
#endif
}

bool TextTrack::isValidKindKeyword(const String& value)
{
    if (value == subtitlesKeyword())
        return true;
    if (value == captionsKeyword())
        return true;
    if (value == descriptionsKeyword())
        return true;
    if (value == chaptersKeyword())
        return true;
    if (value == metadataKeyword())
        return true;

    return false;
}

void TextTrack::setTrackList(TextTrackList* trackList)
{
    if (!trackList && cueTimeline() && m_cues)
        cueTimeline()->removeCues(this, m_cues.get());

    m_trackList = trackList;
    invalidateTrackIndex();
}

void TextTrack::setKind(const AtomicString& newKind)
{
    AtomicString oldKind = kind();
    TrackBase::setKind(newKind);

    // If kind changes from visual to non-visual and mode is 'showing', then force mode to 'hidden'.
    // FIXME: This is not per spec. crbug.com/460923
    if (oldKind != kind() && mode() == showingKeyword()) {
        if (kind() != captionsKeyword() && kind() != subtitlesKeyword())
            setMode(hiddenKeyword());
    }
}

void TextTrack::setMode(const AtomicString& mode)
{
    ASSERT(mode == disabledKeyword() || mode == hiddenKeyword() || mode == showingKeyword());

    // On setting, if the new value isn't equal to what the attribute would currently
    // return, the new value must be processed as follows ...
    if (m_mode == mode)
        return;

    if (m_cues && cueTimeline()) {
        // If mode changes to disabled, remove this track's cues from the client
        // because they will no longer be accessible from the cues() function.
        if (mode == disabledKeyword())
            cueTimeline()->removeCues(this, m_cues.get());
        else if (mode != showingKeyword())
            cueTimeline()->hideCues(this, m_cues.get());
    }

    m_mode = mode;

    if (mode != disabledKeyword() && readinessState() == Loaded) {
        if (m_cues && cueTimeline())
            cueTimeline()->addCues(this, m_cues.get());
    }

    if (mediaElement())
        mediaElement()->textTrackModeChanged(this);
}

TextTrackCueList* TextTrack::cues()
{
    // 4.8.10.12.5 If the text track mode ... is not the text track disabled mode,
    // then the cues attribute must return a live TextTrackCueList object ...
    // Otherwise, it must return null. When an object is returned, the
    // same object must be returned each time.
    // http://www.whatwg.org/specs/web-apps/current-work/#dom-texttrack-cues
    if (m_mode != disabledKeyword())
        return ensureTextTrackCueList();
    return nullptr;
}

void TextTrack::removeAllCues()
{
    if (!m_cues)
        return;

    if (cueTimeline())
        cueTimeline()->removeCues(this, m_cues.get());

    for (size_t i = 0; i < m_cues->length(); ++i)
        m_cues->item(i)->setTrack(0);

    m_cues = nullptr;
}

void TextTrack::addListOfCues(WillBeHeapVector<RefPtrWillBeMember<TextTrackCue>>& listOfNewCues)
{
    TextTrackCueList* cues = ensureTextTrackCueList();

    for (auto& newCue : listOfNewCues) {
        newCue->setTrack(this);
        cues->add(newCue.release());
    }

    if (cueTimeline() && mode() != disabledKeyword())
        cueTimeline()->addCues(this, cues);
}

TextTrackCueList* TextTrack::activeCues()
{
    // 4.8.10.12.5 If the text track mode ... is not the text track disabled mode,
    // then the activeCues attribute must return a live TextTrackCueList object ...
    // ... whose active flag was set when the script started, in text track cue
    // order. Otherwise, it must return null. When an object is returned, the
    // same object must be returned each time.
    // http://www.whatwg.org/specs/web-apps/current-work/#dom-texttrack-activecues
    if (!m_cues || m_mode == disabledKeyword())
        return nullptr;

    if (!m_activeCues)
        m_activeCues = TextTrackCueList::create();

    m_cues->collectActiveCues(*m_activeCues);
    return m_activeCues.get();
}

void TextTrack::addCue(PassRefPtrWillBeRawPtr<TextTrackCue> prpCue)
{
    ASSERT(prpCue);
    RefPtrWillBeRawPtr<TextTrackCue> cue = prpCue;

    // TODO(93143): Add spec-compliant behavior for negative time values.
    if (std::isnan(cue->startTime()) || std::isnan(cue->endTime()) || cue->startTime() < 0 || cue->endTime() < 0)
        return;

    // https://html.spec.whatwg.org/multipage/embedded-content.html#dom-texttrack-addcue

    // The addCue(cue) method of TextTrack objects, when invoked, must run the following steps:

    // (Steps 1 and 2 - pertaining to association of rendering rules - are not implemented.)

    // 3. If the given cue is in a text track list of cues, then remove cue
    // from that text track list of cues.
    if (TextTrack* cueTrack = cue->track())
        cueTrack->removeCue(cue.get(), ASSERT_NO_EXCEPTION);

    // 4. Add cue to the method's TextTrack object's text track's text track list of cues.
    cue->setTrack(this);
    ensureTextTrackCueList()->add(cue);

    if (cueTimeline() && m_mode != disabledKeyword())
        cueTimeline()->addCue(this, cue.get());
}

void TextTrack::removeCue(TextTrackCue* cue, ExceptionState& exceptionState)
{
    ASSERT(cue);

    // https://html.spec.whatwg.org/multipage/embedded-content.html#dom-texttrack-removecue

    // The removeCue(cue) method of TextTrack objects, when invoked, must run the following steps:

    // 1. If the given cue is not currently listed in the method's TextTrack
    // object's text track's text track list of cues, then throw a NotFoundError exception.
    if (cue->track() != this) {
        exceptionState.throwDOMException(NotFoundError, "The specified cue is not listed in the TextTrack's list of cues.");
        return;
    }

    // cue->track() == this implies that cue is in this track's list of cues,
    // so this track should have a list of cues and the cue being removed
    // should be in it.
    ASSERT(m_cues);

    // 2. Remove cue from the method's TextTrack object's text track's text track list of cues.
    bool wasRemoved = m_cues->remove(cue);
    ASSERT_UNUSED(wasRemoved, wasRemoved);

    // If the cue is active, a timeline needs to be available.
    ASSERT(!cue->isActive() || cueTimeline());

    cue->setTrack(0);

    if (cueTimeline())
        cueTimeline()->removeCue(this, cue);
}

VTTRegionList* TextTrack::ensureVTTRegionList()
{
    if (!m_regions)
        m_regions = VTTRegionList::create();

    return m_regions.get();
}

VTTRegionList* TextTrack::regions()
{
    // If the text track mode of the text track that the TextTrack object
    // represents is not the text track disabled mode, then the regions
    // attribute must return a live VTTRegionList object that represents
    // the text track list of regions of the text track. Otherwise, it must
    // return null. When an object is returned, the same object must be returned
    // each time.
    if (RuntimeEnabledFeatures::webVTTRegionsEnabled() && m_mode != disabledKeyword())
        return ensureVTTRegionList();
    return nullptr;
}

void TextTrack::addRegion(PassRefPtrWillBeRawPtr<VTTRegion> prpRegion)
{
    if (!prpRegion)
        return;

    RefPtrWillBeRawPtr<VTTRegion> region = prpRegion;
    VTTRegionList* regionList = ensureVTTRegionList();

    // 1. If the given region is in a text track list of regions, then remove
    // region from that text track list of regions.
    TextTrack* regionTrack = region->track();
    if (regionTrack && regionTrack != this)
        regionTrack->removeRegion(region.get(), ASSERT_NO_EXCEPTION);

    // 2. If the method's TextTrack object's text track list of regions contains
    // a region with the same identifier as region replace the values of that
    // region's width, height, anchor point, viewport anchor point and scroll
    // attributes with those of region.
    VTTRegion* existingRegion = regionList->getRegionById(region->id());
    if (existingRegion) {
        existingRegion->updateParametersFromRegion(region.get());
        return;
    }

    // Otherwise: add region to the method's TextTrack object's text track
    // list of regions.
    region->setTrack(this);
    regionList->add(region);
}

void TextTrack::removeRegion(VTTRegion* region, ExceptionState &exceptionState)
{
    if (!region)
        return;

    // 1. If the given region is not currently listed in the method's TextTrack
    // object's text track list of regions, then throw a NotFoundError exception.
    if (region->track() != this) {
        exceptionState.throwDOMException(NotFoundError, "The specified region is not listed in the TextTrack's list of regions.");
        return;
    }

    if (!m_regions || !m_regions->remove(region)) {
        exceptionState.throwDOMException(InvalidStateError, "Failed to remove the specified region.");
        return;
    }

    region->setTrack(0);
}

void TextTrack::cueWillChange(TextTrackCue* cue)
{
    // The cue may need to be repositioned in the media element's interval tree, may need to
    // be re-rendered, etc, so remove it before the modification...
    if (cueTimeline())
        cueTimeline()->removeCue(this, cue);
}

void TextTrack::cueDidChange(TextTrackCue* cue)
{
    // This method is called through cue->track(), which should imply that this
    // track has a list of cues.
    ASSERT(m_cues && cue->track() == this);

    // Make sure the TextTrackCueList order is up-to-date.
    // FIXME: Only need to do this if the change was to any of the timestamps.
    m_cues->updateCueIndex(cue);

    // Since a call to cueDidChange is always preceded by a call to
    // cueWillChange, the cue should no longer be active when we reach this
    // point (since it was removed from the timeline in cueWillChange).
    ASSERT(!cue->isActive());

    if (m_mode == disabledKeyword())
        return;

    // ... and add it back again if the track is enabled.
    if (cueTimeline())
        cueTimeline()->addCue(this, cue);
}

int TextTrack::trackIndex()
{
    ASSERT(m_trackList);

    if (m_trackIndex == invalidTrackIndex)
        m_trackIndex = m_trackList->getTrackIndex(this);

    return m_trackIndex;
}

void TextTrack::invalidateTrackIndex()
{
    m_trackIndex = invalidTrackIndex;
    m_renderedTrackIndex = invalidTrackIndex;
}

bool TextTrack::isRendered()
{
    if (kind() != captionsKeyword() && kind() != subtitlesKeyword())
        return false;

    if (m_mode != showingKeyword())
        return false;

    return true;
}

TextTrackCueList* TextTrack::ensureTextTrackCueList()
{
    if (!m_cues)
        m_cues = TextTrackCueList::create();

    return m_cues.get();
}

int TextTrack::trackIndexRelativeToRenderedTracks()
{
    ASSERT(m_trackList);

    if (m_renderedTrackIndex == invalidTrackIndex)
        m_renderedTrackIndex = m_trackList->getTrackIndexRelativeToRenderedTracks(this);

    return m_renderedTrackIndex;
}

const AtomicString& TextTrack::interfaceName() const
{
    return EventTargetNames::TextTrack;
}

ExecutionContext* TextTrack::executionContext() const
{
    HTMLMediaElement* owner = mediaElement();
    return owner ? owner->executionContext() : 0;
}

HTMLMediaElement* TextTrack::mediaElement() const
{
    return m_trackList ? m_trackList->owner() : 0;
}

CueTimeline* TextTrack::cueTimeline() const
{
    return mediaElement() ? &mediaElement()->cueTimeline() : nullptr;
}

Node* TextTrack::owner() const
{
    return mediaElement();
}

DEFINE_TRACE(TextTrack)
{
    visitor->trace(m_cues);
    visitor->trace(m_activeCues);
    visitor->trace(m_regions);
    visitor->trace(m_trackList);
    TrackBase::trace(visitor);
    EventTargetWithInlineData::trace(visitor);
}

} // namespace blink
