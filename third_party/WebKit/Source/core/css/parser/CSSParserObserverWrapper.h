// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSParserObserverWrapper_h
#define CSSParserObserverWrapper_h

#include "core/css/parser/CSSParserObserver.h"

namespace blink {

class CSSParserObserverWrapper {
public:
    CSSParserObserverWrapper(CSSParserObserver& observer)
    : m_observer(observer)
    { }

    unsigned startOffset(const CSSParserTokenRange&);
    unsigned previousTokenStartOffset(const CSSParserTokenRange&);
    unsigned endOffset(const CSSParserTokenRange&); // Includes trailing comments

    void skipCommentsBefore(const CSSParserTokenRange&, bool leaveDirectlyBefore);
    void yieldCommentsBefore(const CSSParserTokenRange&);

    CSSParserObserver& observer() { return m_observer; }
    void addComment(unsigned startOffset, unsigned endOffset, unsigned tokensBefore)
    {
        CommentPosition position = {startOffset, endOffset, tokensBefore};
        m_commentOffsets.append(position);
    }
    void addToken(unsigned startOffset) { m_tokenOffsets.append(startOffset); }
    void finalizeConstruction(CSSParserToken* firstParserToken)
    {
        m_firstParserToken = firstParserToken;
        m_commentIterator = m_commentOffsets.begin();
    }

private:
    CSSParserObserver& m_observer;
    Vector<unsigned> m_tokenOffsets;
    CSSParserToken* m_firstParserToken;

    struct CommentPosition {
        unsigned startOffset;
        unsigned endOffset;
        unsigned tokensBefore;
    };

    Vector<CommentPosition> m_commentOffsets;
    Vector<CommentPosition>::iterator m_commentIterator;
};

} // namespace blink

#endif // CSSParserObserverWrapper_h
