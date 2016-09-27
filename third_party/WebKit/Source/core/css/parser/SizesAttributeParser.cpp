// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/SizesAttributeParser.h"

#include "core/MediaTypeNames.h"
#include "core/css/MediaQueryEvaluator.h"
#include "core/css/parser/CSSTokenizer.h"
#include "core/css/parser/SizesCalcParser.h"

namespace blink {

SizesAttributeParser::SizesAttributeParser(PassRefPtr<MediaValues> mediaValues, const String& attribute)
    : m_mediaValues(mediaValues)
    , m_length(0)
    , m_lengthWasSet(false)
{
    m_isValid = parse(CSSTokenizer::Scope(attribute).tokenRange());
}

float SizesAttributeParser::length()
{
    if (m_isValid)
        return effectiveSize();
    return effectiveSizeDefaultValue();
}

bool SizesAttributeParser::calculateLengthInPixels(CSSParserTokenRange range, float& result)
{
    const CSSParserToken& startToken = range.peek();
    CSSParserTokenType type = startToken.type();
    if (type == DimensionToken) {
        double length;
        if (!CSSPrimitiveValue::isLength(startToken.unitType()))
            return false;
        if ((m_mediaValues->computeLength(startToken.numericValue(), startToken.unitType(), length)) && (length >= 0)) {
            result = clampTo<float>(length);
            return true;
        }
    } else if (type == FunctionToken) {
        SizesCalcParser calcParser(range, m_mediaValues);
        if (!calcParser.isValid())
            return false;
        result = calcParser.result();
        return true;
    } else if (type == NumberToken && !startToken.numericValue()) {
        result = 0;
        return true;
    }

    return false;
}

bool SizesAttributeParser::mediaConditionMatches(PassRefPtrWillBeRawPtr<MediaQuerySet> mediaCondition)
{
    // A Media Condition cannot have a media type other then screen.
    MediaQueryEvaluator mediaQueryEvaluator(*m_mediaValues);
    return mediaQueryEvaluator.eval(mediaCondition.get());
}

bool SizesAttributeParser::parse(CSSParserTokenRange range)
{
    // Split on a comma token and parse the result tokens as (media-condition, length) pairs
    while (!range.atEnd()) {
        const CSSParserToken* mediaConditionStart = &range.peek();
        // The length is the last component value before the comma which isn't whitespace or a comment
        const CSSParserToken* lengthTokenStart = &range.peek();
        const CSSParserToken* lengthTokenEnd = &range.peek();
        while (!range.atEnd() && range.peek().type() != CommaToken) {
            lengthTokenStart = &range.peek();
            range.consumeComponentValue();
            lengthTokenEnd = &range.peek();
            range.consumeWhitespace();
        }
        range.consume();

        float length;
        if (!calculateLengthInPixels(range.makeSubRange(lengthTokenStart, lengthTokenEnd), length))
            continue;
        RefPtrWillBeRawPtr<MediaQuerySet> mediaCondition = MediaQueryParser::parseMediaCondition(range.makeSubRange(mediaConditionStart, lengthTokenStart));
        if (!mediaCondition || !mediaConditionMatches(mediaCondition))
            continue;
        m_length = length;
        m_lengthWasSet = true;
        return true;
    }
    return false;
}

float SizesAttributeParser::effectiveSize()
{
    if (m_lengthWasSet)
        return m_length;
    return effectiveSizeDefaultValue();
}

unsigned SizesAttributeParser::effectiveSizeDefaultValue()
{
    // Returning the equivalent of "100vw"
    return m_mediaValues->viewportWidth();
}

} // namespace
