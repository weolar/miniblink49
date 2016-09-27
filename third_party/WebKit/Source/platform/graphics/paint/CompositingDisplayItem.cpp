// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/CompositingDisplayItem.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/skia/SkiaUtils.h"
#include "public/platform/WebDisplayItemList.h"

namespace blink {

void BeginCompositingDisplayItem::replay(GraphicsContext& context)
{
    context.beginLayer(m_opacity, m_xferMode, m_hasBounds ? &m_bounds : nullptr, m_colorFilter);
}

void BeginCompositingDisplayItem::appendToWebDisplayItemList(WebDisplayItemList* list) const
{
    SkRect bounds = WebCoreFloatRectToSKRect(m_bounds);
    list->appendCompositingItem(m_opacity, m_xferMode, m_hasBounds ? &bounds : nullptr, GraphicsContext::WebCoreColorFilterToSkiaColorFilter(m_colorFilter).get());
}

#ifndef NDEBUG
void BeginCompositingDisplayItem::dumpPropertiesAsDebugString(WTF::StringBuilder& stringBuilder) const
{
    DisplayItem::dumpPropertiesAsDebugString(stringBuilder);
    stringBuilder.append(WTF::String::format(", xferMode: %d, opacity: %f", m_xferMode, m_opacity));
    if (m_hasBounds)
        stringBuilder.append(WTF::String::format(", bounds: [%f, %f, %f, %f]", m_bounds.location().x(), m_bounds.location().y(), m_bounds.size().width(), m_bounds.size().height()));
}
#endif

void EndCompositingDisplayItem::replay(GraphicsContext& context)
{
    context.endLayer();
}

void EndCompositingDisplayItem::appendToWebDisplayItemList(WebDisplayItemList* list) const
{
    list->appendEndCompositingItem();
}

} // namespace blink
