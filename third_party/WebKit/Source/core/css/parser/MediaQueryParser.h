// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaQueryParser_h
#define MediaQueryParser_h

#include "core/CoreExport.h"
#include "core/css/MediaList.h"
#include "core/css/MediaQuery.h"
#include "core/css/MediaQueryExp.h"
#include "core/css/parser/CSSParserToken.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "core/css/parser/MediaQueryBlockWatcher.h"
#include "wtf/text/WTFString.h"

namespace blink {

class MediaQuerySet;

class MediaQueryData {
    STACK_ALLOCATED();
private:
    MediaQuery::Restrictor m_restrictor;
    String m_mediaType;
    OwnPtrWillBeMember<ExpressionHeapVector> m_expressions;
    String m_mediaFeature;
    Vector<CSSParserToken, 4> m_valueList;
    bool m_mediaTypeSet;

public:
    MediaQueryData();
    void clear();
    bool addExpression();
    bool tryAddParserToken(CSSParserTokenType, const CSSParserToken&);
    void setMediaType(const String&);
    PassOwnPtrWillBeRawPtr<MediaQuery> takeMediaQuery();

    inline bool currentMediaQueryChanged() const
    {
        return (m_restrictor != MediaQuery::None || m_mediaTypeSet || m_expressions->size() > 0);
    }
    inline MediaQuery::Restrictor restrictor() { return m_restrictor; }

    inline void setRestrictor(MediaQuery::Restrictor restrictor) { m_restrictor = restrictor; }

    inline void setMediaFeature(const String& str) { m_mediaFeature = str; }
};

class CORE_EXPORT MediaQueryParser {
    STACK_ALLOCATED();
public:
    static PassRefPtrWillBeRawPtr<MediaQuerySet> parseMediaQuerySet(const String&);
    static PassRefPtrWillBeRawPtr<MediaQuerySet> parseMediaQuerySet(CSSParserTokenRange);
    static PassRefPtrWillBeRawPtr<MediaQuerySet> parseMediaCondition(CSSParserTokenRange);

private:
    enum ParserType {
        MediaQuerySetParser,
        MediaConditionParser,
    };

    MediaQueryParser(ParserType);
    virtual ~MediaQueryParser();

    PassRefPtrWillBeRawPtr<MediaQuerySet> parseImpl(CSSParserTokenRange);

    void processToken(const CSSParserToken&);

    void readRestrictor(CSSParserTokenType, const CSSParserToken&);
    void readMediaNot(CSSParserTokenType, const CSSParserToken&);
    void readMediaType(CSSParserTokenType, const CSSParserToken&);
    void readAnd(CSSParserTokenType, const CSSParserToken&);
    void readFeatureStart(CSSParserTokenType, const CSSParserToken&);
    void readFeature(CSSParserTokenType, const CSSParserToken&);
    void readFeatureColon(CSSParserTokenType, const CSSParserToken&);
    void readFeatureValue(CSSParserTokenType, const CSSParserToken&);
    void readFeatureEnd(CSSParserTokenType, const CSSParserToken&);
    void skipUntilComma(CSSParserTokenType, const CSSParserToken&);
    void skipUntilBlockEnd(CSSParserTokenType, const CSSParserToken&);
    void done(CSSParserTokenType, const CSSParserToken&);

    typedef void (MediaQueryParser::*State)(CSSParserTokenType, const CSSParserToken&);

    void setStateAndRestrict(State, MediaQuery::Restrictor);
    void handleBlocks(const CSSParserToken&);

    State m_state;
    ParserType m_parserType;
    MediaQueryData m_mediaQueryData;
    RefPtrWillBeMember<MediaQuerySet> m_querySet;
    MediaQueryBlockWatcher m_blockWatcher;

    const static State ReadRestrictor;
    const static State ReadMediaNot;
    const static State ReadMediaType;
    const static State ReadAnd;
    const static State ReadFeatureStart;
    const static State ReadFeature;
    const static State ReadFeatureColon;
    const static State ReadFeatureValue;
    const static State ReadFeatureEnd;
    const static State SkipUntilComma;
    const static State SkipUntilBlockEnd;
    const static State Done;

};

} // namespace blink

#endif // MediaQueryParser_h
