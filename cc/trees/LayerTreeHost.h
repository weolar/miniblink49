#ifndef LayerTreeHost_h
#define LayerTreeHost_h

#include "cc/trees/LayerTreeHost.h"
#include "third_party/WebKit/public/platform/WebLayerTreeView.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/ThreadingPrimitives.h"
#include <set>

namespace blink {
class WebViewClient;
class WebPageImpl;
class WebGestureCurveTarget;
class IntRect;
struct WebFloatSize;
class WebThread;
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

class LayerTreeHostUiThreadClient {
public:
    virtual void paintToMemoryCanvasInUiThread(SkCanvas* canvas, const blink::IntRect& paintRect) = 0;
};

class LayerTreeHost : public blink::WebLayerTreeView {
public:
    LayerTreeHost(blink::WebViewClient* webViewClient, LayerTreeHostUiThreadClient* uiThreadClient);
    ~LayerTreeHost();

    void registerLayer(cc_blink::WebLayerImpl* layer);
    void unregisterLayer(cc_blink::WebLayerImpl* layer);
    cc_blink::WebLayerImpl* getLayerById(int id);

    bool isDestroying() const;

    void updateLayers(SkCanvas* canvas, const blink::IntRect& clip, bool needsFullTreeSync);
    void recordDraw();
    void drawToCanvas(SkCanvas* canvas, const blink::IntRect& clip);
    void updateLayersDrawProperties();

    void setNeedsCommit();
    void setNeedsFullTreeSync();

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
    virtual void setRootLayer(const blink::WebLayer&) OVERRIDE;
    CompositingLayer* getRootCCLayer();
    virtual void clearRootLayer() OVERRIDE;
    virtual void setViewportSize(const blink::WebSize& deviceViewportSize) OVERRIDE;

    // Gives the viewport size in physical device pixels.
    virtual blink::WebSize deviceViewportSize() const OVERRIDE;

    virtual void setDeviceScaleFactor(float) OVERRIDE;
    virtual float deviceScaleFactor() const OVERRIDE;

    // Sets the background color for the viewport.
    virtual void setBackgroundColor(blink::WebColor) OVERRIDE;

    // Sets the background transparency for the viewport. The default is 'false'.
    virtual void setHasTransparentBackground(bool) OVERRIDE;

	virtual void registerForAnimations(blink::WebLayer* layer) OVERRIDE;

    // Sets whether this view is visible. In threaded mode, a view that is not visible will not
    // composite or trigger updateAnimations() or layout() calls until it becomes visible.
    virtual void setVisible(bool) OVERRIDE;

    // Sets the current page scale factor and minimum / maximum limits. Both limits are initially 1 (no page scale allowed).
    virtual void setPageScaleFactorAndLimits(float pageScaleFactor, float minimum, float maximum) OVERRIDE;

    // Starts an animation of the page scale to a target scale factor and scroll offset.
    // If useAnchor is true, destination is a point on the screen that will remain fixed for the duration of the animation.
    // If useAnchor is false, destination is the final top-left scroll position.
    virtual void startPageScaleAnimation(const blink::WebPoint& destination, bool useAnchor, float newPageScale, double durationSec) OVERRIDE;

    virtual void setNeedsAnimate() OVERRIDE;

    virtual void finishAllRendering() OVERRIDE;

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

    void beginRecordActions();
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
    void requestDrawFrameToRunIntoCompositeThread();
    void requestApplyActionsToRunIntoCompositeThread(bool needCheck);
    void setUseLayeredBuffer(bool b);
    static void clearCanvas(SkCanvas* canvas, const blink::IntRect& rect, bool useLayeredBuffer);
    
    void postPaintMessage(const blink::IntRect& paintRect);
    void firePaintEvent(HDC hdc, const RECT* paintRect);
    blink::IntRect getClientRect();

private:
    void applyActionsInCompositeThread(bool needCheck);
    void drawFrameInCompositeThread();
    void paintToMemoryCanvasInUiThread(const blink::IntRect& paintRect);
    void paintToMemoryCanvas(const blink::IntRect& r);
    //void drawDebugLine(SkCanvas* memoryCanvas, const blink::IntRect& paintRect);
    //void drawToCanvas(const IntRect& dirtyRect, skia::PlatformCanvas* canvas);
    
    bool m_isDestroying;

    blink::WebViewClient* m_webViewClient;
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

    bool m_needsFullTreeSync;
    bool m_needTileRender;

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
    blink::WebThread* m_compositeThread;
    WTF::Mutex m_compositeMutex;
    SkCanvas* m_memoryCanvas;
    //SkCanvas* m_memoryCanvasForUi;
    //SkCanvas* m_memoryCanvasInUiThread;

    double m_lastDrawTime;

    static const int m_paintMessageQueueSize = 200;
    Vector<blink::IntRect> m_dirtyRects;
    int m_postpaintMessageCount;
    int m_drawFrameCount;
    int m_drawFrameFinishCount;
    int m_requestApplyActionsCount;
    int m_requestApplyActionsFinishCount;
    bool m_useLayeredBuffer;

    struct WrapSelfForUiThread {
        WrapSelfForUiThread(LayerTreeHost* host)
            : m_host(host) { }
        LayerTreeHost* m_host;
        void paintToMemoryCanvasInUiThread(const blink::IntRect& paintRect);
    };
    friend WrapSelfForUiThread;
    std::set<WrapSelfForUiThread*> m_wrapSelfForUiThreads;
    int m_paintToMemoryCanvasInUiThreadTaskCount;

    bool m_isDrawDirty;
    bool m_hasResize;
};

} // cc

#endif // LayerTreeHost_h