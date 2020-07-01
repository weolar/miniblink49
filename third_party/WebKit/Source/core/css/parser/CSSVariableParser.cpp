// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSVariableParser.h"

#include "core/css/CSSCustomPropertyDeclaration.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "core/css/parser/CSSParserValues.h"

namespace blink {

bool CSSVariableParser::isValidVariableName(const CSSParserToken& token)
{
    if (token.type() != IdentToken)
        return false;

    CSSParserString value = token.value();
    return value.length() >= 2 && value[0] == '-' && value[1] == '-';
}

bool isValidVariableReference(CSSParserTokenRange);

bool classifyBlock(CSSParserTokenRange range, bool& hasReferences, bool isTopLevelBlock = true)
{
    while (!range.atEnd()) {
        if (range.peek().blockType() == CSSParserToken::BlockStart) {
            const CSSParserToken& token = range.peek();
            CSSParserTokenRange block = range.consumeBlock();
            if (token.functionId() == CSSValueVar) {
                if (!isValidVariableReference(block))
                    return false; // Bail if any references are invalid
                hasReferences = true;
                continue;
            }
            if (!classifyBlock(block, hasReferences, false))
                return false;
            continue;
        }

        ASSERT(range.peek().blockType() != CSSParserToken::BlockEnd);

        const CSSParserToken& token = range.consume();
        switch (token.type()) {
        case DelimiterToken: {
            if (token.delimiter() == '!' && isTopLevelBlock)
                return false;
            break;
        }
        case RightParenthesisToken:
        case RightBraceToken:
        case RightBracketToken:
        case BadStringToken:
        case BadUrlToken:
            return false;
        case SemicolonToken:
            if (isTopLevelBlock)
                return false;
            break;
        default:
            break;
        }
    }
    return true;
}

bool isValidVariableReference(CSSParserTokenRange range)
{
    range.consumeWhitespace();
    if (!CSSVariableParser::isValidVariableName(range.consumeIncludingWhitespace()))
        return false;
    if (range.atEnd())
        return true;

    if (range.consume().type() != CommaToken)
        return false;
    if (range.atEnd())
        return false;

    bool hasReferences = false;
    return classifyBlock(range, hasReferences);
}

static CSSValueID classifyVariableRange(CSSParserTokenRange range, bool& hasReferences)
{
    hasReferences = false;

    range.consumeWhitespace();
    if (range.peek().type() == IdentToken) {
        CSSValueID id = range.consumeIncludingWhitespace().id();
        if (range.atEnd() && (id == CSSValueInherit || id == CSSValueInitial || id == CSSValueUnset))
            return id;
    }

    if (classifyBlock(range, hasReferences))
        return CSSValueInternalVariableValue;
    return CSSValueInvalid;
}

bool CSSVariableParser::containsValidVariableReferences(CSSParserTokenRange range)
{
    bool hasReferences;
    CSSValueID type = classifyVariableRange(range, hasReferences);
    return type == CSSValueInternalVariableValue && hasReferences;
}

PassRefPtrWillBeRawPtr<CSSCustomPropertyDeclaration> CSSVariableParser::parseDeclarationValue(const AtomicString& variableName, CSSParserTokenRange range)
{
    if (range.atEnd())
        return nullptr;

    bool hasReferences;
    CSSValueID type = classifyVariableRange(range, hasReferences);

    if (type == CSSValueInvalid)
        return nullptr;
    if (type == CSSValueInternalVariableValue)
        return CSSCustomPropertyDeclaration::create(variableName, CSSVariableData::create(range, hasReferences));
    return CSSCustomPropertyDeclaration::create(variableName, type);
}

} // namespace blink