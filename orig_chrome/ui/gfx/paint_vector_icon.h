// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_PAINT_VECTOR_ICON_H_
#define UI_GFX_PAINT_VECTOR_ICON_H_

#include <stddef.h>

#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/gfx_export.h"
#include "ui/gfx/image/image_skia.h"

namespace gfx {

class Canvas;
enum class VectorIconId;

// Draws a vector icon identified by |id| onto |canvas| at (0, 0). |dip_size|
// is the length of a single edge of the square icon, in device independent
// pixels. |color| is used as the fill.
GFX_EXPORT void PaintVectorIcon(Canvas* canvas,
    VectorIconId id,
    size_t dip_size,
    SkColor color);

// Creates an ImageSkia which will render the icon on demand.
GFX_EXPORT ImageSkia CreateVectorIcon(VectorIconId id,
    size_t dip_size,
    SkColor color);

// As above, but also paints a badge defined by |badge_id| on top of the icon.
// The badge uses the same canvas size and default color as the icon.
GFX_EXPORT ImageSkia CreateVectorIconWithBadge(VectorIconId id,
    size_t dip_size,
    SkColor color,
    VectorIconId badge_id);

#if defined(GFX_VECTOR_ICONS_UNSAFE) || defined(GFX_IMPLEMENTATION)
// Takes a string of the format expected of .icon files and renders onto
// a canvas. This should only be used as a debugging aid and should never be
// used in production code.
GFX_EXPORT ImageSkia CreateVectorIconFromSource(const std::string& source,
    size_t dip_size,
    SkColor color);
#endif

} // namespace gfx

#endif // UI_GFX_PAINT_VECTOR_ICON_H_
