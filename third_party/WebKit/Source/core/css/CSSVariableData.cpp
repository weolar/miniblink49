// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/CSSVariableData.h"

#include "core/css/parser/CSSParserTokenRange.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

template<typename CharacterType> void CSSVariableData::updateTokens(const CSSParserTokenRange& range)
{
    const CharacterType* currentOffset = m_backingString.getCharacters<CharacterType>();
    for (const CSSParserToken& token : range) {
        if (token.hasStringBacking()) {
            unsigned length = token.value().length();
            CSSParserString parserString;
            parserString.init(currentOffset, length);
            m_tokens.append(token.copyWithUpdatedString(parserString));
            currentOffset += length;
        } else {
            m_tokens.append(token);
        }
    }
    ASSERT(currentOffset == m_backingString.getCharacters<CharacterType>() + m_backingString.length());
}

void CSSVariableData::consumeAndUpdateTokens(const CSSParserTokenRange& range)
{
    StringBuilder stringBuilder;
    CSSParserTokenRange localRange = range;

    while (!localRange.atEnd()) {
        CSSParserToken token = localRange.consume();
        if (token.hasStringBacking()) {
            CSSParserString value = token.value();
            if (value.is8Bit())
                stringBuilder.append(value.characters8(), value.length());
            else
                stringBuilder.append(value.characters16(), value.length());
        }
    }
    m_backingString = stringBuilder.toString();
    if (m_backingString.is8Bit())
        updateTokens<LChar>(range);
    else
        updateTokens<UChar>(range);
}

CSSVariableData::CSSVariableData(const CSSParserTokenRange& range, bool needsVariableResolution)
    : m_needsVariableResolution(needsVariableResolution)
{
    ASSERT(!range.atEnd());
    consumeAndUpdateTokens(range);
}

} // namespace blink