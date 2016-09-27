/*
 * Copyright (C) 2006, 2007, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef AutoscrollController_h
#define AutoscrollController_h

#include "core/CoreExport.h"
#include "platform/geometry/IntPoint.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class LocalFrame;
class FrameView;
class Node;
class Page;
class PlatformMouseEvent;
class LayoutBox;
class LayoutObject;

enum AutoscrollType {
    NoAutoscroll,
    AutoscrollForDragAndDrop,
    AutoscrollForSelection,
#if OS(WIN)
    AutoscrollForPanCanStop,
    AutoscrollForPan,
#endif
};

// AutscrollController handels autoscroll and pan scroll for EventHandler.
class CORE_EXPORT AutoscrollController {
public:
    static PassOwnPtr<AutoscrollController> create(Page&);

    static const int noPanScrollRadius = 15;

    void animate(double monotonicFrameBeginTime);
    bool autoscrollInProgress() const;
    bool autoscrollInProgress(const LayoutBox*) const;
    bool panScrollInProgress() const;
    void startAutoscrollForSelection(LayoutObject*);
    void stopAutoscroll();
    void stopAutoscrollIfNeeded(LayoutObject*);
    void updateAutoscrollLayoutObject();
    void updateDragAndDrop(Node* targetNode, const IntPoint& eventPosition, double eventTime);
#if OS(WIN)
    void handleMouseReleaseForPanScrolling(LocalFrame*, const PlatformMouseEvent&);
    void startPanScrolling(LayoutBox*, const IntPoint&);
#endif

private:
    explicit AutoscrollController(Page&);

    void startAutoscroll();

#if OS(WIN)
    void updatePanScrollState(FrameView*, const IntPoint& lastKnownMousePosition);
#endif

    Page& m_page;
    LayoutBox* m_autoscrollLayoutObject;
    AutoscrollType m_autoscrollType;
    IntPoint m_dragAndDropAutoscrollReferencePosition;
    double m_dragAndDropAutoscrollStartTime;
#if OS(WIN)
    IntPoint m_panScrollStartPos;
#endif
};

} // namespace blink

#endif // AutoscrollController_h
