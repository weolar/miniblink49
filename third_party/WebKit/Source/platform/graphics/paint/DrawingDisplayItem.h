// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DrawingDisplayItem_h
#define DrawingDisplayItem_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class PLATFORM_EXPORT DrawingDisplayItem : public DisplayItem {
public:
#if ENABLE(ASSERT)
    enum UnderInvalidationCheckingMode {
        CheckPicture, // Check if the new picture and the old picture are the same
        CheckBitmap, // Check if the new picture and the old picture produce the same bitmap
    };
#endif

    DrawingDisplayItem(const DisplayItemClientWrapper& client
        , Type type
        , PassRefPtr<const SkPicture> picture
#if ENABLE(ASSERT)
        , UnderInvalidationCheckingMode underInvalidationCheckingMode = CheckPicture
#endif
        )
        : DisplayItem(client, type)
        , m_picture(picture && picture->approximateOpCount() ? picture : nullptr)
#if ENABLE(ASSERT)
        , m_underInvalidationCheckingMode(underInvalidationCheckingMode)
#endif
    {
        ASSERT(isDrawingType(type));
    }

    virtual void replay(GraphicsContext&);
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;
    bool drawsContent() const override;

    const SkPicture* picture() const { return m_picture.get(); }

#if ENABLE(ASSERT)
    UnderInvalidationCheckingMode underInvalidationCheckingMode() const { return m_underInvalidationCheckingMode; }
#endif

private:
#ifndef NDEBUG
    void dumpPropertiesAsDebugString(WTF::StringBuilder&) const override;
#endif

    RefPtr<const SkPicture> m_picture;

#if ENABLE(ASSERT)
    UnderInvalidationCheckingMode m_underInvalidationCheckingMode;
#endif
};

} // namespace blink

#endif // DrawingDisplayItem_h
