// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ClipRectsCache_h
#define ClipRectsCache_h

#include "core/layout/ClipRects.h"

#if ENABLE(ASSERT)
#include "core/layout/LayoutBox.h" // For OverlayScrollbarSizeRelevancy.
#endif

namespace blink {

class DeprecatedPaintLayer;

enum ClipRectsCacheSlot {
    // Relative to the ancestor treated as the root (e.g. transformed layer). Used for hit testing.
    RootRelativeClipRects,
    RootRelativeClipRectsIgnoringViewportClip,

    // Relative to the LayoutView's layer. Used for compositing overlap testing.
    AbsoluteClipRects,

    // Relative to painting ancestor. Used for painting.
    PaintingClipRects,
    PaintingClipRectsIgnoringOverflowClip,

    NumberOfClipRectsCacheSlots,
    UncachedClipRects,
};

class ClipRectsCache {
    WTF_MAKE_FAST_ALLOCATED(ClipRectsCache);
public:
    struct Entry {
        Entry()
            : root(nullptr)
#if ENABLE(ASSERT)
            , scrollbarRelevancy(IgnoreOverlayScrollbarSize)
#endif
        {
        }

        const DeprecatedPaintLayer* root;
        RefPtr<ClipRects> clipRects;
#if ENABLE(ASSERT)
        OverlayScrollbarSizeRelevancy scrollbarRelevancy;
#endif
    };

    Entry& get(ClipRectsCacheSlot slot)
    {
        ASSERT(slot < NumberOfClipRectsCacheSlots);
        return m_entries[slot];
    }

    void clear(ClipRectsCacheSlot slot)
    {
        ASSERT(slot < NumberOfClipRectsCacheSlots);
        m_entries[slot] = Entry();
    }

private:
    Entry m_entries[NumberOfClipRectsCacheSlots];
};

} // namespace blink

#endif // ClipRectsCache_h
