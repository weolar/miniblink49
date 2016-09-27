// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSTokenizer.h"

#include "core/css/parser/CSSParserTokenRange.h"
#include "core/css/parser/MediaQueryBlockWatcher.h"
#include <gtest/gtest.h>

namespace blink {

// This let's us see the line numbers of failing tests
#define TEST_TOKENS(string, ...) { \
    String s = string; \
    SCOPED_TRACE(s.ascii().data()); \
    testTokens(string, __VA_ARGS__); \
}

void compareTokens(const CSSParserToken& expected, const CSSParserToken& actual)
{
    ASSERT_EQ(expected.type(), actual.type());
    switch (expected.type()) {
    case DelimiterToken:
        ASSERT_EQ(expected.delimiter(), actual.delimiter());
        break;
    case IdentToken:
    case FunctionToken:
    case StringToken:
    case UrlToken:
        ASSERT_EQ(String(expected.value()), String(actual.value()));
        break;
    case DimensionToken:
        ASSERT_EQ(String(expected.value()), String(actual.value()));
        ASSERT_EQ(expected.numericValueType(), actual.numericValueType());
        ASSERT_DOUBLE_EQ(expected.numericValue(), actual.numericValue());
        break;
    case NumberToken:
        ASSERT_EQ(expected.numericSign(), actual.numericSign());
        // fallthrough
    case PercentageToken:
        ASSERT_EQ(expected.numericValueType(), actual.numericValueType());
        ASSERT_DOUBLE_EQ(expected.numericValue(), actual.numericValue());
        break;
    case UnicodeRangeToken:
        ASSERT_EQ(expected.unicodeRangeStart(), actual.unicodeRangeStart());
        ASSERT_EQ(expected.unicodeRangeEnd(), actual.unicodeRangeEnd());
        break;
    case HashToken:
        ASSERT_EQ(String(expected.value()), String(actual.value()));
        ASSERT_EQ(expected.hashTokenType(), actual.hashTokenType());
        break;
    default:
        break;
    }
}

void testTokens(const String& string, const CSSParserToken& token1, const CSSParserToken& token2 = CSSParserToken(EOFToken), const CSSParserToken& token3 = CSSParserToken(EOFToken))
{
    Vector<CSSParserToken> expectedTokens;
    expectedTokens.append(token1);
    if (token2.type() != EOFToken) {
        expectedTokens.append(token2);
        if (token3.type() != EOFToken)
            expectedTokens.append(token3);
    }

    CSSParserTokenRange expected(expectedTokens);

    CSSTokenizer::Scope actualScope(string);
    CSSParserTokenRange actual = actualScope.tokenRange();

    // Just check that serialization doesn't hit any asserts
    actual.serialize();

    while (!expected.atEnd() || !actual.atEnd())
        compareTokens(expected.consume(), actual.consume());
}

static CSSParserString toParserString(const String& string)
{
    CSSParserString result;
    result.init(string);
    return result;
}

static CSSParserToken ident(const String& string) { return CSSParserToken(IdentToken, toParserString(string)); }
static CSSParserToken atKeyword(const String& string) { return CSSParserToken(AtKeywordToken, toParserString(string)); }
static CSSParserToken string(const String& string) { return CSSParserToken(StringToken, toParserString(string)); }
static CSSParserToken function(const String& string) { return CSSParserToken(FunctionToken, toParserString(string)); }
static CSSParserToken url(const String& string) { return CSSParserToken(UrlToken, toParserString(string)); }
static CSSParserToken hash(const String& string, HashTokenType type) { return CSSParserToken(type, toParserString(string)); }
static CSSParserToken delim(char c) { return CSSParserToken(DelimiterToken, c); }

static CSSParserToken unicodeRange(UChar32 start, UChar32 end)
{
    return CSSParserToken(UnicodeRangeToken, start, end);
}

static CSSParserToken number(NumericValueType type, double value, NumericSign sign)
{
    return CSSParserToken(NumberToken, value, type, sign);
}

static CSSParserToken dimension(NumericValueType type, double value, const String& string)
{
    CSSParserToken token = number(type, value, NoSign); // sign ignored
    token.convertToDimensionWithUnit(toParserString(string));
    return token;
}

static CSSParserToken percentage(NumericValueType type, double value)
{
    CSSParserToken token = number(type, value, NoSign); // sign ignored
    token.convertToPercentage();
    return token;
}

DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, whitespace, (WhitespaceToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, colon, (ColonToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, semicolon, (SemicolonToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, comma, (CommaToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, includeMatch, (IncludeMatchToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, dashMatch, (DashMatchToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, prefixMatch, (PrefixMatchToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, suffixMatch, (SuffixMatchToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, substringMatch, (SubstringMatchToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, column, (ColumnToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, cdo, (CDOToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, cdc, (CDCToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, leftParenthesis, (LeftParenthesisToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, rightParenthesis, (RightParenthesisToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, leftBracket, (LeftBracketToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, rightBracket, (RightBracketToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, leftBrace, (LeftBraceToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, rightBrace, (RightBraceToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, badString, (BadStringToken));
DEFINE_STATIC_LOCAL_NOASSERT(CSSParserToken, badUrl, (BadUrlToken));

String fromUChar32(UChar32 c)
{
    StringBuilder input;
    input.append(c);
    return input.toString();
}

TEST(CSSTokenizerTest, SingleCharacterTokens)
{
    TEST_TOKENS("(", leftParenthesis);
    TEST_TOKENS(")", rightParenthesis);
    TEST_TOKENS("[", leftBracket);
    TEST_TOKENS("]", rightBracket);
    TEST_TOKENS(",", comma);
    TEST_TOKENS(":", colon);
    TEST_TOKENS(";", semicolon);
    TEST_TOKENS(")[", rightParenthesis, leftBracket);
    TEST_TOKENS("[)", leftBracket, rightParenthesis);
    TEST_TOKENS("{}", leftBrace, rightBrace);
    TEST_TOKENS(",,", comma, comma);
}

TEST(CSSTokenizerTest, MultipleCharacterTokens)
{
    TEST_TOKENS("~=", includeMatch);
    TEST_TOKENS("|=", dashMatch);
    TEST_TOKENS("^=", prefixMatch);
    TEST_TOKENS("$=", suffixMatch);
    TEST_TOKENS("*=", substringMatch);
    TEST_TOKENS("||", column);
    TEST_TOKENS("|||", column, delim('|'));
    TEST_TOKENS("<!--", cdo);
    TEST_TOKENS("<!---", cdo, delim('-'));
    TEST_TOKENS("-->", cdc);
}

TEST(CSSTokenizerTest, DelimiterToken)
{
    TEST_TOKENS("^", delim('^'));
    TEST_TOKENS("*", delim('*'));
    TEST_TOKENS("%", delim('%'));
    TEST_TOKENS("~", delim('~'));
    TEST_TOKENS("&", delim('&'));
    TEST_TOKENS("|", delim('|'));
    TEST_TOKENS("\x7f", delim('\x7f'));
    TEST_TOKENS("\1", delim('\x1'));
    TEST_TOKENS("~-", delim('~'), delim('-'));
    TEST_TOKENS("^|", delim('^'), delim('|'));
    TEST_TOKENS("$~", delim('$'), delim('~'));
    TEST_TOKENS("*^", delim('*'), delim('^'));
}

TEST(CSSTokenizerTest, WhitespaceTokens)
{
    TEST_TOKENS("   ", whitespace);
    TEST_TOKENS("\n\rS", whitespace, ident("S"));
    TEST_TOKENS("   *", whitespace, delim('*'));
    TEST_TOKENS("\r\n\f\t2", whitespace, number(IntegerValueType, 2, NoSign));
}

TEST(CSSTokenizerTest, Escapes)
{
    TEST_TOKENS("hel\\6Co", ident("hello"));
    TEST_TOKENS("\\26 B", ident("&B"));
    TEST_TOKENS("'hel\\6c o'", string("hello"));
    TEST_TOKENS("'spac\\65\r\ns'", string("spaces"));
    TEST_TOKENS("spac\\65\r\ns", ident("spaces"));
    TEST_TOKENS("spac\\65\n\rs", ident("space"), whitespace, ident("s"));
    TEST_TOKENS("sp\\61\tc\\65\fs", ident("spaces"));
    TEST_TOKENS("hel\\6c  o", ident("hell"), whitespace, ident("o"));
    TEST_TOKENS("test\\\n", ident("test"), delim('\\'), whitespace);
    TEST_TOKENS("test\\D799", ident("test" + fromUChar32(0xD799)));
    TEST_TOKENS("\\E000", ident(fromUChar32(0xE000)));
    TEST_TOKENS("te\\s\\t", ident("test"));
    TEST_TOKENS("spaces\\ in\\\tident", ident("spaces in\tident"));
    TEST_TOKENS("\\.\\,\\:\\!", ident(".,:!"));
    TEST_TOKENS("\\\r", delim('\\'), whitespace);
    TEST_TOKENS("\\\f", delim('\\'), whitespace);
    TEST_TOKENS("\\\r\n", delim('\\'), whitespace);
    String replacement = fromUChar32(0xFFFD);
    TEST_TOKENS(String("null\\\0", 6), ident("null" + replacement));
    TEST_TOKENS(String("null\\\0\0", 7), ident("null" + replacement + replacement));
    TEST_TOKENS("null\\0", ident("null" + replacement));
    TEST_TOKENS("null\\0000", ident("null" + replacement));
    TEST_TOKENS("large\\110000", ident("large" + replacement));
    TEST_TOKENS("large\\23456a", ident("large" + replacement));
    TEST_TOKENS("surrogate\\D800", ident("surrogate" + replacement));
    TEST_TOKENS("surrogate\\0DABC", ident("surrogate" + replacement));
    TEST_TOKENS("\\00DFFFsurrogate", ident(replacement + "surrogate"));
    TEST_TOKENS("\\10fFfF", ident(fromUChar32(0x10ffff)));
    TEST_TOKENS("\\10fFfF0", ident(fromUChar32(0x10ffff) + "0"));
    TEST_TOKENS("\\10000000", ident(fromUChar32(0x100000) + "00"));
    TEST_TOKENS("eof\\", ident("eof" + replacement));
}

TEST(CSSTokenizerTest, IdentToken)
{
    TEST_TOKENS("simple-ident", ident("simple-ident"));
    TEST_TOKENS("testing123", ident("testing123"));
    TEST_TOKENS("hello!", ident("hello"), delim('!'));
    TEST_TOKENS("world\5", ident("world"), delim('\5'));
    TEST_TOKENS("_under score", ident("_under"), whitespace, ident("score"));
    TEST_TOKENS("-_underscore", ident("-_underscore"));
    TEST_TOKENS("-text", ident("-text"));
    TEST_TOKENS("-\\6d", ident("-m"));
    TEST_TOKENS("--abc", ident("--abc"));
    TEST_TOKENS("--", ident("--"));
    TEST_TOKENS("--11", ident("--11"));
    TEST_TOKENS("---", ident("---"));
    TEST_TOKENS(fromUChar32(0x2003), ident(fromUChar32(0x2003))); // em-space
    TEST_TOKENS(fromUChar32(0xA0), ident(fromUChar32(0xA0))); // non-breaking space
    TEST_TOKENS(fromUChar32(0x1234), ident(fromUChar32(0x1234)));
    TEST_TOKENS(fromUChar32(0x12345), ident(fromUChar32(0x12345)));
    TEST_TOKENS(String("\0", 1), ident(fromUChar32(0xFFFD)));
    TEST_TOKENS(String("ab\0c", 4), ident("ab" + fromUChar32(0xFFFD) + "c"));
    TEST_TOKENS(String("ab\0c", 4), ident("ab" + fromUChar32(0xFFFD) + "c"));
}

TEST(CSSTokenizerTest, FunctionToken)
{
    TEST_TOKENS("scale(2)", function("scale"), number(IntegerValueType, 2, NoSign), rightParenthesis);
    TEST_TOKENS("foo-bar\\ baz(", function("foo-bar baz"));
    TEST_TOKENS("fun\\(ction(", function("fun(ction"));
    TEST_TOKENS("-foo(", function("-foo"));
    TEST_TOKENS("url(\"foo.gif\"", function("url"), string("foo.gif"));
    TEST_TOKENS("foo(  \'bar.gif\'", function("foo"), whitespace, string("bar.gif"));
    // To simplify implementation we drop the whitespace in function(url),whitespace,string()
    TEST_TOKENS("url(  \'bar.gif\'", function("url"), string("bar.gif"));
}

TEST(CSSTokenizerTest, AtKeywordToken)
{
    TEST_TOKENS("@at-keyword", atKeyword("at-keyword"));
    TEST_TOKENS("@testing123", atKeyword("testing123"));
    TEST_TOKENS("@hello!", atKeyword("hello"), delim('!'));
    TEST_TOKENS("@-text", atKeyword("-text"));
    TEST_TOKENS("@--abc", atKeyword("--abc"));
    TEST_TOKENS("@--", atKeyword("--"));
    TEST_TOKENS("@--11", atKeyword("--11"));
    TEST_TOKENS("@---", atKeyword("---"));
    TEST_TOKENS("@\\ ", atKeyword(" "));
    TEST_TOKENS("@-\\ ", atKeyword("- "));
    TEST_TOKENS("@@", delim('@'), delim('@'));
    TEST_TOKENS("@2", delim('@'), number(IntegerValueType, 2, NoSign));
    TEST_TOKENS("@-1", delim('@'), number(IntegerValueType, -1, MinusSign));
}

TEST(CSSTokenizerTest, UrlToken)
{
    TEST_TOKENS("url(foo.gif)", url("foo.gif"));
    TEST_TOKENS("urL(https://example.com/cats.png)", url("https://example.com/cats.png"));
    TEST_TOKENS("uRl(what-a.crazy^URL~this\\ is!)", url("what-a.crazy^URL~this is!"));
    TEST_TOKENS("uRL(123#test)", url("123#test"));
    TEST_TOKENS("Url(escapes\\ \\\"\\'\\)\\()", url("escapes \"')("));
    TEST_TOKENS("UrL(   whitespace   )", url("whitespace"));
    TEST_TOKENS("URl( whitespace-eof ", url("whitespace-eof"));
    TEST_TOKENS("URL(eof", url("eof"));
    TEST_TOKENS("url(not/*a*/comment)", url("not/*a*/comment"));
    TEST_TOKENS("urL()", url(""));
    TEST_TOKENS("uRl(white space),", badUrl, comma);
    TEST_TOKENS("Url(b(ad),", badUrl, comma);
    TEST_TOKENS("uRl(ba'd):", badUrl, colon);
    TEST_TOKENS("urL(b\"ad):", badUrl, colon);
    TEST_TOKENS("uRl(b\"ad):", badUrl, colon);
    TEST_TOKENS("Url(b\\\rad):", badUrl, colon);
    TEST_TOKENS("url(b\\\nad):", badUrl, colon);
    TEST_TOKENS("url(/*'bad')*/", badUrl, delim('*'), delim('/'));
    TEST_TOKENS("url(ba'd\\\\))", badUrl, rightParenthesis);
}

TEST(CSSTokenizerTest, StringToken)
{
    TEST_TOKENS("'text'", string("text"));
    TEST_TOKENS("\"text\"", string("text"));
    TEST_TOKENS("'testing, 123!'", string("testing, 123!"));
    TEST_TOKENS("'es\\'ca\\\"pe'", string("es'ca\"pe"));
    TEST_TOKENS("'\"quotes\"'", string("\"quotes\""));
    TEST_TOKENS("\"'quotes'\"", string("'quotes'"));
    TEST_TOKENS("\"mismatch'", string("mismatch'"));
    TEST_TOKENS("'text\5\t\13'", string("text\5\t\13"));
    TEST_TOKENS("\"end on eof", string("end on eof"));
    TEST_TOKENS("'esca\\\nped'", string("escaped"));
    TEST_TOKENS("\"esc\\\faped\"", string("escaped"));
    TEST_TOKENS("'new\\\rline'", string("newline"));
    TEST_TOKENS("\"new\\\r\nline\"", string("newline"));
    TEST_TOKENS("'bad\nstring", badString, whitespace, ident("string"));
    TEST_TOKENS("'bad\rstring", badString, whitespace, ident("string"));
    TEST_TOKENS("'bad\r\nstring", badString, whitespace, ident("string"));
    TEST_TOKENS("'bad\fstring", badString, whitespace, ident("string"));
    TEST_TOKENS(String("'\0'", 3), string(fromUChar32(0xFFFD)));
    TEST_TOKENS(String("'hel\0lo'", 8), string("hel" + fromUChar32(0xFFFD) + "lo"));
    TEST_TOKENS(String("'h\\65l\0lo'", 10), string("hel" + fromUChar32(0xFFFD) + "lo"));
}

TEST(CSSTokenizerTest, HashToken)
{
    TEST_TOKENS("#id-selector", hash("id-selector", HashTokenId));
    TEST_TOKENS("#FF7700", hash("FF7700", HashTokenId));
    TEST_TOKENS("#3377FF", hash("3377FF", HashTokenUnrestricted));
    TEST_TOKENS("#\\ ", hash(" ", HashTokenId));
    TEST_TOKENS("# ", delim('#'), whitespace);
    TEST_TOKENS("#\\\n", delim('#'), delim('\\'), whitespace);
    TEST_TOKENS("#\\\r\n", delim('#'), delim('\\'), whitespace);
    TEST_TOKENS("#!", delim('#'), delim('!'));
}

TEST(CSSTokenizerTest, NumberToken)
{
    TEST_TOKENS("10", number(IntegerValueType, 10, NoSign));
    TEST_TOKENS("12.0", number(NumberValueType, 12, NoSign));
    TEST_TOKENS("+45.6", number(NumberValueType, 45.6, PlusSign));
    TEST_TOKENS("-7", number(IntegerValueType, -7, MinusSign));
    TEST_TOKENS("010", number(IntegerValueType, 10, NoSign));
    TEST_TOKENS("10e0", number(NumberValueType, 10, NoSign));
    TEST_TOKENS("12e3", number(NumberValueType, 12000, NoSign));
    TEST_TOKENS("3e+1", number(NumberValueType, 30, NoSign));
    TEST_TOKENS("12E-1", number(NumberValueType, 1.2, NoSign));
    TEST_TOKENS(".7", number(NumberValueType, 0.7, NoSign));
    TEST_TOKENS("-.3", number(NumberValueType, -0.3, MinusSign));
    TEST_TOKENS("+637.54e-2", number(NumberValueType, 6.3754, PlusSign));
    TEST_TOKENS("-12.34E+2", number(NumberValueType, -1234, MinusSign));

    TEST_TOKENS("+ 5", delim('+'), whitespace, number(IntegerValueType, 5, NoSign));
    TEST_TOKENS("-+12", delim('-'), number(IntegerValueType, 12, PlusSign));
    TEST_TOKENS("+-21", delim('+'), number(IntegerValueType, -21, MinusSign));
    TEST_TOKENS("++22", delim('+'), number(IntegerValueType, 22, PlusSign));
    TEST_TOKENS("13.", number(IntegerValueType, 13, NoSign), delim('.'));
    TEST_TOKENS("1.e2", number(IntegerValueType, 1, NoSign), delim('.'), ident("e2"));
    TEST_TOKENS("2e3.5", number(NumberValueType, 2000, NoSign), number(NumberValueType, 0.5, NoSign));
    TEST_TOKENS("2e3.", number(NumberValueType, 2000, NoSign), delim('.'));
    TEST_TOKENS("1000000000000000000000000", number(IntegerValueType, 1e24, NoSign));
}

TEST(CSSTokenizerTest, DimensionToken)
{
    TEST_TOKENS("10px", dimension(IntegerValueType, 10, "px"));
    TEST_TOKENS("12.0em", dimension(NumberValueType, 12, "em"));
    TEST_TOKENS("-12.0em", dimension(NumberValueType, -12, "em"));
    TEST_TOKENS("+45.6__qem", dimension(NumberValueType, 45.6, "__qem"));
    TEST_TOKENS("5e", dimension(IntegerValueType, 5, "e"));
    TEST_TOKENS("5px-2px", dimension(IntegerValueType, 5, "px-2px"));
    TEST_TOKENS("5e-", dimension(IntegerValueType, 5, "e-"));
    TEST_TOKENS("5\\ ", dimension(IntegerValueType, 5, " "));
    TEST_TOKENS("40\\70\\78", dimension(IntegerValueType, 40, "px"));
    TEST_TOKENS("4e3e2", dimension(NumberValueType, 4000, "e2"));
    TEST_TOKENS("0x10px", dimension(IntegerValueType, 0, "x10px"));
    TEST_TOKENS("4unit ", dimension(IntegerValueType, 4, "unit"), whitespace);
    TEST_TOKENS("5e+", dimension(IntegerValueType, 5, "e"), delim('+'));
    TEST_TOKENS("2e.5", dimension(IntegerValueType, 2, "e"), number(NumberValueType, 0.5, NoSign));
    TEST_TOKENS("2e+.5", dimension(IntegerValueType, 2, "e"), number(NumberValueType, 0.5, PlusSign));
}

TEST(CSSTokenizerTest, PercentageToken)
{
    TEST_TOKENS("10%", percentage(IntegerValueType, 10));
    TEST_TOKENS("+12.0%", percentage(NumberValueType, 12));
    TEST_TOKENS("-48.99%", percentage(NumberValueType, -48.99));
    TEST_TOKENS("6e-1%", percentage(NumberValueType, 0.6));
    TEST_TOKENS("5%%", percentage(IntegerValueType, 5), delim('%'));
}

TEST(CSSTokenizerTest, UnicodeRangeToken)
{
    TEST_TOKENS("u+012345-123456", unicodeRange(0x012345, 0x123456));
    TEST_TOKENS("U+1234-2345", unicodeRange(0x1234, 0x2345));
    TEST_TOKENS("u+222-111", unicodeRange(0x222, 0x111));
    TEST_TOKENS("U+CafE-d00D", unicodeRange(0xcafe, 0xd00d));
    TEST_TOKENS("U+2??", unicodeRange(0x200, 0x2ff));
    TEST_TOKENS("U+ab12??", unicodeRange(0xab1200, 0xab12ff));
    TEST_TOKENS("u+??????", unicodeRange(0x000000, 0xffffff));
    TEST_TOKENS("u+??", unicodeRange(0x00, 0xff));

    TEST_TOKENS("u+222+111", unicodeRange(0x222, 0x222), number(IntegerValueType, 111, PlusSign));
    TEST_TOKENS("u+12345678", unicodeRange(0x123456, 0x123456), number(IntegerValueType, 78, NoSign));
    TEST_TOKENS("u+123-12345678", unicodeRange(0x123, 0x123456), number(IntegerValueType, 78, NoSign));
    TEST_TOKENS("u+cake", unicodeRange(0xca, 0xca), ident("ke"));
    TEST_TOKENS("u+1234-gggg", unicodeRange(0x1234, 0x1234), ident("-gggg"));
    TEST_TOKENS("U+ab12???", unicodeRange(0xab1200, 0xab12ff), delim('?'));
    TEST_TOKENS("u+a1?-123", unicodeRange(0xa10, 0xa1f), number(IntegerValueType, -123, MinusSign));
    TEST_TOKENS("u+1??4", unicodeRange(0x100, 0x1ff), number(IntegerValueType, 4, NoSign));
    TEST_TOKENS("u+z", ident("u"), delim('+'), ident("z"));
    TEST_TOKENS("u+", ident("u"), delim('+'));
    TEST_TOKENS("u+-543", ident("u"), delim('+'), number(IntegerValueType, -543, MinusSign));
}

TEST(CSSTokenizerTest, CommentToken)
{
    TEST_TOKENS("/*comment*/a", ident("a"));
    TEST_TOKENS("/**\\2f**//", delim('/'));
    TEST_TOKENS("/**y*a*y**/ ", whitespace);
    TEST_TOKENS(",/* \n :) \n */)", comma, rightParenthesis);
    TEST_TOKENS(":/*/*/", colon);
    TEST_TOKENS("/**/*", delim('*'));
    TEST_TOKENS(";/******", semicolon);
}

typedef struct {
    const char* input;
    const unsigned maxLevel;
    const unsigned finalLevel;
} BlockTestCase;

TEST(CSSTokenizerBlockTest, Basic)
{
    BlockTestCase testCases[] = {
        {"(max-width: 800px()), (max-width: 800px)", 2, 0},
        {"(max-width: 900px(()), (max-width: 900px)", 3, 1},
        {"(max-width: 600px(())))), (max-width: 600px)", 3, 0},
        {"(max-width: 500px(((((((((())))), (max-width: 500px)", 11, 6},
        {"(max-width: 800px[]), (max-width: 800px)", 2, 0},
        {"(max-width: 900px[[]), (max-width: 900px)", 3, 2},
        {"(max-width: 600px[[]]]]), (max-width: 600px)", 3, 0},
        {"(max-width: 500px[[[[[[[[[[]]]]), (max-width: 500px)", 11, 7},
        {"(max-width: 800px{}), (max-width: 800px)", 2, 0},
        {"(max-width: 900px{{}), (max-width: 900px)", 3, 2},
        {"(max-width: 600px{{}}}}), (max-width: 600px)", 3, 0},
        {"(max-width: 500px{{{{{{{{{{}}}}), (max-width: 500px)", 11, 7},
        {"[(), (max-width: 400px)", 2, 1},
        {"[{}, (max-width: 500px)", 2, 1},
        {"[{]}], (max-width: 900px)", 2, 0},
        {"[{[]{}{{{}}}}], (max-width: 900px)", 5, 0},
        {"[{[}], (max-width: 900px)", 3, 2},
        {"[({)}], (max-width: 900px)", 3, 2},
        {"[]((), (max-width: 900px)", 2, 1},
        {"((), (max-width: 900px)", 2, 1},
        {"(foo(), (max-width: 900px)", 2, 1},
        {"[](()), (max-width: 900px)", 2, 0},
        {"all an[isdfs bla())(i())]icalc(i)(()), (max-width: 400px)", 3, 0},
        {"all an[isdfs bla())(]icalc(i)(()), (max-width: 500px)", 4, 2},
        {"all an[isdfs bla())(]icalc(i)(())), (max-width: 600px)", 4, 1},
        {"all an[isdfs bla())(]icalc(i)(()))], (max-width: 800px)", 4, 0},
        {0, 0, 0} // Do not remove the terminator line.
    };
    for (int i = 0; testCases[i].input; ++i) {
        CSSTokenizer::Scope scope(testCases[i].input);
        CSSParserTokenRange range = scope.tokenRange();
        MediaQueryBlockWatcher blockWatcher;

        unsigned maxLevel = 0;
        unsigned level = 0;
        while (!range.atEnd()) {
            blockWatcher.handleToken(range.consume());
            level = blockWatcher.blockLevel();
            maxLevel = std::max(level, maxLevel);
        }
        ASSERT_EQ(testCases[i].maxLevel, maxLevel);
        ASSERT_EQ(testCases[i].finalLevel, level);
    }
}

} // namespace
