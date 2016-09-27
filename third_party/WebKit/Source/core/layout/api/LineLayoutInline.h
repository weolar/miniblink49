// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LineLayoutInline_h
#define LineLayoutInline_h

#include "core/layout/LayoutInline.h"
#include "core/layout/api/LineLayoutItem.h"
#include "platform/LayoutUnit.h"

namespace blink {

class ComputedStyle;
class LayoutInline;
class LayoutObject;

class LineLayoutInline : public LineLayoutItem {
public:
    explicit LineLayoutInline(LayoutInline* layoutInline)
        : LineLayoutItem(layoutInline)
    {
    }

    LineLayoutInline(const LineLayoutItem& item)
        : LineLayoutItem(item)
    {
        ASSERT(!item || item.isLayoutInline());
    }

    LineLayoutInline() { }

    LineLayoutItem firstChild() const
    {
        return LineLayoutItem(toInline()->firstChild());
    }

    LineLayoutItem lastChild() const
    {
        return LineLayoutItem(toInline()->lastChild());
    }

protected:
    LayoutInline* toInline()
    {
        return toLayoutInline(layoutObject());
    }

    const LayoutInline* toInline() const
    {
        return toLayoutInline(layoutObject());
    }

};

} // namespace blink

#endif // LineLayoutInline_h
