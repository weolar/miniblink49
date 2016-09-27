// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSTokenizer_h
#define CSSTokenizer_h

#include "core/CoreExport.h"
#include "core/css/parser/CSSParserToken.h"
#include "core/html/parser/InputStreamPreprocessor.h"
#include "wtf/text/WTFString.h"

#include <climits>

namespace blink {

class CSSTokenizerInputStream;
class CSSParserObserverWrapper;
struct CSSParserString;
class CSSParserTokenRange;

class CORE_EXPORT CSSTokenizer {
    WTF_MAKE_NONCOPYABLE(CSSTokenizer);
    WTF_MAKE_FAST_ALLOCATED(CSSTokenizer);
public:
    class CORE_EXPORT Scope {
    public:
        Scope(const String&);
        Scope(const String&, CSSParserObserverWrapper&); // For the inspector

        CSSParserTokenRange tokenRange();
        unsigned tokenCount();

    private:
        void storeString(const String& string) { m_stringPool.append(string); }
        Vector<CSSParserToken> m_tokens;
        // We only allocate strings when escapes are used.
        Vector<String> m_stringPool;
        String m_string;

        friend class CSSTokenizer;
    };

private:
    CSSTokenizer(CSSTokenizerInputStream&, Scope&);

    CSSParserToken nextToken();

    UChar consume();
    void consume(unsigned);
    void reconsume(UChar);

    CSSParserToken consumeNumericToken();
    CSSParserToken consumeIdentLikeToken();
    CSSParserToken consumeNumber();
    CSSParserToken consumeStringTokenUntil(UChar);
    CSSParserToken consumeUnicodeRange();
    CSSParserToken consumeUrlToken();

    void consumeBadUrlRemnants();
    void consumeUntilNonWhitespace();
    void consumeSingleWhitespaceIfNext();
    void consumeUntilCommentEndFound();

    bool consumeIfNext(UChar);
    CSSParserString consumeName();
    UChar32 consumeEscape();

    bool nextTwoCharsAreValidEscape();
    bool nextCharsAreNumber(UChar);
    bool nextCharsAreNumber();
    bool nextCharsAreIdentifier(UChar);
    bool nextCharsAreIdentifier();
    CSSParserToken blockStart(CSSParserTokenType);
    CSSParserToken blockStart(CSSParserTokenType blockType, CSSParserTokenType, CSSParserString);
    CSSParserToken blockEnd(CSSParserTokenType, CSSParserTokenType startType);

    typedef CSSParserToken (CSSTokenizer::*CodePoint)(UChar);

    static const CodePoint codePoints[];
    Vector<CSSParserTokenType> m_blockStack;

    CSSParserToken whiteSpace(UChar);
    CSSParserToken leftParenthesis(UChar);
    CSSParserToken rightParenthesis(UChar);
    CSSParserToken leftBracket(UChar);
    CSSParserToken rightBracket(UChar);
    CSSParserToken leftBrace(UChar);
    CSSParserToken rightBrace(UChar);
    CSSParserToken plusOrFullStop(UChar);
    CSSParserToken comma(UChar);
    CSSParserToken hyphenMinus(UChar);
    CSSParserToken asterisk(UChar);
    CSSParserToken lessThan(UChar);
    CSSParserToken solidus(UChar);
    CSSParserToken colon(UChar);
    CSSParserToken semiColon(UChar);
    CSSParserToken hash(UChar);
    CSSParserToken circumflexAccent(UChar);
    CSSParserToken dollarSign(UChar);
    CSSParserToken verticalLine(UChar);
    CSSParserToken tilde(UChar);
    CSSParserToken commercialAt(UChar);
    CSSParserToken reverseSolidus(UChar);
    CSSParserToken asciiDigit(UChar);
    CSSParserToken letterU(UChar);
    CSSParserToken nameStart(UChar);
    CSSParserToken stringStart(UChar);
    CSSParserToken endOfFile(UChar);

    CSSParserString registerString(const String&);

    CSSTokenizerInputStream& m_input;
    Scope& m_scope;
};



} // namespace blink

#endif // CSSTokenizer_h
