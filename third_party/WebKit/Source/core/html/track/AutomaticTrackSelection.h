// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AutomaticTrackSelection_h
#define AutomaticTrackSelection_h

#include "core/html/track/TextTrackKindUserPreference.h"
#include "platform/heap/Handle.h"

namespace blink {

class TextTrackList;
class TrackGroup;

class AutomaticTrackSelection {
    STACK_ALLOCATED();
public:
    struct Configuration {
        Configuration()
            : disableCurrentlyEnabledTracks(false)
            , forceEnableSubtitleOrCaptionTrack(false)
            , textTrackKindUserPreference(TextTrackKindUserPreference::Default) { }

        bool disableCurrentlyEnabledTracks;
        bool forceEnableSubtitleOrCaptionTrack;
        TextTrackKindUserPreference textTrackKindUserPreference;
    };

    AutomaticTrackSelection(const Configuration&);

    void perform(TextTrackList&);

private:
    void performAutomaticTextTrackSelection(const TrackGroup&);
    void enableDefaultMetadataTextTracks(const TrackGroup&);
    const AtomicString& preferredTrackKind() const;

    const Configuration m_configuration;
};

} // namespace blink

#endif // AutomaticTrackSelection_h
