// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/OpenedFrameTracker.h"

#include "platform/heap/Handle.h"
#include "public/web/WebFrame.h"

namespace blink {

OpenedFrameTracker::OpenedFrameTracker()
{
}

OpenedFrameTracker::~OpenedFrameTracker()
{
#if !ENABLE(OILPAN)
    // Oilpan takes care of clearing weak m_opener fields during GC.
    updateOpener(0);
#endif
}

bool OpenedFrameTracker::isEmpty() const
{
    return m_openedFrames.isEmpty();
}

void OpenedFrameTracker::add(WebFrame* frame)
{
    m_openedFrames.add(frame);
}

void OpenedFrameTracker::remove(WebFrame* frame)
{
    m_openedFrames.remove(frame);
}

void OpenedFrameTracker::updateOpener(WebFrame* frame)
{
    HashSet<WebFrame*>::iterator end = m_openedFrames.end();
    for (HashSet<WebFrame*>::iterator it = m_openedFrames.begin(); it != end; ++it)
        (*it)->m_opener = frame;
}

template <typename VisitorDispatcher>
ALWAYS_INLINE void OpenedFrameTracker::traceFramesImpl(VisitorDispatcher visitor)
{
#if ENABLE(OILPAN)
    HashSet<WebFrame*>::iterator end = m_openedFrames.end();
    for (HashSet<WebFrame*>::iterator it = m_openedFrames.begin(); it != end; ++it)
        WebFrame::traceFrame(visitor, *it);
#endif
}

void OpenedFrameTracker::traceFrames(Visitor* visitor) { traceFramesImpl(visitor); }
void OpenedFrameTracker::traceFrames(InlinedGlobalMarkingVisitor visitor) { traceFramesImpl(visitor); }

} // namespace blink
