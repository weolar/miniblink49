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
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "skia/ext/refptr.h"
#include "cc/raster/RasterResouce.h"
#include "cc/tiles/Tile.h"
#include "cc/tiles/TileGrid.h"
#include "cc/blink/WebLayerImpl.h"

using namespace blink;

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
        s_sharedThreadPool = new RasterTaskWorkerThreadPool(5);
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
    ++m_pendingRasterTaskNum;
}

void RasterTaskWorkerThreadPool::decreasePendingRasterTaskNum() 
{ 
    --m_pendingRasterTaskNum;
}

void RasterTaskWorkerThreadPool::decreaseBusyCountByIndex(int index)
{
    --m_threadBusyCount[index];
}

class RasterTask : public WebThread::Task {
public:
    explicit RasterTask(RasterTaskWorkerThreadPool* pool, TileGrid* tileGrid, SkPicture* picture,
        Tile* tile, const IntRect& dirtyRect, int threadIndex)
        : m_pool(pool)
        , m_tileGrid(tileGrid)
        , m_picture(picture)
        , m_tile(tile)
        , m_dirtyRect(dirtyRect)
        , m_threadIndex(threadIndex)
    {
        m_picture->ref();
        m_tile->ref();
    }

    ~RasterTask()
    {
    }

    void releaseRource()
    {
        m_tile->clearDirtyRect();
        m_tile->unref();
        m_picture->unref();

        m_pool->decreasePendingRasterTaskNum();
        m_pool->decreaseBusyCountByIndex(m_threadIndex);
        m_tileGrid->decreaseRsterTaskCount();
    }

    class RasterResouceLocker {
        WTF_MAKE_NONCOPYABLE(RasterResouceLocker);
    public:
        RasterResouceLocker(RecursiveMutex* lockable, RasterTask* task) 
            : m_lockable(lockable)
            , m_task(task)
        {
            m_lockable->lock();
        }
        ~RasterResouceLocker() {
            m_task->releaseRource();
            m_lockable->unlock();
        }
    private:
        RecursiveMutex* m_lockable;
        RasterTask* m_task;
    };

    void eraseColor()
    {
        //m_tile->bitmap()->eraseColor(0x00ffffff);

        SkCanvas canvas(*m_tile->bitmap());

        IntRect dirtyRect = m_dirtyRect;
        dirtyRect.intersect(m_tile->postion());
        dirtyRect.move(-m_tile->postion().x(), -m_tile->postion().y());

        SkPaint clearColorPaint;
        clearColorPaint.setXfermodeMode(SkXfermode::kClear_Mode);
        clearColorPaint.setColor(m_tile->layer()->backgroundColor());
        canvas.drawIRect(dirtyRect, clearColorPaint);
    }

    virtual void run() override
    {
        RasterResouceLocker locker(&m_tile->mutex(), this);
        if (m_pool->willShutdown() || m_tileGrid->willShutdown() || !m_tile->bitmap())
            return;

        eraseColor();

//         LARGE_INTEGER time0 = { 0 };
//         ::QueryPerformanceCounter(&time0);

        // Uses kPremul_SkAlphaType since the result is not known to be opaque.
        SkImageInfo info = SkImageInfo::MakeN32(m_tile->bitmap()->width(), m_tile->bitmap()->height(), kPremul_SkAlphaType);
        SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);
        size_t stride = info.minRowBytes();
        skia::RefPtr<SkSurface> surface = skia::AdoptRef(SkSurface::NewRasterDirect(info, m_tile->bitmap()->getPixels(), stride, &surfaceProps));
        skia::RefPtr<SkCanvas> canvas = skia::SharePtr(surface->getCanvas());

        SkPaint paint;
        paint.setAntiAlias(true);
        
        canvas->save();
        canvas->scale(/*contents_scale*/1, /*contents_scale*/1);
        
        canvas->translate(-m_tile->postion().x(), -m_tile->postion().y());

        canvas->drawPicture(m_picture, nullptr, &paint);
        canvas->restore();

//         LARGE_INTEGER time1 = { 0 };
//         ::QueryPerformanceCounter(&time1);
//         String outString = String::format("RasterTask::run: %x\n", time1.LowPart - time0.LowPart);
//         OutputDebugStringW(outString.charactersWithNullTermination().data());

#if 1 // debug
        SkPaint paintTest;
        const SkColor color = 0xff000000 | (rand() % 3) * (rand() % 7) * GetTickCount(); // update_count & 0xff;
        paintTest.setColor(color);
        paintTest.setStrokeWidth(2);
        
        canvas->drawLine(0, 0, m_tile->postion().width(), m_tile->postion().height(), paintTest);
        canvas->drawLine(m_tile->postion().width(), 0, 0, m_tile->postion().height(), paintTest);

        Vector<char> str;
        str.resize(257);
        sprintf_s(str.data(), 256, "%d, %d\n", m_tile->xIndex(), m_tile->yIndex());
        paintTest.setColor(0xff000000);
        canvas->drawText(str.data(), strlen(str.data()), 20, 15, paintTest);
#endif
        notifyMainThreadRasterFinish();
    }

    void notifyMainThreadRasterFinish()
    {
        if (m_pool->willShutdown() || m_tileGrid->willShutdown())
            return;

        cc_blink::WebLayerImpl* layer = m_tile->layer();
        if (!layer)
            return;
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&cc_blink::WebLayerImpl::onRasterFinish, layer, layer->id(), m_tile->postion()));
    }

private:
    RasterTaskWorkerThreadPool* m_pool;
    TileGrid* m_tileGrid;
    //RasterResouce* m_rasterResouce;
    SkPicture* m_picture;
    Tile* m_tile;
    IntRect m_dirtyRect;
    int m_threadIndex;
};

// void RasterTaskWorkerThreadPool::postRasterTask(RasterResouce* rasterResouce)
// {
//     int threadIndex = selectOneIdleThread();
//     increasePendingRasterTaskNum();
//     ++m_threadBusyCount[threadIndex];
//     m_threads[threadIndex]->postTask(FROM_HERE, new RasterTask(this, rasterResouce, threadIndex));
// }
void RasterTaskWorkerThreadPool::postRasterTask(TileGrid* tileGrid, SkPicture* picture, Tile* tile, const IntRect& dirtyRect)
{
    //tileGrid->increaseRsterTaskCount();
    int threadIndex = selectOneIdleThread();
    increasePendingRasterTaskNum();
    ++m_threadBusyCount[threadIndex];
    m_threads[threadIndex]->postTask(FROM_HERE, new RasterTask(this, tileGrid, picture, tile, dirtyRect, threadIndex));
}

} // namespace cc
