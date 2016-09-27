// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RotationViewportAnchor_h
#define RotationViewportAnchor_h

#include "platform/geometry/FloatSize.h"
#include "platform/geometry/IntPoint.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/heap/Handle.h"
#include "web/ViewportAnchor.h"

namespace blink {

class FrameView;
class Node;
class PageScaleConstraintsSet;
class PinchViewport;

// The rotation anchor provides a way to anchor a viewport origin to a DOM node.
// In particular, the user supplies an anchor point (in view coordinates, e.g.,
// (0, 0) == viewport origin, (0.5, 0) == viewport top center). The anchor point
// tracks the underlying DOM node; as the node moves or the view is resized, the
// viewport anchor maintains its orientation relative to the node, and the
// viewport origin maintains its orientation relative to the anchor. If there is
// no node or it is lost during the resize, we fall back to the resize anchor
// logic.
class RotationViewportAnchor : public ViewportAnchor {
    STACK_ALLOCATED();
public:
    RotationViewportAnchor(FrameView& rootFrameView, PinchViewport&, const FloatSize& anchorInInnerViewCoords, PageScaleConstraintsSet&);
    ~RotationViewportAnchor();

private:
    void setAnchor();
    void restoreToAnchor();

    FloatPoint getInnerOrigin(const FloatSize& innerSize) const;

    void computeOrigins(const FloatSize& innerSize, IntPoint& mainFrameOffset, FloatPoint& pinchViewportOffset) const;

    float m_oldPageScaleFactor;
    float m_oldMinimumPageScaleFactor;

    // Inner viewport origin in the reference frame of the document in CSS pixels
    FloatPoint m_pinchViewportInDocument;

    // Inner viewport origin in the reference frame of the outer viewport
    // normalized to the outer viewport size.
    FloatSize m_normalizedPinchViewportOffset;

    RefPtrWillBeMember<Node> m_anchorNode;
    LayoutRect m_anchorNodeBounds;

    FloatSize m_anchorInInnerViewCoords;
    FloatSize m_anchorInNodeCoords;

    PageScaleConstraintsSet& m_pageScaleConstraintsSet;
};

} // namespace blink

#endif // RotationViewportAnchor_h
