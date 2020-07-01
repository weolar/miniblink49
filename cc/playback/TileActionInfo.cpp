#include "cc/playback/TileActionInfo.h"

#include "third_party/skia/include/core/SkBitmap.h"

namespace cc {

TileActionInfo::TileActionInfo(int index, int xIndex, int yIndex)
{
    this->index = index;
    this->xIndex = xIndex;
    this->yIndex = yIndex;

    m_bitmap = nullptr;
    m_isSolidColorCoverWholeTile = false;
    m_solidColor = nullptr;
}

TileActionInfo::~TileActionInfo()
{
    if (m_bitmap)
        delete m_bitmap;

    if (m_solidColor)
        delete m_solidColor;
}

}