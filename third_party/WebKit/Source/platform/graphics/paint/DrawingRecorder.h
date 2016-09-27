// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DrawingRecorder_h
#define DrawingRecorder_h

#include "platform/PlatformExport.h"

#include "platform/geometry/FloatRect.h"
#include "platform/graphics/paint/DrawingDisplayItem.h"

#ifndef NDEBUG
#include "wtf/text/WTFString.h"
#endif

namespace blink {

class GraphicsContext;

class PLATFORM_EXPORT DrawingRecorder {
public:
    static bool useCachedDrawingIfPossible(GraphicsContext&, const DisplayItemClientWrapper&, DisplayItem::Type);

    DrawingRecorder(GraphicsContext&, const DisplayItemClientWrapper&, DisplayItem::Type, const FloatRect& cullRect);
    ~DrawingRecorder();

#if ENABLE(ASSERT)
    void setUnderInvalidationCheckingMode(DrawingDisplayItem::UnderInvalidationCheckingMode mode) { m_underInvalidationCheckingMode = mode; }
#endif

private:
    GraphicsContext& m_context;
    DisplayItemClientWrapper m_displayItemClient;
    const DisplayItem::Type m_displayItemType;
#if ENABLE(ASSERT)
    size_t m_displayItemPosition;
    DrawingDisplayItem::UnderInvalidationCheckingMode m_underInvalidationCheckingMode;
#endif
};

} // namespace blink

#endif // DrawingRecorder_h
