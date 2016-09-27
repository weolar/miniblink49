// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSSupportsParser_h
#define CSSSupportsParser_h

namespace blink {

class CSSParserImpl;
class CSSParserTokenRange;

class CSSSupportsParser {
public:
    enum SupportsResult {
        Unsupported = false,
        Supported = true,
        Invalid
    };

    static SupportsResult supportsCondition(CSSParserTokenRange, CSSParserImpl&);

private:
    CSSSupportsParser(CSSParserImpl& parser) : m_parser(parser) { }

    SupportsResult consumeCondition(CSSParserTokenRange);
    SupportsResult consumeNegation(CSSParserTokenRange);

    SupportsResult consumeConditionInParenthesis(CSSParserTokenRange&);

    CSSParserImpl& m_parser;
};

} // namespace blink

#endif // CSSSupportsParser_h
