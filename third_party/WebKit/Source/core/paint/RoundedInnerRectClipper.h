// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RoundedInnerRectClipper_h
#define RoundedInnerRectClipper_h

#include "platform/graphics/paint/DisplayItem.h"

namespace blink {

class FloatRoundedRect;
class LayoutRect;
class LayoutObject;
struct PaintInfo;

enum RoundedInnerRectClipperBehavior {
    ApplyToDisplayListIfEnabled,
    ApplyToContext
};

class RoundedInnerRectClipper {
public:
    RoundedInnerRectClipper(LayoutObject&, const PaintInfo&, const LayoutRect&, const FloatRoundedRect& clipRect, RoundedInnerRectClipperBehavior);
    ~RoundedInnerRectClipper();

private:
    LayoutObject& m_layoutObject;
    const PaintInfo& m_paintInfo;
    bool m_useDisplayItemList;
    DisplayItem::Type m_clipType;
};

} // namespace blink

#endif // RoundedInnerRectClipper_h
