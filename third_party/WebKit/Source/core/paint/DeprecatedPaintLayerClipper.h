/*
 * Copyright (C) 2003, 2009, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
 *
 * Portions are Copyright (C) 1998 Netscape Communications Corporation.
 *
 * Other contributors:
 *   Robert O'Callahan <roc+@cs.cmu.edu>
 *   David Baron <dbaron@fas.harvard.edu>
 *   Christian Biesinger <cbiesinger@web.de>
 *   Randall Jesup <rjesup@wgate.com>
 *   Roland Mainz <roland.mainz@informatik.med.uni-giessen.de>
 *   Josh Soref <timeless@mac.com>
 *   Boris Zbarsky <bzbarsky@mit.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Alternatively, the contents of this file may be used under the terms
 * of either the Mozilla Public License Version 1.1, found at
 * http://www.mozilla.org/MPL/ (the "MPL") or the GNU General Public
 * License Version 2.0, found at http://www.fsf.org/copyleft/gpl.html
 * (the "GPL"), in which case the provisions of the MPL or the GPL are
 * applicable instead of those above.  If you wish to allow use of your
 * version of this file only under the terms of one of those two
 * licenses (the MPL or the GPL) and not to allow others to use your
 * version of this file under the LGPL, indicate your decision by
 * deletingthe provisions above and replace them with the notice and
 * other provisions required by the MPL or the GPL, as the case may be.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under any of the LGPL, the MPL or the GPL.
 */

#ifndef DeprecatedPaintLayerClipper_h
#define DeprecatedPaintLayerClipper_h

#include "core/layout/ClipRectsCache.h"
#include "core/layout/LayoutBox.h"

namespace blink {

class DeprecatedPaintLayer;

enum ShouldRespectOverflowClip {
    IgnoreOverflowClip,
    RespectOverflowClip
};

class ClipRectsContext {
public:
    ClipRectsContext(const DeprecatedPaintLayer* root, ClipRectsCacheSlot slot, OverlayScrollbarSizeRelevancy relevancy = IgnoreOverlayScrollbarSize, const LayoutSize& accumulation = LayoutSize())
        : rootLayer(root)
        , scrollbarRelevancy(relevancy)
        , cacheSlot(slot)
        , subPixelAccumulation(accumulation)
        , respectOverflowClip(slot == PaintingClipRectsIgnoringOverflowClip ? IgnoreOverflowClip : RespectOverflowClip)
        , respectOverflowClipForViewport(slot == RootRelativeClipRectsIgnoringViewportClip ? IgnoreOverflowClip : RespectOverflowClip)
    {
    }

    void setIgnoreOverflowClip()
    {
        ASSERT(!usesCache() || cacheSlot == PaintingClipRects);
        ASSERT(respectOverflowClip == RespectOverflowClip);
        if (usesCache())
            cacheSlot = PaintingClipRectsIgnoringOverflowClip;
        respectOverflowClip = IgnoreOverflowClip;
    }

    bool usesCache() const
    {
        return cacheSlot != UncachedClipRects;
    }

    const DeprecatedPaintLayer* const rootLayer;
    const OverlayScrollbarSizeRelevancy scrollbarRelevancy;

private:
    friend class DeprecatedPaintLayerClipper;

    ClipRectsCacheSlot cacheSlot;
    LayoutSize subPixelAccumulation;
    ShouldRespectOverflowClip respectOverflowClip;
    ShouldRespectOverflowClip respectOverflowClipForViewport;
};

class DeprecatedPaintLayerClipper {
    WTF_MAKE_NONCOPYABLE(DeprecatedPaintLayerClipper);
public:
    explicit DeprecatedPaintLayerClipper(LayoutBoxModelObject&);

    void clearClipRectsIncludingDescendants();
    void clearClipRectsIncludingDescendants(ClipRectsCacheSlot);

    LayoutRect childrenClipRect() const; // Returns the foreground clip rect of the layer in the document's coordinate space.
    LayoutRect localClipRect() const; // Returns the background clip rect of the layer in the local coordinate space.

    ClipRects* getClipRects(const ClipRectsContext&) const;

    ClipRect backgroundClipRect(const ClipRectsContext&) const;

    // This method figures out our layerBounds in coordinates relative to
    // |rootLayer|. It also computes our background and foreground clip rects
    // for painting/event handling.
    // Pass offsetFromRoot if known.
    void calculateRects(const ClipRectsContext&, const LayoutRect& paintDirtyRect, LayoutRect& layerBounds,
        ClipRect& backgroundRect, ClipRect& foregroundRect, ClipRect& outlineRect, const LayoutPoint* offsetFromRoot = 0) const;

private:
    void calculateClipRects(const ClipRectsContext&, ClipRects&) const;

    ClipRects* clipRectsIfCached(const ClipRectsContext&) const;
    ClipRects* storeClipRectsInCache(const ClipRectsContext&, ClipRects* parentClipRects, const ClipRects&) const;

    // cachedClipRects looks buggy: It doesn't check whether context.rootLayer and entry.root match.
    // FIXME: Move callers to clipRectsIfCached, which does the proper checks.
    ClipRects* cachedClipRects(const ClipRectsContext& context) const
    {
        return m_cache ? m_cache->get(context.cacheSlot).clipRects.get() : 0;
    }

    void getOrCalculateClipRects(const ClipRectsContext&, ClipRects&) const;

    DeprecatedPaintLayer* clippingRootForPainting() const;

    ClipRectsCache& cache() const
    {
        if (!m_cache)
            m_cache = adoptPtr(new ClipRectsCache);
        return *m_cache;
    }

    bool shouldRespectOverflowClip(const ClipRectsContext&) const;

    // FIXME: Could this be a LayoutBox?
    LayoutBoxModelObject& m_layoutObject;
    mutable OwnPtr<ClipRectsCache> m_cache;
};

} // namespace blink

#endif // LayerClipper_h
