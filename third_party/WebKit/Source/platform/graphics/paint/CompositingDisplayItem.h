// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CompositingDisplayItem_h
#define CompositingDisplayItem_h

#include "platform/geometry/FloatRect.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "public/platform/WebBlendMode.h"
#include "wtf/PassOwnPtr.h"
#ifndef NDEBUG
#include "wtf/text/WTFString.h"
#endif

namespace blink {

class PLATFORM_EXPORT BeginCompositingDisplayItem : public PairedBeginDisplayItem {
public:
    BeginCompositingDisplayItem(const DisplayItemClientWrapper& client, const SkXfermode::Mode xferMode, const float opacity, const FloatRect* bounds, ColorFilter colorFilter = ColorFilterNone)
        : PairedBeginDisplayItem(client, BeginCompositing)
        , m_xferMode(xferMode)
        , m_opacity(opacity)
        , m_hasBounds(bounds)
        , m_colorFilter(colorFilter)
    {
        if (bounds)
            m_bounds = FloatRect(*bounds);
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#ifndef NDEBUG
    void dumpPropertiesAsDebugString(WTF::StringBuilder&) const override;
#endif
    const SkXfermode::Mode m_xferMode;
    const float m_opacity;
    bool m_hasBounds;
    FloatRect m_bounds;
    ColorFilter m_colorFilter;
};

class PLATFORM_EXPORT EndCompositingDisplayItem : public PairedEndDisplayItem {
public:
    EndCompositingDisplayItem(const DisplayItemClientWrapper& client)
        : PairedEndDisplayItem(client, EndCompositing) { }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return otherType == BeginCompositing; }
#endif
};

} // namespace blink

#endif // CompositingDisplayItem_h
