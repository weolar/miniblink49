// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file provides defines needed by PaintVectorIcon and is implemented
// by the generated file vector_icons.cc.

#ifndef UI_GFX_VECTOR_ICON_TYPES_H_
#define UI_GFX_VECTOR_ICON_TYPES_H_

#include "third_party/skia/include/core/SkScalar.h"

namespace gfx {

enum class VectorIconId;

// The size of a single side of the square canvas to which path coordinates
// are relative, in device independent pixels.
const int kReferenceSizeDip = 48;

// A command to Skia.
enum CommandType {
    // A new <path> element. For the first path, this is assumed.
    NEW_PATH,
    // Sets the color for the current path.
    PATH_COLOR_ARGB,
    // Sets the path to clear mode (Skia's kClear_Mode).
    PATH_MODE_CLEAR,
    // By default, the path will be filled. This changes the paint action to
    // stroke at the given width.
    STROKE,
    // By default, a stroke has a round cap. This sets it to square.
    CAP_SQUARE,
    // These correspond to pathing commands.
    MOVE_TO,
    R_MOVE_TO,
    LINE_TO,
    R_LINE_TO,
    H_LINE_TO,
    R_H_LINE_TO,
    V_LINE_TO,
    R_V_LINE_TO,
    CUBIC_TO,
    R_CUBIC_TO,
    CIRCLE,
    ROUND_RECT,
    CLOSE,
    // Sets the dimensions of the canvas in dip. (Default is kReferenceSizeDip.)
    CANVAS_DIMENSIONS,
    // Sets a bounding rect for the path. This allows fine adjustment because it
    // can tweak edge anti-aliasing. Args are x, y, w, h.
    CLIP,
    // Disables anti-aliasing for this path.
    DISABLE_AA,
    // Flips the x-axis in RTL locales. Default is false, this command sets it to
    // true.
    FLIPS_IN_RTL,
    // Marks the end of the list of commands.
    END
};

// A POD that describes either a path command or an argument for it.
struct PathElement {
    PathElement(CommandType value)
        : type(value)
    {
    }
    PathElement(SkScalar value)
        : arg(value)
    {
    }

    union {
        CommandType type;
        SkScalar arg;
    };
};

// Returns an array of path commands and arguments, terminated by END.
const PathElement* GetPathForVectorIcon(VectorIconId id);
// As above, but returns an icon specifically adjusted for 1x scale factors.
// This draws from icon files that end with .1x.icon. If no such file exists,
// it will fall back to GetPathForVectorIcon.
const PathElement* GetPathForVectorIconAt1xScale(VectorIconId id);

} // namespace gfx

#endif // UI_GFX_VECTOR_ICON_TYPES_H_
