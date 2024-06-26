#include "mc/tiles/TileGrid.h"

#include "third_party/WebKit/public/platform/WebContentLayerClient.h"
#include "third_party/WebKit/Source/platform/geometry/win/IntRectWin.h"
#include "mc/tiles/TileWidthHeight.h"
#include "mc/tiles/Tile.h"
#include "mc/tiles/TilesAddr.h"

#include "mc/blink/WebLayerImpl.h"
#include "mc/raster/RasterTask.h"
#include "mc/trees/LayerTreeHost.h"
#include "mc/trees/DrawProperties.h"
#include "mc/playback/TileActionInfo.h"
#include "mc/playback/LayerChangeAction.h"

#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/WebKit/Source/platform/graphics/GraphicsContext.h"

#include "platform/image-encoders/gdiplus/GDIPlusImageEncoder.h" // TODO
#include "base/rand_util.h"

extern DWORD g_nowTime;

namespace content {
extern int debugPaint;
extern int debugPaintTile;
}

namespace blink {
bool saveDumpFile(const String& url, char* buffer, unsigned int size);
}

namespace mc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, tileGridCount, ("mcTileGrid"));
#endif

const int kXIndexDistanceToWillBeShowedTile = 0; // 1;
const int kYIndexDistanceToWillBeShowedTile = 0; // 2;


TileGrid::TileGrid(mc_blink::WebLayerImpl* layer)
{
    m_rasterTaskCount = 0;
    m_numTileX = 0;
    m_numTileY = 0;
    m_layer = layer;
    m_willShutdown = false;
    m_registerTileMutex = new WTF::Mutex();
    m_tilesAddr = new TilesAddr(this);
    m_tilesMutex = nullptr;
    m_isForceCleanup = false;

#ifndef NDEBUG
    tileGridCount.increment();
#endif
}

int debugRasterCount = 0;
extern LayerTreeHost* gLayerTreeHost;

TileGrid::~TileGrid()
{
    ASSERT(isMainThread());
    m_willShutdown = true;
    
    waitForReleaseTilesInUIThread();

    delete m_tilesAddr;
    ASSERT(0 == m_registerTiles.size());

    m_layer = nullptr;
    
    delete m_registerTileMutex;

#ifndef NDEBUG
    tileGridCount.decrement();
#endif
}

void TileGrid::increaseRsterTaskCount()
{
    atomicIncrement(&m_rasterTaskCount);
}

void TileGrid::decreaseRsterTaskCount()
{
    atomicDecrement(&m_rasterTaskCount);
}

bool TileGrid::willShutdown() const
{
    return m_willShutdown;
}

void TileGrid::waitForReleaseTilesInUIThread()
{
    ASSERT(m_rasterTaskCount >= 0);

//     int dumyDebugRasterCount = debugRasterCount;
//     RasterTaskWorkerThreadPool* debugPool = mc::RasterTaskWorkerThreadPool::shared();
//     while (m_rasterTaskCount != 0) { ::Sleep(50); }
}

mc_blink::WebLayerImpl* TileGrid::layer() const
{
    return m_layer;
}

void TileGrid::registerTile(Tile* tile)
{
    MutexLocker locker(*m_registerTileMutex);
    if (WTF::kNotFound == m_registerTiles.find(tile))
        m_registerTiles.append(tile);
}

void TileGrid::unregisterTile(Tile* tile)
{
    MutexLocker locker(*m_registerTileMutex);
    size_t pos = m_registerTiles.find(tile);
    if (WTF::kNotFound != pos)
        m_registerTiles.remove(pos);
}

void TileGrid::setTilesMutex(WTF::Mutex* tilesMutex)
{
    m_tilesMutex = tilesMutex;
}

void TileGrid::lockTiles()
{
    m_tilesMutex->lock();
}

void TileGrid::unlockTiles()
{
    m_tilesMutex->unlock();
}

// 给定一个长度，获取坐落在哪个tile里
static int getIndexByLength(int length, int tileLength)
{
    return (int)(length / tileLength) /*+ (length % tileLength != 0 ? 1 : 0)*/;
}

// 给定一个长度，获取一共需要多少个tile
static int getIndexNumByLength(int length, int tileLength)
{
    return (int)(length / tileLength) + (length % tileLength != 0 ? 1 : 0);
}

bool TileGrid::isInWillBeShowedArea(Tile* tile) const
{
    int xIndex = tile->xIndex();
    int yIndex = tile->yIndex();

    blink::IntRect pos = getInWillBeShowedAreaPos();
    if (0 == pos.width() || 0 == pos.height())
        return false;

    if (xIndex < pos.x() || xIndex >= pos.maxX() || yIndex < pos.y() || yIndex >= pos.maxY())
        return false;
    return true;
}

blink::IntRect TileGrid::getInWillBeShowedAreaPos() const
{
    if (0 == m_numTileX || 0 == m_numTileY)
        return blink::IntRect();

    int left = getIndexByLength(m_screenRect.x(), kDefaultTileWidth);
    int top = getIndexByLength(m_screenRect.y(), kDefaultTileHeight);
    int right = getIndexByLength(m_screenRect.maxX(), kDefaultTileWidth);
    int buttom = getIndexByLength(m_screenRect.maxY(), kDefaultTileHeight);
    left = std::max(0, left - kXIndexDistanceToWillBeShowedTile);
    top = std::max(0, top - kYIndexDistanceToWillBeShowedTile);
    right = std::min(m_numTileX - 1, right + kXIndexDistanceToWillBeShowedTile);
    buttom = std::min(m_numTileY - 1, buttom + kYIndexDistanceToWillBeShowedTile);

    return blink::IntRect(left, top, right - left + 1, buttom - top + 1);
}

void TileGrid::updateSize(const blink::IntRect& screenRect, const blink::IntSize& newLayerSize)
{
    m_screenRect = screenRect;
    m_needBeShowedArea = blink::IntRect();

    if (m_layerSize == newLayerSize)
        return;
    m_layerSize = newLayerSize;

//     String outString = String::format("TileGrid::updateSize: %p, %d\n", m_layer, m_layerSize.width());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    //ASSERT(m_numTileX * m_numTileY == m_tiles->size());
    int newIndexNumX = getIndexNumByLength(newLayerSize.width(), kDefaultTileWidth);
    int newIndexNumY = getIndexNumByLength(newLayerSize.height(), kDefaultTileHeight);

    TilesAddr::realloByNewXY(&m_tilesAddr, newIndexNumX, newIndexNumY);
    m_numTileX = newIndexNumX;
    m_numTileY = newIndexNumY;

    DrawProps* prop = new DrawProps();
    m_layer->updataDrawToCanvasProperties(prop);
    mc::LayerTreeHost* host = m_layer->layerTreeHost();
    host->appendLayerChangeAction(new LayerChangeActionUpdataTile(host->genActionId(), m_layer->id(), newIndexNumX, newIndexNumY, prop));

//     String outString = String::format("TileGrid::updateSize: %d, %d\n", m_numTileX, m_numTileY);
//     OutputDebugStringW(outString.charactersWithNullTermination().data());
}

int TileGrid::getIndexByTile(const Tile* tile) const
{
    return tile->xIndex() + m_numTileX * tile->yIndex();
}

void TileGrid::doUpdateTilePriority(Tile* tile, Vector<size_t>* hasBitmapTiles, blink::IntRect* newCreatedWhenScrolling)
{
    if (tile->bitmap())
        hasBitmapTiles->append(getIndexByTile(tile));

    if (isInWillBeShowedArea(tile)) {
        if (0 == m_needBeShowedArea.height())
            m_needBeShowedArea = tile->postion();
        else
            m_needBeShowedArea.unite(tile->postion());

        if (TilePriorityNormal == tile->priority() /*&& !tile->dirtyRect().isEmpty()*/) { // 如果是被刚刷新出来的tile
            tile->setAllBoundDirty(); // 有可能在光栅化线程里脏矩形被清空了，所以只要是被刷出来的tile，都要设置脏矩形
            tile->increaseUseingRate();

            // 提交脏区域
            blink::IntRect dirtyRect = tile->dirtyRect();
            dirtyRect.move(tile->postion().x(), tile->postion().y());
            newCreatedWhenScrolling->unite(dirtyRect);
        }

        tile->setPriority(TilePriorityWillBeShowed);
    } else if (TilePriorityWillBeShowed == tile->priority()) { //  如果是刚被踢出显示区域的
        tile->setPriority(TilePriorityNormal);
        tile->setAllBoundDirty();
    }
}

void TileGrid::updateTilePriorityAndCommitInvalidate2(Vector<size_t>* hasBitmapTiles)
{
    blink::IntRect newCreatedWhenScrolling;

    for (int i = m_lastInWillBeShowedAreaPosIndex.x(); i < m_lastInWillBeShowedAreaPosIndex.maxX(); ++i) {
        for (int j = m_lastInWillBeShowedAreaPosIndex.y(); j < m_lastInWillBeShowedAreaPosIndex.maxY(); ++j) {
            Tile* tile = (Tile*)m_tilesAddr->getTileByXY(i, j, [] { return new Tile(); });
            if (!tile)
                continue;
            doUpdateTilePriority(tile, hasBitmapTiles, &newCreatedWhenScrolling);
        }
    }

    blink::IntRect pos = getInWillBeShowedAreaPos();

    for (int i = pos.x(); i < pos.maxX(); ++i) {
        for (int j = pos.y(); j < pos.maxY(); ++j) {
            if (i + m_numTileX * j >= m_tilesAddr->getSize())
                DebugBreak();
            Tile* tile = (Tile*)m_tilesAddr->getTileByXY(i, j, [] { return new Tile(); });
            if (!tile)
                continue;
            doUpdateTilePriority(tile, hasBitmapTiles, &newCreatedWhenScrolling);
        }
    }
    m_lastInWillBeShowedAreaPosIndex = pos;

    if (!newCreatedWhenScrolling.isEmpty())
        invalidate(newCreatedWhenScrolling, true);
}

void TileGrid::savaUnnecessaryTile(RasterTaskGroup* taskGroup, Vector<Tile*>* hasBitmapTiles)
{
    const int maxHasBitmapTiles = 2 * (m_needBeShowedArea.width()*m_needBeShowedArea.height()) / (kDefaultTileWidth*kDefaultTileHeight);
    if ((int)hasBitmapTiles->size() < maxHasBitmapTiles)
        return;

    m_tilesMutex->lock();
    int willWithoutBitmapCount = 0;
    int hasBitmapTilesSize = hasBitmapTiles->size();
    for (int i = hasBitmapTilesSize - 1; i >= 0; --i) {
        Tile* tile = hasBitmapTiles->at(i);
        
        if ((maxHasBitmapTiles + willWithoutBitmapCount > hasBitmapTilesSize) || TilePriorityNormal != tile->priority() || 1 != tile->getRefCnt()) {
            tile->unref(FROM_HERE);
            continue;
        }

        ASSERT(!isInWillBeShowedArea(tile));

        ++willWithoutBitmapCount;
        taskGroup->appendUnnecessaryTileToEvictAfterDrawFrame(tile);
    }
    m_tilesMutex->unlock();
}

struct CompareTileUsing {
    CompareTileUsing(TilesAddr* tiles)
        : m_tilesAddr(tiles) {}

    bool operator()(const size_t& leftIndex, const size_t& rightIndex)
    {
        if (leftIndex >= (size_t)(m_tilesAddr->getSize()) || rightIndex >= (size_t)(m_tilesAddr->getSize())) {
            ASSERT(false);
            return false;
        }
        const Tile* left = (Tile*)m_tilesAddr->getTileByIndex(leftIndex);
        const Tile* right = (Tile*)m_tilesAddr->getTileByIndex(rightIndex);
        return left->usingRate() > right->usingRate();
    }

    TilesAddr* m_tilesAddr;
};

void TileGrid::forceCleanupUnnecessaryTile()
{
    m_isForceCleanup = true;
}

void TileGrid::doCleanupUnnecessaryTile(size_t index, Tile* tile, LayerChangeActionCleanupUnnecessaryTile* cleanupAction)
{
    if (TilePriorityNormal != tile->priority() || 1 != tile->getRefCnt())
        return;
    ASSERT(!isInWillBeShowedArea(tile));

    tile->mutex().lock();
    tile->setPriority(TilePriorityNormal);
    tile->setAllBoundDirty();
    tile->clearBitmap();
    cleanupAction->appendTile(index, tile->xIndex(), tile->yIndex());
    tile->mutex().unlock();

    m_tilesAddr->remove(tile);
}

void TileGrid::cleanupUnnecessaryTiles(Vector<size_t>* hasBitmapTiles)
{
    int taskNum = RasterTaskWorkerThreadPool::shared()->getPendingRasterTaskNum();
    if (!m_isForceCleanup && 5 < taskNum /*&& (base::RandInt(0, 500) != 1)*/)
        return;

    const int maxHasBitmapTiles = 2 * (m_needBeShowedArea.width()*m_needBeShowedArea.height()) / (kDefaultTileWidth*kDefaultTileHeight);
    if (!m_isForceCleanup && (int)hasBitmapTiles->size() < maxHasBitmapTiles)
        return;

    LayerChangeActionCleanupUnnecessaryTile* cleanupAction = new LayerChangeActionCleanupUnnecessaryTile(layer()->id());
    if (m_isForceCleanup) {
        Vector<Tile*> unnecessaryTiles;
        for (TilesAddr::iterator it = m_tilesAddr->begin(); it != m_tilesAddr->end(); ++it) {
            TileBase* tileBase = it->value;
            Tile* tile = (Tile*)tileBase;
            if (!tile->bitmap())
                continue;

            unnecessaryTiles.append(tile);
        }
        for (size_t i = 0; i < unnecessaryTiles.size(); ++i) {
            Tile* tile = unnecessaryTiles.at(i);
            doCleanupUnnecessaryTile(m_tilesAddr->getIndexByTile(tile), tile, cleanupAction);
        }
    } else {
        CompareTileUsing compareTileUsing(m_tilesAddr);
        std::sort(hasBitmapTiles->begin(), hasBitmapTiles->end(), compareTileUsing);

        int willWithoutBitmapCount = 0;
        int hasBitmapTilesSize = hasBitmapTiles->size();
        for (int i = hasBitmapTilesSize - 1; i >= 0; --i) {
            size_t index = hasBitmapTiles->at(i);
            if ((int)index >= m_tilesAddr->getSize()) {
                ASSERT(false);
                continue;
            }
            Tile* tile = (Tile*)m_tilesAddr->getTileByIndex(index);

            if ((maxHasBitmapTiles + willWithoutBitmapCount > hasBitmapTilesSize))
                continue;

            ++willWithoutBitmapCount;
            doCleanupUnnecessaryTile(index, tile, cleanupAction);
        }
    }
    m_isForceCleanup = false;

    if (cleanupAction->isEmpty()) {
        delete cleanupAction;
        cleanupAction = nullptr;
        return;
    }

    LayerTreeHost* host = layer()->layerTreeHost();
    cleanupAction->setActionId(host->genActionId());
    host->appendLayerChangeAction(cleanupAction);
}

void TileGrid::update(blink::WebContentLayerClient* client, RasterTaskGroup* taskGroup, const blink::IntSize& newLayerSize, const blink::IntRect& screenRect)
{
    Vector<size_t> hasBitmapTiles;
    updateSize(screenRect, newLayerSize);    
    updateTilePriorityAndCommitInvalidate2(&hasBitmapTiles);
    applyDirtyRectsToRaster(client, taskGroup);
    cleanupUnnecessaryTiles(&hasBitmapTiles);
}

void TileGrid::markTileDirtyExceptNeedBeShowedArea(const blink::IntRect& dirtyRect)
{
    if (0 == m_numTileX || 0 == m_numTileY || dirtyRect.width() || dirtyRect.height())
        return;
    int left = getIndexByLength(dirtyRect.x(), kDefaultTileWidth);
    int top = getIndexByLength(dirtyRect.y(), kDefaultTileHeight);
    int right = getIndexByLength(dirtyRect.maxX(), kDefaultTileWidth);
    int buttom = getIndexByLength(dirtyRect.maxY(), kDefaultTileHeight);
    left = std::max(0, left);
    top = std::max(0, top);
    right = std::min(m_numTileX - 1, right);
    buttom = std::min(m_numTileY - 1, buttom);

    for (int i = left; i <= right; ++i) {
        for (int j = top; j <= buttom; ++j) {
            Tile* tile = (Tile*)m_tilesAddr->getTileByXY(i, j, [] { return new Tile(); });
            blink::IntRect tilePos = tile->postion();
            if (m_needBeShowedArea.intersects(tilePos) || !tilePos.intersects(dirtyRect)) // 在可视区域内的不记录了，直接记录在脏区域矩形里
                continue;

            blink::IntRect dirtyRectInTile = tilePos;
            dirtyRectInTile.intersect(dirtyRect);
            dirtyRectInTile.move(-tilePos.x(), -tilePos.y());
            ASSERT(dirtyRectInTile.x() >= 0 && dirtyRectInTile.y() >= 0 && dirtyRectInTile.maxX() <= tilePos.width() && dirtyRectInTile.maxY() <= tilePos.height());
            tile->appendDirtyRect(dirtyRectInTile);
        }
    }
}

const size_t kMaxRasterTaskNum = 3;

void TileGrid::invalidate(const blink::IntRect& rect, bool directSaveToDirtyRects)
{
    blink::IntRect dirtyRect = rect;
    blink::IntRect layerRect(blink::IntPoint(), (blink::IntSize)m_layer->bounds());
    dirtyRect.intersect(layerRect);

    bool isTooManyDirtyRects = m_dirtyRects.size() > kMaxRasterTaskNum;
    bool isIntersect = m_needBeShowedArea.intersects(dirtyRect);
    bool isContain = m_needBeShowedArea.contains(dirtyRect);
    if (isIntersect || directSaveToDirtyRects) {
        int maxDelta = -1000000;
        int maxDeltaIndex = 0;
        for (size_t i = 0; i < m_dirtyRects.size(); ++i) {
            blink::IntRect dirtyRectItem = m_dirtyRects[i];
            if (dirtyRectItem.contains(dirtyRect))
                return;

            int delta = intRectArea(&dirtyRectItem) + intRectArea(&dirtyRect) - intUnionArea(&dirtyRectItem, &dirtyRect);
            if (maxDelta < delta) {
                maxDelta = delta;
                maxDeltaIndex = i;
            }
        }
        if (isTooManyDirtyRects)
            m_dirtyRects[maxDeltaIndex].unite(dirtyRect);

//         String outString = String::format("TileGrid::invalidate: %d %d, %d %d\n", dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height());
//         OutputDebugStringW(outString.charactersWithNullTermination().data());

        m_dirtyRects.append(dirtyRect);
        mergeDirtyRectAndClipToCanBeShowedAreaIfNeeded(false);
    }

    if (directSaveToDirtyRects)
        return;

    // 如果超出区域，则简单记录
    if (!isContain)
        markTileDirtyExceptNeedBeShowedArea(dirtyRect);
}

void TileGrid::mergeDirtyRectAndClipToCanBeShowedAreaIfNeeded(bool needClip)
{
    const bool forceMerge = false;
    do {
        int nDirty = (int)m_dirtyRects.size();
        if (nDirty < 1)
            break;

        int bestDelta = forceMerge ? 0x7FFFFFFF : 0;
        int mergeA = 0;
        int mergeB = 0;
        for (int i = 0; i < nDirty - 1; i++) {
            if (needClip)// 有时候调用本函数的时候，m_needBeShowedArea还没成形
                m_dirtyRects[i].intersect(m_needBeShowedArea);

            for (int j = i + 1; j < nDirty; j++) {
                if (needClip)
                    m_dirtyRects[j].intersect(m_needBeShowedArea);
                
                int delta = intUnionArea(&m_dirtyRects[i], &m_dirtyRects[j]) - intRectArea(&m_dirtyRects[i]) - intRectArea(&m_dirtyRects[j]);
                if (bestDelta >= delta) {
                    mergeA = i;
                    mergeB = j;
                    bestDelta = delta;
                }
            }
        }

        if (needClip && m_dirtyRects.size() > 0)
            m_dirtyRects[0].intersect(m_needBeShowedArea);

        if (mergeA == mergeB)
            break;

//         String outString = String::format("TileGrid::mergeDirtyRectAndClipToCanBeShowedAreaIfNeeded: (%d %d, %d %d) (%d %d, %d %d)\n", 
//             m_dirtyRects[mergeA].x(), m_dirtyRects[mergeA].y(), m_dirtyRects[mergeA].width(), m_dirtyRects[mergeA].height(),
//             m_dirtyRects[mergeB].x(), m_dirtyRects[mergeB].y(), m_dirtyRects[mergeB].width(), m_dirtyRects[mergeB].height());
//         OutputDebugStringW(outString.charactersWithNullTermination().data());

        m_dirtyRects[mergeA].unite(m_dirtyRects[mergeB]);
        for (int i = mergeB + 1; i < nDirty; i++)
            m_dirtyRects[i - 1] = m_dirtyRects[i];

        m_dirtyRects.removeLast();
    } while (true);
}

// 录制并发送光栅化
void TileGrid::applyDirtyRectsToRaster(blink::WebContentLayerClient* client, RasterTaskGroup* taskGroup)
{
    if (0 == m_numTileX || 0 == m_numTileY)
        return;

    // TODO 裁剪超出可绘制区域，并合并脏矩形
    mergeDirtyRectAndClipToCanBeShowedAreaIfNeeded(true);

    for (size_t i = 0; i < m_dirtyRects.size(); ++i) {
        blink::IntRect dirtyRect = m_dirtyRects[i];
        if (0 == dirtyRect.width() || 0 == dirtyRect.height())
            continue;

//         String outString = String::format("TileGrid::applyDirtyRectsToRaster:%d %d, %d %d\n", 
//             dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height());
//         OutputDebugStringW(outString.charactersWithNullTermination().data());

        SkRTreeFactory factory;
        SkPictureRecorder* recorder = new SkPictureRecorder();
        SkCanvas* canvas = recorder->beginRecording((SkRect)dirtyRect, &factory, 0);
        client->paintContents(canvas, blink::WebRect(dirtyRect));
        SkPicture* picture = recorder->endRecordingAsPicture();
        delete recorder;
        
        TileActionInfoVector* willRasteredTiles = new TileActionInfoVector();

        int left = getIndexByLength(dirtyRect.x(), kDefaultTileWidth);
        int top = getIndexByLength(dirtyRect.y(), kDefaultTileHeight);
        int right = getIndexByLength(dirtyRect.maxX(), kDefaultTileWidth);
        int buttom = getIndexByLength(dirtyRect.maxY(), kDefaultTileHeight);
        left = std::max(0, left);
        top = std::max(0, top);
        right = std::min(m_numTileX - 1, right);
        buttom = std::min(m_numTileY - 1, buttom);

        for (int i = left; i <= right; ++i) {
            for (int j = top; j <= buttom; ++j) {
                Tile* tile = (Tile*)m_tilesAddr->getTileByXY(i, j, [] { return new Tile(); });
                blink::IntRect tilePos = tile->postion();
                if (!tilePos.intersects(dirtyRect))
                    continue;

                if (picture && isInWillBeShowedArea(tile)) {
                    tile->clearDirtyRect();
                    tile->allocBitmapIfNeeded();
                    tile->increaseUseingRate();

                    // 发去光栅化;
                    willRasteredTiles->append(getIndexByTile(tile), tile->xIndex(), tile->yIndex());                    
                }
            }
        }

        if (0 != willRasteredTiles->size()) {
            picture->ref();
            int64 actionId = taskGroup->postRasterTask(layer(), picture, willRasteredTiles, (SkRect)dirtyRect);
        } else {
            delete willRasteredTiles;
            willRasteredTiles = nullptr;
        }

        picture->unref();
    }
    m_dirtyRects.clear();
}

void TileGrid::drawToCanvas(mc_blink::WebLayerImpl* webLayer, blink::WebCanvas* canvas, const blink::IntRect& clip)
{
}

} // mc