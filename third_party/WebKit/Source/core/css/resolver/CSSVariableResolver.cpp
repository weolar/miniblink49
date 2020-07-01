// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/resolver/CSSVariableResolver.h"

#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/StyleBuilderFunctions.h"
#include "core/css/CSSVariableData.h"
#include "core/css/CSSVariableReferenceValue.h"
#include "core/css/parser/CSSParserToken.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "core/css/parser/CSSParserValues.h"
#include "core/css/parser/CSSPropertyParser.h"
#include "core/css/resolver/StyleBuilder.h"
#include "core/css/resolver/StyleResolverState.h"
#include "core/style/StyleVariableData.h"
#include "wtf/Vector.h"

namespace blink {

// TODO(leviw): This should take a CSSParserTokenRange
static void findEndIndexOfVariableReference(const Vector<CSSParserToken>& resolvedTokens, unsigned startIndex, unsigned& endIndex, unsigned& commaIndex, bool& hasClosingBracket)
{
    endIndex = 0;
    commaIndex = 0;
    unsigned bracketCount = 1;
    for (unsigned i = startIndex; i < resolvedTokens.size(); ++i) {
        CSSParserTokenType type = resolvedTokens[i].type();

        if (type == CommaToken && !commaIndex) {
            commaIndex = i;
        } else if (type == LeftParenthesisToken || type == FunctionToken) {
            bracketCount++;
        } else if (type == RightParenthesisToken) {
            if (bracketCount == 1) {
                hasClosingBracket = true;
                endIndex = i;
                return;
            }
            bracketCount--;
        }
    }
    hasClosingBracket = false;
    endIndex = resolvedTokens.size() - 1;
}

unsigned CSSVariableResolver::resolveVariableTokensRecursive(Vector<CSSParserToken>& resolvedTokens, unsigned startIndex)
{
    unsigned variableLocation = startIndex;
    while (resolvedTokens[variableLocation].type() != IdentToken)
        variableLocation++;

    unsigned commaIndex;
    unsigned endIndex;
    bool hasClosingBracket;
    // Find default value and match braces
    findEndIndexOfVariableReference(resolvedTokens, variableLocation + 1, endIndex, commaIndex, hasClosingBracket);

    unsigned length = endIndex - startIndex + 1;
    unsigned varFunctionPosition = startIndex;

    AtomicString variableName = resolvedTokens[variableLocation].value();

    if (m_variablesSeen.contains(variableName)) {
        m_cycleDetected = true;
        resolvedTokens.clear();
        return 0;
    }

    CSSVariableData* variableData = m_styleVariableData ? m_styleVariableData->getVariable(variableName) : nullptr;
    if (variableData) {
        Vector<CSSParserToken> tokens(variableData->tokens());
        if (variableData->needsVariableResolution()) {
            m_variablesSeen.add(variableName);
            resolveVariableReferencesFromTokens(tokens);
            m_variablesSeen.remove(variableName);

            // The old variable data holds onto the backing string the new resolved CSSVariableData
            // relies on. Ensure it will live beyond us overwriting the RefPtr in StyleVariableData.
            ASSERT(variableData->refCount() > 1);

            m_styleVariableData->setVariable(variableName, CSSVariableData::createResolved(tokens));
        }
        if (tokens.size()) {
            resolvedTokens.remove(startIndex, length);
            resolvedTokens.insert(startIndex, tokens);
            return tokens.size();
        }
    }

    // Fallback on default value if present
    if (!commaIndex || m_cycleDetected) {
        resolvedTokens.clear();
        return 0;
    }

    // Move the tokens to the beginning of the variable reference
    unsigned defaultValueStart = commaIndex + 1;
    unsigned defaultValueLength = endIndex - commaIndex - (hasClosingBracket ? 1 : 0);
    for (unsigned i = 0; i < defaultValueLength; ++i)
        resolvedTokens[varFunctionPosition + i] = resolvedTokens[defaultValueStart + i];
    resolvedTokens.remove(varFunctionPosition + defaultValueLength, length - defaultValueLength);

    resolveVariableReferencesFromTokens(resolvedTokens);

    return resolvedTokens.size();
}

void CSSVariableResolver::resolveVariableReferencesFromTokens(Vector<CSSParserToken>& tokens)
{
    for (unsigned i = 0; i < tokens.size(); ++i) {
        if (tokens[i].functionId() == CSSValueVar) {
            unsigned validTokens = resolveVariableTokensRecursive(tokens, i);
            if (validTokens < 1 || m_cycleDetected) {
                tokens.clear();
                break;
            }
            i += validTokens - 1;
        }
    }
}

void CSSVariableResolver::resolveAndApplyVariableReferences(StyleResolverState& state, CSSPropertyID id, const CSSVariableReferenceValue& value)
{
    // TODO(leviw): This should be a stack
    Vector<CSSParserToken> tokens = value.variableDataValue()->tokens();

    CSSVariableResolver resolver(state.style()->variables());

    resolver.resolveVariableReferencesFromTokens(tokens);

    if (!tokens.size())
        return;

    CSSParserContext context(HTMLStandardMode, 0);

    WillBeHeapVector<CSSProperty, 256> parsedProperties;

	bool usesRemUnits = false;
	CSSParserTokenRange range(tokens);
	CSSParserValueList valueList(range, usesRemUnits);
    CSSPropertyParser::parseValue(id, false, &valueList, context, parsedProperties, StyleRule::Type::Style);

    unsigned parsedPropertiesCount = parsedProperties.size();
    for (unsigned i = 0; i < parsedPropertiesCount; ++i)
        StyleBuilder::applyProperty(parsedProperties[i].id(), state, parsedProperties[i].value());
}

void CSSVariableResolver::resolveVariableDefinitions(StyleVariableData* variables)
{
    if (!variables)
        return;

    for (auto& variable : variables->m_data) {
        if (!variable.value->needsVariableResolution())
            continue;
        Vector<CSSParserToken> resolvedTokens(variable.value->tokens());

        CSSVariableResolver resolver(variables, variable.key);
        resolver.resolveVariableReferencesFromTokens(resolvedTokens);

        variable.value = CSSVariableData::createResolved(resolvedTokens);
    }
}

CSSVariableResolver::CSSVariableResolver(StyleVariableData* styleVariableData)
    : m_styleVariableData(styleVariableData)
    , m_cycleDetected(false)
{
}

CSSVariableResolver::CSSVariableResolver(StyleVariableData* styleVariableData, AtomicString& variable)
    : m_styleVariableData(styleVariableData)
    , m_cycleDetected(false)
{
    m_variablesSeen.add(variable);
}

} // namespace blink