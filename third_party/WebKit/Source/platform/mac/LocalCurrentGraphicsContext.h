/*
 * Copyright (C) 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "platform/PlatformExport.h"
#include "platform/geometry/IntRect.h"
#include "skia/ext/skia_utils_mac.h"
#include "wtf/Noncopyable.h"

OBJC_CLASS NSGraphicsContext;

class SkCanvas;

namespace blink {

class GraphicsContext;

// This class automatically saves and restores the current NSGraphicsContext for
// functions which call out into AppKit and rely on the currentContext being set
class PLATFORM_EXPORT LocalCurrentGraphicsContext {
    WTF_MAKE_NONCOPYABLE(LocalCurrentGraphicsContext);
public:
    LocalCurrentGraphicsContext(GraphicsContext*, const IntRect& dirtyRect);
    // Allows specifying an interest rect to which we clip if slimming paint is enabled and performance would benefit.
    LocalCurrentGraphicsContext(GraphicsContext*, const IntRect* interestRect, const IntRect& dirtyRect);
    LocalCurrentGraphicsContext(SkCanvas*, float deviceScaleFactor, const IntRect* interestRect, const IntRect& dirtyRect);
    ~LocalCurrentGraphicsContext();
    CGContextRef cgContext();
private:

    SkCanvas* m_savedCanvas;
    NSGraphicsContext* m_savedNSGraphicsContext;
    bool m_didSetGraphicsContext;
    IntRect m_inflatedDirtyRect;
    gfx::SkiaBitLocker m_skiaBitLocker;
};

}
