
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/WebLayerImpl.h"

#include <utility>
#include <vector>

#include "cc/trees/LayerTreeHost.h"
#include "cc/tiles/TileGrid.h"
#include "cc/trees/DrawProperties.h"
#include "cc/raster/RasterTaskWorkerThreadPool.h"
#include "cc/playback/LayerChangeAction.h"
#include "third_party/WebKit/public/platform/WebFloatPoint.h"
#include "third_party/WebKit/public/platform/WebFloatRect.h"
#include "third_party/WebKit/public/platform/WebGraphicsLayerDebugInfo.h"
#include "third_party/WebKit/public/platform/WebLayerClient.h"
#include "third_party/WebKit/public/platform/WebLayerPositionConstraint.h"
#include "third_party/WebKit/public/platform/WebLayerScrollClient.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "third_party/WebKit/public/platform/WebFilterOperations.h"
#include "third_party/WebKit/Source/platform/geometry/FloatRect.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

using blink::WebLayer;
using blink::WebFloatPoint;
using blink::WebVector;
using blink::WebRect;
using blink::WebSize;
using blink::WebColor;
using blink::WebFilterOperations;
using blink::FloatRect;
using blink::IntRect;
using blink::WebDoublePoint;
using blink::WebFloatPoint3D;

namespace cc_blink {
namespace {

int g_next_layer_id = 0;

}  // namespace

static void clearLayerActions(WTF::Vector<cc::LayerChangeAction*>* actions);

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webLayerImplCounter, ("ccWebLayerImpl"));
#endif

int debugMaskLayerId = -1;

WebLayerImpl::WebLayerImpl(WebLayerImplClient* client)
    : m_client(client)
	, m_layerType(client->type())
    , m_dirty(true)
    , m_childrenDirty(true)
    , m_opacity(1.0f)
    , m_blendMode(blink::WebBlendModeNormal)
    , m_setIsRootForIsolatedGroup(false)
    , m_opaque(false)
    , m_drawsContent(true)
    , m_shouldFlattenTransform(true)
    , m_3dSortingContextId(0)
    , m_useParentBackfaceVisibility(false)
    , m_backgroundColor(0xff00ffff)
    , m_scrollClipLayerId(-1)
    , m_userScrollableHorizontal(true)
    , m_userScrollableVertical(true)
    , m_haveWheelEventHandlers(false)
    , m_haveScrollEventHandlers(false)
    , m_shouldScrollOnMainThread(true)
    , m_scrollBlocksOn(blink::WebScrollBlocksOnNone)
    , m_isContainerForFixedPositionLayers(false)
    , m_scrollParent(nullptr)
    , m_clipParent(nullptr)
    , m_contentsOpaqueIsFixed(false)
    , m_masksToBounds(false)
    , m_webLayerScrollClient(nullptr)
//     , m_maskLayerId(-1)
//     , m_replicaLayerId(-1)
    , m_maskLayer(nullptr)
    , m_replicaLayer(nullptr)
    , m_layerTreeHost(nullptr)
    , m_stackingOrderChanged(true)
    , m_parent(nullptr)
    , m_scrollChildren(nullptr)
    , m_clipChildren(nullptr)
    , m_tileGrid(nullptr)
    , m_drawProperties(new cc::DrawProperties())
	, m_drawToCanvasProperties(new cc::DrawToCanvasProperties())
    , m_isMaskLayer(false)
    , m_isReplicaLayer(false)
    , m_hasMaskLayerChild(false)
{
    m_id = atomicIncrement(&g_next_layer_id);
    m_webLayerClient = nullptr;

	appendLayerChangeAction(new cc::LayerChangeActionCreate(-1, id(), m_layerType));

    m_backgroundColor = 0x00ffffff | ((rand() % 3) * (rand() % 7) * GetTickCount());
#ifndef NDEBUG
	webLayerImplCounter.increment();
#endif

// 	String outString = String::format("WebLayerImpl::WebLayerImpl:%p %d \n", this, m_id);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

WebLayerImpl::~WebLayerImpl()
{
    ASSERT(!m_parent);

	if (m_tileGrid)
		delete m_tileGrid;
	m_tileGrid = nullptr;

    m_webLayerClient = nullptr;

    removeFromScrollTree();
    removeFromClipTree();

    // Remove the parent reference from all children and dependents.
    removeAllChildren();
    if (m_maskLayer) {
        ASSERT(this == m_maskLayer->parent());
        m_maskLayer->removeFromParent();
    }
    if (m_replicaLayer) {
        ASSERT(this == m_replicaLayer->parent()); // weolar
        m_replicaLayer->removeFromParent();
    }

	// ASSERT(m_layerTreeHost); // �е�layer���ڻ�û����host�ͱ�ɾ��
	if (m_layerTreeHost) {
		m_layerTreeHost->unregisterLayer(this);

		int64 actionId = m_layerTreeHost->genActionId();
		m_layerTreeHost->appendLayerChangeAction(new cc::LayerChangeActionDestroy(actionId, id()));

// 		String outString = String::format("WebLayerImpl::~~~WebLayerImpl:%p m_id:%d, actionId:%d \n", this, m_id, actionId);
// 		OutputDebugStringW(outString.charactersWithNullTermination().data());
	}

	clearLayerActions(&m_savedActionsWhenHostIsNull);

    //TODO m_replicaLayer......
    m_scrollParent = nullptr;
    m_clipParent = nullptr;
    m_maskLayer = nullptr;
    m_replicaLayer = nullptr;

    delete m_drawProperties;
	delete m_drawToCanvasProperties;
#ifndef NDEBUG
	webLayerImplCounter.decrement();
#endif
}

void WebLayerImpl::removeFromScrollTree() {
    if (m_scrollChildren) {
        WTF::HashSet<WebLayerImpl*> copy = *m_scrollChildren;
        for (WTF::HashSet<WebLayerImpl*>::iterator it = copy.begin(); it != copy.end(); ++it)
            (*it)->setScrollParent(nullptr);
    }

    ASSERT(!m_scrollChildren);
    setScrollParent(nullptr);
}

void WebLayerImpl::removeFromClipTree() {
    if (m_clipChildren) {
        WTF::HashSet<WebLayerImpl*> copy = *m_clipChildren;
        for (WTF::HashSet<WebLayerImpl*>::iterator it = copy.begin(); it != copy.end(); ++it)
            (*it)->setClipParent(nullptr);
    }

    ASSERT(!m_clipChildren);
    setClipParent(nullptr);
}

cc::LayerTreeHost* WebLayerImpl::layerTreeHost() const
{
    return m_layerTreeHost;
}

static void applyLayerActions(cc::LayerTreeHost* host, WTF::Vector<cc::LayerChangeAction*>* actions)
{
	for (size_t i = 0; i < actions->size(); ++i) {
		cc::LayerChangeAction* action = actions->at(i);
		ASSERT(-1 == action->actionId());
		action->setActionId(host->genActionId());
		host->appendLayerChangeAction(action);
	}
	actions->clear();
}

void WebLayerImpl::setLayerTreeHost(cc::LayerTreeHost* host)
{
    if (m_layerTreeHost == host)
        return;

    setNeedsCommit(true);

    if (host) {
		RELEASE_ASSERT(!m_layerTreeHost);
        if (m_drawsContent && !m_tileGrid && host->needTileRender())
            m_tileGrid = new cc::TileGrid(this);

        host->registerLayer(this);
		applyLayerActions(host, &m_savedActionsWhenHostIsNull);

        if (m_tileGrid)
            m_tileGrid->setTilesMutex(host->tilesMutex());

        if (0 != m_3dSortingContextId)
            host->increaseNodesCount();
    } else if (m_layerTreeHost) {
		ASSERT(m_layerTreeHost->isDestroying() || false);
//         if (m_tileGrid) {
//             m_layerTreeHost->preDrawFrame();
//             m_layerTreeHost->postDrawFrame();
//             m_tileGrid->waitForReleaseTilesInUIThread();
//         }
//         m_layerTreeHost->unregisterLayer(this);
	}

    m_layerTreeHost = host;

    if (m_scrollParent)
        m_scrollParent->setLayerTreeHost(host);

    if (m_clipParent)
        m_clipParent->setLayerTreeHost(host);
    
    for (size_t i = 0; i < m_children.size(); ++i)
        m_children[i]->setLayerTreeHost(host);
}

void WebLayerImpl::updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    if (m_client)
        m_client->updataAndPaintContents(canvas, clip);
    m_dirty = false;
    m_updateRectInRootLayerCoordinate = blink::IntRect();
}

cc::DrawProperties* WebLayerImpl::drawProperties()
{
    return m_drawProperties;
}

void WebLayerImpl::updataDrawToCanvasProperties(cc::DrawToCanvasProperties* prop)
{
	DebugBreak();
}

const SkMatrix44& WebLayerImpl::drawTransform() const
{
    return m_drawProperties->targetSpaceTransform;
}

bool WebLayerImpl::is3dSorted()
{
    return m_3dSortingContextId != 0;
}

bool WebLayerImpl::isMaskLayer() const
{
    return m_isMaskLayer;
}

bool WebLayerImpl::isReplicaLayer() const
{
    return m_isReplicaLayer;
}

bool WebLayerImpl::hasMaskLayerChild() const
{
    return m_hasMaskLayerChild;
}

static void appendChildrenToDirtyLayers(cc::RasterTaskGroup* taskGroup, WebLayerImpl* layer)
{
    taskGroup->appendDirtyLayer(layer);

    WebLayerImplList& children = layer->children();
    for (size_t i = 0; i < children.size(); ++i) {
        WebLayerImpl* child = children[i];
        taskGroup->appendDirtyLayer(child);
        appendChildrenToDirtyLayers(taskGroup, child);
    }
}

void WebLayerImpl::recordDrawChildren(cc::RasterTaskGroup* taskGroup, int deep)
{
    for (size_t i = 0; i < children().size(); ++i) {
        cc_blink::WebLayerImpl* child = children()[i];

        child->recordDraw(taskGroup);
        child->clearChildrenDirty();

        child->recordDrawChildren(taskGroup, deep + 1);
    }

    if (m_maskLayer)
        m_maskLayer->recordDraw(taskGroup);
}

void WebLayerImpl::recordDraw(cc::RasterTaskGroup* taskGroup)
{
    if (m_client)
        m_client->recordDraw(taskGroup);

    if (!m_updateRectInRootLayerCoordinate.isEmpty()) {
        taskGroup->appendPendingInvalidateRect(m_updateRectInRootLayerCoordinate);
        m_updateRectInRootLayerCoordinate = blink::IntRect();
    }

    if (m_dirty)  // ������ӽڵ�Ҳ����dirty����Ϊ���ڵ���˵Ļ����ӽڵ��combined_transformҲ���
        appendChildrenToDirtyLayers(taskGroup, this);
    m_dirty = false;
}

void WebLayerImpl::drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    if (m_client)
        m_client->drawToCanvas(canvas, clip);
    m_dirty = false;
    m_updateRectInRootLayerCoordinate = blink::IntRect();
}

int WebLayerImpl::id() const 
{
    return m_id;
}

static void clearLayerActions(WTF::Vector<cc::LayerChangeAction*>* actions)
{
	for (size_t i = 0; i < actions->size(); ++i) {
		cc::LayerChangeAction* action = actions->at(i);
		delete action;
	}
	actions->clear();
}

void WebLayerImpl::appendLayerChangeAction(cc::LayerChangeAction* action)
{
	if (m_layerTreeHost) {
        ASSERT(-1 == action->actionId());
		action->setActionId(m_layerTreeHost->genActionId());
		m_layerTreeHost->appendLayerChangeAction(action);
    } else if (m_parent) {
        m_parent->appendLayerChangeAction(action);
    } else
		m_savedActionsWhenHostIsNull.append(action);
}

void WebLayerImpl::addChild(WebLayer* child) 
{
    insertChild(static_cast<WebLayerImpl*>(child), m_children.size());
}

void WebLayerImpl::insertChild(WebLayer* child, size_t index)
{
    WebLayerImpl* childOfImpl = static_cast<WebLayerImpl*>(child);
    childOfImpl->removeFromParent();
    childOfImpl->setParent(this);
    childOfImpl->m_stackingOrderChanged = true;
	
	// ��������layerû��host
	if (!childOfImpl->layerTreeHost())
		childOfImpl->setLayerTreeHost(m_layerTreeHost);

    index = std::min(index, m_children.size());
    m_children.insert(index, childOfImpl);

    //setNeedsFullTreeSync();
    blink::IntRect invalidateRect(blink::IntPoint(child->position().x, child->position().y), child->bounds());
    appendPendingInvalidateRect(invalidateRect);// weolar TODO
    setNeedsCommit(false);
	appendLayerChangeAction(new cc::LayerChangeActionInsertChild(-1, id(), child->id(), index));
}

WebLayerImpl* WebLayerImpl::parent() const
{
    return m_parent;
}

bool WebLayerImpl::hasAncestor(blink::WebLayer* ancestor)
{
    for (const WebLayerImpl* layer = (const WebLayerImpl*)parent(); layer; layer = (const WebLayerImpl*)layer->parent()) {
        if (layer == ancestor)
            return true;
    }
    return false;
}

void WebLayerImpl::appendLayerActionsToParent()
{
	if (!m_parent)
		return;

	WTF::Vector<cc::LayerChangeAction*>* actions = &m_savedActionsWhenHostIsNull;
	for (size_t i = 0; i < actions->size(); ++i) {
		cc::LayerChangeAction* action = actions->at(i);
        m_parent->appendLayerChangeAction(action);
	}
	actions->clear();
}

void WebLayerImpl::setParent(blink::WebLayer* layer)
{
    WebLayerImpl* parent = (WebLayerImpl*)layer;
    ASSERT((!parent || !parent->hasAncestor(this)));
    m_parent = parent;

    // for debug
    if (m_isMaskLayer || m_hasMaskLayerChild) {
        WebLayerImpl* parentLayer = m_parent;
        while (parentLayer) {
            parentLayer->m_hasMaskLayerChild = true;
            parentLayer = parentLayer->parent();
        }
    }

    //setLayerTreeHost(m_parent ? m_parent->layerTreeHost() : nullptr);
	appendLayerActionsToParent();
}

void WebLayerImpl::replaceChild(blink::WebLayer* referenceWebLayer, blink::WebLayer* newWebLayer)
{
    WebLayerImpl* reference = (WebLayerImpl*)referenceWebLayer;
    WebLayer* newLayer = (WebLayerImpl*)newWebLayer;
    ASSERT(reference);
    ASSERT(reference->parent());

    if (reference == newLayer)
        return;

    int referenceIndex = indexOfChild(reference);
    if (referenceIndex == -1) {
        RELEASE_ASSERT(false);
        return;
    }

    reference->removeFromParent();

    if (newLayer) {
        newLayer->removeFromParent();
        insertChild(newLayer, referenceIndex);
    }
}

int WebLayerImpl::indexOfChild(WebLayerImpl* child) const
{
    for (size_t i = 0; i < m_children.size(); ++i) {
        if (m_children[i] == child)
            return i;
    }
    return -1;
}

void WebLayerImpl::removeFromParent()
{
    if (m_parent) {
        m_parent->removeChildOrDependent(this);
		appendLayerChangeAction(new cc::LayerChangeActionRemoveFromParent(-1, id()));
    }
}

void WebLayerImpl::removeChildOrDependent(WebLayerImpl* child) 
{
    if (m_maskLayer == child) {
        m_maskLayer->setParent(NULL);
        m_maskLayer = NULL;
        //setNeedsFullTreeSync();
        blink::IntRect invalidateRect(blink::IntPoint(child->position().x, child->position().y), child->bounds());
        appendPendingInvalidateRect(invalidateRect); // weolar TODO
        return;
    }
    if (m_replicaLayer == child) {
        m_replicaLayer->setParent(NULL);
        m_replicaLayer = NULL;
        //setNeedsFullTreeSync();
        blink::IntRect invalidateRect(blink::IntPoint(child->position().x, child->position().y), child->bounds());
        appendPendingInvalidateRect(invalidateRect); // weolar TODO
        return;
    }

    for (size_t iter = 0; iter != m_children.size(); ++iter) {
        if (m_children[iter] != child)
            continue;

        child->setParent(NULL);
        m_children.remove(iter);
        //setNeedsFullTreeSync();
        blink::IntRect invalidateRect(blink::IntPoint(child->position().x, child->position().y), child->bounds());
        appendPendingInvalidateRect(invalidateRect); // weolar TODO
        return;
    }
}

void WebLayerImpl::removeAllChildren()
{
    while (m_children.size()) {
        WebLayerImpl* layer = m_children[0];
        ASSERT(this == layer->parent());
        layer->removeFromParent();
    }
}

WebLayerImplList& WebLayerImpl::children()
{
    return m_children;
}

void WebLayerImpl::setBounds(const WebSize& size)
{
    if (m_bounds.width() == size.width && m_bounds.height() == size.height)
        return;

    setNeedsCommit(true);

    m_bounds = size;

    if (m_bounds.width() > 3000)
        m_bounds.setWidth(3000);

    if (m_bounds.height() > 3000)
        m_bounds.setHeight(3000);

    setNeedsCommit(true);
}

WebSize WebLayerImpl::bounds() const
{
    return m_bounds;
}

void WebLayerImpl::setMasksToBounds(bool masksToBounds)
{
    if (m_masksToBounds == masksToBounds)
        return;

    setNeedsCommit(true);
    m_masksToBounds = masksToBounds;
    setNeedsCommit(true);
}

bool WebLayerImpl::masksToBounds() const
{
    return m_masksToBounds;
}

void WebLayerImpl::setMaskLayer(WebLayer* maskLayer)
{
    if (!maskLayer) {
        if (nullptr == m_maskLayer)
            return;
        m_maskLayer = nullptr;
        setNeedsCommit(true);
        return;
    }

    if ((WebLayerImpl*)maskLayer == m_maskLayer)
        return;
    setNeedsCommit(true);
    m_maskLayer = (WebLayerImpl*)maskLayer;
    m_maskLayer->m_isMaskLayer = true;
    m_maskLayer->setParent(this);
    setNeedsCommit(true);

    debugMaskLayerId = m_id;

    // for debug
    WebLayerImpl* parentLayer = m_parent;
    while (parentLayer) {
        parentLayer->m_hasMaskLayerChild = true;
        parentLayer = parentLayer->parent();
    }
}

void WebLayerImpl::setReplicaLayer(WebLayer* replicaLayer)
{
    if (!replicaLayer) {
        if (nullptr == m_replicaLayer)
            return;
        m_replicaLayer = nullptr;
        setNeedsCommit(true);
        return;
    }

    if ((WebLayerImpl*)replicaLayer == m_replicaLayer)
        return;
    m_replicaLayer = (WebLayerImpl*)replicaLayer;
    m_replicaLayer->m_isReplicaLayer = true;
    m_replicaLayer->setParent(this);
    setNeedsCommit(true);
}

int WebLayerImpl::maskLayerId() const
{
    if (!m_maskLayer)
        return -1;
    return m_maskLayer->id();
}

int WebLayerImpl::replicaLayerId() const
{
    if (!m_replicaLayer)
        return -1;
    return m_replicaLayer->id();
}

void WebLayerImpl::setOpacity(float opacity) 
{
    if (m_opacity == opacity)
        return;
    m_opacity = opacity;
    setNeedsCommit(true);
}

float WebLayerImpl::opacity() const
{
    return m_opacity;
}

void WebLayerImpl::setBlendMode(blink::WebBlendMode blendMode)
{
    if (m_blendMode == blendMode)
        return;
    m_blendMode = blendMode;
    setNeedsCommit(true);
}

blink::WebBlendMode WebLayerImpl::blendMode() const
{
    return m_blendMode;
}

void WebLayerImpl::setIsRootForIsolatedGroup(bool isolate)
{
    if (m_setIsRootForIsolatedGroup == isolate)
        return;

    m_setIsRootForIsolatedGroup = isolate;
    setNeedsCommit(true);
}

bool WebLayerImpl::isRootForIsolatedGroup()
{
    return m_setIsRootForIsolatedGroup;
}

void WebLayerImpl::setOpaque(bool opaque)
{
    if (m_contentsOpaqueIsFixed)
        return;

    if (m_opaque == opaque)
        return;
    m_opaque = opaque;
    setNeedsCommit(true);
}

bool WebLayerImpl::opaque() const 
{
    return m_opaque;
}

void WebLayerImpl::setPosition(const WebFloatPoint& position)
{
    if (m_position.x() == position.x && m_position.y() == position.y)
        return;
    setNeedsCommit(true);
    m_position = position;
    setNeedsCommit(true);
}

WebFloatPoint WebLayerImpl::position() const 
{
    return m_position;
}

void WebLayerImpl::setTransform(const SkMatrix44& matrix) 
{
    if (m_transform == matrix)
        return;

    setNeedsCommit(true);
    m_transform = matrix;
    setNeedsCommit(true);
}

void WebLayerImpl::setTransformOrigin(const blink::WebFloatPoint3D& point)
{
    if (m_transformOrigin.x() == point.x && m_transformOrigin.y() == point.y && m_transformOrigin.z() == point.z)
        return;

    setNeedsCommit(true);
    m_transformOrigin = point;
    setNeedsCommit(true);
}

blink::WebFloatPoint3D WebLayerImpl::transformOrigin() const
{
    return m_transformOrigin;
}

SkMatrix44 WebLayerImpl::transform() const
{
    return m_transform;
}

void WebLayerImpl::setDrawsContent(bool drawsContent)
{
    if (m_drawsContent == drawsContent)
        return;
    m_drawsContent = drawsContent;

    bool notNeedTileRender = (m_layerTreeHost && !m_layerTreeHost->needTileRender());

    if (m_drawsContent && !m_tileGrid && (!notNeedTileRender))
        m_tileGrid = new cc::TileGrid(this);
    if (!m_drawsContent && m_tileGrid) {
        delete m_tileGrid;
        m_tileGrid = nullptr;
    }

    if (m_tileGrid && m_layerTreeHost)
        m_tileGrid->setTilesMutex(m_layerTreeHost->tilesMutex());
    setNeedsCommit(true);
}

bool WebLayerImpl::drawsContent() const
{
    return m_drawsContent;
}

void WebLayerImpl::setShouldFlattenTransform(bool flatten) 
{
    if (m_shouldFlattenTransform == flatten)
        return;
    m_shouldFlattenTransform = flatten;

    setNeedsCommit(true);
}

bool WebLayerImpl::shouldFlattenTransform()
{
    return m_shouldFlattenTransform;
}

void WebLayerImpl::setRenderingContext(int context)
{
    if (m_3dSortingContextId == context)
        return;
    m_3dSortingContextId = context;
    if (0 != m_3dSortingContextId && m_layerTreeHost)
        m_layerTreeHost->increaseNodesCount();
    else if (0 == m_3dSortingContextId && m_layerTreeHost)
        m_layerTreeHost->decreaseNodesCount();
    setNeedsCommit(true);
}

void WebLayerImpl::setUseParentBackfaceVisibility(bool use_parent_backface_visibility) 
{
    if (m_useParentBackfaceVisibility == use_parent_backface_visibility)
        return;
    m_useParentBackfaceVisibility = use_parent_backface_visibility;
    setNeedsCommit(true);
}

void WebLayerImpl::setBackgroundColor(WebColor color) 
{
    if (m_backgroundColor == color)
        return;
    m_backgroundColor = color;
    setNeedsCommit(true);
}

WebColor WebLayerImpl::backgroundColor() const
{
    return m_backgroundColor;
}

void WebLayerImpl::setFilters(const WebFilterOperations& filters)
{
    if (filters.isEmpty())
        return;
    //setNeedsCommit(true);
}

void WebLayerImpl::setAnimationDelegate(blink::WebCompositorAnimationDelegate* delegate)
{
    notImplemented();
}

bool WebLayerImpl::addAnimation(blink::WebCompositorAnimation* animation) 
{
    notImplemented();
    return true;
}

void WebLayerImpl::removeAnimation(int animation_id) 
{
    notImplemented();
}

void WebLayerImpl::removeAnimation(int animation_id, blink::WebCompositorAnimation::TargetProperty target_property) 
{
    notImplemented();
}

void WebLayerImpl::pauseAnimation(int animation_id, double time_offset) 
{
    notImplemented();
}

bool WebLayerImpl::hasActiveAnimation() 
{
    notImplemented();
    return false;
}

void WebLayerImpl::setForceRenderSurface(bool force_render_surface) {

}

void WebLayerImpl::setScrollPositionDouble(blink::WebDoublePoint position) 
{
    if (m_scrollPositionDouble == position)
        return;
    m_scrollPositionDouble = position;
    setNeedsCommit(true);
}

void WebLayerImpl::setScrollCompensationAdjustment(blink::WebDoublePoint position) 
{
    if (m_scrollCompensationAdjustment == position)
        return;
    m_scrollCompensationAdjustment = position;
    setNeedsCommit(true);
}

blink::WebDoublePoint WebLayerImpl::scrollPositionDouble() const
{
    return m_scrollPositionDouble;
}

void WebLayerImpl::setScrollClipLayer(WebLayer* clip_layer)
{
    if (!clip_layer) {
        m_scrollClipLayerId = -1;
        return;
    }
    if (m_scrollClipLayerId == clip_layer->id())
        return;
    m_scrollClipLayerId = clip_layer->id();
    setNeedsCommit(true);
}

bool WebLayerImpl::scrollable() const
{
    //return layer_->scrollable();
    return m_scrollClipLayerId != -1;
}

void WebLayerImpl::setUserScrollable(bool horizontal, bool vertical)
{
    if (m_userScrollableHorizontal == horizontal && m_userScrollableVertical == vertical)
        return;
    m_userScrollableHorizontal = horizontal;
    m_userScrollableVertical = vertical;
    setNeedsCommit(true);
}

bool WebLayerImpl::userScrollableHorizontal() const
{
    //return layer_->user_scrollable_horizontal();
    return m_userScrollableHorizontal;
}

bool WebLayerImpl::userScrollableVertical() const
{
    //return layer_->user_scrollable_vertical();
    return m_userScrollableVertical;
}

void WebLayerImpl::setHaveWheelEventHandlers(bool have_wheel_event_handlers)
{
    if (m_haveWheelEventHandlers == have_wheel_event_handlers)
        return;
    m_haveWheelEventHandlers = have_wheel_event_handlers;
    setNeedsCommit(false);
}

bool WebLayerImpl::haveWheelEventHandlers() const 
{
    return m_haveWheelEventHandlers;
}

void WebLayerImpl::setHaveScrollEventHandlers(bool have_scroll_event_handlers) 
{
    if (m_haveScrollEventHandlers == have_scroll_event_handlers)
        return;
    m_haveScrollEventHandlers = have_scroll_event_handlers;
    setNeedsCommit(true);
}

bool WebLayerImpl::haveScrollEventHandlers() const 
{
    return m_haveScrollEventHandlers;
}

void WebLayerImpl::setShouldScrollOnMainThread(bool should_scroll_on_main_thread) 
{
    if (m_shouldScrollOnMainThread == should_scroll_on_main_thread)
        return;
    m_shouldScrollOnMainThread = should_scroll_on_main_thread;
    setNeedsCommit(true);
}

bool WebLayerImpl::shouldScrollOnMainThread() const {
    return m_shouldScrollOnMainThread;
}

void WebLayerImpl::setNonFastScrollableRegion(const WebVector<WebRect>& rects)
{
//     if (0 == rects.size())
//         return;
// 
//     size_t size = m_nonFastScrollableRegion.size();
//     for (size_t j = 0; j < rects.size(); ++j) {
//         bool find = false;
//         blink::IntRect rect(rects[j]);
//         for (size_t i = 0; i < m_nonFastScrollableRegion.size(); ++i) {
//             if (!m_nonFastScrollableRegion[i].contains(rect))
//                 continue;
//             find = true;
//         }
//         m_nonFastScrollableRegion.append(rect);
//     }
// 
//     if (size != m_nonFastScrollableRegion.size())
//         setNeedsCommit(true);
}

WebVector<WebRect> WebLayerImpl::nonFastScrollableRegion() const
{
    WebVector<WebRect> result(m_nonFastScrollableRegion.size());;
    for (size_t i = 0; i < m_nonFastScrollableRegion.size(); ++i) {
        result[i] = WebRect(m_nonFastScrollableRegion[i]);
    }
    return result;
}

void WebLayerImpl::setFrameTimingRequests(const WebVector<std::pair<int64_t, WebRect>>& requests) 
{
    notImplemented();
}

WebVector<std::pair<int64_t, WebRect>> WebLayerImpl::frameTimingRequests() const 
{
    notImplemented();
    return WebVector<std::pair<int64_t, WebRect>>();
}

void WebLayerImpl::setTouchEventHandlerRegion(const WebVector<WebRect>& rects) 
{
    if (0 == rects.size())
        return;

    for (size_t i = 0; i < rects.size(); ++i) {
        m_touchEventHandlerRegions.unite((blink::IntRect)rects[i]);
    }
    setNeedsCommit(true);
}

WebVector<WebRect> WebLayerImpl::touchEventHandlerRegion() const 
{
    notImplemented();
    return WebVector<WebRect>();
}

void WebLayerImpl::setScrollBlocksOn(blink::WebScrollBlocksOn blocks) 
{
    if (m_scrollBlocksOn == blocks)
        return;
    m_scrollBlocksOn = blocks;
    setNeedsCommit(true);
}

blink::WebScrollBlocksOn WebLayerImpl::scrollBlocksOn() const
{
    return m_scrollBlocksOn;
}

void WebLayerImpl::setIsContainerForFixedPositionLayers(bool enable) 
{
    if (m_isContainerForFixedPositionLayers == enable)
        return;
    m_isContainerForFixedPositionLayers = enable;
    setNeedsCommit(true);
}

bool WebLayerImpl::isContainerForFixedPositionLayers() const 
{
    return m_isContainerForFixedPositionLayers;
}

void WebLayerImpl::setPositionConstraint(const blink::WebLayerPositionConstraint& constraint) 
{
    if (m_positionConstraint.isFixedPosition == constraint.isFixedPosition &&
        m_positionConstraint.isFixedToRightEdge == constraint.isFixedToRightEdge &&
        m_positionConstraint.isFixedToBottomEdge == constraint.isFixedToBottomEdge)
        return;
    setNeedsCommit(true);
    m_positionConstraint = constraint;
    setNeedsCommit(true);
}

blink::WebLayerPositionConstraint WebLayerImpl::positionConstraint() const
{
    return m_positionConstraint;
}

void WebLayerImpl::setScrollClient(blink::WebLayerScrollClient* scroll_client) 
{
    if (m_webLayerScrollClient == scroll_client)
        return;
    m_webLayerScrollClient = scroll_client;
}

bool WebLayerImpl::isOrphan() const 
{
    return !m_layerTreeHost;
}

void WebLayerImpl::setWebLayerClient(blink::WebLayerClient* client) 
{
    m_webLayerClient = client;
}

void WebLayerImpl::setScrollParent(blink::WebLayer* parent)
{
    if (m_scrollParent == (cc_blink::WebLayerImpl*)parent)
        return;
    m_scrollParent = (cc_blink::WebLayerImpl*)parent;

    if (m_scrollParent)
        m_scrollParent->addScrollChild(this);

    setNeedsCommit(true);
}

void WebLayerImpl::addScrollChild(WebLayerImpl* child) 
{
    if (!m_scrollChildren)
        m_scrollChildren = new WTF::HashSet<WebLayerImpl*>();

    m_scrollChildren->add(child);
    setNeedsCommit(true);
}

void WebLayerImpl::setClipParent(blink::WebLayer* parent)
{
    if (m_clipParent == (cc_blink::WebLayerImpl*)parent)
        return;
    m_clipParent = (cc_blink::WebLayerImpl*)parent;

    if (m_clipParent)
        m_clipParent->addScrollChild(this);
    
    setNeedsCommit(true);
}

void WebLayerImpl::addClipChild(WebLayerImpl* child)
{
    if (!m_clipChildren)
        m_clipChildren = new WTF::HashSet<WebLayerImpl*> ();
    m_clipChildren->add(child);
    setNeedsCommit(true);
}

void WebLayerImpl::setChildrenDirty()
{
    m_childrenDirty = true;
}

void WebLayerImpl::clearChildrenDirty()
{
    m_childrenDirty = false;
}

void WebLayerImpl::setAllParentDirty()
{
    WebLayerImpl* layer = parent();
    while (layer) {
        layer->setChildrenDirty();
        layer = layer->parent();
    }
}

void WebLayerImpl::setContentsOpaqueIsFixed(bool fixed)
{
    m_contentsOpaqueIsFixed = fixed;
}

blink::IntRect WebLayerImpl::mapRectFromRootLayerCoordinateToCurrentLayer(const blink::IntRect& rect)
{
    SkMatrix44 combinedTransformAll;
    SkRect currentLayerRect((SkRect)rect);
    const WebLayerImpl* parentLayer = this;
    while (parentLayer) {
        WebFloatPoint currentLayerPosition = parentLayer->position();
        WebDoublePoint effectiveTotalScrollOffset = parentLayer->scrollPositionDouble();
        WebFloatPoint currentLayerPositionScrolled(currentLayerPosition.x - effectiveTotalScrollOffset.x, currentLayerPosition.y - effectiveTotalScrollOffset.y);

        WebFloatPoint3D transformOrigin = parentLayer->transformOrigin();

        SkMatrix44 combinedTransform(SkMatrix44::kIdentity_Constructor);
        combinedTransform.preTranslate(currentLayerPositionScrolled.x + transformOrigin.x, currentLayerPositionScrolled.y + transformOrigin.y, transformOrigin.z);
        combinedTransform.preConcat(parentLayer->transform());
        combinedTransform.preTranslate(-transformOrigin.x, -transformOrigin.y, -transformOrigin.z);

        combinedTransformAll.postConcat(combinedTransform);
        parentLayer = parentLayer->parent();
    }

    SkMatrix44 combinedMatrix;
    combinedTransformAll.invert(&combinedMatrix);
    ((SkMatrix)combinedMatrix).mapRect(&currentLayerRect);

    return IntRect((int)floor(currentLayerRect.x()), (int)floor(currentLayerRect.y()), 
        1 + (int)ceil(currentLayerRect.width()), 1 + (int)ceil(currentLayerRect.height()));
}


blink::IntRect WebLayerImpl::mapRectFromCurrentLayerCoordinateToRootLayer(const blink::IntRect& rect)
{
    SkMatrix44 combinedTransformAll;
    SkRect rootLayerRect((SkRect)rect);
    const WebLayerImpl* parentLayer = this;
    while (parentLayer) {
        WebFloatPoint currentLayerPosition = parentLayer->position();
        WebDoublePoint effectiveTotalScrollOffset = parentLayer->scrollPositionDouble();
        WebFloatPoint currentLayerPositionScrolled(currentLayerPosition.x - effectiveTotalScrollOffset.x, currentLayerPosition.y - effectiveTotalScrollOffset.y);

        WebFloatPoint3D transformOrigin = parentLayer->transformOrigin();

        SkMatrix44 combinedTransform(SkMatrix44::kIdentity_Constructor);
        combinedTransform.preTranslate(currentLayerPositionScrolled.x + transformOrigin.x, currentLayerPositionScrolled.y + transformOrigin.y, transformOrigin.z);
        combinedTransform.preConcat(parentLayer->transform());
        combinedTransform.preTranslate(-transformOrigin.x, -transformOrigin.y, -transformOrigin.z);

        combinedTransformAll.postConcat(combinedTransform);
        parentLayer = parentLayer->parent();
    }

    ((SkMatrix)combinedTransformAll).mapRect(&rootLayerRect);

    // ��ʱû�㶮���1+��ʲô��˼�������ӵĻ��ڶ���ʱ����в�Ӱ
    return IntRect((int)floor(rootLayerRect.x()), (int)floor(rootLayerRect.y()), 1 + (int)ceil(rootLayerRect.width()), 1 + (int)ceil(rootLayerRect.height()));
}

// invalidate��requestRepaint����������һ�����ڹ�դ���̷߳���ui�߳�ˢ�£�������ֱ������
// ���ڸ��ĳ���һ���դ���������ٷ���
void WebLayerImpl::requestRepaint(const blink::IntRect& rect)
{
    IntRect drawRect(rect);
    drawRect.intersect(blink::IntRect(blink::IntPoint(), m_bounds));

    drawRect = mapRectFromCurrentLayerCoordinateToRootLayer(drawRect);
    
    if (drawRect.isEmpty())
        return;

//     String outString = String::format("WebLayerImpl::requestRepaint: %p %d %d\n", this, drawRect.width(), drawRect.height());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    if (m_layerTreeHost)
        m_layerTreeHost->requestRepaint(drawRect);
}

void WebLayerImpl::appendPendingInvalidateRect(const blink::IntRect& rect)
{
    IntRect drawRect(rect);
    drawRect.intersect(blink::IntRect(blink::IntPoint(), m_bounds));

    drawRect = mapRectFromCurrentLayerCoordinateToRootLayer(drawRect);
    m_updateRectInRootLayerCoordinate.unite((blink::IntRect)(drawRect));

//     if (drawRect.width() > 500 && drawRect.height() > 500)
//         OutputDebugStringA("WebLayerImpl::appendPendingInvalidateRect\n");
//     if (drawRect.width() == 160 && drawRect.height() == 58)
//         OutputDebugStringA("WebLayerImpl::appendPendingInvalidateRect\n");

//     String outString = String::format("WebLayerImpl::appendPendingInvalidateRect:%d %d %d %d\n",
//         drawRect.x(), drawRect.y(), drawRect.width(), drawRect.height());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void WebLayerImpl::requestBoundRepaint(bool directOrPending)
{
    blink::IntRect rect(blink::IntPoint(), m_bounds);
    if (directOrPending)
        requestRepaint(rect);
    else
        appendPendingInvalidateRect(rect);
}

void WebLayerImpl::invalidateRect(const blink::WebRect& rect)
{
    blink::IntRect dirtyRect(rect);
    if (m_tileGrid)
        m_tileGrid->invalidate(dirtyRect, false);
    setNeedsCommit(false);
}

void WebLayerImpl::invalidate()
{
    invalidateRect(blink::WebRect(0, 0, m_bounds.width(), m_bounds.height())); // ˢ�µ�ʱ������ϵ���Ա�layerΪ��
}

void WebLayerImpl::setNeedsCommit(bool needUpdateAllBoundsArea)
{
    m_dirty = true;
    if (!m_layerTreeHost)
        return;

    setAllParentDirty();
    if (needUpdateAllBoundsArea)
        requestBoundRepaint(false);
    m_layerTreeHost->setNeedsCommit();
}

void WebLayerImpl::setNeedsFullTreeSync()
{
    if (m_layerTreeHost)
        m_layerTreeHost->setNeedsFullTreeSync();
    setNeedsCommit(true);
}

void WebLayerImpl::onRasterFinish(int layerId, const blink::IntRect& rect)
{
//     if (!s_liveLayers->contains(layerId))
//         return;
    DebugBreak();

    blink::IntRect updateRect;
    updateRect.unite(rect);

//     if (needUpdateAllBoundsArea) {
//         updateRect.unite(blink::IntRect(0, 0, m_bounds.width(), m_bounds.height()));
//     }

    updateRect = mapRectFromCurrentLayerCoordinateToRootLayer(updateRect);

//     String outString = String::format("RasterTask::notifyMainThreadRasterFinish Y: %d\n", updateRect.y());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    m_layerTreeHost->requestRepaint(updateRect);
}

bool WebLayerImpl::dirty() const
{
    return m_dirty;
}

bool WebLayerImpl::childrenDirty() const
{
    return m_childrenDirty;
}

}  // namespace cc_blink
