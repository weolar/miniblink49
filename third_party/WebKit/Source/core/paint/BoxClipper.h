// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BoxClipper_h
#define BoxClipper_h

#include "platform/geometry/LayoutPoint.h"
#include "platform/graphics/paint/DisplayItem.h"

namespace blink {

class LayoutBox;
struct PaintInfo;

enum ContentsClipBehavior { ForceContentsClip, SkipContentsClipIfPossible };

class BoxClipper {
public:
    BoxClipper(const LayoutBox&, const PaintInfo&, const LayoutPoint& accumulatedOffset, ContentsClipBehavior);
    ~BoxClipper();
private:
    const LayoutBox& m_box;
    const PaintInfo& m_paintInfo;
    DisplayItem::Type m_clipType;
};

} // namespace blink

#endif // BoxClipper_h
