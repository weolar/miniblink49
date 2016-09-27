/*
 * Copyright (C) 2004, 2006, 2009 Apple Inc. All rights reserved.
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

#ifndef SimplifiedBackwardsTextIterator_h
#define SimplifiedBackwardsTextIterator_h

#include "core/dom/Position.h"
#include "core/editing/iterators/FullyClippedStateStack.h"
#include "core/editing/iterators/TextIteratorFlags.h"
#include "platform/heap/Heap.h"

namespace blink {

class LayoutText;
class LayoutTextFragment;

// Iterates through the DOM range, returning all the text, and 0-length boundaries
// at points where replaced elements break up the text flow. The text comes back in
// chunks so as to optimize for performance of the iteration.
template <typename Strategy>
class SimplifiedBackwardsTextIteratorAlgorithm {
    STACK_ALLOCATED();
public:
    SimplifiedBackwardsTextIteratorAlgorithm(const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end, TextIteratorBehaviorFlags = TextIteratorDefaultBehavior);

    bool atEnd() const { return !m_positionNode || m_shouldStop; }
    void advance();

    int length() const { return m_textLength; }

    Node* node() const { return m_node; }

    template<typename BufferType>
    void prependTextTo(BufferType& output)
    {
        if (!m_textLength)
            return;
        if (m_singleCharacterBuffer)
            output.prepend(&m_singleCharacterBuffer, 1);
        else
            m_textContainer.prependTo(output, m_textOffset, m_textLength);
    }

    Node* startContainer() const;
    int endOffset() const;
    PositionAlgorithm<Strategy> startPosition() const;
    PositionAlgorithm<Strategy> endPosition() const;

private:
    void init(Node* startNode, Node* endNode, int startOffset, int endOffset);
    void exitNode();
    bool handleTextNode();
    LayoutText* handleFirstLetter(int& startOffset, int& offsetInNode);
    bool handleReplacedElement();
    bool handleNonTextNode();
    void emitCharacter(UChar, Node*, int startOffset, int endOffset);
    bool advanceRespectingRange(Node*);

    // Current position, not necessarily of the text being returned, but position
    // as we walk through the DOM tree.
    RawPtrWillBeMember<Node> m_node;
    int m_offset;
    bool m_handledNode;
    bool m_handledChildren;
    FullyClippedStateStack m_fullyClippedStack;

    // End of the range.
    RawPtrWillBeMember<Node> m_startNode;
    int m_startOffset;
    // Start of the range.
    RawPtrWillBeMember<Node> m_endNode;
    int m_endOffset;

    // The current text and its position, in the form to be returned from the iterator.
    RawPtrWillBeMember<Node> m_positionNode;
    int m_positionStartOffset;
    int m_positionEndOffset;

    String m_textContainer; // We're interested in the range [m_textOffset, m_textOffset + m_textLength) of m_textContainer.
    int m_textOffset;
    int m_textLength;

    // Used for whitespace characters that aren't in the DOM, so we can point at them.
    UChar m_singleCharacterBuffer;

    // Whether m_node has advanced beyond the iteration range (i.e. m_startNode).
    bool m_havePassedStartNode;

    // Should handle first-letter layoutObject in the next call to handleTextNode.
    bool m_shouldHandleFirstLetter;

    // Used when the iteration should stop if form controls are reached.
    bool m_stopsOnFormControls;

    // Used when m_stopsOnFormControls is set to determine if the iterator should keep advancing.
    bool m_shouldStop;

    // Used in pasting inside password field.
    bool m_emitsOriginalText;
};

extern template class CORE_EXTERN_TEMPLATE_EXPORT SimplifiedBackwardsTextIteratorAlgorithm<EditingStrategy>;
using SimplifiedBackwardsTextIterator = SimplifiedBackwardsTextIteratorAlgorithm<EditingStrategy>;

} // namespace blink

#endif // SimplifiedBackwardsTextIterator_h
