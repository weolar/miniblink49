/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "platform/graphics/ContentLayerDelegate.h"

#include "platform/EventTracer.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/TracedValue.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/transforms/AffineTransform.h"
#include "platform/transforms/TransformationMatrix.h"
#include "public/platform/WebDisplayItemList.h"
#include "public/platform/WebFloatRect.h"
#include "public/platform/WebRect.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"

namespace blink {

ContentLayerDelegate::ContentLayerDelegate(GraphicsContextPainter* painter)
    : m_painter(painter)
{
}

ContentLayerDelegate::~ContentLayerDelegate()
{
}

PassRefPtr<TracedValue> toTracedValue(const WebRect& clip)
{
    RefPtr<TracedValue> tracedValue = TracedValue::create();
    tracedValue->beginArray("clip_rect");
    tracedValue->pushInteger(clip.x);
    tracedValue->pushInteger(clip.y);
    tracedValue->pushInteger(clip.width);
    tracedValue->pushInteger(clip.height);
    tracedValue->endArray();
    return tracedValue;
}

void ContentLayerDelegate::paintContents(
    SkCanvas* canvas, const WebRect& clip,
    WebContentLayerClient::PaintingControlSetting paintingControl)
{
    TRACE_EVENT1("blink,benchmark", "ContentLayerDelegate::paintContents", "clip_rect", toTracedValue(clip));

    ASSERT(!RuntimeEnabledFeatures::slimmingPaintEnabled());

    GraphicsContext::DisabledMode disabledMode = GraphicsContext::NothingDisabled;
    if (paintingControl == WebContentLayerClient::DisplayListPaintingDisabled
        || paintingControl == WebContentLayerClient::DisplayListConstructionDisabled)
        disabledMode = GraphicsContext::FullyDisabled;
    OwnPtr<GraphicsContext> context = GraphicsContext::deprecatedCreateWithCanvas(canvas, disabledMode);

    m_painter->paint(*context, clip);
}

void ContentLayerDelegate::paintContents(
    WebDisplayItemList* webDisplayItemList, const WebRect& clip,
    WebContentLayerClient::PaintingControlSetting paintingControl)
{
    TRACE_EVENT1("blink,benchmark", "ContentLayerDelegate::paintContents", "clip_rect", toTracedValue(clip));

    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());

    DisplayItemList* displayItemList = m_painter->displayItemList();
    ASSERT(displayItemList);
    displayItemList->setDisplayItemConstructionIsDisabled(
        paintingControl == WebContentLayerClient::DisplayListConstructionDisabled);

    // We also disable caching when Painting or Construction are disabled. In both cases we would like
    // to compare assuming the full cost of recording, not the cost of re-using cached content.
    if (paintingControl != WebContentLayerClient::PaintDefaultBehavior)
        displayItemList->invalidateAll();

    GraphicsContext::DisabledMode disabledMode = GraphicsContext::NothingDisabled;
    if (paintingControl == WebContentLayerClient::DisplayListPaintingDisabled
        || paintingControl == WebContentLayerClient::DisplayListConstructionDisabled)
        disabledMode = GraphicsContext::FullyDisabled;
    GraphicsContext context(displayItemList, disabledMode);

    m_painter->paint(context, clip);

    displayItemList->commitNewDisplayItemsAndAppendToWebDisplayItemList(webDisplayItemList);
}

} // namespace blink
