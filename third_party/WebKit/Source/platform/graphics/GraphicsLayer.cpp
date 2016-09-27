/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/graphics/GraphicsLayer.h"

#include "SkImageFilter.h"
#include "SkMatrix44.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/FirstPaintInvalidationTracking.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayerFactory.h"
#include "platform/graphics/Image.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/scroll/ScrollableArea.h"
#include "platform/text/TextStream.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositorAnimation.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebFilterOperations.h"
#include "public/platform/WebFloatPoint.h"
#include "public/platform/WebFloatRect.h"
#include "public/platform/WebGraphicsLayerDebugInfo.h"
#include "public/platform/WebLayer.h"
#include "public/platform/WebPoint.h"
#include "public/platform/WebSize.h"
#include "wtf/CurrentTime.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/text/WTFString.h"
#include <algorithm>

#ifndef NDEBUG
#include <stdio.h>
#endif

namespace blink {

struct PaintInvalidationTrackingInfo {
    Vector<FloatRect> invalidationRects;
    Vector<String> invalidationObjects;
};

typedef HashMap<const GraphicsLayer*, PaintInvalidationTrackingInfo> PaintInvalidationTrackingMap;
static PaintInvalidationTrackingMap& paintInvalidationTrackingMap()
{
    DEFINE_STATIC_LOCAL(PaintInvalidationTrackingMap, map, ());
    return map;
}

PassOwnPtr<GraphicsLayer> GraphicsLayer::create(GraphicsLayerFactory* factory, GraphicsLayerClient* client)
{
    return factory->createGraphicsLayer(client);
}

GraphicsLayer::GraphicsLayer(GraphicsLayerClient* client)
    : m_client(client)
    , m_backgroundColor(Color::transparent)
    , m_opacity(1)
    , m_blendMode(WebBlendModeNormal)
    , m_scrollBlocksOn(WebScrollBlocksOnNone)
    , m_hasTransformOrigin(false)
    , m_contentsOpaque(false)
    , m_shouldFlattenTransform(true)
    , m_backfaceVisibility(true)
    , m_masksToBounds(false)
    , m_drawsContent(false)
    , m_contentsVisible(true)
    , m_isRootForIsolatedGroup(false)
    , m_hasScrollParent(false)
    , m_hasClipParent(false)
    , m_paintingPhase(GraphicsLayerPaintAllWithOverflowClip)
    , m_parent(0)
    , m_maskLayer(0)
    , m_contentsClippingMaskLayer(0)
    , m_replicaLayer(0)
    , m_replicatedLayer(0)
    , m_paintCount(0)
    , m_contentsLayer(0)
    , m_contentsLayerId(0)
    , m_scrollableArea(0)
    , m_3dRenderingContext(0)
{
#if ENABLE(ASSERT)
    if (m_client)
        m_client->verifyNotPainting();
#endif

    m_contentLayerDelegate = adoptPtr(new ContentLayerDelegate(this));
    m_layer = adoptPtr(Platform::current()->compositorSupport()->createContentLayer(m_contentLayerDelegate.get()));
    m_layer->layer()->setDrawsContent(m_drawsContent && m_contentsVisible);
    m_layer->layer()->setWebLayerClient(this);
    m_layer->setAutomaticallyComputeRasterScale(true);
}

GraphicsLayer::~GraphicsLayer()
{
    for (size_t i = 0; i < m_linkHighlights.size(); ++i)
        m_linkHighlights[i]->clearCurrentGraphicsLayer();
    m_linkHighlights.clear();

#if ENABLE(ASSERT)
    if (m_client)
        m_client->verifyNotPainting();
#endif

    if (m_replicaLayer)
        m_replicaLayer->setReplicatedLayer(0);

    if (m_replicatedLayer)
        m_replicatedLayer->setReplicatedByLayer(0);

    removeAllChildren();
    removeFromParent();

    resetTrackedPaintInvalidations();
    ASSERT(!m_parent);
}

void GraphicsLayer::setParent(GraphicsLayer* layer)
{
    ASSERT(!layer || !layer->hasAncestor(this));
    m_parent = layer;
}

#if ENABLE(ASSERT)

bool GraphicsLayer::hasAncestor(GraphicsLayer* ancestor) const
{
    for (GraphicsLayer* curr = parent(); curr; curr = curr->parent()) {
        if (curr == ancestor)
            return true;
    }

    return false;
}

#endif

bool GraphicsLayer::setChildren(const GraphicsLayerVector& newChildren)
{
    // If the contents of the arrays are the same, nothing to do.
    if (newChildren == m_children)
        return false;

    removeAllChildren();

    size_t listSize = newChildren.size();
    for (size_t i = 0; i < listSize; ++i)
        addChildInternal(newChildren[i]);

    updateChildList();

    return true;
}

void GraphicsLayer::addChildInternal(GraphicsLayer* childLayer)
{
    ASSERT(childLayer != this);

    if (childLayer->parent())
        childLayer->removeFromParent();

    childLayer->setParent(this);
    m_children.append(childLayer);

    // Don't call updateChildList here, this function is used in cases where it
    // should not be called until all children are processed.
}

void GraphicsLayer::addChild(GraphicsLayer* childLayer)
{
    addChildInternal(childLayer);
    updateChildList();
}

void GraphicsLayer::addChildBelow(GraphicsLayer* childLayer, GraphicsLayer* sibling)
{
    ASSERT(childLayer != this);
    childLayer->removeFromParent();

    bool found = false;
    for (unsigned i = 0; i < m_children.size(); i++) {
        if (sibling == m_children[i]) {
            m_children.insert(i, childLayer);
            found = true;
            break;
        }
    }

    childLayer->setParent(this);

    if (!found)
        m_children.append(childLayer);

    updateChildList();
}

void GraphicsLayer::removeAllChildren()
{
    while (!m_children.isEmpty()) {
        GraphicsLayer* curLayer = m_children.last();
        ASSERT(curLayer->parent());
        curLayer->removeFromParent();
    }
}

void GraphicsLayer::removeFromParent()
{
    if (m_parent) {
        // We use reverseFind so that removeAllChildren() isn't n^2.
        m_parent->m_children.remove(m_parent->m_children.reverseFind(this));
        setParent(0);
    }

    platformLayer()->removeFromParent();
}

void GraphicsLayer::setReplicatedByLayer(GraphicsLayer* layer)
{
    // FIXME: this could probably be a full early exit.
    if (m_replicaLayer != layer) {
        if (m_replicaLayer)
            m_replicaLayer->setReplicatedLayer(0);

        if (layer)
            layer->setReplicatedLayer(this);

        m_replicaLayer = layer;
    }

    WebLayer* webReplicaLayer = layer ? layer->platformLayer() : 0;
    platformLayer()->setReplicaLayer(webReplicaLayer);
}

void GraphicsLayer::setOffsetFromLayoutObject(const IntSize& offset, ShouldSetNeedsDisplay shouldSetNeedsDisplay)
{
    setOffsetDoubleFromLayoutObject(offset);
}

void GraphicsLayer::setOffsetDoubleFromLayoutObject(const DoubleSize& offset, ShouldSetNeedsDisplay shouldSetNeedsDisplay)
{
    if (offset == m_offsetFromLayoutObject)
        return;

    m_offsetFromLayoutObject = offset;

    // If the compositing layer offset changes, we need to repaint.
    if (shouldSetNeedsDisplay == SetNeedsDisplay)
        setNeedsDisplay();
}

void GraphicsLayer::paintGraphicsLayerContents(GraphicsContext& context, const IntRect& clip)
{
    if (!m_client)
        return;
    if (firstPaintInvalidationTrackingEnabled())
        m_debugInfo.clearAnnotatedInvalidateRects();
    incrementPaintCount();
#ifndef NDEBUG
    if (m_displayItemList && contentsOpaque()) {
        ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
        FloatRect rect(FloatPoint(), size());
        if (!DrawingRecorder::useCachedDrawingIfPossible(context, *this, DisplayItem::DebugRedFill)) {
            DrawingRecorder recorder(context, *this, DisplayItem::DebugRedFill, rect);
            context.fillRect(rect, SK_ColorRED);
        }
    }
#endif
    m_client->paintContents(this, context, m_paintingPhase, clip);
}

void GraphicsLayer::updateChildList()
{
    WebLayer* childHost = m_layer->layer();
    childHost->removeAllChildren();

    clearContentsLayerIfUnregistered();

    if (m_contentsLayer) {
        // FIXME: add the contents layer in the correct order with negative z-order children.
        // This does not cause visible rendering issues because currently contents layers are only used
        // for replaced elements that don't have children.
        childHost->addChild(m_contentsLayer);
    }

    for (size_t i = 0; i < m_children.size(); ++i)
        childHost->addChild(m_children[i]->platformLayer());

    for (size_t i = 0; i < m_linkHighlights.size(); ++i)
        childHost->addChild(m_linkHighlights[i]->layer());
}

void GraphicsLayer::updateLayerIsDrawable()
{
    // For the rest of the accelerated compositor code, there is no reason to make a
    // distinction between drawsContent and contentsVisible. So, for m_layer->layer(), these two
    // flags are combined here. m_contentsLayer shouldn't receive the drawsContent flag
    // so it is only given contentsVisible.

    m_layer->layer()->setDrawsContent(m_drawsContent && m_contentsVisible);
    if (WebLayer* contentsLayer = contentsLayerIfRegistered())
        contentsLayer->setDrawsContent(m_contentsVisible);

    if (m_drawsContent) {
        m_layer->layer()->invalidate();
        for (size_t i = 0; i < m_linkHighlights.size(); ++i)
            m_linkHighlights[i]->invalidate();
    }
}

void GraphicsLayer::updateContentsRect()
{
    WebLayer* contentsLayer = contentsLayerIfRegistered();
    if (!contentsLayer)
        return;

    contentsLayer->setPosition(FloatPoint(m_contentsRect.x(), m_contentsRect.y()));
    contentsLayer->setBounds(IntSize(m_contentsRect.width(), m_contentsRect.height()));

    if (m_contentsClippingMaskLayer) {
        if (m_contentsClippingMaskLayer->size() != m_contentsRect.size()) {
            m_contentsClippingMaskLayer->setSize(m_contentsRect.size());
            m_contentsClippingMaskLayer->setNeedsDisplay();
        }
        m_contentsClippingMaskLayer->setPosition(FloatPoint());
        m_contentsClippingMaskLayer->setOffsetFromLayoutObject(offsetFromLayoutObject() + IntSize(m_contentsRect.location().x(), m_contentsRect.location().y()));
    }
}

static HashSet<int>* s_registeredLayerSet;

void GraphicsLayer::registerContentsLayer(WebLayer* layer)
{
    if (!s_registeredLayerSet)
        s_registeredLayerSet = new HashSet<int>;
    if (s_registeredLayerSet->contains(layer->id()))
        CRASH();
    s_registeredLayerSet->add(layer->id());
}

void GraphicsLayer::unregisterContentsLayer(WebLayer* layer)
{
    ASSERT(s_registeredLayerSet);
    if (!s_registeredLayerSet->contains(layer->id()))
        CRASH();
    s_registeredLayerSet->remove(layer->id());
}

void GraphicsLayer::setContentsTo(WebLayer* layer)
{
    bool childrenChanged = false;
    if (layer) {
        ASSERT(s_registeredLayerSet);
        if (!s_registeredLayerSet->contains(layer->id()))
            CRASH();
        if (m_contentsLayerId != layer->id()) {
            setupContentsLayer(layer);
            childrenChanged = true;
        }
        updateContentsRect();
    } else {
        if (m_contentsLayer) {
            childrenChanged = true;

            // The old contents layer will be removed via updateChildList.
            m_contentsLayer = 0;
            m_contentsLayerId = 0;
        }
    }

    if (childrenChanged)
        updateChildList();
}

void GraphicsLayer::setupContentsLayer(WebLayer* contentsLayer)
{
    ASSERT(contentsLayer);
    m_contentsLayer = contentsLayer;
    m_contentsLayerId = m_contentsLayer->id();

    m_contentsLayer->setWebLayerClient(this);
    m_contentsLayer->setTransformOrigin(FloatPoint3D());
    m_contentsLayer->setUseParentBackfaceVisibility(true);

    // It is necessary to call setDrawsContent as soon as we receive the new contentsLayer, for
    // the correctness of early exit conditions in setDrawsContent() and setContentsVisible().
    m_contentsLayer->setDrawsContent(m_contentsVisible);

    // Insert the content layer first. Video elements require this, because they have
    // shadow content that must display in front of the video.
    m_layer->layer()->insertChild(m_contentsLayer, 0);
    WebLayer* borderWebLayer = m_contentsClippingMaskLayer ? m_contentsClippingMaskLayer->platformLayer() : 0;
    m_contentsLayer->setMaskLayer(borderWebLayer);

    m_contentsLayer->setRenderingContext(m_3dRenderingContext);
}

void GraphicsLayer::clearContentsLayerIfUnregistered()
{
    if (!m_contentsLayerId || s_registeredLayerSet->contains(m_contentsLayerId))
        return;

    m_contentsLayer = 0;
    m_contentsLayerId = 0;
}

GraphicsLayerDebugInfo& GraphicsLayer::debugInfo()
{
    return m_debugInfo;
}

WebGraphicsLayerDebugInfo* GraphicsLayer::takeDebugInfoFor(WebLayer* layer)
{
    GraphicsLayerDebugInfo* clone = m_debugInfo.clone();
    clone->setDebugName(debugName(layer));
    return clone;
}

WebLayer* GraphicsLayer::contentsLayerIfRegistered()
{
    clearContentsLayerIfUnregistered();
    return m_contentsLayer;
}

void GraphicsLayer::resetTrackedPaintInvalidations()
{
    paintInvalidationTrackingMap().remove(this);
}

void GraphicsLayer::trackPaintInvalidationRect(const FloatRect& rect)
{
    if (rect.isEmpty())
        return;

    // The caller must check isTrackingPaintInvalidations() before calling this method
    // to avoid constructing the rect unnecessarily.
    ASSERT(isTrackingPaintInvalidations());

    paintInvalidationTrackingMap().add(this, PaintInvalidationTrackingInfo()).storedValue->value.invalidationRects.append(rect);
}

void GraphicsLayer::trackPaintInvalidationObject(const String& objectDebugString)
{
    if (objectDebugString.isEmpty())
        return;

    // The caller must check isTrackingPaintInvalidations() before calling this method
    // because constructing the debug string will be costly.
    ASSERT(isTrackingPaintInvalidations());

    if (!RuntimeEnabledFeatures::slimmingPaintEnabled())
        return;

    paintInvalidationTrackingMap().add(this, PaintInvalidationTrackingInfo()).storedValue->value.invalidationObjects.append(objectDebugString);
}

static bool compareFloatRects(const FloatRect& a, const FloatRect& b)
{
    if (a.x() != b.x())
        return a.x() > b.x();
    if (a.y() != b.y())
        return a.y() > b.y();
    if (a.width() != b.width())
        return a.width() > b.width();
    return a.height() > b.height();
}

template <typename T>
static PassRefPtr<JSONArray> pointAsJSONArray(const T& point)
{
    RefPtr<JSONArray> array = adoptRef(new JSONArray);
    array->pushNumber(point.x());
    array->pushNumber(point.y());
    return array;
}

template <typename T>
static PassRefPtr<JSONArray> sizeAsJSONArray(const T& size)
{
    RefPtr<JSONArray> array = adoptRef(new JSONArray);
    array->pushNumber(size.width());
    array->pushNumber(size.height());
    return array;
}

template <typename T>
static PassRefPtr<JSONArray> rectAsJSONArray(const T& rect)
{
    RefPtr<JSONArray> array = adoptRef(new JSONArray);
    array->pushNumber(rect.x());
    array->pushNumber(rect.y());
    array->pushNumber(rect.width());
    array->pushNumber(rect.height());
    return array;
}

static double roundCloseToZero(double number)
{
    return std::abs(number) < 1e-7 ? 0 : number;
}

static PassRefPtr<JSONArray> transformAsJSONArray(const TransformationMatrix& t)
{
    RefPtr<JSONArray> array = adoptRef(new JSONArray);
    {
        RefPtr<JSONArray> row = adoptRef(new JSONArray);
        row->pushNumber(roundCloseToZero(t.m11()));
        row->pushNumber(roundCloseToZero(t.m12()));
        row->pushNumber(roundCloseToZero(t.m13()));
        row->pushNumber(roundCloseToZero(t.m14()));
        array->pushArray(row);
    }
    {
        RefPtr<JSONArray> row = adoptRef(new JSONArray);
        row->pushNumber(roundCloseToZero(t.m21()));
        row->pushNumber(roundCloseToZero(t.m22()));
        row->pushNumber(roundCloseToZero(t.m23()));
        row->pushNumber(roundCloseToZero(t.m24()));
        array->pushArray(row);
    }
    {
        RefPtr<JSONArray> row = adoptRef(new JSONArray);
        row->pushNumber(roundCloseToZero(t.m31()));
        row->pushNumber(roundCloseToZero(t.m32()));
        row->pushNumber(roundCloseToZero(t.m33()));
        row->pushNumber(roundCloseToZero(t.m34()));
        array->pushArray(row);
    }
    {
        RefPtr<JSONArray> row = adoptRef(new JSONArray);
        row->pushNumber(roundCloseToZero(t.m41()));
        row->pushNumber(roundCloseToZero(t.m42()));
        row->pushNumber(roundCloseToZero(t.m43()));
        row->pushNumber(roundCloseToZero(t.m44()));
        array->pushArray(row);
    }
    return array;
}

static String pointerAsString(const void* ptr)
{
    TextStream ts;
    ts << ptr;
    return ts.release();
}

PassRefPtr<JSONObject> GraphicsLayer::layerTreeAsJSON(LayerTreeFlags flags, RenderingContextMap& renderingContextMap) const
{
    RefPtr<JSONObject> json = adoptRef(new JSONObject);

    if (flags & LayerTreeIncludesDebugInfo) {
        json->setString("this", pointerAsString(this));
        json->setString("debugName", m_client->debugName(this));
    }

    if (m_position != FloatPoint())
        json->setArray("position", pointAsJSONArray(m_position));

    if (m_hasTransformOrigin && m_transformOrigin != FloatPoint3D(m_size.width() * 0.5f, m_size.height() * 0.5f, 0))
        json->setArray("transformOrigin", pointAsJSONArray(m_transformOrigin));

    if (m_size != IntSize())
        json->setArray("bounds", sizeAsJSONArray(m_size));

    if (m_opacity != 1)
        json->setNumber("opacity", m_opacity);

    if (m_blendMode != WebBlendModeNormal)
        json->setString("blendMode", compositeOperatorName(CompositeSourceOver, m_blendMode));

    if ((flags & LayerTreeIncludesScrollBlocksOn) && m_scrollBlocksOn) {
        RefPtr<JSONArray> scrollBlocksOnJSON = adoptRef(new JSONArray);
        if (m_scrollBlocksOn & WebScrollBlocksOnStartTouch)
            scrollBlocksOnJSON->pushString("StartTouch");
        if (m_scrollBlocksOn & WebScrollBlocksOnWheelEvent)
            scrollBlocksOnJSON->pushString("WheelEvent");
        if (m_scrollBlocksOn & WebScrollBlocksOnScrollEvent)
            scrollBlocksOnJSON->pushString("ScrollEvent");
        json->setArray("scrollBlocksOn", scrollBlocksOnJSON);
    }

    if (m_isRootForIsolatedGroup)
        json->setBoolean("isolate", m_isRootForIsolatedGroup);

    if (m_contentsOpaque)
        json->setBoolean("contentsOpaque", m_contentsOpaque);

    if (!m_shouldFlattenTransform)
        json->setBoolean("shouldFlattenTransform", m_shouldFlattenTransform);

    if (m_3dRenderingContext) {
        RenderingContextMap::const_iterator it = renderingContextMap.find(m_3dRenderingContext);
        int contextId = renderingContextMap.size() + 1;
        if (it == renderingContextMap.end())
            renderingContextMap.set(m_3dRenderingContext, contextId);
        else
            contextId = it->value;

        json->setNumber("3dRenderingContext", contextId);
    }

    if (m_drawsContent)
        json->setBoolean("drawsContent", m_drawsContent);

    if (!m_contentsVisible)
        json->setBoolean("contentsVisible", m_contentsVisible);

    if (!m_backfaceVisibility)
        json->setString("backfaceVisibility", m_backfaceVisibility ? "visible" : "hidden");

    if (flags & LayerTreeIncludesDebugInfo)
        json->setString("client", pointerAsString(m_client));

    if (m_backgroundColor.alpha())
        json->setString("backgroundColor", m_backgroundColor.nameForLayoutTreeAsText());

    if (!m_transform.isIdentity())
        json->setArray("transform", transformAsJSONArray(m_transform));

    if (m_replicaLayer)
        json->setObject("replicaLayer", m_replicaLayer->layerTreeAsJSON(flags, renderingContextMap));

    if (m_replicatedLayer)
        json->setString("replicatedLayer", flags & LayerTreeIncludesDebugInfo ? pointerAsString(m_replicatedLayer) : "");

    PaintInvalidationTrackingMap::iterator it = paintInvalidationTrackingMap().find(this);
    if (it != paintInvalidationTrackingMap().end()) {
        if (flags & LayerTreeIncludesPaintInvalidationRects) {
            Vector<FloatRect>& rects = it->value.invalidationRects;
            if (!rects.isEmpty()) {
                std::sort(rects.begin(), rects.end(), &compareFloatRects);
                RefPtr<JSONArray> rectsJSON = adoptRef(new JSONArray);
                for (auto& rect : rects) {
                    if (rect.isEmpty())
                        continue;
                    rectsJSON->pushArray(rectAsJSONArray(rect));
                }
                json->setArray("repaintRects", rectsJSON);
            }
        }

        if (RuntimeEnabledFeatures::slimmingPaintEnabled() && (flags & LayerTreeIncludesPaintInvalidationObjects)) {
            Vector<String>& clients = it->value.invalidationObjects;
            if (!clients.isEmpty()) {
                RefPtr<JSONArray> clientsJSON = adoptRef(new JSONArray);
                for (auto& clientString : clients)
                    clientsJSON->pushString(clientString);
                json->setArray("paintInvalidationClients", clientsJSON);
            }
        }
    }

    if ((flags & LayerTreeIncludesPaintingPhases) && m_paintingPhase) {
        RefPtr<JSONArray> paintingPhasesJSON = adoptRef(new JSONArray);
        if (m_paintingPhase & GraphicsLayerPaintBackground)
            paintingPhasesJSON->pushString("GraphicsLayerPaintBackground");
        if (m_paintingPhase & GraphicsLayerPaintForeground)
            paintingPhasesJSON->pushString("GraphicsLayerPaintForeground");
        if (m_paintingPhase & GraphicsLayerPaintMask)
            paintingPhasesJSON->pushString("GraphicsLayerPaintMask");
        if (m_paintingPhase & GraphicsLayerPaintChildClippingMask)
            paintingPhasesJSON->pushString("GraphicsLayerPaintChildClippingMask");
        if (m_paintingPhase & GraphicsLayerPaintOverflowContents)
            paintingPhasesJSON->pushString("GraphicsLayerPaintOverflowContents");
        if (m_paintingPhase & GraphicsLayerPaintCompositedScroll)
            paintingPhasesJSON->pushString("GraphicsLayerPaintCompositedScroll");
        json->setArray("paintingPhases", paintingPhasesJSON);
    }

    if (flags & LayerTreeIncludesClipAndScrollParents) {
        if (m_hasScrollParent)
            json->setBoolean("hasScrollParent", true);
        if (m_hasClipParent)
            json->setBoolean("hasClipParent", true);
    }

    if (flags & (LayerTreeIncludesDebugInfo | LayerTreeIncludesCompositingReasons)) {
        bool debug = flags & LayerTreeIncludesDebugInfo;
        RefPtr<JSONArray> compositingReasonsJSON = adoptRef(new JSONArray);
        for (size_t i = 0; i < kNumberOfCompositingReasons; ++i) {
            if (m_debugInfo.compositingReasons() & kCompositingReasonStringMap[i].reason)
                compositingReasonsJSON->pushString(debug ? kCompositingReasonStringMap[i].description : kCompositingReasonStringMap[i].shortName);
        }
        json->setArray("compositingReasons", compositingReasonsJSON);
    }

    if (m_children.size()) {
        RefPtr<JSONArray> childrenJSON = adoptRef(new JSONArray);
        for (size_t i = 0; i < m_children.size(); i++)
            childrenJSON->pushObject(m_children[i]->layerTreeAsJSON(flags, renderingContextMap));
        json->setArray("children", childrenJSON);
    }

    return json;
}

String GraphicsLayer::layerTreeAsText(LayerTreeFlags flags) const
{
    RenderingContextMap renderingContextMap;
    RefPtr<JSONObject> json = layerTreeAsJSON(flags, renderingContextMap);
    return json->toPrettyJSONString();
}

String GraphicsLayer::debugName(WebLayer* webLayer) const
{
    String name;
    if (!m_client)
        return name;

    String highlightDebugName;
    for (size_t i = 0; i < m_linkHighlights.size(); ++i) {
        if (webLayer == m_linkHighlights[i]->layer()) {
            highlightDebugName = "LinkHighlight[" + String::number(i) + "] for " + m_client->debugName(this);
            break;
        }
    }

    if (webLayer == m_contentsLayer) {
        name = "ContentsLayer for " + m_client->debugName(this);
    } else if (!highlightDebugName.isEmpty()) {
        name = highlightDebugName;
    } else if (webLayer == m_layer->layer()) {
        name = m_client->debugName(this);
    } else {
        ASSERT_NOT_REACHED();
    }
    return name;
}

void GraphicsLayer::setCompositingReasons(CompositingReasons reasons)
{
    m_debugInfo.setCompositingReasons(reasons);
}

void GraphicsLayer::setOwnerNodeId(int nodeId)
{
    m_debugInfo.setOwnerNodeId(nodeId);
}

void GraphicsLayer::setPosition(const FloatPoint& point)
{
    m_position = point;
    platformLayer()->setPosition(m_position);
}

void GraphicsLayer::setSize(const FloatSize& size)
{
    // We are receiving negative sizes here that cause assertions to fail in the compositor. Clamp them to 0 to
    // avoid those assertions.
    // FIXME: This should be an ASSERT instead, as negative sizes should not exist in WebCore.
    FloatSize clampedSize = size;
    if (clampedSize.width() < 0 || clampedSize.height() < 0)
        clampedSize = FloatSize();

    if (clampedSize == m_size)
        return;

    m_size = clampedSize;

    m_layer->layer()->setBounds(flooredIntSize(m_size));
    // Note that we don't resize m_contentsLayer. It's up the caller to do that.

#ifndef NDEBUG
    // The red debug fill needs to be invalidated if the layer resizes.
    if (m_displayItemList) {
        ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
        m_displayItemList->invalidate(displayItemClient());
    }
#endif
}

void GraphicsLayer::setTransform(const TransformationMatrix& transform)
{
    m_transform = transform;
    platformLayer()->setTransform(TransformationMatrix::toSkMatrix44(m_transform));
}

void GraphicsLayer::setTransformOrigin(const FloatPoint3D& transformOrigin)
{
    m_hasTransformOrigin = true;
    m_transformOrigin = transformOrigin;
    platformLayer()->setTransformOrigin(transformOrigin);
}

void GraphicsLayer::setShouldFlattenTransform(bool shouldFlatten)
{
    if (shouldFlatten == m_shouldFlattenTransform)
        return;

    m_shouldFlattenTransform = shouldFlatten;

    m_layer->layer()->setShouldFlattenTransform(shouldFlatten);
}

void GraphicsLayer::setRenderingContext(int context)
{
    if (m_3dRenderingContext == context)
        return;

    m_3dRenderingContext = context;
    m_layer->layer()->setRenderingContext(context);

    if (m_contentsLayer)
        m_contentsLayer->setRenderingContext(m_3dRenderingContext);
}

void GraphicsLayer::setMasksToBounds(bool masksToBounds)
{
    m_masksToBounds = masksToBounds;
    m_layer->layer()->setMasksToBounds(m_masksToBounds);
}

void GraphicsLayer::setDrawsContent(bool drawsContent)
{
    // Note carefully this early-exit is only correct because we also properly call
    // WebLayer::setDrawsContent whenever m_contentsLayer is set to a new layer in setupContentsLayer().
    if (drawsContent == m_drawsContent)
        return;

    m_drawsContent = drawsContent;
    updateLayerIsDrawable();
}

void GraphicsLayer::setContentsVisible(bool contentsVisible)
{
    // Note carefully this early-exit is only correct because we also properly call
    // WebLayer::setDrawsContent whenever m_contentsLayer is set to a new layer in setupContentsLayer().
    if (contentsVisible == m_contentsVisible)
        return;

    m_contentsVisible = contentsVisible;
    updateLayerIsDrawable();
}

void GraphicsLayer::setClipParent(WebLayer* parent)
{
    m_hasClipParent = !!parent;
    m_layer->layer()->setClipParent(parent);
}

void GraphicsLayer::setScrollParent(WebLayer* parent)
{
    m_hasScrollParent = !!parent;
    m_layer->layer()->setScrollParent(parent);
}

void GraphicsLayer::setBackgroundColor(const Color& color)
{
    if (color == m_backgroundColor)
        return;

    m_backgroundColor = color;
    m_layer->layer()->setBackgroundColor(m_backgroundColor.rgb());
}

void GraphicsLayer::setContentsOpaque(bool opaque)
{
    m_contentsOpaque = opaque;
    m_layer->layer()->setOpaque(m_contentsOpaque);
    clearContentsLayerIfUnregistered();
    if (m_contentsLayer)
        m_contentsLayer->setOpaque(opaque);
}

void GraphicsLayer::setMaskLayer(GraphicsLayer* maskLayer)
{
    if (maskLayer == m_maskLayer)
        return;

    m_maskLayer = maskLayer;
    WebLayer* maskWebLayer = m_maskLayer ? m_maskLayer->platformLayer() : 0;
    m_layer->layer()->setMaskLayer(maskWebLayer);
}

void GraphicsLayer::setContentsClippingMaskLayer(GraphicsLayer* contentsClippingMaskLayer)
{
    if (contentsClippingMaskLayer == m_contentsClippingMaskLayer)
        return;

    m_contentsClippingMaskLayer = contentsClippingMaskLayer;
    WebLayer* contentsLayer = contentsLayerIfRegistered();
    if (!contentsLayer)
        return;
    WebLayer* contentsClippingMaskWebLayer = m_contentsClippingMaskLayer ? m_contentsClippingMaskLayer->platformLayer() : 0;
    contentsLayer->setMaskLayer(contentsClippingMaskWebLayer);
    updateContentsRect();
}

void GraphicsLayer::setBackfaceVisibility(bool visible)
{
    m_backfaceVisibility = visible;
    m_layer->setDoubleSided(m_backfaceVisibility);
}

void GraphicsLayer::setOpacity(float opacity)
{
    float clampedOpacity = std::max(std::min(opacity, 1.0f), 0.0f);
    m_opacity = clampedOpacity;
    platformLayer()->setOpacity(opacity);
}

void GraphicsLayer::setBlendMode(WebBlendMode blendMode)
{
    if (m_blendMode == blendMode)
        return;
    m_blendMode = blendMode;
    platformLayer()->setBlendMode(blendMode);
}

void GraphicsLayer::setScrollBlocksOn(WebScrollBlocksOn scrollBlocksOn)
{
    if (m_scrollBlocksOn == scrollBlocksOn)
        return;
    m_scrollBlocksOn = scrollBlocksOn;
    platformLayer()->setScrollBlocksOn(scrollBlocksOn);
}

void GraphicsLayer::setIsRootForIsolatedGroup(bool isolated)
{
    if (m_isRootForIsolatedGroup == isolated)
        return;
    m_isRootForIsolatedGroup = isolated;
    platformLayer()->setIsRootForIsolatedGroup(isolated);
}

void GraphicsLayer::setContentsNeedsDisplay()
{
    if (WebLayer* contentsLayer = contentsLayerIfRegistered()) {
        contentsLayer->invalidate();
        if (isTrackingPaintInvalidations())
            trackPaintInvalidationRect(m_contentsRect);
    }
}

void GraphicsLayer::setNeedsDisplay()
{
    if (drawsContent()) {
        m_layer->layer()->invalidate();
        if (isTrackingPaintInvalidations())
            trackPaintInvalidationRect(FloatRect(FloatPoint(), m_size));
        for (size_t i = 0; i < m_linkHighlights.size(); ++i)
            m_linkHighlights[i]->invalidate();

        if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
            displayItemList()->invalidateAll();
            if (isTrackingPaintInvalidations())
                trackPaintInvalidationObject("##ALL##");
        }
    }
}

void GraphicsLayer::setNeedsDisplayInRect(const IntRect& rect, PaintInvalidationReason invalidationReason)
{
    if (drawsContent()) {
        m_layer->layer()->invalidateRect(rect);
        if (firstPaintInvalidationTrackingEnabled())
            m_debugInfo.appendAnnotatedInvalidateRect(rect, invalidationReason);
        if (isTrackingPaintInvalidations())
            trackPaintInvalidationRect(rect);
        for (size_t i = 0; i < m_linkHighlights.size(); ++i)
            m_linkHighlights[i]->invalidate();
    }
}

void GraphicsLayer::invalidateDisplayItemClient(const DisplayItemClientWrapper& displayItemClient)
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    displayItemList()->invalidate(displayItemClient.displayItemClient());
    if (isTrackingPaintInvalidations())
        trackPaintInvalidationObject(displayItemClient.debugName());
}

void GraphicsLayer::setContentsRect(const IntRect& rect)
{
    if (rect == m_contentsRect)
        return;

    m_contentsRect = rect;
    updateContentsRect();
}

void GraphicsLayer::setContentsToImage(Image* image)
{
    SkBitmap bitmap;
    if (image && image->bitmapForCurrentFrame(&bitmap)) {
        if (!m_imageLayer) {
            m_imageLayer = adoptPtr(Platform::current()->compositorSupport()->createImageLayer());
            registerContentsLayer(m_imageLayer->layer());
        }
        m_imageLayer->setImageBitmap(bitmap);
        m_imageLayer->layer()->setOpaque(image->currentFrameKnownToBeOpaque());
        updateContentsRect();
    } else {
        if (m_imageLayer) {
            unregisterContentsLayer(m_imageLayer->layer());
            m_imageLayer.clear();
        }
    }

    setContentsTo(m_imageLayer ? m_imageLayer->layer() : 0);
}

void GraphicsLayer::setContentsToNinePatch(Image* image, const IntRect& aperture)
{
    if (m_ninePatchLayer) {
        unregisterContentsLayer(m_ninePatchLayer->layer());
        m_ninePatchLayer.clear();
    }
    SkBitmap bitmap;
    if (image && image->bitmapForCurrentFrame(&bitmap)) {
        m_ninePatchLayer = adoptPtr(Platform::current()->compositorSupport()->createNinePatchLayer());
        int borderWidth = bitmap.width() - aperture.width();
        int borderHeight = bitmap.height() - aperture.height();
        WebRect border(aperture.x(), aperture.y(), borderWidth, borderHeight);

        m_ninePatchLayer->setBitmap(bitmap);
        m_ninePatchLayer->setAperture(aperture);
        m_ninePatchLayer->setBorder(border);

        m_ninePatchLayer->layer()->setOpaque(image->currentFrameKnownToBeOpaque());
        registerContentsLayer(m_ninePatchLayer->layer());
    }
    setContentsTo(m_ninePatchLayer ? m_ninePatchLayer->layer() : 0);
}

bool GraphicsLayer::addAnimation(PassOwnPtr<WebCompositorAnimation> popAnimation)
{
    OwnPtr<WebCompositorAnimation> animation(popAnimation);
    ASSERT(animation);
    platformLayer()->setAnimationDelegate(this);

    // Remove any existing animations with the same animation id and target property.
    platformLayer()->removeAnimation(animation->id(), animation->targetProperty());
    return platformLayer()->addAnimation(animation.leakPtr());
}

void GraphicsLayer::pauseAnimation(int animationId, double timeOffset)
{
    platformLayer()->pauseAnimation(animationId, timeOffset);
}

void GraphicsLayer::removeAnimation(int animationId)
{
    platformLayer()->removeAnimation(animationId);
}

WebLayer* GraphicsLayer::platformLayer() const
{
    return m_layer->layer();
}

void GraphicsLayer::setFilters(const FilterOperations& filters)
{
    SkiaImageFilterBuilder builder;
    OwnPtr<WebFilterOperations> webFilters = adoptPtr(Platform::current()->compositorSupport()->createFilterOperations());
    FilterOutsets outsets = filters.outsets();
    builder.setCropOffset(FloatSize(outsets.left(), outsets.top()));
    builder.buildFilterOperations(filters, webFilters.get());
    m_layer->layer()->setFilters(*webFilters);
}

void GraphicsLayer::setFilterQuality(SkFilterQuality filterQuality)
{
    if (m_imageLayer)
        m_imageLayer->setNearestNeighbor(filterQuality == kNone_SkFilterQuality);
}

void GraphicsLayer::setPaintingPhase(GraphicsLayerPaintingPhase phase)
{
    if (m_paintingPhase == phase)
        return;
    m_paintingPhase = phase;
    setNeedsDisplay();
}

void GraphicsLayer::addLinkHighlight(LinkHighlightClient* linkHighlight)
{
    ASSERT(linkHighlight && !m_linkHighlights.contains(linkHighlight));
    m_linkHighlights.append(linkHighlight);
    linkHighlight->layer()->setWebLayerClient(this);
    updateChildList();
}

void GraphicsLayer::removeLinkHighlight(LinkHighlightClient* linkHighlight)
{
    m_linkHighlights.remove(m_linkHighlights.find(linkHighlight));
    updateChildList();
}

void GraphicsLayer::setScrollableArea(ScrollableArea* scrollableArea, bool isViewport)
{
    if (m_scrollableArea == scrollableArea)
        return;

    m_scrollableArea = scrollableArea;

    // Viewport scrolling may involve pinch zoom and gets routed through
    // WebViewImpl explicitly rather than via GraphicsLayer::didScroll.
    if (isViewport)
        m_layer->layer()->setScrollClient(0);
    else
        m_layer->layer()->setScrollClient(this);
}

void GraphicsLayer::paint(GraphicsContext& context, const IntRect& clip)
{
    paintGraphicsLayerContents(context, clip);
}


void GraphicsLayer::notifyAnimationStarted(double monotonicTime, int group)
{
    if (m_client)
        m_client->notifyAnimationStarted(this, monotonicTime, group);
}

void GraphicsLayer::notifyAnimationFinished(double, int group)
{
    if (m_scrollableArea)
        m_scrollableArea->notifyCompositorAnimationFinished(group);
}

void GraphicsLayer::didScroll()
{
    if (m_scrollableArea) {
        DoublePoint newPosition = m_scrollableArea->minimumScrollPosition() + toDoubleSize(m_layer->layer()->scrollPositionDouble());

        // FrameView::setScrollPosition doesn't work for compositor commits (interacts poorly with programmatic scroll animations)
        // so we need to use the ScrollableArea version. The FrameView method should go away soon anyway.
        m_scrollableArea->ScrollableArea::setScrollPosition(newPosition, CompositorScroll);
    }
}

DisplayItemList* GraphicsLayer::displayItemList()
{
    if (!RuntimeEnabledFeatures::slimmingPaintEnabled())
        return 0;
    if (!m_displayItemList)
        m_displayItemList = DisplayItemList::create();
    return m_displayItemList.get();
}

} // namespace blink

#ifndef NDEBUG
void showGraphicsLayerTree(const blink::GraphicsLayer* layer)
{
    if (!layer) {
        fprintf(stderr, "Cannot showGraphicsLayerTree for (nil).\n");
        return;
    }

    String output = layer->layerTreeAsText(blink::LayerTreeIncludesDebugInfo);
    fprintf(stderr, "%s\n", output.utf8().data());
}
#endif
