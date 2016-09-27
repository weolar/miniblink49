// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RecordDrawHelp_h
#define RecordDrawHelp_h

#include <wtf/Functional.h>
#include "cc/raster/RasterTaskClient.h"

namespace cc {

typedef Function<void(SkCanvas* canvas, const blink::IntRect& clip)> DrawFunction;

class RecordDrawHelp : public RasterTaskClient {
public:
    RecordDrawHelp()
        : m_bitmapWrap(nullptr)
        , m_rasterTaskCount(0)
        , m_willBeDelete(false)
    {
        ;
    }

    ~RecordDrawHelp()
    {
        ASSERT(isMainThread());
        m_willBeDelete = true;
        if (m_bitmapWrap) {
            //MutexLocker locker(m_mutex);

            ASSERT(1 <= m_bitmapWrap->getRef());
            m_bitmapWrap->ref();

            // 循环等光栅化线程结束
            while (0 < m_rasterTaskCount) { ::Sleep(10); }
            while (2 != m_bitmapWrap->getRef()) { ::Sleep(10); }
            m_bitmapWrap->deref();
            m_bitmapWrap->deref();
        }
    }

    void recordDraw(cc_blink::WebLayerImpl* webLayer, OwnPtr<DrawFunction> drawFunction)
    {
        DebugBreak();

        if (!webLayer || /*webLayer->updateRect().isEmpty() ||*/ !webLayer->drawsContent())
            return;

        blink::IntSize bounds = webLayer->bounds();
        if (bounds.isEmpty()) {
            m_bounds = bounds;
            return;
        }

        blink::FloatRect layerRect(webLayer->position().x, webLayer->position().y, bounds.width(), bounds.height());

        cc::LayerTreeHost* treeHost = webLayer->layerTreeHost();
        blink::IntRect canDrawRect = blink::IntRect(blink::IntPoint(), treeHost->canDrawSize());
        canDrawRect = webLayer->mapRectFromRootLayerCoordinateToCurrentLayer(canDrawRect);

        blink::IntRect updateRect;// = webLayer->updateRect(); weolar
        updateRect.intersect(canDrawRect);

        SkRTreeFactory factory;
        SkPictureRecorder* recorder = new SkPictureRecorder();
        SkCanvas* canvas = recorder->beginRecording(layerRect.width(), layerRect.height(), &factory);
        canvas->clipRect(SkRect::MakeWH(layerRect.width(), layerRect.height()));

        (*drawFunction)(canvas, blink::WebRect(0, 0, bounds.width(), bounds.height()));

        SkPicture* picture = recorder->endRecordingAsPicture();
        delete recorder;

        if (m_bitmapWrap && m_bounds != bounds) {
            MutexLocker locker(m_mutex);
            m_bitmapWrap->deref();
            m_bitmapWrap = nullptr;
        }

        if (!m_bitmapWrap) {
            m_bitmapWrap = new cc::SkBitmapRefWrap();
            m_bitmapWrap->ref();
            m_bitmapWrap->bitmap()->allocN32Pixels(bounds.width(), bounds.height(), false);
            m_bitmapWrap->bitmap()->eraseColor(0x00ffffff);
        }

        m_bounds = bounds;
        m_rasterTaskCount++;
        cc::RasterTaskWorkerThreadPool::shared()->postRasterTask(new cc::RasterResouce(m_bitmapWrap->ref(), &m_mutex, picture, layerRect, this));
    }

    void drawToCanvas(cc_blink::WebLayerImpl* webLayer, blink::WebCanvas* canvas, const blink::IntRect& clip)
    {
        if (!webLayer->drawsContent() || !bitmap())
            return;
        MutexLocker locker(m_mutex);

        blink::IntRect clipRect(clip);
        clipRect.intersect(blink::IntRect(0, 0, bitmap()->width(), bitmap()->height()));
        //SkIRect isrc = SkIRect::MakeXYWH(clipRect.x(), clipRect.y(), clipRect.width(), clipRect.height());
        SkIRect isrc = SkIRect::MakeXYWH(0, 0, bitmap()->width(), bitmap()->height());

        SkPaint paint;
        paint.setAntiAlias(true);
        canvas->drawBitmapRect(*bitmap(), &isrc, SkRect::MakeFromIRect(isrc), &paint);
    }

    SkBitmapRefWrap* bitmapWrap() { return m_bitmapWrap; }

    SkBitmap* bitmap() 
    {
        if (!m_bitmapWrap)
            return nullptr;
        return m_bitmapWrap->bitmap(); 
    }

    RecursiveMutex& mutex() { return m_mutex; }

    // RasterTaskClient
    virtual void rasterTaskFinish() override { m_rasterTaskCount--; }
    virtual bool willBeDelete() override { return m_willBeDelete; }

private:
    SkBitmapRefWrap* m_bitmapWrap;
    RecursiveMutex m_mutex;
    blink::IntSize m_bounds;

    int m_rasterTaskCount;
    bool m_willBeDelete;
};

} // cc

#endif // RecordDrawHelp_h