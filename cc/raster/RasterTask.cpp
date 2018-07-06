/*
* Copyright (C) 2013 weolar Inc.  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following disclaimer
* in the documentation and/or other materials provided with the
* distribution.
*     * Neither the name of weolar Inc. nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*/

#include "config.h"
#include "cc/raster/RasterTask.h"

#include "cc/raster/RasterResouce.h"
#include "cc/raster/RasterFilters.h"
#include "cc/tiles/Tile.h"
#include "cc/tiles/TileGrid.h"
#include "cc/tiles/TileWidthHeight.h"
#include "cc/blink/WebLayerImpl.h"
#include "cc/blink/WebFilterOperationsImpl.h"
#include "cc/trees/LayerTreeHost.h"
#include "cc/trees/DrawProperties.h"
#include "cc/playback/LayerChangeAction.h"
#include "cc/playback/TileActionInfo.h"
#include "skia/ext/refptr.h"
#include "skia/ext/analysis_canvas.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/WebKit/Source/platform/image-encoders/gdiplus/GDIPlusImageEncoder.h"

extern DWORD g_rasterTaskCount;
extern float g_contentScale;

namespace blink {
bool saveDumpFile(const String& url, char* buffer, unsigned int size);
}

namespace content {
extern int debugPaint;
extern int debugPaintTile;
}

namespace cc {

static RasterTaskWorkerThreadPool* s_sharedThreadPool = nullptr;

RasterTaskWorkerThreadPool::RasterTaskWorkerThreadPool(int threadNum)
{
    m_willShutdown = false;
    m_pendingRasterTaskNum = 0;
    init(threadNum);
    srand(34567);
}

RasterTaskWorkerThreadPool::~RasterTaskWorkerThreadPool()
{
}

void RasterTaskWorkerThreadPool::init(int threadNum)
{
    if (threadNum <= 0)
        threadNum = 1;

    for (int i = 0; i < threadNum; ++i) {
        m_threads.append(blink::Platform::current()->createThread("RasterTaskWorkerThreadPool"));
        m_threadBusyCount.append(0);
    }
}

void RasterTaskWorkerThreadPool::shutdown()
{
    ASSERT(s_sharedThreadPool);
    m_willShutdown = true;
    while (m_pendingRasterTaskNum > 0) { Sleep(20); }

    for (size_t i = 0; i < m_threads.size(); ++i) {
        delete m_threads[i];
    }
    m_threads.clear();
}

RasterTaskWorkerThreadPool* RasterTaskWorkerThreadPool::shared()
{
    ASSERT(isMainThread());
    if (nullptr == s_sharedThreadPool)
        s_sharedThreadPool = new RasterTaskWorkerThreadPool(1);
    return s_sharedThreadPool;
}

int RasterTaskWorkerThreadPool::selectOneIdleThread()
{
    int minBusy = m_threadBusyCount[0];
    int minBusyIndex = 0;
    for (size_t i = 0; i < m_threadBusyCount.size(); ++i) {
        if (minBusy > m_threadBusyCount[i]) {
            minBusy = m_threadBusyCount[i];
            minBusyIndex = i;
        }
    }
    return minBusyIndex;
}

void RasterTaskWorkerThreadPool::increasePendingRasterTaskNum()
{
    atomicIncrement(&m_pendingRasterTaskNum);
}

void RasterTaskWorkerThreadPool::decreasePendingRasterTaskNum() 
{
    atomicDecrement(&m_pendingRasterTaskNum);
}

int RasterTaskWorkerThreadPool::getPendingRasterTaskNum() const
{
    return m_pendingRasterTaskNum;
}

void RasterTaskWorkerThreadPool::increaseBusyCountByIndex(int index)
{
    atomicIncrement(&m_threadBusyCount[index]);
}

void RasterTaskWorkerThreadPool::decreaseBusyCountByIndex(int index)
{
    atomicDecrement(&m_threadBusyCount[index]);
}

class RasteredTileBitmap {
public:
    RasteredTileBitmap()
    {
        m_isSolidColor = false;
    }

    ~RasteredTileBitmap()
    {

    }

    bool isSolidColor() const
    {
        return m_isSolidColor;
    }

private:
    int xIndex;
    int yIndex;
    bool m_isSolidColor;
    SkColor m_solidColor;
};

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, rasterTaskCounter, ("ccRasterTask"));
#endif

class RasterTask : public blink::WebThread::Task {
public:
    explicit RasterTask(
        RasterTaskWorkerThreadPool* pool, 
        SkPicture* picture,
        const SkRect& dirtyRect, 
        int threadIndex,
        bool isOpaque,
        const cc_blink::WebFilterOperationsImpl* filterOperations,
        LayerChangeActionBlend* blendAction,
        RasterTaskGroup* group
        )
        : m_pool(pool)
        , m_picture(picture)
        , m_dirtyRect(dirtyRect)
        , m_threadIndex(threadIndex)
        , m_isOpaque(isOpaque)
        , m_blendAction(blendAction)
        , m_group(group)
        , m_filterOperations(filterOperations ? new cc_blink::WebFilterOperationsImpl(*filterOperations) : nullptr)
        , m_contentScale(g_contentScale)
    {
#ifndef NDEBUG
        rasterTaskCounter.increment();
#endif
    }

    ~RasterTask()
    {
#ifndef NDEBUG
        rasterTaskCounter.decrement();
#endif
    }

    void releaseRource()
    {
        m_picture->unref();
        m_group->unref();

        m_pool->decreasePendingRasterTaskNum();
        m_pool->decreaseBusyCountByIndex(m_threadIndex);
    }

    class RasterResouceLocker {
        WTF_MAKE_NONCOPYABLE(RasterResouceLocker);
    public:
        RasterResouceLocker(Mutex* lockable, RasterTask* task) 
            : m_lockable(lockable)
            , m_task(task)
        {
            m_lockable->lock();
        }
        ~RasterResouceLocker() 
        {
            m_lockable->unlock();
        }
    private:
        Mutex* m_lockable;
        RasterTask* m_task;
    };

    virtual void run() override
    {
        DWORD nowTime = (DWORD)(WTF::currentTimeMS() * 100);
        raster();
        releaseRource();
        g_rasterTaskCount++;

        DWORD nowTime2 = (DWORD)(WTF::currentTimeMS() * 100);
        
//         String output = String::format("RasterTask.run: %d\n", nowTime2 - nowTime);
//         OutputDebugStringA(output.utf8().data());
    }

    bool performSolidColorAnalysis(const SkRect& tilePos, SkColor* color)
    {
        skia::AnalysisCanvas canvas(tilePos.width(), tilePos.height());
        canvas.translate(-tilePos.x(), -tilePos.y());
        canvas.clipRect(tilePos, SkRegion::kIntersect_Op);
        m_picture->playback(&canvas, &canvas);

        return canvas.GetColorIfSolid(color);
    }

    void raster()
    {
        const Vector<TileActionInfo*>& infos = m_blendAction->getWillRasteredTiles()->infos();
#if 0
        for (size_t i = 0; i < infos.size(); ++i) {
            TileActionInfo* info = infos[i];
            SkRect tilePos = SkRect::MakeXYWH(info->xIndex * kDefaultTileWidth, info->yIndex * kDefaultTileHeight, kDefaultTileWidth, kDefaultTileHeight);
            tilePos.intersect(m_dirtyRect);

            if (1 == info->xIndex && 0 == info->yIndex)
                OutputDebugStringA("");

            info->m_isSolidColor = performSolidColorAnalysis(tilePos, &info->m_solidColor);

            ASSERT(!info->m_bitmap);
            if (!info->m_isSolidColor)
                info->m_bitmap = doRaster(m_dirtyRect);
        }
#elif 1
        for (size_t i = 0; i < infos.size(); ++i) {
            TileActionInfo* info = infos[i];
            ASSERT(!info->m_solidColor);
            SkRect tilePos = SkRect::MakeXYWH(info->xIndex * kDefaultTileWidth, info->yIndex * kDefaultTileHeight, kDefaultTileWidth, kDefaultTileHeight);
            SkRect dirtyRectInTile(tilePos);

            dirtyRectInTile.intersect(m_dirtyRect);
            SkColor solidColor;
            bool isSolidColor = performSolidColorAnalysis(dirtyRectInTile, &solidColor);

            if (blink::RuntimeEnabledFeatures::alwaysIsNotSolideColorEnabled())
                isSolidColor = false;

            if (isSolidColor) {
                info->m_solidColor = new SkColor(solidColor);
                info->m_isSolidColorCoverWholeTile = m_dirtyRect.contains(tilePos);
            }
        }

        SkBitmap* bitmap = doRaster(m_dirtyRect);

        m_blendAction->setDirtyRectBitmap(bitmap);
        m_blendAction->setContentScale(m_contentScale);

        if (0) {
            Vector<unsigned char> output;
            blink::GDIPlusImageEncoder::encode(*bitmap, blink::GDIPlusImageEncoder::PNG, &output);
            blink::saveDumpFile("E:\\mycode\\miniblink49\\trunk\\out\\1.png", (char*)output.data(), output.size());
        }
#endif
    }

    SkBitmap* doRaster(const SkRect& dirtyRect)
    {
        SkBitmap* bitmap = new SkBitmap;
        bitmap->allocN32Pixels(dirtyRect.width(), dirtyRect.height());

        // Uses kPremul_SkAlphaType since the result is not known to be opaque.
        SkImageInfo info = SkImageInfo::MakeN32(dirtyRect.width(), dirtyRect.height(), m_isOpaque ? kOpaque_SkAlphaType : kPremul_SkAlphaType); // TODO
        SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);
        size_t stride = info.minRowBytes();
        skia::RefPtr<SkSurface> surface = skia::AdoptRef(SkSurface::NewRasterDirect(info, bitmap->getPixels(), stride, &surfaceProps));
        skia::RefPtr<SkCanvas> canvas = skia::SharePtr(surface->getCanvas());

        SkPaint paint;
        paint.setAntiAlias(false);

        if (!m_isOpaque)
            bitmap->eraseARGB(0, 0xff, 0xff, 0xff); // TODO

        canvas->save();
        canvas->scale(m_contentScale, m_contentScale);
        canvas->translate(-dirtyRect.x(), -dirtyRect.y());
        canvas->drawPicture(m_picture, nullptr, nullptr);
        canvas->restore();

        skia::RefPtr<SkImageFilter> filter = RasterFilter::buildImageFilter(m_filterOperations, blink::FloatSize(dirtyRect.width(), dirtyRect.height()));
        // TODO(ajuma): Apply the filter in the same pass as the content where
        // possible (e.g. when there's no origin offset). See crbug.com/308201.
        return applyImageFilter(filter.get(), bitmap);        
    }

    SkBitmap* applyImageFilter(SkImageFilter* filter, SkBitmap* toFilter) const
    {
        if (!filter)
            return toFilter;

        SkBitmap* filterBitmap = new SkBitmap();
        if (!filterBitmap->tryAllocPixels(SkImageInfo::MakeN32Premul(toFilter->width(), toFilter->height()))) {
            delete filterBitmap;
            return toFilter;
        }

        SkCanvas canvas(*filterBitmap);
        SkPaint paint;
        paint.setImageFilter(filter);
        canvas.clear(SK_ColorTRANSPARENT);
        canvas.drawBitmap(*toFilter, 0, 0, &paint);
        delete toFilter;
        return filterBitmap;
    }

    int threadIndex() const { return m_threadIndex; }

    int64 getActionId() const
    {
        if (m_blendAction)
            return m_blendAction->actionId();
        return -1;
    }

private:
    RasterTaskWorkerThreadPool* m_pool;
    SkPicture* m_picture;
    SkRect m_dirtyRect;
    int m_threadIndex;
    LayerChangeActionBlend* m_blendAction;
    bool m_isOpaque;
    RasterTaskGroup* m_group;
    const cc_blink::WebFilterOperationsImpl* m_filterOperations;

    float m_contentScale; // 绘制低分辨率的时候用
};

RasterTaskGroup* RasterTaskWorkerThreadPool::beginPostRasterTask(LayerTreeHost* host)
{
    return new RasterTaskGroup(this, host);
}

//////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, rasterTaskGroupCount, ("ccRasterTaskGroup"));
#endif

RasterTaskGroup::RasterTaskGroup(RasterTaskWorkerThreadPool* pool, LayerTreeHost* host)
{
    m_ref = 1;
    m_mutex = new WTF::Mutex();
    m_pool = pool;
    m_host = host;
    m_drawPropUpdataAction = new LayerChangeActionDrawPropUpdata();
    m_lastBlendActionForPendingInvalidateRect = nullptr;
//     String outString = String::format("RasterTaskGroup::RasterTaskGroup: %p\n", this);
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

#ifndef NDEBUG
    rasterTaskGroupCount.increment();
#endif
}

RasterTaskGroup::~RasterTaskGroup()
{
#ifndef NDEBUG
    rasterTaskGroupCount.decrement();
#endif
}

void RasterTaskGroup::postImageLayerAction(int imageLayerId, SkBitmapRefWrap* bitmap)
{
    m_blendAndImageActions.append(new LayerChangeActionUpdataImageLayer(m_host->genActionId(), imageLayerId, bitmap));
}

int RasterTaskGroup::getPendingRasterTaskNum() const
{
    return m_pool->getPendingRasterTaskNum();
}

int64 RasterTaskGroup::postRasterTask(cc_blink::WebLayerImpl* layer, SkPicture* picture, TileActionInfoVector* willRasteredTiles, const SkRect& dirtyRect)
{
    ref();

    m_host->enablePaint();

    int layerId = layer->id();
    
    LayerChangeActionBlend* blendAction = new LayerChangeActionBlend(m_host->genActionId(), layerId, willRasteredTiles, dirtyRect);
    m_blendAndImageActions.append(blendAction);
    m_lastBlendActionForPendingInvalidateRect = blendAction;

    int threadIndex = m_pool->selectOneIdleThread();

    RasterTask* task = new RasterTask(m_pool, picture, dirtyRect, threadIndex, layer->opaque(), layer->getFilters(), blendAction, this);
    m_pool->increasePendingRasterTaskNum();
    m_pool->increaseBusyCountByIndex(task->threadIndex());
    m_pool->m_threads[task->threadIndex()]->postTask(FROM_HERE, task);

    SkRect updateRect = layer->mapRectFromCurrentLayerCoordinateToRootLayer(dirtyRect);
    appendPendingInvalidateRect(updateRect);

    return blendAction->actionId();
}

bool RasterTaskGroup::endPostRasterTask()
{
    bool noneNeedCommit = (m_drawPropUpdataAction->dirtyRects().isEmpty() && 0 == m_blendAndImageActions.size());
    unref();
    return noneNeedCommit;
}

void RasterTaskGroup::appendPendingInvalidateRect(const SkRect& r)
{
    if (m_drawPropUpdataAction->isActionIdEmpty())
        m_drawPropUpdataAction->setActionId(m_host->genActionId());
    m_drawPropUpdataAction->appendPendingInvalidateRect(r);
}

void RasterTaskGroup::appendDirtyLayer(cc_blink::WebLayerImpl* layer)
{
    if (m_drawPropUpdataAction->isActionIdEmpty())
        m_drawPropUpdataAction->setActionId(m_host->genActionId());
    m_drawPropUpdataAction->appendDirtyLayer(layer);
}

void RasterTaskGroup::appendTileToUIThreadRelease(Tile* tile)
{
    DebugBreak();
}

void RasterTaskGroup::appendUnnecessaryTileToEvictAfterDrawFrame(Tile* tile)
{
    DebugBreak();
}

void RasterTaskGroup::waitHostRasteringIndexOld()
{
}

void RasterTaskGroup::ref()
{
    m_mutex->lock();
    ++m_ref;
    m_mutex->unlock();
}

void RasterTaskGroup::unref()
{
    WTF::Mutex* mutex = m_mutex;

    mutex->lock();

    --m_ref;
    if (0 < m_ref) {
        mutex->unlock();
        return;
    }

    // 有时候dirty layer不为0但dirty rect为0，因为有些layer可能只是位置关系变了，但不需要刷新
    // 有时候没有脏矩形，但有layer需要更新，比如刚创建的时候，layer没有边框位置
    if (m_drawPropUpdataAction->dirtyRects().isEmpty() && 0 == m_blendAndImageActions.size()) {
        ASSERT(isMainThread());
    }

    if (m_lastBlendActionForPendingInvalidateRect) {
        if (m_drawPropUpdataAction) { // 脏矩形转移，可以延迟提交
            m_lastBlendActionForPendingInvalidateRect->appendPendingInvalidateRects(m_drawPropUpdataAction->dirtyRects());
            m_drawPropUpdataAction->cleanupPendingInvalidateRectIfHasAlendAction();
        }
    }

    if (!m_drawPropUpdataAction->dirtyRects().isEmpty() || !m_drawPropUpdataAction->isDirtyLayerEmpty()) {
        if (-1 == m_drawPropUpdataAction->actionId()) {
            RELEASE_ASSERT(false);
            m_drawPropUpdataAction->setActionId(m_host->genActionId());
        }
        m_host->appendLayerChangeAction(m_drawPropUpdataAction);
    } else {
        ASSERT(m_drawPropUpdataAction->isActionIdEmpty());
        delete m_drawPropUpdataAction;
        m_drawPropUpdataAction = nullptr;
    }

    for (size_t i = 0; i < m_blendAndImageActions.size(); ++i)
        m_host->appendLayerChangeAction(m_blendAndImageActions[i]);

    delete this;
    mutex->unlock();
    delete mutex;
}

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, dirtyLayerInfoCount, ("ccDirtyLayerInfo"));
#endif

DirtyLayerInfo::DirtyLayerInfo(cc_blink::WebLayerImpl* layer)
{
    m_layerId = layer->id();
    m_drawToCanvasProperties = new DrawToCanvasProperties();
    m_drawToCanvasProperties->screenSpaceTransform = layer->drawProperties()->screenSpaceTransform;
    m_drawToCanvasProperties->targetSpaceTransform = layer->drawProperties()->targetSpaceTransform;
    m_drawToCanvasProperties->currentTransform = layer->drawProperties()->currentTransform;
    m_drawToCanvasProperties->bounds = layer->bounds();

#ifndef NDEBUG
    dirtyLayerInfoCount.increment();
#endif
}

DirtyLayerInfo::~DirtyLayerInfo()
{
    delete m_drawToCanvasProperties;

#ifndef NDEBUG
    dirtyLayerInfoCount.decrement();
#endif
}

int DirtyLayerInfo::layerId() const
{
    return m_layerId;
}

DrawToCanvasProperties* DirtyLayerInfo::properties()
{
    return m_drawToCanvasProperties;
}

bool DirtyLayerInfo::isSameLayer(cc_blink::WebLayerImpl* layer)
{
    return layer == m_tempLayer;
}

DirtyLayers::DirtyLayers()
{
    m_drawingIndex = -1;
}

DirtyLayers::~DirtyLayers()
{
    for (size_t i = 0; i < m_layers.size(); ++i) {
        delete m_layers[i];
    }
    m_layers.clear();
}

void DirtyLayers::setDrawingIndex(int64 drawingIndex)
{
    m_drawingIndex = drawingIndex;
}

void DirtyLayers::add(cc_blink::WebLayerImpl* layer)
{
    for (size_t i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i]->isSameLayer(layer))
            return;
    }
    m_layers.append(new DirtyLayerInfo(layer));
}

int64 DirtyLayers::drawingIndex() const
{
    return m_drawingIndex;
}

WTF::Vector<DirtyLayerInfo*>& DirtyLayers::layers()
{
    return m_layers;
}

void DirtyLayers::appendPendingInvalidateRect(const blink::IntRect& r)
{
    m_pendingInvalidateRect.unite(r);
}

blink::IntRect DirtyLayers::dirtyRect() const
{
    return m_pendingInvalidateRect;
}

} // namespace cc
