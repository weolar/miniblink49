// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutObjectDrawingRecorder_h
#define LayoutObjectDrawingRecorder_h

#include "core/layout/LayoutObject.h"
#include "core/paint/PaintPhase.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/paint/DisplayItemCacheSkipper.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "wtf/Optional.h"

namespace blink {

class GraphicsContext;

// Convenience wrapper of DrawingRecorder for LayoutObject painters.
class LayoutObjectDrawingRecorder final {
public:
    static bool useCachedDrawingIfPossible(GraphicsContext& context, const LayoutObject& layoutObject, DisplayItem::Type displayItemType)
    {
        if (layoutObject.fullPaintInvalidationReason() == PaintInvalidationDelayedFull)
            return false;
        return DrawingRecorder::useCachedDrawingIfPossible(context, layoutObject, displayItemType);
    }

    static bool useCachedDrawingIfPossible(GraphicsContext& context, const LayoutObject& layoutObject, PaintPhase phase)
    {
        return useCachedDrawingIfPossible(context, layoutObject, DisplayItem::paintPhaseToDrawingType(phase));
    }

    LayoutObjectDrawingRecorder(GraphicsContext& context, const LayoutObject& layoutObject, DisplayItem::Type displayItemType, const FloatRect& clip)
    {
        // We may paint a delayed-invalidation object before it's actually invalidated.
        if (layoutObject.fullPaintInvalidationReason() == PaintInvalidationDelayedFull)
            m_cacheSkipper.emplace(context);
        m_drawingRecorder.emplace(context, layoutObject, displayItemType, clip);
    }

    LayoutObjectDrawingRecorder(GraphicsContext& context, const LayoutObject& layoutObject, PaintPhase phase, const FloatRect& clip)
        : LayoutObjectDrawingRecorder(context, layoutObject, DisplayItem::paintPhaseToDrawingType(phase), clip) { }

    LayoutObjectDrawingRecorder(GraphicsContext& context, const LayoutObject& layoutObject, DisplayItem::Type type, const LayoutRect& clip)
        : LayoutObjectDrawingRecorder(context, layoutObject, type, pixelSnappedIntRect(clip)) { }

#if ENABLE(ASSERT)
    void setUnderInvalidationCheckingMode(DrawingDisplayItem::UnderInvalidationCheckingMode mode) { m_drawingRecorder->setUnderInvalidationCheckingMode(mode); }
#endif

private:
    Optional<DisplayItemCacheSkipper> m_cacheSkipper;
    Optional<DrawingRecorder> m_drawingRecorder;
};

} // namespace blink

#endif // LayoutObjectDrawingRecorder_h
