// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FixedPositionContainerDisplayItem_h
#define FixedPositionContainerDisplayItem_h

#include "platform/geometry/LayoutSize.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class PLATFORM_EXPORT BeginFixedPositionContainerDisplayItem : public PairedBeginDisplayItem {
public:
    BeginFixedPositionContainerDisplayItem(const DisplayItemClientWrapper& client) : PairedBeginDisplayItem(client, BeginFixedPositionContainer) { }

    void replay(GraphicsContext&) final { }
    void appendToWebDisplayItemList(WebDisplayItemList*) const final;
};

class PLATFORM_EXPORT EndFixedPositionContainerDisplayItem : public PairedEndDisplayItem {
public:
    EndFixedPositionContainerDisplayItem(const DisplayItemClientWrapper& client) : PairedEndDisplayItem(client, EndFixedPositionContainer) { }

    void replay(GraphicsContext&) final { }
    void appendToWebDisplayItemList(WebDisplayItemList*) const final;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return otherType == BeginFixedPositionContainer; }
#endif
};

} // namespace blink

#endif // FixedPositionContainerDisplayItem_h
