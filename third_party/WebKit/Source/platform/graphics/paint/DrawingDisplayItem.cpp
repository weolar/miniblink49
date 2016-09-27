// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/DrawingDisplayItem.h"

#include "platform/graphics/GraphicsContext.h"
#include "public/platform/WebDisplayItemList.h"

namespace blink {

void DrawingDisplayItem::replay(GraphicsContext& context)
{
    if (m_picture)
        context.drawPicture(m_picture.get());
}

void DrawingDisplayItem::appendToWebDisplayItemList(WebDisplayItemList* list) const
{
    if (m_picture)
        list->appendDrawingItem(m_picture.get());
}

bool DrawingDisplayItem::drawsContent() const
{
    return m_picture;
}

#ifndef NDEBUG
void DrawingDisplayItem::dumpPropertiesAsDebugString(WTF::StringBuilder& stringBuilder) const
{
    DisplayItem::dumpPropertiesAsDebugString(stringBuilder);
    if (m_picture) {
        stringBuilder.append(WTF::String::format(", rect: [%f,%f,%f,%f]",
            m_picture->cullRect().x(), m_picture->cullRect().y(),
            m_picture->cullRect().width(), m_picture->cullRect().height()));
    }
}
#endif

}
