#ifndef LayerTreeHost_h
#define LayerTreeHost_h

#include "third_party/WebKit/public/platform/WebLayerTreeView.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/HashSet.h"
#include "third_party/skia/include/core/SkRect.h"

namespace blink {
class WebViewClient;
class WebGestureCurveTarget;
class IntRect;
struct WebFloatSize;
class WebThread;
class WebThreadSupportingGC;
}

namespace cc_blink {
class WebLayerImpl;
}

namespace WTF {
class Mutex;
}

namespace content {
class WebPageImpl;
}

class SkCanvas;

namespace cc {

class DirtyLayers;
class Tile;
class TileGrid;
class LayerChangeAction;
class CompositingLayer;
class ActionsFrameGroup;
class LayerTreeHostClent;

class LayerTreeHostUiThreadClient {
public:
    virtual void paintToMemoryCanvasInUiThread(SkCanvas* canvas, const blink::IntRect& paintRect) = 0;
};

class LayerTreeHost : public blink::WebLayerTreeView {
public:
    LayerTreeHost(LayerTreeHostClent* hostClient, LayerTreeHostUiThreadClient* uiThreadClient);
    ~LayerTreeHost();

    void registerLayer(cc_blink::WebLayerImpl* layer);
    void unregisterLayer(cc_blink::WebLayerImpl* layer);
    cc_blink::WebLayerImpl* getLayerById(int id);

    void gc();

    bool isDestroying() const;

    //void updateLayers(SkCanvas* canvas, const blink::IntRect& clip, bool needsFullTreeSync);
    void recordDraw();
    bool drawToCanvas(SkCanvas* canvas, const SkRect& clip);
    void updateLayersDrawProperties();

    //void setNeedsCommit();
    //void setNeedsFullTreeSync();
    //void didUpdateLayout();

    void requestRepaint(const blink::IntRect& repaintRect);

    // 从光栅化线程发出，通知渲染新的一帧。当然也可能从主线程发出，如果没有光栅化，但又有滚动等情况
    void requestDrawFrameLocked(DirtyLayers* dirtyLayers, Vector<Tile*>* tilesToUIThreadRelease);
    bool preDrawFrame();
    void postDrawFrame();
    bool applyActions(bool needCheck);

    void scrollBy(const blink::WebFloatSize& delta, const blink::WebFloatSize& velocity);

    // 通常比deviceViewportSize要大一点
    blink::IntSize canDrawSize();

    // WebLayerTreeView
    
    // Sets the root of the tree. The root is set by way of the constructor.
    virtual void setRootLayer(const blink::WebLayer&) override;
    CompositingLayer* getRootCCLayer();
    virtual void clearRootLayer() override;
    virtual void setViewportSize(const blink::WebSize& deviceViewportSize) override;

    // Gives the viewport size in physical device pixels.
    virtual blink::WebSize deviceViewportSize() const override;

    virtual void setDeviceScaleFactor(float) override;
    virtual float deviceScaleFactor() const override;

    // Sets the background color for the viewport.
    virtual void setBackgroundColor(blink::WebColor) override;
    blink::WebColor getBackgroundColor() const;

    // Sets the background transparency for the viewport. The default is 'false'.
    virtual void setHasTransparentBackground(bool) override;
    bool getHasTransparentBackground() const;

    virtual void registerForAnimations(blink::WebLayer* layer) override;

    // Sets whether this view is visible. In threaded mode, a view that is not visible will not
    // composite or trigger updateAnimations() or layout() calls until it becomes visible.
    virtual void setVisible(bool) override;

    // Sets the current page scale factor and minimum / maximum limits. Both limits are initially 1 (no page scale allowed).
    virtual void setPageScaleFactorAndLimits(float pageScaleFactor, float minimum, float maximum) override;

    // Starts an animation of the page scale to a target scale factor and scroll offset.
    // If useAnchor is true, destination is a point on the screen that will remain fixed for the duration of the animation.
    // If useAnchor is false, destination is the final top-left scroll position.
    virtual void startPageScaleAnimation(const blink::WebPoint& destination, bool useAnchor, float newPageScale, double durationSec) override;

    virtual void setNeedsAnimate() override;

    virtual void finishAllRendering() override;

    void showDebug();

    void setWebGestureCurveTarget(blink::WebGestureCurveTarget* webGestureCurveTarget);

    void setNeedTileRender(bool b) { m_needTileRender = b; }
    bool needTileRender() { return m_needTileRender; }

    void increaseNodesCount();
    void decreaseNodesCount();
    bool has3dNodes();

    WTF::Mutex* tilesMutex();

    int64 createDrawingIndex();
    int64 drawingIndex();

    void addRasteringIndex(int64 index);
    int64 frontRasteringIndex();
    void popRasteringIndex();

    bool canRecordActions() const;
    void beginRecordActions(bool isComefromMainframe);
    void endRecordActions();

    int64 genActionId();
    void appendLayerChangeAction(LayerChangeAction* action);
    void registerCCLayer(CompositingLayer* layer);
    void unregisterCCLayer(CompositingLayer* layer);
    CompositingLayer* getCCLayerById(int id);
    bool isRootCCLayerEmpty() const { return !m_rootCCLayer; }

    SkCanvas* getMemoryCanvasLocked();
    void releaseMemoryCanvasLocked();

    bool isDrawDirty();
    void paintToBit(void* bits, int pitch);

    struct BitInfo {
        uint32_t* pixels;
        SkCanvas* tempCanvas;
        int width;
        int height;
    };
    BitInfo* getBitBegin();
    void getBitEnd(const BitInfo* bitInfo);

    void requestDrawFrameToRunIntoCompositeThread();
    void requestApplyActionsToRunIntoCompositeThread(bool needCheck);
    //void setUseLayeredBuffer(bool b);
    //bool getIsUseLayeredBuffer() const { return m_useLayeredBuffer; }
    static void clearCanvas(SkCanvas* canvas, const SkRect& rect, bool useLayeredBuffer);

    void setDrawMinInterval(double drawMinInterval);
    
    void postPaintMessage(const SkRect& paintRect);
    void firePaintEvent(HDC hdc, const RECT* paintRect);
    blink::IntRect getClientRect();

    void setLayerTreeDirty();
    bool isLayerTreeDirty() const;

    void disablePaint();
    void enablePaint();

    cc_blink::WebLayerImpl* getRootLayer() { return m_rootLayer; }
    const cc_blink::WebLayerImpl* getConstRootLayer() { return m_rootLayer; }

    void appendPendingRepaintRect(const SkRect& r);

private:
    void requestPaintToMemoryCanvasToUiThread(const SkRect& r);
    void onApplyActionsInCompositeThread(bool needCheck);
    void waitForApplyActions();
    void waitForDrawFrame();
    void drawFrameInCompositeThread();
    void paintToMemoryCanvasInUiThread(const SkRect& paintRect);
    void paintToMemoryCanvasInCompositeThread(const SkRect& r);
    
    bool m_isDestroying;

    LayerTreeHostClent* m_hostClient;
    LayerTreeHostUiThreadClient* m_uiThreadClient;
    blink::WebGestureCurveTarget * m_webGestureCurveTarget;

    cc_blink::WebLayerImpl* m_rootLayer;
    CompositingLayer* m_rootCCLayer;

    blink::IntSize m_deviceViewportSize;
    blink::IntRect m_clientRect;
    float m_deviceScaleFactor;
    blink::WebColor m_backgroundColor;
    bool m_hasTransparentBackground;
    bool m_visible;
    float m_pageScaleFactor;
    float m_minimum;
    float m_maximum;
    bool m_needTileRender;
    bool m_layerTreeDirty; // 需要WebPageImpl.recordDraw

    int m_3dNodesCount;

    WTF::Mutex* m_tilesMutex;

    int64 m_newestDrawingIndex;
    int64 m_drawingIndex;
    WTF::Vector<int64> m_rasteringIndexs;

    WTF::HashMap<int, cc_blink::WebLayerImpl*> m_liveLayers;

    WTF::Mutex* m_rasterNotifMutex;
    WTF::Vector<DirtyLayers*> m_dirtyLayersGroup;
    WTF::Vector<Tile*> m_tilesToUIThreadRelease;

    ActionsFrameGroup* m_actionsFrameGroup;
    WTF::Vector<LayerChangeAction*> m_actions;
    WTF::HashMap<int, CompositingLayer*> m_liveCCLayers;
    //////////////////////////////////////////////////////////////////////////
    WTF::OwnPtr<blink::WebThreadSupportingGC> m_compositeThread;
    WTF::Mutex m_compositeMutex;
    WTF::Mutex m_rootCCLayerMutex;
    SkCanvas* m_memoryCanvas;

    double m_lastCompositeTime;
    double m_lastPaintTime;
    mutable double m_lastRecordTime;
    double m_drawMinInterval;

    static const int m_paintMessageQueueSize = 200;
    Vector<SkRect> m_dirtyRectsForComposite;
    Vector<SkRect> m_dirtyRectsForUi;
    int m_postpaintMessageCount;
    int m_drawFrameCount;
    int m_drawFrameFinishCount;
    int m_requestApplyActionsCount;
    int m_requestApplyActionsFinishCount;

    struct WrapSelfForUiThread : public blink::WebThread::TaskObserver {
        WrapSelfForUiThread(LayerTreeHost* host)
            : m_host(host) { }
        LayerTreeHost* m_host;
        void paintInUiThread();
        void endPaint();

        virtual ~WrapSelfForUiThread() override;
        virtual void willProcessTask() override;
        virtual void didProcessTask() override;
    };
    friend WrapSelfForUiThread;
    WTF::HashSet<WrapSelfForUiThread*> m_wrapSelfForUiThreads;
    int m_paintToMemoryCanvasInUiThreadTaskCount;

    bool m_isDrawDirty;
    bool m_hasResize;

    WTF::Vector<SkRect> m_pendingRepaintRectsInRootLayerCoordinate;
};

} // cc

#endif // LayerTreeHost_h