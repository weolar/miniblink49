// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CachedDisplayItem_h
#define CachedDisplayItem_h

#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/Assertions.h"

namespace blink {

// A placeholder of DisplayItem in the new paint list of DisplayItemList, to indicate that
// the DisplayItem has not been changed and should be replaced with the cached DisplayItem
// when merging new paint list to cached paint list.
class PLATFORM_EXPORT CachedDisplayItem : public DisplayItem {
public:
    CachedDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : DisplayItem(client, type)
    {
        ASSERT(isCachedType(type));
    }

private:
    // CachedDisplayItem is never replayed or appended to WebDisplayItemList.
    void replay(GraphicsContext&) final { ASSERT_NOT_REACHED(); }
    void appendToWebDisplayItemList(WebDisplayItemList*) const final { ASSERT_NOT_REACHED(); }
};

} // namespace blink

#endif // CachedDisplayItem_h
