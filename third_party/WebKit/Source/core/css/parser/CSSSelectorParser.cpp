// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSSelectorParser.h"

#include "core/css/CSSSelectorList.h"
#include "core/css/StyleSheetContents.h"
#include "core/frame/UseCounter.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

static void recordSelectorStats(const CSSParserContext& context, const CSSSelectorList& selectorList)
{
    if (!context.useCounter())
        return;

    for (const CSSSelector* selector = selectorList.first(); selector; selector = CSSSelectorList::next(*selector)) {
        for (const CSSSelector* current = selector; current ; current = current->tagHistory()) {
            UseCounter::Feature feature = UseCounter::NumberOfFeatures;
            switch (current->pseudoType()) {
            case CSSSelector::PseudoUnresolved:
                feature = UseCounter::CSSSelectorPseudoUnresolved;
                break;
            case CSSSelector::PseudoContent:
                feature = UseCounter::CSSSelectorPseudoContent;
                break;
            case CSSSelector::PseudoHost:
                feature = UseCounter::CSSSelectorPseudoHost;
                break;
            case CSSSelector::PseudoHostContext:
                feature = UseCounter::CSSSelectorPseudoHostContext;
                break;
            case CSSSelector::PseudoFullScreenDocument:
                feature = UseCounter::CSSSelectorPseudoFullScreenDocument;
                break;
            case CSSSelector::PseudoFullScreenAncestor:
                feature = UseCounter::CSSSelectorPseudoFullScreenAncestor;
                break;
            case CSSSelector::PseudoFullScreen:
                feature = UseCounter::CSSSelectorPseudoFullScreen;
                break;
            default:
                break;
            }
            if (feature != UseCounter::NumberOfFeatures)
                context.useCounter()->count(feature);
            if (current->selectorList())
                recordSelectorStats(context, *current->selectorList());
        }
    }
}

void CSSSelectorParser::parseSelector(CSSParserTokenRange range, const CSSParserContext& context, const AtomicString& defaultNamespace, StyleSheetContents* styleSheet, CSSSelectorList& output)
{
    CSSSelectorParser parser(context, defaultNamespace, styleSheet);
    range.consumeWhitespace();
    CSSSelectorList result;
    parser.consumeComplexSelectorList(range, result);
    if (range.atEnd()) {
        output.adopt(result);
        recordSelectorStats(context, output);
    }
    ASSERT(!(output.isValid() && parser.m_failedParsing));
}

CSSSelectorParser::CSSSelectorParser(const CSSParserContext& context, const AtomicString& defaultNamespace, StyleSheetContents* styleSheet)
: m_context(context)
, m_defaultNamespace(defaultNamespace)
, m_styleSheet(styleSheet)
, m_failedParsing(false)
{
}

void CSSSelectorParser::consumeComplexSelectorList(CSSParserTokenRange& range, CSSSelectorList& output)
{
    Vector<OwnPtr<CSSParserSelector>> selectorList;
    OwnPtr<CSSParserSelector> selector = consumeComplexSelector(range);
    if (!selector)
        return;
    selectorList.append(selector.release());
    while (!range.atEnd() && range.peek().type() == CommaToken) {
        range.consumeIncludingWhitespace();
        selector = consumeComplexSelector(range);
        if (!selector)
            return;
        selectorList.append(selector.release());
    }

    if (!m_failedParsing)
        output.adoptSelectorVector(selectorList);
}

void CSSSelectorParser::consumeCompoundSelectorList(CSSParserTokenRange& range, CSSSelectorList& output)
{
    Vector<OwnPtr<CSSParserSelector>> selectorList;
    OwnPtr<CSSParserSelector> selector = consumeCompoundSelector(range);
    range.consumeWhitespace();
    if (!selector)
        return;
    selectorList.append(selector.release());
    while (!range.atEnd() && range.peek().type() == CommaToken) {
        // FIXME: This differs from the spec grammar:
        // Spec: compound_selector S* [ COMMA S* compound_selector ]* S*
        // Impl: compound_selector S* [ COMMA S* compound_selector S* ]*
        range.consumeIncludingWhitespace();
        selector = consumeCompoundSelector(range);
        range.consumeWhitespace();
        if (!selector)
            return;
        selectorList.append(selector.release());
    }

    if (!m_failedParsing)
        output.adoptSelectorVector(selectorList);
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::consumeComplexSelector(CSSParserTokenRange& range)
{
    OwnPtr<CSSParserSelector> selector = consumeCompoundSelector(range);
    if (!selector)
        return nullptr;
    while (CSSSelector::Relation combinator = consumeCombinator(range)) {
        OwnPtr<CSSParserSelector> nextSelector = consumeCompoundSelector(range);
        if (!nextSelector)
            return combinator == CSSSelector::Descendant ? selector.release() : nullptr;
        CSSParserSelector* end = nextSelector.get();
        while (end->tagHistory())
            end = end->tagHistory();
        end->setRelation(combinator);
        if (selector->pseudoType() == CSSSelector::PseudoContent)
            end->setRelationIsAffectedByPseudoContent();
        end->setTagHistory(selector.release());

        selector = nextSelector.release();
    }

    return selector.release();
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::consumeCompoundSelector(CSSParserTokenRange& range)
{
    OwnPtr<CSSParserSelector> compoundSelector;

    AtomicString namespacePrefix;
    AtomicString elementName;
    bool hasNamespace;
    if (!consumeName(range, elementName, namespacePrefix, hasNamespace)) {
        compoundSelector = consumeSimpleSelector(range);
        if (!compoundSelector)
            return nullptr;
    }
    if (m_context.isHTMLDocument())
        elementName = elementName.lower();

    while (OwnPtr<CSSParserSelector> simpleSelector = consumeSimpleSelector(range)) {
        if (compoundSelector)
            compoundSelector = addSimpleSelectorToCompound(compoundSelector.release(), simpleSelector.release());
        else
            compoundSelector = simpleSelector.release();
    }

    if (!compoundSelector) {
        if (hasNamespace)
            return CSSParserSelector::create(determineNameInNamespace(namespacePrefix, elementName));
        return CSSParserSelector::create(QualifiedName(nullAtom, elementName, m_defaultNamespace));
    }
    prependTypeSelectorIfNeeded(namespacePrefix, elementName, compoundSelector.get());
    return compoundSelector.release();
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::consumeSimpleSelector(CSSParserTokenRange& range)
{
    const CSSParserToken& token = range.peek();
    OwnPtr<CSSParserSelector> selector;
    if (token.type() == HashToken)
        selector = consumeId(range);
    else if (token.type() == DelimiterToken && token.delimiter() == '.')
        selector = consumeClass(range);
    else if (token.type() == LeftBracketToken)
        selector = consumeAttribute(range);
    else if (token.type() == ColonToken)
        selector = consumePseudo(range);
    else
        return nullptr;
    if (!selector)
        m_failedParsing = true;
    return selector.release();
}

bool CSSSelectorParser::consumeName(CSSParserTokenRange& range, AtomicString& name, AtomicString& namespacePrefix, bool& hasNamespace)
{
    name = nullAtom;
    namespacePrefix = nullAtom;
    hasNamespace = false;

    const CSSParserToken& firstToken = range.peek();
    if (firstToken.type() == IdentToken) {
        name = firstToken.value();
        range.consume();
    } else if (firstToken.type() == DelimiterToken && firstToken.delimiter() == '*') {
        name = starAtom;
        range.consume();
    } else if (firstToken.type() == DelimiterToken && firstToken.delimiter() == '|') {
        // No namespace
    } else {
        return false;
    }

    if (range.peek().type() != DelimiterToken || range.peek().delimiter() != '|')
        return true;
    range.consume();

    hasNamespace = true;
    namespacePrefix = name;
    const CSSParserToken& nameToken = range.consume();
    if (nameToken.type() == IdentToken) {
        name = nameToken.value();
    } else if (nameToken.type() == DelimiterToken && nameToken.delimiter() == '*') {
        name = starAtom;
    } else {
        name = nullAtom;
        namespacePrefix = nullAtom;
        return false;
    }

    return true;
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::consumeId(CSSParserTokenRange& range)
{
    ASSERT(range.peek().type() == HashToken);
    if (range.peek().hashTokenType() != HashTokenId)
        return nullptr;
    OwnPtr<CSSParserSelector> selector = CSSParserSelector::create();
    selector->setMatch(CSSSelector::Id);
    const AtomicString& value = range.consume().value();
    if (isQuirksModeBehavior(m_context.mode()))
        selector->setValue(value.lower());
    else
        selector->setValue(value);
    return selector.release();
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::consumeClass(CSSParserTokenRange& range)
{
    ASSERT(range.peek().type() == DelimiterToken);
    ASSERT(range.peek().delimiter() == '.');
    range.consume();
    if (range.peek().type() != IdentToken)
        return nullptr;
    OwnPtr<CSSParserSelector> selector = CSSParserSelector::create();
    selector->setMatch(CSSSelector::Class);
    const AtomicString& value = range.consume().value();
    if (isQuirksModeBehavior(m_context.mode()))
        selector->setValue(value.lower());
    else
        selector->setValue(value);
    return selector.release();
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::consumeAttribute(CSSParserTokenRange& range)
{
    ASSERT(range.peek().type() == LeftBracketToken);
    CSSParserTokenRange block = range.consumeBlock();
    block.consumeWhitespace();

    AtomicString namespacePrefix;
    AtomicString attributeName;
    bool hasNamespace;
    if (!consumeName(block, attributeName, namespacePrefix, hasNamespace))
        return nullptr;
    block.consumeWhitespace();

    if (m_context.isHTMLDocument())
        attributeName = attributeName.lower();

    QualifiedName qualifiedName = hasNamespace
        ? determineNameInNamespace(namespacePrefix, attributeName)
        : QualifiedName(nullAtom, attributeName, nullAtom);

    OwnPtr<CSSParserSelector> selector = CSSParserSelector::create();

    if (block.atEnd()) {
        selector->setAttribute(qualifiedName, CSSSelector::CaseSensitive);
        selector->setMatch(CSSSelector::AttributeSet);
        return selector.release();
    }

    selector->setMatch(consumeAttributeMatch(block));

    const CSSParserToken& attributeValue = block.consumeIncludingWhitespace();
    if (attributeValue.type() != IdentToken && attributeValue.type() != StringToken)
        return nullptr;
    selector->setValue(attributeValue.value());
    selector->setAttribute(qualifiedName, consumeAttributeFlags(block));

    if (!block.atEnd())
        return nullptr;
    return selector.release();
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::consumePseudo(CSSParserTokenRange& range)
{
    ASSERT(range.peek().type() == ColonToken);
    range.consume();

    int colons = 1;
    if (range.peek().type() == ColonToken) {
        range.consume();
        colons++;
    }

    const CSSParserToken& token = range.peek();
    if (token.type() != IdentToken && token.type() != FunctionToken)
        return nullptr;

    OwnPtr<CSSParserSelector> selector = CSSParserSelector::create();
    selector->setMatch(colons == 1 ? CSSSelector::PseudoClass : CSSSelector::PseudoElement);

    String value = token.value();
    bool hasArguments = token.type() == FunctionToken;
    selector->updatePseudoType(AtomicString(value.is8Bit() ? value.lower() : value), hasArguments);

    if (token.type() == IdentToken) {
        range.consume();
        if (selector->pseudoType() == CSSSelector::PseudoUnknown)
            return nullptr;
        return selector.release();
    }

    CSSParserTokenRange block = range.consumeBlock();
    block.consumeWhitespace();
    if (selector->pseudoType() == CSSSelector::PseudoUnknown)
        return nullptr;

    switch (selector->pseudoType()) {
    case CSSSelector::PseudoHost:
    case CSSSelector::PseudoHostContext:
    case CSSSelector::PseudoAny:
    case CSSSelector::PseudoCue:
        {
            OwnPtr<CSSSelectorList> selectorList = adoptPtr(new CSSSelectorList());
            consumeCompoundSelectorList(block, *selectorList);
            if (!selectorList->isValid() || !block.atEnd())
                return nullptr;
            selector->setSelectorList(selectorList.release());
            return selector.release();
        }
    case CSSSelector::PseudoNot:
        {
            OwnPtr<CSSParserSelector> innerSelector = consumeCompoundSelector(block);
            block.consumeWhitespace();
            if (!innerSelector || !innerSelector->isSimple() || !block.atEnd())
                return nullptr;
            Vector<OwnPtr<CSSParserSelector>> selectorVector;
            selectorVector.append(innerSelector.release());
            selector->adoptSelectorVector(selectorVector);
            return selector.release();
        }
    case CSSSelector::PseudoLang:
        {
            // FIXME: CSS Selectors Level 4 allows :lang(*-foo)
            const CSSParserToken& ident = block.consumeIncludingWhitespace();
            if (ident.type() != IdentToken || !block.atEnd())
                return nullptr;
            selector->setArgument(ident.value());
            return selector.release();
        }
    case CSSSelector::PseudoNthChild:
    case CSSSelector::PseudoNthLastChild:
    case CSSSelector::PseudoNthOfType:
    case CSSSelector::PseudoNthLastOfType:
        {
            std::pair<int, int> ab;
            if (!consumeANPlusB(block, ab))
                return nullptr;
            block.consumeWhitespace();
            if (!block.atEnd())
                return nullptr;
            selector->setNth(ab.first, ab.second);
            return selector.release();
        }
    default:
        break;
    }

    return nullptr;
}

CSSSelector::Relation CSSSelectorParser::consumeCombinator(CSSParserTokenRange& range)
{
    CSSSelector::Relation fallbackResult = CSSSelector::SubSelector;
    while (range.peek().type() == WhitespaceToken) {
        range.consume();
        fallbackResult = CSSSelector::Descendant;
    }

    if (range.peek().type() != DelimiterToken)
        return fallbackResult;

    UChar delimiter = range.peek().delimiter();

    if (delimiter == '+' || delimiter == '~' || delimiter == '>') {
        range.consumeIncludingWhitespace();
        if (delimiter == '+')
            return CSSSelector::DirectAdjacent;
        if (delimiter == '~')
            return CSSSelector::IndirectAdjacent;
        return CSSSelector::Child;
    }

    // Match /deep/
    if (delimiter != '/')
        return fallbackResult;
    range.consume();
    const CSSParserToken& ident = range.consume();
    if (ident.type() != IdentToken || !ident.valueEqualsIgnoringCase("deep"))
        m_failedParsing = true;
    const CSSParserToken& slash = range.consumeIncludingWhitespace();
    if (slash.type() != DelimiterToken || slash.delimiter() != '/')
        m_failedParsing = true;
    return CSSSelector::ShadowDeep;
}

CSSSelector::Match CSSSelectorParser::consumeAttributeMatch(CSSParserTokenRange& range)
{
    const CSSParserToken& token = range.consumeIncludingWhitespace();
    switch (token.type()) {
    case IncludeMatchToken:
        return CSSSelector::AttributeList;
    case DashMatchToken:
        return CSSSelector::AttributeHyphen;
    case PrefixMatchToken:
        return CSSSelector::AttributeBegin;
    case SuffixMatchToken:
        return CSSSelector::AttributeEnd;
    case SubstringMatchToken:
        return CSSSelector::AttributeContain;
    case DelimiterToken:
        if (token.delimiter() == '=')
            return CSSSelector::AttributeExact;
    default:
        m_failedParsing = true;
        return CSSSelector::AttributeExact;
    }
}

CSSSelector::AttributeMatchType CSSSelectorParser::consumeAttributeFlags(CSSParserTokenRange& range)
{
    if (range.peek().type() != IdentToken)
        return CSSSelector::CaseSensitive;
    const CSSParserToken& flag = range.consumeIncludingWhitespace();
    if (String(flag.value()) == "i") {
        if (RuntimeEnabledFeatures::cssAttributeCaseSensitivityEnabled() || isUASheetBehavior(m_context.mode()))
            return CSSSelector::CaseInsensitive;
    }
    m_failedParsing = true;
    return CSSSelector::CaseSensitive;
}

bool CSSSelectorParser::consumeANPlusB(CSSParserTokenRange& range, std::pair<int, int>& result)
{
    const CSSParserToken& token = range.consume();
    if (token.type() == NumberToken && token.numericValueType() == IntegerValueType) {
        result = std::make_pair(0, static_cast<int>(token.numericValue()));
        return true;
    }
    if (token.type() == IdentToken) {
        if (token.valueEqualsIgnoringCase("odd")) {
            result = std::make_pair(2, 1);
            return true;
        }
        if (token.valueEqualsIgnoringCase("even")) {
            result = std::make_pair(2, 0);
            return true;
        }
    }

    // The 'n' will end up as part of an ident or dimension. For a valid <an+b>,
    // this will store a string of the form 'n', 'n-', or 'n-123'.
    String nString;

    if (token.type() == DelimiterToken && token.delimiter() == '+' && range.peek().type() == IdentToken) {
        result.first = 1;
        nString = range.consume().value();
    } else if (token.type() == DimensionToken && token.numericValueType() == IntegerValueType) {
        result.first = token.numericValue();
        nString = token.value();
    } else if (token.type() == IdentToken) {
        if (token.value()[0] == '-') {
            result.first = -1;
            nString = String(token.value()).substring(1);
        } else {
            result.first = 1;
            nString = token.value();
        }
    }

    range.consumeWhitespace();

    if (nString.isEmpty() || !isASCIIAlphaCaselessEqual(nString[0], 'n'))
        return false;
    if (nString.length() > 1 && nString[1] != '-')
        return false;

    if (nString.length() > 2) {
        bool valid;
        result.second = nString.substring(1).toIntStrict(&valid);
        return valid;
    }

    NumericSign sign = nString.length() == 1 ? NoSign : MinusSign;
    if (sign == NoSign && range.peek().type() == DelimiterToken) {
        char delimiterSign = range.consumeIncludingWhitespace().delimiter();
        if (delimiterSign == '+')
            sign = PlusSign;
        else if (delimiterSign == '-')
            sign = MinusSign;
        else
            return false;
    }

    if (sign == NoSign && range.peek().type() != NumberToken) {
        result.second = 0;
        return true;
    }

    const CSSParserToken& b = range.consume();
    if (b.type() != NumberToken || b.numericValueType() != IntegerValueType)
        return false;
    if ((b.numericSign() == NoSign) == (sign == NoSign))
        return false;
    result.second = b.numericValue();
    if (sign == MinusSign)
        result.second = -result.second;
    return true;
}

QualifiedName CSSSelectorParser::determineNameInNamespace(const AtomicString& prefix, const AtomicString& localName)
{
    if (!m_styleSheet)
        return QualifiedName(prefix, localName, m_defaultNamespace);
    return QualifiedName(prefix, localName, m_styleSheet->determineNamespace(prefix));
}

void CSSSelectorParser::prependTypeSelectorIfNeeded(const AtomicString& namespacePrefix, const AtomicString& elementName, CSSParserSelector* compoundSelector)
{
    if (elementName.isNull() && m_defaultNamespace == starAtom && !compoundSelector->crossesTreeScopes())
        return;

    AtomicString determinedElementName = elementName.isNull() ? starAtom : elementName;
    AtomicString determinedNamespace = namespacePrefix != nullAtom && m_styleSheet ? m_styleSheet->determineNamespace(namespacePrefix) : m_defaultNamespace;
    QualifiedName tag(namespacePrefix, determinedElementName, determinedNamespace);

    if (compoundSelector->crossesTreeScopes())
        return rewriteSpecifiersWithElementNameForCustomPseudoElement(tag, compoundSelector, elementName.isNull());

    if (compoundSelector->pseudoType() == CSSSelector::PseudoContent)
        return rewriteSpecifiersWithElementNameForContentPseudoElement(tag, compoundSelector, elementName.isNull());

    // *:host never matches, so we can't discard the * otherwise we can't tell the
    // difference between *:host and just :host.
    if (tag == anyQName() && !compoundSelector->hasHostPseudoSelector())
        return;
    compoundSelector->prependTagSelector(tag, elementName.isNull());
}

void CSSSelectorParser::rewriteSpecifiersWithElementNameForCustomPseudoElement(const QualifiedName& tag, CSSParserSelector* specifiers, bool tagIsImplicit)
{
    CSSParserSelector* lastShadowPseudo = specifiers;
    CSSParserSelector* history = specifiers;
    while (history->tagHistory()) {
        history = history->tagHistory();
        if (history->crossesTreeScopes() || history->hasShadowPseudo())
            lastShadowPseudo = history;
    }

    if (lastShadowPseudo->tagHistory()) {
        if (tag != anyQName())
            lastShadowPseudo->tagHistory()->prependTagSelector(tag, tagIsImplicit);
        return;
    }

    // For shadow-ID pseudo-elements to be correctly matched, the ShadowPseudo combinator has to be used.
    // We therefore create a new Selector with that combinator here in any case, even if matching any (host) element in any namespace (i.e. '*').
    OwnPtr<CSSParserSelector> elementNameSelector = CSSParserSelector::create(tag);
    lastShadowPseudo->setTagHistory(elementNameSelector.release());
    lastShadowPseudo->setRelation(CSSSelector::ShadowPseudo);
}

void CSSSelectorParser::rewriteSpecifiersWithElementNameForContentPseudoElement(const QualifiedName& tag, CSSParserSelector* specifiers, bool tagIsImplicit)
{
    CSSParserSelector* last = specifiers;
    CSSParserSelector* history = specifiers;
    while (history->tagHistory()) {
        history = history->tagHistory();
        if (history->pseudoType() == CSSSelector::PseudoContent || history->relationIsAffectedByPseudoContent())
            last = history;
    }

    if (last->tagHistory()) {
        if (tag != anyQName())
            last->tagHistory()->prependTagSelector(tag, tagIsImplicit);
        return;
    }

    // For shadow-ID pseudo-elements to be correctly matched, the ShadowPseudo combinator has to be used.
    // We therefore create a new Selector with that combinator here in any case, even if matching any (host) element in any namespace (i.e. '*').
    OwnPtr<CSSParserSelector> elementNameSelector = CSSParserSelector::create(tag);
    last->setTagHistory(elementNameSelector.release());
}

PassOwnPtr<CSSParserSelector> CSSSelectorParser::addSimpleSelectorToCompound(PassOwnPtr<CSSParserSelector> compoundSelector, PassOwnPtr<CSSParserSelector> simpleSelector)
{
    // The tagHistory is a linked list that stores combinator separated compound selectors
    // from right-to-left. Yet, within a single compound selector, stores the simple selectors
    // from left-to-right.
    //
    // ".a.b > div#id" is stored in a tagHistory as [div, #id, .a, .b], each element in the
    // list stored with an associated relation (combinator or SubSelector).
    //
    // ::cue, ::shadow, and custom pseudo elements have an implicit ShadowPseudo combinator
    // to their left, which really makes for a new compound selector, yet it's consumed by
    // the selector parser as a single compound selector.
    //
    // Example: input#x::-webkit-clear-button -> [ ::-webkit-clear-button, input, #x ]
    //
    // ::content is kept at the end of the compound in order easily know when to call
    // setRelationIsAffectedByPseudoContent.
    //
    // We are currently not dropping selectors containing multiple instances of ::content,
    // ::shadow, ::cue, and custom pseudo elements in arbitrary order. There are known
    // issues like crbug.com/478563
    //
    // TODO(rune@opera.com): We should try to remove the need for the re-ordering tricks
    // below and in the remaining rewrite* methods by using a more suitable storage
    // structure in CSSSelectorParser.
    //
    // The code below is to keep ::content at the end of the compound, and to keep the
    // tagHistory order correct for implicit ShadowPseudo and juggling multiple (two?)
    // compounds.

    CSSSelector::Relation relation = CSSSelector::SubSelector;

    if (simpleSelector->crossesTreeScopes() || simpleSelector->pseudoType() == CSSSelector::PseudoContent) {
        if (simpleSelector->crossesTreeScopes())
            relation = CSSSelector::ShadowPseudo;
        simpleSelector->appendTagHistory(relation, compoundSelector);
        return simpleSelector;
    }
    if (compoundSelector->crossesTreeScopes() || compoundSelector->pseudoType() == CSSSelector::PseudoContent) {
        if (compoundSelector->crossesTreeScopes())
            relation = CSSSelector::ShadowPseudo;
        compoundSelector->insertTagHistory(CSSSelector::SubSelector, simpleSelector, relation);
        return compoundSelector;
    }

    // All other simple selectors are added to the end of the compound.
    compoundSelector->appendTagHistory(CSSSelector::SubSelector, simpleSelector);
    return compoundSelector;
}

} // namespace blink
