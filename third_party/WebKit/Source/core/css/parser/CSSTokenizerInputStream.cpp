// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSTokenizerInputStream.h"

#include "core/css/parser/CSSParserString.h"
#include "core/html/parser/InputStreamPreprocessor.h"

namespace blink {

CSSTokenizerInputStream::CSSTokenizerInputStream(String input)
    : m_offset(0)
    , m_stringLength(input.length())
    , m_string(input.impl())
{
}

UChar CSSTokenizerInputStream::peek(unsigned lookaheadOffset)
{
    if ((m_offset + lookaheadOffset) >= m_stringLength)
        return kEndOfFileMarker;
    UChar result = (*m_string)[m_offset + lookaheadOffset];
    return result ? result : 0xFFFD;
}

void CSSTokenizerInputStream::pushBack(UChar cc)
{
    --m_offset;
    ASSERT(nextInputChar() == cc);
}

double CSSTokenizerInputStream::getDouble(unsigned start, unsigned end)
{
    ASSERT(start <= end && ((m_offset + end) <= m_stringLength));
    bool isResultOK = false;
    double result = 0.0;
    if (start < end) {
        if (m_string->is8Bit())
            result = charactersToDouble(m_string->characters8() + m_offset + start, end - start, &isResultOK);
        else
            result = charactersToDouble(m_string->characters16() + m_offset + start, end - start, &isResultOK);
    }
    // FIXME: It looks like callers ensure we have a valid number
    return isResultOK ? result : 0.0;
}

CSSParserString CSSTokenizerInputStream::rangeAsCSSParserString(unsigned start, unsigned length) const
{
    ASSERT(start + length <= m_stringLength);
    CSSParserString result;
    if (m_string->is8Bit())
        result.init(m_string->characters8() + start, length);
    else
        result.init(m_string->characters16() + start, length);
    return result;
}

} // namespace blink
