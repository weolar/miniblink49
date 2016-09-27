/*
 * Copyright (C) 2006, 2007 Rob Buis
 * Copyright (C) 2008 Apple, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/dom/StyleElement.h"

#include "bindings/core/v8/ScriptController.h"
#include "core/css/MediaList.h"
#include "core/css/MediaQueryEvaluator.h"
#include "core/css/StyleSheetContents.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/ScriptableDocumentParser.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/HTMLStyleElement.h"
#include "core/svg/SVGStyleElement.h"
#include "platform/TraceEvent.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

static bool isCSS(Element* element, const AtomicString& type)
{
    return type.isEmpty() || (element->isHTMLElement() ? equalIgnoringCase(type, "text/css") : (type == "text/css"));
}

StyleElement::StyleElement(Document* document, bool createdByParser)
    : m_createdByParser(createdByParser)
    , m_loading(false)
    , m_registeredAsCandidate(false)
    , m_startPosition(TextPosition::belowRangePosition())
{
    if (createdByParser && document && document->scriptableDocumentParser() && !document->isInDocumentWrite())
        m_startPosition = document->scriptableDocumentParser()->textPosition();
}

StyleElement::~StyleElement()
{
#if !ENABLE(OILPAN)
    if (m_sheet)
        clearSheet();
#endif
}

StyleElement::ProcessingResult StyleElement::processStyleSheet(Document& document, Element* element)
{
    TRACE_EVENT0("blink", "StyleElement::processStyleSheet");
    ASSERT(element);
    ASSERT(element->inDocument());

    m_registeredAsCandidate = true;
    document.styleEngine().addStyleSheetCandidateNode(element, m_createdByParser);
    if (m_createdByParser)
        return ProcessingSuccessful;

    return process(element);
}

void StyleElement::insertedInto(Element* element, ContainerNode* insertionPoint)
{
    if (!insertionPoint->inDocument() || !element->isInShadowTree())
        return;
    if (ShadowRoot* scope = element->containingShadowRoot())
        scope->registerScopedHTMLStyleChild();
}

void StyleElement::removedFrom(Element* element, ContainerNode* insertionPoint)
{
    if (!insertionPoint->inDocument())
        return;

    ShadowRoot* shadowRoot = element->containingShadowRoot();
    if (!shadowRoot)
        shadowRoot = insertionPoint->containingShadowRoot();

    if (shadowRoot)
        shadowRoot->unregisterScopedHTMLStyleChild();

    Document& document = element->document();
    if (m_registeredAsCandidate) {
        document.styleEngine().removeStyleSheetCandidateNode(element, shadowRoot ? *toTreeScope(shadowRoot) : toTreeScope(document));
        m_registeredAsCandidate = false;
    }

    RefPtrWillBeRawPtr<StyleSheet> removedSheet = m_sheet.get();

    if (m_sheet)
        clearSheet(element);
    if (removedSheet)
        document.removedStyleSheet(removedSheet.get(), AnalyzedStyleUpdate);
}

void StyleElement::clearDocumentData(Document& document, Element* element)
{
    if (m_sheet)
        m_sheet->clearOwnerNode();

    if (m_registeredAsCandidate) {
        ASSERT(element->inDocument());
        document.styleEngine().removeStyleSheetCandidateNode(element, element->treeScope());
        m_registeredAsCandidate = false;
    }
}

StyleElement::ProcessingResult StyleElement::childrenChanged(Element* element)
{
    ASSERT(element);
    if (m_createdByParser)
        return ProcessingSuccessful;

    return process(element);
}

StyleElement::ProcessingResult StyleElement::finishParsingChildren(Element* element)
{
    ASSERT(element);
    ProcessingResult result = process(element);
    m_createdByParser = false;
    return result;
}

StyleElement::ProcessingResult StyleElement::process(Element* element)
{
    if (!element || !element->inDocument())
        return ProcessingSuccessful;
    return createSheet(element, element->textFromChildren());
}

void StyleElement::clearSheet(Element* ownerElement)
{
    ASSERT(m_sheet);

    if (ownerElement && m_sheet->isLoading())
        ownerElement->document().styleEngine().removePendingSheet(ownerElement);

    m_sheet.release()->clearOwnerNode();
}

static bool shouldBypassMainWorldCSP(Element* element)
{
    // Main world CSP is bypassed within an isolated world.
    LocalFrame* frame = element->document().frame();
    if (frame && frame->script().shouldBypassMainWorldCSP())
        return true;

    // Main world CSP is bypassed for style elements in user agent shadow DOM.
    ShadowRoot* root = element->containingShadowRoot();
    if (root && root->type() == ShadowRootType::UserAgent)
        return true;

    return false;
}

StyleElement::ProcessingResult StyleElement::createSheet(Element* e, const String& text)
{
    ASSERT(e);
    ASSERT(e->inDocument());
    Document& document = e->document();
    if (m_sheet)
        clearSheet(e);

    const ContentSecurityPolicy* csp = document.contentSecurityPolicy();
    bool passesContentSecurityPolicyChecks = shouldBypassMainWorldCSP(e)
        || csp->allowStyleWithHash(text)
        || csp->allowStyleWithNonce(e->fastGetAttribute(HTMLNames::nonceAttr))
        || csp->allowInlineStyle(e->document().url(), m_startPosition.m_line, text);

    // If type is empty or CSS, this is a CSS style sheet.
    const AtomicString& type = this->type();
    if (isCSS(e, type) && passesContentSecurityPolicyChecks) {
        RefPtrWillBeRawPtr<MediaQuerySet> mediaQueries = MediaQuerySet::create(media());

        MediaQueryEvaluator screenEval("screen", true);
        MediaQueryEvaluator printEval("print", true);
        if (screenEval.eval(mediaQueries.get()) || printEval.eval(mediaQueries.get())) {
            m_loading = true;
            TextPosition startPosition = m_startPosition == TextPosition::belowRangePosition() ? TextPosition::minimumPosition() : m_startPosition;
            m_sheet = document.styleEngine().createSheet(e, text, startPosition);
            m_sheet->setMediaQueries(mediaQueries.release());
            m_loading = false;
        }
    }

    if (m_sheet)
        m_sheet->contents()->checkLoaded();

    return passesContentSecurityPolicyChecks ? ProcessingSuccessful : ProcessingFatalError;
}

bool StyleElement::isLoading() const
{
    if (m_loading)
        return true;
    return m_sheet ? m_sheet->isLoading() : false;
}

bool StyleElement::sheetLoaded(Document& document)
{
    if (isLoading())
        return false;

    document.styleEngine().removePendingSheet(m_sheet->ownerNode());
    return true;
}

void StyleElement::startLoadingDynamicSheet(Document& document)
{
    document.styleEngine().addPendingSheet();
}

DEFINE_TRACE(StyleElement)
{
    visitor->trace(m_sheet);
}

}
