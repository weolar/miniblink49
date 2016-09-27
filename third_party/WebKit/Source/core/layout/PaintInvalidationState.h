// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PaintInvalidationState_h
#define PaintInvalidationState_h

#include "platform/geometry/LayoutRect.h"
#include "platform/transforms/AffineTransform.h"
#include "wtf/Noncopyable.h"

namespace blink {

class LayoutBoxModelObject;
class LayoutObject;
class LayoutSVGModelObject;
class LayoutView;

class PaintInvalidationState {
    WTF_MAKE_NONCOPYABLE(PaintInvalidationState);
public:
    PaintInvalidationState(PaintInvalidationState& next, LayoutBoxModelObject& layoutObject, const LayoutBoxModelObject& paintInvalidationContainer);
    PaintInvalidationState(PaintInvalidationState& next, const LayoutSVGModelObject& layoutObject);

    PaintInvalidationState(const LayoutView& layoutView, Vector<LayoutObject*>& pendingDelayedPaintInvalidations)
        : PaintInvalidationState(layoutView, pendingDelayedPaintInvalidations, nullptr) { }
    PaintInvalidationState(const LayoutView& layoutView, PaintInvalidationState& ownerPaintInvalidationState)
        : PaintInvalidationState(layoutView, ownerPaintInvalidationState.m_pendingDelayedPaintInvalidations, &ownerPaintInvalidationState) { }

    const LayoutRect& clipRect() const { return m_clipRect; }
    const LayoutSize& paintOffset() const { return m_paintOffset; }
    const AffineTransform& svgTransform() const { ASSERT(m_svgTransform); return *m_svgTransform; }

    bool cachedOffsetsEnabled() const { return m_cachedOffsetsEnabled; }
    bool isClipped() const { return m_clipped; }

    bool ancestorHadPaintInvalidationForLocationChange() const { return m_ancestorHadPaintInvalidationForLocationChange; }
    void setAncestorHadPaintInvalidationForLocationChange() { m_ancestorHadPaintInvalidationForLocationChange = true; }

    const LayoutBoxModelObject& paintInvalidationContainer() const { return m_paintInvalidationContainer; }

    bool canMapToContainer(const LayoutBoxModelObject* container) const
    {
        return m_cachedOffsetsEnabled && container == &m_paintInvalidationContainer;
    }

    // Records |obj| as needing paint invalidation on the next frame. See the definition of PaintInvalidationDelayedFull for more details.
    void pushDelayedPaintInvalidationTarget(LayoutObject& obj) { m_pendingDelayedPaintInvalidations.append(&obj); }
    Vector<LayoutObject*>& pendingDelayedPaintInvalidationTargets() { return m_pendingDelayedPaintInvalidations; }

private:
    PaintInvalidationState(const LayoutView&, Vector<LayoutObject*>& pendingDelayedPaintInvalidations, PaintInvalidationState* ownerPaintInvalidationState);

    void applyClipIfNeeded(const LayoutObject&);
    void addClipRectRelativeToPaintOffset(const LayoutSize& clipSize);

    friend class ForceHorriblySlowRectMapping;

    bool m_clipped;
    mutable bool m_cachedOffsetsEnabled;
    bool m_ancestorHadPaintInvalidationForLocationChange;

    LayoutRect m_clipRect;

    // x/y offset from paint invalidation container. Includes relative positioning and scroll offsets.
    LayoutSize m_paintOffset;

    const LayoutBoxModelObject& m_paintInvalidationContainer;

    // Transform from the initial viewport coordinate system of an outermost
    // SVG root to the userspace _before_ the relevant element. Combining this
    // with |m_paintOffset| yields the "final" offset.
    OwnPtr<AffineTransform> m_svgTransform;

    Vector<LayoutObject*>& m_pendingDelayedPaintInvalidations;
};

} // namespace blink

#endif // PaintInvalidationState_h
