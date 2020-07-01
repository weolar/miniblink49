// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/InlineTextBoxPainter.h"

#include "core/dom/DocumentMarkerController.h"
#include "core/dom/RenderedDocumentMarker.h"
#include "core/editing/CompositionUnderline.h"
#include "core/editing/CompositionUnderlineRangeFilter.h"
#include "core/editing/Editor.h"
#include "core/editing/InputMethodController.h"
#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutTextCombine.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/TextPainter.h"
#include "platform/graphics/GraphicsContextStateSaver.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "wtf/Optional.h"

namespace blink {

typedef WTF::HashMap<const InlineTextBox*, TextBlobPtr> InlineTextBoxBlobCacheMap;
static InlineTextBoxBlobCacheMap* gTextBlobCache;

static const int misspellingLineThickness = 3;

void InlineTextBoxPainter::removeFromTextBlobCache(InlineTextBox& inlineTextBox)
{
    if (gTextBlobCache)
        gTextBlobCache->remove(&inlineTextBox);
}

static TextBlobPtr* addToTextBlobCache(InlineTextBox& inlineTextBox)
{
    if (!gTextBlobCache)
        gTextBlobCache = new InlineTextBoxBlobCacheMap;
    return &gTextBlobCache->add(&inlineTextBox, nullptr).storedValue->value;
}

static bool paintsMarkerHighlights(const LayoutObject& layoutObject)
{
    return layoutObject.node() && layoutObject.document().markers().hasMarkers(layoutObject.node());
}

void InlineTextBoxPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_inlineTextBox.isLineBreak() || !paintInfo.shouldPaintWithinRoot(&m_inlineTextBox.layoutObject()) || m_inlineTextBox.layoutObject().style()->visibility() != VISIBLE
        || m_inlineTextBox.truncation() == cFullTruncation || paintInfo.phase == PaintPhaseOutline || !m_inlineTextBox.len())
        return;

    ASSERT(paintInfo.phase != PaintPhaseSelfOutline && paintInfo.phase != PaintPhaseChildOutlines);

    LayoutRect logicalVisualOverflow = m_inlineTextBox.logicalOverflowRect();
    LayoutUnit logicalStart = logicalVisualOverflow.x() + (m_inlineTextBox.isHorizontal() ? paintOffset.x() : paintOffset.y());
    LayoutUnit logicalExtent = logicalVisualOverflow.width();

    LayoutUnit paintEnd = m_inlineTextBox.isHorizontal() ? paintInfo.rect.maxX() : paintInfo.rect.maxY();
    LayoutUnit paintStart = m_inlineTextBox.isHorizontal() ? paintInfo.rect.x() : paintInfo.rect.y();

    // We round the y-axis to ensure consistent line heights.
    LayoutPoint adjustedPaintOffset = LayoutPoint(paintOffset.x(), paintOffset.y().round());

    if (logicalStart >= paintEnd || logicalStart + logicalExtent <= paintStart)
        return;

    bool isPrinting = m_inlineTextBox.layoutObject().document().printing();

    // Determine whether or not we're selected.
    bool haveSelection = !isPrinting && paintInfo.phase != PaintPhaseTextClip && m_inlineTextBox.selectionState() != LayoutObject::SelectionNone;
    if (!haveSelection && paintInfo.phase == PaintPhaseSelection) {
        // When only painting the selection, don't bother to paint if there is none.
        return;
    }

    // Determine whether or not we have composition underlines to draw.
    bool containsComposition = m_inlineTextBox.layoutObject().node() && m_inlineTextBox.layoutObject().frame()->inputMethodController().compositionNode() == m_inlineTextBox.layoutObject().node();
    bool useCustomUnderlines = containsComposition && m_inlineTextBox.layoutObject().frame()->inputMethodController().compositionUsesCustomUnderlines();

    // The text clip phase already has a DrawingRecorder. Text clips are initiated only in BoxPainter::paintLayerExtended, which is already
    // within a DrawingRecorder.
    Optional<DrawingRecorder> drawingRecorder;
    if (RuntimeEnabledFeatures::slimmingPaintEnabled() && paintInfo.phase != PaintPhaseTextClip) {
        if (DrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_inlineTextBox, DisplayItem::paintPhaseToDrawingType(paintInfo.phase)))
            return;
        LayoutRect paintRect(logicalVisualOverflow);
        m_inlineTextBox.logicalRectToPhysicalRect(paintRect);
        if (paintInfo.phase != PaintPhaseSelection && (haveSelection || containsComposition || paintsMarkerHighlights(m_inlineTextBox.layoutObject())))
            paintRect.unite(m_inlineTextBox.localSelectionRect(m_inlineTextBox.start(), m_inlineTextBox.start() + m_inlineTextBox.len()));
        paintRect.moveBy(adjustedPaintOffset);
        drawingRecorder.emplace(*paintInfo.context, m_inlineTextBox, DisplayItem::paintPhaseToDrawingType(paintInfo.phase), paintRect);
    }

    if (m_inlineTextBox.truncation() != cNoTruncation) {
        if (m_inlineTextBox.layoutObject().containingBlock()->style()->isLeftToRightDirection() != m_inlineTextBox.isLeftToRightDirection()) {
            // Make the visible fragment of text hug the edge closest to the rest of the run by moving the origin
            // at which we start drawing text.
            // e.g. In the case of LTR text truncated in an RTL Context, the correct behavior is:
            // |Hello|CBA| -> |...He|CBA|
            // In order to draw the fragment "He" aligned to the right edge of it's box, we need to start drawing
            // farther to the right.
            // NOTE: WebKit's behavior differs from that of IE which appears to just overlay the ellipsis on top of the
            // truncated string i.e.  |Hello|CBA| -> |...lo|CBA|
            LayoutUnit widthOfVisibleText = m_inlineTextBox.layoutObject().width(m_inlineTextBox.start(), m_inlineTextBox.truncation(), m_inlineTextBox.textPos(), m_inlineTextBox.isLeftToRightDirection() ? LTR : RTL, m_inlineTextBox.isFirstLineStyle());
            LayoutUnit widthOfHiddenText = m_inlineTextBox.logicalWidth() - widthOfVisibleText;
            // FIXME: The hit testing logic also needs to take this translation into account.
            LayoutSize truncationOffset(m_inlineTextBox.isLeftToRightDirection() ? widthOfHiddenText : -widthOfHiddenText, 0);
            adjustedPaintOffset.move(m_inlineTextBox.isHorizontal() ? truncationOffset : truncationOffset.transposedSize());
        }
    }

    GraphicsContext* context = paintInfo.context;
    const ComputedStyle& styleToUse = m_inlineTextBox.layoutObject().styleRef(m_inlineTextBox.isFirstLineStyle());

    LayoutPoint boxOrigin(m_inlineTextBox.locationIncludingFlipping());
    boxOrigin.move(adjustedPaintOffset.x(), adjustedPaintOffset.y());
    LayoutRect boxRect(boxOrigin, LayoutSize(m_inlineTextBox.logicalWidth(), m_inlineTextBox.logicalHeight()));

    bool shouldRotate = false;
    LayoutTextCombine* combinedText = nullptr;
    if (!m_inlineTextBox.isHorizontal()) {
        if (styleToUse.hasTextCombine() && m_inlineTextBox.layoutObject().isCombineText()) {
            combinedText = &toLayoutTextCombine(m_inlineTextBox.layoutObject());
            if (!combinedText->isCombined())
                combinedText = nullptr;
        }
        if (combinedText) {
            combinedText->updateFont();
            boxRect.setWidth(combinedText->inlineWidthForLayout());
        } else {
            shouldRotate = true;
            context->concatCTM(TextPainter::rotation(boxRect, TextPainter::Clockwise));
        }
    }

    // Determine text colors.
    bool usesTextAsClip = paintInfo.phase == PaintPhaseTextClip;
    TextPainter::Style textStyle = TextPainter::textPaintingStyle(m_inlineTextBox.layoutObject(), styleToUse, usesTextAsClip, isPrinting);
    TextPainter::Style selectionStyle = TextPainter::selectionPaintingStyle(m_inlineTextBox.layoutObject(), haveSelection, usesTextAsClip, isPrinting, textStyle);
    bool paintSelectedTextOnly = (paintInfo.phase == PaintPhaseSelection);
    bool paintSelectedTextSeparately = !paintSelectedTextOnly && textStyle != selectionStyle;

    // Set our font.
    const Font& font = styleToUse.font();

    LayoutPoint textOrigin(boxOrigin.x(), boxOrigin.y() + font.fontMetrics().ascent());

    // 1. Paint backgrounds behind text if needed. Examples of such backgrounds include selection
    // and composition highlights.
    if (paintInfo.phase != PaintPhaseSelection && paintInfo.phase != PaintPhaseTextClip && !isPrinting) {
        if (containsComposition) {
            paintCompositionBackgrounds(context, boxOrigin, styleToUse, font, useCustomUnderlines);
        }

        paintDocumentMarkers(context, boxOrigin, styleToUse, font, true);

        if (haveSelection && !useCustomUnderlines) {
            if (combinedText)
                paintSelection<InlineTextBoxPainter::PaintOptions::CombinedText>(context, boxRect, styleToUse, font, selectionStyle.fillColor, combinedText);
            else
                paintSelection<InlineTextBoxPainter::PaintOptions::Normal>(context, boxRect, styleToUse, font, selectionStyle.fillColor);
        }
    }

    // 2. Now paint the foreground, including text and decorations like underline/overline (in quirks mode only).
    int length = m_inlineTextBox.len();
    StringView string = m_inlineTextBox.layoutObject().text().createView();
    ASSERT(m_inlineTextBox.start() + length <= string.length());
    if (static_cast<unsigned>(length) != string.length() || m_inlineTextBox.start())
        string.narrow(m_inlineTextBox.start(), length);
    int maximumLength = m_inlineTextBox.layoutObject().textLength() - m_inlineTextBox.start();

    StringBuilder charactersWithHyphen;
    TextRun textRun = m_inlineTextBox.constructTextRun(styleToUse, font, string, maximumLength, m_inlineTextBox.hasHyphen() ? &charactersWithHyphen : 0);
    if (m_inlineTextBox.hasHyphen())
        length = textRun.length();

//     if (textRun.codePath() == TextRun::ForceComplex)
//         return;

    int selectionStart = 0;
    int selectionEnd = 0;
    if (paintSelectedTextOnly || paintSelectedTextSeparately)
        m_inlineTextBox.selectionStartEnd(selectionStart, selectionEnd);

    bool respectHyphen = selectionEnd == static_cast<int>(m_inlineTextBox.len()) && m_inlineTextBox.hasHyphen();
    if (respectHyphen)
        selectionEnd = textRun.length();

    if (m_inlineTextBox.truncation() != cNoTruncation) {
        selectionStart = std::min<int>(selectionStart, m_inlineTextBox.truncation());
        selectionEnd = std::min<int>(selectionEnd, m_inlineTextBox.truncation());
        length = m_inlineTextBox.truncation();
    }

    TextPainter textPainter(context, font, textRun, textOrigin, boxRect, m_inlineTextBox.isHorizontal());
    TextEmphasisPosition emphasisMarkPosition;
    bool hasTextEmphasis = m_inlineTextBox.getEmphasisMarkPosition(styleToUse, emphasisMarkPosition);
    if (hasTextEmphasis)
        textPainter.setEmphasisMark(styleToUse.textEmphasisMarkString(), emphasisMarkPosition);
    if (combinedText)
        textPainter.setCombinedText(combinedText);

    if (!paintSelectedTextOnly) {
        // FIXME: Truncate right-to-left text correctly.
        int startOffset = 0;
        int endOffset = length;
        if (paintSelectedTextSeparately && selectionStart < selectionEnd) {
            startOffset = selectionEnd;
            endOffset = selectionStart;
        }

        // FIXME: This cache should probably ultimately be held somewhere else.
        // A hashmap is convenient to avoid a memory hit when the
        // RuntimeEnabledFeature is off.
        bool textBlobIsCacheable = RuntimeEnabledFeatures::textBlobEnabled() && startOffset == 0 && endOffset == length;
        TextBlobPtr* cachedTextBlob = 0;
        if (textBlobIsCacheable)
            cachedTextBlob = addToTextBlobCache(m_inlineTextBox);
        textPainter.paint(startOffset, endOffset, length, textStyle, cachedTextBlob);
    }

    if ((paintSelectedTextOnly || paintSelectedTextSeparately) && selectionStart < selectionEnd) {
        // paint only the text that is selected
        bool textBlobIsCacheable = RuntimeEnabledFeatures::textBlobEnabled() && selectionStart == 0 && selectionEnd == length;
        TextBlobPtr* cachedTextBlob = 0;
        if (textBlobIsCacheable)
            cachedTextBlob = addToTextBlobCache(m_inlineTextBox);
        textPainter.paint(selectionStart, selectionEnd, length, selectionStyle, cachedTextBlob);
    }

    // Paint decorations
    TextDecoration textDecorations = styleToUse.textDecorationsInEffect();
    if (textDecorations != TextDecorationNone && !paintSelectedTextOnly) {
        GraphicsContextStateSaver stateSaver(*context, false);
        TextPainter::updateGraphicsContext(context, textStyle, m_inlineTextBox.isHorizontal(), stateSaver);
        if (combinedText)
            context->concatCTM(TextPainter::rotation(boxRect, TextPainter::Clockwise));
        paintDecoration(context, boxOrigin, textDecorations);
        if (combinedText)
            context->concatCTM(TextPainter::rotation(boxRect, TextPainter::Counterclockwise));
    }

    if (paintInfo.phase == PaintPhaseForeground) {
        paintDocumentMarkers(context, boxOrigin, styleToUse, font, false);

        // Paint custom underlines for compositions.
        if (useCustomUnderlines) {
            const Vector<CompositionUnderline>& underlines = m_inlineTextBox.layoutObject().frame()->inputMethodController().customCompositionUnderlines();
            CompositionUnderlineRangeFilter filter(underlines, m_inlineTextBox.start(), m_inlineTextBox.end());
            for (CompositionUnderlineRangeFilter::ConstIterator it = filter.begin(); it != filter.end(); ++it) {
                if (it->color == Color::transparent)
                    continue;
                paintCompositionUnderline(context, boxOrigin, *it);
            }
        }
    }

    if (shouldRotate)
        context->concatCTM(TextPainter::rotation(boxRect, TextPainter::Counterclockwise));
}

unsigned InlineTextBoxPainter::underlinePaintStart(const CompositionUnderline& underline)
{
    return std::max(static_cast<unsigned>(m_inlineTextBox.start()), underline.startOffset);
}

unsigned InlineTextBoxPainter::underlinePaintEnd(const CompositionUnderline& underline)
{
    unsigned paintEnd = std::min(m_inlineTextBox.end() + 1, underline.endOffset); // end() points at the last char, not past it.
    if (m_inlineTextBox.truncation() != cNoTruncation)
        paintEnd = std::min(paintEnd, static_cast<unsigned>(m_inlineTextBox.start() + m_inlineTextBox.truncation()));
    return paintEnd;
}

void InlineTextBoxPainter::paintCompositionBackgrounds(GraphicsContext* pt, const LayoutPoint& boxOrigin, const ComputedStyle& style, const Font& font, bool useCustomUnderlines)
{
    if (useCustomUnderlines) {
        // Paint custom background highlights for compositions.
        const Vector<CompositionUnderline>& underlines = m_inlineTextBox.layoutObject().frame()->inputMethodController().customCompositionUnderlines();
        CompositionUnderlineRangeFilter filter(underlines, m_inlineTextBox.start(), m_inlineTextBox.end());
        for (CompositionUnderlineRangeFilter::ConstIterator it = filter.begin(); it != filter.end(); ++it) {
            if (it->backgroundColor == Color::transparent)
                continue;
            paintSingleCompositionBackgroundRun(pt, boxOrigin, style, font, it->backgroundColor, underlinePaintStart(*it), underlinePaintEnd(*it));
        }

    } else {
        paintSingleCompositionBackgroundRun(pt, boxOrigin, style, font, LayoutTheme::theme().platformDefaultCompositionBackgroundColor(),
            m_inlineTextBox.layoutObject().frame()->inputMethodController().compositionStart(),
            m_inlineTextBox.layoutObject().frame()->inputMethodController().compositionEnd());
    }
}

void InlineTextBoxPainter::paintSingleCompositionBackgroundRun(GraphicsContext* context, const LayoutPoint& boxOrigin, const ComputedStyle& style, const Font& font, Color backgroundColor, int startPos, int endPos)
{
    int sPos = std::max(startPos - static_cast<int>(m_inlineTextBox.start()), 0);
    int ePos = std::min(endPos - static_cast<int>(m_inlineTextBox.start()), static_cast<int>(m_inlineTextBox.len()));
    if (sPos >= ePos)
        return;

    int deltaY = m_inlineTextBox.layoutObject().style()->isFlippedLinesWritingMode() ? m_inlineTextBox.root().selectionBottom() - m_inlineTextBox.logicalBottom() : m_inlineTextBox.logicalTop() - m_inlineTextBox.root().selectionTop();
    int selHeight = m_inlineTextBox.root().selectionHeight();
    FloatPoint localOrigin(boxOrigin.x().toFloat(), boxOrigin.y().toFloat() - deltaY);
    context->drawHighlightForText(font, m_inlineTextBox.constructTextRun(style, font), localOrigin, selHeight, backgroundColor, sPos, ePos);
}

void InlineTextBoxPainter::paintDocumentMarkers(GraphicsContext* pt, const LayoutPoint& boxOrigin, const ComputedStyle& style, const Font& font, bool background)
{
    if (!m_inlineTextBox.layoutObject().node())
        return;

    DocumentMarkerVector markers = m_inlineTextBox.layoutObject().document().markers().markersFor(m_inlineTextBox.layoutObject().node());
    DocumentMarkerVector::const_iterator markerIt = markers.begin();

    // Give any document markers that touch this run a chance to draw before the text has been drawn.
    // Note end() points at the last char, not one past it like endOffset and ranges do.
    for ( ; markerIt != markers.end(); ++markerIt) {
        DocumentMarker* marker = *markerIt;

        // Paint either the background markers or the foreground markers, but not both
        switch (marker->type()) {
        case DocumentMarker::Grammar:
        case DocumentMarker::Spelling:
            if (background)
                continue;
            break;
        case DocumentMarker::TextMatch:
            if (!background)
                continue;
            break;
        default:
            continue;
        }

        if (marker->endOffset() <= m_inlineTextBox.start()) {
            // marker is completely before this run.  This might be a marker that sits before the
            // first run we draw, or markers that were within runs we skipped due to truncation.
            continue;
        }
        if (marker->startOffset() > m_inlineTextBox.end()) {
            // marker is completely after this run, bail.  A later run will paint it.
            break;
        }

        // marker intersects this run.  Paint it.
        switch (marker->type()) {
        case DocumentMarker::Spelling:
            m_inlineTextBox.paintDocumentMarker(pt, boxOrigin, marker, style, font, false);
            break;
        case DocumentMarker::Grammar:
            m_inlineTextBox.paintDocumentMarker(pt, boxOrigin, marker, style, font, true);
            break;
        case DocumentMarker::TextMatch:
            m_inlineTextBox.paintTextMatchMarker(pt, boxOrigin, marker, style, font);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
    }
}

static GraphicsContext::DocumentMarkerLineStyle lineStyleForMarkerType(DocumentMarker::MarkerType markerType)
{
    switch (markerType) {
    case DocumentMarker::Spelling:
        return GraphicsContext::DocumentMarkerSpellingLineStyle;
    case DocumentMarker::Grammar:
        return GraphicsContext::DocumentMarkerGrammarLineStyle;
    default:
        ASSERT_NOT_REACHED();
        return GraphicsContext::DocumentMarkerSpellingLineStyle;
    }
}

void InlineTextBoxPainter::paintDocumentMarker(GraphicsContext* pt, const LayoutPoint& boxOrigin, DocumentMarker* marker, const ComputedStyle& style, const Font& font, bool grammar)
{
    // Never print spelling/grammar markers (5327887)
    if (m_inlineTextBox.layoutObject().document().printing())
        return;

    if (m_inlineTextBox.truncation() == cFullTruncation)
        return;

    LayoutUnit start = 0; // start of line to draw, relative to tx
    LayoutUnit width = m_inlineTextBox.logicalWidth(); // how much line to draw

    // Determine whether we need to measure text
    bool markerSpansWholeBox = true;
    if (m_inlineTextBox.start() <= marker->startOffset())
        markerSpansWholeBox = false;
    if ((m_inlineTextBox.end() + 1) != marker->endOffset()) // end points at the last char, not past it
        markerSpansWholeBox = false;
    if (m_inlineTextBox.truncation() != cNoTruncation)
        markerSpansWholeBox = false;

    if (!markerSpansWholeBox || grammar) {
        int startPosition = std::max<int>(marker->startOffset() - m_inlineTextBox.start(), 0);
        int endPosition = std::min<int>(marker->endOffset() - static_cast<int>(m_inlineTextBox.start()), m_inlineTextBox.len());

        if (m_inlineTextBox.truncation() != cNoTruncation)
            endPosition = std::min<int>(endPosition, m_inlineTextBox.truncation());

        // Calculate start & width
        int deltaY = m_inlineTextBox.layoutObject().style()->isFlippedLinesWritingMode() ? m_inlineTextBox.root().selectionBottom() - m_inlineTextBox.logicalBottom() : m_inlineTextBox.logicalTop() - m_inlineTextBox.root().selectionTop();
        int selHeight = m_inlineTextBox.root().selectionHeight();
        LayoutPoint startPoint(boxOrigin.x(), boxOrigin.y() - deltaY);
        TextRun run = m_inlineTextBox.constructTextRun(style, font);

        // FIXME: Convert the document markers to float rects.
        IntRect markerRect = enclosingIntRect(font.selectionRectForText(run, FloatPoint(startPoint), selHeight, startPosition, endPosition));
        start = markerRect.x() - startPoint.x();
        width = markerRect.width();
    }

    // IMPORTANT: The misspelling underline is not considered when calculating the text bounds, so we have to
    // make sure to fit within those bounds.  This means the top pixel(s) of the underline will overlap the
    // bottom pixel(s) of the glyphs in smaller font sizes.  The alternatives are to increase the line spacing (bad!!)
    // or decrease the underline thickness.  The overlap is actually the most useful, and matches what AppKit does.
    // So, we generally place the underline at the bottom of the text, but in larger fonts that's not so good so
    // we pin to two pixels under the baseline.
    int lineThickness = misspellingLineThickness;
    int baseline = m_inlineTextBox.layoutObject().style(m_inlineTextBox.isFirstLineStyle())->fontMetrics().ascent();
    int descent = m_inlineTextBox.logicalHeight() - baseline;
    int underlineOffset;
    if (descent <= (lineThickness + 2)) {
        // Place the underline at the very bottom of the text in small/medium fonts.
        underlineOffset = m_inlineTextBox.logicalHeight() - lineThickness;
    } else {
        // In larger fonts, though, place the underline up near the baseline to prevent a big gap.
        underlineOffset = baseline + 2;
    }
    pt->drawLineForDocumentMarker(FloatPoint((boxOrigin.x() + start).toFloat(), (boxOrigin.y() + underlineOffset).toFloat()), width.toFloat(), lineStyleForMarkerType(marker->type()));
}

template <InlineTextBoxPainter::PaintOptions options>
void InlineTextBoxPainter::paintSelection(GraphicsContext* context, const LayoutRect& boxRect, const ComputedStyle& style, const Font& font, Color textColor, LayoutTextCombine* combinedText)
{
    // See if we have a selection to paint at all.
    int sPos, ePos;
    m_inlineTextBox.selectionStartEnd(sPos, ePos);
    if (sPos >= ePos)
        return;

    Color c = m_inlineTextBox.layoutObject().selectionBackgroundColor();
    if (!c.alpha())
        return;

    // If the text color ends up being the same as the selection background, invert the selection
    // background.
    if (textColor == c)
        c = Color(0xff - c.red(), 0xff - c.green(), 0xff - c.blue());

    // If the text is truncated, let the thing being painted in the truncation
    // draw its own highlight.
    int length = m_inlineTextBox.truncation() != cNoTruncation ? m_inlineTextBox.truncation() : m_inlineTextBox.len();
    StringView string = m_inlineTextBox.layoutObject().text().createView();

    if (string.length() != static_cast<unsigned>(length) || m_inlineTextBox.start())
        string.narrow(m_inlineTextBox.start(), length);

    StringBuilder charactersWithHyphen;
    bool respectHyphen = ePos == length && m_inlineTextBox.hasHyphen();
    TextRun textRun = m_inlineTextBox.constructTextRun(style, font, string, m_inlineTextBox.layoutObject().textLength() - m_inlineTextBox.start(), respectHyphen ? &charactersWithHyphen : 0);
    if (respectHyphen)
        ePos = textRun.length();

    GraphicsContextStateSaver stateSaver(*context);

    if (options == InlineTextBoxPainter::PaintOptions::CombinedText) {
        ASSERT(combinedText);
        // We can't use the height of m_inlineTextBox because LayoutTextCombine's inlineTextBox is horizontal within vertical flow
        LayoutRect clipRect(boxRect);
        combinedText->transformLayoutRect(clipRect);
        context->clip(FloatRect(clipRect));
        combinedText->transformToInlineCoordinates(*context, boxRect);
        context->drawHighlightForText(font, textRun, FloatPoint(boxRect.location()), boxRect.height(), c, sPos, ePos);
        return;
    }

    LayoutUnit selectionBottom = m_inlineTextBox.root().selectionBottom();
    LayoutUnit selectionTop = m_inlineTextBox.root().selectionTopAdjustedForPrecedingBlock();

    int deltaY = roundToInt(m_inlineTextBox.layoutObject().style()->isFlippedLinesWritingMode() ? selectionBottom - m_inlineTextBox.logicalBottom() : m_inlineTextBox.logicalTop() - selectionTop);
    int selHeight = std::max(0, roundToInt(selectionBottom - selectionTop));

    FloatPoint localOrigin(boxRect.x().toFloat(), (boxRect.y() - deltaY).toFloat());
    FloatRect clipRect(localOrigin, FloatSize(m_inlineTextBox.logicalWidth().toFloat(), selHeight));

    context->clip(clipRect);
    context->drawHighlightForText(font, textRun, localOrigin, selHeight, c, sPos, ePos);
}

static int computeUnderlineOffset(const TextUnderlinePosition underlinePosition, const FontMetrics& fontMetrics, const InlineTextBox* inlineTextBox, const float textDecorationThickness)
{
    // Compute the gap between the font and the underline. Use at least one
    // pixel gap, if underline is thick then use a bigger gap.
    int gap = 0;

    // Underline position of zero means draw underline on Baseline Position,
    // in Blink we need at least 1-pixel gap to adding following check.
    // Positive underline Position means underline should be drawn above baselin e
    // and negative value means drawing below baseline, negating the value as in Blink
    // downward Y-increases.

    if (fontMetrics.underlinePosition())
        gap = -fontMetrics.underlinePosition();
    else
        gap = std::max<int>(1, ceilf(textDecorationThickness / 2.f));

    // FIXME: We support only horizontal text for now.
    switch (underlinePosition) {
    case TextUnderlinePositionAuto:
        return fontMetrics.ascent() + gap; // Position underline near the alphabetic baseline.
    case TextUnderlinePositionUnder: {
        // Position underline relative to the under edge of the lowest element's content box.
        const LayoutUnit offset = inlineTextBox->root().maxLogicalTop() - inlineTextBox->logicalTop();
        if (offset > 0)
            return inlineTextBox->logicalHeight() + gap + offset;
        return inlineTextBox->logicalHeight() + gap;
    }
    }

    ASSERT_NOT_REACHED();
    return fontMetrics.ascent() + gap;
}

static bool shouldSetDecorationAntialias(TextDecorationStyle decorationStyle)
{
    return decorationStyle == TextDecorationStyleDotted || decorationStyle == TextDecorationStyleDashed;
}

static bool shouldSetDecorationAntialias(TextDecorationStyle underline, TextDecorationStyle overline, TextDecorationStyle linethrough)
{
    return shouldSetDecorationAntialias(underline) || shouldSetDecorationAntialias(overline) || shouldSetDecorationAntialias(linethrough);
}

static StrokeStyle textDecorationStyleToStrokeStyle(TextDecorationStyle decorationStyle)
{
    StrokeStyle strokeStyle = SolidStroke;
    switch (decorationStyle) {
    case TextDecorationStyleSolid:
        strokeStyle = SolidStroke;
        break;
    case TextDecorationStyleDouble:
        strokeStyle = DoubleStroke;
        break;
    case TextDecorationStyleDotted:
        strokeStyle = DottedStroke;
        break;
    case TextDecorationStyleDashed:
        strokeStyle = DashedStroke;
        break;
    case TextDecorationStyleWavy:
        strokeStyle = WavyStroke;
        break;
    }

    return strokeStyle;
}

static void adjustStepToDecorationLength(float& step, float& controlPointDistance, float length)
{
    ASSERT(step > 0);

    if (length <= 0)
        return;

    unsigned stepCount = static_cast<unsigned>(length / step);

    // Each Bezier curve starts at the same pixel that the previous one
    // ended. We need to subtract (stepCount - 1) pixels when calculating the
    // length covered to account for that.
    float uncoveredLength = length - (stepCount * step - (stepCount - 1));
    float adjustment = uncoveredLength / stepCount;
    step += adjustment;
    controlPointDistance += adjustment;
}

/*
 * Draw one cubic Bezier curve and repeat the same pattern long the the decoration's axis.
 * The start point (p1), controlPoint1, controlPoint2 and end point (p2) of the Bezier curve
 * form a diamond shape:
 *
 *                              step
 *                         |-----------|
 *
 *                   controlPoint1
 *                         +
 *
 *
 *                  . .
 *                .     .
 *              .         .
 * (x1, y1) p1 +           .            + p2 (x2, y2) - <--- Decoration's axis
 *                          .         .               |
 *                            .     .                 |
 *                              . .                   | controlPointDistance
 *                                                    |
 *                                                    |
 *                         +                          -
 *                   controlPoint2
 *
 *             |-----------|
 *                 step
 */
static void strokeWavyTextDecoration(GraphicsContext* context, FloatPoint p1, FloatPoint p2, float strokeThickness)
{
    context->adjustLineToPixelBoundaries(p1, p2, strokeThickness, context->strokeStyle());

    Path path;
    path.moveTo(p1);

    // Distance between decoration's axis and Bezier curve's control points.
    // The height of the curve is based on this distance. Use a minimum of 6 pixels distance since
    // the actual curve passes approximately at half of that distance, that is 3 pixels.
    // The minimum height of the curve is also approximately 3 pixels. Increases the curve's height
    // as strockThickness increases to make the curve looks better.
    float controlPointDistance = 3 * std::max<float>(2, strokeThickness);

    // Increment used to form the diamond shape between start point (p1), control
    // points and end point (p2) along the axis of the decoration. Makes the
    // curve wider as strockThickness increases to make the curve looks better.
    float step = 2 * std::max<float>(2, strokeThickness);

    bool isVerticalLine = (p1.x() == p2.x());

    if (isVerticalLine) {
        ASSERT(p1.x() == p2.x());

        float xAxis = p1.x();
        float y1;
        float y2;

        if (p1.y() < p2.y()) {
            y1 = p1.y();
            y2 = p2.y();
        } else {
            y1 = p2.y();
            y2 = p1.y();
        }

        adjustStepToDecorationLength(step, controlPointDistance, y2 - y1);
        FloatPoint controlPoint1(xAxis + controlPointDistance, 0);
        FloatPoint controlPoint2(xAxis - controlPointDistance, 0);

        for (float y = y1; y + 2 * step <= y2;) {
            controlPoint1.setY(y + step);
            controlPoint2.setY(y + step);
            y += 2 * step;
            path.addBezierCurveTo(controlPoint1, controlPoint2, FloatPoint(xAxis, y));
        }
    } else {
        ASSERT(p1.y() == p2.y());

        float yAxis = p1.y();
        float x1;
        float x2;

        if (p1.x() < p2.x()) {
            x1 = p1.x();
            x2 = p2.x();
        } else {
            x1 = p2.x();
            x2 = p1.x();
        }

        adjustStepToDecorationLength(step, controlPointDistance, x2 - x1);
        FloatPoint controlPoint1(0, yAxis + controlPointDistance);
        FloatPoint controlPoint2(0, yAxis - controlPointDistance);

        for (float x = x1; x + 2 * step <= x2;) {
            controlPoint1.setX(x + step);
            controlPoint2.setX(x + step);
            x += 2 * step;
            path.addBezierCurveTo(controlPoint1, controlPoint2, FloatPoint(x, yAxis));
        }
    }

    context->setShouldAntialias(true);
    context->strokePath(path);
}

static void paintAppliedDecoration(GraphicsContext* context, FloatPoint start, float width, float doubleOffset, int wavyOffsetFactor,
    LayoutObject::AppliedTextDecoration decoration, float thickness, bool antialiasDecoration, bool isPrinting)
{
    context->setStrokeStyle(textDecorationStyleToStrokeStyle(decoration.style));
    context->setStrokeColor(decoration.color);

    switch (decoration.style) {
    case TextDecorationStyleWavy:
        strokeWavyTextDecoration(context, start + FloatPoint(0, doubleOffset * wavyOffsetFactor), start + FloatPoint(width, doubleOffset * wavyOffsetFactor), thickness);
        break;
    case TextDecorationStyleDotted:
    case TextDecorationStyleDashed:
        context->setShouldAntialias(antialiasDecoration);
        // Fall through
    default:
        context->drawLineForText(FloatPoint(start), width, isPrinting);

        if (decoration.style == TextDecorationStyleDouble)
            context->drawLineForText(start + FloatPoint(0, doubleOffset), width, isPrinting);
    }
}

void InlineTextBoxPainter::paintDecoration(GraphicsContext* context, const LayoutPoint& boxOrigin, TextDecoration deco)
{
    GraphicsContextStateSaver stateSaver(*context);

    if (m_inlineTextBox.truncation() == cFullTruncation)
        return;

    LayoutPoint localOrigin(boxOrigin);

    LayoutUnit width = m_inlineTextBox.logicalWidth();
    if (m_inlineTextBox.truncation() != cNoTruncation) {
        width = m_inlineTextBox.layoutObject().width(m_inlineTextBox.start(), m_inlineTextBox.truncation(), m_inlineTextBox.textPos(), m_inlineTextBox.isLeftToRightDirection() ? LTR : RTL, m_inlineTextBox.isFirstLineStyle());
        if (!m_inlineTextBox.isLeftToRightDirection())
            localOrigin.move(m_inlineTextBox.logicalWidth() - width, 0);
    }

    // Get the text decoration colors.
    LayoutObject::AppliedTextDecoration underline, overline, linethrough;
    m_inlineTextBox.layoutObject().getTextDecorations(deco, underline, overline, linethrough, true);
    if (m_inlineTextBox.isFirstLineStyle())
        m_inlineTextBox.layoutObject().getTextDecorations(deco, underline, overline, linethrough, true, true);

    // Use a special function for underlines to get the positioning exactly right.
    bool isPrinting = m_inlineTextBox.layoutObject().document().printing();

    const ComputedStyle& styleToUse = m_inlineTextBox.layoutObject().styleRef(m_inlineTextBox.isFirstLineStyle());
    float baseline = styleToUse.fontMetrics().ascent();

    // Set the thick of the line to be 10% (or something else ?)of the computed font size and not less than 1px.
    // Using computedFontSize should take care of zoom as well.

    // Update Underline thickness, in case we have Faulty Font Metrics calculating underline thickness by old method.
    float textDecorationThickness = styleToUse.fontMetrics().underlineThickness();
    int fontHeightInt  = (int)(styleToUse.fontMetrics().floatHeight() + 0.5);
    if ((textDecorationThickness == 0.f) || (textDecorationThickness >= (fontHeightInt >> 1)))
        textDecorationThickness = std::max(1.f, styleToUse.computedFontSize() / 10.f);

    context->setStrokeThickness(textDecorationThickness);

    bool antialiasDecoration = shouldSetDecorationAntialias(overline.style, underline.style, linethrough.style)
        && BoxPainter::shouldAntialiasLines(context);

    // Offset between lines - always non-zero, so lines never cross each other.
    float doubleOffset = textDecorationThickness + 1.f;

    if (deco & TextDecorationUnderline) {
        const int underlineOffset = computeUnderlineOffset(styleToUse.textUnderlinePosition(), styleToUse.fontMetrics(), &m_inlineTextBox, textDecorationThickness);
        paintAppliedDecoration(context, FloatPoint(localOrigin) + FloatPoint(0, underlineOffset), width.toFloat(), doubleOffset, 1, underline, textDecorationThickness, antialiasDecoration, isPrinting);
    }
    if (deco & TextDecorationOverline) {
        paintAppliedDecoration(context, FloatPoint(localOrigin), width.toFloat(), -doubleOffset, 1, overline, textDecorationThickness, antialiasDecoration, isPrinting);
    }
    if (deco & TextDecorationLineThrough) {
        const float lineThroughOffset = 2 * baseline / 3;
        paintAppliedDecoration(context, FloatPoint(localOrigin) + FloatPoint(0, lineThroughOffset), width.toFloat(), doubleOffset, 0, linethrough, textDecorationThickness, antialiasDecoration, isPrinting);
    }
}

void InlineTextBoxPainter::paintCompositionUnderline(GraphicsContext* ctx, const LayoutPoint& boxOrigin, const CompositionUnderline& underline)
{
    if (m_inlineTextBox.truncation() == cFullTruncation)
        return;

    unsigned paintStart = underlinePaintStart(underline);
    unsigned paintEnd = underlinePaintEnd(underline);

    // start of line to draw, relative to paintOffset.
    float start = paintStart == static_cast<unsigned>(m_inlineTextBox.start()) ? 0 :
        m_inlineTextBox.layoutObject().width(m_inlineTextBox.start(), paintStart - m_inlineTextBox.start(), m_inlineTextBox.textPos(), m_inlineTextBox.isLeftToRightDirection() ? LTR : RTL, m_inlineTextBox.isFirstLineStyle());
    // how much line to draw
    float width = (paintStart == static_cast<unsigned>(m_inlineTextBox.start()) && paintEnd == static_cast<unsigned>(m_inlineTextBox.end()) + 1) ? m_inlineTextBox.logicalWidth().toFloat() :
        m_inlineTextBox.layoutObject().width(paintStart, paintEnd - paintStart, m_inlineTextBox.textPos() + start, m_inlineTextBox.isLeftToRightDirection() ? LTR : RTL, m_inlineTextBox.isFirstLineStyle());

    // Thick marked text underlines are 2px thick as long as there is room for the 2px line under the baseline.
    // All other marked text underlines are 1px thick.
    // If there's not enough space the underline will touch or overlap characters.
    int lineThickness = 1;
    int baseline = m_inlineTextBox.layoutObject().style(m_inlineTextBox.isFirstLineStyle())->fontMetrics().ascent();
    if (underline.thick && m_inlineTextBox.logicalHeight() - baseline >= 2)
        lineThickness = 2;

    // We need to have some space between underlines of subsequent clauses, because some input methods do not use different underline styles for those.
    // We make each line shorter, which has a harmless side effect of shortening the first and last clauses, too.
    start += 1;
    width -= 2;

    ctx->setStrokeColor(underline.color);
    ctx->setStrokeThickness(lineThickness);
    ctx->drawLineForText(FloatPoint(boxOrigin.x() + start, (boxOrigin.y() + m_inlineTextBox.logicalHeight() - lineThickness).toFloat()), width, m_inlineTextBox.layoutObject().document().printing());
}

void InlineTextBoxPainter::paintTextMatchMarker(GraphicsContext* pt, const LayoutPoint& boxOrigin, DocumentMarker* marker, const ComputedStyle& style, const Font& font)
{
    // Use same y positioning and height as for selection, so that when the selection and this highlight are on
    // the same word there are no pieces sticking out.
    int deltaY = m_inlineTextBox.layoutObject().style()->isFlippedLinesWritingMode() ? m_inlineTextBox.root().selectionBottom() - m_inlineTextBox.logicalBottom() : m_inlineTextBox.logicalTop() - m_inlineTextBox.root().selectionTop();
    int selHeight = m_inlineTextBox.root().selectionHeight();

    int sPos = std::max(marker->startOffset() - m_inlineTextBox.start(), (unsigned)0);
    int ePos = std::min(marker->endOffset() - m_inlineTextBox.start(), m_inlineTextBox.len());
    TextRun run = m_inlineTextBox.constructTextRun(style, font);

    // Optionally highlight the text
    if (m_inlineTextBox.layoutObject().frame()->editor().markedTextMatchesAreHighlighted()) {
        Color color = marker->activeMatch() ?
            LayoutTheme::theme().platformActiveTextSearchHighlightColor() :
            LayoutTheme::theme().platformInactiveTextSearchHighlightColor();
        GraphicsContextStateSaver stateSaver(*pt);
        pt->clip(FloatRect(boxOrigin.x().toFloat(), (boxOrigin.y() - deltaY).toFloat(), m_inlineTextBox.logicalWidth().toFloat(), selHeight));
        pt->drawHighlightForText(font, run, FloatPoint(boxOrigin.x().toFloat(), (boxOrigin.y() - deltaY).toFloat()), selHeight, color, sPos, ePos);
    }
}


} // namespace blink
