/*
 * Copyright (C) 2004, 2008, 2009, 2010 Apple Inc. All rights reserved.
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
#include "core/editing/Caret.h"

#include "core/dom/Document.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLTextFormControlElement.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutView.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/graphics/GraphicsContext.h"

namespace blink {

CaretBase::CaretBase(CaretVisibility visibility)
    : m_caretVisibility(visibility)
{
}

DragCaretController::DragCaretController()
    : CaretBase(Visible)
{
}

PassOwnPtrWillBeRawPtr<DragCaretController> DragCaretController::create()
{
    return adoptPtrWillBeNoop(new DragCaretController);
}

bool DragCaretController::isContentRichlyEditable() const
{
    return isRichlyEditablePosition(m_position.deepEquivalent());
}

void DragCaretController::setCaretPosition(const VisiblePosition& position)
{
    // for querying Layer::compositingState()
    // This code is probably correct, since it doesn't occur in a stack that involves updating compositing state.
    DisableCompositingQueryAsserts disabler;

    if (Node* node = m_position.deepEquivalent().deprecatedNode())
        invalidateCaretRect(node);
    m_position = position;
    Document* document = nullptr;
    if (Node* node = m_position.deepEquivalent().deprecatedNode()) {
        invalidateCaretRect(node);
        document = &node->document();
    }
    if (m_position.isNull() || m_position.isOrphan()) {
        clearCaretRect();
    } else {
        document->updateLayoutTreeIfNeeded();
        updateCaretRect(document, m_position);
    }
}

static bool removingNodeRemovesPosition(Node& node, const Position& position)
{
    if (!position.anchorNode())
        return false;

    if (position.anchorNode() == node)
        return true;

    if (!node.isElementNode())
        return false;

    Element& element = toElement(node);
    return element.containsIncludingShadowDOM(position.anchorNode());
}

void DragCaretController::nodeWillBeRemoved(Node& node)
{
    if (!hasCaret() || !node.inActiveDocument())
        return;

    if (!removingNodeRemovesPosition(node, m_position.deepEquivalent()))
        return;

    m_position.deepEquivalent().document()->layoutView()->clearSelection();
    clear();
}

DEFINE_TRACE(DragCaretController)
{
    visitor->trace(m_position);
}

void CaretBase::clearCaretRect()
{
    m_caretLocalRect = LayoutRect();
}

static inline bool caretRendersInsideNode(Node* node)
{
    return node && !isRenderedTableElement(node) && !editingIgnoresContent(node);
}

LayoutBlock* CaretBase::caretLayoutObject(Node* node)
{
    if (!node)
        return 0;

    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject)
        return 0;

    // if caretNode is a block and caret is inside it then caret should be painted by that block
    bool paintedByBlock = layoutObject->isLayoutBlock() && caretRendersInsideNode(node);
    return paintedByBlock ? toLayoutBlock(layoutObject) : layoutObject->containingBlock();
}

static void mapCaretRectToCaretPainter(LayoutObject* caretLayoutObject, LayoutBlock* caretPainter, LayoutRect& caretRect)
{
    // FIXME: This shouldn't be called on un-rooted subtrees.
    // FIXME: This should probably just use mapLocalToContainer.
    // Compute an offset between the caretLayoutObject and the caretPainter.

    ASSERT(caretLayoutObject->isDescendantOf(caretPainter));

    bool unrooted = false;
    while (caretLayoutObject != caretPainter) {
        LayoutObject* containerObject = caretLayoutObject->container();
        if (!containerObject) {
            unrooted = true;
            break;
        }
        caretRect.move(caretLayoutObject->offsetFromContainer(containerObject, caretRect.location()));
        caretLayoutObject = containerObject;
    }

    if (unrooted)
        caretRect = LayoutRect();
}

bool CaretBase::updateCaretRect(Document* document, const PositionWithAffinity& caretPosition)
{
    m_caretLocalRect = LayoutRect();

    if (caretPosition.position().isNull())
        return false;

    ASSERT(caretPosition.position().deprecatedNode()->layoutObject());

    // First compute a rect local to the layoutObject at the selection start.
    LayoutObject* layoutObject;
    m_caretLocalRect = localCaretRectOfPosition(caretPosition, layoutObject);

    // Get the layoutObject that will be responsible for painting the caret
    // (which is either the layoutObject we just found, or one of its containers).
    LayoutBlock* caretPainter = caretLayoutObject(caretPosition.position().anchorNode());

    mapCaretRectToCaretPainter(layoutObject, caretPainter, m_caretLocalRect);

    return true;
}

bool CaretBase::updateCaretRect(Document* document, const VisiblePosition& caretPosition)
{
    return updateCaretRect(document, PositionWithAffinity(caretPosition.deepEquivalent(), caretPosition.affinity()));
}

IntRect CaretBase::absoluteBoundsForLocalRect(Node* node, const LayoutRect& rect) const
{
    LayoutBlock* caretPainter = caretLayoutObject(node);
    if (!caretPainter)
        return IntRect();

    LayoutRect localRect(rect);
    caretPainter->flipForWritingMode(localRect);
    return caretPainter->localToAbsoluteQuad(FloatRect(localRect)).enclosingBoundingBox();
}

void CaretBase::invalidateLocalCaretRect(Node* node, const LayoutRect& rect)
{
    LayoutBlock* caretPainter = caretLayoutObject(node);
    if (!caretPainter)
        return;

    // FIXME: Need to over-paint 1 pixel to workaround some rounding problems.
    // https://bugs.webkit.org/show_bug.cgi?id=108283
    LayoutRect inflatedRect = rect;
    inflatedRect.inflate(1);

    // FIXME: We should use mapLocalToContainer() since we know we're not un-rooted.
    mapCaretRectToCaretPainter(node->layoutObject(), caretPainter, inflatedRect);

    // FIXME: We should not allow paint invalidation out of paint invalidation state. crbug.com/457415
    DisablePaintInvalidationStateAsserts disabler;
    caretPainter->invalidatePaintRectangle(inflatedRect);
}

bool CaretBase::shouldRepaintCaret(Node& node) const
{
    // If PositionAnchorType::BeforeAnchor or PositionAnchorType::AfterAnchor,
    // carets need to be repainted not only when the node is contentEditable but
    // also when its parentNode() is contentEditable.
    return node.isContentEditable() || (node.parentNode() && node.parentNode()->isContentEditable());
}

bool CaretBase::shouldRepaintCaret(const LayoutView* view) const
{
    ASSERT(view);
    if (FrameView* frameView = view->frameView()) {
        LocalFrame& frame = frameView->frame(); // The frame where the selection started
        return frame.settings() && frame.settings()->caretBrowsingEnabled();
    }
    return false;
}

void CaretBase::invalidateCaretRect(Node* node, bool caretRectChanged)
{
    if (caretRectChanged)
        return;

    if (LayoutView* view = node->document().layoutView()) {
        if (node->isContentEditable(Node::UserSelectAllIsAlwaysNonEditable) || shouldRepaintCaret(view))
            invalidateLocalCaretRect(node, localCaretRectWithoutUpdate());
    }
}

void CaretBase::paintCaret(Node* node, GraphicsContext* context, const LayoutPoint& paintOffset, const LayoutRect& clipRect) const
{
    if (m_caretVisibility == Hidden)
        return;

    LayoutRect drawingRect = localCaretRectWithoutUpdate();
    if (LayoutBlock* layoutObject = caretLayoutObject(node))
        layoutObject->flipForWritingMode(drawingRect);
    drawingRect.moveBy(roundedIntPoint(paintOffset));
    LayoutRect caret = intersection(drawingRect, clipRect);
    if (caret.isEmpty())
        return;

    Color caretColor = Color::black;

    Element* element;
    if (node->isElementNode())
        element = toElement(node);
    else
        element = node->parentElement();

    if (element && element->layoutObject())
        caretColor = element->layoutObject()->resolveColor(CSSPropertyColor);

    context->fillRect(caret, caretColor);
}

LayoutBlock* DragCaretController::caretLayoutObject() const
{
    return CaretBase::caretLayoutObject(m_position.deepEquivalent().anchorNode());
}

void DragCaretController::paintDragCaret(LocalFrame* frame, GraphicsContext* p, const LayoutPoint& paintOffset, const LayoutRect& clipRect) const
{
    if (m_position.deepEquivalent().deprecatedNode()->document().frame() == frame)
        paintCaret(m_position.deepEquivalent().deprecatedNode(), p, paintOffset, clipRect);
}

}
