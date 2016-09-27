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

#ifndef MarkupFormatter_h
#define MarkupFormatter_h

#include "core/editing/EditingStrategy.h"
#include "core/editing/markup.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

class Attribute;
class DocumentType;
class Element;
class Node;

typedef HashMap<AtomicString, AtomicString> Namespaces;

enum EntityMask {
    EntityAmp = 0x0001,
    EntityLt = 0x0002,
    EntityGt = 0x0004,
    EntityQuot = 0x0008,
    EntityNbsp = 0x0010,

    // Non-breaking space needs to be escaped in innerHTML for compatibility reason. See http://trac.webkit.org/changeset/32879
    // However, we cannot do this in a XML document because it does not have the entity reference defined (See the bug 19215).
    EntityMaskInCDATA = 0,
    EntityMaskInPCDATA = EntityAmp | EntityLt | EntityGt,
    EntityMaskInHTMLPCDATA = EntityMaskInPCDATA | EntityNbsp,
    EntityMaskInAttributeValue = EntityAmp | EntityQuot | EntityLt | EntityGt,
    EntityMaskInHTMLAttributeValue = EntityAmp | EntityQuot | EntityNbsp,
};

enum class SerializationType {
    AsOwnerDocument,
    ForcedXML
};

class MarkupFormatter final {
    WTF_MAKE_NONCOPYABLE(MarkupFormatter);
    STACK_ALLOCATED();
public:
    static void appendAttributeValue(StringBuilder&, const String&, bool);
    static void appendCDATASection(StringBuilder&, const String&);
    static void appendCharactersReplacingEntities(StringBuilder&, const String&, unsigned, unsigned, EntityMask);
    static void appendComment(StringBuilder&, const String&);
    static void appendDocumentType(StringBuilder&, const DocumentType&);
    static void appendNamespace(StringBuilder&, const AtomicString& prefix, const AtomicString& namespaceURI, Namespaces&);
    static void appendProcessingInstruction(StringBuilder&, const String& target, const String& data);
    static void appendXMLDeclaration(StringBuilder&, const Document&);

    MarkupFormatter(EAbsoluteURLs, SerializationType = SerializationType::AsOwnerDocument);
    ~MarkupFormatter();

    void appendStartMarkup(StringBuilder&, const Node&, Namespaces*);
    void appendEndMarkup(StringBuilder&, const Element&);

    bool serializeAsHTMLDocument(const Node&) const;

    void appendText(StringBuilder&, Text&);
    void appendOpenTag(StringBuilder&, const Element&, Namespaces*);
    void appendCloseTag(StringBuilder&, const Element&);
    void appendAttribute(StringBuilder&, const Element&, const Attribute&, Namespaces*);

    bool shouldAddNamespaceElement(const Element&, Namespaces&) const;
    bool shouldAddNamespaceAttribute(const Attribute&, const Element&) const;
    EntityMask entityMaskForText(const Text&) const;
    bool shouldSelfClose(const Element&) const;

private:
    String resolveURLIfNeeded(const Element&, const String&) const;
    void appendQuotedURLAttributeValue(StringBuilder&, const Element&, const Attribute&);

    const EAbsoluteURLs m_resolveURLsMethod;
    SerializationType m_serializationType;
};

}

#endif
