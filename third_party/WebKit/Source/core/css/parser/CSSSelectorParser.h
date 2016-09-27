// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSSelectorParser_h
#define CSSSelectorParser_h

#include "core/CoreExport.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "core/css/parser/CSSParserValues.h"

namespace blink {

class CSSSelectorList;
class StyleSheetContents;

// FIXME: We should consider building CSSSelectors directly instead of using
// the intermediate CSSParserSelector.
class CORE_EXPORT CSSSelectorParser {
public:
    static void parseSelector(CSSParserTokenRange, const CSSParserContext&, const AtomicString& defaultNamespace, StyleSheetContents*, CSSSelectorList&);

    static bool consumeANPlusB(CSSParserTokenRange&, std::pair<int, int>&);

private:
    CSSSelectorParser(const CSSParserContext&, const AtomicString& defaultNamespace, StyleSheetContents*);

    // These will all consume trailing comments if successful

    void consumeComplexSelectorList(CSSParserTokenRange&, CSSSelectorList&);
    void consumeCompoundSelectorList(CSSParserTokenRange&, CSSSelectorList&);

    PassOwnPtr<CSSParserSelector> consumeComplexSelector(CSSParserTokenRange&);
    PassOwnPtr<CSSParserSelector> consumeCompoundSelector(CSSParserTokenRange&);
    // This doesn't include element names, since they're handled specially
    PassOwnPtr<CSSParserSelector> consumeSimpleSelector(CSSParserTokenRange&);

    bool consumeName(CSSParserTokenRange&, AtomicString& name, AtomicString& namespacePrefix, bool& hasNamespace);

    // These will return nullptr when the selector is invalid
    PassOwnPtr<CSSParserSelector> consumeId(CSSParserTokenRange&);
    PassOwnPtr<CSSParserSelector> consumeClass(CSSParserTokenRange&);
    PassOwnPtr<CSSParserSelector> consumePseudo(CSSParserTokenRange&);
    PassOwnPtr<CSSParserSelector> consumeAttribute(CSSParserTokenRange&);

    CSSSelector::Relation consumeCombinator(CSSParserTokenRange&);
    CSSSelector::Match consumeAttributeMatch(CSSParserTokenRange&);
    CSSSelector::AttributeMatchType consumeAttributeFlags(CSSParserTokenRange&);

    QualifiedName determineNameInNamespace(const AtomicString& prefix, const AtomicString& localName);
    void prependTypeSelectorIfNeeded(const AtomicString& namespacePrefix, const AtomicString& elementName, CSSParserSelector*);
    void rewriteSpecifiersWithElementNameForCustomPseudoElement(const QualifiedName& tag, CSSParserSelector*, bool tagIsImplicit);
    void rewriteSpecifiersWithElementNameForContentPseudoElement(const QualifiedName& tag, CSSParserSelector*, bool tagIsImplicit);
    static PassOwnPtr<CSSParserSelector> addSimpleSelectorToCompound(PassOwnPtr<CSSParserSelector> compoundSelector, PassOwnPtr<CSSParserSelector> simpleSelector);

    const CSSParserContext& m_context;
    AtomicString m_defaultNamespace;
    StyleSheetContents* m_styleSheet; // FIXME: Should be const

    bool m_failedParsing;
};

} // namespace

#endif // CSSSelectorParser_h
