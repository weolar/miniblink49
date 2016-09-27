/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#include "core/page/TouchDisambiguation.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/NodeTraversal.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutBlock.h"
#include <algorithm>
#include <cmath>

namespace blink {

static IntRect boundingBoxForEventNodes(Node* eventNode)
{
    if (!eventNode->document().view())
        return IntRect();

    IntRect result;
    Node* node = eventNode;
    while (node) {
        // Skip the whole sub-tree if the node doesn't propagate events.
        if (node != eventNode && node->willRespondToMouseClickEvents()) {
            node = NodeTraversal::nextSkippingChildren(*node, eventNode);
            continue;
        }
        result.unite(node->pixelSnappedBoundingBox());
        node = NodeTraversal::next(*node, eventNode);
    }
    return eventNode->document().view()->contentsToRootFrame(result);
}

static float scoreTouchTarget(IntPoint touchPoint, int padding, IntRect boundingBox)
{
    if (boundingBox.isEmpty())
        return 0;

    float reciprocalPadding = 1.f / padding;
    float score = 1;

    IntSize distance = boundingBox.differenceToPoint(touchPoint);
    score *= std::max((padding - abs(distance.width())) * reciprocalPadding, 0.f);
    score *= std::max((padding - abs(distance.height())) * reciprocalPadding, 0.f);

    return score;
}

struct TouchTargetData {
    IntRect windowBoundingBox;
    float score;
};

void findGoodTouchTargets(const IntRect& touchBoxInRootFrame, LocalFrame* mainFrame, Vector<IntRect>& goodTargets, WillBeHeapVector<RawPtrWillBeMember<Node>>& highlightNodes)
{
    goodTargets.clear();

    int touchPointPadding = ceil(std::max(touchBoxInRootFrame.width(), touchBoxInRootFrame.height()) * 0.5);

    IntPoint touchPoint = touchBoxInRootFrame.center();
    IntPoint contentsPoint = mainFrame->view()->rootFrameToContents(touchPoint);

    HitTestResult result = mainFrame->eventHandler().hitTestResultAtPoint(contentsPoint, HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased, LayoutSize(touchPointPadding, touchPointPadding));
    const WillBeHeapListHashSet<RefPtrWillBeMember<Node>>& hitResults = result.listBasedTestResult();

    // Blacklist nodes that are container of disambiguated nodes.
    // It is not uncommon to have a clickable <div> that contains other clickable objects.
    // This heuristic avoids excessive disambiguation in that case.
    WillBeHeapHashSet<RawPtrWillBeMember<Node>> blackList;
    for (const auto& hitResult : hitResults) {
        // Ignore any Nodes that can't be clicked on.
        LayoutObject* layoutObject = hitResult.get()->layoutObject();
        if (!layoutObject || !hitResult.get()->willRespondToMouseClickEvents())
            continue;

        // Blacklist all of the Node's containers.
        for (LayoutBlock* container = layoutObject->containingBlock(); container; container = container->containingBlock()) {
            Node* containerNode = container->node();
            if (!containerNode)
                continue;
            if (!blackList.add(containerNode).isNewEntry)
                break;
        }
    }

    WillBeHeapHashMap<RawPtrWillBeMember<Node>, TouchTargetData> touchTargets;
    float bestScore = 0;
    for (const auto& hitResult : hitResults) {
        for (Node* node = hitResult.get(); node; node = node->parentNode()) {
            if (blackList.contains(node))
                continue;
            if (node->isDocumentNode() || isHTMLHtmlElement(*node) || isHTMLBodyElement(*node))
                break;
            if (node->willRespondToMouseClickEvents()) {
                TouchTargetData& targetData = touchTargets.add(node, TouchTargetData()).storedValue->value;
                targetData.windowBoundingBox = boundingBoxForEventNodes(node);
                targetData.score = scoreTouchTarget(touchPoint, touchPointPadding, targetData.windowBoundingBox);
                bestScore = std::max(bestScore, targetData.score);
                break;
            }
        }
    }

    for (const auto& touchTarget : touchTargets) {
        // Currently the scoring function uses the overlap area with the fat point as the score.
        // We ignore the candidates that has less than 1/2 overlap (we consider not really ambiguous enough) than the best candidate to avoid excessive popups.
        if (touchTarget.value.score < bestScore * 0.5)
            continue;
        goodTargets.append(touchTarget.value.windowBoundingBox);
        highlightNodes.append(touchTarget.key);
    }
}

} // namespace blink
