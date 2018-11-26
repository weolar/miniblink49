/*
* Copyright (C) 2013 Google Inc.  All rights reserved.
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
*     * Neither the name of Google Inc. nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef cc_raster_RasterTask_h
#define cc_raster_RasterTask_h

#include "wtf/Vector.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"

namespace blink {
class WebThread;
class IntRect;
}

namespace cc_blink {
class WebLayerImpl;
}

namespace WTF {
class Mutex;
}

namespace cc {

class RasterResouce;
class RasterTask;
class TileGrid;
class Tile;
class RasterTaskWorkerThreadPool;
class LayerTreeHost;
class SkBitmapRefWrap;
class TileActionInfoVector;
class LayerChangeActionBlend;
class LayerChangeActionDrawPropUpdata;
class LayerChangeAction;
class LayerChangeActionUpdataImageLayer;
struct DrawToCanvasProperties;

class DirtyLayerInfo {
public:
    DirtyLayerInfo(cc_blink::WebLayerImpl* layer);
    ~DirtyLayerInfo();
    int layerId() const;
    DrawToCanvasProperties* properties();

    bool isSameLayer(cc_blink::WebLayerImpl* layer);

private:
    int m_layerId;
    DrawToCanvasProperties* m_drawToCanvasProperties;
    Vector<int>* m_hasBitmapTilesToEvict;

    cc_blink::WebLayerImpl* m_tempLayer;
};

class DirtyLayers {
public:
    DirtyLayers();
    ~DirtyLayers();

    void setDrawingIndex(int64 drawingIndex);

    void add(cc_blink::WebLayerImpl* layer);
    int64 drawingIndex() const;
    WTF::Vector<DirtyLayerInfo*>& layers();

    //void appendDirtyLayer(cc_blink::WebLayerImpl* layer);
    void appendPendingInvalidateRect(const blink::IntRect& r);

    blink::IntRect dirtyRect() const;

private:
    WTF::Vector<DirtyLayerInfo*> m_layers;
    int64 m_drawingIndex;
    blink::IntRect m_pendingInvalidateRect;
};

class RasterTaskGroup {
public:
    RasterTaskGroup(RasterTaskWorkerThreadPool* pool, LayerTreeHost* host);
    ~RasterTaskGroup();

    void postImageLayerAction(int imageLayerId, SkBitmapRefWrap* bitmap);
    int64 postRasterTask(cc_blink::WebLayerImpl* layer, SkPicture* picture, TileActionInfoVector* willRasteredTiles, const SkRect& dirtyRect);
    bool endPostRasterTask();
    void appendPendingInvalidateRect(const SkRect& r); // r是根层坐标系
    void appendDirtyLayer(cc_blink::WebLayerImpl* layer);
    void appendTileToUIThreadRelease(Tile* tile);
    void appendUnnecessaryTileToEvictAfterDrawFrame(Tile* tile);

    void waitHostRasteringIndexOld();

    int getPendingRasterTaskNum() const;

    bool isTaskEmpty() const { return m_tasks.size() == 0; }

    void ref();
    void unref();

private:
    int m_ref;
    Vector<RasterTask*> m_tasks;
    RasterTaskWorkerThreadPool* m_pool;
    
    //cc_blink::WebLayerImpl* m_layer;
    LayerTreeHost* m_host;
    WTF::Mutex* m_mutex;
    WTF::Vector<LayerChangeAction*> m_blendAndImageActions;
    LayerChangeActionBlend* m_lastBlendActionForPendingInvalidateRect;
    LayerChangeActionDrawPropUpdata* m_drawPropUpdataAction;
    LayerChangeActionUpdataImageLayer* m_updataImageLayerAction;
};

class RasterTaskWorkerThreadPool {
public:
    RasterTaskWorkerThreadPool(int threadNum);
    void init(int threadNum);
    void shutdown();

    RasterTaskGroup* beginPostRasterTask(LayerTreeHost* host);
    void postRasterTask(TileGrid* tileGrid, SkPicture* picture, Vector<Tile*>* willRasteredTiles, const blink::IntRect& dirtyRect);

    // It is an error to call shared() before init() or after shutdown();
    static RasterTaskWorkerThreadPool* shared();

    int selectOneIdleThread();

    bool willShutdown() const { return m_willShutdown; }
    void increasePendingRasterTaskNum();
    void decreasePendingRasterTaskNum();
    int getPendingRasterTaskNum() const;

    void increaseBusyCountByIndex(int index);
    void decreaseBusyCountByIndex(int index);

private:
    friend class RasterTaskGroup;

    RasterTaskWorkerThreadPool();
    ~RasterTaskWorkerThreadPool();

    Vector<blink::WebThread*> m_threads;
    Vector<int> m_threadBusyCount;
    bool m_willShutdown;
    int m_pendingRasterTaskNum;
};

} // namespace cc

#endif // RasterTaskWorkerThreadPool_h
