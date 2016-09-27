/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009, 2010, 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011 Igalia S.L.
 * Copyright (C) 2011 Motorola Mobility. All rights reserved.
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
#include "core/editing/StyledMarkupAccumulator.h"

#include "core/css/StylePropertySet.h"
#include "core/dom/Text.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

namespace {

size_t totalLength(const Vector<String>& strings)
{
    size_t length = 0;
    for (const auto& string : strings)
        length += string.length();
    return length;
}

} // namespace

using namespace HTMLNames;

StyledMarkupAccumulator::StyledMarkupAccumulator(EAbsoluteURLs shouldResolveURLs, const TextOffset& start, const TextOffset& end, const PassRefPtrWillBeRawPtr<Document> document, EAnnotateForInterchange shouldAnnotate, ConvertBlocksToInlines convertBlocksToInlines)
    : m_formatter(shouldResolveURLs)
    , m_start(start)
    , m_end(end)
    , m_document(document)
    , m_shouldAnnotate(shouldAnnotate)
    , m_convertBlocksToInlines(convertBlocksToInlines)
{
}

void StyledMarkupAccumulator::appendEndTag(const Element& element)
{
    appendEndMarkup(m_result, element);
}

void StyledMarkupAccumulator::appendStartMarkup(Node& node)
{
    m_formatter.appendStartMarkup(m_result, node, nullptr);
}

void StyledMarkupAccumulator::appendEndMarkup(StringBuilder& result, const Element& element)
{
    m_formatter.appendEndMarkup(result, element);
}

void StyledMarkupAccumulator::appendText(Text& text)
{
    const String& str = text.data();
    unsigned length = str.length();
    unsigned start = 0;
    if (m_end.isNotNull()) {
        if (text == m_end.text())
            length = m_end.offset();
    }
    if (m_start.isNotNull()) {
        if (text == m_start.text()) {
            start = m_start.offset();
            length -= start;
        }
    }
    MarkupFormatter::appendCharactersReplacingEntities(m_result, str, start, length, m_formatter.entityMaskForText(text));
}

void StyledMarkupAccumulator::appendTextWithInlineStyle(Text& text, PassRefPtrWillBeRawPtr<EditingStyle> inlineStyle)
{
    if (inlineStyle) {
        // wrappingStyleForSerialization should have removed -webkit-text-decorations-in-effect
        ASSERT(propertyMissingOrEqualToNone(inlineStyle->style(), CSSPropertyWebkitTextDecorationsInEffect));
        ASSERT(m_document);

        m_result.appendLiteral("<span style=\"");
        MarkupFormatter::appendAttributeValue(m_result, inlineStyle->style()->asText(), m_document->isHTMLDocument());
        m_result.appendLiteral("\">");
    }
    if (!shouldAnnotate()) {
        appendText(text);
    } else {
        const bool useRenderedText = !enclosingElementWithTag(firstPositionInNode(&text), selectTag);
        String content = useRenderedText ? renderedText(text) : stringValueForRange(text);
        StringBuilder buffer;
        MarkupFormatter::appendCharactersReplacingEntities(buffer, content, 0, content.length(), EntityMaskInPCDATA);
        m_result.append(convertHTMLTextToInterchangeFormat(buffer.toString(), text));
    }
    if (inlineStyle)
        m_result.append("</span>");
}

void StyledMarkupAccumulator::appendElementWithInlineStyle(const Element& element, PassRefPtrWillBeRawPtr<EditingStyle> style)
{
    appendElementWithInlineStyle(m_result, element, style);
}

void StyledMarkupAccumulator::appendElementWithInlineStyle(StringBuilder& out, const Element& element, PassRefPtrWillBeRawPtr<EditingStyle> style)
{
    const bool documentIsHTML = element.document().isHTMLDocument();
    m_formatter.appendOpenTag(out, element, nullptr);
    AttributeCollection attributes = element.attributes();
    for (const auto& attribute : attributes) {
        // We'll handle the style attribute separately, below.
        if (attribute.name() == styleAttr)
            continue;
        m_formatter.appendAttribute(out, element, attribute, nullptr);
    }
    if (style && !style->isEmpty()) {
        out.appendLiteral(" style=\"");
        MarkupFormatter::appendAttributeValue(out, style->style()->asText(), documentIsHTML);
        out.append('\"');
    }
    m_formatter.appendCloseTag(out, element);
}

void StyledMarkupAccumulator::appendElement(const Element& element)
{
    appendElement(m_result, element);
}

void StyledMarkupAccumulator::appendElement(StringBuilder& out, const Element& element)
{
    m_formatter.appendOpenTag(out, element, nullptr);
    AttributeCollection attributes = element.attributes();
    for (const auto& attribute : attributes)
        m_formatter.appendAttribute(out, element, attribute, nullptr);
    m_formatter.appendCloseTag(out, element);
}

void StyledMarkupAccumulator::wrapWithStyleNode(StylePropertySet* style)
{
    // wrappingStyleForSerialization should have removed -webkit-text-decorations-in-effect
    ASSERT(propertyMissingOrEqualToNone(style, CSSPropertyWebkitTextDecorationsInEffect));
    ASSERT(m_document);

    StringBuilder openTag;
    openTag.appendLiteral("<div style=\"");
    MarkupFormatter::appendAttributeValue(openTag, style->asText(), m_document->isHTMLDocument());
    openTag.appendLiteral("\">");
    m_reversedPrecedingMarkup.append(openTag.toString());

    m_result.append("</div>");
}

String StyledMarkupAccumulator::takeResults()
{
    StringBuilder result;
    result.reserveCapacity(totalLength(m_reversedPrecedingMarkup) + m_result.length());

    for (size_t i = m_reversedPrecedingMarkup.size(); i > 0; --i)
        result.append(m_reversedPrecedingMarkup[i - 1]);

    result.append(m_result);

    // We remove '\0' characters because they are not visibly rendered to the user.
    return result.toString().replace(0, "");
}

String StyledMarkupAccumulator::renderedText(Text& textNode)
{
    int startOffset = 0;
    int endOffset = textNode.length();
    if (m_start.text() == textNode)
        startOffset = m_start.offset();
    if (m_end.text() == textNode)
        endOffset = m_end.offset();
    return plainText(Position(&textNode, startOffset), Position(&textNode, endOffset));
}

String StyledMarkupAccumulator::stringValueForRange(const Text& node)
{
    if (m_start.isNull())
        return node.data();

    String str = node.data();
    if (m_start.text() == node)
        str.truncate(m_end.offset());
    if (m_end.text() == node)
        str.remove(0, m_start.offset());
    return str;
}

bool StyledMarkupAccumulator::shouldAnnotate() const
{
    return m_shouldAnnotate == AnnotateForInterchange;
}

void StyledMarkupAccumulator::pushMarkup(const String& str)
{
    m_reversedPrecedingMarkup.append(str);
}

void StyledMarkupAccumulator::appendInterchangeNewline()
{
    DEFINE_STATIC_LOCAL(const String, interchangeNewlineString, ("<br class=\"" AppleInterchangeNewline "\">"));
    m_result.append(interchangeNewlineString);
}

} // namespace blink
