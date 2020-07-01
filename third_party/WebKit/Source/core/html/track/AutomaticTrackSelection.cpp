// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/track/AutomaticTrackSelection.h"

#include "core/html/track/TextTrack.h"
#include "core/html/track/TextTrackList.h"
#include "platform/Language.h"

namespace blink {

class TrackGroup {
    STACK_ALLOCATED();
public:
    enum GroupKind {
        CaptionsAndSubtitles,
        Description,
        Chapter,
        Metadata
    };

    explicit TrackGroup(GroupKind kind)
        : visibleTrack(nullptr)
        , defaultTrack(nullptr)
        , kind(kind)
        , hasSrcLang(false)
    {
    }

    WillBeHeapVector<RefPtrWillBeMember<TextTrack>> tracks;
    RefPtrWillBeMember<TextTrack> visibleTrack;
    RefPtrWillBeMember<TextTrack> defaultTrack;
    GroupKind kind;
    bool hasSrcLang;
};

static int textTrackLanguageSelectionScore(const TextTrack& track)
{
    if (track.language().isEmpty())
        return 0;

    Vector<AtomicString> languages = userPreferredLanguages();
    size_t languageMatchIndex = indexOfBestMatchingLanguageInList(track.language(), languages);
    if (languageMatchIndex >= languages.size())
        return 0;

    return languages.size() - languageMatchIndex;
}

static int textTrackSelectionScore(const TextTrack& track)
{
    if (track.kind() != TextTrack::captionsKeyword() && track.kind() != TextTrack::subtitlesKeyword())
        return 0;

    return textTrackLanguageSelectionScore(track);
}

AutomaticTrackSelection::AutomaticTrackSelection(const Configuration& configuration)
    : m_configuration(configuration)
{
}

const AtomicString& AutomaticTrackSelection::preferredTrackKind() const
{
    if (m_configuration.textTrackKindUserPreference == TextTrackKindUserPreference::Subtitles)
        return TextTrack::subtitlesKeyword();
    if (m_configuration.textTrackKindUserPreference == TextTrackKindUserPreference::Captions)
        return TextTrack::captionsKeyword();
    return nullAtom;
}

void AutomaticTrackSelection::performAutomaticTextTrackSelection(const TrackGroup& group)
{
    ASSERT(group.tracks.size());

    // First, find the track in the group that should be enabled (if any).
    WillBeHeapVector<RefPtrWillBeMember<TextTrack>> currentlyEnabledTracks;
    RefPtrWillBeRawPtr<TextTrack> trackToEnable = nullptr;
    RefPtrWillBeRawPtr<TextTrack> defaultTrack = nullptr;
    RefPtrWillBeRawPtr<TextTrack> preferredTrack = nullptr;
    RefPtrWillBeRawPtr<TextTrack> fallbackTrack = nullptr;

    int highestTrackScore = 0;

    for (size_t i = 0; i < group.tracks.size(); ++i) {
        RefPtrWillBeRawPtr<TextTrack> textTrack = group.tracks[i];

        if (m_configuration.disableCurrentlyEnabledTracks && textTrack->mode() == TextTrack::showingKeyword())
            currentlyEnabledTracks.append(textTrack);

        int trackScore = textTrackSelectionScore(*textTrack);

        if (textTrack->kind() == preferredTrackKind())
            trackScore += 1;
        if (trackScore) {
            // * If the text track kind is subtitles or captions and the user has indicated an interest in having a
            // track with this text track kind, text track language, and text track label enabled, and there is no
            // other text track in the media element's list of text tracks with a text track kind of either subtitles
            // or captions whose text track mode is showing
            //    Let the text track mode be showing.
            if (trackScore > highestTrackScore) {
                preferredTrack = textTrack;
                highestTrackScore = trackScore;
            }
            if (!defaultTrack && textTrack->isDefault())
                defaultTrack = textTrack;

            if (!fallbackTrack)
                fallbackTrack = textTrack;
        } else if (!group.visibleTrack && !defaultTrack && textTrack->isDefault()) {
            // * If the track element has a default attribute specified, and there is no other text track in the media
            // element's list of text tracks whose text track mode is showing or showing by default
            //    Let the text track mode be showing by default.
            defaultTrack = textTrack;
        }
    }

    if (m_configuration.textTrackKindUserPreference != TextTrackKindUserPreference::Default)
        trackToEnable = preferredTrack;

    if (!trackToEnable && defaultTrack)
        trackToEnable = defaultTrack;

    if (!trackToEnable && m_configuration.forceEnableSubtitleOrCaptionTrack && group.kind == TrackGroup::CaptionsAndSubtitles) {
        if (fallbackTrack) {
            trackToEnable = fallbackTrack;
        } else {
            trackToEnable = group.tracks[0];
        }
    }

    if (currentlyEnabledTracks.size()) {
        for (size_t i = 0; i < currentlyEnabledTracks.size(); ++i) {
            RefPtrWillBeRawPtr<TextTrack> textTrack = currentlyEnabledTracks[i];
            if (textTrack != trackToEnable)
                textTrack->setMode(TextTrack::disabledKeyword());
        }
    }

    if (trackToEnable)
        trackToEnable->setMode(TextTrack::showingKeyword());
}

void AutomaticTrackSelection::enableDefaultMetadataTextTracks(const TrackGroup& group)
{
    ASSERT(group.tracks.size());

    // https://html.spec.whatwg.org/multipage/embedded-content.html#honor-user-preferences-for-automatic-text-track-selection

    // 4. If there are any text tracks in the media element's list of text
    // tracks whose text track kind is metadata that correspond to track
    // elements with a default attribute set whose text track mode is set to
    // disabled, then set the text track mode of all such tracks to hidden
    for (auto& textTrack : group.tracks) {
        if (textTrack->mode() != TextTrack::disabledKeyword())
            continue;
        if (!textTrack->isDefault())
            continue;
        textTrack->setMode(TextTrack::hiddenKeyword());
    }
}

void AutomaticTrackSelection::perform(TextTrackList& textTracks)
{
    TrackGroup captionAndSubtitleTracks(TrackGroup::CaptionsAndSubtitles);
    TrackGroup descriptionTracks(TrackGroup::Description);
    TrackGroup chapterTracks(TrackGroup::Chapter);
    TrackGroup metadataTracks(TrackGroup::Metadata);

    for (size_t i = 0; i < textTracks.length(); ++i) {
        RefPtrWillBeRawPtr<TextTrack> textTrack = textTracks.item(i);
        if (!textTrack)
            continue;

        String kind = textTrack->kind();
        TrackGroup* currentGroup;
        if (kind == TextTrack::subtitlesKeyword() || kind == TextTrack::captionsKeyword()) {
            currentGroup = &captionAndSubtitleTracks;
        } else if (kind == TextTrack::descriptionsKeyword()) {
            currentGroup = &descriptionTracks;
        } else if (kind == TextTrack::chaptersKeyword()) {
            currentGroup = &chapterTracks;
        } else {
            ASSERT(kind == TextTrack::metadataKeyword());
            currentGroup = &metadataTracks;
        }

        if (!currentGroup->visibleTrack && textTrack->mode() == TextTrack::showingKeyword())
            currentGroup->visibleTrack = textTrack;
        if (!currentGroup->defaultTrack && textTrack->isDefault())
            currentGroup->defaultTrack = textTrack;

        // Do not add this track to the group if it has already been automatically configured
        // as we only want to perform selection once per track so that adding another track
        // after the initial configuration doesn't reconfigure every track - only those that
        // should be changed by the new addition. For example all metadata tracks are
        // disabled by default, and we don't want a track that has been enabled by script
        // to be disabled automatically when a new metadata track is added later.
        if (textTrack->hasBeenConfigured())
            continue;

        if (textTrack->language().length())
            currentGroup->hasSrcLang = true;
        currentGroup->tracks.append(textTrack);
    }

    if (captionAndSubtitleTracks.tracks.size())
        performAutomaticTextTrackSelection(captionAndSubtitleTracks);
    if (descriptionTracks.tracks.size())
        performAutomaticTextTrackSelection(descriptionTracks);
    if (chapterTracks.tracks.size())
        performAutomaticTextTrackSelection(chapterTracks);
    if (metadataTracks.tracks.size())
        enableDefaultMetadataTextTracks(metadataTracks);
}

} // namespace blink
