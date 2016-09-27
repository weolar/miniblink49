// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSTokenizer.h"

namespace blink {
#include "core/CSSTokenizerCodepoints.cpp"
}

#include "core/css/parser/CSSParserObserverWrapper.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "core/css/parser/CSSTokenizerInputStream.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

CSSTokenizer::Scope::Scope(const String& string)
: m_string(string)
{
    // According to the spec, we should perform preprocessing here.
    // See: http://dev.w3.org/csswg/css-syntax/#input-preprocessing
    //
    // However, we can skip this step since:
    // * We're using HTML spaces (which accept \r and \f as a valid white space)
    // * Do not count white spaces
    // * CSSTokenizerInputStream::peek replaces NULLs for replacement characters

    if (string.isEmpty())
        return;

    // To avoid resizing we err on the side of reserving too much space.
    // Most strings we tokenize have about 3.5 to 5 characters per token.
    m_tokens.reserveInitialCapacity(string.length() / 3);

    CSSTokenizerInputStream input(string);
    CSSTokenizer tokenizer(input, *this);
    while (true) {
        CSSParserToken token = tokenizer.nextToken();
        if (token.type() == CommentToken)
            continue;
        if (token.type() == EOFToken)
            return;
        m_tokens.append(token);
    }
}

CSSTokenizer::Scope::Scope(const String& string, CSSParserObserverWrapper& wrapper)
: m_string(string)
{
    if (string.isEmpty())
        return;

    CSSTokenizerInputStream input(string);
    CSSTokenizer tokenizer(input, *this);

    unsigned offset = 0;
    while (true) {
        CSSParserToken token = tokenizer.nextToken();
        if (token.type() == EOFToken)
            break;
        if (token.type() == CommentToken) {
            wrapper.addComment(offset, input.offset(), m_tokens.size());
        } else {
            m_tokens.append(token);
            wrapper.addToken(offset);
        }
        offset = input.offset();
    }

    wrapper.addToken(offset);
    wrapper.finalizeConstruction(m_tokens.begin());
}

CSSParserTokenRange CSSTokenizer::Scope::tokenRange()
{
    return m_tokens;
}

unsigned CSSTokenizer::Scope::tokenCount()
{
    return m_tokens.size();
}

// http://dev.w3.org/csswg/css-syntax/#name-start-code-point
static bool isNameStart(UChar c)
{
    if (isASCIIAlpha(c))
        return true;
    if (c == '_')
        return true;
    return !isASCII(c);
}

// http://dev.w3.org/csswg/css-syntax/#name-code-point
static bool isNameChar(UChar c)
{
    return isNameStart(c) || isASCIIDigit(c) || c == '-';
}

static bool isNewLine(UChar cc)
{
    // We check \r and \f here, since we have no preprocessing stage
    return (cc == '\r' || cc == '\n' || cc == '\f');
}

// http://dev.w3.org/csswg/css-syntax/#check-if-two-code-points-are-a-valid-escape
static bool twoCharsAreValidEscape(UChar first, UChar second)
{
    return first == '\\' && !isNewLine(second);
}

CSSTokenizer::CSSTokenizer(CSSTokenizerInputStream& inputStream, Scope& scope)
    : m_input(inputStream)
    , m_scope(scope)
{
}

void CSSTokenizer::reconsume(UChar c)
{
    m_input.pushBack(c);
}

UChar CSSTokenizer::consume()
{
    UChar current = m_input.nextInputChar();
    m_input.advance();
    return current;
}

void CSSTokenizer::consume(unsigned offset)
{
    m_input.advance(offset);
}

CSSParserToken CSSTokenizer::whiteSpace(UChar cc)
{
    consumeUntilNonWhitespace();
    return CSSParserToken(WhitespaceToken);
}

static bool popIfBlockMatches(Vector<CSSParserTokenType>& blockStack, CSSParserTokenType type)
{
    if (!blockStack.isEmpty() && blockStack.last() == type) {
        blockStack.removeLast();
        return true;
    }
    return false;
}

CSSParserToken CSSTokenizer::blockStart(CSSParserTokenType type)
{
    m_blockStack.append(type);
    return CSSParserToken(type, CSSParserToken::BlockStart);
}

CSSParserToken CSSTokenizer::blockStart(CSSParserTokenType blockType, CSSParserTokenType type, CSSParserString name)
{
    m_blockStack.append(blockType);
    return CSSParserToken(type, name, CSSParserToken::BlockStart);
}

CSSParserToken CSSTokenizer::blockEnd(CSSParserTokenType type, CSSParserTokenType startType)
{
    if (popIfBlockMatches(m_blockStack, startType))
        return CSSParserToken(type, CSSParserToken::BlockEnd);
    return CSSParserToken(type);
}

CSSParserToken CSSTokenizer::leftParenthesis(UChar cc)
{
    return blockStart(LeftParenthesisToken);
}

CSSParserToken CSSTokenizer::rightParenthesis(UChar cc)
{
    return blockEnd(RightParenthesisToken, LeftParenthesisToken);
}

CSSParserToken CSSTokenizer::leftBracket(UChar cc)
{
    return blockStart(LeftBracketToken);
}

CSSParserToken CSSTokenizer::rightBracket(UChar cc)
{
    return blockEnd(RightBracketToken, LeftBracketToken);
}

CSSParserToken CSSTokenizer::leftBrace(UChar cc)
{
    return blockStart(LeftBraceToken);
}

CSSParserToken CSSTokenizer::rightBrace(UChar cc)
{
    return blockEnd(RightBraceToken, LeftBraceToken);
}

CSSParserToken CSSTokenizer::plusOrFullStop(UChar cc)
{
    if (nextCharsAreNumber(cc)) {
        reconsume(cc);
        return consumeNumericToken();
    }
    return CSSParserToken(DelimiterToken, cc);
}

CSSParserToken CSSTokenizer::asterisk(UChar cc)
{
    ASSERT(cc == '*');
    if (consumeIfNext('='))
        return CSSParserToken(SubstringMatchToken);
    return CSSParserToken(DelimiterToken, '*');
}

CSSParserToken CSSTokenizer::lessThan(UChar cc)
{
    ASSERT(cc == '<');
    if (m_input.peek(0) == '!' && m_input.peek(1) == '-' && m_input.peek(2) == '-') {
        consume(3);
        return CSSParserToken(CDOToken);
    }
    return CSSParserToken(DelimiterToken, '<');
}

CSSParserToken CSSTokenizer::comma(UChar cc)
{
    return CSSParserToken(CommaToken);
}

CSSParserToken CSSTokenizer::hyphenMinus(UChar cc)
{
    if (nextCharsAreNumber(cc)) {
        reconsume(cc);
        return consumeNumericToken();
    }
    if (m_input.peek(0) == '-' && m_input.peek(1) == '>') {
        consume(2);
        return CSSParserToken(CDCToken);
    }
    if (nextCharsAreIdentifier(cc)) {
        reconsume(cc);
        return consumeIdentLikeToken();
    }
    return CSSParserToken(DelimiterToken, cc);
}

CSSParserToken CSSTokenizer::solidus(UChar cc)
{
    if (consumeIfNext('*')) {
        // These get ignored, but we need a value to return.
        consumeUntilCommentEndFound();
        return CSSParserToken(CommentToken);
    }

    return CSSParserToken(DelimiterToken, cc);
}

CSSParserToken CSSTokenizer::colon(UChar cc)
{
    return CSSParserToken(ColonToken);
}

CSSParserToken CSSTokenizer::semiColon(UChar cc)
{
    return CSSParserToken(SemicolonToken);
}

CSSParserToken CSSTokenizer::hash(UChar cc)
{
    UChar nextChar = m_input.nextInputChar();
    if (isNameChar(nextChar) || twoCharsAreValidEscape(nextChar, m_input.peek(1))) {
        HashTokenType type = nextCharsAreIdentifier() ? HashTokenId : HashTokenUnrestricted;
        return CSSParserToken(type, consumeName());
    }

    return CSSParserToken(DelimiterToken, cc);
}

CSSParserToken CSSTokenizer::circumflexAccent(UChar cc)
{
    ASSERT(cc == '^');
    if (consumeIfNext('='))
        return CSSParserToken(PrefixMatchToken);
    return CSSParserToken(DelimiterToken, '^');
}

CSSParserToken CSSTokenizer::dollarSign(UChar cc)
{
    ASSERT(cc == '$');
    if (consumeIfNext('='))
        return CSSParserToken(SuffixMatchToken);
    return CSSParserToken(DelimiterToken, '$');
}

CSSParserToken CSSTokenizer::verticalLine(UChar cc)
{
    ASSERT(cc == '|');
    if (consumeIfNext('='))
        return CSSParserToken(DashMatchToken);
    if (consumeIfNext('|'))
        return CSSParserToken(ColumnToken);
    return CSSParserToken(DelimiterToken, '|');
}

CSSParserToken CSSTokenizer::tilde(UChar cc)
{
    ASSERT(cc == '~');
    if (consumeIfNext('='))
        return CSSParserToken(IncludeMatchToken);
    return CSSParserToken(DelimiterToken, '~');
}

CSSParserToken CSSTokenizer::commercialAt(UChar cc)
{
    ASSERT(cc == '@');
    if (nextCharsAreIdentifier())
        return CSSParserToken(AtKeywordToken, consumeName());
    return CSSParserToken(DelimiterToken, '@');
}

CSSParserToken CSSTokenizer::reverseSolidus(UChar cc)
{
    if (twoCharsAreValidEscape(cc, m_input.nextInputChar())) {
        reconsume(cc);
        return consumeIdentLikeToken();
    }
    return CSSParserToken(DelimiterToken, cc);
}

CSSParserToken CSSTokenizer::asciiDigit(UChar cc)
{
    reconsume(cc);
    return consumeNumericToken();
}

CSSParserToken CSSTokenizer::letterU(UChar cc)
{
    if (m_input.nextInputChar() == '+'
        && (isASCIIHexDigit(m_input.peek(1)) || m_input.peek(1) == '?')) {
        consume();
        return consumeUnicodeRange();
    }
    reconsume(cc);
    return consumeIdentLikeToken();
}

CSSParserToken CSSTokenizer::nameStart(UChar cc)
{
    reconsume(cc);
    return consumeIdentLikeToken();
}

CSSParserToken CSSTokenizer::stringStart(UChar cc)
{
    return consumeStringTokenUntil(cc);
}

CSSParserToken CSSTokenizer::endOfFile(UChar cc)
{
    return CSSParserToken(EOFToken);
}

CSSParserToken CSSTokenizer::nextToken()
{
    // Unlike the HTMLTokenizer, the CSS Syntax spec is written
    // as a stateless, (fixed-size) look-ahead tokenizer.
    // We could move to the stateful model and instead create
    // states for all the "next 3 codepoints are X" cases.
    // State-machine tokenizers are easier to write to handle
    // incremental tokenization of partial sources.
    // However, for now we follow the spec exactly.
    UChar cc = consume();
    CodePoint codePointFunc = 0;

    if (isASCII(cc)) {
        ASSERT_WITH_SECURITY_IMPLICATION(cc < codePointsNumber);
        codePointFunc = codePoints[cc];
    } else {
        codePointFunc = &CSSTokenizer::nameStart;
    }

    if (codePointFunc)
        return ((this)->*(codePointFunc))(cc);
    return CSSParserToken(DelimiterToken, cc);
}

static NumericSign getSign(CSSTokenizerInputStream& input, unsigned& offset)
{
    if (input.nextInputChar() == '+') {
        ++offset;
        return PlusSign;
    }
    if (input.nextInputChar() == '-') {
        ++offset;
        return MinusSign;
    }
    return NoSign;
}

static double getInteger(CSSTokenizerInputStream& input, unsigned& offset)
{
    unsigned intStartPos = offset;
    offset = input.skipWhilePredicate<isASCIIDigit>(offset);
    unsigned intEndPos = offset;
    return input.getDouble(intStartPos, intEndPos);
}

static double getFraction(CSSTokenizerInputStream& input, unsigned& offset)
{
    if (input.peek(offset) != '.' || !isASCIIDigit(input.peek(offset + 1)))
        return 0;
    unsigned startOffset = offset;
    offset = input.skipWhilePredicate<isASCIIDigit>(offset + 1);
    return input.getDouble(startOffset, offset);
}

static double getExponent(CSSTokenizerInputStream& input, unsigned& offset, int& sign)
{
    unsigned exponentStartPos = 0;
    unsigned exponentEndPos = 0;
    if ((input.peek(offset) == 'E' || input.peek(offset) == 'e')) {
        int offsetBeforeExponent = offset;
        ++offset;
        if (input.peek(offset) == '+') {
            ++offset;
        } else if (input.peek(offset) =='-') {
            sign = -1;
            ++offset;
        }
        exponentStartPos = offset;
        offset = input.skipWhilePredicate<isASCIIDigit>(offset);
        exponentEndPos = offset;
        if (exponentEndPos == exponentStartPos)
            offset = offsetBeforeExponent;
    }
    return input.getDouble(exponentStartPos, exponentEndPos);
}

// This method merges the following spec sections for efficiency
// http://www.w3.org/TR/css3-syntax/#consume-a-number
// http://www.w3.org/TR/css3-syntax/#convert-a-string-to-a-number
CSSParserToken CSSTokenizer::consumeNumber()
{
    ASSERT(nextCharsAreNumber());
    NumericValueType type = IntegerValueType;
    double value = 0;
    unsigned offset = 0;
    int exponentSign = 1;
    NumericSign sign = getSign(m_input, offset);
    double integerPart = getInteger(m_input, offset);
    unsigned integerPartEndOffset = offset;

    double fractionPart = getFraction(m_input, offset);
    double exponentPart = getExponent(m_input, offset, exponentSign);
    double exponent = pow(10, (float)exponentSign * (double)exponentPart);
    value = ((double)integerPart + fractionPart) * exponent;
    if (sign == MinusSign)
        value = -value;

    m_input.advance(offset);
    if (offset != integerPartEndOffset)
        type = NumberValueType;

    return CSSParserToken(NumberToken, value, type, sign);
}

// http://www.w3.org/TR/css3-syntax/#consume-a-numeric-token
CSSParserToken CSSTokenizer::consumeNumericToken()
{
    CSSParserToken token = consumeNumber();
    if (nextCharsAreIdentifier())
        token.convertToDimensionWithUnit(consumeName());
    else if (consumeIfNext('%'))
        token.convertToPercentage();
    return token;
}

// http://dev.w3.org/csswg/css-syntax/#consume-ident-like-token
CSSParserToken CSSTokenizer::consumeIdentLikeToken()
{
    CSSParserString name = consumeName();
    if (consumeIfNext('(')) {
        if (name.equalIgnoringCase("url")) {
            // The spec is slightly different so as to avoid dropping whitespace
            // tokens, but they wouldn't be used and this is easier.
            consumeUntilNonWhitespace();
            UChar next = m_input.nextInputChar();
            if (next != '"' && next != '\'')
                return consumeUrlToken();
        }
        return blockStart(LeftParenthesisToken, FunctionToken, name);
    }
    return CSSParserToken(IdentToken, name);
}

// http://dev.w3.org/csswg/css-syntax/#consume-a-string-token
CSSParserToken CSSTokenizer::consumeStringTokenUntil(UChar endingCodePoint)
{
    // Strings without escapes get handled without allocations
    for (unsigned size = 0; ; size++) {
        UChar cc = m_input.peekWithoutReplacement(size);
        if (cc == endingCodePoint) {
            unsigned startOffset = m_input.offset();
            m_input.advance(size + 1);
            return CSSParserToken(StringToken, m_input.rangeAsCSSParserString(startOffset, size));
        }
        if (isNewLine(cc)) {
            m_input.advance(size);
            return CSSParserToken(BadStringToken);
        }
        if (cc == '\0' || cc == '\\')
            break;
    }

    StringBuilder output;
    while (true) {
        UChar cc = consume();
        if (cc == endingCodePoint || cc == kEndOfFileMarker)
            return CSSParserToken(StringToken, registerString(output.toString()));
        if (isNewLine(cc)) {
            reconsume(cc);
            return CSSParserToken(BadStringToken);
        }
        if (cc == '\\') {
            if (m_input.nextInputChar() == kEndOfFileMarker)
                continue;
            if (isNewLine(m_input.nextInputChar()))
                consumeSingleWhitespaceIfNext(); // This handles \r\n for us
            else
                output.append(consumeEscape());
        } else {
            output.append(cc);
        }
    }
}

CSSParserToken CSSTokenizer::consumeUnicodeRange()
{
    ASSERT(isASCIIHexDigit(m_input.nextInputChar()) || m_input.nextInputChar() == '?');
    int lengthRemaining = 6;
    UChar32 start = 0;

    while (lengthRemaining && isASCIIHexDigit(m_input.nextInputChar())) {
        start = start * 16 + toASCIIHexValue(consume());
        --lengthRemaining;
    }

    UChar32 end = start;
    if (lengthRemaining && consumeIfNext('?')) {
        do {
            start *= 16;
            end = end * 16 + 0xF;
            --lengthRemaining;
        } while (lengthRemaining && consumeIfNext('?'));
    } else if (m_input.nextInputChar() == '-' && isASCIIHexDigit(m_input.peek(1))) {
        consume();
        lengthRemaining = 6;
        end = 0;
        do {
            end = end * 16 + toASCIIHexValue(consume());
            --lengthRemaining;
        } while (lengthRemaining && isASCIIHexDigit(m_input.nextInputChar()));
    }

    return CSSParserToken(UnicodeRangeToken, start, end);
}

// http://dev.w3.org/csswg/css-syntax/#non-printable-code-point
static bool isNonPrintableCodePoint(UChar cc)
{
    return (cc >= '\0' && cc <= '\x8') || cc == '\xb' || (cc >= '\xe' && cc <= '\x1f') || cc == '\x7f';
}

// http://dev.w3.org/csswg/css-syntax/#consume-url-token
CSSParserToken CSSTokenizer::consumeUrlToken()
{
    consumeUntilNonWhitespace();

    // URL tokens without escapes get handled without allocations
    for (unsigned size = 0; ; size++) {
        UChar cc = m_input.peekWithoutReplacement(size);
        if (cc == ')') {
            unsigned startOffset = m_input.offset();
            m_input.advance(size + 1);
            return CSSParserToken(UrlToken, m_input.rangeAsCSSParserString(startOffset, size));
        }
        if (cc <= ' ' || cc == '\\' || cc == '"' || cc == '\'' || cc == '(' || cc == '\x7f')
            break;
    }

    StringBuilder result;
    while (true) {
        UChar cc = consume();
        if (cc == ')' || cc == kEndOfFileMarker)
            return CSSParserToken(UrlToken, registerString(result.toString()));

        if (isHTMLSpace(cc)) {
            consumeUntilNonWhitespace();
            if (consumeIfNext(')') || m_input.nextInputChar() == kEndOfFileMarker)
                return CSSParserToken(UrlToken, registerString(result.toString()));
            break;
        }

        if (cc == '"' || cc == '\'' || cc == '(' || isNonPrintableCodePoint(cc))
            break;

        if (cc == '\\') {
            if (twoCharsAreValidEscape(cc, m_input.nextInputChar())) {
                result.append(consumeEscape());
                continue;
            }
            break;
        }

        result.append(cc);
    }

    consumeBadUrlRemnants();
    return CSSParserToken(BadUrlToken);
}

// http://dev.w3.org/csswg/css-syntax/#consume-the-remnants-of-a-bad-url
void CSSTokenizer::consumeBadUrlRemnants()
{
    while (true) {
        UChar cc = consume();
        if (cc == ')' || cc == kEndOfFileMarker)
            return;
        if (twoCharsAreValidEscape(cc, m_input.nextInputChar()))
            consumeEscape();
    }
}

void CSSTokenizer::consumeUntilNonWhitespace()
{
    // Using HTML space here rather than CSS space since we don't do preprocessing
    while (isHTMLSpace<UChar>(m_input.nextInputChar()))
        consume();
}

void CSSTokenizer::consumeSingleWhitespaceIfNext()
{
    // We check for \r\n and HTML spaces since we don't do preprocessing
    UChar c = m_input.nextInputChar();
    if (c == '\r' && m_input.peek(1) == '\n')
        consume(2);
    else if (isHTMLSpace(c))
        consume();
}

void CSSTokenizer::consumeUntilCommentEndFound()
{
    UChar c = consume();
    while (true) {
        if (c == kEndOfFileMarker)
            return;
        if (c != '*') {
            c = consume();
            continue;
        }
        c = consume();
        if (c == '/')
            return;
    }
}

bool CSSTokenizer::consumeIfNext(UChar character)
{
    if (m_input.nextInputChar() == character) {
        consume();
        return true;
    }
    return false;
}

// http://www.w3.org/TR/css3-syntax/#consume-a-name
CSSParserString CSSTokenizer::consumeName()
{
    // Names without escapes get handled without allocations
    for (unsigned size = 0; ; ++size) {
        UChar cc = m_input.peekWithoutReplacement(size);
        if (cc == '\0' || cc == '\\')
            break;
        if (!isNameChar(cc)) {
            unsigned startOffset = m_input.offset();
            m_input.advance(size);
            return m_input.rangeAsCSSParserString(startOffset, size);
        }
    }

    StringBuilder result;
    while (true) {
        UChar cc = consume();
        if (isNameChar(cc)) {
            result.append(cc);
            continue;
        }
        if (twoCharsAreValidEscape(cc, m_input.nextInputChar())) {
            result.append(consumeEscape());
            continue;
        }
        reconsume(cc);
        return registerString(result.toString());
    }
}

// http://dev.w3.org/csswg/css-syntax/#consume-an-escaped-code-point
UChar32 CSSTokenizer::consumeEscape()
{
    UChar cc = consume();
    ASSERT(!isNewLine(cc));
    if (isASCIIHexDigit(cc)) {
        unsigned consumedHexDigits = 1;
        StringBuilder hexChars;
        hexChars.append(cc);
        while (consumedHexDigits < 6 && isASCIIHexDigit(m_input.nextInputChar())) {
            cc = consume();
            hexChars.append(cc);
            consumedHexDigits++;
        };
        consumeSingleWhitespaceIfNext();
        bool ok = false;
        UChar32 codePoint = hexChars.toString().toUIntStrict(&ok, 16);
        ASSERT(ok);
        if (codePoint == 0 || (0xD800 <= codePoint && codePoint <= 0xDFFF) || codePoint > 0x10FFFF)
            return replacementCharacter;
        return codePoint;
    }

    if (cc == kEndOfFileMarker)
        return replacementCharacter;
    return cc;
}

bool CSSTokenizer::nextTwoCharsAreValidEscape()
{
    return twoCharsAreValidEscape(m_input.nextInputChar(), m_input.peek(1));
}

// http://www.w3.org/TR/css3-syntax/#starts-with-a-number
bool CSSTokenizer::nextCharsAreNumber(UChar first)
{
    UChar second = m_input.nextInputChar();
    if (isASCIIDigit(first))
        return true;
    if (first == '+' || first == '-')
        return ((isASCIIDigit(second)) || (second == '.' && isASCIIDigit(m_input.peek(1))));
    if (first =='.')
        return (isASCIIDigit(second));
    return false;
}

bool CSSTokenizer::nextCharsAreNumber()
{
    UChar first = consume();
    bool areNumber = nextCharsAreNumber(first);
    reconsume(first);
    return areNumber;
}

// http://dev.w3.org/csswg/css-syntax/#would-start-an-identifier
bool CSSTokenizer::nextCharsAreIdentifier(UChar first)
{
    UChar second = m_input.nextInputChar();
    if (isNameStart(first) || twoCharsAreValidEscape(first, second))
        return true;

    if (first == '-')
        return isNameStart(second) || second == '-' || nextTwoCharsAreValidEscape();

    return false;
}

bool CSSTokenizer::nextCharsAreIdentifier()
{
    UChar first = consume();
    bool areIdentifier = nextCharsAreIdentifier(first);
    reconsume(first);
    return areIdentifier;
}

CSSParserString CSSTokenizer::registerString(const String& string)
{
    m_scope.storeString(string);
    CSSParserString result;
    result.init(string);
    return result;
}

} // namespace blink
