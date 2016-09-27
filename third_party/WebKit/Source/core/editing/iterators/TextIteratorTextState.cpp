/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2005 Alexey Proskuryakov.
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
#include "core/editing/iterators/TextIteratorTextState.h"

namespace blink {

TextIteratorTextState::TextIteratorTextState(bool emitsOriginalText)
    : m_textLength(0)
    , m_singleCharacterBuffer(0)
    , m_positionNode(nullptr)
    , m_positionStartOffset(0)
    , m_positionEndOffset(0)
    , m_hasEmitted(false)
    , m_lastCharacter(0)
    , m_emitsOriginalText(emitsOriginalText)
    { }

UChar TextIteratorTextState::characterAt(unsigned index) const
{
    ASSERT_WITH_SECURITY_IMPLICATION(index < static_cast<unsigned>(length()));
    if (!(index < static_cast<unsigned>(length())))
        return 0;

    if (m_singleCharacterBuffer) {
        ASSERT(!index);
        ASSERT(length() == 1);
        return m_singleCharacterBuffer;
    }

    return string()[positionStartOffset() + index];
}

String TextIteratorTextState::substring(unsigned position, unsigned length) const
{
    ASSERT_WITH_SECURITY_IMPLICATION(position <= static_cast<unsigned>(this->length()));
    ASSERT_WITH_SECURITY_IMPLICATION(position + length <= static_cast<unsigned>(this->length()));
    if (!length)
        return emptyString();
    if (m_singleCharacterBuffer) {
        ASSERT(!position);
        ASSERT(length == 1);
        return String(&m_singleCharacterBuffer, 1);
    }
    return string().substring(positionStartOffset() + position, length);
}

void TextIteratorTextState::appendTextToStringBuilder(StringBuilder& builder, unsigned position, unsigned maxLength) const
{
    unsigned lengthToAppend = std::min(static_cast<unsigned>(length()) - position, maxLength);
    if (!lengthToAppend)
        return;
    if (m_singleCharacterBuffer) {
        ASSERT(!position);
        builder.append(m_singleCharacterBuffer);
    } else {
        builder.append(string(), positionStartOffset() + position, lengthToAppend);
    }
}

void TextIteratorTextState::updateForReplacedElement(Node* baseNode)
{
    m_hasEmitted = true;
    m_positionNode = baseNode->parentNode();
    m_positionOffsetBaseNode = baseNode;
    m_positionStartOffset = 0;
    m_positionEndOffset = 1;
    m_singleCharacterBuffer = 0;

    m_textLength = 0;
    m_lastCharacter = 0;
}

void TextIteratorTextState::emitAltText(Node* node)
{
    m_text = toHTMLElement(node)->altText();
    m_textLength = m_text.length();
    m_lastCharacter = m_textLength ? m_text[m_textLength - 1] : 0;
}

void TextIteratorTextState::flushPositionOffsets() const
{
    if (!m_positionOffsetBaseNode)
        return;
    int index = m_positionOffsetBaseNode->nodeIndex();
    m_positionStartOffset += index;
    m_positionEndOffset += index;
    m_positionOffsetBaseNode = nullptr;
}

void TextIteratorTextState::emitCharacter(UChar c, Node* textNode, Node* offsetBaseNode, int textStartOffset, int textEndOffset)
{
    ASSERT(textNode);
    m_hasEmitted = true;

    // Remember information with which to construct the TextIterator::range().
    // NOTE: textNode is often not a text node, so the range will specify child nodes of positionNode
    m_positionNode = textNode;
    m_positionOffsetBaseNode = offsetBaseNode;
    m_positionStartOffset = textStartOffset;
    m_positionEndOffset = textEndOffset;

    // remember information with which to construct the TextIterator::characters() and length()
    m_singleCharacterBuffer = c;
    ASSERT(m_singleCharacterBuffer);
    m_textLength = 1;

    // remember some iteration state
    m_lastCharacter = c;
}

void TextIteratorTextState::emitText(Node* textNode, LayoutText* layoutObject, int textStartOffset, int textEndOffset)
{
    ASSERT(textNode);
    m_text = m_emitsOriginalText ? layoutObject->originalText() : layoutObject->text();
    ASSERT(!m_text.isEmpty());
    ASSERT(0 <= textStartOffset && textStartOffset < static_cast<int>(m_text.length()));
    ASSERT(0 <= textEndOffset && textEndOffset <= static_cast<int>(m_text.length()));
    ASSERT(textStartOffset <= textEndOffset);

    m_positionNode = textNode;
    m_positionOffsetBaseNode = nullptr;
    m_positionStartOffset = textStartOffset;
    m_positionEndOffset = textEndOffset;
    m_singleCharacterBuffer = 0;
    m_textLength = textEndOffset - textStartOffset;
    m_lastCharacter = m_text[textEndOffset - 1];

    m_hasEmitted = true;
}

EphemeralRange TextIteratorTextState::range() const
{
    flushPositionOffsets();
    return EphemeralRange(Position(positionNode(), positionStartOffset()), Position(positionNode(), positionEndOffset()));
}

} // namespace blink
