// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SubtreeDisplayItem_h
#define SubtreeDisplayItem_h

#include "platform/geometry/FloatRect.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/Assertions.h"

namespace blink {

class PLATFORM_EXPORT SubtreeCachedDisplayItem : public DisplayItem {
public:
    SubtreeCachedDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : DisplayItem(client, type)
    {
        ASSERT(isSubtreeCachedType(type));
    }

    void replay(GraphicsContext&) final { ASSERT_NOT_REACHED(); }
    void appendToWebDisplayItemList(WebDisplayItemList*) const final { ASSERT_NOT_REACHED(); }
};

class PLATFORM_EXPORT BeginSubtreeDisplayItem : public PairedBeginDisplayItem {
public:
    BeginSubtreeDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : PairedBeginDisplayItem(client, type)
    {
        ASSERT(isBeginSubtreeType(type));
    }
};

class PLATFORM_EXPORT EndSubtreeDisplayItem : public PairedEndDisplayItem {
public:
    EndSubtreeDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : PairedEndDisplayItem(client, type)
    {
        ASSERT(isEndSubtreeType(type));
    }

#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return DisplayItem::isBeginSubtreeType(otherType); }
#endif
};

} // namespace blink

#endif // SubtreeDisplayItem_h
