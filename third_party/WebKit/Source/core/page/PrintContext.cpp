/*
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2007 Apple Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/page/PrintContext.h"

#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutView.h"
#include "third_party/skia/include/core/SkAnnotation.h"

namespace blink {

// By imaging to a width a little wider than the available pixels,
// thin pages will be scaled down a little, matching the way they
// print in IE and Camino. This lets them use fewer sheets than they
// would otherwise, which is presumably why other browsers do this.
// Wide pages will be scaled down more than this.
const float printingMinimumShrinkFactor = 1.25f;

// This number determines how small we are willing to reduce the page content
// in order to accommodate the widest line. If the page would have to be
// reduced smaller to make the widest line fit, we just clip instead (this
// behavior matches MacIE and Mozilla, at least)
const float printingMaximumShrinkFactor = 2;

PrintContext::PrintContext(LocalFrame* frame)
    : m_frame(frame)
    , m_isPrinting(false)
    , m_linkedDestinationsValid(false)
{
}

PrintContext::~PrintContext()
{
    if (m_isPrinting)
        end();
}

void PrintContext::computePageRects(const FloatRect& printRect, float headerHeight, float footerHeight, float userScaleFactor, float& outPageHeight)
{
    m_pageRects.clear();
    outPageHeight = 0;

    if (!m_frame->document() || !m_frame->view() || !m_frame->document()->layoutView())
        return;

    if (userScaleFactor <= 0) {
        WTF_LOG_ERROR("userScaleFactor has bad value %.2f", userScaleFactor);
        return;
    }

    LayoutView* view = m_frame->document()->layoutView();
    const IntRect& documentRect = view->documentRect();
    FloatSize pageSize = m_frame->resizePageRectsKeepingRatio(FloatSize(printRect.width(), printRect.height()), FloatSize(documentRect.width(), documentRect.height()));
    float pageWidth = pageSize.width();
    float pageHeight = pageSize.height();

    outPageHeight = pageHeight; // this is the height of the page adjusted by margins
    pageHeight -= headerHeight + footerHeight;

    if (pageHeight <= 0) {
        WTF_LOG_ERROR("pageHeight has bad value %.2f", pageHeight);
        return;
    }

    computePageRectsWithPageSizeInternal(FloatSize(pageWidth / userScaleFactor, pageHeight / userScaleFactor));
}

void PrintContext::computePageRectsWithPageSize(const FloatSize& pageSizeInPixels)
{
    m_pageRects.clear();
    computePageRectsWithPageSizeInternal(pageSizeInPixels);
}

void PrintContext::computePageRectsWithPageSizeInternal(const FloatSize& pageSizeInPixels)
{
    if (!m_frame->document() || !m_frame->view() || !m_frame->document()->layoutView())
        return;

    LayoutView* view = m_frame->document()->layoutView();

    IntRect docRect = view->documentRect();

    int pageWidth = pageSizeInPixels.width();
    int pageHeight = pageSizeInPixels.height();

    bool isHorizontal = view->style()->isHorizontalWritingMode();

    int docLogicalHeight = isHorizontal ? docRect.height() : docRect.width();
    int pageLogicalHeight = isHorizontal ? pageHeight : pageWidth;
    int pageLogicalWidth = isHorizontal ? pageWidth : pageHeight;

    int inlineDirectionStart;
    int inlineDirectionEnd;
    int blockDirectionStart;
    int blockDirectionEnd;
    if (isHorizontal) {
        if (view->style()->isFlippedBlocksWritingMode()) {
            blockDirectionStart = docRect.maxY();
            blockDirectionEnd = docRect.y();
        } else {
            blockDirectionStart = docRect.y();
            blockDirectionEnd = docRect.maxY();
        }
        inlineDirectionStart = view->style()->isLeftToRightDirection() ? docRect.x() : docRect.maxX();
        inlineDirectionEnd = view->style()->isLeftToRightDirection() ? docRect.maxX() : docRect.x();
    } else {
        if (view->style()->isFlippedBlocksWritingMode()) {
            blockDirectionStart = docRect.maxX();
            blockDirectionEnd = docRect.x();
        } else {
            blockDirectionStart = docRect.x();
            blockDirectionEnd = docRect.maxX();
        }
        inlineDirectionStart = view->style()->isLeftToRightDirection() ? docRect.y() : docRect.maxY();
        inlineDirectionEnd = view->style()->isLeftToRightDirection() ? docRect.maxY() : docRect.y();
    }

    unsigned pageCount = ceilf((float)docLogicalHeight / pageLogicalHeight);
    for (unsigned i = 0; i < pageCount; ++i) {
        int pageLogicalTop = blockDirectionEnd > blockDirectionStart ?
                                blockDirectionStart + i * pageLogicalHeight :
                                blockDirectionStart - (i + 1) * pageLogicalHeight;

        int pageLogicalLeft = inlineDirectionEnd > inlineDirectionStart ? inlineDirectionStart : inlineDirectionStart - pageLogicalWidth;
        IntRect pageRect(pageLogicalLeft, pageLogicalTop, pageLogicalWidth, pageLogicalHeight);
        if (!isHorizontal)
            pageRect = pageRect.transposedRect();
        m_pageRects.append(pageRect);

    }
}

void PrintContext::begin(float width, float height)
{
    // This function can be called multiple times to adjust printing parameters without going back to screen mode.
    m_isPrinting = true;

    FloatSize originalPageSize = FloatSize(width, height);
    FloatSize minLayoutSize = m_frame->resizePageRectsKeepingRatio(originalPageSize, FloatSize(width * printingMinimumShrinkFactor, height * printingMinimumShrinkFactor));

    // This changes layout, so callers need to make sure that they don't paint to screen while in printing mode.
    m_frame->setPrinting(true, minLayoutSize, originalPageSize, printingMaximumShrinkFactor / printingMinimumShrinkFactor);
}

void PrintContext::end()
{
    ASSERT(m_isPrinting);
    m_isPrinting = false;
    m_frame->setPrinting(false, FloatSize(), FloatSize(), 0);
    m_linkedDestinations.clear();
    m_linkedDestinationsValid = false;
}

static LayoutBoxModelObject* enclosingBoxModelObject(LayoutObject* object)
{

    while (object && !object->isBoxModelObject())
        object = object->parent();
    if (!object)
        return nullptr;
    return toLayoutBoxModelObject(object);
}

int PrintContext::pageNumberForElement(Element* element, const FloatSize& pageSizeInPixels)
{
    // Make sure the element is not freed during the layout.
    RefPtrWillBeRawPtr<Element> protect(element);
    element->document().updateLayout();

    LayoutBoxModelObject* box = enclosingBoxModelObject(element->layoutObject());
    if (!box)
        return -1;

    LocalFrame* frame = element->document().frame();
    FloatRect pageRect(FloatPoint(0, 0), pageSizeInPixels);
    PrintContext printContext(frame);
    printContext.begin(pageRect.width(), pageRect.height());
    FloatSize scaledPageSize = pageSizeInPixels;
    scaledPageSize.scale(frame->view()->contentsSize().width() / pageRect.width());
    printContext.computePageRectsWithPageSize(scaledPageSize);

    int top = box->pixelSnappedOffsetTop();
    int left = box->pixelSnappedOffsetLeft();
    size_t pageNumber = 0;
    for (; pageNumber < printContext.pageCount(); pageNumber++) {
        const IntRect& page = printContext.pageRect(pageNumber);
        if (page.x() <= left && left < page.maxX() && page.y() <= top && top < page.maxY())
            return pageNumber;
    }
    return -1;
}

void PrintContext::collectLinkedDestinations(Node* node)
{
    for (Node* i = node->firstChild(); i; i = i->nextSibling())
        collectLinkedDestinations(i);

    if (!node->isLink() || !node->isElementNode())
        return;
    const AtomicString& href = toElement(node)->getAttribute(HTMLNames::hrefAttr);
    if (href.isNull())
        return;
    KURL url = node->document().completeURL(href);
    if (!url.isValid())
        return;

    if (url.hasFragmentIdentifier() && equalIgnoringFragmentIdentifier(url, node->document().baseURL())) {
        String name = url.fragmentIdentifier();
        if (Element* element = node->document().findAnchor(name))
            m_linkedDestinations.set(name, element);
    }
}

void PrintContext::outputLinkedDestinations(SkCanvas* canvas, const IntRect& pageRect)
{
    if (!m_linkedDestinationsValid) {
        // Collect anchors in the top-level frame only because our PrintContext
        // supports only one namespace for the anchors.
        collectLinkedDestinations(frame()->document());
        m_linkedDestinationsValid = true;
    }

    for (const auto& entry : m_linkedDestinations) {
        LayoutObject* layoutObject = entry.value->layoutObject();
        if (!layoutObject || !layoutObject->frameView())
            continue;
        IntRect boundingBox = layoutObject->absoluteBoundingBoxRect();
        boundingBox = layoutObject->frameView()->convertToContainingWindow(boundingBox);
        if (!pageRect.intersects(boundingBox))
            continue;
        IntPoint point = boundingBox.minXMinYCorner();
        point.clampNegativeToZero();
        SkAutoDataUnref nameData(SkData::NewWithCString(entry.key.utf8().data()));
        SkAnnotateNamedDestination(canvas, SkPoint::Make(point.x(), point.y()), nameData);
    }
}

String PrintContext::pageProperty(LocalFrame* frame, const char* propertyName, int pageNumber)
{
    Document* document = frame->document();
    PrintContext printContext(frame);
    printContext.begin(800); // Any width is OK here.
    document->updateLayout();
    RefPtr<ComputedStyle> style = document->styleForPage(pageNumber);

    // Implement formatters for properties we care about.
    if (!strcmp(propertyName, "margin-left")) {
        if (style->marginLeft().isAuto())
            return String("auto");
        return String::number(style->marginLeft().value());
    }
    if (!strcmp(propertyName, "line-height"))
        return String::number(style->lineHeight().value());
    if (!strcmp(propertyName, "font-size"))
        return String::number(style->fontDescription().computedPixelSize());
    if (!strcmp(propertyName, "font-family"))
        return style->fontDescription().family().family().string();
    if (!strcmp(propertyName, "size"))
        return String::number(style->pageSize().width()) + ' ' + String::number(style->pageSize().height());

    return String("pageProperty() unimplemented for: ") + propertyName;
}

bool PrintContext::isPageBoxVisible(LocalFrame* frame, int pageNumber)
{
    return frame->document()->isPageBoxVisible(pageNumber);
}

String PrintContext::pageSizeAndMarginsInPixels(LocalFrame* frame, int pageNumber, int width, int height, int marginTop, int marginRight, int marginBottom, int marginLeft)
{
    IntSize pageSize(width, height);
    frame->document()->pageSizeAndMarginsInPixels(pageNumber, pageSize, marginTop, marginRight, marginBottom, marginLeft);

    return "(" + String::number(pageSize.width()) + ", " + String::number(pageSize.height()) + ") " +
           String::number(marginTop) + ' ' + String::number(marginRight) + ' ' + String::number(marginBottom) + ' ' + String::number(marginLeft);
}

int PrintContext::numberOfPages(LocalFrame* frame, const FloatSize& pageSizeInPixels)
{
    frame->document()->updateLayout();

    FloatRect pageRect(FloatPoint(0, 0), pageSizeInPixels);
    PrintContext printContext(frame);
    printContext.begin(pageRect.width(), pageRect.height());
    // Account for shrink-to-fit.
    FloatSize scaledPageSize = pageSizeInPixels;
    scaledPageSize.scale(frame->view()->contentsSize().width() / pageRect.width());
    printContext.computePageRectsWithPageSize(scaledPageSize);
    return printContext.pageCount();
}

DEFINE_TRACE(PrintContext)
{
#if ENABLE(OILPAN)
    visitor->trace(m_frame);
    visitor->trace(m_linkedDestinations);
#endif
}

}
