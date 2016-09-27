/*
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
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
#include "core/layout/LayoutTreeAsText.h"

#include "core/HTMLNames.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/Document.h"
#include "core/dom/PseudoElement.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLElement.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutDetailsMarker.h"
#include "core/layout/LayoutFileUploadControl.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutListItem.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/layout/svg/LayoutSVGContainer.h"
#include "core/layout/svg/LayoutSVGGradientStop.h"
#include "core/layout/svg/LayoutSVGImage.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/LayoutSVGPath.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/LayoutSVGText.h"
#include "core/layout/svg/SVGLayoutTreeAsText.h"
#include "core/page/PrintContext.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/LayoutUnit.h"
#include "wtf/HexNumber.h"
#include "wtf/Vector.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

using namespace HTMLNames;

static void printBorderStyle(TextStream& ts, const EBorderStyle borderStyle)
{
    switch (borderStyle) {
    case BNONE:
        ts << "none";
        break;
    case BHIDDEN:
        ts << "hidden";
        break;
    case INSET:
        ts << "inset";
        break;
    case GROOVE:
        ts << "groove";
        break;
    case RIDGE:
        ts << "ridge";
        break;
    case OUTSET:
        ts << "outset";
        break;
    case DOTTED:
        ts << "dotted";
        break;
    case DASHED:
        ts << "dashed";
        break;
    case SOLID:
        ts << "solid";
        break;
    case DOUBLE:
        ts << "double";
        break;
    }

    ts << " ";
}

static String getTagName(Node* n)
{
    if (n->isDocumentNode())
        return "";
    if (n->nodeType() == Node::COMMENT_NODE)
        return "COMMENT";
    return n->nodeName();
}

static bool isEmptyOrUnstyledAppleStyleSpan(const Node* node)
{
    if (!isHTMLSpanElement(node))
        return false;

    const HTMLElement& elem = toHTMLElement(*node);
    if (elem.getAttribute(classAttr) != "Apple-style-span")
        return false;

    if (!elem.hasChildren())
        return true;

    const StylePropertySet* inlineStyleDecl = elem.inlineStyle();
    return (!inlineStyleDecl || inlineStyleDecl->isEmpty());
}

String quoteAndEscapeNonPrintables(const String& s)
{
    StringBuilder result;
    result.append('"');
    for (unsigned i = 0; i != s.length(); ++i) {
        UChar c = s[i];
        if (c == '\\') {
            result.append('\\');
            result.append('\\');
        } else if (c == '"') {
            result.append('\\');
            result.append('"');
        } else if (c == '\n' || c == noBreakSpaceCharacter) {
            result.append(' ');
        } else {
            if (c >= 0x20 && c < 0x7F) {
                result.append(c);
            } else {
                result.append('\\');
                result.append('x');
                result.append('{');
                appendUnsignedAsHex(c, result);
                result.append('}');
            }
        }
    }
    result.append('"');
    return result.toString();
}

TextStream& operator<<(TextStream& ts, const Color& c)
{
    return ts << c.nameForLayoutTreeAsText();
}

void LayoutTreeAsText::writeLayoutObject(TextStream& ts, const LayoutObject& o, LayoutAsTextBehavior behavior)
{
    ts << o.decoratedName();

    if (behavior & LayoutAsTextShowAddresses)
        ts << " " << static_cast<const void*>(&o);

    if (o.style() && o.style()->zIndex())
        ts << " zI: " << o.style()->zIndex();

    if (o.node()) {
        String tagName = getTagName(o.node());
        if (!tagName.isEmpty()) {
            ts << " {" << tagName << "}";
            // flag empty or unstyled AppleStyleSpan because we never
            // want to leave them in the DOM
            if (isEmptyOrUnstyledAppleStyleSpan(o.node()))
                ts << " *empty or unstyled AppleStyleSpan*";
        }
    }

    LayoutBlock* cb = o.containingBlock();
    bool adjustForTableCells = cb ? cb->isTableCell() : false;

    LayoutRect r;
    if (o.isText()) {
        // FIXME: Would be better to dump the bounding box x and y rather than the first run's x and y, but that would involve updating
        // many test results.
        const LayoutText& text = toLayoutText(o);
        IntRect linesBox = text.linesBoundingBox();
        r = LayoutRect(IntRect(text.firstRunX(), text.firstRunY(), linesBox.width(), linesBox.height()));
        if (adjustForTableCells && !text.firstTextBox())
            adjustForTableCells = false;
    } else if (o.isLayoutInline()) {
        // FIXME: Would be better not to just dump 0, 0 as the x and y here.
        const LayoutInline& inlineFlow = toLayoutInline(o);
        r = LayoutRect(IntRect(0, 0, inlineFlow.linesBoundingBox().width(), inlineFlow.linesBoundingBox().height()));
        adjustForTableCells = false;
    } else if (o.isTableCell()) {
        // FIXME: Deliberately dump the "inner" box of table cells, since that is what current results reflect.  We'd like
        // to clean up the results to dump both the outer box and the intrinsic padding so that both bits of information are
        // captured by the results.
        const LayoutTableCell& cell = toLayoutTableCell(o);
        r = LayoutRect(cell.location().x(), cell.location().y() + cell.intrinsicPaddingBefore(), cell.size().width(), cell.size().height() - cell.intrinsicPaddingBefore() - cell.intrinsicPaddingAfter());
    } else if (o.isBox()) {
        r = toLayoutBox(&o)->frameRect();
    }

    // FIXME: Temporary in order to ensure compatibility with existing layout test results.
    if (adjustForTableCells)
        r.move(0, -toLayoutTableCell(o.containingBlock())->intrinsicPaddingBefore());

    if (o.isLayoutView()) {
        r.setWidth(toLayoutView(o).viewWidth(IncludeScrollbars));
        r.setHeight(toLayoutView(o).viewHeight(IncludeScrollbars));
    }

    ts << " " << r;

    if (!(o.isText() && !o.isBR())) {
        if (o.isFileUploadControl())
            ts << " " << quoteAndEscapeNonPrintables(toLayoutFileUploadControl(&o)->fileTextValue());

        if (o.parent()) {
            Color color = o.resolveColor(CSSPropertyColor);
            if (o.parent()->resolveColor(CSSPropertyColor) != color)
                ts << " [color=" << color << "]";

            // Do not dump invalid or transparent backgrounds, since that is the default.
            Color backgroundColor = o.resolveColor(CSSPropertyBackgroundColor);
            if (o.parent()->resolveColor(CSSPropertyBackgroundColor) != backgroundColor
                && backgroundColor.rgb())
                ts << " [bgcolor=" << backgroundColor << "]";

            Color textFillColor = o.resolveColor(CSSPropertyWebkitTextFillColor);
            if (o.parent()->resolveColor(CSSPropertyWebkitTextFillColor) != textFillColor
                && textFillColor != color && textFillColor.rgb())
                ts << " [textFillColor=" << textFillColor << "]";

            Color textStrokeColor = o.resolveColor(CSSPropertyWebkitTextStrokeColor);
            if (o.parent()->resolveColor(CSSPropertyWebkitTextStrokeColor) != textStrokeColor
                && textStrokeColor != color && textStrokeColor.rgb())
                ts << " [textStrokeColor=" << textStrokeColor << "]";

            if (o.parent()->style()->textStrokeWidth() != o.style()->textStrokeWidth() && o.style()->textStrokeWidth() > 0)
                ts << " [textStrokeWidth=" << o.style()->textStrokeWidth() << "]";
        }

        if (!o.isBoxModelObject())
            return;

        const LayoutBoxModelObject& box = toLayoutBoxModelObject(o);
        if (box.borderTop() || box.borderRight() || box.borderBottom() || box.borderLeft()) {
            ts << " [border:";

            BorderValue prevBorder = o.style()->borderTop();
            if (!box.borderTop()) {
                ts << " none";
            } else {
                ts << " (" << box.borderTop() << "px ";
                printBorderStyle(ts, o.style()->borderTopStyle());
                ts << o.resolveColor(CSSPropertyBorderTopColor) << ")";
            }

            if (o.style()->borderRight() != prevBorder) {
                prevBorder = o.style()->borderRight();
                if (!box.borderRight()) {
                    ts << " none";
                } else {
                    ts << " (" << box.borderRight() << "px ";
                    printBorderStyle(ts, o.style()->borderRightStyle());
                    ts << o.resolveColor(CSSPropertyBorderRightColor) << ")";
                }
            }

            if (o.style()->borderBottom() != prevBorder) {
                prevBorder = box.style()->borderBottom();
                if (!box.borderBottom()) {
                    ts << " none";
                } else {
                    ts << " (" << box.borderBottom() << "px ";
                    printBorderStyle(ts, o.style()->borderBottomStyle());
                    ts << o.resolveColor(CSSPropertyBorderBottomColor) << ")";
                }
            }

            if (o.style()->borderLeft() != prevBorder) {
                prevBorder = o.style()->borderLeft();
                if (!box.borderLeft()) {
                    ts << " none";
                } else {
                    ts << " (" << box.borderLeft() << "px ";
                    printBorderStyle(ts, o.style()->borderLeftStyle());
                    ts << o.resolveColor(CSSPropertyBorderLeftColor) << ")";
                }
            }

            ts << "]";
        }
    }

    if (o.isTableCell()) {
        const LayoutTableCell& c = toLayoutTableCell(o);
        ts << " [r=" << c.rowIndex() << " c=" << c.col() << " rs=" << c.rowSpan() << " cs=" << c.colSpan() << "]";
    }

    if (o.isDetailsMarker()) {
        ts << ": ";
        switch (toLayoutDetailsMarker(&o)->orientation()) {
        case LayoutDetailsMarker::Left:
            ts << "left";
            break;
        case LayoutDetailsMarker::Right:
            ts << "right";
            break;
        case LayoutDetailsMarker::Up:
            ts << "up";
            break;
        case LayoutDetailsMarker::Down:
            ts << "down";
            break;
        }
    }

    if (o.isListMarker()) {
        String text = toLayoutListMarker(o).text();
        if (!text.isEmpty()) {
            if (text.length() != 1) {
                text = quoteAndEscapeNonPrintables(text);
            } else {
                switch (text[0]) {
                case bulletCharacter:
                    text = "bullet";
                    break;
                case blackSquareCharacter:
                    text = "black square";
                    break;
                case whiteBulletCharacter:
                    text = "white bullet";
                    break;
                default:
                    text = quoteAndEscapeNonPrintables(text);
                }
            }
            ts << ": " << text;
        }
    }

    if (behavior & LayoutAsTextShowIDAndClass) {
        Node* node = o.node();
        if (node && node->isElementNode()) {
            Element& element = toElement(*node);
            if (element.hasID())
                ts << " id=\"" + element.getIdAttribute() + "\"";

            if (element.hasClass()) {
                ts << " class=\"";
                for (size_t i = 0; i < element.classNames().size(); ++i) {
                    if (i > 0)
                        ts << " ";
                    ts << element.classNames()[i];
                }
                ts << "\"";
            }
        }
    }

    if (behavior & LayoutAsTextShowLayoutState) {
        bool needsLayout = o.selfNeedsLayout() || o.needsPositionedMovementLayout() || o.posChildNeedsLayout() || o.normalChildNeedsLayout();
        if (needsLayout)
            ts << " (needs layout:";

        bool havePrevious = false;
        if (o.selfNeedsLayout()) {
            ts << " self";
            havePrevious = true;
        }

        if (o.needsPositionedMovementLayout()) {
            if (havePrevious)
                ts << ",";
            havePrevious = true;
            ts << " positioned movement";
        }

        if (o.normalChildNeedsLayout()) {
            if (havePrevious)
                ts << ",";
            havePrevious = true;
            ts << " child";
        }

        if (o.posChildNeedsLayout()) {
            if (havePrevious)
                ts << ",";
            ts << " positioned child";
        }

        if (needsLayout)
            ts << ")";
    }
}

static void writeInlineBox(TextStream& ts, const InlineBox& box, int indent)
{
    writeIndent(ts, indent);
    ts << "+ ";
    ts << box.boxName() << " {" << box.layoutObject().debugName() << "}"
        << " pos=(" << box.x() << "," << box.y() << ")"
        << " size=(" << box.width() << "," << box.height() << ")"
        << " baseline=" << box.baselinePosition(AlphabeticBaseline)
        << "/" << box.baselinePosition(IdeographicBaseline);
}

static void writeInlineTextBox(TextStream& ts, const InlineTextBox& textBox, int indent)
{
    writeInlineBox(ts, textBox, indent);
    String value = textBox.text();
    value.replaceWithLiteral('\\', "\\\\");
    value.replaceWithLiteral('\n', "\\n");
    value.replaceWithLiteral('"', "\\\"");
    ts << " range=(" << textBox.start() << "," << (textBox.start() + textBox.len()) << ")"
        << " \"" << value << "\"";
}

static void writeInlineFlowBox(TextStream& ts, const InlineFlowBox& rootBox, int indent)
{
    writeInlineBox(ts, rootBox, indent);
    ts << "\n";
    for (const InlineBox* box = rootBox.firstChild(); box; box = box->nextOnLine()) {
        if (box->isInlineFlowBox()) {
            writeInlineFlowBox(ts, static_cast<const InlineFlowBox&>(*box), indent + 1);
            continue;
        }
        if (box->isInlineTextBox())
            writeInlineTextBox(ts, static_cast<const InlineTextBox&>(*box), indent + 1);
        else
            writeInlineBox(ts, *box, indent + 1);
        ts << "\n";
    }
}

void LayoutTreeAsText::writeLineBoxTree(TextStream& ts, const LayoutBlockFlow& o, int indent)
{
    for (const InlineFlowBox* rootBox = o.firstLineBox(); rootBox; rootBox = rootBox->nextLineBox()) {
        writeInlineFlowBox(ts, *rootBox, indent);
    }
}

static void writeTextRun(TextStream& ts, const LayoutText& o, const InlineTextBox& run)
{
    // FIXME: For now use an "enclosingIntRect" model for x, y and logicalWidth, although this makes it harder
    // to detect any changes caused by the conversion to floating point. :(
    int x = run.x();
    int y = run.y();
    int logicalWidth = (run.left() + run.logicalWidth()).ceil() - x;

    // FIXME: Table cell adjustment is temporary until results can be updated.
    if (o.containingBlock()->isTableCell())
        y -= toLayoutTableCell(o.containingBlock())->intrinsicPaddingBefore();

    ts << "text run at (" << x << "," << y << ") width " << logicalWidth;
    if (!run.isLeftToRightDirection() || run.dirOverride()) {
        ts << (!run.isLeftToRightDirection() ? " RTL" : " LTR");
        if (run.dirOverride())
            ts << " override";
    }
    ts << ": "
        << quoteAndEscapeNonPrintables(String(o.text()).substring(run.start(), run.len()));
    if (run.hasHyphen())
        ts << " + hyphen string " << quoteAndEscapeNonPrintables(o.style()->hyphenString());
    ts << "\n";
}

void write(TextStream& ts, const LayoutObject& o, int indent, LayoutAsTextBehavior behavior)
{
    if (o.isSVGShape()) {
        write(ts, toLayoutSVGShape(o), indent);
        return;
    }
    if (o.isSVGGradientStop()) {
        writeSVGGradientStop(ts, toLayoutSVGGradientStop(o), indent);
        return;
    }
    if (o.isSVGResourceContainer()) {
        writeSVGResourceContainer(ts, o, indent);
        return;
    }
    if (o.isSVGContainer()) {
        writeSVGContainer(ts, o, indent);
        return;
    }
    if (o.isSVGRoot()) {
        write(ts, toLayoutSVGRoot(o), indent);
        return;
    }
    if (o.isSVGText()) {
        writeSVGText(ts, toLayoutSVGText(o), indent);
        return;
    }
    if (o.isSVGInlineText()) {
        writeSVGInlineText(ts, toLayoutSVGInlineText(o), indent);
        return;
    }
    if (o.isSVGImage()) {
        writeSVGImage(ts, toLayoutSVGImage(o), indent);
        return;
    }

    writeIndent(ts, indent);

    LayoutTreeAsText::writeLayoutObject(ts, o, behavior);
    ts << "\n";

    if ((behavior & LayoutAsTextShowLineTrees) && o.isLayoutBlockFlow()) {
        LayoutTreeAsText::writeLineBoxTree(ts, toLayoutBlockFlow(o), indent + 1);
    }

    if (o.isText() && !o.isBR()) {
        const LayoutText& text = toLayoutText(o);
        for (InlineTextBox* box = text.firstTextBox(); box; box = box->nextTextBox()) {
            writeIndent(ts, indent + 1);
            writeTextRun(ts, text, *box);
        }
    }

    for (LayoutObject* child = o.slowFirstChild(); child; child = child->nextSibling()) {
        if (child->hasLayer())
            continue;
        write(ts, *child, indent + 1, behavior);
    }

    if (o.isLayoutPart()) {
        Widget* widget = toLayoutPart(o).widget();
        if (widget && widget->isFrameView()) {
            FrameView* view = toFrameView(widget);
            LayoutView* root = view->layoutView();
            if (root) {
                root->document().updateLayout();
                DeprecatedPaintLayer* layer = root->layer();
                if (layer)
                    LayoutTreeAsText::writeLayers(ts, layer, layer, layer->rect(), indent + 1, behavior);
            }
        }
    }
}

enum LayerPaintPhase {
    LayerPaintPhaseAll = 0,
    LayerPaintPhaseBackground = -1,
    LayerPaintPhaseForeground = 1
};

static void write(TextStream& ts, DeprecatedPaintLayer& layer,
    const LayoutRect& layerBounds, const LayoutRect& backgroundClipRect, const LayoutRect& clipRect, const LayoutRect& outlineClipRect,
    LayerPaintPhase paintPhase = LayerPaintPhaseAll, int indent = 0, LayoutAsTextBehavior behavior = LayoutAsTextBehaviorNormal)
{
    IntRect adjustedLayoutBounds = pixelSnappedIntRect(layerBounds);
    IntRect adjustedLayoutBoundsWithScrollbars = adjustedLayoutBounds;
    IntRect adjustedBackgroundClipRect = pixelSnappedIntRect(backgroundClipRect);
    IntRect adjustedClipRect = pixelSnappedIntRect(clipRect);
    IntRect adjustedOutlineClipRect = pixelSnappedIntRect(outlineClipRect);

    Settings* settings = layer.layoutObject()->document().settings();
    bool reportFrameScrollInfo = layer.layoutObject()->isLayoutView() && settings && !settings->rootLayerScrolls();

    if (reportFrameScrollInfo) {
        LayoutView* layoutView = toLayoutView(layer.layoutObject());

        adjustedLayoutBoundsWithScrollbars.setWidth(layoutView->viewWidth(IncludeScrollbars));
        adjustedLayoutBoundsWithScrollbars.setHeight(layoutView->viewHeight(IncludeScrollbars));
    }

    writeIndent(ts, indent);

    if (layer.layoutObject()->style()->visibility() == HIDDEN)
        ts << "hidden ";

    ts << "layer ";

    if (behavior & LayoutAsTextShowAddresses)
        ts << static_cast<const void*>(&layer) << " ";

    ts << adjustedLayoutBoundsWithScrollbars;

    if (!adjustedLayoutBounds.isEmpty()) {
        if (!adjustedBackgroundClipRect.contains(adjustedLayoutBounds))
            ts << " backgroundClip " << adjustedBackgroundClipRect;
        if (!adjustedClipRect.contains(adjustedLayoutBoundsWithScrollbars))
            ts << " clip " << adjustedClipRect;
        if (!adjustedOutlineClipRect.contains(adjustedLayoutBounds))
            ts << " outlineClip " << adjustedOutlineClipRect;
    }
    if (layer.isTransparent())
        ts << " transparent";

    if (layer.layoutObject()->hasOverflowClip() || reportFrameScrollInfo) {
        ScrollableArea* scrollableArea;
        if (reportFrameScrollInfo)
            scrollableArea = toLayoutView(layer.layoutObject())->frameView();
        else
            scrollableArea = layer.scrollableArea();

        DoublePoint adjustedScrollOffset = scrollableArea->scrollPositionDouble() + toDoubleSize(scrollableArea->scrollOrigin());
        if (adjustedScrollOffset.x())
            ts << " scrollX " << adjustedScrollOffset.x();
        if (adjustedScrollOffset.y())
            ts << " scrollY " << adjustedScrollOffset.y();
        if (layer.layoutBox() && layer.layoutBox()->pixelSnappedClientWidth() != layer.layoutBox()->pixelSnappedScrollWidth())
            ts << " scrollWidth " << layer.layoutBox()->pixelSnappedScrollWidth();
        if (layer.layoutBox() && layer.layoutBox()->pixelSnappedClientHeight() != layer.layoutBox()->pixelSnappedScrollHeight())
            ts << " scrollHeight " << layer.layoutBox()->pixelSnappedScrollHeight();
    }

    if (paintPhase == LayerPaintPhaseBackground)
        ts << " layerType: background only";
    else if (paintPhase == LayerPaintPhaseForeground)
        ts << " layerType: foreground only";

    if (layer.layoutObject()->style()->hasBlendMode())
        ts << " blendMode: " << compositeOperatorName(CompositeSourceOver, layer.layoutObject()->style()->blendMode());

    if (behavior & LayoutAsTextShowCompositedLayers) {
        if (layer.hasCompositedDeprecatedPaintLayerMapping()) {
            ts << " (composited, bounds="
                << layer.compositedDeprecatedPaintLayerMapping()->compositedBounds()
                << ", drawsContent="
                << layer.compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()->drawsContent()
                << (layer.shouldIsolateCompositedDescendants() ? ", isolatesCompositedBlending" : "")
                << ")";
        }
    }

    ts << "\n";

    if (paintPhase != LayerPaintPhaseBackground)
        write(ts, *layer.layoutObject(), indent + 1, behavior);
}

static Vector<DeprecatedPaintLayerStackingNode*> normalFlowListFor(DeprecatedPaintLayerStackingNode* node)
{
    DeprecatedPaintLayerStackingNodeIterator it(*node, NormalFlowChildren);
    Vector<DeprecatedPaintLayerStackingNode*> vector;
    while (DeprecatedPaintLayerStackingNode* normalFlowChild = it.next())
        vector.append(normalFlowChild);
    return vector;
}

void LayoutTreeAsText::writeLayers(TextStream& ts, const DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* layer,
    const LayoutRect& paintRect, int indent, LayoutAsTextBehavior behavior)
{
    // Calculate the clip rects we should use.
    LayoutRect layerBounds;
    ClipRect damageRect, clipRectToApply, outlineRect;
    layer->clipper().calculateRects(ClipRectsContext(rootLayer, UncachedClipRects), paintRect, layerBounds, damageRect, clipRectToApply, outlineRect);

    // Ensure our lists are up-to-date.
    layer->stackingNode()->updateLayerListsIfNeeded();

    bool shouldPaint = (behavior & LayoutAsTextShowAllLayers) ? true : layer->intersectsDamageRect(layerBounds, damageRect.rect(), rootLayer);

    Vector<DeprecatedPaintLayerStackingNode*>* negList = layer->stackingNode()->negZOrderList();
    bool paintsBackgroundSeparately = negList && negList->size() > 0;
    if (shouldPaint && paintsBackgroundSeparately)
        write(ts, *layer, layerBounds, damageRect.rect(), clipRectToApply.rect(), outlineRect.rect(), LayerPaintPhaseBackground, indent, behavior);

    if (negList) {
        int currIndent = indent;
        if (behavior & LayoutAsTextShowLayerNesting) {
            writeIndent(ts, indent);
            ts << " negative z-order list(" << negList->size() << ")\n";
            ++currIndent;
        }
        for (unsigned i = 0; i != negList->size(); ++i)
            writeLayers(ts, rootLayer, negList->at(i)->layer(), paintRect, currIndent, behavior);
    }

    if (shouldPaint)
        write(ts, *layer, layerBounds, damageRect.rect(), clipRectToApply.rect(), outlineRect.rect(), paintsBackgroundSeparately ? LayerPaintPhaseForeground : LayerPaintPhaseAll, indent, behavior);

    Vector<DeprecatedPaintLayerStackingNode*> normalFlowList = normalFlowListFor(layer->stackingNode());
    if (!normalFlowList.isEmpty()) {
        int currIndent = indent;
        if (behavior & LayoutAsTextShowLayerNesting) {
            writeIndent(ts, indent);
            ts << " normal flow list(" << normalFlowList.size() << ")\n";
            ++currIndent;
        }
        for (unsigned i = 0; i != normalFlowList.size(); ++i)
            writeLayers(ts, rootLayer, normalFlowList.at(i)->layer(), paintRect, currIndent, behavior);
    }

    if (Vector<DeprecatedPaintLayerStackingNode*>* posList = layer->stackingNode()->posZOrderList()) {
        int currIndent = indent;
        if (behavior & LayoutAsTextShowLayerNesting) {
            writeIndent(ts, indent);
            ts << " positive z-order list(" << posList->size() << ")\n";
            ++currIndent;
        }
        for (unsigned i = 0; i != posList->size(); ++i)
            writeLayers(ts, rootLayer, posList->at(i)->layer(), paintRect, currIndent, behavior);
    }
}

String nodePositionAsStringForTesting(Node* node)
{
    StringBuilder result;

    Element* body = node->document().body();
    Node* parent;
    for (Node* n = node; n; n = parent) {
        parent = n->parentOrShadowHostNode();
        if (n != node)
            result.appendLiteral(" of ");
        if (parent) {
            if (body && n == body) {
                // We don't care what offset body may be in the document.
                result.appendLiteral("body");
                break;
            }
            if (n->isShadowRoot()) {
                result.append('{');
                result.append(getTagName(n));
                result.append('}');
            } else {
                result.appendLiteral("child ");
                result.appendNumber(n->nodeIndex());
                result.appendLiteral(" {");
                result.append(getTagName(n));
                result.append('}');
            }
        } else {
            result.appendLiteral("document");
        }
    }

    return result.toString();
}

static void writeSelection(TextStream& ts, const LayoutObject* o)
{
    Node* n = o->node();
    if (!n || !n->isDocumentNode())
        return;

    Document* doc = toDocument(n);
    LocalFrame* frame = doc->frame();
    if (!frame)
        return;

    VisibleSelection selection = frame->selection().selection();
    if (selection.isCaret()) {
        ts << "caret: position " << selection.start().deprecatedEditingOffset() << " of " << nodePositionAsStringForTesting(selection.start().deprecatedNode());
        if (selection.affinity() == UPSTREAM)
            ts << " (upstream affinity)";
        ts << "\n";
    } else if (selection.isRange()) {
        ts << "selection start: position " << selection.start().deprecatedEditingOffset() << " of " << nodePositionAsStringForTesting(selection.start().deprecatedNode()) << "\n"
            << "selection end:   position " << selection.end().deprecatedEditingOffset() << " of " << nodePositionAsStringForTesting(selection.end().deprecatedNode()) << "\n";
    }
}

static String externalRepresentation(LayoutBox* layoutObject, LayoutAsTextBehavior behavior)
{
    TextStream ts;
    if (!layoutObject->hasLayer())
        return ts.release();

    DeprecatedPaintLayer* layer = layoutObject->layer();
    LayoutTreeAsText::writeLayers(ts, layer, layer, layer->rect(), 0, behavior);
    writeSelection(ts, layoutObject);
    return ts.release();
}

String externalRepresentation(LocalFrame* frame, LayoutAsTextBehavior behavior)
{
    if (!(behavior & LayoutAsTextDontUpdateLayout))
        frame->document()->updateLayout();

    LayoutObject* layoutObject = frame->contentLayoutObject();
    if (!layoutObject || !layoutObject->isBox())
        return String();

    PrintContext printContext(frame);
    if (behavior & LayoutAsTextPrintingMode)
        printContext.begin(toLayoutBox(layoutObject)->size().width().toFloat());

    return externalRepresentation(toLayoutBox(layoutObject), behavior);
}

String externalRepresentation(Element* element, LayoutAsTextBehavior behavior)
{
    // Doesn't support printing mode.
    ASSERT(!(behavior & LayoutAsTextPrintingMode));
    if (!(behavior & LayoutAsTextDontUpdateLayout))
        element->document().updateLayout();

    LayoutObject* layoutObject = element->layoutObject();
    if (!layoutObject || !layoutObject->isBox())
        return String();

    return externalRepresentation(toLayoutBox(layoutObject), behavior | LayoutAsTextShowAllLayers);
}

static void writeCounterValuesFromChildren(TextStream& stream, LayoutObject* parent, bool& isFirstCounter)
{
    for (LayoutObject* child = parent->slowFirstChild(); child; child = child->nextSibling()) {
        if (child->isCounter()) {
            if (!isFirstCounter)
                stream << " ";
            isFirstCounter = false;
            String str(toLayoutText(child)->text());
            stream << str;
        }
    }
}

String counterValueForElement(Element* element)
{
    // Make sure the element is not freed during the layout.
    RefPtrWillBeRawPtr<Element> protector(element);
    element->document().updateLayout();
    TextStream stream;
    bool isFirstCounter = true;
    // The counter layoutObjects should be children of :before or :after pseudo-elements.
    if (LayoutObject* before = element->pseudoElementLayoutObject(BEFORE))
        writeCounterValuesFromChildren(stream, before, isFirstCounter);
    if (LayoutObject* after = element->pseudoElementLayoutObject(AFTER))
        writeCounterValuesFromChildren(stream, after, isFirstCounter);
    return stream.release();
}

String markerTextForListItem(Element* element)
{
    // Make sure the element is not freed during the layout.
    RefPtrWillBeRawPtr<Element> protector(element);
    element->document().updateLayout();

    LayoutObject* layoutObject = element->layoutObject();
    if (!layoutObject || !layoutObject->isListItem())
        return String();

    return toLayoutListItem(layoutObject)->markerText();
}

} // namespace blink
