// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/track/VideoTrackList.h"

#include "core/html/HTMLMediaElement.h"
#include "core/html/track/VideoTrack.h"

namespace blink {

PassRefPtrWillBeRawPtr<VideoTrackList> VideoTrackList::create(HTMLMediaElement& mediaElement)
{
    return adoptRefWillBeNoop(new VideoTrackList(mediaElement));
}

VideoTrackList::~VideoTrackList()
{
}

VideoTrackList::VideoTrackList(HTMLMediaElement& mediaElement)
    : TrackListBase<VideoTrack>(&mediaElement)
{
}

const AtomicString& VideoTrackList::interfaceName() const
{
    return EventTargetNames::VideoTrackList;
}

int VideoTrackList::selectedIndex() const
{
    for (unsigned i = 0; i < length(); ++i) {
        VideoTrack* track = anonymousIndexedGetter(i);

        if (track->selected())
            return i;
    }

    return -1;
}

void VideoTrackList::trackSelected(WebMediaPlayer::TrackId selectedTrackId)
{
    // Clear the selected flag on the previously selected track, if any.
    for (unsigned i = 0; i < length(); ++i) {
        VideoTrack* track = anonymousIndexedGetter(i);

        if (track->trackId() != selectedTrackId)
            track->clearSelected();
        else
            ASSERT(track->selected());
    }

    scheduleChangeEvent();
}

} // namespace blink
