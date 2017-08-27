#ifndef TileGrid_h
#define TileGrid_h

#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/public/platform/WebCanvas.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/HashSet.h"

namespace cc_blink {
class WebLayerImpl;
}

namespace blink {
class WebContentLayerClient;
}

namespace WTF {
class Mutex;
}

class SkPicture;

namespace cc {

class Tile;
class RasterTaskGroup;

// ��������DisplayListRecordingSourceͳһ���ñ���
class TileGrid {
public:
    TileGrid(cc_blink::WebLayerImpl*);
    ~TileGrid();
    void registerTile(Tile* tile);
    void unregisterTile(Tile* tile);
    // from raster task worker thread;
    void increaseRsterTaskCount();
    void decreaseRsterTaskCount();
    bool willShutdown() const;
    void waitForReleaseTilesInUIThread();

    void mergeDirtyRectAndClipToCanBeShowedAreaIfNeeded(bool needClip);

    void update(blink::WebContentLayerClient* client, RasterTaskGroup* taskGroup, const blink::IntSize& newLayerSize, const blink::IntRect& screenRect);

    bool isInWillBeShowedArea(Tile* tile) const;
    blink::IntRect getInWillBeShowedAreaPos() const;

    Tile* getTileByXY(int xIndex, int yIndex);
    void invalidate(const blink::IntRect& dirtyRect, bool directSaveToDirtyRects);
    //void appendPendingInvalidateRect(const blink::IntRect& r);

    void drawToCanvas(cc_blink::WebLayerImpl* webLayer, blink::WebCanvas* canvas, const blink::IntRect& clip);

    cc_blink::WebLayerImpl* layer() const;

    void setTilesMutex(WTF::Mutex* tilesMutex);

    void lockTiles();
    void unlockTiles();

    Vector<Tile*>* m_tiles; // for test!~!!!!!!!!!!!!!!

private:
    void updateTilePriorityAndCommitInvalidate(Vector<size_t>* hasBitmapTiles);
    void updateTilePriorityAndCommitInvalidate2(Vector<size_t>* hasBitmapTiles);
    void doUpdateTilePriority(Tile* tile, Vector<size_t>* hasBitmapTiles, blink::IntRect* newCreatedWhenScrolling);
    void applyDirtyRectsToRaster(blink::WebContentLayerClient* client, RasterTaskGroup* taskGroup);
    void markTileDirtyExceptNeedBeShowedArea(const blink::IntRect& dirtyRect);
    void savaUnnecessaryTile(RasterTaskGroup* taskGroup, Vector<Tile*>* hasBitmapTiles);
    void cleanupUnnecessaryTile(Vector<size_t>* hasBitmapTiles);
    void updateSize(const blink::IntRect& screenRect, const blink::IntSize& newLayerSize);
    int getIndexByTile(const Tile* tile) const;

    Vector<Tile*> m_registerTiles; // �����ã���¼���л�û�ͷŵ�tile������m_tiles�Ѿ�û�м�¼��
    int m_numTileX;
    int m_numTileY;
    Vector<blink::IntRect> m_dirtyRects;
    cc_blink::WebLayerImpl* m_layer;
    blink::IntSize m_layerSize;
    blink::IntRect m_needBeShowedArea;
    blink::IntRect m_screenRect;
    bool m_willShutdown;
    blink::IntRect m_lastInWillBeShowedAreaPosIndex;

    WTF::Mutex* m_registerTileMutex; // for debug
    int m_rasterTaskCount;

    WTF::Mutex* m_tilesMutex; // from layerTreeHost()->tilesMutex()
};

}

#endif // TileGrid_h