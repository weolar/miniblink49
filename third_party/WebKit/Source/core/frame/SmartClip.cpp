/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/frame/SmartClip.h"

#include "core/dom/ContainerNode.h"
#include "core/dom/Document.h"
#include "core/dom/NodeTraversal.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/layout/LayoutObject.h"
#include "core/page/Page.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

static IntRect convertToContentCoordinatesWithoutCollapsingToZero(const IntRect& rectInViewport, const FrameView* view)
{
    IntRect rectInContents = view->viewportToContents(rectInViewport);
    if (rectInViewport.width() > 0 && !rectInContents.width())
        rectInContents.setWidth(1);
    if (rectInViewport.height() > 0 && !rectInContents.height())
        rectInContents.setHeight(1);
    return rectInContents;
}

static Node* nodeInsideFrame(Node* node)
{
    if (node->isFrameOwnerElement())
        return toHTMLFrameOwnerElement(node)->contentDocument();
    return nullptr;
}

IntRect SmartClipData::rectInViewport() const
{
    return m_rectInViewport;
}

const String& SmartClipData::clipData() const
{
    return m_string;
}

SmartClip::SmartClip(PassRefPtrWillBeRawPtr<LocalFrame> frame)
    : m_frame(frame)
{
}

SmartClipData SmartClip::dataForRect(const IntRect& cropRectInViewport)
{
    Node* bestNode = findBestOverlappingNode(m_frame->document(), cropRectInViewport);
    if (!bestNode)
        return SmartClipData();

    if (Node* nodeFromFrame = nodeInsideFrame(bestNode)) {
        // FIXME: This code only hit-tests a single iframe. It seems like we ought support nested frames.
        if (Node* bestNodeInFrame = findBestOverlappingNode(nodeFromFrame, cropRectInViewport))
            bestNode = bestNodeInFrame;
    }

    WillBeHeapVector<RawPtrWillBeMember<Node>> hitNodes;
    collectOverlappingChildNodes(bestNode, cropRectInViewport, hitNodes);

    if (hitNodes.isEmpty() || hitNodes.size() == bestNode->countChildren()) {
        hitNodes.clear();
        hitNodes.append(bestNode);
    }

    // Unite won't work with the empty rect, so we initialize to the first rect.
    IntRect unitedRects = hitNodes[0]->pixelSnappedBoundingBox();
    StringBuilder collectedText;
    for (size_t i = 0; i < hitNodes.size(); ++i) {
        collectedText.append(extractTextFromNode(hitNodes[i]));
        unitedRects.unite(hitNodes[i]->pixelSnappedBoundingBox());
    }

    return SmartClipData(bestNode, m_frame->document()->view()->contentsToViewport(unitedRects), collectedText.toString());
}

float SmartClip::pageScaleFactor()
{
    return m_frame->page()->pageScaleFactor();
}

// This function is a bit of a mystery. If you understand what it does, please
// consider adding a more descriptive name.
Node* SmartClip::minNodeContainsNodes(Node* minNode, Node* newNode)
{
    if (!newNode)
        return minNode;
    if (!minNode)
        return newNode;

    IntRect minNodeRect = minNode->pixelSnappedBoundingBox();
    IntRect newNodeRect = newNode->pixelSnappedBoundingBox();

    Node* parentMinNode = minNode->parentNode();
    Node* parentNewNode = newNode->parentNode();

    if (minNodeRect.contains(newNodeRect)) {
        if (parentMinNode && parentNewNode && parentNewNode->parentNode() == parentMinNode)
            return parentMinNode;
        return minNode;
    }

    if (newNodeRect.contains(minNodeRect)) {
        if (parentMinNode && parentNewNode && parentMinNode->parentNode() == parentNewNode)
            return parentNewNode;
        return newNode;
    }

    // This loop appears to find the nearest ancestor of minNode (in DOM order)
    // that contains the newNodeRect. It's very unclear to me why that's an
    // interesting node to find. Presumably this loop will often just return
    // the documentElement.
    Node* node = minNode;
    while (node) {
        if (node->layoutObject()) {
            IntRect nodeRect = node->pixelSnappedBoundingBox();
            if (nodeRect.contains(newNodeRect)) {
                return node;
            }
        }
        node = node->parentNode();
    }

    return nullptr;
}

Node* SmartClip::findBestOverlappingNode(Node* rootNode, const IntRect& cropRectInViewport)
{
    if (!rootNode)
        return nullptr;

    IntRect resizedCropRect = convertToContentCoordinatesWithoutCollapsingToZero(cropRectInViewport, rootNode->document().view());

    Node* node = rootNode;
    Node* minNode = nullptr;

    while (node) {
        IntRect nodeRect = node->pixelSnappedBoundingBox();

        if (node->isElementNode() && equalIgnoringCase(toElement(node)->fastGetAttribute(HTMLNames::aria_hiddenAttr), "true")) {
            node = NodeTraversal::nextSkippingChildren(*node, rootNode);
            continue;
        }

        LayoutObject* layoutObject = node->layoutObject();
        if (layoutObject && !nodeRect.isEmpty()) {
            if (layoutObject->isText()
                || layoutObject->isLayoutImage()
                || node->isFrameOwnerElement()
                || (layoutObject->style()->hasBackgroundImage() && !shouldSkipBackgroundImage(node))) {
                if (resizedCropRect.intersects(nodeRect)) {
                    minNode = minNodeContainsNodes(minNode, node);
                } else {
                    node = NodeTraversal::nextSkippingChildren(*node, rootNode);
                    continue;
                }
            }
        }
        node = NodeTraversal::next(*node, rootNode);
    }

    return minNode;
}

// This function appears to heuristically guess whether to include a background
// image in the smart clip. It seems to want to include sprites created from
// CSS background images but to skip actual backgrounds.
bool SmartClip::shouldSkipBackgroundImage(Node* node)
{
    ASSERT(node);
    // Apparently we're only interested in background images on spans and divs.
    if (!isHTMLSpanElement(*node) && !isHTMLDivElement(*node))
        return true;

    // This check actually makes a bit of sense. If you're going to sprite an
    // image out of a CSS background, you're probably going to specify a height
    // or a width. On the other hand, if we've got a legit background image,
    // it's very likely the height or the width will be set to auto.
    LayoutObject* layoutObject = node->layoutObject();
    if (layoutObject && (layoutObject->style()->logicalHeight().isAuto() || layoutObject->style()->logicalWidth().isAuto()))
        return true;

    return false;
}

void SmartClip::collectOverlappingChildNodes(Node* parentNode, const IntRect& cropRectInViewport, WillBeHeapVector<RawPtrWillBeMember<Node>>& hitNodes)
{
    if (!parentNode)
        return;
    IntRect resizedCropRect = convertToContentCoordinatesWithoutCollapsingToZero(cropRectInViewport, parentNode->document().view());
    for (Node* child = parentNode->firstChild(); child; child = child->nextSibling()) {
        IntRect childRect = child->pixelSnappedBoundingBox();
        if (resizedCropRect.intersects(childRect))
            hitNodes.append(child);
    }
}

String SmartClip::extractTextFromNode(Node* node)
{
    // Science has proven that no text nodes are ever positioned at y == -99999.
    int prevYPos = -99999;

    StringBuilder result;
    for (Node& currentNode : NodeTraversal::inclusiveDescendantsOf(*node)) {
        const ComputedStyle* style = currentNode.ensureComputedStyle();
        if (style && style->userSelect() == SELECT_NONE)
            continue;

        if (Node* nodeFromFrame = nodeInsideFrame(&currentNode))
            result.append(extractTextFromNode(nodeFromFrame));

        IntRect nodeRect = currentNode.pixelSnappedBoundingBox();
        if (currentNode.layoutObject() && !nodeRect.isEmpty()) {
            if (currentNode.isTextNode()) {
                String nodeValue = currentNode.nodeValue();

                // It's unclear why we blacklist solitary "\n" node values.
                // Maybe we're trying to ignore <br> tags somehow?
                if (nodeValue == "\n")
                    nodeValue = "";

                if (nodeRect.y() != prevYPos) {
                    prevYPos = nodeRect.y();
                    result.append('\n');
                }

                result.append(nodeValue);
            }
        }
    }

    return result.toString();
}

} // namespace blink
