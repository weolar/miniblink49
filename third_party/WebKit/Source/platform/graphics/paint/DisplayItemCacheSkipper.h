// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DisplayItemCacheSkipper_h
#define DisplayItemCacheSkipper_h

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"

namespace blink {

class DisplayItemCacheSkipper {
public:
    DisplayItemCacheSkipper(GraphicsContext& context)
        : m_context(context)
    {
        if (RuntimeEnabledFeatures::slimmingPaintEnabled())
            context.displayItemList()->beginSkippingCache();
    }
    ~DisplayItemCacheSkipper()
    {
        if (RuntimeEnabledFeatures::slimmingPaintEnabled())
            m_context.displayItemList()->endSkippingCache();
    }

private:
    GraphicsContext& m_context;
};

} // namespace blink

#endif // DisplayItemCacheSkipper_h
