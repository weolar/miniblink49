// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LineLayoutBox_h
#define LineLayoutBox_h

#include "core/layout/LayoutBox.h"
#include "core/layout/api/LineLayoutItem.h"
#include "platform/LayoutUnit.h"

namespace blink {

class LayoutBox;

class LineLayoutBox : public LineLayoutItem {
public:
    explicit LineLayoutBox(LayoutBox* layoutBox)
        : LineLayoutItem(layoutBox)
    {
    }

    LineLayoutBox(const LineLayoutItem& item)
        : LineLayoutItem(item)
    {
        ASSERT(!item || item.isBox());
    }

    LineLayoutBox() { }

    void setLogicalHeight(LayoutUnit size)
    {
        toBox()->setLogicalHeight(size);
    }

    LayoutUnit logicalHeight() const
    {
        return toBox()->logicalHeight();
    }

private:
    LayoutBox* toBox()
    {
        return toLayoutBox(layoutObject());
    }

    const LayoutBox* toBox() const
    {
        return toLayoutBox(layoutObject());
    }
};

} // namespace blink

#endif // LineLayoutBox_h
