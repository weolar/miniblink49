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

#ifndef StyledMarkupAccumulator_h
#define StyledMarkupAccumulator_h

#include "core/editing/EditingStyle.h"
#include "core/editing/MarkupFormatter.h"
#include "core/editing/TextOffset.h"
#include "wtf/Forward.h"

namespace blink {

class Document;
class StylePropertySet;
class Text;

class StyledMarkupAccumulator final {
    WTF_MAKE_NONCOPYABLE(StyledMarkupAccumulator);
    STACK_ALLOCATED();
public:
    StyledMarkupAccumulator(EAbsoluteURLs, const TextOffset& start, const TextOffset& end, const PassRefPtrWillBeRawPtr<Document>, EAnnotateForInterchange, ConvertBlocksToInlines);

    void appendEndTag(const Element&);
    void appendInterchangeNewline();

    void appendText(Text&);
    void appendTextWithInlineStyle(Text&, PassRefPtrWillBeRawPtr<EditingStyle>);

    void wrapWithStyleNode(StylePropertySet*);
    String takeResults();

    void pushMarkup(const String&);

    void appendElement(const Element&);
    void appendElement(StringBuilder&, const Element&);
    void appendElementWithInlineStyle(const Element&, PassRefPtrWillBeRawPtr<EditingStyle>);
    void appendElementWithInlineStyle(StringBuilder&, const Element&, PassRefPtrWillBeRawPtr<EditingStyle>);
    void appendStartMarkup(Node&);

    bool shouldAnnotate() const;
    bool convertBlocksToInlines() const { return m_convertBlocksToInlines == ConvertBlocksToInlines::Convert; }

private:
    String renderedText(Text&);
    String stringValueForRange(const Text&);

    void appendEndMarkup(StringBuilder&, const Element&);

    MarkupFormatter m_formatter;
    const TextOffset m_start;
    const TextOffset m_end;
    const RefPtrWillBeMember<Document> m_document;
    const EAnnotateForInterchange m_shouldAnnotate;
    StringBuilder m_result;
    Vector<String> m_reversedPrecedingMarkup;
    const ConvertBlocksToInlines m_convertBlocksToInlines;
};

} // namespace blink

#endif // StyledMarkupAccumulator_h
