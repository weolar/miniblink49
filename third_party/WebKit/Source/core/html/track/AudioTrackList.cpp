// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/track/AudioTrackList.h"

namespace blink {

PassRefPtrWillBeRawPtr<AudioTrackList> AudioTrackList::create(HTMLMediaElement& mediaElement)
{
    return adoptRefWillBeNoop(new AudioTrackList(mediaElement));
}

AudioTrackList::~AudioTrackList()
{
}

AudioTrackList::AudioTrackList(HTMLMediaElement& mediaElement)
    : TrackListBase<AudioTrack>(&mediaElement)
{
}

bool AudioTrackList::hasEnabledTrack() const
{
    for (unsigned i = 0; i < length(); ++i) {
        if (anonymousIndexedGetter(i)->enabled())
            return true;
    }

    return false;
}

const AtomicString& AudioTrackList::interfaceName() const
{
    return EventTargetNames::AudioTrackList;
}

}
