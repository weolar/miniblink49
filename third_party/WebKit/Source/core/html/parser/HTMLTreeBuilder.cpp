/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 * Copyright (C) 2011, 2014 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/parser/HTMLTreeBuilder.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/MathMLNames.h"
#include "core/SVGNames.h"
#include "core/XLinkNames.h"
#include "core/XMLNSNames.h"
#include "core/XMLNames.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/ElementTraversal.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/parser/AtomicHTMLToken.h"
#include "core/html/parser/HTMLDocumentParser.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/parser/HTMLStackItem.h"
#include "core/html/parser/HTMLToken.h"
#include "core/html/parser/HTMLTokenizer.h"
#include "platform/NotImplemented.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/MainThread.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

using namespace HTMLNames;

namespace {

inline bool isHTMLSpaceOrReplacementCharacter(UChar character)
{
    return isHTMLSpace<UChar>(character) || character == replacementCharacter;
}

}

static TextPosition uninitializedPositionValue1()
{
    return TextPosition(OrdinalNumber::fromOneBasedInt(-1), OrdinalNumber::first());
}

static inline bool isAllWhitespace(const String& string)
{
    return string.isAllSpecialCharacters<isHTMLSpace<UChar>>();
}

static inline bool isAllWhitespaceOrReplacementCharacters(const String& string)
{
    return string.isAllSpecialCharacters<isHTMLSpaceOrReplacementCharacter>();
}

static bool isNumberedHeaderTag(const AtomicString& tagName)
{
    return tagName == h1Tag
        || tagName == h2Tag
        || tagName == h3Tag
        || tagName == h4Tag
        || tagName == h5Tag
        || tagName == h6Tag;
}

static bool isCaptionColOrColgroupTag(const AtomicString& tagName)
{
    return tagName == captionTag
        || tagName == colTag
        || tagName == colgroupTag;
}

static bool isTableCellContextTag(const AtomicString& tagName)
{
    return tagName == thTag || tagName == tdTag;
}

static bool isTableBodyContextTag(const AtomicString& tagName)
{
    return tagName == tbodyTag
        || tagName == tfootTag
        || tagName == theadTag;
}

static bool isNonAnchorNonNobrFormattingTag(const AtomicString& tagName)
{
    return tagName == bTag
        || tagName == bigTag
        || tagName == codeTag
        || tagName == emTag
        || tagName == fontTag
        || tagName == iTag
        || tagName == sTag
        || tagName == smallTag
        || tagName == strikeTag
        || tagName == strongTag
        || tagName == ttTag
        || tagName == uTag;
}

static bool isNonAnchorFormattingTag(const AtomicString& tagName)
{
    return tagName == nobrTag
        || isNonAnchorNonNobrFormattingTag(tagName);
}

// http://www.whatwg.org/specs/web-apps/current-work/multipage/parsing.html#formatting
static bool isFormattingTag(const AtomicString& tagName)
{
    return tagName == aTag || isNonAnchorFormattingTag(tagName);
}

static HTMLFormElement* closestFormAncestor(Element& element)
{
    ASSERT(isMainThread());
    return Traversal<HTMLFormElement>::firstAncestorOrSelf(element);
}

class HTMLTreeBuilder::CharacterTokenBuffer {
    WTF_MAKE_NONCOPYABLE(CharacterTokenBuffer);
public:
    explicit CharacterTokenBuffer(AtomicHTMLToken* token)
        : m_characters(token->characters().impl())
        , m_current(0)
        , m_end(token->characters().length())
    {
        ASSERT(!isEmpty());
    }

    explicit CharacterTokenBuffer(const String& characters)
        : m_characters(characters.impl())
        , m_current(0)
        , m_end(characters.length())
    {
        ASSERT(!isEmpty());
    }

    ~CharacterTokenBuffer()
    {
        ASSERT(isEmpty());
    }

    bool isEmpty() const { return m_current == m_end; }

    void skipAtMostOneLeadingNewline()
    {
        ASSERT(!isEmpty());
        if ((*m_characters)[m_current] == '\n')
            ++m_current;
    }

    void skipLeadingWhitespace()
    {
        skipLeading<isHTMLSpace<UChar>>();
    }

    String takeLeadingWhitespace()
    {
        return takeLeading<isHTMLSpace<UChar>>();
    }

    void skipLeadingNonWhitespace()
    {
        skipLeading<isNotHTMLSpace<UChar>>();
    }

    String takeRemaining()
    {
        ASSERT(!isEmpty());
        unsigned start = m_current;
        m_current = m_end;
        // Notice that substring is smart enough to return *this when start == 0.
        return String(m_characters->substring(start, m_end - start));
    }

    void giveRemainingTo(StringBuilder& recipient)
    {
        if (m_characters->is8Bit())
            recipient.append(m_characters->characters8() + m_current, m_end - m_current);
        else
            recipient.append(m_characters->characters16() + m_current, m_end - m_current);
        m_current = m_end;
    }

    String takeRemainingWhitespace()
    {
        ASSERT(!isEmpty());
        const unsigned start = m_current;
        m_current = m_end; // One way or another, we're taking everything!

        unsigned length = 0;
        for (unsigned i = start; i < m_end; ++i) {
            if (isHTMLSpace<UChar>((*m_characters)[i]))
                ++length;
        }
        // Returning the null string when there aren't any whitespace
        // characters is slightly cleaner semantically because we don't want
        // to insert a text node (as opposed to inserting an empty text node).
        if (!length)
            return String();
        if (length == start - m_end) // It's all whitespace.
            return String(m_characters->substring(start, start - m_end));

        StringBuilder result;
        result.reserveCapacity(length);
        for (unsigned i = start; i < m_end; ++i) {
            UChar c = (*m_characters)[i];
            if (isHTMLSpace<UChar>(c))
                result.append(c);
        }

        return result.toString();
    }

private:
    template<bool characterPredicate(UChar)>
    void skipLeading()
    {
        ASSERT(!isEmpty());
        while (characterPredicate((*m_characters)[m_current])) {
            if (++m_current == m_end)
                return;
        }
    }

    template<bool characterPredicate(UChar)>
    String takeLeading()
    {
        ASSERT(!isEmpty());
        const unsigned start = m_current;
        skipLeading<characterPredicate>();
        if (start == m_current)
            return String();
        return String(m_characters->substring(start, m_current - start));
    }

    RefPtr<StringImpl> m_characters;
    unsigned m_current;
    unsigned m_end;
};

HTMLTreeBuilder::HTMLTreeBuilder(HTMLDocumentParser* parser, HTMLDocument* document, ParserContentPolicy parserContentPolicy, bool, const HTMLParserOptions& options)
    : m_framesetOk(true)
#if ENABLE(ASSERT)
    , m_isAttached(true)
#endif
    , m_tree(document, parserContentPolicy)
    , m_insertionMode(InitialMode)
    , m_originalInsertionMode(InitialMode)
    , m_shouldSkipLeadingNewline(false)
    , m_parser(parser)
    , m_scriptToProcessStartPosition(uninitializedPositionValue1())
    , m_options(options)
{
}

// FIXME: Member variables should be grouped into self-initializing structs to
// minimize code duplication between these constructors.
HTMLTreeBuilder::HTMLTreeBuilder(HTMLDocumentParser* parser, DocumentFragment* fragment, Element* contextElement, ParserContentPolicy parserContentPolicy, const HTMLParserOptions& options)
    : m_framesetOk(true)
#if ENABLE(ASSERT)
    , m_isAttached(true)
#endif
    , m_fragmentContext(fragment, contextElement)
    , m_tree(fragment, parserContentPolicy)
    , m_insertionMode(InitialMode)
    , m_originalInsertionMode(InitialMode)
    , m_shouldSkipLeadingNewline(false)
    , m_parser(parser)
    , m_scriptToProcessStartPosition(uninitializedPositionValue1())
    , m_options(options)
{
    ASSERT(isMainThread());
    ASSERT(contextElement);

    // Steps 4.2-4.6 of the HTML5 Fragment Case parsing algorithm:
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/the-end.html#fragment-case
    // For efficiency, we skip step 4.2 ("Let root be a new html element with no attributes")
    // and instead use the DocumentFragment as a root node.
    m_tree.openElements()->pushRootNode(HTMLStackItem::create(fragment, HTMLStackItem::ItemForDocumentFragmentNode));

    if (isHTMLTemplateElement(*contextElement))
        m_templateInsertionModes.append(TemplateContentsMode);

    resetInsertionModeAppropriately();
    m_tree.setForm(closestFormAncestor(*contextElement));
}

HTMLTreeBuilder::~HTMLTreeBuilder()
{
}

DEFINE_TRACE(HTMLTreeBuilder)
{
    visitor->trace(m_fragmentContext);
    visitor->trace(m_tree);
    visitor->trace(m_parser);
    visitor->trace(m_scriptToProcess);
}

void HTMLTreeBuilder::detach()
{
#if ENABLE(ASSERT)
    // This call makes little sense in fragment mode, but for consistency
    // DocumentParser expects detach() to always be called before it's destroyed.
    m_isAttached = false;
#endif
    // HTMLConstructionSite might be on the callstack when detach() is called
    // otherwise we'd just call m_tree.clear() here instead.
    m_tree.detach();
}

HTMLTreeBuilder::FragmentParsingContext::FragmentParsingContext()
    : m_fragment(nullptr)
{
}

HTMLTreeBuilder::FragmentParsingContext::FragmentParsingContext(DocumentFragment* fragment, Element* contextElement)
    : m_fragment(fragment)
{
    ASSERT(!fragment->hasChildren());
    m_contextElementStackItem = HTMLStackItem::create(contextElement, HTMLStackItem::ItemForContextElement);
}

HTMLTreeBuilder::FragmentParsingContext::~FragmentParsingContext()
{
}

DEFINE_TRACE(HTMLTreeBuilder::FragmentParsingContext)
{
    visitor->trace(m_fragment);
    visitor->trace(m_contextElementStackItem);
}

PassRefPtrWillBeRawPtr<Element> HTMLTreeBuilder::takeScriptToProcess(TextPosition& scriptStartPosition)
{
    ASSERT(m_scriptToProcess);
    ASSERT(!m_tree.hasPendingTasks());
    // Unpause ourselves, callers may pause us again when processing the script.
    // The HTML5 spec is written as though scripts are executed inside the tree
    // builder.  We pause the parser to exit the tree builder, and then resume
    // before running scripts.
    scriptStartPosition = m_scriptToProcessStartPosition;
    m_scriptToProcessStartPosition = uninitializedPositionValue1();
    return m_scriptToProcess.release();
}

void HTMLTreeBuilder::constructTree(AtomicHTMLToken* token)
{
    if (shouldProcessTokenInForeignContent(token))
        processTokenInForeignContent(token);
    else
        processToken(token);

    if (m_parser->tokenizer()) {
        bool inForeignContent = false;
        if (!m_tree.isEmpty()) {
            HTMLStackItem* adjustedCurrentNode = adjustedCurrentStackItem();
            inForeignContent = !adjustedCurrentNode->isInHTMLNamespace()
                && !HTMLElementStack::isHTMLIntegrationPoint(adjustedCurrentNode)
                && !HTMLElementStack::isMathMLTextIntegrationPoint(adjustedCurrentNode);
        }

        m_parser->tokenizer()->setForceNullCharacterReplacement(m_insertionMode == TextMode || inForeignContent);
        m_parser->tokenizer()->setShouldAllowCDATA(inForeignContent);
    }

    m_tree.executeQueuedTasks();
    // We might be detached now.
}

void HTMLTreeBuilder::processToken(AtomicHTMLToken* token)
{
    if (token->type() == HTMLToken::Character) {
        processCharacter(token);
        return;
    }

    // Any non-character token needs to cause us to flush any pending text immediately.
    // NOTE: flush() can cause any queued tasks to execute, possibly re-entering the parser.
    m_tree.flush(FlushAlways);
    m_shouldSkipLeadingNewline = false;

    switch (token->type()) {
    case HTMLToken::Uninitialized:
    case HTMLToken::Character:
        ASSERT_NOT_REACHED();
        break;
    case HTMLToken::DOCTYPE:
        processDoctypeToken(token);
        break;
    case HTMLToken::StartTag:
        processStartTag(token);
        break;
    case HTMLToken::EndTag:
        processEndTag(token);
        break;
    case HTMLToken::Comment:
        processComment(token);
        break;
    case HTMLToken::EndOfFile:
        processEndOfFile(token);
        break;
    }
}

void HTMLTreeBuilder::processDoctypeToken(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::DOCTYPE);
    if (m_insertionMode == InitialMode) {
        m_tree.insertDoctype(token);
        setInsertionMode(BeforeHTMLMode);
        return;
    }
    if (m_insertionMode == InTableTextMode) {
        defaultForInTableText();
        processDoctypeToken(token);
        return;
    }
    parseError(token);
}

void HTMLTreeBuilder::processFakeStartTag(const QualifiedName& tagName, const Vector<Attribute>& attributes)
{
    // FIXME: We'll need a fancier conversion than just "localName" for SVG/MathML tags.
    AtomicHTMLToken fakeToken(HTMLToken::StartTag, tagName.localName(), attributes);
    processStartTag(&fakeToken);
}

void HTMLTreeBuilder::processFakeEndTag(const AtomicString& tagName)
{
    AtomicHTMLToken fakeToken(HTMLToken::EndTag, tagName);
    processEndTag(&fakeToken);
}

void HTMLTreeBuilder::processFakeEndTag(const QualifiedName& tagName)
{
    // FIXME: We'll need a fancier conversion than just "localName" for SVG/MathML tags.
    processFakeEndTag(tagName.localName());
}

void HTMLTreeBuilder::processFakePEndTagIfPInButtonScope()
{
    if (!m_tree.openElements()->inButtonScope(pTag.localName()))
        return;
    AtomicHTMLToken endP(HTMLToken::EndTag, pTag.localName());
    processEndTag(&endP);
}

namespace {

bool isLi(const HTMLStackItem* item)
{
    return item->hasTagName(liTag);
}

bool isDdOrDt(const HTMLStackItem* item)
{
    return item->hasTagName(ddTag)
        || item->hasTagName(dtTag);
}

}

template <bool shouldClose(const HTMLStackItem*)>
void HTMLTreeBuilder::processCloseWhenNestedTag(AtomicHTMLToken* token)
{
    m_framesetOk = false;
    HTMLElementStack::ElementRecord* nodeRecord = m_tree.openElements()->topRecord();
    while (1) {
        RefPtrWillBeRawPtr<HTMLStackItem> item = nodeRecord->stackItem();
        if (shouldClose(item.get())) {
            ASSERT(item->isElementNode());
            processFakeEndTag(item->localName());
            break;
        }
        if (item->isSpecialNode() && !item->hasTagName(addressTag) && !item->hasTagName(divTag) && !item->hasTagName(pTag))
            break;
        nodeRecord = nodeRecord->next();
    }
    processFakePEndTagIfPInButtonScope();
    m_tree.insertHTMLElement(token);
}

typedef HashMap<AtomicString, QualifiedName> PrefixedNameToQualifiedNameMap;

template <typename TableQualifiedName>
static void mapLoweredLocalNameToName(PrefixedNameToQualifiedNameMap* map, const TableQualifiedName* const* names, size_t length)
{
    for (size_t i = 0; i < length; ++i) {
        const QualifiedName& name = *names[i];
        const AtomicString& localName = name.localName();
        AtomicString loweredLocalName = localName.lower();
        if (loweredLocalName != localName)
            map->add(loweredLocalName, name);
    }
}

static void adjustSVGTagNameCase(AtomicHTMLToken* token)
{
    static PrefixedNameToQualifiedNameMap* caseMap = 0;
    if (!caseMap) {
        caseMap = new PrefixedNameToQualifiedNameMap;
        OwnPtr<const SVGQualifiedName*[]> svgTags = SVGNames::getSVGTags();
        mapLoweredLocalNameToName(caseMap, svgTags.get(), SVGNames::SVGTagsCount);
    }

    const QualifiedName& casedName = caseMap->get(token->name());
    if (casedName.localName().isNull())
        return;
    token->setName(casedName.localName());
}

template<PassOwnPtr<const QualifiedName*[]> getAttrs(), unsigned length>
static void adjustAttributes(AtomicHTMLToken* token)
{
    static PrefixedNameToQualifiedNameMap* caseMap = 0;
    if (!caseMap) {
        caseMap = new PrefixedNameToQualifiedNameMap;
        OwnPtr<const QualifiedName*[]> attrs = getAttrs();
        mapLoweredLocalNameToName(caseMap, attrs.get(), length);
    }

    for (unsigned i = 0; i < token->attributes().size(); ++i) {
        Attribute& tokenAttribute = token->attributes().at(i);
        const QualifiedName& casedName = caseMap->get(tokenAttribute.localName());
        if (!casedName.localName().isNull())
            tokenAttribute.parserSetName(casedName);
    }
}

static void adjustSVGAttributes(AtomicHTMLToken* token)
{
    adjustAttributes<SVGNames::getSVGAttrs, SVGNames::SVGAttrsCount>(token);
}

static void adjustMathMLAttributes(AtomicHTMLToken* token)
{
    adjustAttributes<MathMLNames::getMathMLAttrs, MathMLNames::MathMLAttrsCount>(token);
}

static void addNamesWithPrefix(PrefixedNameToQualifiedNameMap* map, const AtomicString& prefix, const QualifiedName* const* names, size_t length)
{
    for (size_t i = 0; i < length; ++i) {
        const QualifiedName* name = names[i];
        const AtomicString& localName = name->localName();
        AtomicString prefixColonLocalName = prefix + ':' + localName;
        QualifiedName nameWithPrefix(prefix, localName, name->namespaceURI());
        map->add(prefixColonLocalName, nameWithPrefix);
    }
}

static void adjustForeignAttributes(AtomicHTMLToken* token)
{
    static PrefixedNameToQualifiedNameMap* map = 0;
    if (!map) {
        map = new PrefixedNameToQualifiedNameMap;

        OwnPtr<const QualifiedName*[]> attrs = XLinkNames::getXLinkAttrs();
        addNamesWithPrefix(map, xlinkAtom, attrs.get(), XLinkNames::XLinkAttrsCount);

        OwnPtr<const QualifiedName*[]> xmlAttrs = XMLNames::getXMLAttrs();
        addNamesWithPrefix(map, xmlAtom, xmlAttrs.get(), XMLNames::XMLAttrsCount);

        map->add(WTF::xmlnsAtom, XMLNSNames::xmlnsAttr);
        map->add("xmlns:xlink", QualifiedName(xmlnsAtom, xlinkAtom, XMLNSNames::xmlnsNamespaceURI));
    }

    for (unsigned i = 0; i < token->attributes().size(); ++i) {
        Attribute& tokenAttribute = token->attributes().at(i);
        const QualifiedName& name = map->get(tokenAttribute.localName());
        if (!name.localName().isNull())
            tokenAttribute.parserSetName(name);
    }
}

void HTMLTreeBuilder::processStartTagForInBody(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    if (token->name() == htmlTag) {
        processHtmlStartTagForInBody(token);
        return;
    }
    if (token->name() == baseTag
        || token->name() == basefontTag
        || token->name() == bgsoundTag
        || token->name() == commandTag
        || token->name() == linkTag
        || token->name() == metaTag
        || token->name() == noframesTag
        || token->name() == scriptTag
        || token->name() == styleTag
        || token->name() == titleTag) {
        bool didProcess = processStartTagForInHead(token);
        ASSERT_UNUSED(didProcess, didProcess);
        return;
    }
    if (token->name() == bodyTag) {
        parseError(token);
        if (!m_tree.openElements()->secondElementIsHTMLBodyElement() || m_tree.openElements()->hasOnlyOneElement() || m_tree.openElements()->hasTemplateInHTMLScope()) {
            ASSERT(isParsingFragmentOrTemplateContents());
            return;
        }
        m_framesetOk = false;
        m_tree.insertHTMLBodyStartTagInBody(token);
        return;
    }
    if (token->name() == framesetTag) {
        parseError(token);
        if (!m_tree.openElements()->secondElementIsHTMLBodyElement() || m_tree.openElements()->hasOnlyOneElement()) {
            ASSERT(isParsingFragmentOrTemplateContents());
            return;
        }
        if (!m_framesetOk)
            return;
        m_tree.openElements()->bodyElement()->remove(ASSERT_NO_EXCEPTION);
        m_tree.openElements()->popUntil(m_tree.openElements()->bodyElement());
        m_tree.openElements()->popHTMLBodyElement();
        ASSERT(m_tree.openElements()->top() == m_tree.openElements()->htmlElement());
        m_tree.insertHTMLElement(token);
        setInsertionMode(InFramesetMode);
        return;
    }
    if (token->name() == addressTag
        || token->name() == articleTag
        || token->name() == asideTag
        || token->name() == blockquoteTag
        || token->name() == centerTag
        || token->name() == detailsTag
        || token->name() == dirTag
        || token->name() == divTag
        || token->name() == dlTag
        || token->name() == fieldsetTag
        || token->name() == figcaptionTag
        || token->name() == figureTag
        || token->name() == footerTag
        || token->name() == headerTag
        || token->name() == hgroupTag
        || token->name() == mainTag
        || token->name() == menuTag
        || token->name() == navTag
        || token->name() == olTag
        || token->name() == pTag
        || token->name() == sectionTag
        || token->name() == summaryTag
        || token->name() == ulTag) {
        processFakePEndTagIfPInButtonScope();
        m_tree.insertHTMLElement(token);
        return;
    }
    if (isNumberedHeaderTag(token->name())) {
        processFakePEndTagIfPInButtonScope();
        if (m_tree.currentStackItem()->isNumberedHeaderElement()) {
            parseError(token);
            m_tree.openElements()->pop();
        }
        m_tree.insertHTMLElement(token);
        return;
    }
    if (token->name() == preTag || token->name() == listingTag) {
        processFakePEndTagIfPInButtonScope();
        m_tree.insertHTMLElement(token);
        m_shouldSkipLeadingNewline = true;
        m_framesetOk = false;
        return;
    }
    if (token->name() == formTag) {
        if (m_tree.form()) {
            parseError(token);
            return;
        }
        processFakePEndTagIfPInButtonScope();
        m_tree.insertHTMLFormElement(token);
        return;
    }
    if (token->name() == liTag) {
        processCloseWhenNestedTag<isLi>(token);
        return;
    }
    if (token->name() == ddTag || token->name() == dtTag) {
        processCloseWhenNestedTag<isDdOrDt>(token);
        return;
    }
    if (token->name() == plaintextTag) {
        processFakePEndTagIfPInButtonScope();
        m_tree.insertHTMLElement(token);
        if (m_parser->tokenizer())
            m_parser->tokenizer()->setState(HTMLTokenizer::PLAINTEXTState);
        return;
    }
    if (token->name() == buttonTag) {
        if (m_tree.openElements()->inScope(buttonTag)) {
            parseError(token);
            processFakeEndTag(buttonTag);
            processStartTag(token); // FIXME: Could we just fall through here?
            return;
        }
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertHTMLElement(token);
        m_framesetOk = false;
        return;
    }
    if (token->name() == aTag) {
        Element* activeATag = m_tree.activeFormattingElements()->closestElementInScopeWithName(aTag.localName());
        if (activeATag) {
            parseError(token);
            processFakeEndTag(aTag);
            m_tree.activeFormattingElements()->remove(activeATag);
            if (m_tree.openElements()->contains(activeATag))
                m_tree.openElements()->remove(activeATag);
        }
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertFormattingElement(token);
        return;
    }
    if (isNonAnchorNonNobrFormattingTag(token->name())) {
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertFormattingElement(token);
        return;
    }
    if (token->name() == nobrTag) {
        m_tree.reconstructTheActiveFormattingElements();
        if (m_tree.openElements()->inScope(nobrTag)) {
            parseError(token);
            processFakeEndTag(nobrTag);
            m_tree.reconstructTheActiveFormattingElements();
        }
        m_tree.insertFormattingElement(token);
        return;
    }
    if (token->name() == appletTag
        || token->name() == embedTag
        || token->name() == objectTag) {
        if (!pluginContentIsAllowed(m_tree.parserContentPolicy()))
            return;
    }
    if (token->name() == appletTag
        || token->name() == marqueeTag
        || token->name() == objectTag) {
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertHTMLElement(token);
        m_tree.activeFormattingElements()->appendMarker();
        m_framesetOk = false;
        return;
    }
    if (token->name() == tableTag) {
        if (!m_tree.inQuirksMode() && m_tree.openElements()->inButtonScope(pTag))
            processFakeEndTag(pTag);
        m_tree.insertHTMLElement(token);
        m_framesetOk = false;
        setInsertionMode(InTableMode);
        return;
    }
    if (token->name() == imageTag) {
        parseError(token);
        // Apparently we're not supposed to ask.
        token->setName(imgTag.localName());
        // Note the fall through to the imgTag handling below!
    }
    if (token->name() == areaTag
        || token->name() == brTag
        || token->name() == embedTag
        || token->name() == imgTag
        || token->name() == keygenTag
        || token->name() == wbrTag) {
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertSelfClosingHTMLElement(token);
        m_framesetOk = false;
        return;
    }
    if (token->name() == inputTag) {
        Attribute* typeAttribute = token->getAttributeItem(typeAttr);
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertSelfClosingHTMLElement(token);
        if (!typeAttribute || !equalIgnoringCase(typeAttribute->value(), "hidden"))
            m_framesetOk = false;
        return;
    }
    if ((RuntimeEnabledFeatures::contextMenuEnabled() && token->name() == menuitemTag)
        || token->name() == paramTag
        || token->name() == sourceTag
        || token->name() == trackTag) {
        m_tree.insertSelfClosingHTMLElement(token);
        return;
    }
    if (token->name() == hrTag) {
        processFakePEndTagIfPInButtonScope();
        m_tree.insertSelfClosingHTMLElement(token);
        m_framesetOk = false;
        return;
    }
    if (token->name() == textareaTag) {
        m_tree.insertHTMLElement(token);
        m_shouldSkipLeadingNewline = true;
        if (m_parser->tokenizer())
            m_parser->tokenizer()->setState(HTMLTokenizer::RCDATAState);
        m_originalInsertionMode = m_insertionMode;
        m_framesetOk = false;
        setInsertionMode(TextMode);
        return;
    }
    if (token->name() == xmpTag) {
        processFakePEndTagIfPInButtonScope();
        m_tree.reconstructTheActiveFormattingElements();
        m_framesetOk = false;
        processGenericRawTextStartTag(token);
        return;
    }
    if (token->name() == iframeTag) {
        m_framesetOk = false;
        processGenericRawTextStartTag(token);
        return;
    }
    if (token->name() == noembedTag && m_options.pluginsEnabled) {
        processGenericRawTextStartTag(token);
        return;
    }
    if (token->name() == noscriptTag && m_options.scriptEnabled) {
        processGenericRawTextStartTag(token);
        return;
    }
    if (token->name() == selectTag) {
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertHTMLElement(token);
        m_framesetOk = false;
        if (m_insertionMode == InTableMode
             || m_insertionMode == InCaptionMode
             || m_insertionMode == InColumnGroupMode
             || m_insertionMode == InTableBodyMode
             || m_insertionMode == InRowMode
             || m_insertionMode == InCellMode)
            setInsertionMode(InSelectInTableMode);
        else
            setInsertionMode(InSelectMode);
        return;
    }
    if (token->name() == optgroupTag || token->name() == optionTag) {
        if (m_tree.currentStackItem()->hasTagName(optionTag)) {
            AtomicHTMLToken endOption(HTMLToken::EndTag, optionTag.localName());
            processEndTag(&endOption);
        }
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertHTMLElement(token);
        return;
    }
    if (token->name() == rbTag || token->name() == rtcTag) {
        if (m_tree.openElements()->inScope(rubyTag.localName())) {
            m_tree.generateImpliedEndTags();
            if (!m_tree.currentStackItem()->hasTagName(rubyTag))
                parseError(token);
        }
        m_tree.insertHTMLElement(token);
        return;
    }
    if (token->name() == rtTag || token->name() == rpTag) {
        if (m_tree.openElements()->inScope(rubyTag.localName())) {
            m_tree.generateImpliedEndTagsWithExclusion(rtcTag.localName());
            if (!m_tree.currentStackItem()->hasTagName(rubyTag) && !m_tree.currentStackItem()->hasTagName(rtcTag))
                parseError(token);
        }
        m_tree.insertHTMLElement(token);
        return;
    }
    if (token->name() == MathMLNames::mathTag.localName()) {
        m_tree.reconstructTheActiveFormattingElements();
        adjustMathMLAttributes(token);
        adjustForeignAttributes(token);
        m_tree.insertForeignElement(token, MathMLNames::mathmlNamespaceURI);
        return;
    }
    if (token->name() == SVGNames::svgTag.localName()) {
        m_tree.reconstructTheActiveFormattingElements();
        adjustSVGAttributes(token);
        adjustForeignAttributes(token);
        m_tree.insertForeignElement(token, SVGNames::svgNamespaceURI);
        return;
    }
    if (isCaptionColOrColgroupTag(token->name())
        || token->name() == frameTag
        || token->name() == headTag
        || isTableBodyContextTag(token->name())
        || isTableCellContextTag(token->name())
        || token->name() == trTag) {
        parseError(token);
        return;
    }
    if (token->name() == templateTag) {
        processTemplateStartTag(token);
        return;
    }
    m_tree.reconstructTheActiveFormattingElements();
    m_tree.insertHTMLElement(token);
}

void HTMLTreeBuilder::processTemplateStartTag(AtomicHTMLToken* token)
{
    m_tree.activeFormattingElements()->appendMarker();
    m_tree.insertHTMLElement(token);
    m_templateInsertionModes.append(TemplateContentsMode);
    setInsertionMode(TemplateContentsMode);
}

bool HTMLTreeBuilder::processTemplateEndTag(AtomicHTMLToken* token)
{
    ASSERT(token->name() == templateTag.localName());
    if (!m_tree.openElements()->hasTemplateInHTMLScope()) {
        ASSERT(m_templateInsertionModes.isEmpty() || (m_templateInsertionModes.size() == 1 && isHTMLTemplateElement(m_fragmentContext.contextElement())));
        parseError(token);
        return false;
    }
    m_tree.generateImpliedEndTags();
    if (!m_tree.currentStackItem()->hasTagName(templateTag))
        parseError(token);
    m_tree.openElements()->popUntilPopped(templateTag);
    m_tree.activeFormattingElements()->clearToLastMarker();
    m_templateInsertionModes.removeLast();
    resetInsertionModeAppropriately();
    return true;
}

bool HTMLTreeBuilder::processEndOfFileForInTemplateContents(AtomicHTMLToken* token)
{
    AtomicHTMLToken endTemplate(HTMLToken::EndTag, templateTag.localName());
    if (!processTemplateEndTag(&endTemplate))
        return false;

    processEndOfFile(token);
    return true;
}

bool HTMLTreeBuilder::processColgroupEndTagForInColumnGroup()
{
    if (m_tree.currentIsRootNode() || isHTMLTemplateElement(*m_tree.currentNode())) {
        ASSERT(isParsingFragmentOrTemplateContents());
        // FIXME: parse error
        return false;
    }
    m_tree.openElements()->pop();
    setInsertionMode(InTableMode);
    return true;
}

// http://www.whatwg.org/specs/web-apps/current-work/#adjusted-current-node
HTMLStackItem* HTMLTreeBuilder::adjustedCurrentStackItem() const
{
    ASSERT(!m_tree.isEmpty());
    if (isParsingFragment() && m_tree.openElements()->hasOnlyOneElement())
        return m_fragmentContext.contextElementStackItem();

    return m_tree.currentStackItem();
}

// http://www.whatwg.org/specs/web-apps/current-work/multipage/tokenization.html#close-the-cell
void HTMLTreeBuilder::closeTheCell()
{
    ASSERT(insertionMode() == InCellMode);
    if (m_tree.openElements()->inTableScope(tdTag)) {
        ASSERT(!m_tree.openElements()->inTableScope(thTag));
        processFakeEndTag(tdTag);
        return;
    }
    ASSERT(m_tree.openElements()->inTableScope(thTag));
    processFakeEndTag(thTag);
    ASSERT(insertionMode() == InRowMode);
}

void HTMLTreeBuilder::processStartTagForInTable(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    if (token->name() == captionTag) {
        m_tree.openElements()->popUntilTableScopeMarker();
        m_tree.activeFormattingElements()->appendMarker();
        m_tree.insertHTMLElement(token);
        setInsertionMode(InCaptionMode);
        return;
    }
    if (token->name() == colgroupTag) {
        m_tree.openElements()->popUntilTableScopeMarker();
        m_tree.insertHTMLElement(token);
        setInsertionMode(InColumnGroupMode);
        return;
    }
    if (token->name() == colTag) {
        processFakeStartTag(colgroupTag);
        ASSERT(InColumnGroupMode);
        processStartTag(token);
        return;
    }
    if (isTableBodyContextTag(token->name())) {
        m_tree.openElements()->popUntilTableScopeMarker();
        m_tree.insertHTMLElement(token);
        setInsertionMode(InTableBodyMode);
        return;
    }
    if (isTableCellContextTag(token->name())
        || token->name() == trTag) {
        processFakeStartTag(tbodyTag);
        ASSERT(insertionMode() == InTableBodyMode);
        processStartTag(token);
        return;
    }
    if (token->name() == tableTag) {
        parseError(token);
        if (!processTableEndTagForInTable()) {
            ASSERT(isParsingFragmentOrTemplateContents());
            return;
        }
        processStartTag(token);
        return;
    }
    if (token->name() == styleTag || token->name() == scriptTag) {
        processStartTagForInHead(token);
        return;
    }
    if (token->name() == inputTag) {
        Attribute* typeAttribute = token->getAttributeItem(typeAttr);
        if (typeAttribute && equalIgnoringCase(typeAttribute->value(), "hidden")) {
            parseError(token);
            m_tree.insertSelfClosingHTMLElement(token);
            return;
        }
        // Fall through to "anything else" case.
    }
    if (token->name() == formTag) {
        parseError(token);
        if (m_tree.form())
            return;
        m_tree.insertHTMLFormElement(token, true);
        m_tree.openElements()->pop();
        return;
    }
    if (token->name() == templateTag) {
        processTemplateStartTag(token);
        return;
    }
    parseError(token);
    HTMLConstructionSite::RedirectToFosterParentGuard redirecter(m_tree);
    processStartTagForInBody(token);
}

void HTMLTreeBuilder::processStartTag(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    switch (insertionMode()) {
    case InitialMode:
        ASSERT(insertionMode() == InitialMode);
        defaultForInitial();
        // Fall through.
    case BeforeHTMLMode:
        ASSERT(insertionMode() == BeforeHTMLMode);
        if (token->name() == htmlTag) {
            m_tree.insertHTMLHtmlStartTagBeforeHTML(token);
            setInsertionMode(BeforeHeadMode);
            return;
        }
        defaultForBeforeHTML();
        // Fall through.
    case BeforeHeadMode:
        ASSERT(insertionMode() == BeforeHeadMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        if (token->name() == headTag) {
            m_tree.insertHTMLHeadElement(token);
            setInsertionMode(InHeadMode);
            return;
        }
        defaultForBeforeHead();
        // Fall through.
    case InHeadMode:
        ASSERT(insertionMode() == InHeadMode);
        if (processStartTagForInHead(token))
            return;
        defaultForInHead();
        // Fall through.
    case AfterHeadMode:
        ASSERT(insertionMode() == AfterHeadMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        if (token->name() == bodyTag) {
            m_framesetOk = false;
            m_tree.insertHTMLBodyElement(token);
            setInsertionMode(InBodyMode);
            return;
        }
        if (token->name() == framesetTag) {
            m_tree.insertHTMLElement(token);
            setInsertionMode(InFramesetMode);
            return;
        }
        if (token->name() == baseTag
            || token->name() == basefontTag
            || token->name() == bgsoundTag
            || token->name() == linkTag
            || token->name() == metaTag
            || token->name() == noframesTag
            || token->name() == scriptTag
            || token->name() == styleTag
            || token->name() == templateTag
            || token->name() == titleTag) {
            parseError(token);
            ASSERT(m_tree.head());
            m_tree.openElements()->pushHTMLHeadElement(m_tree.headStackItem());
            processStartTagForInHead(token);
            m_tree.openElements()->removeHTMLHeadElement(m_tree.head());
            return;
        }
        if (token->name() == headTag) {
            parseError(token);
            return;
        }
        defaultForAfterHead();
        // Fall through
    case InBodyMode:
        ASSERT(insertionMode() == InBodyMode);
        processStartTagForInBody(token);
        break;
    case InTableMode:
        ASSERT(insertionMode() == InTableMode);
        processStartTagForInTable(token);
        break;
    case InCaptionMode:
        ASSERT(insertionMode() == InCaptionMode);
        if (isCaptionColOrColgroupTag(token->name())
            || isTableBodyContextTag(token->name())
            || isTableCellContextTag(token->name())
            || token->name() == trTag) {
            parseError(token);
            if (!processCaptionEndTagForInCaption()) {
                ASSERT(isParsingFragment());
                return;
            }
            processStartTag(token);
            return;
        }
        processStartTagForInBody(token);
        break;
    case InColumnGroupMode:
        ASSERT(insertionMode() == InColumnGroupMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        if (token->name() == colTag) {
            m_tree.insertSelfClosingHTMLElement(token);
            return;
        }
        if (token->name() == templateTag) {
            processTemplateStartTag(token);
            return;
        }
        if (!processColgroupEndTagForInColumnGroup()) {
            ASSERT(isParsingFragmentOrTemplateContents());
            return;
        }
        processStartTag(token);
        break;
    case InTableBodyMode:
        ASSERT(insertionMode() == InTableBodyMode);
        if (token->name() == trTag) {
            m_tree.openElements()->popUntilTableBodyScopeMarker(); // How is there ever anything to pop?
            m_tree.insertHTMLElement(token);
            setInsertionMode(InRowMode);
            return;
        }
        if (isTableCellContextTag(token->name())) {
            parseError(token);
            processFakeStartTag(trTag);
            ASSERT(insertionMode() == InRowMode);
            processStartTag(token);
            return;
        }
        if (isCaptionColOrColgroupTag(token->name()) || isTableBodyContextTag(token->name())) {
            // FIXME: This is slow.
            if (!m_tree.openElements()->inTableScope(tbodyTag) && !m_tree.openElements()->inTableScope(theadTag) && !m_tree.openElements()->inTableScope(tfootTag)) {
                ASSERT(isParsingFragmentOrTemplateContents());
                parseError(token);
                return;
            }
            m_tree.openElements()->popUntilTableBodyScopeMarker();
            ASSERT(isTableBodyContextTag(m_tree.currentStackItem()->localName()));
            processFakeEndTag(m_tree.currentStackItem()->localName());
            processStartTag(token);
            return;
        }
        processStartTagForInTable(token);
        break;
    case InRowMode:
        ASSERT(insertionMode() == InRowMode);
        if (isTableCellContextTag(token->name())) {
            m_tree.openElements()->popUntilTableRowScopeMarker();
            m_tree.insertHTMLElement(token);
            setInsertionMode(InCellMode);
            m_tree.activeFormattingElements()->appendMarker();
            return;
        }
        if (token->name() == trTag
            || isCaptionColOrColgroupTag(token->name())
            || isTableBodyContextTag(token->name())) {
            if (!processTrEndTagForInRow()) {
                ASSERT(isParsingFragmentOrTemplateContents());
                return;
            }
            ASSERT(insertionMode() == InTableBodyMode);
            processStartTag(token);
            return;
        }
        processStartTagForInTable(token);
        break;
    case InCellMode:
        ASSERT(insertionMode() == InCellMode);
        if (isCaptionColOrColgroupTag(token->name())
            || isTableCellContextTag(token->name())
            || token->name() == trTag
            || isTableBodyContextTag(token->name())) {
            // FIXME: This could be more efficient.
            if (!m_tree.openElements()->inTableScope(tdTag) && !m_tree.openElements()->inTableScope(thTag)) {
                ASSERT(isParsingFragment());
                parseError(token);
                return;
            }
            closeTheCell();
            processStartTag(token);
            return;
        }
        processStartTagForInBody(token);
        break;
    case AfterBodyMode:
    case AfterAfterBodyMode:
        ASSERT(insertionMode() == AfterBodyMode || insertionMode() == AfterAfterBodyMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        setInsertionMode(InBodyMode);
        processStartTag(token);
        break;
    case InHeadNoscriptMode:
        ASSERT(insertionMode() == InHeadNoscriptMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        if (token->name() == basefontTag
            || token->name() == bgsoundTag
            || token->name() == linkTag
            || token->name() == metaTag
            || token->name() == noframesTag
            || token->name() == styleTag) {
            bool didProcess = processStartTagForInHead(token);
            ASSERT_UNUSED(didProcess, didProcess);
            return;
        }
        if (token->name() == htmlTag || token->name() == noscriptTag) {
            parseError(token);
            return;
        }
        defaultForInHeadNoscript();
        processToken(token);
        break;
    case InFramesetMode:
        ASSERT(insertionMode() == InFramesetMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        if (token->name() == framesetTag) {
            m_tree.insertHTMLElement(token);
            return;
        }
        if (token->name() == frameTag) {
            m_tree.insertSelfClosingHTMLElement(token);
            return;
        }
        if (token->name() == noframesTag) {
            processStartTagForInHead(token);
            return;
        }
        if (token->name() == templateTag) {
            processTemplateStartTag(token);
            return;
        }
        parseError(token);
        break;
    case AfterFramesetMode:
    case AfterAfterFramesetMode:
        ASSERT(insertionMode() == AfterFramesetMode || insertionMode() == AfterAfterFramesetMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        if (token->name() == noframesTag) {
            processStartTagForInHead(token);
            return;
        }
        parseError(token);
        break;
    case InSelectInTableMode:
        ASSERT(insertionMode() == InSelectInTableMode);
        if (token->name() == captionTag
            || token->name() == tableTag
            || isTableBodyContextTag(token->name())
            || token->name() == trTag
            || isTableCellContextTag(token->name())) {
            parseError(token);
            AtomicHTMLToken endSelect(HTMLToken::EndTag, selectTag.localName());
            processEndTag(&endSelect);
            processStartTag(token);
            return;
        }
        // Fall through
    case InSelectMode:
        ASSERT(insertionMode() == InSelectMode || insertionMode() == InSelectInTableMode);
        if (token->name() == htmlTag) {
            processHtmlStartTagForInBody(token);
            return;
        }
        if (token->name() == optionTag) {
            if (m_tree.currentStackItem()->hasTagName(optionTag)) {
                AtomicHTMLToken endOption(HTMLToken::EndTag, optionTag.localName());
                processEndTag(&endOption);
            }
            m_tree.insertHTMLElement(token);
            return;
        }
        if (token->name() == optgroupTag) {
            if (m_tree.currentStackItem()->hasTagName(optionTag)) {
                AtomicHTMLToken endOption(HTMLToken::EndTag, optionTag.localName());
                processEndTag(&endOption);
            }
            if (m_tree.currentStackItem()->hasTagName(optgroupTag)) {
                AtomicHTMLToken endOptgroup(HTMLToken::EndTag, optgroupTag.localName());
                processEndTag(&endOptgroup);
            }
            m_tree.insertHTMLElement(token);
            return;
        }
        if (token->name() == selectTag) {
            parseError(token);
            AtomicHTMLToken endSelect(HTMLToken::EndTag, selectTag.localName());
            processEndTag(&endSelect);
            return;
        }
        if (token->name() == inputTag
            || token->name() == keygenTag
            || token->name() == textareaTag) {
            parseError(token);
            if (!m_tree.openElements()->inSelectScope(selectTag)) {
                ASSERT(isParsingFragment());
                return;
            }
            AtomicHTMLToken endSelect(HTMLToken::EndTag, selectTag.localName());
            processEndTag(&endSelect);
            processStartTag(token);
            return;
        }
        if (token->name() == scriptTag) {
            bool didProcess = processStartTagForInHead(token);
            ASSERT_UNUSED(didProcess, didProcess);
            return;
        }
        if (token->name() == templateTag) {
            processTemplateStartTag(token);
            return;
        }
        break;
    case InTableTextMode:
        defaultForInTableText();
        processStartTag(token);
        break;
    case TextMode:
        ASSERT_NOT_REACHED();
        break;
    case TemplateContentsMode:
        if (token->name() == templateTag) {
            processTemplateStartTag(token);
            return;
        }

        if (token->name() == linkTag
            || token->name() == scriptTag
            || token->name() == styleTag
            || token->name() == metaTag) {
            processStartTagForInHead(token);
            return;
        }

        InsertionMode insertionMode = TemplateContentsMode;
        if (token->name() == frameTag)
            insertionMode = InFramesetMode;
        else if (token->name() == colTag)
            insertionMode = InColumnGroupMode;
        else if (isCaptionColOrColgroupTag(token->name()) || isTableBodyContextTag(token->name()))
            insertionMode = InTableMode;
        else if (token->name() == trTag)
            insertionMode = InTableBodyMode;
        else if (isTableCellContextTag(token->name()))
            insertionMode = InRowMode;
        else
            insertionMode = InBodyMode;

        ASSERT(insertionMode != TemplateContentsMode);
        ASSERT(m_templateInsertionModes.last() == TemplateContentsMode);
        m_templateInsertionModes.last() = insertionMode;
        setInsertionMode(insertionMode);

        processStartTag(token);
        break;
    }
}

void HTMLTreeBuilder::processHtmlStartTagForInBody(AtomicHTMLToken* token)
{
    parseError(token);
    if (m_tree.openElements()->hasTemplateInHTMLScope()) {
        ASSERT(isParsingTemplateContents());
        return;
    }
    m_tree.insertHTMLHtmlStartTagInBody(token);
}

bool HTMLTreeBuilder::processBodyEndTagForInBody(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    ASSERT(token->name() == bodyTag);
    if (!m_tree.openElements()->inScope(bodyTag.localName())) {
        parseError(token);
        return false;
    }
    //notImplemented(); // Emit a more specific parse error based on stack contents. // weolar
    setInsertionMode(AfterBodyMode);
    return true;
}

void HTMLTreeBuilder::processAnyOtherEndTagForInBody(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    HTMLElementStack::ElementRecord* record = m_tree.openElements()->topRecord();
    while (1) {
        RefPtrWillBeRawPtr<HTMLStackItem> item = record->stackItem();
        if (item->matchesHTMLTag(token->name())) {
            m_tree.generateImpliedEndTagsWithExclusion(token->name());
            if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
                parseError(token);
            m_tree.openElements()->popUntilPopped(item->element());
            return;
        }
        if (item->isSpecialNode()) {
            parseError(token);
            return;
        }
        record = record->next();
    }
}

// http://www.whatwg.org/specs/web-apps/current-work/multipage/tokenization.html#parsing-main-inbody
void HTMLTreeBuilder::callTheAdoptionAgency(AtomicHTMLToken* token)
{
    // The adoption agency algorithm is N^2. We limit the number of iterations
    // to stop from hanging the whole browser. This limit is specified in the
    // adoption agency algorithm:
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/tree-construction.html#parsing-main-inbody
    static const int outerIterationLimit = 8;
    static const int innerIterationLimit = 3;

    // 1, 2, 3 and 16 are covered by the for() loop.
    for (int i = 0; i < outerIterationLimit; ++i) {
        // 4.
        Element* formattingElement = m_tree.activeFormattingElements()->closestElementInScopeWithName(token->name());
        // 4.a
        if (!formattingElement)
            return processAnyOtherEndTagForInBody(token);
        // 4.c
        if ((m_tree.openElements()->contains(formattingElement)) && !m_tree.openElements()->inScope(formattingElement)) {
            parseError(token);
            notImplemented(); // Check the stack of open elements for a more specific parse error.
            return;
        }
        // 4.b
        HTMLElementStack::ElementRecord* formattingElementRecord = m_tree.openElements()->find(formattingElement);
        if (!formattingElementRecord) {
            parseError(token);
            m_tree.activeFormattingElements()->remove(formattingElement);
            return;
        }
        // 4.d
        if (formattingElement != m_tree.currentElement())
            parseError(token);
        // 5.
        HTMLElementStack::ElementRecord* furthestBlock = m_tree.openElements()->furthestBlockForFormattingElement(formattingElement);
        // 6.
        if (!furthestBlock) {
            m_tree.openElements()->popUntilPopped(formattingElement);
            m_tree.activeFormattingElements()->remove(formattingElement);
            return;
        }
        // 7.
        ASSERT(furthestBlock->isAbove(formattingElementRecord));
        RefPtrWillBeRawPtr<HTMLStackItem> commonAncestor = formattingElementRecord->next()->stackItem();
        // 8.
        HTMLFormattingElementList::Bookmark bookmark = m_tree.activeFormattingElements()->bookmarkFor(formattingElement);
        // 9.
        HTMLElementStack::ElementRecord* node = furthestBlock;
        HTMLElementStack::ElementRecord* nextNode = node->next();
        HTMLElementStack::ElementRecord* lastNode = furthestBlock;
        // 9.1, 9.2, 9.3 and 9.11 are covered by the for() loop.
        for (int i = 0; i < innerIterationLimit; ++i) {
            // 9.4
            node = nextNode;
            ASSERT(node);
            nextNode = node->next(); // Save node->next() for the next iteration in case node is deleted in 9.5.
            // 9.5
            if (!m_tree.activeFormattingElements()->contains(node->element())) {
                m_tree.openElements()->remove(node->element());
                node = 0;
                continue;
            }
            // 9.6
            if (node == formattingElementRecord)
                break;
            // 9.7
            RefPtrWillBeRawPtr<HTMLStackItem> newItem = m_tree.createElementFromSavedToken(node->stackItem().get());

            HTMLFormattingElementList::Entry* nodeEntry = m_tree.activeFormattingElements()->find(node->element());
            nodeEntry->replaceElement(newItem);
            node->replaceElement(newItem.release());

            // 9.8
            if (lastNode == furthestBlock)
                bookmark.moveToAfter(nodeEntry);
            // 9.9
            m_tree.reparent(node, lastNode);
            // 9.10
            lastNode = node;
        }
        // 10.
        m_tree.insertAlreadyParsedChild(commonAncestor.get(), lastNode);
        // 11.
        RefPtrWillBeRawPtr<HTMLStackItem> newItem = m_tree.createElementFromSavedToken(formattingElementRecord->stackItem().get());
        // 12.
        m_tree.takeAllChildren(newItem.get(), furthestBlock);
        // 13.
        m_tree.reparent(furthestBlock, newItem.get());
        // 14.
        m_tree.activeFormattingElements()->swapTo(formattingElement, newItem, bookmark);
        // 15.
        m_tree.openElements()->remove(formattingElement);
        m_tree.openElements()->insertAbove(newItem, furthestBlock);
    }
}

void HTMLTreeBuilder::resetInsertionModeAppropriately()
{
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/parsing.html#reset-the-insertion-mode-appropriately
    bool last = false;
    HTMLElementStack::ElementRecord* nodeRecord = m_tree.openElements()->topRecord();
    while (1) {
        RefPtrWillBeRawPtr<HTMLStackItem> item = nodeRecord->stackItem();
        if (item->node() == m_tree.openElements()->rootNode()) {
            last = true;
            if (isParsingFragment())
                item = m_fragmentContext.contextElementStackItem();
        }
        if (item->hasTagName(templateTag))
            return setInsertionMode(m_templateInsertionModes.last());
        if (item->hasTagName(selectTag)) {
            if (!last) {
                while (item->node() != m_tree.openElements()->rootNode() && !item->hasTagName(templateTag)) {
                    nodeRecord = nodeRecord->next();
                    item = nodeRecord->stackItem();
                    if (item->hasTagName(tableTag))
                        return setInsertionMode(InSelectInTableMode);
                }
            }
            return setInsertionMode(InSelectMode);
        }
        if (item->hasTagName(tdTag) || item->hasTagName(thTag))
            return setInsertionMode(InCellMode);
        if (item->hasTagName(trTag))
            return setInsertionMode(InRowMode);
        if (item->hasTagName(tbodyTag) || item->hasTagName(theadTag) || item->hasTagName(tfootTag))
            return setInsertionMode(InTableBodyMode);
        if (item->hasTagName(captionTag))
            return setInsertionMode(InCaptionMode);
        if (item->hasTagName(colgroupTag)) {
            return setInsertionMode(InColumnGroupMode);
        }
        if (item->hasTagName(tableTag))
            return setInsertionMode(InTableMode);
        if (item->hasTagName(headTag)) {
            if (!m_fragmentContext.fragment() || m_fragmentContext.contextElement() != item->node())
                return setInsertionMode(InHeadMode);
            return setInsertionMode(InBodyMode);
        }
        if (item->hasTagName(bodyTag))
            return setInsertionMode(InBodyMode);
        if (item->hasTagName(framesetTag)) {
            return setInsertionMode(InFramesetMode);
        }
        if (item->hasTagName(htmlTag)) {
            if (m_tree.headStackItem())
                return setInsertionMode(AfterHeadMode);

            ASSERT(isParsingFragment());
            return setInsertionMode(BeforeHeadMode);
        }
        if (last) {
            ASSERT(isParsingFragment());
            return setInsertionMode(InBodyMode);
        }
        nodeRecord = nodeRecord->next();
    }
}

void HTMLTreeBuilder::processEndTagForInTableBody(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    if (isTableBodyContextTag(token->name())) {
        if (!m_tree.openElements()->inTableScope(token->name())) {
            parseError(token);
            return;
        }
        m_tree.openElements()->popUntilTableBodyScopeMarker();
        m_tree.openElements()->pop();
        setInsertionMode(InTableMode);
        return;
    }
    if (token->name() == tableTag) {
        // FIXME: This is slow.
        if (!m_tree.openElements()->inTableScope(tbodyTag) && !m_tree.openElements()->inTableScope(theadTag) && !m_tree.openElements()->inTableScope(tfootTag)) {
            ASSERT(isParsingFragmentOrTemplateContents());
            parseError(token);
            return;
        }
        m_tree.openElements()->popUntilTableBodyScopeMarker();
        ASSERT(isTableBodyContextTag(m_tree.currentStackItem()->localName()));
        processFakeEndTag(m_tree.currentStackItem()->localName());
        processEndTag(token);
        return;
    }
    if (token->name() == bodyTag
        || isCaptionColOrColgroupTag(token->name())
        || token->name() == htmlTag
        || isTableCellContextTag(token->name())
        || token->name() == trTag) {
        parseError(token);
        return;
    }
    processEndTagForInTable(token);
}

void HTMLTreeBuilder::processEndTagForInRow(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    if (token->name() == trTag) {
        processTrEndTagForInRow();
        return;
    }
    if (token->name() == tableTag) {
        if (!processTrEndTagForInRow()) {
            ASSERT(isParsingFragmentOrTemplateContents());
            return;
        }
        ASSERT(insertionMode() == InTableBodyMode);
        processEndTag(token);
        return;
    }
    if (isTableBodyContextTag(token->name())) {
        if (!m_tree.openElements()->inTableScope(token->name())) {
            parseError(token);
            return;
        }
        processFakeEndTag(trTag);
        ASSERT(insertionMode() == InTableBodyMode);
        processEndTag(token);
        return;
    }
    if (token->name() == bodyTag
        || isCaptionColOrColgroupTag(token->name())
        || token->name() == htmlTag
        || isTableCellContextTag(token->name())) {
        parseError(token);
        return;
    }
    processEndTagForInTable(token);
}

void HTMLTreeBuilder::processEndTagForInCell(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    if (isTableCellContextTag(token->name())) {
        if (!m_tree.openElements()->inTableScope(token->name())) {
            parseError(token);
            return;
        }
        m_tree.generateImpliedEndTags();
        if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
            parseError(token);
        m_tree.openElements()->popUntilPopped(token->name());
        m_tree.activeFormattingElements()->clearToLastMarker();
        setInsertionMode(InRowMode);
        return;
    }
    if (token->name() == bodyTag
        || isCaptionColOrColgroupTag(token->name())
        || token->name() == htmlTag) {
        parseError(token);
        return;
    }
    if (token->name() == tableTag
        || token->name() == trTag
        || isTableBodyContextTag(token->name())) {
        if (!m_tree.openElements()->inTableScope(token->name())) {
            ASSERT(isTableBodyContextTag(token->name()) || m_tree.openElements()->inTableScope(templateTag) || isParsingFragment());
            parseError(token);
            return;
        }
        closeTheCell();
        processEndTag(token);
        return;
    }
    processEndTagForInBody(token);
}

void HTMLTreeBuilder::processEndTagForInBody(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    if (token->name() == bodyTag) {
        processBodyEndTagForInBody(token);
        return;
    }
    if (token->name() == htmlTag) {
        AtomicHTMLToken endBody(HTMLToken::EndTag, bodyTag.localName());
        if (processBodyEndTagForInBody(&endBody))
            processEndTag(token);
        return;
    }
    if (token->name() == addressTag
        || token->name() == articleTag
        || token->name() == asideTag
        || token->name() == blockquoteTag
        || token->name() == buttonTag
        || token->name() == centerTag
        || token->name() == detailsTag
        || token->name() == dirTag
        || token->name() == divTag
        || token->name() == dlTag
        || token->name() == fieldsetTag
        || token->name() == figcaptionTag
        || token->name() == figureTag
        || token->name() == footerTag
        || token->name() == headerTag
        || token->name() == hgroupTag
        || token->name() == listingTag
        || token->name() == mainTag
        || token->name() == menuTag
        || token->name() == navTag
        || token->name() == olTag
        || token->name() == preTag
        || token->name() == sectionTag
        || token->name() == summaryTag
        || token->name() == ulTag) {
        if (!m_tree.openElements()->inScope(token->name())) {
            parseError(token);
            return;
        }
        m_tree.generateImpliedEndTags();
        if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
            parseError(token);
        m_tree.openElements()->popUntilPopped(token->name());
        return;
    }
    if (token->name() == formTag) {
        RefPtrWillBeRawPtr<Element> node = m_tree.takeForm();
        if (!node || !m_tree.openElements()->inScope(node.get())) {
            parseError(token);
            return;
        }
        m_tree.generateImpliedEndTags();
        if (m_tree.currentElement() != node.get())
            parseError(token);
        m_tree.openElements()->remove(node.get());
    }
    if (token->name() == pTag) {
        if (!m_tree.openElements()->inButtonScope(token->name())) {
            parseError(token);
            processFakeStartTag(pTag);
            ASSERT(m_tree.openElements()->inScope(token->name()));
            processEndTag(token);
            return;
        }
        m_tree.generateImpliedEndTagsWithExclusion(token->name());
        if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
            parseError(token);
        m_tree.openElements()->popUntilPopped(token->name());
        return;
    }
    if (token->name() == liTag) {
        if (!m_tree.openElements()->inListItemScope(token->name())) {
            parseError(token);
            return;
        }
        m_tree.generateImpliedEndTagsWithExclusion(token->name());
        if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
            parseError(token);
        m_tree.openElements()->popUntilPopped(token->name());
        return;
    }
    if (token->name() == ddTag
        || token->name() == dtTag) {
        if (!m_tree.openElements()->inScope(token->name())) {
            parseError(token);
            return;
        }
        m_tree.generateImpliedEndTagsWithExclusion(token->name());
        if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
            parseError(token);
        m_tree.openElements()->popUntilPopped(token->name());
        return;
    }
    if (isNumberedHeaderTag(token->name())) {
        if (!m_tree.openElements()->hasNumberedHeaderElementInScope()) {
            parseError(token);
            return;
        }
        m_tree.generateImpliedEndTags();
        if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
            parseError(token);
        m_tree.openElements()->popUntilNumberedHeaderElementPopped();
        return;
    }
    if (isFormattingTag(token->name())) {
        callTheAdoptionAgency(token);
        return;
    }
    if (token->name() == appletTag
        || token->name() == marqueeTag
        || token->name() == objectTag) {
        if (!m_tree.openElements()->inScope(token->name())) {
            parseError(token);
            return;
        }
        m_tree.generateImpliedEndTags();
        if (!m_tree.currentStackItem()->matchesHTMLTag(token->name()))
            parseError(token);
        m_tree.openElements()->popUntilPopped(token->name());
        m_tree.activeFormattingElements()->clearToLastMarker();
        return;
    }
    if (token->name() == brTag) {
        parseError(token);
        processFakeStartTag(brTag);
        return;
    }
    if (token->name() == templateTag) {
        processTemplateEndTag(token);
        return;
    }
    processAnyOtherEndTagForInBody(token);
}

bool HTMLTreeBuilder::processCaptionEndTagForInCaption()
{
    if (!m_tree.openElements()->inTableScope(captionTag.localName())) {
        ASSERT(isParsingFragment());
        // FIXME: parse error
        return false;
    }
    m_tree.generateImpliedEndTags();
    // FIXME: parse error if (!m_tree.currentStackItem()->hasTagName(captionTag))
    m_tree.openElements()->popUntilPopped(captionTag.localName());
    m_tree.activeFormattingElements()->clearToLastMarker();
    setInsertionMode(InTableMode);
    return true;
}

bool HTMLTreeBuilder::processTrEndTagForInRow()
{
    if (!m_tree.openElements()->inTableScope(trTag)) {
        ASSERT(isParsingFragmentOrTemplateContents());
        // FIXME: parse error
        return false;
    }
    m_tree.openElements()->popUntilTableRowScopeMarker();
    ASSERT(m_tree.currentStackItem()->hasTagName(trTag));
    m_tree.openElements()->pop();
    setInsertionMode(InTableBodyMode);
    return true;
}

bool HTMLTreeBuilder::processTableEndTagForInTable()
{
    if (!m_tree.openElements()->inTableScope(tableTag)) {
        ASSERT(isParsingFragmentOrTemplateContents());
        // FIXME: parse error.
        return false;
    }
    m_tree.openElements()->popUntilPopped(tableTag.localName());
    resetInsertionModeAppropriately();
    return true;
}

void HTMLTreeBuilder::processEndTagForInTable(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    if (token->name() == tableTag) {
        processTableEndTagForInTable();
        return;
    }
    if (token->name() == bodyTag
        || isCaptionColOrColgroupTag(token->name())
        || token->name() == htmlTag
        || isTableBodyContextTag(token->name())
        || isTableCellContextTag(token->name())
        || token->name() == trTag) {
        parseError(token);
        return;
    }
    parseError(token);
    // Is this redirection necessary here?
    HTMLConstructionSite::RedirectToFosterParentGuard redirecter(m_tree);
    processEndTagForInBody(token);
}

void HTMLTreeBuilder::processEndTag(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndTag);
    switch (insertionMode()) {
    case InitialMode:
        ASSERT(insertionMode() == InitialMode);
        defaultForInitial();
        // Fall through.
    case BeforeHTMLMode:
        ASSERT(insertionMode() == BeforeHTMLMode);
        if (token->name() != headTag && token->name() != bodyTag && token->name() != htmlTag && token->name() != brTag) {
            parseError(token);
            return;
        }
        defaultForBeforeHTML();
        // Fall through.
    case BeforeHeadMode:
        ASSERT(insertionMode() == BeforeHeadMode);
        if (token->name() != headTag && token->name() != bodyTag && token->name() != htmlTag && token->name() != brTag) {
            parseError(token);
            return;
        }
        defaultForBeforeHead();
        // Fall through.
    case InHeadMode:
        ASSERT(insertionMode() == InHeadMode);
        // FIXME: This case should be broken out into processEndTagForInHead,
        // because other end tag cases now refer to it ("process the token for using the rules of the "in head" insertion mode").
        // but because the logic falls through to AfterHeadMode, that gets a little messy.
        if (token->name() == templateTag) {
            processTemplateEndTag(token);
            return;
        }
        if (token->name() == headTag) {
            m_tree.openElements()->popHTMLHeadElement();
            setInsertionMode(AfterHeadMode);
            return;
        }
        if (token->name() != bodyTag && token->name() != htmlTag && token->name() != brTag) {
            parseError(token);
            return;
        }
        defaultForInHead();
        // Fall through.
    case AfterHeadMode:
        ASSERT(insertionMode() == AfterHeadMode);
        if (token->name() != bodyTag && token->name() != htmlTag && token->name() != brTag) {
            parseError(token);
            return;
        }
        defaultForAfterHead();
        // Fall through
    case InBodyMode:
        ASSERT(insertionMode() == InBodyMode);
        processEndTagForInBody(token);
        break;
    case InTableMode:
        ASSERT(insertionMode() == InTableMode);
        processEndTagForInTable(token);
        break;
    case InCaptionMode:
        ASSERT(insertionMode() == InCaptionMode);
        if (token->name() == captionTag) {
            processCaptionEndTagForInCaption();
            return;
        }
        if (token->name() == tableTag) {
            parseError(token);
            if (!processCaptionEndTagForInCaption()) {
                ASSERT(isParsingFragment());
                return;
            }
            processEndTag(token);
            return;
        }
        if (token->name() == bodyTag
            || token->name() == colTag
            || token->name() == colgroupTag
            || token->name() == htmlTag
            || isTableBodyContextTag(token->name())
            || isTableCellContextTag(token->name())
            || token->name() == trTag) {
            parseError(token);
            return;
        }
        processEndTagForInBody(token);
        break;
    case InColumnGroupMode:
        ASSERT(insertionMode() == InColumnGroupMode);
        if (token->name() == colgroupTag) {
            processColgroupEndTagForInColumnGroup();
            return;
        }
        if (token->name() == colTag) {
            parseError(token);
            return;
        }
        if (token->name() == templateTag) {
            processTemplateEndTag(token);
            return;
        }
        if (!processColgroupEndTagForInColumnGroup()) {
            ASSERT(isParsingFragmentOrTemplateContents());
            return;
        }
        processEndTag(token);
        break;
    case InRowMode:
        ASSERT(insertionMode() == InRowMode);
        processEndTagForInRow(token);
        break;
    case InCellMode:
        ASSERT(insertionMode() == InCellMode);
        processEndTagForInCell(token);
        break;
    case InTableBodyMode:
        ASSERT(insertionMode() == InTableBodyMode);
        processEndTagForInTableBody(token);
        break;
    case AfterBodyMode:
        ASSERT(insertionMode() == AfterBodyMode);
        if (token->name() == htmlTag) {
            if (isParsingFragment()) {
                parseError(token);
                return;
            }
            setInsertionMode(AfterAfterBodyMode);
            return;
        }
        // Fall through.
    case AfterAfterBodyMode:
        ASSERT(insertionMode() == AfterBodyMode || insertionMode() == AfterAfterBodyMode);
        parseError(token);
        setInsertionMode(InBodyMode);
        processEndTag(token);
        break;
    case InHeadNoscriptMode:
        ASSERT(insertionMode() == InHeadNoscriptMode);
        if (token->name() == noscriptTag) {
            ASSERT(m_tree.currentStackItem()->hasTagName(noscriptTag));
            m_tree.openElements()->pop();
            ASSERT(m_tree.currentStackItem()->hasTagName(headTag));
            setInsertionMode(InHeadMode);
            return;
        }
        if (token->name() != brTag) {
            parseError(token);
            return;
        }
        defaultForInHeadNoscript();
        processToken(token);
        break;
    case TextMode:
        if (token->name() == scriptTag) {
            // Pause ourselves so that parsing stops until the script can be processed by the caller.
            ASSERT(m_tree.currentStackItem()->hasTagName(scriptTag));
            if (scriptingContentIsAllowed(m_tree.parserContentPolicy()))
                m_scriptToProcess = m_tree.currentElement();
            m_tree.openElements()->pop();
            setInsertionMode(m_originalInsertionMode);

            if (m_parser->tokenizer()) {
                // We must set the tokenizer's state to
                // DataState explicitly if the tokenizer didn't have a chance to.
                m_parser->tokenizer()->setState(HTMLTokenizer::DataState);
            }
            return;
        }
        m_tree.openElements()->pop();
        setInsertionMode(m_originalInsertionMode);
        break;
    case InFramesetMode:
        ASSERT(insertionMode() == InFramesetMode);
        if (token->name() == framesetTag) {
            bool ignoreFramesetForFragmentParsing  = m_tree.currentIsRootNode();
            ignoreFramesetForFragmentParsing = ignoreFramesetForFragmentParsing || m_tree.openElements()->hasTemplateInHTMLScope();
            if (ignoreFramesetForFragmentParsing) {
                ASSERT(isParsingFragmentOrTemplateContents());
                parseError(token);
                return;
            }
            m_tree.openElements()->pop();
            if (!isParsingFragment() && !m_tree.currentStackItem()->hasTagName(framesetTag))
                setInsertionMode(AfterFramesetMode);
            return;
        }
        if (token->name() == templateTag) {
            processTemplateEndTag(token);
            return;
        }
        break;
    case AfterFramesetMode:
        ASSERT(insertionMode() == AfterFramesetMode);
        if (token->name() == htmlTag) {
            setInsertionMode(AfterAfterFramesetMode);
            return;
        }
        // Fall through.
    case AfterAfterFramesetMode:
        ASSERT(insertionMode() == AfterFramesetMode || insertionMode() == AfterAfterFramesetMode);
        parseError(token);
        break;
    case InSelectInTableMode:
        ASSERT(insertionMode() == InSelectInTableMode);
        if (token->name() == captionTag
            || token->name() == tableTag
            || isTableBodyContextTag(token->name())
            || token->name() == trTag
            || isTableCellContextTag(token->name())) {
            parseError(token);
            if (m_tree.openElements()->inTableScope(token->name())) {
                AtomicHTMLToken endSelect(HTMLToken::EndTag, selectTag.localName());
                processEndTag(&endSelect);
                processEndTag(token);
            }
            return;
        }
        // Fall through.
    case InSelectMode:
        ASSERT(insertionMode() == InSelectMode || insertionMode() == InSelectInTableMode);
        if (token->name() == optgroupTag) {
            if (m_tree.currentStackItem()->hasTagName(optionTag) && m_tree.oneBelowTop() && m_tree.oneBelowTop()->hasTagName(optgroupTag))
                processFakeEndTag(optionTag);
            if (m_tree.currentStackItem()->hasTagName(optgroupTag)) {
                m_tree.openElements()->pop();
                return;
            }
            parseError(token);
            return;
        }
        if (token->name() == optionTag) {
            if (m_tree.currentStackItem()->hasTagName(optionTag)) {
                m_tree.openElements()->pop();
                return;
            }
            parseError(token);
            return;
        }
        if (token->name() == selectTag) {
            if (!m_tree.openElements()->inSelectScope(token->name())) {
                ASSERT(isParsingFragment());
                parseError(token);
                return;
            }
            m_tree.openElements()->popUntilPopped(selectTag.localName());
            resetInsertionModeAppropriately();
            return;
        }
        if (token->name() == templateTag) {
            processTemplateEndTag(token);
            return;
        }
        break;
    case InTableTextMode:
        defaultForInTableText();
        processEndTag(token);
        break;
    case TemplateContentsMode:
        if (token->name() == templateTag) {
            processTemplateEndTag(token);
            return;
        }
        break;
    }
}

void HTMLTreeBuilder::processComment(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::Comment);
    if (m_insertionMode == InitialMode
        || m_insertionMode == BeforeHTMLMode
        || m_insertionMode == AfterAfterBodyMode
        || m_insertionMode == AfterAfterFramesetMode) {
        m_tree.insertCommentOnDocument(token);
        return;
    }
    if (m_insertionMode == AfterBodyMode) {
        m_tree.insertCommentOnHTMLHtmlElement(token);
        return;
    }
    if (m_insertionMode == InTableTextMode) {
        defaultForInTableText();
        processComment(token);
        return;
    }
    m_tree.insertComment(token);
}

void HTMLTreeBuilder::processCharacter(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::Character);
    CharacterTokenBuffer buffer(token);
    processCharacterBuffer(buffer);
}

void HTMLTreeBuilder::processCharacterBuffer(CharacterTokenBuffer& buffer)
{
ReprocessBuffer:
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/tokenization.html#parsing-main-inbody
    // Note that this logic is different than the generic \r\n collapsing
    // handled in the input stream preprocessor. This logic is here as an
    // "authoring convenience" so folks can write:
    //
    // <pre>
    // lorem ipsum
    // lorem ipsum
    // </pre>
    //
    // without getting an extra newline at the start of their <pre> element.
    if (m_shouldSkipLeadingNewline) {
        m_shouldSkipLeadingNewline = false;
        buffer.skipAtMostOneLeadingNewline();
        if (buffer.isEmpty())
            return;
    }

    switch (insertionMode()) {
    case InitialMode: {
        ASSERT(insertionMode() == InitialMode);
        buffer.skipLeadingWhitespace();
        if (buffer.isEmpty())
            return;
        defaultForInitial();
        // Fall through.
    }
    case BeforeHTMLMode: {
        ASSERT(insertionMode() == BeforeHTMLMode);
        buffer.skipLeadingWhitespace();
        if (buffer.isEmpty())
            return;
        defaultForBeforeHTML();
        // Fall through.
    }
    case BeforeHeadMode: {
        ASSERT(insertionMode() == BeforeHeadMode);
        buffer.skipLeadingWhitespace();
        if (buffer.isEmpty())
            return;
        defaultForBeforeHead();
        // Fall through.
    }
    case InHeadMode: {
        ASSERT(insertionMode() == InHeadMode);
        String leadingWhitespace = buffer.takeLeadingWhitespace();
        if (!leadingWhitespace.isEmpty())
            m_tree.insertTextNode(leadingWhitespace, AllWhitespace);
        if (buffer.isEmpty())
            return;
        defaultForInHead();
        // Fall through.
    }
    case AfterHeadMode: {
        ASSERT(insertionMode() == AfterHeadMode);
        String leadingWhitespace = buffer.takeLeadingWhitespace();
        if (!leadingWhitespace.isEmpty())
            m_tree.insertTextNode(leadingWhitespace, AllWhitespace);
        if (buffer.isEmpty())
            return;
        defaultForAfterHead();
        // Fall through.
    }
    case InBodyMode:
    case InCaptionMode:
    case TemplateContentsMode:
    case InCellMode: {
        ASSERT(insertionMode() == InBodyMode || insertionMode() == InCaptionMode || insertionMode() == InCellMode || insertionMode() == TemplateContentsMode);
        processCharacterBufferForInBody(buffer);
        break;
    }
    case InTableMode:
    case InTableBodyMode:
    case InRowMode: {
        ASSERT(insertionMode() == InTableMode || insertionMode() == InTableBodyMode || insertionMode() == InRowMode);
        ASSERT(m_pendingTableCharacters.isEmpty());
        if (m_tree.currentStackItem()->isElementNode()
            && (m_tree.currentStackItem()->hasTagName(tableTag)
                || m_tree.currentStackItem()->hasTagName(tbodyTag)
                || m_tree.currentStackItem()->hasTagName(tfootTag)
                || m_tree.currentStackItem()->hasTagName(theadTag)
                || m_tree.currentStackItem()->hasTagName(trTag))) {
            m_originalInsertionMode = m_insertionMode;
            setInsertionMode(InTableTextMode);
            // Note that we fall through to the InTableTextMode case below.
        } else {
            HTMLConstructionSite::RedirectToFosterParentGuard redirecter(m_tree);
            processCharacterBufferForInBody(buffer);
            break;
        }
        // Fall through.
    }
    case InTableTextMode: {
        buffer.giveRemainingTo(m_pendingTableCharacters);
        break;
    }
    case InColumnGroupMode: {
        ASSERT(insertionMode() == InColumnGroupMode);
        String leadingWhitespace = buffer.takeLeadingWhitespace();
        if (!leadingWhitespace.isEmpty())
            m_tree.insertTextNode(leadingWhitespace, AllWhitespace);
        if (buffer.isEmpty())
            return;
        if (!processColgroupEndTagForInColumnGroup()) {
            ASSERT(isParsingFragmentOrTemplateContents());
            // The spec tells us to drop these characters on the floor.
            buffer.skipLeadingNonWhitespace();
            if (buffer.isEmpty())
                return;
        }
        goto ReprocessBuffer;
    }
    case AfterBodyMode:
    case AfterAfterBodyMode: {
        ASSERT(insertionMode() == AfterBodyMode || insertionMode() == AfterAfterBodyMode);
        // FIXME: parse error
        setInsertionMode(InBodyMode);
        goto ReprocessBuffer;
    }
    case TextMode: {
        ASSERT(insertionMode() == TextMode);
        m_tree.insertTextNode(buffer.takeRemaining());
        break;
    }
    case InHeadNoscriptMode: {
        ASSERT(insertionMode() == InHeadNoscriptMode);
        String leadingWhitespace = buffer.takeLeadingWhitespace();
        if (!leadingWhitespace.isEmpty())
            m_tree.insertTextNode(leadingWhitespace, AllWhitespace);
        if (buffer.isEmpty())
            return;
        defaultForInHeadNoscript();
        goto ReprocessBuffer;
    }
    case InFramesetMode:
    case AfterFramesetMode: {
        ASSERT(insertionMode() == InFramesetMode || insertionMode() == AfterFramesetMode || insertionMode() == AfterAfterFramesetMode);
        String leadingWhitespace = buffer.takeRemainingWhitespace();
        if (!leadingWhitespace.isEmpty())
            m_tree.insertTextNode(leadingWhitespace, AllWhitespace);
        // FIXME: We should generate a parse error if we skipped over any
        // non-whitespace characters.
        break;
    }
    case InSelectInTableMode:
    case InSelectMode: {
        ASSERT(insertionMode() == InSelectMode || insertionMode() == InSelectInTableMode);
        m_tree.insertTextNode(buffer.takeRemaining());
        break;
    }
    case AfterAfterFramesetMode: {
        String leadingWhitespace = buffer.takeRemainingWhitespace();
        if (!leadingWhitespace.isEmpty()) {
            m_tree.reconstructTheActiveFormattingElements();
            m_tree.insertTextNode(leadingWhitespace, AllWhitespace);
        }
        // FIXME: We should generate a parse error if we skipped over any
        // non-whitespace characters.
        break;
    }
    }
}

void HTMLTreeBuilder::processCharacterBufferForInBody(CharacterTokenBuffer& buffer)
{
    m_tree.reconstructTheActiveFormattingElements();
    const String& characters = buffer.takeRemaining();
    m_tree.insertTextNode(characters);
    if (m_framesetOk && !isAllWhitespaceOrReplacementCharacters(characters))
        m_framesetOk = false;
}

void HTMLTreeBuilder::processEndOfFile(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::EndOfFile);
    switch (insertionMode()) {
    case InitialMode:
        ASSERT(insertionMode() == InitialMode);
        defaultForInitial();
        // Fall through.
    case BeforeHTMLMode:
        ASSERT(insertionMode() == BeforeHTMLMode);
        defaultForBeforeHTML();
        // Fall through.
    case BeforeHeadMode:
        ASSERT(insertionMode() == BeforeHeadMode);
        defaultForBeforeHead();
        // Fall through.
    case InHeadMode:
        ASSERT(insertionMode() == InHeadMode);
        defaultForInHead();
        // Fall through.
    case AfterHeadMode:
        ASSERT(insertionMode() == AfterHeadMode);
        defaultForAfterHead();
        // Fall through
    case InBodyMode:
    case InCellMode:
    case InCaptionMode:
    case InRowMode:
        ASSERT(insertionMode() == InBodyMode || insertionMode() == InCellMode || insertionMode() == InCaptionMode || insertionMode() == InRowMode || insertionMode() == TemplateContentsMode);
        //notImplemented(); // Emit parse error based on what elements are still open.
        if (!m_templateInsertionModes.isEmpty() && processEndOfFileForInTemplateContents(token))
            return;
        break;
    case AfterBodyMode:
    case AfterAfterBodyMode:
        ASSERT(insertionMode() == AfterBodyMode || insertionMode() == AfterAfterBodyMode);
        break;
    case InHeadNoscriptMode:
        ASSERT(insertionMode() == InHeadNoscriptMode);
        defaultForInHeadNoscript();
        processEndOfFile(token);
        return;
    case AfterFramesetMode:
    case AfterAfterFramesetMode:
        ASSERT(insertionMode() == AfterFramesetMode || insertionMode() == AfterAfterFramesetMode);
        break;
    case InColumnGroupMode:
        if (m_tree.currentIsRootNode()) {
            ASSERT(isParsingFragment());
            return; // FIXME: Should we break here instead of returning?
        }
        ASSERT(m_tree.currentNode()->hasTagName(colgroupTag) || isHTMLTemplateElement(m_tree.currentNode()));
        processColgroupEndTagForInColumnGroup();
        // Fall through
    case InFramesetMode:
    case InTableMode:
    case InTableBodyMode:
    case InSelectInTableMode:
    case InSelectMode:
        ASSERT(insertionMode() == InSelectMode || insertionMode() == InSelectInTableMode || insertionMode() == InTableMode || insertionMode() == InFramesetMode || insertionMode() == InTableBodyMode || insertionMode() == InColumnGroupMode);
        if (m_tree.currentNode() != m_tree.openElements()->rootNode())
            parseError(token);
        if (!m_templateInsertionModes.isEmpty() && processEndOfFileForInTemplateContents(token))
            return;
        break;
    case InTableTextMode:
        defaultForInTableText();
        processEndOfFile(token);
        return;
    case TextMode:
        parseError(token);
//         if (m_tree.currentStackItem()->hasTagName(scriptTag))
//             notImplemented(); // mark the script element as "already started".
        m_tree.openElements()->pop();
        ASSERT(m_originalInsertionMode != TextMode);
        setInsertionMode(m_originalInsertionMode);
        processEndOfFile(token);
        return;
    case TemplateContentsMode:
        if (processEndOfFileForInTemplateContents(token))
            return;
        break;
    }
    m_tree.processEndOfFile();
}

void HTMLTreeBuilder::defaultForInitial()
{
    m_tree.setDefaultCompatibilityMode();
    // FIXME: parse error
    setInsertionMode(BeforeHTMLMode);
}

void HTMLTreeBuilder::defaultForBeforeHTML()
{
    AtomicHTMLToken startHTML(HTMLToken::StartTag, htmlTag.localName());
    m_tree.insertHTMLHtmlStartTagBeforeHTML(&startHTML);
    setInsertionMode(BeforeHeadMode);
}

void HTMLTreeBuilder::defaultForBeforeHead()
{
    AtomicHTMLToken startHead(HTMLToken::StartTag, headTag.localName());
    processStartTag(&startHead);
}

void HTMLTreeBuilder::defaultForInHead()
{
    AtomicHTMLToken endHead(HTMLToken::EndTag, headTag.localName());
    processEndTag(&endHead);
}

void HTMLTreeBuilder::defaultForInHeadNoscript()
{
    AtomicHTMLToken endNoscript(HTMLToken::EndTag, noscriptTag.localName());
    processEndTag(&endNoscript);
}

void HTMLTreeBuilder::defaultForAfterHead()
{
    AtomicHTMLToken startBody(HTMLToken::StartTag, bodyTag.localName());
    processStartTag(&startBody);
    m_framesetOk = true;
}

void HTMLTreeBuilder::defaultForInTableText()
{
    String characters = m_pendingTableCharacters.toString();
    m_pendingTableCharacters.clear();
    if (!isAllWhitespace(characters)) {
        // FIXME: parse error
        HTMLConstructionSite::RedirectToFosterParentGuard redirecter(m_tree);
        m_tree.reconstructTheActiveFormattingElements();
        m_tree.insertTextNode(characters, NotAllWhitespace);
        m_framesetOk = false;
        setInsertionMode(m_originalInsertionMode);
        return;
    }
    m_tree.insertTextNode(characters);
    setInsertionMode(m_originalInsertionMode);
}

bool HTMLTreeBuilder::processStartTagForInHead(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    if (token->name() == htmlTag) {
        processHtmlStartTagForInBody(token);
        return true;
    }
    if (token->name() == baseTag
        || token->name() == basefontTag
        || token->name() == bgsoundTag
        || token->name() == commandTag
        || token->name() == linkTag
        || token->name() == metaTag) {
        m_tree.insertSelfClosingHTMLElement(token);
        // Note: The custom processing for the <meta> tag is done in HTMLMetaElement::process().
        return true;
    }
    if (token->name() == titleTag) {
        processGenericRCDATAStartTag(token);
        return true;
    }
    if (token->name() == noscriptTag) {
        if (m_options.scriptEnabled) {
            processGenericRawTextStartTag(token);
            return true;
        }
        m_tree.insertHTMLElement(token);
        setInsertionMode(InHeadNoscriptMode);
        return true;
    }
    if (token->name() == noframesTag || token->name() == styleTag) {
        processGenericRawTextStartTag(token);
        return true;
    }
    if (token->name() == scriptTag) {
        processScriptStartTag(token);
        return true;
    }
    if (token->name() == templateTag) {
        processTemplateStartTag(token);
        return true;
    }
    if (token->name() == headTag) {
        parseError(token);
        return true;
    }
    return false;
}

void HTMLTreeBuilder::processGenericRCDATAStartTag(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    m_tree.insertHTMLElement(token);
    if (m_parser->tokenizer())
        m_parser->tokenizer()->setState(HTMLTokenizer::RCDATAState);
    m_originalInsertionMode = m_insertionMode;
    setInsertionMode(TextMode);
}

void HTMLTreeBuilder::processGenericRawTextStartTag(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    m_tree.insertHTMLElement(token);
    if (m_parser->tokenizer())
        m_parser->tokenizer()->setState(HTMLTokenizer::RAWTEXTState);
    m_originalInsertionMode = m_insertionMode;
    setInsertionMode(TextMode);
}

void HTMLTreeBuilder::processScriptStartTag(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    m_tree.insertScriptElement(token);
    if (m_parser->tokenizer())
        m_parser->tokenizer()->setState(HTMLTokenizer::ScriptDataState);
    m_originalInsertionMode = m_insertionMode;

    TextPosition position = m_parser->textPosition();

    m_scriptToProcessStartPosition = position;

    setInsertionMode(TextMode);
}

// http://www.whatwg.org/specs/web-apps/current-work/multipage/tree-construction.html#tree-construction
bool HTMLTreeBuilder::shouldProcessTokenInForeignContent(AtomicHTMLToken* token)
{
    if (m_tree.isEmpty())
        return false;
    HTMLStackItem* adjustedCurrentNode = adjustedCurrentStackItem();

    if (adjustedCurrentNode->isInHTMLNamespace())
        return false;
    if (HTMLElementStack::isMathMLTextIntegrationPoint(adjustedCurrentNode)) {
        if (token->type() == HTMLToken::StartTag
            && token->name() != MathMLNames::mglyphTag
            && token->name() != MathMLNames::malignmarkTag)
            return false;
        if (token->type() == HTMLToken::Character)
            return false;
    }
    if (adjustedCurrentNode->hasTagName(MathMLNames::annotation_xmlTag)
        && token->type() == HTMLToken::StartTag
        && token->name() == SVGNames::svgTag)
        return false;
    if (HTMLElementStack::isHTMLIntegrationPoint(adjustedCurrentNode)) {
        if (token->type() == HTMLToken::StartTag)
            return false;
        if (token->type() == HTMLToken::Character)
            return false;
    }
    if (token->type() == HTMLToken::EndOfFile)
        return false;
    return true;
}

void HTMLTreeBuilder::processTokenInForeignContent(AtomicHTMLToken* token)
{
    if (token->type() == HTMLToken::Character) {
        const String& characters = token->characters();
        m_tree.insertTextNode(characters);
        if (m_framesetOk && !isAllWhitespaceOrReplacementCharacters(characters))
            m_framesetOk = false;
        return;
    }

    m_tree.flush(FlushAlways);
    HTMLStackItem* adjustedCurrentNode = adjustedCurrentStackItem();

    switch (token->type()) {
    case HTMLToken::Uninitialized:
        ASSERT_NOT_REACHED();
        break;
    case HTMLToken::DOCTYPE:
        parseError(token);
        break;
    case HTMLToken::StartTag: {
        if (token->name() == bTag
            || token->name() == bigTag
            || token->name() == blockquoteTag
            || token->name() == bodyTag
            || token->name() == brTag
            || token->name() == centerTag
            || token->name() == codeTag
            || token->name() == ddTag
            || token->name() == divTag
            || token->name() == dlTag
            || token->name() == dtTag
            || token->name() == emTag
            || token->name() == embedTag
            || isNumberedHeaderTag(token->name())
            || token->name() == headTag
            || token->name() == hrTag
            || token->name() == iTag
            || token->name() == imgTag
            || token->name() == liTag
            || token->name() == listingTag
            || token->name() == menuTag
            || token->name() == metaTag
            || token->name() == nobrTag
            || token->name() == olTag
            || token->name() == pTag
            || token->name() == preTag
            || token->name() == rubyTag
            || token->name() == sTag
            || token->name() == smallTag
            || token->name() == spanTag
            || token->name() == strongTag
            || token->name() == strikeTag
            || token->name() == subTag
            || token->name() == supTag
            || token->name() == tableTag
            || token->name() == ttTag
            || token->name() == uTag
            || token->name() == ulTag
            || token->name() == varTag
            || (token->name() == fontTag && (token->getAttributeItem(colorAttr) || token->getAttributeItem(faceAttr) || token->getAttributeItem(sizeAttr)))) {
            parseError(token);
            m_tree.openElements()->popUntilForeignContentScopeMarker();
            processStartTag(token);
            return;
        }
        const AtomicString& currentNamespace = adjustedCurrentNode->namespaceURI();
        if (currentNamespace == MathMLNames::mathmlNamespaceURI)
            adjustMathMLAttributes(token);
        if (currentNamespace == SVGNames::svgNamespaceURI) {
            adjustSVGTagNameCase(token);
            adjustSVGAttributes(token);
        }
        adjustForeignAttributes(token);
        m_tree.insertForeignElement(token, currentNamespace);
        break;
    }
    case HTMLToken::EndTag: {
        if (adjustedCurrentNode->namespaceURI() == SVGNames::svgNamespaceURI)
            adjustSVGTagNameCase(token);

        if (token->name() == SVGNames::scriptTag && m_tree.currentStackItem()->hasTagName(SVGNames::scriptTag)) {
            if (scriptingContentIsAllowed(m_tree.parserContentPolicy()))
                m_scriptToProcess = m_tree.currentElement();
            m_tree.openElements()->pop();
            return;
        }
        if (!m_tree.currentStackItem()->isInHTMLNamespace()) {
            // FIXME: This code just wants an Element* iterator, instead of an ElementRecord*
            HTMLElementStack::ElementRecord* nodeRecord = m_tree.openElements()->topRecord();
            if (!nodeRecord->stackItem()->hasLocalName(token->name()))
                parseError(token);
            while (1) {
                if (nodeRecord->stackItem()->hasLocalName(token->name())) {
                    m_tree.openElements()->popUntilPopped(nodeRecord->element());
                    return;
                }
                nodeRecord = nodeRecord->next();

                if (nodeRecord->stackItem()->isInHTMLNamespace())
                    break;
            }
        }
        // Otherwise, process the token according to the rules given in the section corresponding to the current insertion mode in HTML content.
        processEndTag(token);
        break;
    }
    case HTMLToken::Comment:
        m_tree.insertComment(token);
        break;
    case HTMLToken::Character:
    case HTMLToken::EndOfFile:
        ASSERT_NOT_REACHED();
        break;
    }
}

void HTMLTreeBuilder::finished()
{
    if (isParsingFragment())
        return;

    ASSERT(m_templateInsertionModes.isEmpty());
    ASSERT(m_isAttached);
    // Warning, this may detach the parser. Do not do anything else after this.
    m_tree.finishedParsing();
}

void HTMLTreeBuilder::parseError(AtomicHTMLToken*)
{
}

} // namespace blink
