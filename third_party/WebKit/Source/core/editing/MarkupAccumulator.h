/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef MarkupAccumulator_h
#define MarkupAccumulator_h

#include "core/editing/EditingStrategy.h"
#include "core/editing/MarkupFormatter.h"
#include "core/editing/markup.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

class Attribute;
class DocumentType;
class Element;
class Node;

class MarkupAccumulator {
    WTF_MAKE_NONCOPYABLE(MarkupAccumulator);
    STACK_ALLOCATED();
public:
    MarkupAccumulator(EAbsoluteURLs, SerializationType = SerializationType::AsOwnerDocument);
    virtual ~MarkupAccumulator();

    void appendString(const String&);
    virtual void appendStartTag(Node&, Namespaces* = nullptr);
    virtual void appendEndTag(const Element&);
    void appendStartMarkup(StringBuilder&, Node&, Namespaces*);
    void appendEndMarkup(StringBuilder&, const Element&);

    bool serializeAsHTMLDocument(const Node&) const;
    String toString() { return m_markup.toString(); }

    virtual void appendCustomAttributes(StringBuilder&, const Element&, Namespaces*);

    virtual void appendText(StringBuilder&, Text&);
    virtual bool shouldIgnoreAttribute(const Attribute&);
    virtual void appendElement(StringBuilder&, Element&, Namespaces*);
    void appendOpenTag(StringBuilder&, const Element&, Namespaces*);
    void appendCloseTag(StringBuilder&, const Element&);
    virtual void appendAttribute(StringBuilder&, const Element&, const Attribute&, Namespaces*);

    EntityMask entityMaskForText(const Text&) const;

private:
    MarkupFormatter m_formatter;
    StringBuilder m_markup;
};

template<typename Strategy>
String serializeNodes(MarkupAccumulator&, Node&, EChildrenOnly);

extern template String serializeNodes<EditingStrategy>(MarkupAccumulator&, Node&, EChildrenOnly);

}

#endif
