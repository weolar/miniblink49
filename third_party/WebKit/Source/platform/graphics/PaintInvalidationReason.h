// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PaintInvalidationReason_h
#define PaintInvalidationReason_h

#include "platform/PlatformExport.h"

namespace blink {

enum PaintInvalidationReason {
    PaintInvalidationNone,
    PaintInvalidationIncremental,
    PaintInvalidationRectangle,
    // The following reasons will all cause full invalidation of the LayoutObject.
    PaintInvalidationFull, // Any unspecified reason of full invalidation.
    PaintInvalidationStyleChange,
    PaintInvalidationForcedByLayout,
    PaintInvalidationCompositingUpdate,
    PaintInvalidationBorderBoxChange,
    PaintInvalidationContentBoxChange,
    PaintInvalidationLayoutOverflowBoxChange,
    PaintInvalidationBoundsChange,
    PaintInvalidationLocationChange,
    PaintInvalidationBackgroundObscurationChange,
    PaintInvalidationBecameVisible,
    PaintInvalidationBecameInvisible,
    PaintInvalidationScroll,
    PaintInvalidationSelection,
    PaintInvalidationFocusRing,
    PaintInvalidationLayer,
    PaintInvalidationLayoutObjectInsertion,
    PaintInvalidationLayoutObjectRemoval,
    // PaintInvalidationDelayedFull means that PaintInvalidationFull is needed in order to fully paint
    // the content, but that painting of the object can be delayed until a future frame.
    // This can be the case for an object whose content is not visible to the user.
    PaintInvalidationDelayedFull
};

PLATFORM_EXPORT const char* paintInvalidationReasonToString(PaintInvalidationReason);

inline bool isFullPaintInvalidationReason(PaintInvalidationReason reason)
{
    return reason >= PaintInvalidationFull;
}

} // namespace blink

#endif // PaintInvalidationReason_h
