// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScrollDisplayItem_h
#define ScrollDisplayItem_h

#include "platform/geometry/IntSize.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/FastAllocBase.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class PLATFORM_EXPORT BeginScrollDisplayItem : public PairedBeginDisplayItem {
public:
    BeginScrollDisplayItem(const DisplayItemClientWrapper& client, Type type, const IntSize& currentOffset)
        : PairedBeginDisplayItem(client, type)
        , m_currentOffset(currentOffset)
    {
        ASSERT(isScrollType(type));
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#ifndef NDEBUG
    void dumpPropertiesAsDebugString(WTF::StringBuilder&) const final;
#endif

    const IntSize m_currentOffset;
};

class PLATFORM_EXPORT EndScrollDisplayItem : public PairedEndDisplayItem {
public:
    EndScrollDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : PairedEndDisplayItem(client, type)
    {
        ASSERT(isEndScrollType(type));
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return DisplayItem::isScrollType(otherType); }
#endif
};

} // namespace blink

#endif // ScrollDisplayItem_h
