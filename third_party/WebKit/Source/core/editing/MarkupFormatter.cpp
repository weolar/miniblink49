/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2009, 2010 Google Inc. All rights reserved.
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
#include "core/editing/MarkupFormatter.h"

#include "core/HTMLNames.h"
#include "core/XLinkNames.h"
#include "core/XMLNSNames.h"
#include "core/XMLNames.h"
#include "core/dom/CDATASection.h"
#include "core/dom/Comment.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentType.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/editing/Editor.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

using namespace HTMLNames;

struct EntityDescription {
    UChar entity;
    const CString& reference;
    EntityMask mask;
};

template <typename CharType>
static inline void appendCharactersReplacingEntitiesInternal(StringBuilder& result, CharType* text, unsigned length, const EntityDescription entityMaps[], unsigned entityMapsCount, EntityMask entityMask)
{
    unsigned positionAfterLastEntity = 0;
    for (unsigned i = 0; i < length; ++i) {
        for (unsigned entityIndex = 0; entityIndex < entityMapsCount; ++entityIndex) {
            if (text[i] == entityMaps[entityIndex].entity && entityMaps[entityIndex].mask & entityMask) {
                result.append(text + positionAfterLastEntity, i - positionAfterLastEntity);
                const CString& replacement = entityMaps[entityIndex].reference;
                result.append(replacement.data(), replacement.length());
                positionAfterLastEntity = i + 1;
                break;
            }
        }
    }
    result.append(text + positionAfterLastEntity, length - positionAfterLastEntity);
}

void MarkupFormatter::appendCharactersReplacingEntities(StringBuilder& result, const String& source, unsigned offset, unsigned length, EntityMask entityMask)
{
    DEFINE_STATIC_LOCAL(const CString, ampReference, ("&amp;"));
    DEFINE_STATIC_LOCAL(const CString, ltReference, ("&lt;"));
    DEFINE_STATIC_LOCAL(const CString, gtReference, ("&gt;"));
    DEFINE_STATIC_LOCAL(const CString, quotReference, ("&quot;"));
    DEFINE_STATIC_LOCAL(const CString, nbspReference, ("&nbsp;"));

    static const EntityDescription entityMaps[] = {
        { '&', ampReference, EntityAmp },
        { '<', ltReference, EntityLt },
        { '>', gtReference, EntityGt },
        { '"', quotReference, EntityQuot },
        { noBreakSpaceCharacter, nbspReference, EntityNbsp },
    };

    if (!(offset + length))
        return;

    ASSERT(offset + length <= source.length());
    if (source.is8Bit())
        appendCharactersReplacingEntitiesInternal(result, source.characters8() + offset, length, entityMaps, WTF_ARRAY_LENGTH(entityMaps), entityMask);
    else
        appendCharactersReplacingEntitiesInternal(result, source.characters16() + offset, length, entityMaps, WTF_ARRAY_LENGTH(entityMaps), entityMask);
}

MarkupFormatter::MarkupFormatter(EAbsoluteURLs resolveUrlsMethod, SerializationType serializationType)
    : m_resolveURLsMethod(resolveUrlsMethod)
    , m_serializationType(serializationType)
{
}

MarkupFormatter::~MarkupFormatter()
{
}

String MarkupFormatter::resolveURLIfNeeded(const Element& element, const String& urlString) const
{
    switch (m_resolveURLsMethod) {
    case ResolveAllURLs:
        return element.document().completeURL(urlString).string();

    case ResolveNonLocalURLs:
        if (!element.document().url().isLocalFile())
            return element.document().completeURL(urlString).string();
        break;

    case DoNotResolveURLs:
        break;
    }
    return urlString;
}

void MarkupFormatter::appendStartMarkup(StringBuilder& result, const Node& node, Namespaces* namespaces)
{
    switch (node.nodeType()) {
    case Node::TEXT_NODE:
        ASSERT_NOT_REACHED();
        break;
    case Node::COMMENT_NODE:
        appendComment(result, toComment(node).data());
        break;
    case Node::DOCUMENT_NODE:
        appendXMLDeclaration(result, toDocument(node));
        break;
    case Node::DOCUMENT_FRAGMENT_NODE:
        break;
    case Node::DOCUMENT_TYPE_NODE:
        appendDocumentType(result, toDocumentType(node));
        break;
    case Node::PROCESSING_INSTRUCTION_NODE:
        appendProcessingInstruction(result, toProcessingInstruction(node).target(), toProcessingInstruction(node).data());
        break;
    case Node::ELEMENT_NODE:
        ASSERT_NOT_REACHED();
        break;
    case Node::CDATA_SECTION_NODE:
        appendCDATASection(result, toCDATASection(node).data());
        break;
    case Node::ATTRIBUTE_NODE:
        ASSERT_NOT_REACHED();
        break;
    }
}

static bool elementCannotHaveEndTag(const Node& node)
{
    if (!node.isHTMLElement())
        return false;

    // FIXME: ieForbidsInsertHTML may not be the right function to call here
    // ieForbidsInsertHTML is used to disallow setting innerHTML/outerHTML
    // or createContextualFragment.  It does not necessarily align with
    // which elements should be serialized w/o end tags.
    return toHTMLElement(node).ieForbidsInsertHTML();
}

void MarkupFormatter::appendEndMarkup(StringBuilder& result, const Element& element)
{
    if (shouldSelfClose(element) || (!element.hasChildren() && elementCannotHaveEndTag(element)))
        return;

    result.appendLiteral("</");
    result.append(element.tagQName().toString());
    result.append('>');
}

void MarkupFormatter::appendAttributeValue(StringBuilder& result, const String& attribute, bool documentIsHTML)
{
    appendCharactersReplacingEntities(result, attribute, 0, attribute.length(),
        documentIsHTML ? EntityMaskInHTMLAttributeValue : EntityMaskInAttributeValue);
}

void MarkupFormatter::appendQuotedURLAttributeValue(StringBuilder& result, const Element& element, const Attribute& attribute)
{
    ASSERT(element.isURLAttribute(attribute));
    const String resolvedURLString = resolveURLIfNeeded(element, attribute.value());
    UChar quoteChar = '"';
    String strippedURLString = resolvedURLString.stripWhiteSpace();
    if (protocolIsJavaScript(strippedURLString)) {
        // minimal escaping for javascript urls
        if (strippedURLString.contains('&'))
            strippedURLString.replaceWithLiteral('&', "&amp;");

        if (strippedURLString.contains('"')) {
            if (strippedURLString.contains('\''))
                strippedURLString.replaceWithLiteral('"', "&quot;");
            else
                quoteChar = '\'';
        }
        result.append(quoteChar);
        result.append(strippedURLString);
        result.append(quoteChar);
        return;
    }

    // FIXME: This does not fully match other browsers. Firefox percent-escapes non-ASCII characters for innerHTML.
    result.append(quoteChar);
    appendAttributeValue(result, resolvedURLString, false);
    result.append(quoteChar);
}

void MarkupFormatter::appendNamespace(StringBuilder& result, const AtomicString& prefix, const AtomicString& namespaceURI, Namespaces& namespaces)
{
    if (namespaceURI.isEmpty())
        return;

    const AtomicString& lookupKey = (!prefix) ? emptyAtom : prefix;
    AtomicString foundURI = namespaces.get(lookupKey);
    if (foundURI != namespaceURI) {
        namespaces.set(lookupKey, namespaceURI);
        result.append(' ');
        result.append(xmlnsAtom.string());
        if (!prefix.isEmpty()) {
            result.append(':');
            result.append(prefix);
        }

        result.appendLiteral("=\"");
        appendAttributeValue(result, namespaceURI, false);
        result.append('"');
    }
}

void MarkupFormatter::appendText(StringBuilder& result, Text& text)
{
    const String& str = text.data();
    appendCharactersReplacingEntities(result, str, 0, str.length(), entityMaskForText(text));
}

void MarkupFormatter::appendComment(StringBuilder& result, const String& comment)
{
    // FIXME: Comment content is not escaped, but XMLSerializer (and possibly other callers) should raise an exception if it includes "-->".
    result.appendLiteral("<!--");
    result.append(comment);
    result.appendLiteral("-->");
}

void MarkupFormatter::appendXMLDeclaration(StringBuilder& result, const Document& document)
{
    if (!document.hasXMLDeclaration())
        return;

    result.appendLiteral("<?xml version=\"");
    result.append(document.xmlVersion());
    const String& encoding = document.xmlEncoding();
    if (!encoding.isEmpty()) {
        result.appendLiteral("\" encoding=\"");
        result.append(encoding);
    }
    if (document.xmlStandaloneStatus() != Document::StandaloneUnspecified) {
        result.appendLiteral("\" standalone=\"");
        if (document.xmlStandalone())
            result.appendLiteral("yes");
        else
            result.appendLiteral("no");
    }

    result.appendLiteral("\"?>");
}

void MarkupFormatter::appendDocumentType(StringBuilder& result, const DocumentType& n)
{
    if (n.name().isEmpty())
        return;

    result.appendLiteral("<!DOCTYPE ");
    result.append(n.name());
    if (!n.publicId().isEmpty()) {
        result.appendLiteral(" PUBLIC \"");
        result.append(n.publicId());
        result.append('"');
        if (!n.systemId().isEmpty()) {
            result.appendLiteral(" \"");
            result.append(n.systemId());
            result.append('"');
        }
    } else if (!n.systemId().isEmpty()) {
        result.appendLiteral(" SYSTEM \"");
        result.append(n.systemId());
        result.append('"');
    }
    result.append('>');
}

void MarkupFormatter::appendProcessingInstruction(StringBuilder& result, const String& target, const String& data)
{
    // FIXME: PI data is not escaped, but XMLSerializer (and possibly other callers) this should raise an exception if it includes "?>".
    result.appendLiteral("<?");
    result.append(target);
    result.append(' ');
    result.append(data);
    result.appendLiteral("?>");
}

void MarkupFormatter::appendOpenTag(StringBuilder& result, const Element& element, Namespaces* namespaces)
{
    result.append('<');
    result.append(element.tagQName().toString());
    if (!serializeAsHTMLDocument(element) && namespaces && shouldAddNamespaceElement(element, *namespaces))
        appendNamespace(result, element.prefix(), element.namespaceURI(), *namespaces);
}

void MarkupFormatter::appendCloseTag(StringBuilder& result, const Element& element)
{
    if (shouldSelfClose(element)) {
        if (element.isHTMLElement())
            result.append(' '); // XHTML 1.0 <-> HTML compatibility.
        result.append('/');
    }
    result.append('>');
}

static inline bool attributeIsInSerializedNamespace(const Attribute& attribute)
{
    return attribute.namespaceURI() == XMLNames::xmlNamespaceURI
        || attribute.namespaceURI() == XLinkNames::xlinkNamespaceURI
        || attribute.namespaceURI() == XMLNSNames::xmlnsNamespaceURI;
}

void MarkupFormatter::appendAttribute(StringBuilder& result, const Element& element, const Attribute& attribute, Namespaces* namespaces)
{
    bool documentIsHTML = serializeAsHTMLDocument(element);

    QualifiedName prefixedName = attribute.name();
    if (documentIsHTML && !attributeIsInSerializedNamespace(attribute)) {
        result.append(' ');
        result.append(attribute.name().localName());
    } else {
        if (attribute.namespaceURI() == XMLNSNames::xmlnsNamespaceURI) {
            if (!attribute.prefix() && attribute.localName() != xmlnsAtom)
                prefixedName.setPrefix(xmlnsAtom);
            if (namespaces) { // Account for the namespace attribute we're about to append.
                const AtomicString& lookupKey = (!attribute.prefix()) ? emptyAtom : attribute.localName();
                namespaces->set(lookupKey, attribute.value());
            }
        } else if (attribute.namespaceURI() == XMLNames::xmlNamespaceURI) {
            if (!attribute.prefix())
                prefixedName.setPrefix(xmlAtom);
        } else {
            if (attribute.namespaceURI() == XLinkNames::xlinkNamespaceURI) {
                if (!attribute.prefix())
                    prefixedName.setPrefix(xlinkAtom);
            }

            if (namespaces && shouldAddNamespaceAttribute(attribute, element)) {
                if (!prefixedName.prefix()) {
                    // This behavior is in process of being standardized. See crbug.com/248044 and https://www.w3.org/Bugs/Public/show_bug.cgi?id=24208
                    String prefixPrefix("ns", 2);
                    for (unsigned i = attribute.namespaceURI().impl()->existingHash(); ; ++i) {
                        AtomicString newPrefix(String(prefixPrefix + String::number(i)));
                        AtomicString foundURI = namespaces->get(newPrefix);
                        if (foundURI == attribute.namespaceURI() || foundURI == nullAtom) {
                            // We already generated a prefix for this namespace.
                            prefixedName.setPrefix(newPrefix);
                            break;
                        }
                    }
                }
                ASSERT(prefixedName.prefix());
                appendNamespace(result, prefixedName.prefix(), attribute.namespaceURI(), *namespaces);
            }
        }
        result.append(' ');
        result.append(prefixedName.toString());
    }

    result.append('=');

    if (element.isURLAttribute(attribute)) {
        appendQuotedURLAttributeValue(result, element, attribute);
    } else {
        result.append('"');
        appendAttributeValue(result, attribute.value(), documentIsHTML);
        result.append('"');
    }
}

void MarkupFormatter::appendCDATASection(StringBuilder& result, const String& section)
{
    // FIXME: CDATA content is not escaped, but XMLSerializer (and possibly other callers) should raise an exception if it includes "]]>".
    result.appendLiteral("<![CDATA[");
    result.append(section);
    result.appendLiteral("]]>");
}

bool MarkupFormatter::shouldAddNamespaceElement(const Element& element, Namespaces& namespaces) const
{
    // Don't add namespace attribute if it is already defined for this elem.
    const AtomicString& prefix = element.prefix();
    if (prefix.isEmpty()) {
        if (element.hasAttribute(xmlnsAtom)) {
            namespaces.set(emptyAtom, element.namespaceURI());
            return false;
        }
        return true;
    }

    return !element.hasAttribute(WTF::xmlnsWithColon + prefix);
}

bool MarkupFormatter::shouldAddNamespaceAttribute(const Attribute& attribute, const Element& element) const
{
    // xmlns and xmlns:prefix attributes should be handled by another branch in appendAttribute.
    ASSERT(attribute.namespaceURI() != XMLNSNames::xmlnsNamespaceURI);

    // Attributes are in the null namespace by default.
    if (!attribute.namespaceURI())
        return false;

    // Attributes without a prefix will need one generated for them, and an xmlns attribute for that prefix.
    if (!attribute.prefix())
        return true;

    return !element.hasAttribute(WTF::xmlnsWithColon + attribute.prefix());
}

EntityMask MarkupFormatter::entityMaskForText(const Text& text) const
{
    if (!serializeAsHTMLDocument(text))
        return EntityMaskInPCDATA;

    // TODO(hajimehoshi): We need to switch EditingStrategy.
    const QualifiedName* parentName = nullptr;
    if (text.parentElement())
        parentName = &(text.parentElement())->tagQName();

    if (parentName && (*parentName == scriptTag || *parentName == styleTag || *parentName == xmpTag))
        return EntityMaskInCDATA;
    return EntityMaskInHTMLPCDATA;
}

// Rules of self-closure
// 1. No elements in HTML documents use the self-closing syntax.
// 2. Elements w/ children never self-close because they use a separate end tag.
// 3. HTML elements which do not have a "forbidden" end tag will close with a separate end tag.
// 4. Other elements self-close.
bool MarkupFormatter::shouldSelfClose(const Element& element) const
{
    if (serializeAsHTMLDocument(element))
        return false;
    if (element.hasChildren())
        return false;
    if (element.isHTMLElement() && !elementCannotHaveEndTag(element))
        return false;
    return true;
}

bool MarkupFormatter::serializeAsHTMLDocument(const Node& node) const
{
    if (m_serializationType == SerializationType::ForcedXML)
        return false;
    return node.document().isHTMLDocument();
}

}
