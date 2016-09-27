/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011, 2012, 2013 Apple Inc.  All rights reserved.
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

#ifndef TextTrack_h
#define TextTrack_h

#include "core/CoreExport.h"
#include "core/events/EventTarget.h"
#include "core/html/track/TrackBase.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class CueTimeline;
class ExceptionState;
class HTMLMediaElement;
class TextTrack;
class TextTrackCue;
class TextTrackCueList;
class TextTrackList;
class VTTRegion;
class VTTRegionList;

class CORE_EXPORT TextTrack : public EventTargetWithInlineData, public TrackBase {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_EVENT_TARGET(TrackBase);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(TextTrack);
public:
    static PassRefPtrWillBeRawPtr<TextTrack> create(const AtomicString& kind, const AtomicString& label, const AtomicString& language)
    {
        return adoptRefWillBeNoop(new TextTrack(kind, label, language, emptyAtom, AddTrack));
    }
    ~TextTrack() override;

    virtual void setTrackList(TextTrackList*);
    TextTrackList* trackList() { return m_trackList; }

    void setKind(const AtomicString&) override;

    static const AtomicString& subtitlesKeyword();
    static const AtomicString& captionsKeyword();
    static const AtomicString& descriptionsKeyword();
    static const AtomicString& chaptersKeyword();
    static const AtomicString& metadataKeyword();
    static bool isValidKindKeyword(const String&);

    static const AtomicString& disabledKeyword();
    static const AtomicString& hiddenKeyword();
    static const AtomicString& showingKeyword();

    AtomicString mode() const { return m_mode; }
    virtual void setMode(const AtomicString&);

    enum ReadinessState { NotLoaded = 0, Loading = 1, Loaded = 2, FailedToLoad = 3 };
    ReadinessState readinessState() const { return m_readinessState; }
    void setReadinessState(ReadinessState state) { m_readinessState = state; }

    TextTrackCueList* cues();
    TextTrackCueList* activeCues();

    HTMLMediaElement* mediaElement() const;
    Node* owner() const;

    void addCue(PassRefPtrWillBeRawPtr<TextTrackCue>);
    void removeCue(TextTrackCue*, ExceptionState&);

    VTTRegionList* regions();
    void addRegion(PassRefPtrWillBeRawPtr<VTTRegion>);
    void removeRegion(VTTRegion*, ExceptionState&);

    void cueWillChange(TextTrackCue*);
    void cueDidChange(TextTrackCue*);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(cuechange);

    enum TextTrackType { TrackElement, AddTrack, InBand };
    TextTrackType trackType() const { return m_trackType; }

    int trackIndex();
    void invalidateTrackIndex();

    bool isRendered();
    int trackIndexRelativeToRenderedTracks();

    bool hasBeenConfigured() const { return m_hasBeenConfigured; }
    void setHasBeenConfigured(bool flag) { m_hasBeenConfigured = flag; }

    virtual bool isDefault() const { return false; }

    void removeAllCues();

    // EventTarget methods
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    DECLARE_VIRTUAL_TRACE();

protected:
    TextTrack(const AtomicString& kind, const AtomicString& label, const AtomicString& language, const AtomicString& id, TextTrackType);

    bool isValidKind(const AtomicString& kind) const override { return isValidKindKeyword(kind); }
    AtomicString defaultKind() const override { return subtitlesKeyword(); }

    void addListOfCues(WillBeHeapVector<RefPtrWillBeMember<TextTrackCue>>&);

private:
    CueTimeline* cueTimeline() const;

    TextTrackCueList* ensureTextTrackCueList();
    RefPtrWillBeMember<TextTrackCueList> m_cues;
    RefPtrWillBeMember<TextTrackCueList> m_activeCues;

    VTTRegionList* ensureVTTRegionList();
    RefPtrWillBeMember<VTTRegionList> m_regions;

    RawPtrWillBeMember<TextTrackList> m_trackList;
    AtomicString m_mode;
    TextTrackType m_trackType;
    ReadinessState m_readinessState;
    int m_trackIndex;
    int m_renderedTrackIndex;
    bool m_hasBeenConfigured;
};

DEFINE_TRACK_TYPE_CASTS(TextTrack, TrackBase::TextTrack);

} // namespace blink

#endif // TextTrack_h
