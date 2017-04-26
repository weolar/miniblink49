#include "cc/tiles/TileGrid.h"

#include "third_party/WebKit/public/platform/WebContentLayerClient.h"
#include "third_party/WebKit/Source/platform/geometry/win/IntRectWin.h"
#include "cc/tiles/TileWidthHeight.h"
#include "cc/tiles/Tile.h"
#include "cc/blink/WebLayerImpl.h"
#include "cc/raster/RasterTaskWorkerThreadPool.h"
#include "cc/trees/LayerTreeHost.h"
#include "cc/playback/TileActionInfo.h"
#include "cc/playback/LayerChangeAction.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/WebKit/Source/platform/graphics/GraphicsContext.h"

#include "platform/image-encoders/gdiplus/GDIPlusImageEncoder.h" // TODO
#include "base/rand_util.h"

namespace content {
extern int debugPaint;
extern int debugPaintTile;
}

namespace blink {
bool saveDumpFile(const String& url, char* buffer, unsigned int size);
}

namespace cc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, tileGridCount, ("ccTileGrid"));
#endif

const int kXIndexDistanceToWillBeShowedTile = 0;// 1;
const int kYIndexDistanceToWillBeShowedTile = 0;// 2;

TileGrid::TileGrid(cc_blink::WebLayerImpl* layer)
{
    m_rasterTaskCount = 0;
    m_numTileX = 0;
    m_numTileY = 0;
    m_layer = layer;
    m_willShutdown = false;
    m_registerTileMutex = new WTF::Mutex();
    m_tiles = new Vector<Tile*>();
    m_tilesMutex = nullptr;

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

    for (size_t i = 0; i < m_tiles->size(); ++i) {
        Tile* tile = m_tiles->at(i);
        tile->unref(FROM_HERE);
    }

	if (0 != m_registerTiles.size()) {
		OutputDebugStringA("TileGrid::~TileGrid: 0 != m_registerTiles.size():\n");

		Tile* xxTile = m_registerTiles[0];
		Vector<TileTraceLocation*>* locations = xxTile->refFrom();
		for (size_t j = 0; j < locations->size(); ++j) {
			TileTraceLocation* location = locations->at(j);
			OutputDebugStringA(location->functionName());
			OutputDebugStringA("\n");
		}
		xxTile = m_registerTiles[1];
	}
    ASSERT(0 == m_registerTiles.size());

	m_layer = nullptr;

    delete m_tiles;
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
//     RasterTaskWorkerThreadPool* debugPool = cc::RasterTaskWorkerThreadPool::shared();
//     while (m_rasterTaskCount != 0) { ::Sleep(50); }
}

cc_blink::WebLayerImpl* TileGrid::layer() const
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

Tile* TileGrid::getTileByXY(int xIndex, int yIndex)
{
    if (m_numTileX <= xIndex || m_numTileY <= yIndex)
        return nullptr;
    return *(m_tiles->data() + m_numTileX * yIndex + xIndex);
}

// ����һ�����ȣ���ȡ�������ĸ�tile��
static int getIndexByLength(int length, int tileLength)
{
    return (int)(length / tileLength) /*+ (length % tileLength != 0 ? 1 : 0)*/;
}

// ����һ�����ȣ���ȡһ����Ҫ���ٸ�tile
static int getIndexNumByLength(int length, int tileLength)
{
    return (int)(length / tileLength) + (length % tileLength != 0 ? 1 : 0);
}

bool TileGrid::isInWillBeShowedArea(Tile* tile)
{
    int xIndex = tile->xIndex();
    int yIndex = tile->yIndex();
    int left = getIndexByLength(m_screenRect.x() , kDefaultTileWidth);
    int top = getIndexByLength(m_screenRect.y(), kDefaultTileHeight);
    int right = getIndexByLength(m_screenRect.maxX(), kDefaultTileWidth);
    int buttom = getIndexByLength(m_screenRect.maxY(), kDefaultTileHeight);
    left = std::max(0, left - kXIndexDistanceToWillBeShowedTile);
    top = std::max(0, top - kYIndexDistanceToWillBeShowedTile);
    right = std::min(m_numTileX, right + kXIndexDistanceToWillBeShowedTile);
    buttom = std::min(m_numTileY, buttom + kYIndexDistanceToWillBeShowedTile);

    if (xIndex < left || xIndex > right || yIndex < top || yIndex > buttom)
        return false;
    return true;
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

    ASSERT(m_numTileX * m_numTileY == m_tiles->size());
    int newIndexNumX = getIndexNumByLength(newLayerSize.width(), kDefaultTileWidth);
    int newIndexNumY = getIndexNumByLength(newLayerSize.height(), kDefaultTileHeight);

    Vector<Tile*>* newTiles = new Vector<Tile*>;
    newTiles->resize(newIndexNumX * newIndexNumY);
    int index = 0;
    for (int y = 0; y < newIndexNumY; ++y) {
        for (int x = 0; x < newIndexNumX; ++x) {
            Tile* tile = getTileByXY(x, y);
            if (!tile)
                tile = new Tile(this, x, y);
            else {
                ASSERT(x == tile->xIndex() && y == tile->yIndex());
                tile->ref(FROM_HERE);
            }
            
            newTiles->at(index) = tile;
            ++index;
        }
    }
    for (size_t i = 0; i < m_tiles->size(); ++i) {
        Tile* tile = m_tiles->at(i);
        tile->unref(FROM_HERE);
    }
    delete m_tiles;
    m_tiles = newTiles;
    m_numTileX = newIndexNumX;
    m_numTileY = newIndexNumY;

    cc::LayerTreeHost* host = m_layer->layerTreeHost();
    host->appendLayerChangeAction(new LayerChangeActionUpdataTile(host->genActionId(), m_layer->id(), newIndexNumX, newIndexNumY));

// 	String outString = String::format("TileGrid::updateSize: id %d, %d %d\n", m_layer->id(), newLayerSize.width(), newLayerSize.height());
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void TileGrid::updateTilePriorityAndCommitInvalidate(Vector<size_t>* hasBitmapTiles)
{
    // ��������tile:
    // 1����¼��bitmap��tile��
    // 2������ǹ����������µ�tile�����Ƿ��࣬��������ύˢ�£�
    // 3�����������ص�tile��Ҫ����
    // 4�������bitmap��tile̫�࣬�ѵ�һ����¼������tileɾ��һЩ��������
    
    int x = 0;
    int y = 0;

//     String outString = String::format("TileGrid::updateTilePriorityAndCommitInvalidate 1: %d\n", m_needBeShowedArea.height());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());
    blink::IntRect newCreatedWhenScrolling;
    Vector<Tile*> debugTiles;

    for (size_t i = 0; i < m_tiles->size(); ++i) {
        Tile* tile = m_tiles->at(i);
        if (tile->bitmap())
            hasBitmapTiles->append(i);

        if (isInWillBeShowedArea(tile)) {
            if (0 == m_needBeShowedArea.height())
                m_needBeShowedArea = tile->postion();
            else
                m_needBeShowedArea.unite(tile->postion());

//             String outString = String::format("TileGrid::updateTilePriorityAndCommitInvalidate tile: %d %d, %d %d %d\n",
//                 tile->xIndex(), tile->yIndex(), tile->priority(), tile->dirtyRect().width(), tile->dirtyRect().height());
//             OutputDebugStringW(outString.charactersWithNullTermination().data());

            if (TilePriorityNormal == tile->priority() /*&& !tile->dirtyRect().isEmpty()*/) { // ����Ǳ���ˢ�³�����tile
                tile->setAllBoundDirty(); // �п����ڹ�դ���߳�������α�����ˣ�����ֻҪ�Ǳ�ˢ������tile����Ҫ���������
                tile->increaseUseingRate();

                // �ύ������
                blink::IntRect dirtyRect = tile->dirtyRect();
                dirtyRect.move(tile->postion().x(), tile->postion().y());
                newCreatedWhenScrolling.unite(dirtyRect);
                debugTiles.append(tile);
            }
             
            tile->setPriority(TilePriorityWillBeShowed);
        } else if (TilePriorityWillBeShowed == tile->priority()) { //  ����Ǹձ��߳���ʾ�����
            tile->setPriority(TilePriorityNormal);
            tile->setAllBoundDirty();
        }
    }

    if (!newCreatedWhenScrolling.isEmpty()) {
        if (newCreatedWhenScrolling.width() > 1000 && newCreatedWhenScrolling.height() > 700) {
            for (size_t i = 0; i < debugTiles.size(); ++i) {
                Tile* tile = debugTiles.at(i);
            }
        }
        invalidate(newCreatedWhenScrolling, true);
    }

//     outString = String::format("TileGrid::updateTilePriorityAndCommitInvalidate 2: %d\n\n", m_needBeShowedArea.height());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void TileGrid::savaUnnecessaryTile(RasterTaskGroup* taskGroup, Vector<Tile*>* hasBitmapTiles)
{
    const int maxHasBitmapTiles = 2 * (m_needBeShowedArea.width()*m_needBeShowedArea.height()) / (kDefaultTileWidth*kDefaultTileHeight);
    if (hasBitmapTiles->size() < maxHasBitmapTiles)
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

// static bool compareTileUsing(Tile*& left, Tile*& right)
// { 
//     return left->usingRate() > right->usingRate();
// }

struct CompareTileUsing
{
	CompareTileUsing(Vector<Tile*>* tiles)
		: m_tiles(tiles) {}

	bool operator()(const size_t& leftIndex, const size_t& rightIndex)
	{
		if (leftIndex >= (size_t)(m_tiles->size()) || rightIndex >= (size_t)(m_tiles->size())) {
			ASSERT(false);
			return false;
		}
		const Tile* left = m_tiles->at(leftIndex);
		const Tile* right = m_tiles->at(rightIndex);
		return left->usingRate() > right->usingRate();
	}

	Vector<Tile*>* m_tiles;
};

void TileGrid::cleanupUnnecessaryTile(Vector<size_t>* hasBitmapTiles)
{
    int taskNum = RasterTaskWorkerThreadPool::shared()->pendingRasterTaskNum();
    if (0 != taskNum /*&& (base::RandInt(0, 500) != 1)*/)
        return;

    const int maxHasBitmapTiles = 2 * (m_needBeShowedArea.width()*m_needBeShowedArea.height()) / (kDefaultTileWidth*kDefaultTileHeight);
    if (hasBitmapTiles->size() < maxHasBitmapTiles)
        return;

	LayerChangeActionCleanupUnnecessaryTile* cleanupAction = new LayerChangeActionCleanupUnnecessaryTile(layer()->id());

	CompareTileUsing compareTileUsing(m_tiles);
    std::sort(hasBitmapTiles->begin(), hasBitmapTiles->end(), compareTileUsing);

    int willWithoutBitmapCount = 0;
    int hasBitmapTilesSize = hasBitmapTiles->size();
    for (int i = hasBitmapTilesSize - 1; i >= 0; --i) {
		size_t index = hasBitmapTiles->at(i);
		if (index >= m_tiles->size()) {
			ASSERT(false);
			continue;
		}
		Tile* tile = m_tiles->at(index);

        if ((maxHasBitmapTiles + willWithoutBitmapCount > hasBitmapTilesSize) || TilePriorityNormal != tile->priority() || 1 != tile->getRefCnt())
            continue;

        ASSERT(!isInWillBeShowedArea(tile));

        ++willWithoutBitmapCount;
        
        MutexLocker locker(tile->mutex());
        tile->setPriority(TilePriorityNormal);
        tile->setAllBoundDirty();
        tile->clearBitmap();
		cleanupAction->appendTile(index, tile->xIndex(), tile->yIndex());
    }

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
    updateTilePriorityAndCommitInvalidate(&hasBitmapTiles);
    applyDirtyRectsToRaster(client, taskGroup);
    cleanupUnnecessaryTile(&hasBitmapTiles);
}

void TileGrid::markTileDirtyExceptNeedBeShowedArea(const blink::IntRect& dirtyRect)
{
    for (size_t j = 0; j < m_tiles->size(); ++j) {
        Tile* tile = m_tiles->at(j);
        blink::IntRect tilePos = tile->postion();
        if (m_needBeShowedArea.intersects(tilePos) || !tilePos.intersects(dirtyRect)) // �ڿ��������ڵĲ���¼�ˣ�ֱ�Ӽ�¼�������������
            continue;

        blink::IntRect dirtyRectInTile = tilePos;
        dirtyRectInTile.intersect(dirtyRect);
        dirtyRectInTile.move(-tilePos.x(), -tilePos.y());
        ASSERT(dirtyRectInTile.x() >= 0 && dirtyRectInTile.y() >= 0 && dirtyRectInTile.maxX() <= tilePos.width() && dirtyRectInTile.maxY() <= tilePos.height());
        tile->appendDirtyRect(dirtyRectInTile);
    }
}

void TileGrid::invalidate(const blink::IntRect& rect, bool directSaveToDirtyRects)
{
    blink::IntRect dirtyRect = rect;
    blink::IntRect layerRect(blink::IntPoint(), (blink::IntSize)m_layer->bounds());
    dirtyRect.intersect(layerRect);
    if (m_dirtyRects.size() > 100) {
        OutputDebugStringA("TileGrid::invalidate > 100 \n");
        DebugBreak();
    }    

    bool isIntersect = m_needBeShowedArea.intersects(dirtyRect);
    bool isContain = m_needBeShowedArea.contains(dirtyRect);
    if (isIntersect || directSaveToDirtyRects) {
        for (size_t i = 0; i < m_dirtyRects.size(); ++i) {
            blink::IntRect dirtyRectItem = m_dirtyRects[i];
            if (dirtyRectItem.contains(dirtyRect))
                return;
        }

//         String outString = String::format("TileGrid::invalidate: %d %d, %d %d\n", dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height());
//         OutputDebugStringW(outString.charactersWithNullTermination().data());

        m_dirtyRects.append(dirtyRect);
        mergeDirtyRectAndClipToCanBeShowedAreaIfNeeded(false);
    }

    if (directSaveToDirtyRects)
        return;

    // �������������򵥼�¼
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
            if (needClip)// ��ʱ����ñ�������ʱ��m_needBeShowedArea��û����
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

// ¼�Ʋ����͹�դ��
void TileGrid::applyDirtyRectsToRaster(blink::WebContentLayerClient* client, RasterTaskGroup* taskGroup)
{
    // TODO �ü������ɻ������򣬲��ϲ������
    mergeDirtyRectAndClipToCanBeShowedAreaIfNeeded(true);
    
    for (size_t i = 0; i < m_dirtyRects.size(); ++i) {
        blink::IntRect dirtyRect = m_dirtyRects[i];

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

        for (size_t j = 0; j < m_tiles->size(); ++j) {
            Tile* tile = m_tiles->at(j);
            blink::IntRect tilePos = tile->postion();
            if (!tilePos.intersects(dirtyRect))
                continue;

            if (picture && isInWillBeShowedArea(tile)) {
                tile->clearDirtyRect();
                tile->allocBitmapIfNeeded();
                tile->increaseUseingRate();

                // ��ȥ��դ��;
                willRasteredTiles->append(j, tile->xIndex(), tile->yIndex());
            }
        }

        if (0 != willRasteredTiles->size()) {
            picture->ref();
            taskGroup->postRasterTask(layer(), picture, willRasteredTiles, dirtyRect);
        } else {
            delete willRasteredTiles;
            willRasteredTiles = nullptr;
        }

        picture->unref();
    }
    m_dirtyRects.clear();
}

void TileGrid::drawToCanvas(cc_blink::WebLayerImpl* webLayer, blink::WebCanvas* canvas, const blink::IntRect& clip)
{
//     if (!webLayer->drawsContent())
//         return;
// 
//     int opacity = (int)(255 * webLayer->opacity());
// 
//     for (size_t i = 0; i < m_tiles->size(); ++i) {
//         Tile* tile = m_tiles->at(i);
//         if (!tile->postion().intersects(clip)/* || !tile->bitmap()*/) // weolar
//             continue;
// //         if (!tile->bitmap() && !tile->isNotInit())
// //             DebugBreak();
//         if (!tile->bitmap())
//             continue;
// 
//         MutexLocker locker(tile->mutex());
// 
// //         if (content::debugPaintTile == i)
// //             OutputDebugStringA("");
// 
//         if (0) {
//             int index = 0;
//             for (size_t j = content::debugPaintTile; j < m_tiles->size(); ++j, ++index) {
//                 if (index > 10)
//                     break;
// 
//                 Tile* tile = m_tiles->at(j);
//                 Vector<unsigned char> output;
//                 blink::GDIPlusImageEncoder::encode(*tile->bitmap(), blink::GDIPlusImageEncoder::PNG, &output);
//                 blink::saveDumpFile("", (char*)output.data(), output.size());
//             }
//         }
// 
//         blink::IntRect tilePostion = tile->postion();
//         //tilePostion.intersect(clip);
//         SkIRect dst = (SkIRect)(tilePostion);
//         SkIRect src = SkIRect::MakeWH(tile->bitmap()->width(), tile->bitmap()->height());
// 
//         SkPaint paint;
//         paint.setAntiAlias(false);
//         paint.setAlpha(opacity);
//         //paint.setFilterQuality(kLow_SkFilterQuality);
// 
// 		// debug
// //         OwnPtr<blink::GraphicsContext> context = blink::GraphicsContext::deprecatedCreateWithCanvas(canvas, blink::GraphicsContext::NothingDisabled);
// //         context->setStrokeStyle(blink::SolidStroke);
// //         context->setStrokeColor(0xff000000 | (::GetTickCount() + rand()));
// //         context->drawLine(blink::IntPoint(tilePostion.x(), tilePostion.y()), blink::IntPoint(tilePostion.maxX(), tilePostion.maxY()));
// //         context->drawLine(blink::IntPoint(tilePostion.maxX(), tilePostion.y()), blink::IntPoint(tilePostion.x(), tilePostion.maxY()));
// //         context->strokeRect(tilePostion, 2);
// //         context->fillRect(tilePostion, 0x00000000 | (::GetTickCount() + rand()));
//         
//         canvas->drawBitmapRect(*tile->bitmap(), nullptr, SkRect::MakeFromIRect(dst), &paint);
//     }
}

} // cc