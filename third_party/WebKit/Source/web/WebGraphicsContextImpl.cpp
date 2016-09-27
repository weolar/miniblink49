// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/WebGraphicsContextImpl.h"

#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DrawingRecorder.h"

namespace blink {

WebGraphicsContextImpl::WebGraphicsContextImpl(GraphicsContext& graphicsContext, const DisplayItemClientWrapper& client, DisplayItem::Type type)
    : m_graphicsContext(graphicsContext)
    , m_client(client)
    , m_type(type)
#ifndef NDEBUG
    , m_hasBegunDrawing(false)
#endif
{
}

WebGraphicsContextImpl::~WebGraphicsContextImpl()
{
}

WebCanvas* WebGraphicsContextImpl::beginDrawing(const WebFloatRect& bounds)
{
#ifndef NDEBUG
    ASSERT(!m_hasBegunDrawing);
    m_hasBegunDrawing = true;
#endif
    ASSERT(!DrawingRecorder::useCachedDrawingIfPossible(m_graphicsContext, m_client, m_type));
    m_drawingRecorder = adoptPtr(new DrawingRecorder(m_graphicsContext, m_client, m_type, bounds));
    WebCanvas* canvas = m_graphicsContext.canvas();
    ASSERT(canvas);
    return canvas;
}

void WebGraphicsContextImpl::endDrawing()
{
    ASSERT(m_drawingRecorder);
    m_drawingRecorder.clear();
}

} // namespace blink
