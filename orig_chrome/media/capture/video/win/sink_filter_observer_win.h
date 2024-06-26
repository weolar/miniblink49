// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Observer class of Sinkfilter. The implementor of this class receive video
// frames from the SinkFilter DirectShow filter.

#ifndef MEDIA_VIDEO_CAPTURE_WIN_SINK_FILTER_OBSERVER_WIN_H_
#define MEDIA_VIDEO_CAPTURE_WIN_SINK_FILTER_OBSERVER_WIN_H_

namespace media {

class SinkFilterObserver {
public:
    // SinkFilter will call this function with all frames delivered to it.
    // buffer in only valid during this function call.
    virtual void FrameReceived(const uint8* buffer, int length,
        base::TimeTicks timestamp)
        = 0;

protected:
    virtual ~SinkFilterObserver();
};

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_WIN_SINK_FILTER_OBSERVER_WIN_H_
