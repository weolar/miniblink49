// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSVariableResolver_h
#define CSSVariableResolver_h

#include "core/CSSPropertyNames.h"
#include "core/css/parser/CSSParserToken.h"
#include "wtf/HashSet.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/AtomicStringHash.h"

namespace blink {

class CSSVariableReferenceValue;
class StyleResolverState;
class StyleVariableData;

class CSSVariableResolver {
public:
    static void resolveVariableDefinitions(StyleVariableData*);
    static void resolveAndApplyVariableReferences(StyleResolverState&, CSSPropertyID, const CSSVariableReferenceValue&);

private:
    CSSVariableResolver(StyleVariableData*);
    CSSVariableResolver(StyleVariableData*, AtomicString& variable);

    unsigned resolveVariableTokensRecursive(Vector<CSSParserToken>&, unsigned startOffset);
    void resolveVariableReferencesFromTokens(Vector<CSSParserToken>& tokens);

    StyleVariableData* m_styleVariableData;
    HashSet<AtomicString> m_variablesSeen;
    bool m_cycleDetected;
};

} // namespace blink

#endif // CSSVariableResolver