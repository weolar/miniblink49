// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LineLayoutText_h
#define LineLayoutText_h

#include "core/layout/LayoutText.h"
#include "core/layout/api/LineLayoutItem.h"
#include "platform/LayoutUnit.h"
#include "platform/text/TextPath.h"
#include "wtf/Forward.h"

namespace blink {

class LayoutText;

class LineLayoutText : public LineLayoutItem {
public:
    explicit LineLayoutText(LayoutText* layoutObject) : LineLayoutItem(layoutObject) { }

    LineLayoutText(const LineLayoutItem& item)
        : LineLayoutItem(item)
    {
        ASSERT(!item || item.isText());
    }

    LineLayoutText() { }

    bool isWordBreak() const
    {
        return toText()->isWordBreak();
    }

    bool isAllCollapsibleWhitespace() const
    {
        return toText()->isAllCollapsibleWhitespace();
    }

    UChar characterAt(unsigned offset) const
    {
        return toText()->characterAt(offset);
    }

    unsigned textLength() const
    {
        return toText()->textLength();
    }

private:
    LayoutText* toText() { return toLayoutText(layoutObject()); }
    const LayoutText* toText() const { return toLayoutText(layoutObject()); }
};

} // namespace blink

#endif // LineLayoutText_h
