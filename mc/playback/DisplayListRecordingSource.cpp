#include "cc/playback/DisplayListRecordingSource.h"

namespace cc {

void DisplayListRecordingSource::invalidate(const IntRect& dirtyRect)
{
    IntRect inval = dirtyRect;
    inval.intersect(IntRect(0, 0, m_size.width(), m_size.height()));
    if (inval.isEmpty())
        return;

    // TODO: Support multiple non-intersecting webkit invals
    if (m_dirtyRects.size())
        m_dirtyRects[0].unite(inval);
    else
        m_dirtyRects.append(inval);
}

void DisplayListRecordingSource::setSize(const IntSize& size)
{
    ;
}

void DisplayListRecordingSource::applyDirtyRects(TileGrid* tileGrid, const IntRect& screenRect)
{
    // TODO 合并脏矩形
    Vector<IntRect> m_dirtyRects;
    for (size_t i = 0; i < m_dirtyRects.size(); ++i) {
        IntRect dirtyRect = m_dirtyRects[i];
        Vector<Tile*>& tiles = tileGrid->tiles();
        for (size_t j = 0; j < tiles.size(); ++j) {
            Tile* tile = tiles[i];
            blink::IntRect tilePos = tile->postion();
            if (!tilePos.intersects(dirtyRect))
                continue;
            blink::IntRect dirtyRectInTile = tilePos;
            dirtyRectInTile.intersect(dirtyRect);
            tile->setDirtyRect(dirtyRectInTile);
            if (TileGrid::isInWillBeShowedArea(tile, screenRect)) {
                // 发去光栅化;
            }
        }
    }
}

void DisplayListRecordingSource::updateRecordingSource(WebLayerImplClient* client, RecordingSourceContainer& recordingSourceContainer)
{

}

void DisplayListRecordingSource::updateRecordingSourcesIfNeeded(WebLayerImplClient* client)
{
    applyDirtyRects();
    for (size_t i = 0; i < m_recordingSourcePile.size(); i++) {
        RecordingSourceContainer& recordingSourceContainer = m_recordingSourcePile[i];
        if (recordingSourceContainer.dirty)
            updateRecordingSource(client, recordingSourceContainer);
    }
}

} // cc