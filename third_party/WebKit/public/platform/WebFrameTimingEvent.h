// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebFrameTimingEvent_h
#define WebFrameTimingEvent_h

namespace blink {

// A struct to pass PerformanceRenderEvent or PerformanceCompositeEvent info
// to Blink.
struct WebFrameTimingEvent {
    WebFrameTimingEvent(unsigned sourceFrame, double startTime, double finishTime)
        : sourceFrame(sourceFrame)
        , startTime(startTime)
        , finishTime(finishTime)
    {
    }

    WebFrameTimingEvent(unsigned sourceFrame, double startTime)
        : sourceFrame(sourceFrame)
        , startTime(startTime)
    {
    }

    WebFrameTimingEvent() { }

    WebFrameTimingEvent& operator=(const WebFrameTimingEvent& rhs)
    {
        sourceFrame = rhs.sourceFrame;
        startTime = rhs.startTime;
        finishTime = rhs.finishTime;
        return *this;
    }
    unsigned sourceFrame;
    double startTime;
    double finishTime;
};

} // namespace blink

#endif
