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
#include "cc/raster/RasterTaskWorkerThreadPool.h"

#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkCanvas.h"

#include "skia/ext/refptr.h"
#include "cc/raster/RasterResouce.h"
#include "cc/tiles/Tile.h"
#include "cc/tiles/TileGrid.h"
#include "cc/blink/WebLayerImpl.h"
#include "cc/trees/LayerTreeHost.h"
#include "cc/trees/DrawProperties.h"
#include "cc/playback/LayerChangeAction.h"

#include "platform/image-encoders/gdiplus/GDIPlusImageEncoder.h" // TODO

extern DWORD g_rasterTime;
extern DWORD g_nowTime;
extern int g_mouseTest;
extern DWORD g_rasterTimeInMouse;

using namespace blink;

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
        m_threads.append(Platform::current()->createThread("RasterTaskWorkerThreadPool"));
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

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, rasterTaskCounter, ("ccRasterTask"));
#endif

class RasterTask : public WebThread::Task {
public:
    explicit RasterTask(
        RasterTaskWorkerThreadPool* pool, 
        SkPicture* picture,
        const SkRect& dirtyRect, 
        int threadIndex,
        bool isOpaque,
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
        SkBitmap* bitmap = raster();
        m_blendAction->setBitmap(bitmap);
        releaseRource();

//         DWORD nowTime = (DWORD)(WTF::currentTimeMS() * 100);
//         DWORD detTime = nowTime - g_nowTime;
//         InterlockedExchange((LONG*)&g_nowTime, nowTime);

//         String out = String::format("RasterTask.run: %d\n", detTime);
//         OutputDebugStringA(out.utf8().data());
    }

    SkBitmap* raster()
    {
        SkBitmap* bitmap = new SkBitmap;
        bitmap->allocN32Pixels(m_dirtyRect.width(), m_dirtyRect.height());
        if (!m_isOpaque)
            bitmap->eraseColor(0x00ff11ff); // TODO
        
        // Uses kPremul_SkAlphaType since the result is not known to be opaque.
        SkImageInfo info = SkImageInfo::MakeN32(m_dirtyRect.width(), m_dirtyRect.height(), m_isOpaque ? kOpaque_SkAlphaType : kPremul_SkAlphaType); // TODO
        SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);
        size_t stride = info.minRowBytes();
        skia::RefPtr<SkSurface> surface = skia::AdoptRef(SkSurface::NewRasterDirect(info, bitmap->getPixels(), stride, &surfaceProps));
        skia::RefPtr<SkCanvas> canvas = skia::SharePtr(surface->getCanvas());

        SkPaint paint;
        paint.setAntiAlias(false);

        canvas->save();
        canvas->scale(1, 1);
        //canvas->clipRect(SkRect::MakeIWH(m_dirtyRect.width(), m_dirtyRect.height()));
        canvas->translate(-m_dirtyRect.x(), -m_dirtyRect.y());
        canvas->drawPicture(m_picture, nullptr, /*&paint*/nullptr);
        canvas->restore();

        return bitmap;
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
    
    LayerChangeActionBlend* blendAction = new LayerChangeActionBlend(m_host->genActionId(), layerId, willRasteredTiles, dirtyRect, nullptr);
    m_blendAndImageActions.append(blendAction);
    m_lastBlendActionForPendingInvalidateRect = blendAction;

    int threadIndex = m_pool->selectOneIdleThread();

    RasterTask* task = new RasterTask(m_pool, picture, dirtyRect, threadIndex, layer->opaque(), blendAction, this);
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

    // ��ʱ��dirty layer��Ϊ0��dirty rectΪ0����Ϊ��Щlayer����ֻ��λ�ù�ϵ���ˣ�������Ҫˢ��
    // ��ʱ��û������Σ�����layer��Ҫ���£�����մ�����ʱ��layerû�б߿�λ��
    if (m_drawPropUpdataAction->dirtyRects().isEmpty() && 0 == m_blendAndImageActions.size()) {
        ASSERT(isMainThread());
    }

//     if (0 != m_blendAndImageActions.size() || !m_drawPropUpdataAction->dirtyRect().isEmpty()) {
//         String outString = String::format("RasterTaskGroup::unref: %p\n", this);
//         OutputDebugStringW(outString.charactersWithNullTermination().data());
//     }

    if (m_lastBlendActionForPendingInvalidateRect) {
        if (m_drawPropUpdataAction) { // �����ת�ƣ������ӳ��ύ
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
