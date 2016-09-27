// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/PaintInvalidationReason.h"

#include "wtf/Assertions.h"

namespace blink {

const char* paintInvalidationReasonToString(PaintInvalidationReason reason)
{
    switch (reason) {
    case PaintInvalidationNone:
        return "none";
    case PaintInvalidationIncremental:
        return "incremental";
    case PaintInvalidationRectangle:
        return "invalidate paint rectangle";
    case PaintInvalidationFull:
        return "full";
    case PaintInvalidationStyleChange:
        return "style change";
    case PaintInvalidationForcedByLayout:
        return "forced by layout";
    case PaintInvalidationCompositingUpdate:
        return "compositing update";
    case PaintInvalidationBorderBoxChange:
        return "border box change";
    case PaintInvalidationContentBoxChange:
        return "content box change";
    case PaintInvalidationLayoutOverflowBoxChange:
        return "layout overflow box change";
    case PaintInvalidationBoundsChange:
        return "bounds change";
    case PaintInvalidationLocationChange:
        return "location change";
    case PaintInvalidationBackgroundObscurationChange:
        return "background obscuration change";
    case PaintInvalidationBecameVisible:
        return "became visible";
    case PaintInvalidationBecameInvisible:
        return "became invisible";
    case PaintInvalidationScroll:
        return "scroll";
    case PaintInvalidationSelection:
        return "selection";
    case PaintInvalidationFocusRing:
        return "focus ring";
    case PaintInvalidationLayer:
        return "layer";
    case PaintInvalidationLayoutObjectInsertion:
        return "layoutObject insertion";
    case PaintInvalidationLayoutObjectRemoval:
        return "layoutObject removal";
    case PaintInvalidationDelayedFull:
        return "delayed full";
    }
    ASSERT_NOT_REACHED();
    return "";
}

}
