/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/dom/PositionIterator.h"

namespace blink {

template <typename Strategy>
PositionIteratorAlgorithm<Strategy>::PositionIteratorAlgorithm(Node* anchorNode, int offsetInAnchor)
    : m_anchorNode(anchorNode)
    , m_nodeAfterPositionInAnchor(Strategy::childAt(*anchorNode, offsetInAnchor))
    , m_offsetInAnchor(m_nodeAfterPositionInAnchor ? 0 : offsetInAnchor)
{
}
template <typename Strategy>
PositionIteratorAlgorithm<Strategy>::PositionIteratorAlgorithm(const PositionAlgorithm<Strategy>& pos)
    : PositionIteratorAlgorithm(pos.anchorNode(), pos.deprecatedEditingOffset())
{
}

template <typename Strategy>
PositionIteratorAlgorithm<Strategy>::PositionIteratorAlgorithm()
    : m_anchorNode(nullptr)
    , m_nodeAfterPositionInAnchor(nullptr)
    , m_offsetInAnchor(0)
{
}

template <typename Strategy>
PositionIteratorAlgorithm<Strategy>::operator PositionAlgorithm<Strategy>() const
{
    if (m_nodeAfterPositionInAnchor) {
        ASSERT(Strategy::parent(*m_nodeAfterPositionInAnchor) == m_anchorNode);
        // FIXME: This check is inadaquete because any ancestor could be ignored by editing
        if (Strategy::editingIgnoresContent(Strategy::parent(*m_nodeAfterPositionInAnchor)))
            return PositionAlgorithm<Strategy>::beforeNode(m_anchorNode);
        return PositionAlgorithm<Strategy>::inParentBeforeNode(*m_nodeAfterPositionInAnchor);
    }
    if (Strategy::hasChildren(*m_anchorNode))
        return PositionAlgorithm<Strategy>::lastPositionInOrAfterNode(m_anchorNode);
    return PositionAlgorithm<Strategy>::createLegacyEditingPosition(m_anchorNode, m_offsetInAnchor);
}

template <typename Strategy>
void PositionIteratorAlgorithm<Strategy>::increment()
{
    if (!m_anchorNode)
        return;

    if (m_nodeAfterPositionInAnchor) {
        m_anchorNode = m_nodeAfterPositionInAnchor;
        m_nodeAfterPositionInAnchor = Strategy::firstChild(*m_anchorNode);
        m_offsetInAnchor = 0;
        return;
    }

    if (!Strategy::hasChildren(*m_anchorNode) && m_offsetInAnchor < Strategy::lastOffsetForEditing(m_anchorNode)) {
        m_offsetInAnchor = PositionAlgorithm<Strategy>::uncheckedNextOffset(m_anchorNode, m_offsetInAnchor);
    } else {
        m_nodeAfterPositionInAnchor = m_anchorNode;
        m_anchorNode = Strategy::parent(*m_nodeAfterPositionInAnchor);
        m_nodeAfterPositionInAnchor = Strategy::nextSibling(*m_nodeAfterPositionInAnchor);
        m_offsetInAnchor = 0;
    }
}

template <typename Strategy>
void PositionIteratorAlgorithm<Strategy>::decrement()
{
    if (!m_anchorNode)
        return;

    if (m_nodeAfterPositionInAnchor) {
        m_anchorNode = Strategy::previousSibling(*m_nodeAfterPositionInAnchor);
        if (m_anchorNode) {
            m_nodeAfterPositionInAnchor = nullptr;
            m_offsetInAnchor = Strategy::hasChildren(*m_anchorNode) ? 0 : Strategy::lastOffsetForEditing(m_anchorNode);
        } else {
            m_nodeAfterPositionInAnchor = Strategy::parent(*m_nodeAfterPositionInAnchor);
            m_anchorNode = Strategy::parent(*m_nodeAfterPositionInAnchor);
            m_offsetInAnchor = 0;
        }
        return;
    }

    if (Strategy::hasChildren(*m_anchorNode)) {
        m_anchorNode = Strategy::lastChild(*m_anchorNode);
        m_offsetInAnchor = Strategy::hasChildren(*m_anchorNode)? 0 : Strategy::lastOffsetForEditing(m_anchorNode);
    } else {
        if (m_offsetInAnchor) {
            m_offsetInAnchor = PositionAlgorithm<Strategy>::uncheckedPreviousOffset(m_anchorNode, m_offsetInAnchor);
        } else {
            m_nodeAfterPositionInAnchor = m_anchorNode;
            m_anchorNode = Strategy::parent(*m_anchorNode);
        }
    }
}

template <typename Strategy>
bool PositionIteratorAlgorithm<Strategy>::atStart() const
{
    if (!m_anchorNode)
        return true;
    if (Strategy::parent(*m_anchorNode))
        return false;
    return (!Strategy::hasChildren(*m_anchorNode) && !m_offsetInAnchor) || (m_nodeAfterPositionInAnchor && !Strategy::previousSibling(*m_nodeAfterPositionInAnchor));
}

template <typename Strategy>
bool PositionIteratorAlgorithm<Strategy>::atEnd() const
{
    if (!m_anchorNode)
        return true;
    if (m_nodeAfterPositionInAnchor)
        return false;
    return !Strategy::parent(*m_anchorNode) && (Strategy::hasChildren(*m_anchorNode) || m_offsetInAnchor >= Strategy::lastOffsetForEditing(m_anchorNode));
}

template <typename Strategy>
bool PositionIteratorAlgorithm<Strategy>::atStartOfNode() const
{
    if (!m_anchorNode)
        return true;
    if (!m_nodeAfterPositionInAnchor)
        return !Strategy::hasChildren(*m_anchorNode) && !m_offsetInAnchor;
    return !Strategy::previousSibling(*m_nodeAfterPositionInAnchor);
}

template <typename Strategy>
bool PositionIteratorAlgorithm<Strategy>::atEndOfNode() const
{
    if (!m_anchorNode)
        return true;
    if (m_nodeAfterPositionInAnchor)
        return false;
    return Strategy::hasChildren(*m_anchorNode) || m_offsetInAnchor >= Strategy::lastOffsetForEditing(m_anchorNode);
}

template <typename Strategy>
bool PositionIteratorAlgorithm<Strategy>::isCandidate() const
{
    if (!m_anchorNode)
        return false;

    LayoutObject* layoutObject = m_anchorNode->layoutObject();
    if (!layoutObject)
        return false;

    if (layoutObject->style()->visibility() != VISIBLE)
        return false;

    if (layoutObject->isBR()) {
        // For br element, the condition
        // |(!Strategy::hasChildren(*m_anchorNode) ||
        //   m_nodeAfterPositionInAnchor)| corresponds to the condition
        // |m_anchorType != PositionAnchorType::AfterAnchor| in
        // |Position.isCandaite|. Both conditions say this position is not in
        // tail of the element. If conditions lose consistency,
        // VisiblePosition::canonicalPosition will fail on
        // |canonicalizeCandidate(previousCandidate(position))|,
        // because previousCandidate returns a Position converted from
        // a "Candidate" PositionIterator and cannonicalizeCandidate(Position)
        // assumes the Position is "Candidate".
        return !m_offsetInAnchor && (!Strategy::hasChildren(*m_anchorNode) || m_nodeAfterPositionInAnchor) && !PositionAlgorithm<Strategy>::nodeIsUserSelectNone(Strategy::parent(*m_anchorNode));
    }
    if (layoutObject->isText())
        return !PositionAlgorithm<Strategy>::nodeIsUserSelectNone(m_anchorNode) && PositionAlgorithm<Strategy>(*this).inRenderedText();

    if (layoutObject->isSVG()) {
        // We don't consider SVG elements are contenteditable except for
        // associated layoutObject returns isText() true, e.g. LayoutSVGInlineText.
        return false;
    }

    if (isRenderedHTMLTableElement(m_anchorNode) || editingIgnoresContent(m_anchorNode))
        return (atStartOfNode() || atEndOfNode()) && !PositionAlgorithm<Strategy>::nodeIsUserSelectNone(Strategy::parent(*m_anchorNode));

    if (!isHTMLHtmlElement(*m_anchorNode) && layoutObject->isLayoutBlockFlow()) {
        if (toLayoutBlock(layoutObject)->logicalHeight() || isHTMLBodyElement(*m_anchorNode)) {
            if (!PositionAlgorithm<Strategy>::hasRenderedNonAnonymousDescendantsWithHeight(layoutObject))
                return atStartOfNode() && !PositionAlgorithm<Strategy>::nodeIsUserSelectNone(m_anchorNode);
            return m_anchorNode->hasEditableStyle() && !PositionAlgorithm<Strategy>::nodeIsUserSelectNone(m_anchorNode) && PositionAlgorithm<Strategy>(*this).atEditingBoundary();
        }
    }

    return false;
}

template class PositionIteratorAlgorithm<EditingStrategy>;
template class PositionIteratorAlgorithm<EditingInComposedTreeStrategy>;

} // namespace blink
