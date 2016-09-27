/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#include "core/dom/StyleSheetCandidate.h"

#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/dom/StyleEngine.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/HTMLStyleElement.h"
#include "core/html/imports/HTMLImport.h"
#include "core/svg/SVGStyleElement.h"

namespace blink {

using namespace HTMLNames;

AtomicString StyleSheetCandidate::title() const
{
    return isElement() ? toElement(node()).fastGetAttribute(titleAttr) : nullAtom;
}

bool StyleSheetCandidate::isXSL() const
{
    return !node().document().isHTMLDocument() && m_type == Pi && toProcessingInstruction(node()).isXSL();
}

bool StyleSheetCandidate::isImport() const
{
    return m_type == HTMLLink && toHTMLLinkElement(node()).isImport();
}

bool StyleSheetCandidate::isCSSStyle() const
{
    return m_type == HTMLStyle || m_type == SVGStyle;
}

Document* StyleSheetCandidate::importedDocument() const
{
    ASSERT(isImport());
    return toHTMLLinkElement(node()).import();
}

bool StyleSheetCandidate::isAlternate() const
{
    if (!isElement())
        return false;
    return toElement(node()).getAttribute(relAttr).contains("alternate");
}

bool StyleSheetCandidate::isEnabledViaScript() const
{
    return isHTMLLink() && toHTMLLinkElement(node()).isEnabledViaScript();
}

bool StyleSheetCandidate::isEnabledAndLoading() const
{
    return isHTMLLink() && !toHTMLLinkElement(node()).isDisabled() && toHTMLLinkElement(node()).styleSheetIsLoading();
}

bool StyleSheetCandidate::hasPreferrableName(const String& currentPreferrableName) const
{
    ASSERT(isEnabledAndLoading() || sheet());
    return !isEnabledViaScript() && !title().isEmpty() && !isAlternate() && currentPreferrableName.isEmpty();
}

bool StyleSheetCandidate::canBeActivated(const String& currentPreferrableName) const
{
    StyleSheet* sheet = this->sheet();
    if (!sheet || sheet->disabled() || !sheet->isCSSStyleSheet())
        return false;
    const AtomicString& title = this->title();
    if (!isEnabledViaScript() && !title.isEmpty() && title != currentPreferrableName)
        return false;
    if (isAlternate() && title.isEmpty())
        return false;

    return true;
}

StyleSheetCandidate::Type StyleSheetCandidate::typeOf(Node& node)
{
    if (node.nodeType() == Node::PROCESSING_INSTRUCTION_NODE)
        return Pi;

    if (node.isHTMLElement()) {
        if (isHTMLLinkElement(node))
            return HTMLLink;
        if (isHTMLStyleElement(node))
            return HTMLStyle;

        ASSERT_NOT_REACHED();
        return HTMLStyle;
    }

    if (isSVGStyleElement(node))
        return SVGStyle;

    ASSERT_NOT_REACHED();
    return HTMLStyle;
}

StyleSheet* StyleSheetCandidate::sheet() const
{
    switch (m_type) {
    case HTMLLink:
        return toHTMLLinkElement(node()).sheet();
    case HTMLStyle:
        return toHTMLStyleElement(node()).sheet();
    case SVGStyle:
        return toSVGStyleElement(node()).sheet();
    case Pi:
        return toProcessingInstruction(node()).sheet();
    }

    ASSERT_NOT_REACHED();
    return 0;
}

}
