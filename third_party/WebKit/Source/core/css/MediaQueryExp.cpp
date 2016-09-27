/*
 * CSS Media Query
 *
 * Copyright (C) 2006 Kimmo Kinnunen <kimmo.t.kinnunen@nokia.com>.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/css/MediaQueryExp.h"

#include "core/css/CSSPrimitiveValue.h"
#include "core/css/parser/CSSParserToken.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "platform/Decimal.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/text/StringBuffer.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace MediaFeatureNames;

static inline bool featureWithValidIdent(const String& mediaFeature, CSSValueID ident)
{
    if (mediaFeature == displayModeMediaFeature)
        return ident == CSSValueFullscreen || ident == CSSValueStandalone || ident == CSSValueMinimalUi || ident == CSSValueBrowser;

    if (mediaFeature == orientationMediaFeature)
        return ident == CSSValuePortrait || ident == CSSValueLandscape;

    if (mediaFeature == pointerMediaFeature || mediaFeature == anyPointerMediaFeature)
        return ident == CSSValueNone || ident == CSSValueCoarse || ident == CSSValueFine;

    if (mediaFeature == hoverMediaFeature || mediaFeature == anyHoverMediaFeature)
        return ident == CSSValueNone || ident == CSSValueOnDemand || ident == CSSValueHover;

    if (mediaFeature == scanMediaFeature)
        return ident == CSSValueInterlace || ident == CSSValueProgressive;

    return false;
}

static inline bool featureWithValidPositiveLength(const String& mediaFeature, const CSSParserToken& token)
{
    if (!(CSSPrimitiveValue::isLength(token.unitType()) || (token.type() == NumberToken && token.numericValue() == 0)) || token.numericValue() < 0)
        return false;


    return mediaFeature == heightMediaFeature
        || mediaFeature == maxHeightMediaFeature
        || mediaFeature == minHeightMediaFeature
        || mediaFeature == widthMediaFeature
        || mediaFeature == maxWidthMediaFeature
        || mediaFeature == minWidthMediaFeature
        || mediaFeature == deviceHeightMediaFeature
        || mediaFeature == maxDeviceHeightMediaFeature
        || mediaFeature == minDeviceHeightMediaFeature
        || mediaFeature == deviceWidthMediaFeature
        || mediaFeature == minDeviceWidthMediaFeature
        || mediaFeature == maxDeviceWidthMediaFeature;
}

static inline bool featureWithValidDensity(const String& mediaFeature, const CSSParserToken& token)
{
    if ((token.unitType() != CSSPrimitiveValue::CSS_DPPX && token.unitType() != CSSPrimitiveValue::CSS_DPI && token.unitType() != CSSPrimitiveValue::CSS_DPCM) || token.numericValue() <= 0)
        return false;

    return mediaFeature == resolutionMediaFeature
        || mediaFeature == minResolutionMediaFeature
        || mediaFeature == maxResolutionMediaFeature;
}

static inline bool featureWithPositiveInteger(const String& mediaFeature, const CSSParserToken& token)
{
    if (token.numericValueType() != IntegerValueType || token.numericValue() < 0)
        return false;

    return mediaFeature == colorMediaFeature
        || mediaFeature == maxColorMediaFeature
        || mediaFeature == minColorMediaFeature
        || mediaFeature == colorIndexMediaFeature
        || mediaFeature == maxColorIndexMediaFeature
        || mediaFeature == minColorIndexMediaFeature
        || mediaFeature == monochromeMediaFeature
        || mediaFeature == maxMonochromeMediaFeature
        || mediaFeature == minMonochromeMediaFeature;
}

static inline bool featureWithPositiveNumber(const String& mediaFeature, const CSSParserToken& token)
{
    if (token.type() != NumberToken || token.numericValue() < 0)
        return false;

    return mediaFeature == transform3dMediaFeature
        || mediaFeature == devicePixelRatioMediaFeature
        || mediaFeature == maxDevicePixelRatioMediaFeature
        || mediaFeature == minDevicePixelRatioMediaFeature;
}

static inline bool featureWithZeroOrOne(const String& mediaFeature, const CSSParserToken& token)
{
    if (token.numericValueType() != IntegerValueType || !(token.numericValue() == 1 || !token.numericValue()))
        return false;

    return mediaFeature == gridMediaFeature;
}

static inline bool featureWithAspectRatio(const String& mediaFeature)
{
    return mediaFeature == aspectRatioMediaFeature
        || mediaFeature == deviceAspectRatioMediaFeature
        || mediaFeature == minAspectRatioMediaFeature
        || mediaFeature == maxAspectRatioMediaFeature
        || mediaFeature == minDeviceAspectRatioMediaFeature
        || mediaFeature == maxDeviceAspectRatioMediaFeature;
}

static inline bool featureWithoutValue(const String& mediaFeature)
{
    // Media features that are prefixed by min/max cannot be used without a value.
    return mediaFeature == monochromeMediaFeature
        || mediaFeature == colorMediaFeature
        || mediaFeature == colorIndexMediaFeature
        || mediaFeature == gridMediaFeature
        || mediaFeature == heightMediaFeature
        || mediaFeature == widthMediaFeature
        || mediaFeature == deviceHeightMediaFeature
        || mediaFeature == deviceWidthMediaFeature
        || mediaFeature == orientationMediaFeature
        || mediaFeature == aspectRatioMediaFeature
        || mediaFeature == deviceAspectRatioMediaFeature
        || mediaFeature == hoverMediaFeature
        || mediaFeature == anyHoverMediaFeature
        || mediaFeature == transform3dMediaFeature
        || mediaFeature == pointerMediaFeature
        || mediaFeature == anyPointerMediaFeature
        || mediaFeature == devicePixelRatioMediaFeature
        || mediaFeature == resolutionMediaFeature
        || mediaFeature == scanMediaFeature;
}

bool MediaQueryExp::isViewportDependent() const
{
    return m_mediaFeature == widthMediaFeature
        || m_mediaFeature == heightMediaFeature
        || m_mediaFeature == minWidthMediaFeature
        || m_mediaFeature == minHeightMediaFeature
        || m_mediaFeature == maxWidthMediaFeature
        || m_mediaFeature == maxHeightMediaFeature
        || m_mediaFeature == orientationMediaFeature
        || m_mediaFeature == aspectRatioMediaFeature
        || m_mediaFeature == minAspectRatioMediaFeature
        || m_mediaFeature == devicePixelRatioMediaFeature
        || m_mediaFeature == resolutionMediaFeature
        || m_mediaFeature == maxAspectRatioMediaFeature;
}

MediaQueryExp::MediaQueryExp(const MediaQueryExp& other)
    : m_mediaFeature(other.mediaFeature())
    , m_expValue(other.expValue())
{
}

MediaQueryExp::MediaQueryExp(const String& mediaFeature, const MediaQueryExpValue& expValue)
    : m_mediaFeature(mediaFeature)
    , m_expValue(expValue)
{
}

CSSValueID cssValueKeywordID(const CSSParserString&);

PassOwnPtrWillBeRawPtr<MediaQueryExp> MediaQueryExp::createIfValid(const String& mediaFeature, const Vector<CSSParserToken, 4>& tokenList)
{
    ASSERT(!mediaFeature.isNull());

    MediaQueryExpValue expValue;
    String lowerMediaFeature = attemptStaticStringCreation(mediaFeature.lower());

    // Create value for media query expression that must have 1 or more values.
    if (tokenList.size() == 0 && featureWithoutValue(lowerMediaFeature)) {
        // Valid, creates a MediaQueryExp with an 'invalid' MediaQueryExpValue
    } else if (tokenList.size() == 1) {
        CSSParserToken token = tokenList.first();

        if (token.type() == IdentToken) {
            CSSValueID ident = cssValueKeywordID(token.value());
            if (!featureWithValidIdent(lowerMediaFeature, ident))
                return nullptr;
            expValue.id = ident;
            expValue.unit = CSSPrimitiveValue::CSS_VALUE_ID;
            expValue.isID = true;
        } else if (token.type() == NumberToken || token.type() == PercentageToken || token.type() == DimensionToken) {
            // Check for numeric token types since it is only safe for these types to call numericValue.
            if (featureWithValidDensity(lowerMediaFeature, token)
                || featureWithValidPositiveLength(lowerMediaFeature, token)) {
                // Media features that must have non-negative <density>, ie. dppx, dpi or dpcm,
                // or Media features that must have non-negative <length> or number value.
                expValue.value = token.numericValue();
                expValue.unit = token.unitType();
                expValue.isValue = true;
            } else if (featureWithPositiveInteger(lowerMediaFeature, token)
                || featureWithPositiveNumber(lowerMediaFeature, token)
                || featureWithZeroOrOne(lowerMediaFeature, token)) {
                // Media features that must have non-negative integer value,
                // or media features that must have non-negative number value,
                // or media features that must have (0|1) value.
                expValue.value = token.numericValue();
                expValue.unit = CSSPrimitiveValue::CSS_NUMBER;
                expValue.isValue = true;
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    } else if (tokenList.size() == 3 && featureWithAspectRatio(lowerMediaFeature)) {
        // TODO(timloh): <ratio> is supposed to allow whitespace around the '/'
        // Applicable to device-aspect-ratio and aspect-ratio.
        const CSSParserToken& numerator = tokenList[0];
        const CSSParserToken& delimiter = tokenList[1];
        const CSSParserToken& denominator = tokenList[2];
        if (delimiter.type() != DelimiterToken || delimiter.delimiter() != '/')
            return nullptr;
        if (numerator.type() != NumberToken || numerator.numericValue() <= 0 || numerator.numericValueType() != IntegerValueType)
            return nullptr;
        if (denominator.type() != NumberToken || denominator.numericValue() <= 0 || denominator.numericValueType() != IntegerValueType)
            return nullptr;

        expValue.numerator = (unsigned)numerator.numericValue();
        expValue.denominator = (unsigned)denominator.numericValue();
        expValue.isRatio = true;
    } else {
        return nullptr;
    }

    return adoptPtrWillBeNoop(new MediaQueryExp(lowerMediaFeature, expValue));
}

MediaQueryExp::~MediaQueryExp()
{
}

bool MediaQueryExp::operator==(const MediaQueryExp& other) const
{
    return (other.m_mediaFeature == m_mediaFeature)
        && ((!other.m_expValue.isValid() && !m_expValue.isValid())
            || (other.m_expValue.isValid() && m_expValue.isValid() && other.m_expValue.equals(m_expValue)));
}

String MediaQueryExp::serialize() const
{
    StringBuilder result;
    result.append('(');
    result.append(m_mediaFeature.lower());
    if (m_expValue.isValid()) {
        result.appendLiteral(": ");
        result.append(m_expValue.cssText());
    }
    result.append(')');

    return result.toString();
}

static inline String printNumber(double number)
{
    return Decimal::fromDouble(number).toString();
}

String MediaQueryExpValue::cssText() const
{
    StringBuilder output;
    if (isValue) {
        output.append(printNumber(value));
        output.append(CSSPrimitiveValue::unitTypeToString(unit));
    } else if (isRatio) {
        output.append(printNumber(numerator));
        output.append('/');
        output.append(printNumber(denominator));
    } else if (isID) {
        output.append(getValueName(id));
    }

    return output.toString();
}

} // namespace
