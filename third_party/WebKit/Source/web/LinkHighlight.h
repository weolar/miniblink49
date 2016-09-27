/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LinkHighlight_h
#define LinkHighlight_h

#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/IntPoint.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/Path.h"
#include "public/platform/WebCompositorAnimationDelegate.h"
#include "public/platform/WebContentLayer.h"
#include "public/platform/WebContentLayerClient.h"
#include "public/platform/WebLayer.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class LayoutBoxModelObject;
class Node;
struct WebRect;
class WebViewImpl;

class LinkHighlight final : public WebContentLayerClient, public WebCompositorAnimationDelegate, LinkHighlightClient {
public:
    static PassOwnPtr<LinkHighlight> create(Node*, WebViewImpl*);
    ~LinkHighlight() override;

    WebContentLayer* contentLayer();
    WebLayer* clipLayer();
    void startHighlightAnimationIfNeeded();
    void updateGeometry();

    // WebContentLayerClient implementation.
    void paintContents(WebCanvas*, const WebRect& clipRect, WebContentLayerClient::PaintingControlSetting) override;
    void paintContents(WebDisplayItemList*, const WebRect& clipRect, WebContentLayerClient::PaintingControlSetting) override;

    // WebCompositorAnimationDelegate implementation.
    void notifyAnimationStarted(double monotonicTime, int group) override;
    void notifyAnimationFinished(double monotonicTime, int group) override;

    // LinkHighlightClient implementation.
    void invalidate() override;
    WebLayer* layer() override;
    void clearCurrentGraphicsLayer() override;

    GraphicsLayer* currentGraphicsLayerForTesting() const { return m_currentGraphicsLayer; }

private:
    LinkHighlight(Node*, WebViewImpl*);

    void releaseResources();
    void computeQuads(const Node&, WTF::Vector<FloatQuad>&) const;

    void attachLinkHighlightToCompositingLayer(const LayoutBoxModelObject* paintInvalidationContainer);
    void clearGraphicsLayerLinkHighlightPointer();
    // This function computes the highlight path, and returns true if it has changed
    // size since the last call to this function.
    bool computeHighlightLayerPathAndPosition(const LayoutBoxModelObject*);

    OwnPtr<WebContentLayer> m_contentLayer;
    OwnPtr<WebLayer> m_clipLayer;
    Path m_path;

    RefPtrWillBePersistent<Node> m_node;
    WebViewImpl* m_owningWebViewImpl;
    GraphicsLayer* m_currentGraphicsLayer;

    bool m_geometryNeedsUpdate;
    bool m_isAnimating;
    double m_startTime;
};

} // namespace blink

#endif
