// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FixedPositionDisplayItem_h
#define FixedPositionDisplayItem_h

#include "platform/geometry/LayoutSize.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/FastAllocBase.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class PLATFORM_EXPORT BeginFixedPositionDisplayItem : public PairedBeginDisplayItem {
public:
    BeginFixedPositionDisplayItem(const DisplayItemClientWrapper& client)
        : PairedBeginDisplayItem(client, BeginFixedPosition) { }

    void replay(GraphicsContext&) final { }
    void appendToWebDisplayItemList(WebDisplayItemList*) const final;
};

class PLATFORM_EXPORT EndFixedPositionDisplayItem : public PairedEndDisplayItem {
public:
    EndFixedPositionDisplayItem(const DisplayItemClientWrapper& client)
        : PairedEndDisplayItem(client, EndFixedPosition) { }

    void replay(GraphicsContext&) final { }
    void appendToWebDisplayItemList(WebDisplayItemList*) const final;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return otherType == BeginFixedPosition; }
#endif
};

} // namespace blink

#endif // FixedPositionDisplayItem_h
