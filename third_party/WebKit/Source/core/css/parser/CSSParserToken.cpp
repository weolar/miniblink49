// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSParserToken.h"

#include "core/css/CSSMarkup.h"
#include "core/css/parser/CSSPropertyParser.h"
#include "wtf/HashMap.h"
#include "wtf/text/StringBuilder.h"
#include <limits.h>

namespace blink {

CSSParserToken::CSSParserToken(CSSParserTokenType type, BlockType blockType)
    : m_type(type)
    , m_blockType(blockType)
{
}

// Just a helper used for Delimiter tokens.
CSSParserToken::CSSParserToken(CSSParserTokenType type, UChar c)
    : m_type(type)
    , m_blockType(NotBlock)
    , m_delimiter(c)
{
    ASSERT(m_type == DelimiterToken);
}

CSSParserToken::CSSParserToken(CSSParserTokenType type, CSSParserString value, BlockType blockType)
    : m_type(type)
    , m_blockType(blockType)
{
    initValueFromCSSParserString(value);
	m_id = -1;
}

CSSParserToken::CSSParserToken(CSSParserTokenType type, double numericValue, NumericValueType numericValueType, NumericSign sign)
    : m_type(type)
    , m_blockType(NotBlock)
    , m_numericValueType(numericValueType)
    , m_numericSign(sign)
    , m_unit(CSSPrimitiveValue::CSS_NUMBER)
    , m_numericValue(numericValue)
{
    ASSERT(type == NumberToken);
}

CSSParserToken::CSSParserToken(CSSParserTokenType type, UChar32 start, UChar32 end)
    : m_type(UnicodeRangeToken)
    , m_blockType(NotBlock)
{
    ASSERT_UNUSED(type, type == UnicodeRangeToken);
    m_unicodeRange.start = start;
    m_unicodeRange.end = end;
}

CSSParserToken::CSSParserToken(HashTokenType type, CSSParserString value)
    : m_type(HashToken)
    , m_blockType(NotBlock)
    , m_hashTokenType(type)
{
    initValueFromCSSParserString(value);
}

void CSSParserToken::convertToDimensionWithUnit(CSSParserString unit)
{
    ASSERT(m_type == NumberToken);
    m_type = DimensionToken;
    initValueFromCSSParserString(unit);
    m_unit = CSSPrimitiveValue::fromName(unit);
}

void CSSParserToken::convertToPercentage()
{
    ASSERT(m_type == NumberToken);
    m_type = PercentageToken;
    m_unit = CSSPrimitiveValue::CSS_PERCENTAGE;
}

UChar CSSParserToken::delimiter() const
{
    ASSERT(m_type == DelimiterToken);
    return m_delimiter;
}

NumericSign CSSParserToken::numericSign() const
{
    // This is valid for DimensionToken and PercentageToken, but only used
    // in <an+b> parsing on NumberTokens.
    ASSERT(m_type == NumberToken);
    return static_cast<NumericSign>(m_numericSign);
}

NumericValueType CSSParserToken::numericValueType() const
{
    ASSERT(m_type == NumberToken || m_type == PercentageToken || m_type == DimensionToken);
    return static_cast<NumericValueType>(m_numericValueType);
}

double CSSParserToken::numericValue() const
{
    ASSERT(m_type == NumberToken || m_type == PercentageToken || m_type == DimensionToken);
    return m_numericValue;
}

CSSPropertyID CSSParserToken::parseAsUnresolvedCSSPropertyID() const
{
    ASSERT(m_type == IdentToken);
    return unresolvedCSSPropertyID(value());
}

CSSValueID CSSParserToken::id() const
{
	if (m_type != IdentToken)
		return CSSValueInvalid;
	if (m_id < 0)
		m_id = cssValueKeywordID(value());
	return static_cast<CSSValueID>(m_id);
}

CSSValueID CSSParserToken::functionId() const
{
	if (m_type != FunctionToken)
		return CSSValueInvalid;
	if (m_id < 0)
		m_id = cssValueKeywordID(value());
	return static_cast<CSSValueID>(m_id);
}

void CSSParserToken::serialize(StringBuilder& builder) const
{
    // This is currently only used for @supports CSSOM. To keep our implementation
    // simple we handle some of the edge cases incorrectly (see comments below).
    switch (type()) {
    case IdentToken:
        return serializeIdentifier(value(), builder);
    case FunctionToken:
        serializeIdentifier(value(), builder);
        return builder.append('(');
    case AtKeywordToken:
        builder.append('@');
        return serializeIdentifier(value(), builder);
    case HashToken:
        // This will always serialize as a hash-token with 'id' type instead of
        // preserving the type of the input.
        builder.append('#');
        return serializeIdentifier(value(), builder);
    case UrlToken:
        builder.append("url(");
        serializeIdentifier(value(), builder);
        return builder.append(")");
    case DelimiterToken:
        if (delimiter() == '\\')
            return builder.append("\\\n");
        return builder.append(delimiter());
    case NumberToken:
        // These won't properly preserve the NumericValueType flag
        return builder.appendNumber(numericValue());
    case PercentageToken:
        builder.appendNumber(numericValue());
        return builder.append('%');
    case DimensionToken:
        // This will incorrectly serialize e.g. 4e3e2 as 4000e2
        builder.appendNumber(numericValue());
        return serializeIdentifier(value(), builder);
    case UnicodeRangeToken:
        return builder.append(String::format("U+%X-%X", unicodeRangeStart(), unicodeRangeEnd()));
    case StringToken:
        return serializeString(value(), builder);

    case IncludeMatchToken:
        return builder.append("~=");
    case DashMatchToken:
        return builder.append("|=");
    case PrefixMatchToken:
        return builder.append("^=");
    case SuffixMatchToken:
        return builder.append("$=");
    case SubstringMatchToken:
        return builder.append("*=");
    case ColumnToken:
        return builder.append("||");
    case CDOToken:
        return builder.append("<!--");
    case CDCToken:
        return builder.append("-->");
    case BadStringToken:
        return builder.append("'\n");
    case BadUrlToken:
        return builder.append("url(()");
    case WhitespaceToken:
        return builder.append(' ');
    case ColonToken:
        return builder.append(':');
    case SemicolonToken:
        return builder.append(';');
    case CommaToken:
        return builder.append(',');
    case LeftParenthesisToken:
        return builder.append('(');
    case RightParenthesisToken:
        return builder.append(')');
    case LeftBracketToken:
        return builder.append('[');
    case RightBracketToken:
        return builder.append(']');
    case LeftBraceToken:
        return builder.append('{');
    case RightBraceToken:
        return builder.append('}');

    case EOFToken:
    case CommentToken:
        ASSERT_NOT_REACHED();
        return;
    }
}

bool CSSParserToken::hasStringBacking() const
{
	CSSParserTokenType tokenType = type();
	return tokenType == IdentToken
		|| tokenType == FunctionToken
		|| tokenType == AtKeywordToken
		|| tokenType == HashToken
		|| tokenType == UrlToken
		|| tokenType == DimensionToken
		|| tokenType == StringToken;
}

CSSParserToken CSSParserToken::copyWithUpdatedString(const CSSParserString& parserString) const
{
	CSSParserToken copy(*this);
	copy.initValueFromCSSParserString(parserString);
	return copy;
}


} // namespace blink
