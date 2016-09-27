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
#include "core/paint/DeprecatedPaintLayerStackingNodeIterator.h"

// FIXME: We should build our primitive on top of
// DeprecatedLayerStackingNode and remove this include.
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/DeprecatedPaintLayerStackingNode.h"

namespace blink {

DeprecatedPaintLayerStackingNodeIterator::DeprecatedPaintLayerStackingNodeIterator(const DeprecatedPaintLayerStackingNode& root, unsigned whichChildren)
    : m_root(root)
    , m_remainingChildren(whichChildren)
    , m_index(0)
{
    m_currentNormalFlowChild = root.layer()->firstChild();
}

DeprecatedPaintLayerStackingNode* DeprecatedPaintLayerStackingNodeIterator::next()
{
    if (m_remainingChildren & NegativeZOrderChildren) {
        Vector<DeprecatedPaintLayerStackingNode*>* negZOrderList = m_root.negZOrderList();
        if (negZOrderList && m_index < negZOrderList->size())
            return negZOrderList->at(m_index++);

        m_index = 0;
        m_remainingChildren &= ~NegativeZOrderChildren;
    }

    if (m_remainingChildren & NormalFlowChildren) {
        for (; m_currentNormalFlowChild; m_currentNormalFlowChild = m_currentNormalFlowChild->nextSibling()) {
            if (!m_currentNormalFlowChild->stackingNode()->isTreatedAsStackingContextForPainting() && !m_currentNormalFlowChild->isReflection()) {
                DeprecatedPaintLayer* normalFlowChild = m_currentNormalFlowChild;
                m_currentNormalFlowChild = m_currentNormalFlowChild->nextSibling();
                return normalFlowChild->stackingNode();
            }
        }

        // We reset the iterator in case we reuse it.
        m_currentNormalFlowChild = m_root.layer()->firstChild();
        m_remainingChildren &= ~NormalFlowChildren;
    }

    if (m_remainingChildren & PositiveZOrderChildren) {
        Vector<DeprecatedPaintLayerStackingNode*>* posZOrderList = m_root.posZOrderList();
        if (posZOrderList && m_index < posZOrderList->size())
            return posZOrderList->at(m_index++);

        m_index = 0;
        m_remainingChildren &= ~PositiveZOrderChildren;
    }

    return 0;
}

DeprecatedPaintLayerStackingNode* DeprecatedPaintLayerStackingNodeReverseIterator::next()
{
    if (m_remainingChildren & NegativeZOrderChildren) {
        Vector<DeprecatedPaintLayerStackingNode*>* negZOrderList = m_root.negZOrderList();
        if (negZOrderList && m_index >= 0)
            return negZOrderList->at(m_index--);

        m_remainingChildren &= ~NegativeZOrderChildren;
        setIndexToLastItem();
    }

    if (m_remainingChildren & NormalFlowChildren) {
        for (; m_currentNormalFlowChild; m_currentNormalFlowChild = m_currentNormalFlowChild->previousSibling()) {
            if (!m_currentNormalFlowChild->stackingNode()->isTreatedAsStackingContextForPainting() && !m_currentNormalFlowChild->isReflection()) {
                DeprecatedPaintLayer* normalFlowChild = m_currentNormalFlowChild;
                m_currentNormalFlowChild = m_currentNormalFlowChild->previousSibling();
                return normalFlowChild->stackingNode();
            }
        }

        m_remainingChildren &= ~NormalFlowChildren;
        setIndexToLastItem();
    }

    if (m_remainingChildren & PositiveZOrderChildren) {
        Vector<DeprecatedPaintLayerStackingNode*>* posZOrderList = m_root.posZOrderList();
        if (posZOrderList && m_index >= 0)
            return posZOrderList->at(m_index--);

        m_remainingChildren &= ~PositiveZOrderChildren;
        setIndexToLastItem();
    }

    return 0;
}

void DeprecatedPaintLayerStackingNodeReverseIterator::setIndexToLastItem()
{
    if (m_remainingChildren & NegativeZOrderChildren) {
        Vector<DeprecatedPaintLayerStackingNode*>* negZOrderList = m_root.negZOrderList();
        if (negZOrderList) {
            m_index  = negZOrderList->size() - 1;
            return;
        }

        m_remainingChildren &= ~NegativeZOrderChildren;
    }

    if (m_remainingChildren & NormalFlowChildren) {
        m_currentNormalFlowChild = m_root.layer()->lastChild();
        return;
    }

    if (m_remainingChildren & PositiveZOrderChildren) {
        Vector<DeprecatedPaintLayerStackingNode*>* posZOrderList = m_root.posZOrderList();
        if (posZOrderList) {
            m_index = posZOrderList->size() - 1;
            return;
        }

        m_remainingChildren &= ~PositiveZOrderChildren;
    }

    // No more list to visit.
    ASSERT(!m_remainingChildren);
    m_index = -1;
}

} // namespace blink
