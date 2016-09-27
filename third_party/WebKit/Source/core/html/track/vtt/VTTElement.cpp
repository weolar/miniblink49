/*
 * Copyright (C) 2013 Apple Inc.  All rights reserved.
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
#include "core/html/track/vtt/VTTElement.h"

#include "core/HTMLElementFactory.h"

namespace blink {

static const QualifiedName& nodeTypeToTagName(VTTNodeType nodeType)
{
    DEFINE_STATIC_LOCAL(QualifiedName, cTag, (nullAtom, "c", nullAtom));
    DEFINE_STATIC_LOCAL(QualifiedName, vTag, (nullAtom, "v", nullAtom));
    DEFINE_STATIC_LOCAL(QualifiedName, langTag, (nullAtom, "lang", nullAtom));
    DEFINE_STATIC_LOCAL(QualifiedName, bTag, (nullAtom, "b", nullAtom));
    DEFINE_STATIC_LOCAL(QualifiedName, uTag, (nullAtom, "u", nullAtom));
    DEFINE_STATIC_LOCAL(QualifiedName, iTag, (nullAtom, "i", nullAtom));
    DEFINE_STATIC_LOCAL(QualifiedName, rubyTag, (nullAtom, "ruby", nullAtom));
    DEFINE_STATIC_LOCAL(QualifiedName, rtTag, (nullAtom, "rt", nullAtom));
    switch (nodeType) {
    case VTTNodeTypeClass:
        return cTag;
    case VTTNodeTypeItalic:
        return iTag;
    case VTTNodeTypeLanguage:
        return langTag;
    case VTTNodeTypeBold:
        return bTag;
    case VTTNodeTypeUnderline:
        return uTag;
    case VTTNodeTypeRuby:
        return rubyTag;
    case VTTNodeTypeRubyText:
        return rtTag;
    case VTTNodeTypeVoice:
        return vTag;
    case VTTNodeTypeNone:
    default:
        ASSERT_NOT_REACHED();
        return cTag; // Make the compiler happy.
    }
}

VTTElement::VTTElement(VTTNodeType nodeType, Document* document)
    : Element(nodeTypeToTagName(nodeType), document, CreateElement)
    , m_isPastNode(0)
    , m_webVTTNodeType(nodeType)
{
}

PassRefPtrWillBeRawPtr<VTTElement> VTTElement::create(VTTNodeType nodeType, Document* document)
{
    return adoptRefWillBeNoop(new VTTElement(nodeType, document));
}

PassRefPtrWillBeRawPtr<Element> VTTElement::cloneElementWithoutAttributesAndChildren()
{
    RefPtrWillBeRawPtr<VTTElement> clone = create(static_cast<VTTNodeType>(m_webVTTNodeType), &document());
    clone->setLanguage(m_language);
    return clone.release();
}

PassRefPtrWillBeRawPtr<HTMLElement> VTTElement::createEquivalentHTMLElement(Document& document)
{
    RefPtrWillBeRawPtr<HTMLElement> htmlElement = nullptr;
    switch (m_webVTTNodeType) {
    case VTTNodeTypeClass:
    case VTTNodeTypeLanguage:
    case VTTNodeTypeVoice:
        htmlElement = HTMLElementFactory::createHTMLElement(HTMLNames::spanTag.localName(), document);
        htmlElement.get()->setAttribute(HTMLNames::titleAttr, getAttribute(voiceAttributeName()));
        htmlElement.get()->setAttribute(HTMLNames::langAttr, getAttribute(langAttributeName()));
        break;
    case VTTNodeTypeItalic:
        htmlElement = HTMLElementFactory::createHTMLElement(HTMLNames::iTag.localName(), document);
        break;
    case VTTNodeTypeBold:
        htmlElement = HTMLElementFactory::createHTMLElement(HTMLNames::bTag.localName(), document);
        break;
    case VTTNodeTypeUnderline:
        htmlElement = HTMLElementFactory::createHTMLElement(HTMLNames::uTag.localName(), document);
        break;
    case VTTNodeTypeRuby:
        htmlElement = HTMLElementFactory::createHTMLElement(HTMLNames::rubyTag.localName(), document);
        break;
    case VTTNodeTypeRubyText:
        htmlElement = HTMLElementFactory::createHTMLElement(HTMLNames::rtTag.localName(), document);
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    htmlElement.get()->setAttribute(HTMLNames::classAttr, getAttribute(HTMLNames::classAttr));
    return htmlElement;
}

void VTTElement::setIsPastNode(bool isPastNode)
{
    if (!!m_isPastNode == isPastNode)
        return;

    m_isPastNode = isPastNode;
    setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::createWithExtraData(StyleChangeReason::PseudoClass, StyleChangeExtraData::Past));
}

} // namespace blink
