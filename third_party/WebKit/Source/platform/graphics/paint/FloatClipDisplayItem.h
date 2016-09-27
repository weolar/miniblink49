// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FloatClipDisplayItem_h
#define FloatClipDisplayItem_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatRect.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class RoundedRect;

class PLATFORM_EXPORT FloatClipDisplayItem : public PairedBeginDisplayItem {
public:
    FloatClipDisplayItem(const DisplayItemClientWrapper& client, Type type, const FloatRect& clipRect)
        : PairedBeginDisplayItem(client, type)
        , m_clipRect(clipRect)
    {
        ASSERT(isFloatClipType(type));
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#ifndef NDEBUG
    void dumpPropertiesAsDebugString(WTF::StringBuilder&) const override;
#endif

    const FloatRect m_clipRect;
};

class PLATFORM_EXPORT EndFloatClipDisplayItem : public PairedEndDisplayItem {
public:
    EndFloatClipDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : PairedEndDisplayItem(client, type)
    {
        ASSERT(isEndFloatClipType(type));
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return DisplayItem::isFloatClipType(otherType); }
#endif
};

} // namespace blink

#endif // FloatClipDisplayItem_h
