// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSParserImpl_h
#define CSSParserImpl_h

#include "core/CSSPropertyNames.h"
#include "core/css/CSSProperty.h"
#include "core/css/CSSPropertySourceData.h"
#include "core/css/parser/CSSParserMode.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "platform/heap/Handle.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class CSSParserObserver;
class CSSParserObserverWrapper;
class StyleRule;
class StyleRuleBase;
class StyleRuleCharset;
class StyleRuleFontFace;
class StyleRuleImport;
class StyleRuleKeyframe;
class StyleRuleKeyframes;
class StyleRuleMedia;
class StyleRuleNamespace;
class StyleRulePage;
class StyleRuleSupports;
class StyleRuleViewport;
class StyleSheetContents;
class ImmutableStylePropertySet;
class Element;
class MutableStylePropertySet;

class CSSParserImpl {
    STACK_ALLOCATED();
public:
    CSSParserImpl(const CSSParserContext&, StyleSheetContents* = nullptr);

    enum AllowedRulesType {
        // As per css-syntax, css-cascade and css-namespaces, @charset rules
        // must come first, followed by @import then @namespace.
        // AllowImportRules actually means we allow @import and any rules thay
        // may follow it, i.e. @namespace rules and regular rules.
        // AllowCharsetRules and AllowNamespaceRules behave similarly.
        AllowCharsetRules,
        AllowImportRules,
        AllowNamespaceRules,
        RegularRules,
        KeyframeRules,
        NoRules, // For parsing at-rules inside declaration lists
    };

    static bool parseValue(MutableStylePropertySet*, CSSPropertyID, const String&, bool important, const CSSParserContext&);
    static PassRefPtrWillBeRawPtr<ImmutableStylePropertySet> parseInlineStyleDeclaration(const String&, Element*);
    static bool parseDeclarationList(MutableStylePropertySet*, const String&, const CSSParserContext&);
    static PassRefPtrWillBeRawPtr<StyleRuleBase> parseRule(const String&, const CSSParserContext&, StyleSheetContents*, AllowedRulesType);
    static void parseStyleSheet(const String&, const CSSParserContext&, StyleSheetContents*);

    static PassOwnPtr<Vector<double>> parseKeyframeKeyList(const String&);

    bool supportsDeclaration(CSSParserTokenRange&);

    static void parseDeclarationListForInspector(const String&, const CSSParserContext&, CSSParserObserver&);
    static void parseStyleSheetForInspector(const String&, const CSSParserContext&, StyleSheetContents*, CSSParserObserver&);

private:
    enum RuleListType {
        TopLevelRuleList,
        RegularRuleList,
        KeyframesRuleList
    };

    // Returns whether the first encountered rule was valid
    template<typename T>
    bool consumeRuleList(CSSParserTokenRange, RuleListType, T callback);

    // These two functions update the range they're given
    PassRefPtrWillBeRawPtr<StyleRuleBase> consumeAtRule(CSSParserTokenRange&, AllowedRulesType);
    PassRefPtrWillBeRawPtr<StyleRuleBase> consumeQualifiedRule(CSSParserTokenRange&, AllowedRulesType);

    static PassRefPtrWillBeRawPtr<StyleRuleCharset> consumeCharsetRule(CSSParserTokenRange prelude);
    PassRefPtrWillBeRawPtr<StyleRuleImport> consumeImportRule(CSSParserTokenRange prelude);
    PassRefPtrWillBeRawPtr<StyleRuleNamespace> consumeNamespaceRule(CSSParserTokenRange prelude); // This can set m_defaultNamespace
    PassRefPtrWillBeRawPtr<StyleRuleMedia> consumeMediaRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleSupports> consumeSupportsRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleViewport> consumeViewportRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleFontFace> consumeFontFaceRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRuleKeyframes> consumeKeyframesRule(bool webkitPrefixed, CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRulePage> consumePageRule(CSSParserTokenRange prelude, CSSParserTokenRange block);

    PassRefPtrWillBeRawPtr<StyleRuleKeyframe> consumeKeyframeStyleRule(CSSParserTokenRange prelude, CSSParserTokenRange block);
    PassRefPtrWillBeRawPtr<StyleRule> consumeStyleRule(CSSParserTokenRange prelude, CSSParserTokenRange block);

    void consumeDeclarationList(CSSParserTokenRange, StyleRule::Type);
    void consumeDeclaration(CSSParserTokenRange, StyleRule::Type);
    void consumeDeclarationValue(CSSParserTokenRange, CSSPropertyID, bool important, StyleRule::Type);
	void consumeVariableDeclarationValue(CSSParserTokenRange, const AtomicString& variableName, bool important);

    static PassOwnPtr<Vector<double>> consumeKeyframeKeyList(CSSParserTokenRange);

    // FIXME: Can we build StylePropertySets directly?
    // FIXME: Investigate using a smaller inline buffer
    WillBeHeapVector<CSSProperty, 256> m_parsedProperties;
    CSSParserContext m_context;

    AtomicString m_defaultNamespace;
    RawPtrWillBeMember<StyleSheetContents> m_styleSheet;

    // For the inspector
    CSSParserObserverWrapper* m_observerWrapper;
};

} // namespace blink

#endif // CSSParserImpl_h
