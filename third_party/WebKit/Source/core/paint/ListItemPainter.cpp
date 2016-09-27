// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/ListItemPainter.h"

#include "core/layout/LayoutListItem.h"
#include "core/paint/BlockPainter.h"
#include "platform/geometry/LayoutPoint.h"

namespace blink {

void ListItemPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!m_layoutListItem.logicalHeight() && m_layoutListItem.hasOverflowClip())
        return;

    BlockPainter(m_layoutListItem).paint(paintInfo, paintOffset);
}

} // namespace blink
