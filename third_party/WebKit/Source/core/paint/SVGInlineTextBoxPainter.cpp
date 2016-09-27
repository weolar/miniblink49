// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGInlineTextBoxPainter.h"

#include "core/dom/DocumentMarkerController.h"
#include "core/dom/RenderedDocumentMarker.h"
#include "core/editing/Editor.h"
#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/line/InlineFlowBox.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/layout/svg/line/SVGInlineTextBox.h"
#include "core/paint/InlineTextBoxPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/SVGPaintContext.h"
#include "core/style/ShadowList.h"
#include "platform/graphics/GraphicsContextStateSaver.h"

namespace blink {

static inline bool textShouldBePainted(LayoutSVGInlineText& textLayoutObject)
{
    // Font::pixelSize(), returns FontDescription::computedPixelSize(), which returns "int(x + 0.5)".
    // If the absolute font size on screen is below x=0.5, don't render anything.
    return textLayoutObject.scaledFont().fontDescription().computedPixelSize();
}

bool SVGInlineTextBoxPainter::shouldPaintSelection() const
{
    bool isPrinting = m_svgInlineTextBox.layoutObject().document().printing();
    return !isPrinting && m_svgInlineTextBox.selectionState() != LayoutObject::SelectionNone;
}

void SVGInlineTextBoxPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(paintInfo.shouldPaintWithinRoot(&m_svgInlineTextBox.layoutObject()));
    ASSERT(paintInfo.phase == PaintPhaseForeground || paintInfo.phase == PaintPhaseSelection);
    ASSERT(m_svgInlineTextBox.truncation() == cNoTruncation);

    if (m_svgInlineTextBox.layoutObject().style()->visibility() != VISIBLE)
        return;

    // We're explicitly not supporting composition & custom underlines and custom highlighters -- unlike InlineTextBox.
    // If we ever need that for SVG, it's very easy to refactor and reuse the code.

    if (paintInfo.phase == PaintPhaseSelection && !shouldPaintSelection())
        return;

    LayoutSVGInlineText& textLayoutObject = toLayoutSVGInlineText(m_svgInlineTextBox.layoutObject());
    if (!textShouldBePainted(textLayoutObject))
        return;

    DisplayItem::Type displayItemType = DisplayItem::paintPhaseToDrawingType(paintInfo.phase);
    if (!DrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_svgInlineTextBox, displayItemType)) {
        LayoutObject& parentLayoutObject = m_svgInlineTextBox.parent()->layoutObject();
        const ComputedStyle& style = parentLayoutObject.styleRef();

        DrawingRecorder recorder(*paintInfo.context, m_svgInlineTextBox, displayItemType, paintInfo.rect);
        InlineTextBoxPainter(m_svgInlineTextBox).paintDocumentMarkers(
            paintInfo.context, paintOffset, style,
            textLayoutObject.scaledFont(), true);

        if (!m_svgInlineTextBox.textFragments().isEmpty())
            paintTextFragments(paintInfo, parentLayoutObject);
    }
}

void SVGInlineTextBoxPainter::paintTextFragments(const PaintInfo& paintInfo, LayoutObject& parentLayoutObject)
{
    const ComputedStyle& style = parentLayoutObject.styleRef();
    const SVGComputedStyle& svgStyle = style.svgStyle();

    bool hasFill = svgStyle.hasFill();
    bool hasVisibleStroke = svgStyle.hasVisibleStroke();

    const ComputedStyle* selectionStyle = &style;
    bool shouldPaintSelection = this->shouldPaintSelection();
    if (shouldPaintSelection) {
        selectionStyle = parentLayoutObject.getCachedPseudoStyle(SELECTION);
        if (selectionStyle) {
            const SVGComputedStyle& svgSelectionStyle = selectionStyle->svgStyle();

            if (!hasFill)
                hasFill = svgSelectionStyle.hasFill();
            if (!hasVisibleStroke)
                hasVisibleStroke = svgSelectionStyle.hasVisibleStroke();
        } else {
            selectionStyle = &style;
        }
    }

    if (paintInfo.isRenderingClipPathAsMaskImage()) {
        hasFill = true;
        hasVisibleStroke = false;
    }

    AffineTransform fragmentTransform;
    unsigned textFragmentsSize = m_svgInlineTextBox.textFragments().size();
    for (unsigned i = 0; i < textFragmentsSize; ++i) {
        SVGTextFragment& fragment = m_svgInlineTextBox.textFragments().at(i);

        GraphicsContextStateSaver stateSaver(*paintInfo.context, false);
        fragment.buildFragmentTransform(fragmentTransform);
        if (!fragmentTransform.isIdentity()) {
            stateSaver.save();
            paintInfo.context->concatCTM(fragmentTransform);
        }

        // Spec: All text decorations except line-through should be drawn before the text is filled and stroked; thus, the text is rendered on top of these decorations.
        unsigned decorations = style.textDecorationsInEffect();
        if (decorations & TextDecorationUnderline)
            paintDecoration(paintInfo, TextDecorationUnderline, fragment);
        if (decorations & TextDecorationOverline)
            paintDecoration(paintInfo, TextDecorationOverline, fragment);

        for (int i = 0; i < 3; i++) {
            switch (svgStyle.paintOrderType(i)) {
            case PT_FILL:
                if (hasFill)
                    paintText(paintInfo, style, *selectionStyle, fragment, ApplyToFillMode, shouldPaintSelection);
                break;
            case PT_STROKE:
                if (hasVisibleStroke)
                    paintText(paintInfo, style, *selectionStyle, fragment, ApplyToStrokeMode, shouldPaintSelection);
                break;
            case PT_MARKERS:
                // Markers don't apply to text
                break;
            default:
                ASSERT_NOT_REACHED();
                break;
            }
        }

        // Spec: Line-through should be drawn after the text is filled and stroked; thus, the line-through is rendered on top of the text.
        if (decorations & TextDecorationLineThrough)
            paintDecoration(paintInfo, TextDecorationLineThrough, fragment);
    }
}

void SVGInlineTextBoxPainter::paintSelectionBackground(const PaintInfo& paintInfo)
{
    if (m_svgInlineTextBox.layoutObject().style()->visibility() != VISIBLE)
        return;

    ASSERT(!m_svgInlineTextBox.layoutObject().document().printing());

    if (paintInfo.phase == PaintPhaseSelection || !shouldPaintSelection())
        return;

    Color backgroundColor = m_svgInlineTextBox.layoutObject().selectionBackgroundColor();
    if (!backgroundColor.alpha())
        return;

    LayoutSVGInlineText& textLayoutObject = toLayoutSVGInlineText(m_svgInlineTextBox.layoutObject());
    if (!textShouldBePainted(textLayoutObject))
        return;

    const ComputedStyle& style = m_svgInlineTextBox.parent()->layoutObject().styleRef();

    int startPosition, endPosition;
    m_svgInlineTextBox.selectionStartEnd(startPosition, endPosition);

    int fragmentStartPosition = 0;
    int fragmentEndPosition = 0;
    AffineTransform fragmentTransform;
    unsigned textFragmentsSize = m_svgInlineTextBox.textFragments().size();
    for (unsigned i = 0; i < textFragmentsSize; ++i) {
        SVGTextFragment& fragment = m_svgInlineTextBox.textFragments().at(i);

        fragmentStartPosition = startPosition;
        fragmentEndPosition = endPosition;
        if (!m_svgInlineTextBox.mapStartEndPositionsIntoFragmentCoordinates(fragment, fragmentStartPosition, fragmentEndPosition))
            continue;

        GraphicsContextStateSaver stateSaver(*paintInfo.context);
        fragment.buildFragmentTransform(fragmentTransform);
        if (!fragmentTransform.isIdentity())
            paintInfo.context->concatCTM(fragmentTransform);

        paintInfo.context->setFillColor(backgroundColor);
        paintInfo.context->fillRect(m_svgInlineTextBox.selectionRectForTextFragment(fragment, fragmentStartPosition, fragmentEndPosition, style), backgroundColor);
    }
}

static inline LayoutObject* findLayoutObjectDefininingTextDecoration(InlineFlowBox* parentBox)
{
    // Lookup first layout object in parent hierarchy which has text-decoration set.
    LayoutObject* layoutObject = 0;
    while (parentBox) {
        layoutObject = &parentBox->layoutObject();

        if (layoutObject->style() && layoutObject->style()->textDecoration() != TextDecorationNone)
            break;

        parentBox = parentBox->parent();
    }

    ASSERT(layoutObject);
    return layoutObject;
}

// Offset from the baseline for |decoration|. Positive offsets are above the baseline.
static inline float baselineOffsetForDecoration(TextDecoration decoration, const FontMetrics& fontMetrics, float thickness)
{
    // FIXME: For SVG Fonts we need to use the attributes defined in the <font-face> if specified.
    // Compatible with Batik/Presto.
    if (decoration == TextDecorationUnderline)
        return -thickness * 1.5f;
    if (decoration == TextDecorationOverline)
        return fontMetrics.floatAscent() - thickness;
    if (decoration == TextDecorationLineThrough)
        return fontMetrics.floatAscent() * 3 / 8.0f;

    ASSERT_NOT_REACHED();
    return 0.0f;
}

static inline float thicknessForDecoration(TextDecoration, const Font& font)
{
    // FIXME: For SVG Fonts we need to use the attributes defined in the <font-face> if specified.
    // Compatible with Batik/Presto
    return font.fontDescription().computedSize() / 20.0f;
}

void SVGInlineTextBoxPainter::paintDecoration(const PaintInfo& paintInfo, TextDecoration decoration, const SVGTextFragment& fragment)
{
    if (m_svgInlineTextBox.layoutObject().style()->textDecorationsInEffect() == TextDecorationNone)
        return;

    if (fragment.width <= 0)
        return;

    // Find out which style defined the text-decoration, as its fill/stroke properties have to be used for drawing instead of ours.
    LayoutObject* decorationLayoutObject = findLayoutObjectDefininingTextDecoration(m_svgInlineTextBox.parent());
    const ComputedStyle& decorationStyle = decorationLayoutObject->styleRef();

    if (decorationStyle.visibility() == HIDDEN)
        return;

    float scalingFactor = 1;
    Font scaledFont;
    LayoutSVGInlineText::computeNewScaledFontForStyle(decorationLayoutObject, &decorationStyle, scalingFactor, scaledFont);
    ASSERT(scalingFactor);

    float thickness = thicknessForDecoration(decoration, scaledFont);
    if (thickness <= 0)
        return;

    float decorationOffset = baselineOffsetForDecoration(decoration, scaledFont.fontMetrics(), thickness);
    FloatPoint decorationOrigin(fragment.x, fragment.y - decorationOffset / scalingFactor);

    Path path;
    path.addRect(FloatRect(decorationOrigin, FloatSize(fragment.width, thickness / scalingFactor)));

    const SVGComputedStyle& svgDecorationStyle = decorationStyle.svgStyle();

    for (int i = 0; i < 3; i++) {
        switch (svgDecorationStyle.paintOrderType(i)) {
        case PT_FILL:
            if (svgDecorationStyle.hasFill()) {
                SkPaint fillPaint;
                if (!SVGPaintContext::paintForLayoutObject(paintInfo, decorationStyle, *decorationLayoutObject, ApplyToFillMode, fillPaint))
                    break;
                fillPaint.setAntiAlias(true);
                paintInfo.context->drawPath(path.skPath(), fillPaint);
            }
            break;
        case PT_STROKE:
            if (svgDecorationStyle.hasVisibleStroke()) {
                SkPaint strokePaint;
                if (!SVGPaintContext::paintForLayoutObject(paintInfo, decorationStyle, *decorationLayoutObject, ApplyToStrokeMode, strokePaint))
                    break;
                strokePaint.setAntiAlias(true);
                StrokeData strokeData;
                SVGLayoutSupport::applyStrokeStyleToStrokeData(strokeData, decorationStyle, *decorationLayoutObject);
                if (svgDecorationStyle.vectorEffect() == VE_NON_SCALING_STROKE)
                    strokeData.setThickness(strokeData.thickness() / scalingFactor);
                strokeData.setupPaint(&strokePaint);
                paintInfo.context->drawPath(path.skPath(), strokePaint);
            }
            break;
        case PT_MARKERS:
            break;
        default:
            ASSERT_NOT_REACHED();
        }
    }
}

void SVGInlineTextBoxPainter::paintTextWithShadows(const PaintInfo& paintInfo, const ComputedStyle& style,
    TextRun& textRun, const SVGTextFragment& fragment, int startPosition, int endPosition,
    LayoutSVGResourceMode resourceMode)
{
    LayoutSVGInlineText& textLayoutObject = toLayoutSVGInlineText(m_svgInlineTextBox.layoutObject());

    float scalingFactor = textLayoutObject.scalingFactor();
    ASSERT(scalingFactor);

    const Font& scaledFont = textLayoutObject.scaledFont();
    const ShadowList* shadowList = style.textShadow();
    GraphicsContext* context = paintInfo.context;

    // Text shadows are disabled when printing. http://crbug.com/258321
    bool hasShadow = shadowList && !context->printing();

    FloatPoint textOrigin(fragment.x, fragment.y);
    FloatSize textSize(fragment.width, fragment.height);
    AffineTransform paintServerTransform;
    const AffineTransform* additionalPaintServerTransform = 0;

    GraphicsContextStateSaver stateSaver(*context, false);
    if (scalingFactor != 1) {
        textOrigin.scale(scalingFactor, scalingFactor);
        textSize.scale(scalingFactor);
        stateSaver.save();
        context->scale(1 / scalingFactor, 1 / scalingFactor);
        // Adjust the paint-server coordinate space.
        paintServerTransform.scale(scalingFactor);
        additionalPaintServerTransform = &paintServerTransform;
    }

    SkPaint paint;
    if (!SVGPaintContext::paintForLayoutObject(paintInfo, style, m_svgInlineTextBox.parent()->layoutObject(), resourceMode, paint, additionalPaintServerTransform))
        return;
    paint.setAntiAlias(true);

    if (hasShadow) {
        OwnPtr<DrawLooperBuilder> drawLooperBuilder = shadowList->createDrawLooper(DrawLooperBuilder::ShadowRespectsAlpha, style.visitedDependentColor(CSSPropertyColor));
        RefPtr<SkDrawLooper> drawLooper = drawLooperBuilder->detachDrawLooper();
        paint.setLooper(drawLooper.get());
    }

    if (resourceMode == ApplyToStrokeMode) {
        StrokeData strokeData;
        SVGLayoutSupport::applyStrokeStyleToStrokeData(strokeData, style, m_svgInlineTextBox.parent()->layoutObject());
        if (style.svgStyle().vectorEffect() != VE_NON_SCALING_STROKE)
            strokeData.setThickness(strokeData.thickness() * scalingFactor);
        strokeData.setupPaint(&paint);
    }

    TextRunPaintInfo textRunPaintInfo(textRun);
    textRunPaintInfo.from = startPosition;
    textRunPaintInfo.to = endPosition;

    float baseline = scaledFont.fontMetrics().floatAscent();
    textRunPaintInfo.bounds = FloatRect(textOrigin.x(), textOrigin.y() - baseline,
        textSize.width(), textSize.height());

    context->drawText(scaledFont, textRunPaintInfo, textOrigin, paint);
}

void SVGInlineTextBoxPainter::paintText(const PaintInfo& paintInfo, const ComputedStyle& style,
    const ComputedStyle& selectionStyle, const SVGTextFragment& fragment,
    LayoutSVGResourceMode resourceMode, bool shouldPaintSelection)
{
    int startPosition = 0;
    int endPosition = 0;
    if (shouldPaintSelection) {
        m_svgInlineTextBox.selectionStartEnd(startPosition, endPosition);
        shouldPaintSelection = m_svgInlineTextBox.mapStartEndPositionsIntoFragmentCoordinates(fragment, startPosition, endPosition);
    }

    // Fast path if there is no selection, just draw the whole chunk part using the regular style
    TextRun textRun = m_svgInlineTextBox.constructTextRun(style, fragment);
    if (!shouldPaintSelection || startPosition >= endPosition) {
        paintTextWithShadows(paintInfo, style, textRun, fragment, 0, fragment.length, resourceMode);
        return;
    }

    // Eventually draw text using regular style until the start position of the selection
    bool paintSelectedTextOnly = paintInfo.phase == PaintPhaseSelection;
    if (startPosition > 0 && !paintSelectedTextOnly)
        paintTextWithShadows(paintInfo, style, textRun, fragment, 0, startPosition, resourceMode);

    // Draw text using selection style from the start to the end position of the selection
    if (style != selectionStyle) {
        StyleDifference diff;
        diff.setNeedsPaintInvalidationObject();
        SVGResourcesCache::clientStyleChanged(&m_svgInlineTextBox.parent()->layoutObject(), diff, selectionStyle);
    }

    paintTextWithShadows(paintInfo, selectionStyle, textRun, fragment, startPosition, endPosition, resourceMode);

    if (style != selectionStyle) {
        StyleDifference diff;
        diff.setNeedsPaintInvalidationObject();
        SVGResourcesCache::clientStyleChanged(&m_svgInlineTextBox.parent()->layoutObject(), diff, style);
    }

    // Eventually draw text using regular style from the end position of the selection to the end of the current chunk part
    if (endPosition < static_cast<int>(fragment.length) && !paintSelectedTextOnly)
        paintTextWithShadows(paintInfo, style, textRun, fragment, endPosition, fragment.length, resourceMode);
}

void SVGInlineTextBoxPainter::paintTextMatchMarker(GraphicsContext* context, const LayoutPoint&, DocumentMarker* marker, const ComputedStyle& style, const Font& font)
{
    // SVG is only interested in the TextMatch markers.
    if (marker->type() != DocumentMarker::TextMatch)
        return;

    LayoutSVGInlineText& textLayoutObject = toLayoutSVGInlineText(m_svgInlineTextBox.layoutObject());

    AffineTransform fragmentTransform;
    for (InlineTextBox* box = textLayoutObject.firstTextBox(); box; box = box->nextTextBox()) {
        if (!box->isSVGInlineTextBox())
            continue;

        SVGInlineTextBox* textBox = toSVGInlineTextBox(box);

        int markerStartPosition = std::max<int>(marker->startOffset() - textBox->start(), 0);
        int markerEndPosition = std::min<int>(marker->endOffset() - textBox->start(), textBox->len());

        if (markerStartPosition >= markerEndPosition)
            continue;

        const Vector<SVGTextFragment>& fragments = textBox->textFragments();
        unsigned textFragmentsSize = fragments.size();
        for (unsigned i = 0; i < textFragmentsSize; ++i) {
            const SVGTextFragment& fragment = fragments.at(i);

            int fragmentStartPosition = markerStartPosition;
            int fragmentEndPosition = markerEndPosition;
            if (!textBox->mapStartEndPositionsIntoFragmentCoordinates(fragment, fragmentStartPosition, fragmentEndPosition))
                continue;

            FloatRect fragmentRect = textBox->selectionRectForTextFragment(fragment, fragmentStartPosition, fragmentEndPosition, style);
            fragment.buildFragmentTransform(fragmentTransform);

            // Draw the marker highlight.
            if (m_svgInlineTextBox.layoutObject().frame()->editor().markedTextMatchesAreHighlighted()) {
                Color color = marker->activeMatch() ?
                    LayoutTheme::theme().platformActiveTextSearchHighlightColor() :
                    LayoutTheme::theme().platformInactiveTextSearchHighlightColor();
                GraphicsContextStateSaver stateSaver(*context);
                if (!fragmentTransform.isIdentity())
                    context->concatCTM(fragmentTransform);
                context->setFillColor(color);
                context->fillRect(fragmentRect, color);
            }
        }
    }
}

} // namespace blink
