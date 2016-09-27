// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/BackgroundImageGeometry.h"

namespace blink {

void BackgroundImageGeometry::setNoRepeatX(int xOffset)
{
    m_destRect.move(std::max(xOffset, 0), 0);
    m_phase.setX(-std::min(xOffset, 0));
    m_destRect.setWidth(m_tileSize.width() + std::min(xOffset, 0));
}
void BackgroundImageGeometry::setNoRepeatY(int yOffset)
{
    m_destRect.move(0, std::max(yOffset, 0));
    m_phase.setY(-std::min(yOffset, 0));
    m_destRect.setHeight(m_tileSize.height() + std::min(yOffset, 0));
}

void BackgroundImageGeometry::useFixedAttachment(const IntPoint& attachmentPoint)
{
    IntPoint alignedPoint = attachmentPoint;
    m_phase.move(std::max(alignedPoint.x() - m_destRect.x(), 0), std::max(alignedPoint.y() - m_destRect.y(), 0));
}

void BackgroundImageGeometry::clip(const IntRect& clipRect)
{
    m_destRect.intersect(clipRect);
}

} // namespace blink
