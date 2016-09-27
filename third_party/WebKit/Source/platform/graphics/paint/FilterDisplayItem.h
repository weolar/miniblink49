// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FilterDisplayItem_h
#define FilterDisplayItem_h

#include "platform/geometry/FloatRect.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "public/platform/WebFilterOperations.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#ifndef NDEBUG
#include "wtf/text/WTFString.h"
#endif

namespace blink {

class PLATFORM_EXPORT BeginFilterDisplayItem : public PairedBeginDisplayItem {
public:
    BeginFilterDisplayItem(const DisplayItemClientWrapper& client, PassRefPtr<SkImageFilter> imageFilter, const FloatRect& bounds, PassOwnPtr<WebFilterOperations> filterOperations = nullptr)
        : PairedBeginDisplayItem(client, BeginFilter)
        , m_imageFilter(imageFilter)
        , m_webFilterOperations(filterOperations)
        , m_bounds(bounds) { }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;
    bool drawsContent() const override;

private:
#ifndef NDEBUG
    void dumpPropertiesAsDebugString(WTF::StringBuilder&) const override;
#endif

    // FIXME: m_imageFilter should be replaced with m_webFilterOperations when copying data to the compositor.
    RefPtr<SkImageFilter> m_imageFilter;
    OwnPtr<WebFilterOperations> m_webFilterOperations;
    const FloatRect m_bounds;
};

class PLATFORM_EXPORT EndFilterDisplayItem : public PairedEndDisplayItem {
public:
    EndFilterDisplayItem(const DisplayItemClientWrapper& client)
        : PairedEndDisplayItem(client, EndFilter) { }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final { return otherType == BeginFilter; }
#endif
};

}

#endif // FilterDisplayItem_h
