// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BackgroundImageGeometry_h
#define BackgroundImageGeometry_h

#include "platform/geometry/IntPoint.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/IntSize.h"

namespace blink {

class BackgroundImageGeometry {
public:
    BackgroundImageGeometry()
        : m_hasNonLocalGeometry(false)
    { }

    IntRect destRect() const { return m_destRect; }
    void setDestRect(const IntRect& destRect)
    {
        m_destRect = destRect;
    }

    IntPoint phase() const { return m_phase; }
    void setPhase(const IntPoint& phase)
    {
        m_phase = phase;
    }

    IntSize tileSize() const { return m_tileSize; }
    void setTileSize(const IntSize& tileSize)
    {
        m_tileSize = tileSize;
    }

    // Space-size represents extra width and height that may be added to
    // the image if used as a pattern with repeat: space
    IntSize spaceSize() const { return m_repeatSpacing; }
    void setSpaceSize(const IntSize& repeatSpacing)
    {
        m_repeatSpacing = repeatSpacing;
    }

    void setPhaseX(int x) { m_phase.setX(x); }
    void setPhaseY(int y) { m_phase.setY(y); }

    void setNoRepeatX(int xOffset);
    void setNoRepeatY(int yOffset);

    void useFixedAttachment(const IntPoint& attachmentPoint);

    void clip(const IntRect&);

    void setHasNonLocalGeometry(bool hasNonLocalGeometry = true) { m_hasNonLocalGeometry = hasNonLocalGeometry; }
    bool hasNonLocalGeometry() const { return m_hasNonLocalGeometry; }

private:
    IntRect m_destRect;
    IntPoint m_phase;
    IntSize m_tileSize;
    IntSize m_repeatSpacing;
    bool m_hasNonLocalGeometry; // Has background-attachment: fixed. Implies that we can't always cheaply compute destRect.
};

} // namespace blink

#endif // BackgroundImageGeometry_h
