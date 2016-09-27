// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VideoTrackList_h
#define VideoTrackList_h

#include "core/html/track/TrackListBase.h"
#include "core/html/track/VideoTrack.h"

namespace blink {

class VideoTrackList final : public TrackListBase<VideoTrack> {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<VideoTrackList> create(HTMLMediaElement&);

    ~VideoTrackList() override;

    int selectedIndex() const;

    // EventTarget
    const AtomicString& interfaceName() const override;

    void trackSelected(WebMediaPlayer::TrackId selectedTrackId);

private:
    explicit VideoTrackList(HTMLMediaElement&);
};

} // namespace blink

#endif // VideoTrackList_h
