// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_LAYER_IMPL_H_
#define CC_BLINK_WEB_LAYER_IMPL_H_

#include <string>
#include <utility>

#include "third_party/WebKit/public/platform/WebCString.h"
#include "third_party/WebKit/public/platform/WebColor.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimation.h"
#include "third_party/WebKit/public/platform/WebDoublePoint.h"
#include "third_party/WebKit/public/platform/WebFloatPoint.h"
#include "third_party/WebKit/public/platform/WebLayer.h"
#include "third_party/WebKit/public/platform/WebPoint.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebVector.h"
#include "third_party/WebKit/public/platform/WebLayerPositionConstraint.h"
#include "third_party/WebKit/public/platform/WebCanvas.h"
#include "third_party/WebKit/public/platform/WebContentLayerClient.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "cc/blink/WebLayerImplClient.h"

namespace blink {
class WebFilterOperations;
class WebLayerClient;
struct WebFloatRect;
}

namespace cc {
class LayerTreeHost;
class TileGrid;
struct DrawProps;
class LayerChangeAction;
}

namespace cc_blink {

class WebToCCAnimationDelegateAdapter;
class WebLayerImpl;
class WebFilterOperationsImpl;

typedef WTF::Vector<WebLayerImpl*> WebLayerImplList;

class WebLayerImpl : public blink::WebLayer, public WebLayerImplClient {
public:
    WebLayerImpl(WebLayerImplClient* client);
    //explicit WebLayerImpl(scoped_refptr<cc::Layer>);
    ~WebLayerImpl() override;

    void setLayerTreeHost(cc::LayerTreeHost* host);
    cc::LayerTreeHost* layerTreeHost() const;

    void gc();

    // WebLayerImplClient
    virtual void updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip) override;
    virtual WebLayerImplClient::Type type() const override { return m_layerType; }
    void recordDrawChildren(cc::RasterTaskGroup* taskGroup, int deep);
    virtual void recordDraw(cc::RasterTaskGroup* taskGroup) override;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) override;

    // If set to true, content opaqueness cannot be changed using setOpaque.
    // However, it can still be modified using SetContentsOpaque on the
    // cc::Layer.
    void setContentsOpaqueIsFixed(bool fixed);

    blink::IntRect mapRectFromRootLayerCoordinateToCurrentLayer(const blink::IntRect& rect);
    SkRect mapRectFromCurrentLayerCoordinateToRootLayer(const SkRect& rect);

    // WebLayer implementation.
    int id() const override;
    void invalidateRect(const blink::WebRect&) override;
    void invalidate() override;
    void addChild(blink::WebLayer* child) override;
    void insertChild(blink::WebLayer* child, size_t index) override;
    bool hasAncestor(blink::WebLayer* child);
    void replaceChild(blink::WebLayer* reference, blink::WebLayer* new_layer) override;
    int indexOfChild(WebLayerImpl* child) const;
    void setParent(blink::WebLayer* child);
    WebLayerImpl* parent() const;
    void removeFromParent() override;
    void removeAllChildren() override;
    WebLayerImplList& children();
    void removeChildOrDependent(WebLayerImpl* child);
    void setBounds(const blink::WebSize& bounds) override;
    blink::WebSize bounds() const override;
    void setMasksToBounds(bool masksToBounds) override;
    bool masksToBounds() const override;
    void setMaskLayer(blink::WebLayer* mask) override;
    void setReplicaLayer(blink::WebLayer* replica) override;
    void setOpacity(float opacity) override;
    float opacity() const override;
    void setBlendMode(blink::WebBlendMode blend_mode) override;
    blink::WebBlendMode blendMode() const override;
    void setIsRootForIsolatedGroup(bool root) override;
    bool isRootForIsolatedGroup() override;
    void setOpaque(bool opaque) override;
    bool opaque() const override;
    void setPosition(const blink::WebFloatPoint& position) override;
    blink::WebFloatPoint position() const override;
    void setTransform(const SkMatrix44& transform) override;
    void setTransformOrigin(const blink::WebFloatPoint3D& point) override;
    blink::WebFloatPoint3D transformOrigin() const override;
    SkMatrix44 transform() const override;
    void setDrawsContent(bool draws_content) override;
    bool drawsContent() const override;
    void setShouldFlattenTransform(bool flatten) override;
    bool shouldFlattenTransform();
    void setRenderingContext(int context) override;
    void setUseParentBackfaceVisibility(bool visible) override;
    void setDoubleSided(bool isDoubleSided);
    void setBackgroundColor(blink::WebColor color) override;
    blink::WebColor backgroundColor() const override;
    void setFilters(const blink::WebFilterOperations& filters) override;
    const WebFilterOperationsImpl* getFilters() const;
    //void setBackgroundFilters(const blink::WebFilterOperations& filters) override;
    void setAnimationDelegate(
        blink::WebCompositorAnimationDelegate* delegate) override;
    bool addAnimation(blink::WebCompositorAnimation* animation) override;
    void removeAnimation(int animation_id) override;
    void removeAnimation(int animation_id, blink::WebCompositorAnimation::TargetProperty) override;
    void pauseAnimation(int animation_id, double time_offset) override;
    bool hasActiveAnimation() override;
    void setForceRenderSurface(bool force) override;
    void setScrollPositionDouble(blink::WebDoublePoint position) override;
    blink::WebDoublePoint scrollPositionDouble() const override;
    void setScrollCompensationAdjustment(blink::WebDoublePoint position) override;
    void setScrollClipLayer(blink::WebLayer* clip_layer) override;
    bool scrollable() const override;
    void setUserScrollable(bool horizontal, bool vertical) override;
    bool userScrollableHorizontal() const override;
    bool userScrollableVertical() const override;
    void setHaveWheelEventHandlers(bool have_wheel_event_handlers) override;
    bool haveWheelEventHandlers() const override;
    void setHaveScrollEventHandlers(bool have_scroll_event_handlers) override;
    bool haveScrollEventHandlers() const override;
    void setShouldScrollOnMainThread(bool scroll_on_main) override;
    bool shouldScrollOnMainThread() const override;
    void setNonFastScrollableRegion(
        const blink::WebVector<blink::WebRect>& region) override;
    blink::WebVector<blink::WebRect> nonFastScrollableRegion() const override;
    void setTouchEventHandlerRegion(
        const blink::WebVector<blink::WebRect>& region) override;
    blink::WebVector<blink::WebRect> touchEventHandlerRegion() const override;
    void setScrollBlocksOn(blink::WebScrollBlocksOn) override;
    blink::WebScrollBlocksOn scrollBlocksOn() const override;
    void setFrameTimingRequests(const blink::WebVector<std::pair<int64_t, blink::WebRect>>& requests) override;
    blink::WebVector<std::pair<int64_t, blink::WebRect>> frameTimingRequests()
        const override;
    void setIsContainerForFixedPositionLayers(bool is_container) override;
    bool isContainerForFixedPositionLayers() const override;
    void setPositionConstraint(
        const blink::WebLayerPositionConstraint& constraint) override;
    blink::WebLayerPositionConstraint positionConstraint() const override;
    void setScrollClient(blink::WebLayerScrollClient* client) override;
    bool isOrphan() const override;
    void setWebLayerClient(blink::WebLayerClient* client) override;

    void setScrollParent(blink::WebLayer* parent) override;
    void setClipParent(blink::WebLayer* parent) override;
    void removeFromScrollTree();
    void removeFromClipTree();
    void addScrollChild(WebLayerImpl* child);
    void addClipChild(WebLayerImpl* child);

    bool dirty() const;
    bool childrenDirty() const;

    void setChildrenDirty();
    void clearChildrenDirty();
    void setAllParentDirty();
    void requestSelfAndAncestorBoundRepaint();

    const WebLayerImplClient* client() { return m_client; }
    //blink::IntRect updateRect() const { return m_updateRect; };
    cc::TileGrid* tileGrid() { return m_tileGrid; }

    void requestRepaint(const blink::IntRect& rect);
    void requestBoundRepaint(bool directOrPending);
    void appendPendingInvalidateRect(const SkRect& rect);

    cc::DrawProps* drawProperties();
    //cc::DrawToCanvasProperties* drawToCanvasProperties(); // 在上屏时使用本属性，和DrawProperties比，不一定是最新的坐标属性，需要光栅化后来更新
    void updataDrawToCanvasProperties(cc::DrawProps* prop);
    const SkMatrix44& drawTransform() const;

    int maskLayerId() const;
    int replicaLayerId() const;

    bool is3dSorted();

    bool isMaskLayer() const;
    bool isReplicaLayer() const;

    bool hasMaskLayerChild() const;

protected:
    void setNeedsCommit(bool needUpdateAllBoundsArea);
    void appendLayerChangeAction(cc::LayerChangeAction* action);
    void appendLayerActionsToParent();

    WebLayerImplClient::Type m_layerType;
    int m_id;
    blink::FloatPoint m_position;
    blink::IntSize m_bounds;

    cc::TileGrid* m_tileGrid;
    WebLayerImplList m_children;
    bool m_dirty;
    bool m_childrenDirty;
    bool m_stackingOrderChanged;
    WebLayerImpl* m_parent;

    blink::WebLayerClient* m_webLayerClient;
    WebLayerImplClient* m_client;

    SkRect m_updateRectInRootLayerCoordinate;
    
    float m_opacity;
    blink::WebBlendMode m_blendMode;
    bool m_setIsRootForIsolatedGroup;
    bool m_opaque;
    SkMatrix44 m_transform;
    blink::FloatPoint3D m_transformOrigin;
    bool m_drawsContent;
    bool m_shouldFlattenTransform;
    int context;
    int  m_3dSortingContextId;
    bool m_useParentBackfaceVisibility;
    bool m_isDoubleSided;
    blink::WebColor m_backgroundColor;
    blink::WebDoublePoint m_scrollPositionDouble;
    blink::WebDoublePoint m_scrollCompensationAdjustment;
    int m_scrollClipLayerId;
    bool m_userScrollableHorizontal;
    bool m_userScrollableVertical;
    bool m_haveWheelEventHandlers;
    bool m_haveScrollEventHandlers;
    bool m_shouldScrollOnMainThread;
    blink::IntRect m_touchEventHandlerRegions;
    blink::WebScrollBlocksOn m_scrollBlocksOn;
    bool m_isContainerForFixedPositionLayers;
    //cc_blink::WebLayerImpl* m_scrollParent;
    //cc_blink::WebLayerImpl* m_clipParent;
    cc_blink::WebLayerImpl* m_maskLayer;
    cc_blink::WebLayerImpl* m_replicaLayer;
    WTF::HashSet<WebLayerImpl*>* m_scrollChildren;
    WTF::HashSet<WebLayerImpl*>* m_clipChildren;

    bool m_contentsOpaqueIsFixed;
    blink::WebLayerPositionConstraint m_positionConstraint;
    bool m_masksToBounds;
    blink::WebLayerScrollClient* m_webLayerScrollClient;
//     int m_maskLayerId;
//     int m_replicaLayerId;
    cc::LayerTreeHost* m_layerTreeHost;

    WTF::Vector<blink::IntRect> m_nonFastScrollableRegion;

    cc::DrawProps* m_drawProperties;
    //cc::DrawToCanvasProperties* m_drawToCanvasProperties;

    WTF::Vector<cc::LayerChangeAction*> m_savedActionsWhenHostIsNull;
    bool m_hasMaskLayerChild;
    bool m_isMaskLayer;
    bool m_isReplicaLayer;

    cc_blink::WebFilterOperationsImpl* m_filterOperations;

private:
    DISALLOW_COPY_AND_ASSIGN(WebLayerImpl);
};

}  // namespace cc_blink

#endif  // CC_BLINK_WEB_LAYER_IMPL_H_
