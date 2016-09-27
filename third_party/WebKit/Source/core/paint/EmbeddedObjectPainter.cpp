// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/EmbeddedObjectPainter.h"

#include "core/frame/Settings.h"
#include "core/layout/LayoutEmbeddedObject.h"
#include "core/layout/LayoutTheme.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/fonts/Font.h"
#include "platform/fonts/FontSelector.h"
#include "platform/graphics/GraphicsContextStateSaver.h"
#include "platform/graphics/Path.h"
#include "platform/text/TextRun.h"

namespace blink {

static const float replacementTextRoundedRectHeight = 18;
static const float replacementTextRoundedRectLeftRightTextMargin = 6;
static const float replacementTextRoundedRectOpacity = 0.20f;
static const float replacementTextRoundedRectRadius = 5;
static const float replacementTextTextOpacity = 0.55f;

static Font replacementTextFont()
{
    FontDescription fontDescription;
    LayoutTheme::theme().systemFont(CSSValueWebkitSmallControl, fontDescription);
    fontDescription.setWeight(FontWeightBold);
    fontDescription.setComputedSize(fontDescription.specifiedSize());
    Font font(fontDescription);
    font.update(nullptr);
    return font;
}

void EmbeddedObjectPainter::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!m_layoutEmbeddedObject.showsUnavailablePluginIndicator())
        return;

    if (paintInfo.phase == PaintPhaseSelection)
        return;

    GraphicsContext* context = paintInfo.context;
    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, m_layoutEmbeddedObject, paintInfo.phase))
        return;

    FloatRect contentRect = m_layoutEmbeddedObject.contentBoxRect();
    contentRect.moveBy(roundedIntPoint(paintOffset));
    LayoutObjectDrawingRecorder drawingRecorder(*context, m_layoutEmbeddedObject, paintInfo.phase, contentRect);
    GraphicsContextStateSaver stateSaver(*context);
    context->clip(contentRect);

    Font font = replacementTextFont();
    // TODO(trchen): Speculative fix for crbug.com/481880
    // With last resort font, how could this ever be null?
    ASSERT(font.primaryFont());
    if (!font.primaryFont())
        return;
    TextRun textRun(m_layoutEmbeddedObject.unavailablePluginReplacementText());
    FloatSize textGeometry(font.width(textRun), font.fontMetrics().height());

    FloatRect backgroundRect(0, 0, textGeometry.width() + 2 * replacementTextRoundedRectLeftRightTextMargin, replacementTextRoundedRectHeight);
    backgroundRect.move(contentRect.center() - backgroundRect.center());
    Path roundedBackgroundRect;
    roundedBackgroundRect.addRoundedRect(backgroundRect, FloatSize(replacementTextRoundedRectRadius, replacementTextRoundedRectRadius));
    context->setFillColor(scaleAlpha(Color::white, replacementTextRoundedRectOpacity));
    context->fillPath(roundedBackgroundRect);

    FloatRect textRect(FloatPoint(), textGeometry);
    textRect.move(contentRect.center() - textRect.center());
    TextRunPaintInfo runInfo(textRun);
    runInfo.bounds = backgroundRect;
    context->setFillColor(scaleAlpha(Color::black, replacementTextTextOpacity));
    context->drawBidiText(font, runInfo, textRect.location() + FloatSize(0, font.fontMetrics().ascent()));
}

} // namespace blink
