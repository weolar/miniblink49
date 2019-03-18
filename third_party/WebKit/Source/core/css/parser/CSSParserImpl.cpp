// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSParserImpl.h"

#include "core/css/CSSCustomPropertyDeclaration.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/StylePropertySet.h"
#include "core/css/StyleRuleImport.h"
#include "core/css/StyleRuleKeyframe.h"
#include "core/css/StyleRuleNamespace.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/parser/CSSParserObserver.h"
#include "core/css/parser/CSSParserObserverWrapper.h"
#include "core/css/parser/CSSParserValues.h"
#include "core/css/parser/CSSPropertyParser.h"
#include "core/css/parser/CSSSelectorParser.h"
#include "core/css/parser/CSSSupportsParser.h"
#include "core/css/parser/CSSTokenizer.h"
#include "core/css/parser/CSSVariableParser.h"
#include "core/css/parser/MediaQueryParser.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/frame/UseCounter.h"
#include "platform/TraceEvent.h"
#include "wtf/BitArray.h"

namespace blink {

CSSParserImpl::CSSParserImpl(const CSSParserContext& context, StyleSheetContents* styleSheet)
    : m_context(context)
    , m_defaultNamespace(starAtom)
    , m_styleSheet(styleSheet)
    , m_observerWrapper(nullptr)
{
}

bool CSSParserImpl::parseValue(MutableStylePropertySet* declaration, CSSPropertyID unresolvedProperty, const String& string, bool important, const CSSParserContext& context)
{
    CSSParserImpl parser(context);
    StyleRule::Type ruleType = StyleRule::Style;
    if (declaration->cssParserMode() == CSSViewportRuleMode)
        ruleType = StyleRule::Viewport;
    CSSTokenizer::Scope scope(string);
    parser.consumeDeclarationValue(scope.tokenRange(), unresolvedProperty, important, ruleType);
    if (parser.m_parsedProperties.isEmpty())
        return false;
    return declaration->addParsedProperties(parser.m_parsedProperties);
}

static inline void filterProperties(bool important, const WillBeHeapVector<CSSProperty, 256>& input, WillBeHeapVector<CSSProperty, 256>& output, size_t& unusedEntries, BitArray<numCSSProperties>& seenProperties)
{
    // Add properties in reverse order so that highest priority definitions are reached first. Duplicate definitions can then be ignored when found.
    for (size_t i = input.size(); i--; ) {
        const CSSProperty& property = input[i];
        if (property.isImportant() != important)
            continue;
        const unsigned propertyIDIndex = property.id() - firstCSSProperty;
        // All custom properties use the same CSSPropertyID so we can't remove repeated definitions
        if (property.id() != CSSPropertyVariable) {
            if (seenProperties.get(propertyIDIndex))
                continue;
            seenProperties.set(propertyIDIndex);
        }

        output[--unusedEntries] = property;
    }
}

static PassRefPtrWillBeRawPtr<ImmutableStylePropertySet> createStylePropertySet(WillBeHeapVector<CSSProperty, 256>& parsedProperties, CSSParserMode mode)
{
    BitArray<numCSSProperties> seenProperties;
    size_t unusedEntries = parsedProperties.size();
    WillBeHeapVector<CSSProperty, 256> results(unusedEntries);

    filterProperties(true, parsedProperties, results, unusedEntries, seenProperties);
    filterProperties(false, parsedProperties, results, unusedEntries, seenProperties);

    RefPtrWillBeRawPtr<ImmutableStylePropertySet> result = ImmutableStylePropertySet::create(results.data() + unusedEntries, results.size() - unusedEntries, mode);
    parsedProperties.clear();
    return result.release();
}

PassRefPtrWillBeRawPtr<ImmutableStylePropertySet> CSSParserImpl::parseInlineStyleDeclaration(const String& string, Element* element)
{
    Document& document = element->document();
    CSSParserContext context = CSSParserContext(document.elementSheet().contents()->parserContext(), UseCounter::getFrom(&document));
    CSSParserMode mode = element->isHTMLElement() && !document.inQuirksMode() ? HTMLStandardMode : HTMLQuirksMode;
    context.setMode(mode);
    CSSParserImpl parser(context, document.elementSheet().contents());
    CSSTokenizer::Scope scope(string);
    parser.consumeDeclarationList(scope.tokenRange(), StyleRule::Style);
    return createStylePropertySet(parser.m_parsedProperties, mode);
}

bool CSSParserImpl::parseDeclarationList(MutableStylePropertySet* declaration, const String& string, const CSSParserContext& context)
{
    CSSParserImpl parser(context);
    StyleRule::Type ruleType = StyleRule::Style;
    if (declaration->cssParserMode() == CSSViewportRuleMode)
        ruleType = StyleRule::Viewport;
    CSSTokenizer::Scope scope(string);
    parser.consumeDeclarationList(scope.tokenRange(), ruleType);
    if (parser.m_parsedProperties.isEmpty())
        return false;

    BitArray<numCSSProperties> seenProperties;
    size_t unusedEntries = parser.m_parsedProperties.size();
    WillBeHeapVector<CSSProperty, 256> results(unusedEntries);
    filterProperties(true, parser.m_parsedProperties, results, unusedEntries, seenProperties);
    filterProperties(false, parser.m_parsedProperties, results, unusedEntries, seenProperties);
    if (unusedEntries)
        results.remove(0, unusedEntries);
    return declaration->addParsedProperties(results);
}

PassRefPtrWillBeRawPtr<StyleRuleBase> CSSParserImpl::parseRule(const String& string, const CSSParserContext& context, StyleSheetContents* styleSheet, AllowedRulesType allowedRules)
{
    CSSParserImpl parser(context, styleSheet);
    CSSTokenizer::Scope scope(string);
    CSSParserTokenRange range = scope.tokenRange();
    range.consumeWhitespace();
    if (range.atEnd())
        return nullptr; // Parse error, empty rule
    RefPtrWillBeRawPtr<StyleRuleBase> rule;
    if (range.peek().type() == AtKeywordToken)
        rule = parser.consumeAtRule(range, allowedRules);
    else
        rule = parser.consumeQualifiedRule(range, allowedRules);
    if (!rule)
        return nullptr; // Parse error, failed to consume rule
    range.consumeWhitespace();
    if (!rule || !range.atEnd())
        return nullptr; // Parse error, trailing garbage
    return rule;
}

void CSSParserImpl::parseStyleSheet(const String& string, const CSSParserContext& context, StyleSheetContents* styleSheet)
{
    TRACE_EVENT_BEGIN2(
        "blink,blink_style", "CSSParserImpl::parseStyleSheet",
        "baseUrl", context.baseURL().string().utf8(),
        "mode", context.mode());

    TRACE_EVENT_BEGIN0("blink,blink_style", "CSSParserImpl::parseStyleSheet.tokenize");
    CSSTokenizer::Scope scope(string);
    TRACE_EVENT_END0("blink,blink_style", "CSSParserImpl::parseStyleSheet.tokenize");

    TRACE_EVENT_BEGIN0("blink,blink_style", "CSSParserImpl::parseStyleSheet.parse");
    CSSParserImpl parser(context, styleSheet);
    bool firstRuleValid = parser.consumeRuleList(scope.tokenRange(), TopLevelRuleList, [&styleSheet](PassRefPtrWillBeRawPtr<StyleRuleBase> rule) {
        if (rule->isCharsetRule())
            return;
        styleSheet->parserAppendRule(rule);
    });
    styleSheet->setHasSyntacticallyValidCSSHeader(firstRuleValid);
    TRACE_EVENT_END0("blink,blink_style", "CSSParserImpl::parseStyleSheet.parse");

    TRACE_EVENT_END2(
        "blink,blink_style", "CSSParserImpl::parseStyleSheet",
        "tokenCount", scope.tokenCount(),
        "length", string.length());
}

PassOwnPtr<Vector<double>> CSSParserImpl::parseKeyframeKeyList(const String& keyList)
{
    return consumeKeyframeKeyList(CSSTokenizer::Scope(keyList).tokenRange());
}

bool CSSParserImpl::supportsDeclaration(CSSParserTokenRange& range)
{
    ASSERT(m_parsedProperties.isEmpty());
    consumeDeclaration(range, StyleRule::Style);
    bool result = !m_parsedProperties.isEmpty();
    m_parsedProperties.clear();
    return result;
}

void CSSParserImpl::parseDeclarationListForInspector(const String& declaration, const CSSParserContext& context, CSSParserObserver& observer)
{
    CSSParserImpl parser(context);
    CSSParserObserverWrapper wrapper(observer);
    parser.m_observerWrapper = &wrapper;
    CSSTokenizer::Scope scope(declaration, wrapper);
    observer.startRuleHeader(StyleRule::Style, 0);
    observer.endRuleHeader(1);
    parser.consumeDeclarationList(scope.tokenRange(), StyleRule::Style);
}

void CSSParserImpl::parseStyleSheetForInspector(const String& string, const CSSParserContext& context, StyleSheetContents* styleSheet, CSSParserObserver& observer)
{
    CSSParserImpl parser(context, styleSheet);
    CSSParserObserverWrapper wrapper(observer);
    parser.m_observerWrapper = &wrapper;
    CSSTokenizer::Scope scope(string, wrapper);
    bool firstRuleValid = parser.consumeRuleList(scope.tokenRange(), TopLevelRuleList, [&styleSheet](PassRefPtrWillBeRawPtr<StyleRuleBase> rule) {
        if (rule->isCharsetRule())
            return;
        styleSheet->parserAppendRule(rule);
    });
    styleSheet->setHasSyntacticallyValidCSSHeader(firstRuleValid);
}

static CSSParserImpl::AllowedRulesType computeNewAllowedRules(CSSParserImpl::AllowedRulesType allowedRules, StyleRuleBase* rule)
{
    if (!rule || allowedRules == CSSParserImpl::KeyframeRules || allowedRules == CSSParserImpl::NoRules)
        return allowedRules;
    ASSERT(allowedRules <= CSSParserImpl::RegularRules);
    if (rule->isCharsetRule() || rule->isImportRule())
        return CSSParserImpl::AllowImportRules;
    if (rule->isNamespaceRule())
        return CSSParserImpl::AllowNamespaceRules;
    return CSSParserImpl::RegularRules;
}

template<typename T>
bool CSSParserImpl::consumeRuleList(CSSParserTokenRange range, RuleListType ruleListType, const T callback)
{
    AllowedRulesType allowedRules = RegularRules;
    switch (ruleListType) {
    case TopLevelRuleList:
        allowedRules = AllowCharsetRules;
        break;
    case RegularRuleList:
        allowedRules = RegularRules;
        break;
    case KeyframesRuleList:
        allowedRules = KeyframeRules;
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    bool seenRule = false;
    bool firstRuleValid = false;
    while (!range.atEnd()) {
        RefPtrWillBeRawPtr<StyleRuleBase> rule;
        switch (range.peek().type()) {
        case WhitespaceToken:
            range.consumeWhitespace();
            continue;
        case AtKeywordToken:
            rule = consumeAtRule(range, allowedRules);
            break;
        case CDOToken:
        case CDCToken:
            if (ruleListType == TopLevelRuleList) {
                range.consume();
                continue;
            }
            // fallthrough
        default:
            rule = consumeQualifiedRule(range, allowedRules);
            break;
        }
        if (!seenRule) {
            seenRule = true;
            firstRuleValid = rule;
        }
        if (rule) {
            allowedRules = computeNewAllowedRules(allowedRules, rule.get());
            callback(rule.release());
        }
    }

    return firstRuleValid;
}

PassRefPtrWillBeRawPtr<StyleRuleBase> CSSParserImpl::consumeAtRule(CSSParserTokenRange& range, AllowedRulesType allowedRules)
{
    ASSERT(range.peek().type() == AtKeywordToken);
    const CSSParserString& name = range.consume().value();
    const CSSParserToken* preludeStart = &range.peek();
    while (!range.atEnd() && range.peek().type() != LeftBraceToken && range.peek().type() != SemicolonToken)
        range.consumeComponentValue();

    CSSParserTokenRange prelude = range.makeSubRange(preludeStart, &range.peek());

    if (range.atEnd() || range.peek().type() == SemicolonToken) {
        range.consume();
        if (allowedRules == AllowCharsetRules && name.equalIgnoringCase("charset"))
            return consumeCharsetRule(prelude);
        if (allowedRules <= AllowImportRules && name.equalIgnoringCase("import"))
            return consumeImportRule(prelude);
        if (allowedRules <= AllowNamespaceRules && name.equalIgnoringCase("namespace"))
            return consumeNamespaceRule(prelude);
        return nullptr; // Parse error, unrecognised at-rule without block
    }

    CSSParserTokenRange block = range.consumeBlock();
    if (allowedRules == KeyframeRules)
        return nullptr; // Parse error, no at-rules supported inside @keyframes
    if (allowedRules == NoRules)
        return nullptr; // Parse error, no at-rules supported inside declaration lists

    ASSERT(allowedRules <= RegularRules);

    if (name.equalIgnoringCase("media"))
        return consumeMediaRule(prelude, block);
    if (name.equalIgnoringCase("supports"))
        return consumeSupportsRule(prelude, block);
    if (name.equalIgnoringCase("viewport"))
        return consumeViewportRule(prelude, block);
    if (name.equalIgnoringCase("font-face"))
        return consumeFontFaceRule(prelude, block);
    if (name.equalIgnoringCase("-webkit-keyframes"))
        return consumeKeyframesRule(true, prelude, block);
    if (name.equalIgnoringCase("keyframes"))
        return consumeKeyframesRule(false, prelude, block);
    if (name.equalIgnoringCase("page"))
        return consumePageRule(prelude, block);
    return nullptr; // Parse error, unrecognised at-rule with block
}

PassRefPtrWillBeRawPtr<StyleRuleBase> CSSParserImpl::consumeQualifiedRule(CSSParserTokenRange& range, AllowedRulesType allowedRules)
{
    const CSSParserToken* preludeStart = &range.peek();
    while (!range.atEnd() && range.peek().type() != LeftBraceToken)
        range.consumeComponentValue();

    if (range.atEnd())
        return nullptr; // Parse error, EOF instead of qualified rule block

    CSSParserTokenRange prelude = range.makeSubRange(preludeStart, &range.peek());
    CSSParserTokenRange block = range.consumeBlock();

    if (allowedRules <= RegularRules)
        return consumeStyleRule(prelude, block);
    if (allowedRules == KeyframeRules)
        return consumeKeyframeStyleRule(prelude, block);

    ASSERT_NOT_REACHED();
    return nullptr;
}

// This may still consume tokens if it fails
static AtomicString consumeStringOrURI(CSSParserTokenRange& range)
{
    const CSSParserToken& token = range.peek();

    if (token.type() == StringToken || token.type() == UrlToken)
        return range.consumeIncludingWhitespace().value();

    if (token.type() != FunctionToken || !token.valueEqualsIgnoringCase("url"))
        return AtomicString();

    CSSParserTokenRange contents = range.consumeBlock();
    const CSSParserToken& uri = contents.consumeIncludingWhitespace();
    ASSERT(uri.type() == StringToken);
    if (!contents.atEnd())
        return AtomicString();
    return uri.value();
}

PassRefPtrWillBeRawPtr<StyleRuleCharset> CSSParserImpl::consumeCharsetRule(CSSParserTokenRange prelude)
{
    prelude.consumeWhitespace();
    const CSSParserToken& string = prelude.consumeIncludingWhitespace();
    if (string.type() != StringToken || !prelude.atEnd())
        return nullptr; // Parse error, expected a single string
    return StyleRuleCharset::create();
}

PassRefPtrWillBeRawPtr<StyleRuleImport> CSSParserImpl::consumeImportRule(CSSParserTokenRange prelude)
{
    prelude.consumeWhitespace();
    AtomicString uri(consumeStringOrURI(prelude));
    if (uri.isNull())
        return nullptr; // Parse error, expected string or URI

    if (m_observerWrapper) {
        unsigned endOffset = m_observerWrapper->endOffset(prelude);
        m_observerWrapper->observer().startRuleHeader(StyleRule::Import, m_observerWrapper->startOffset(prelude));
        m_observerWrapper->observer().endRuleHeader(endOffset);
        m_observerWrapper->observer().startRuleBody(endOffset);
        m_observerWrapper->observer().endRuleBody(endOffset);
    }

    return StyleRuleImport::create(uri, MediaQueryParser::parseMediaQuerySet(prelude));
}

PassRefPtrWillBeRawPtr<StyleRuleNamespace> CSSParserImpl::consumeNamespaceRule(CSSParserTokenRange prelude)
{
    prelude.consumeWhitespace();
    AtomicString namespacePrefix;
    if (prelude.peek().type() == IdentToken)
        namespacePrefix = prelude.consumeIncludingWhitespace().value();

    AtomicString uri(consumeStringOrURI(prelude));
    prelude.consumeWhitespace();
    if (uri.isNull() || !prelude.atEnd())
        return nullptr; // Parse error, expected string or URI

    if (namespacePrefix.isNull())
        m_defaultNamespace = uri;
    return StyleRuleNamespace::create(namespacePrefix, uri);
}

PassRefPtrWillBeRawPtr<StyleRuleMedia> CSSParserImpl::consumeMediaRule(CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase>> rules;

    if (m_observerWrapper) {
        CSSParserTokenRange preludeWithoutWhitespace = prelude;
        preludeWithoutWhitespace.consumeWhitespace();
        m_observerWrapper->observer().startRuleHeader(StyleRule::Media, m_observerWrapper->startOffset(preludeWithoutWhitespace));
        m_observerWrapper->observer().endRuleHeader(m_observerWrapper->endOffset(preludeWithoutWhitespace));
        m_observerWrapper->observer().startRuleBody(m_observerWrapper->previousTokenStartOffset(block));
    }

    consumeRuleList(block, RegularRuleList, [&rules](PassRefPtrWillBeRawPtr<StyleRuleBase> rule) {
        rules.append(rule);
    });

    if (m_observerWrapper)
        m_observerWrapper->observer().endRuleBody(m_observerWrapper->endOffset(block));

    return StyleRuleMedia::create(MediaQueryParser::parseMediaQuerySet(prelude), rules);
}

PassRefPtrWillBeRawPtr<StyleRuleSupports> CSSParserImpl::consumeSupportsRule(CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    CSSSupportsParser::SupportsResult supported = CSSSupportsParser::supportsCondition(prelude, *this);
    if (supported == CSSSupportsParser::Invalid)
        return nullptr; // Parse error, invalid @supports condition

    if (m_observerWrapper) {
        m_observerWrapper->observer().startRuleHeader(StyleRule::Supports, m_observerWrapper->startOffset(prelude));
        m_observerWrapper->observer().endRuleHeader(m_observerWrapper->endOffset(prelude));
        m_observerWrapper->observer().startRuleBody(m_observerWrapper->previousTokenStartOffset(block));
    }

    WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase>> rules;
    consumeRuleList(block, RegularRuleList, [&rules](PassRefPtrWillBeRawPtr<StyleRuleBase> rule) {
        rules.append(rule);
    });

    if (m_observerWrapper)
        m_observerWrapper->observer().endRuleBody(m_observerWrapper->endOffset(block));

    return StyleRuleSupports::create(prelude.serialize().stripWhiteSpace(), supported, rules);
}

PassRefPtrWillBeRawPtr<StyleRuleViewport> CSSParserImpl::consumeViewportRule(CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    // Allow @viewport rules from UA stylesheets even if the feature is disabled.
    if (!RuntimeEnabledFeatures::cssViewportEnabled() && !isUASheetBehavior(m_context.mode()))
        return nullptr;

    prelude.consumeWhitespace();
    if (!prelude.atEnd())
        return nullptr; // Parser error; @viewport prelude should be empty

    if (m_observerWrapper) {
        unsigned endOffset = m_observerWrapper->endOffset(prelude);
        m_observerWrapper->observer().startRuleHeader(StyleRule::Viewport, m_observerWrapper->startOffset(prelude));
        m_observerWrapper->observer().endRuleHeader(endOffset);
        m_observerWrapper->observer().startRuleBody(endOffset);
        m_observerWrapper->observer().endRuleBody(endOffset);
    }

    consumeDeclarationList(block, StyleRule::Viewport);
    return StyleRuleViewport::create(createStylePropertySet(m_parsedProperties, CSSViewportRuleMode));
}

PassRefPtrWillBeRawPtr<StyleRuleFontFace> CSSParserImpl::consumeFontFaceRule(CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    prelude.consumeWhitespace();
    if (!prelude.atEnd())
        return nullptr; // Parse error; @font-face prelude should be empty

    if (m_observerWrapper) {
        unsigned endOffset = m_observerWrapper->endOffset(prelude);
        m_observerWrapper->observer().startRuleHeader(StyleRule::FontFace, m_observerWrapper->startOffset(prelude));
        m_observerWrapper->observer().endRuleHeader(endOffset);
        m_observerWrapper->observer().startRuleBody(endOffset);
        m_observerWrapper->observer().endRuleBody(endOffset);
    }

    if (m_styleSheet)
        m_styleSheet->setHasFontFaceRule(true);

    consumeDeclarationList(block, StyleRule::FontFace);
    return StyleRuleFontFace::create(createStylePropertySet(m_parsedProperties, m_context.mode()));
}

PassRefPtrWillBeRawPtr<StyleRuleKeyframes> CSSParserImpl::consumeKeyframesRule(bool webkitPrefixed, CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    prelude.consumeWhitespace();
    const CSSParserToken& nameToken = prelude.consumeIncludingWhitespace();
    if (!prelude.atEnd())
        return nullptr; // Parse error; expected single non-whitespace token in @keyframes header

    String name;
    if (nameToken.type() == IdentToken) {
        name = nameToken.value();
    } else if (nameToken.type() == StringToken && webkitPrefixed) {
        if (m_context.useCounter())
            m_context.useCounter()->count(UseCounter::QuotedKeyframesRule);
        name = nameToken.value();
    } else {
        return nullptr; // Parse error; expected ident token in @keyframes header
    }

    if (m_observerWrapper) {
        unsigned endOffset = m_observerWrapper->endOffset(prelude);
        m_observerWrapper->observer().startRuleHeader(StyleRule::Keyframes, m_observerWrapper->startOffset(prelude));
        m_observerWrapper->observer().endRuleHeader(m_observerWrapper->endOffset(prelude));
        m_observerWrapper->observer().startRuleBody(endOffset);
        m_observerWrapper->observer().endRuleBody(endOffset);
    }

    RefPtrWillBeRawPtr<StyleRuleKeyframes> keyframeRule = StyleRuleKeyframes::create();
    consumeRuleList(block, KeyframesRuleList, [keyframeRule](PassRefPtrWillBeRawPtr<StyleRuleBase> keyframe) {
        keyframeRule->parserAppendKeyframe(toStyleRuleKeyframe(keyframe.get()));
    });
    keyframeRule->setName(name);
    keyframeRule->setVendorPrefixed(webkitPrefixed);
    return keyframeRule.release();
}

PassRefPtrWillBeRawPtr<StyleRulePage> CSSParserImpl::consumePageRule(CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    // We only support a small subset of the css-page spec.
    prelude.consumeWhitespace();
    AtomicString typeSelector;
    if (prelude.peek().type() == IdentToken)
        typeSelector = prelude.consume().value();

    AtomicString pseudo;
    if (prelude.peek().type() == ColonToken) {
        prelude.consume();
        if (prelude.peek().type() != IdentToken)
            return nullptr; // Parse error; expected ident token following colon in @page header
        pseudo = prelude.consume().value();
    }

    prelude.consumeWhitespace();
    if (!prelude.atEnd())
        return nullptr; // Parse error; extra tokens in @page header

    OwnPtr<CSSParserSelector> selector;
    if (!typeSelector.isNull() && pseudo.isNull()) {
        selector = CSSParserSelector::create(QualifiedName(nullAtom, typeSelector, m_defaultNamespace));
    } else {
        selector = CSSParserSelector::create();
        if (!pseudo.isNull()) {
            selector->setMatch(CSSSelector::PagePseudoClass);
            selector->updatePseudoType(pseudo.lower());
            if (selector->pseudoType() == CSSSelector::PseudoUnknown)
                return nullptr; // Parse error; unknown page pseudo-class
        }
        if (!typeSelector.isNull())
            selector->prependTagSelector(QualifiedName(nullAtom, typeSelector, m_defaultNamespace));
    }

    if (m_observerWrapper) {
        unsigned endOffset = m_observerWrapper->endOffset(prelude);
        m_observerWrapper->observer().startRuleHeader(StyleRule::Page, m_observerWrapper->startOffset(prelude));
        m_observerWrapper->observer().endRuleHeader(endOffset);
    }

    selector->setForPage();
    Vector<OwnPtr<CSSParserSelector>> selectorVector;
    selectorVector.append(selector.release());
    CSSSelectorList selectorList;
    selectorList.adoptSelectorVector(selectorVector);

    consumeDeclarationList(block, StyleRule::Style);

    return StyleRulePage::create(selectorList, createStylePropertySet(m_parsedProperties, m_context.mode()));
}

PassRefPtrWillBeRawPtr<StyleRuleKeyframe> CSSParserImpl::consumeKeyframeStyleRule(CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    OwnPtr<Vector<double>> keyList = consumeKeyframeKeyList(prelude);
    if (!keyList)
        return nullptr;
    consumeDeclarationList(block, StyleRule::Keyframes);
    return StyleRuleKeyframe::create(keyList.release(), createStylePropertySet(m_parsedProperties, m_context.mode()));
}

static void observeSelectors(CSSParserObserverWrapper& wrapper, CSSParserTokenRange selectors)
{
    // This is easier than hooking into the CSSSelectorParser
    selectors.consumeWhitespace();
    CSSParserTokenRange originalRange = selectors;
    wrapper.observer().startRuleHeader(StyleRule::Style, wrapper.startOffset(originalRange));

    while (!selectors.atEnd()) {
        const CSSParserToken* selectorStart = &selectors.peek();
        while (!selectors.atEnd() && selectors.peek().type() != CommaToken)
            selectors.consumeComponentValue();
        CSSParserTokenRange selector = selectors.makeSubRange(selectorStart, &selectors.peek());
        selectors.consumeIncludingWhitespace();

        wrapper.observer().observeSelector(wrapper.startOffset(selector), wrapper.endOffset(selector));
    }

    wrapper.observer().endRuleHeader(wrapper.endOffset(originalRange));
}


PassRefPtrWillBeRawPtr<StyleRule> CSSParserImpl::consumeStyleRule(CSSParserTokenRange prelude, CSSParserTokenRange block)
{
    CSSSelectorList selectorList;
    CSSSelectorParser::parseSelector(prelude, m_context, m_defaultNamespace, m_styleSheet, selectorList);
    if (!selectorList.isValid())
        return nullptr; // Parse error, invalid selector list

    if (m_observerWrapper)
        observeSelectors(*m_observerWrapper, prelude);

    consumeDeclarationList(block, StyleRule::Style);

    return StyleRule::create(selectorList, createStylePropertySet(m_parsedProperties, m_context.mode()));
}

void CSSParserImpl::consumeDeclarationList(CSSParserTokenRange range, StyleRule::Type ruleType)
{
    ASSERT(m_parsedProperties.isEmpty());

    bool useObserver = m_observerWrapper && ruleType == StyleRule::Style;
    if (useObserver) {
        m_observerWrapper->observer().startRuleBody(m_observerWrapper->previousTokenStartOffset(range));
        m_observerWrapper->skipCommentsBefore(range, true);
    }

    while (!range.atEnd()) {
        switch (range.peek().type()) {
        case WhitespaceToken:
        case SemicolonToken:
            range.consume();
            break;
        case IdentToken: {
            const CSSParserToken* declarationStart = &range.peek();

            if (useObserver)
                m_observerWrapper->yieldCommentsBefore(range);

            while (!range.atEnd() && range.peek().type() != SemicolonToken)
                range.consumeComponentValue();

            consumeDeclaration(range.makeSubRange(declarationStart, &range.peek()), ruleType);

            if (useObserver)
                m_observerWrapper->skipCommentsBefore(range, false);
            break;
        }
        case AtKeywordToken: {
            RefPtrWillBeRawPtr<StyleRuleBase> rule = consumeAtRule(range, NoRules);
            ASSERT_UNUSED(rule, !rule);
            break;
        }
        default: // Parse error, unexpected token in declaration list
            while (!range.atEnd() && range.peek().type() != SemicolonToken)
                range.consumeComponentValue();
            break;
        }
    }

    // Yield remaining comments
    if (useObserver) {
        m_observerWrapper->yieldCommentsBefore(range);
        m_observerWrapper->observer().endRuleBody(m_observerWrapper->endOffset(range));
    }
}

void CSSParserImpl::consumeDeclaration(CSSParserTokenRange range, StyleRule::Type ruleType)
{
    CSSParserTokenRange rangeCopy = range; // For inspector callbacks

    ASSERT(range.peek().type() == IdentToken);
    //CSSPropertyID unresolvedProperty = range.consumeIncludingWhitespace().parseAsUnresolvedCSSPropertyID();
	const CSSParserToken& token = range.consumeIncludingWhitespace();
	CSSPropertyID unresolvedProperty = token.parseAsUnresolvedCSSPropertyID();

    if (range.consume().type() != ColonToken)
        return; // Parse error

    bool important = false;
    const CSSParserToken* declarationValueEnd = range.end();
    const CSSParserToken* last = range.end() - 1;
    while (last->type() == WhitespaceToken)
        --last;
    if (last->type() == IdentToken && last->valueEqualsIgnoringCase("important")) {
        --last;
        while (last->type() == WhitespaceToken)
            --last;
        if (last->type() == DelimiterToken && last->delimiter() == '!') {
            important = true;
            declarationValueEnd = last;
        }
    }

	if (RuntimeEnabledFeatures::cssVariablesEnabled() && unresolvedProperty == CSSPropertyInvalid && CSSVariableParser::isValidVariableName(token)) {
		AtomicString variableName = token.value();
		consumeVariableDeclarationValue(range.makeSubRange(&range.peek(), declarationValueEnd), variableName, important);
		return;
		
	}

    if (important && (ruleType == StyleRule::FontFace || ruleType == StyleRule::Keyframes))
        return;

    if (m_observerWrapper && ruleType == StyleRule::Style) {
        size_t propertiesCount = m_parsedProperties.size();
        if (unresolvedProperty != CSSPropertyInvalid)
            consumeDeclarationValue(range.makeSubRange(&range.peek(), declarationValueEnd), unresolvedProperty, important, ruleType);
        m_observerWrapper->observer().observeProperty(
            m_observerWrapper->startOffset(rangeCopy), m_observerWrapper->endOffset(rangeCopy),
            important, m_parsedProperties.size() != propertiesCount);
        return;
    }

    if (unresolvedProperty == CSSPropertyInvalid)
        return;

    consumeDeclarationValue(range.makeSubRange(&range.peek(), declarationValueEnd), unresolvedProperty, important, ruleType);
}

void CSSParserImpl::consumeVariableDeclarationValue(CSSParserTokenRange range, const AtomicString& variableName, bool important)
{
	if (RefPtrWillBeRawPtr<CSSCustomPropertyDeclaration> value = CSSVariableParser::parseDeclarationValue(variableName, range))
		m_parsedProperties.append(CSSProperty(CSSPropertyVariable, value.release(), important));
}


void CSSParserImpl::consumeDeclarationValue(CSSParserTokenRange range, CSSPropertyID unresolvedProperty, bool important, StyleRule::Type ruleType)
{
    bool usesRemUnits;
    CSSParserValueList valueList(range, usesRemUnits);
    if (!valueList.size())
        return; // Parser error
    if (usesRemUnits && m_styleSheet)
        m_styleSheet->parserSetUsesRemUnits(true);
    CSSPropertyParser::parseValue(unresolvedProperty, important, &valueList, m_context, m_parsedProperties, ruleType);
}

PassOwnPtr<Vector<double>> CSSParserImpl::consumeKeyframeKeyList(CSSParserTokenRange range)
{
    OwnPtr<Vector<double>> result = adoptPtr(new Vector<double>);
    while (true) {
        range.consumeWhitespace();
        const CSSParserToken& token = range.consumeIncludingWhitespace();
        if (token.type() == PercentageToken && token.numericValue() >= 0 && token.numericValue() <= 100)
            result->append(token.numericValue() / 100);
        else if (token.type() == IdentToken && token.valueEqualsIgnoringCase("from"))
            result->append(0);
        else if (token.type() == IdentToken && token.valueEqualsIgnoringCase("to"))
            result->append(1);
        else
            return nullptr; // Parser error, invalid value in keyframe selector
        if (range.atEnd())
            return result.release();
        if (range.consume().type() != CommaToken)
            return nullptr; // Parser error
    }
}

} // namespace blink
