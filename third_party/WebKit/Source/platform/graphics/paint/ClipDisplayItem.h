// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ClipDisplayItem_h
#define ClipDisplayItem_h

#include "SkRegion.h"
#include "platform/PlatformExport.h"
#include "platform/geometry/FloatRoundedRect.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class PLATFORM_EXPORT ClipDisplayItem : public PairedBeginDisplayItem {
public:
    ClipDisplayItem(const DisplayItemClientWrapper& client, Type type, const IntRect& clipRect)
        : PairedBeginDisplayItem(client, type)
        , m_clipRect(clipRect)
    {
        ASSERT(isClipType(type));
    }

    ClipDisplayItem(const DisplayItemClientWrapper& client, Type type, const IntRect& clipRect, Vector<FloatRoundedRect>& roundedRectClips)
        : ClipDisplayItem(client, type, clipRect)
    {
        m_roundedRectClips.swap(roundedRectClips);
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#ifndef NDEBUG
    void dumpPropertiesAsDebugString(WTF::StringBuilder&) const override;
#endif
    const IntRect m_clipRect;
    Vector<FloatRoundedRect> m_roundedRectClips;
};

class PLATFORM_EXPORT EndClipDisplayItem : public PairedEndDisplayItem {
public:
    EndClipDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : PairedEndDisplayItem(client, type)
    {
        ASSERT(isEndClipType(type));
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return DisplayItem::isClipType(otherType); }
#endif
};

} // namespace blink

#endif // ClipDisplayItem_h
