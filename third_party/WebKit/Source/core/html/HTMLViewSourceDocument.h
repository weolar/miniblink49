/*
 * Copyright (C) 2006, 2008, 2009 Apple Inc. All rights reserved.
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
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HTMLViewSourceDocument_h
#define HTMLViewSourceDocument_h

#include "core/html/HTMLDocument.h"

namespace blink {

class HTMLTableCellElement;
class HTMLTableSectionElement;
class HTMLToken;

class HTMLViewSourceDocument final : public HTMLDocument {
public:
    enum SourceAnnotation {
        AnnotateSourceAsSafe,
        AnnotateSourceAsXSS
    };

    static PassRefPtrWillBeRawPtr<HTMLViewSourceDocument> create(const DocumentInit& initializer, const String& mimeType)
    {
        return adoptRefWillBeNoop(new HTMLViewSourceDocument(initializer, mimeType));
    }

    void addSource(const String&, HTMLToken&, SourceAnnotation);

    DECLARE_VIRTUAL_TRACE();

private:
    HTMLViewSourceDocument(const DocumentInit&, const String& mimeType);

    PassRefPtrWillBeRawPtr<DocumentParser> createParser() override;

    void processDoctypeToken(const String& source, HTMLToken&);
    void processEndOfFileToken(const String& source, HTMLToken&);
    void processTagToken(const String& source, HTMLToken&, SourceAnnotation);
    void processCommentToken(const String& source, HTMLToken&);
    void processCharacterToken(const String& source, HTMLToken&, SourceAnnotation);

    void createContainingTable();
    PassRefPtrWillBeRawPtr<Element> addSpanWithClassName(const AtomicString&);
    void addLine(const AtomicString& className);
    void finishLine();
    void addText(const String& text, const AtomicString& className, SourceAnnotation = AnnotateSourceAsSafe);
    int addRange(const String& source, int start, int end, const AtomicString& className, bool isLink = false, bool isAnchor = false, const AtomicString& link = nullAtom);
    void maybeAddSpanForAnnotation(SourceAnnotation);

    PassRefPtrWillBeRawPtr<Element> addLink(const AtomicString& url, bool isAnchor);
    PassRefPtrWillBeRawPtr<Element> addBase(const AtomicString& href);

    String m_type;
    RefPtrWillBeMember<Element> m_current;
    RefPtrWillBeMember<HTMLTableSectionElement> m_tbody;
    RefPtrWillBeMember<HTMLTableCellElement> m_td;
    int m_lineNumber;
};

}

#endif // HTMLViewSourceDocument_h
