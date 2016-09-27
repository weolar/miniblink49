// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSParserObserverWrapper.h"

#include "core/css/parser/CSSParserTokenRange.h"

namespace blink {

unsigned CSSParserObserverWrapper::startOffset(const CSSParserTokenRange& range)
{
    return m_tokenOffsets[range.begin() - m_firstParserToken];
}

unsigned CSSParserObserverWrapper::previousTokenStartOffset(const CSSParserTokenRange& range)
{
    if (range.begin() == m_firstParserToken)
        return 0;
    return m_tokenOffsets[range.begin() - m_firstParserToken - 1];
}

unsigned CSSParserObserverWrapper::endOffset(const CSSParserTokenRange& range)
{
    return m_tokenOffsets[range.end() - m_firstParserToken];
}

void CSSParserObserverWrapper::skipCommentsBefore(const CSSParserTokenRange& range, bool leaveDirectlyBefore)
{
    unsigned startIndex = range.begin() - m_firstParserToken;
    if (!leaveDirectlyBefore)
        startIndex++;
    while (m_commentIterator < m_commentOffsets.end() && m_commentIterator->tokensBefore < startIndex)
        m_commentIterator++;
}

void CSSParserObserverWrapper::yieldCommentsBefore(const CSSParserTokenRange& range)
{
    unsigned startIndex = range.begin() - m_firstParserToken;
    while (m_commentIterator < m_commentOffsets.end() && m_commentIterator->tokensBefore <= startIndex) {
        m_observer.observeComment(m_commentIterator->startOffset, m_commentIterator->endOffset);
        m_commentIterator++;
    }
}

} // namespace blink
