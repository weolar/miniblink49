// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/MediaQueryParser.h"

#include "core/MediaTypeNames.h"
#include "core/css/parser/CSSPropertyParser.h"
#include "core/css/parser/CSSTokenizer.h"
#include "wtf/Vector.h"

namespace blink {

PassRefPtrWillBeRawPtr<MediaQuerySet> MediaQueryParser::parseMediaQuerySet(const String& queryString)
{
    return parseMediaQuerySet(CSSTokenizer::Scope(queryString).tokenRange());
}

PassRefPtrWillBeRawPtr<MediaQuerySet> MediaQueryParser::parseMediaQuerySet(CSSParserTokenRange range)
{
    return MediaQueryParser(MediaQuerySetParser).parseImpl(range);
}

PassRefPtrWillBeRawPtr<MediaQuerySet> MediaQueryParser::parseMediaCondition(CSSParserTokenRange range)
{
    return MediaQueryParser(MediaConditionParser).parseImpl(range);
}

const MediaQueryParser::State MediaQueryParser::ReadRestrictor = &MediaQueryParser::readRestrictor;
const MediaQueryParser::State MediaQueryParser::ReadMediaNot = &MediaQueryParser::readMediaNot;
const MediaQueryParser::State MediaQueryParser::ReadMediaType = &MediaQueryParser::readMediaType;
const MediaQueryParser::State MediaQueryParser::ReadAnd = &MediaQueryParser::readAnd;
const MediaQueryParser::State MediaQueryParser::ReadFeatureStart = &MediaQueryParser::readFeatureStart;
const MediaQueryParser::State MediaQueryParser::ReadFeature = &MediaQueryParser::readFeature;
const MediaQueryParser::State MediaQueryParser::ReadFeatureColon = &MediaQueryParser::readFeatureColon;
const MediaQueryParser::State MediaQueryParser::ReadFeatureValue = &MediaQueryParser::readFeatureValue;
const MediaQueryParser::State MediaQueryParser::ReadFeatureEnd = &MediaQueryParser::readFeatureEnd;
const MediaQueryParser::State MediaQueryParser::SkipUntilComma = &MediaQueryParser::skipUntilComma;
const MediaQueryParser::State MediaQueryParser::SkipUntilBlockEnd = &MediaQueryParser::skipUntilBlockEnd;
const MediaQueryParser::State MediaQueryParser::Done = &MediaQueryParser::done;

MediaQueryParser::MediaQueryParser(ParserType parserType)
    : m_parserType(parserType)
    , m_querySet(MediaQuerySet::create())
{
    if (parserType == MediaQuerySetParser)
        m_state = &MediaQueryParser::readRestrictor;
    else // MediaConditionParser
        m_state = &MediaQueryParser::readMediaNot;
}

MediaQueryParser::~MediaQueryParser() { }

void MediaQueryParser::setStateAndRestrict(State state, MediaQuery::Restrictor restrictor)
{
    m_mediaQueryData.setRestrictor(restrictor);
    m_state = state;
}

// State machine member functions start here
void MediaQueryParser::readRestrictor(CSSParserTokenType type, const CSSParserToken& token)
{
    readMediaType(type, token);
}

void MediaQueryParser::readMediaNot(CSSParserTokenType type, const CSSParserToken& token)
{
	if (type == IdentToken && token.value().equalIgnoringCase("not"))
        setStateAndRestrict(ReadFeatureStart, MediaQuery::Not);
    else
        readFeatureStart(type, token);
}

static bool isRestrictorOrLogicalOperator(const String& tokenValue)
{
    // FIXME: it would be more efficient to use lower-case always for tokenValue.
    return equalIgnoringCase(tokenValue, "not")
        || equalIgnoringCase(tokenValue, "and")
        || equalIgnoringCase(tokenValue, "or")
        || equalIgnoringCase(tokenValue, "only");
}

void MediaQueryParser::readMediaType(CSSParserTokenType type, const CSSParserToken& token)
{
    if (type == LeftParenthesisToken) {
        if (m_mediaQueryData.restrictor() != MediaQuery::None)
            m_state = SkipUntilComma;
        else
            m_state = ReadFeature;
    } else if (type == IdentToken) {
		if (m_state == ReadRestrictor && token.value().equalIgnoringCase("not")) {
            setStateAndRestrict(ReadMediaType, MediaQuery::Not);
        } else if (m_state == ReadRestrictor && token.value().equalIgnoringCase("only")) {
            setStateAndRestrict(ReadMediaType, MediaQuery::Only);
        } else if (m_mediaQueryData.restrictor() != MediaQuery::None
            && isRestrictorOrLogicalOperator(token.value())) {
            m_state = SkipUntilComma;
        } else {
            m_mediaQueryData.setMediaType(token.value());
            m_state = ReadAnd;
        }
    } else if (type == EOFToken && (!m_querySet->queryVector().size() || m_state != ReadRestrictor)) {
        m_state = Done;
    } else {
        m_state = SkipUntilComma;
        if (type == CommaToken)
            skipUntilComma(type, token);
    }
}

void MediaQueryParser::readAnd(CSSParserTokenType type, const CSSParserToken& token)
{
	if (type == IdentToken && token.value().equalIgnoringCase("and")) {
        m_state = ReadFeatureStart;
    } else if (type == CommaToken && m_parserType != MediaConditionParser) {
        m_querySet->addMediaQuery(m_mediaQueryData.takeMediaQuery());
        m_state = ReadRestrictor;
    } else if (type == EOFToken) {
        m_state = Done;
    } else {
        m_state = SkipUntilComma;
    }
}

void MediaQueryParser::readFeatureStart(CSSParserTokenType type, const CSSParserToken& token)
{
    if (type == LeftParenthesisToken)
        m_state = ReadFeature;
    else
        m_state = SkipUntilComma;
}

void MediaQueryParser::readFeature(CSSParserTokenType type, const CSSParserToken& token)
{
    if (type == IdentToken) {
        m_mediaQueryData.setMediaFeature(token.value());
        m_state = ReadFeatureColon;
    } else {
        m_state = SkipUntilComma;
    }
}

void MediaQueryParser::readFeatureColon(CSSParserTokenType type, const CSSParserToken& token)
{
    if (type == ColonToken)
        m_state = ReadFeatureValue;
    else if (type == RightParenthesisToken || type == EOFToken)
        readFeatureEnd(type, token);
    else
        m_state = SkipUntilBlockEnd;
}

void MediaQueryParser::readFeatureValue(CSSParserTokenType type, const CSSParserToken& token)
{
    if (type == DimensionToken && token.unitType() == CSSPrimitiveValue::CSS_UNKNOWN) {
        m_state = SkipUntilComma;
    } else {
        if (m_mediaQueryData.tryAddParserToken(type, token))
            m_state = ReadFeatureEnd;
        else
            m_state = SkipUntilBlockEnd;
    }
}

void MediaQueryParser::readFeatureEnd(CSSParserTokenType type, const CSSParserToken& token)
{
    if (type == RightParenthesisToken || type == EOFToken) {
        if (m_mediaQueryData.addExpression())
            m_state = ReadAnd;
        else
            m_state = SkipUntilComma;
    } else if (type == DelimiterToken && token.delimiter() == '/') {
        m_mediaQueryData.tryAddParserToken(type, token);
        m_state = ReadFeatureValue;
    } else {
        m_state = SkipUntilBlockEnd;
    }
}

void MediaQueryParser::skipUntilComma(CSSParserTokenType type, const CSSParserToken& token)
{
    if ((type == CommaToken && !m_blockWatcher.blockLevel()) || type == EOFToken) {
        m_state = ReadRestrictor;
        m_mediaQueryData.clear();
        m_querySet->addMediaQuery(MediaQuery::createNotAll());
    }
}

void MediaQueryParser::skipUntilBlockEnd(CSSParserTokenType type, const CSSParserToken& token)
{
    if (token.blockType() == CSSParserToken::BlockEnd && !m_blockWatcher.blockLevel())
        m_state = SkipUntilComma;
}

void MediaQueryParser::done(CSSParserTokenType type, const CSSParserToken& token) { }

void MediaQueryParser::handleBlocks(const CSSParserToken& token)
{
    if (token.blockType() == CSSParserToken::BlockStart
        && (token.type() != LeftParenthesisToken || m_blockWatcher.blockLevel()))
            m_state = SkipUntilBlockEnd;
}

void MediaQueryParser::processToken(const CSSParserToken& token)
{
    CSSParserTokenType type = token.type();

    handleBlocks(token);
    m_blockWatcher.handleToken(token);

    // Call the function that handles current state
    if (type != WhitespaceToken)
        ((this)->*(m_state))(type, token);
}

// The state machine loop
PassRefPtrWillBeRawPtr<MediaQuerySet> MediaQueryParser::parseImpl(CSSParserTokenRange range)
{
    while (!range.atEnd())
        processToken(range.consume());

    // FIXME: Can we get rid of this special case?
    if (m_parserType == MediaQuerySetParser)
        processToken(CSSParserToken(EOFToken));

    if (m_state != ReadAnd && m_state != ReadRestrictor && m_state != Done && m_state != ReadMediaNot)
        m_querySet->addMediaQuery(MediaQuery::createNotAll());
    else if (m_mediaQueryData.currentMediaQueryChanged())
        m_querySet->addMediaQuery(m_mediaQueryData.takeMediaQuery());

    return m_querySet;
}

MediaQueryData::MediaQueryData()
    : m_restrictor(MediaQuery::None)
    , m_mediaType(MediaTypeNames::all)
    , m_expressions(adoptPtrWillBeNoop(new ExpressionHeapVector))
    , m_mediaTypeSet(false)
{
}

void MediaQueryData::clear()
{
    m_restrictor = MediaQuery::None;
    m_mediaType = MediaTypeNames::all;
    m_mediaTypeSet = false;
    m_mediaFeature = String();
    m_valueList.clear();
    m_expressions = adoptPtrWillBeNoop(new ExpressionHeapVector);
}

PassOwnPtrWillBeRawPtr<MediaQuery> MediaQueryData::takeMediaQuery()
{
    OwnPtrWillBeRawPtr<MediaQuery> mediaQuery = adoptPtrWillBeNoop(new MediaQuery(m_restrictor, m_mediaType, m_expressions.release()));
    clear();
    return mediaQuery.release();
}

bool MediaQueryData::addExpression()
{
    OwnPtrWillBeRawPtr<MediaQueryExp> expression = MediaQueryExp::createIfValid(m_mediaFeature, m_valueList);
    bool isValid = !!expression;
    m_expressions->append(expression.release());
    m_valueList.clear();
    return isValid;
}

bool MediaQueryData::tryAddParserToken(CSSParserTokenType type, const CSSParserToken& token)
{
    if (type == NumberToken || type == PercentageToken || type == DimensionToken
        || type == DelimiterToken || type == IdentToken) {
        m_valueList.append(token);
        return true;
    }

    return false;
}

void MediaQueryData::setMediaType(const String& mediaType)
{
    m_mediaType = mediaType;
    m_mediaTypeSet = true;
}

} // namespace blink
