// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSParserToken_h
#define CSSParserToken_h

#include "core/CoreExport.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/parser/CSSParserString.h"

namespace blink {

enum CSSParserTokenType {
    IdentToken = 0,
    FunctionToken,
    AtKeywordToken,
    HashToken,
    UrlToken,
    BadUrlToken,
    DelimiterToken,
    NumberToken,
    PercentageToken,
    DimensionToken,
    IncludeMatchToken,
    DashMatchToken,
    PrefixMatchToken,
    SuffixMatchToken,
    SubstringMatchToken,
    ColumnToken,
    UnicodeRangeToken,
    WhitespaceToken,
    CDOToken,
    CDCToken,
    ColonToken,
    SemicolonToken,
    CommaToken,
    LeftParenthesisToken,
    RightParenthesisToken,
    LeftBracketToken,
    RightBracketToken,
    LeftBraceToken,
    RightBraceToken,
    StringToken,
    BadStringToken,
    EOFToken,
    CommentToken,
};

enum NumericSign {
    NoSign,
    PlusSign,
    MinusSign,
};

enum NumericValueType {
    IntegerValueType,
    NumberValueType,
};

enum HashTokenType {
    HashTokenId,
    HashTokenUnrestricted,
};

class CORE_EXPORT CSSParserToken {
public:
    enum BlockType {
        NotBlock,
        BlockStart,
        BlockEnd,
    };

    CSSParserToken(CSSParserTokenType, BlockType = NotBlock);
    CSSParserToken(CSSParserTokenType, CSSParserString, BlockType = NotBlock);

    CSSParserToken(CSSParserTokenType, UChar); // for DelimiterToken
    CSSParserToken(CSSParserTokenType, double, NumericValueType, NumericSign); // for NumberToken
    CSSParserToken(CSSParserTokenType, UChar32, UChar32); // for UnicodeRangeToken

    CSSParserToken(HashTokenType, CSSParserString);

    // Converts NumberToken to DimensionToken.
    void convertToDimensionWithUnit(CSSParserString);

    // Converts NumberToken to PercentageToken.
    void convertToPercentage();

    CSSParserTokenType type() const { return static_cast<CSSParserTokenType>(m_type); }
    CSSParserString value() const
    {
        CSSParserString ret;
        ret.initRaw(m_valueDataCharRaw, m_valueLength, m_valueIs8Bit);
        return ret;
    }
    bool valueEqualsIgnoringCase(const char* str) const { return value().equalIgnoringCase(str); }

    UChar delimiter() const;
    NumericSign numericSign() const;
    NumericValueType numericValueType() const;
    double numericValue() const;
    HashTokenType hashTokenType() const { ASSERT(m_type == HashToken); return m_hashTokenType; }
    BlockType blockType() const { return static_cast<BlockType>(m_blockType); }
    CSSPrimitiveValue::UnitType unitType() const { return static_cast<CSSPrimitiveValue::UnitType>(m_unit); }
    UChar32 unicodeRangeStart() const { ASSERT(m_type == UnicodeRangeToken); return m_unicodeRange.start; }
    UChar32 unicodeRangeEnd() const { ASSERT(m_type == UnicodeRangeToken); return m_unicodeRange.end; }

    CSSPropertyID parseAsUnresolvedCSSPropertyID() const;

    void serialize(StringBuilder&) const;

	CSSValueID id() const;
	CSSValueID functionId() const;

	bool hasStringBacking() const;
	CSSParserToken copyWithUpdatedString(const CSSParserString&) const;
	
private:
    void initValueFromCSSParserString(const CSSParserString& value)
    {
        m_valueLength = value.m_length;
        m_valueIs8Bit = value.m_is8Bit;
        m_valueDataCharRaw = value.m_data.charactersRaw;
    }
    unsigned m_type : 6; // CSSParserTokenType
    unsigned m_blockType : 2; // BlockType
    unsigned m_numericValueType : 1; // NumericValueType
    unsigned m_numericSign : 2; // NumericSign
    unsigned m_unit : 7; // CSSPrimitiveValue::UnitType

    // m_value... is an unpacked CSSParserString so that we can pack it
    // tightly with the rest of this object for a smaller object size.
    bool m_valueIs8Bit : 1;
    unsigned m_valueLength;
    const void* m_valueDataCharRaw; // Either LChar* or UChar*.

    union {
        UChar m_delimiter;
        HashTokenType m_hashTokenType;
        double m_numericValue;
		mutable int m_id;

        struct {
            UChar32 start;
            UChar32 end;
        } m_unicodeRange;
    };
};

} // namespace blink

namespace WTF {
template <>
struct IsTriviallyMoveAssignable<blink::CSSParserToken> {
    static const bool value = true;
};
}

#endif // CSSSParserToken_h
