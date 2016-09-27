// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGraphicsContextImpl_h
#define WebGraphicsContextImpl_h

#include "platform/graphics/paint/DisplayItem.h"
#include "platform/graphics/paint/DisplayItemClient.h"
#include "public/web/WebGraphicsContext.h"
#include "wtf/OwnPtr.h"

namespace blink {

class DrawingRecorder;
class GraphicsContext;

// Wraps a blink::GraphicsContext.
// Internal consumers can extract the underlying context (via WebGraphicsContextImpl).
// External consumers can make a single drawing using a WebCanvas.
class WebGraphicsContextImpl : public WebGraphicsContext {
public:
    WebGraphicsContextImpl(GraphicsContext&, const DisplayItemClientWrapper&, DisplayItem::Type);
    ~WebGraphicsContextImpl();
    GraphicsContext& graphicsContext() { return m_graphicsContext; }

    // blink::WebGraphicsContext
    WebCanvas* beginDrawing(const WebFloatRect& bounds) override;
    void endDrawing() override;

private:
    GraphicsContext& m_graphicsContext;
    DisplayItemClientWrapper m_client;
    DisplayItem::Type m_type;
    OwnPtr<DrawingRecorder> m_drawingRecorder;
#ifndef NDEBUG
    bool m_hasBegunDrawing;
#endif
};

// Source/web/ assumes that WebGraphicsContextImpl is the only implementation.
DEFINE_TYPE_CASTS(WebGraphicsContextImpl, WebGraphicsContext, webGraphicsContext, true, true);

} // namespace blink

#endif
