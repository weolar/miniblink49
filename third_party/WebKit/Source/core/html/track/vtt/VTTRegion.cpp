/*
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/track/vtt/VTTRegion.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ClientRect.h"
#include "core/dom/DOMTokenList.h"
#include "core/dom/ElementTraversal.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/track/vtt/VTTParser.h"
#include "core/html/track/vtt/VTTScanner.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutObject.h"
#include "platform/Logging.h"
#include "wtf/MathExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

// The following values default values are defined within the WebVTT Regions Spec.
// https://dvcs.w3.org/hg/text-tracks/raw-file/default/608toVTT/region.html

// The region occupies by default 100% of the width of the video viewport.
static const float defaultWidth = 100;

// The region has, by default, 3 lines of text.
static const long defaultHeightInLines = 3;

// The region and viewport are anchored in the bottom left corner.
static const float defaultAnchorPointX = 0;
static const float defaultAnchorPointY = 100;

// The region doesn't have scrolling text, by default.
static const bool defaultScroll = false;

// Default region line-height (vh units)
static const float lineHeight = 5.33;

// Default scrolling animation time period (s).
static const float scrollTime = 0.433;

static bool isNonPercentage(double value, const char* method, ExceptionState& exceptionState)
{
    if (value < 0 || value > 100) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexOutsideRange("value", value, 0.0, ExceptionMessages::InclusiveBound, 100.0, ExceptionMessages::InclusiveBound));
        return true;
    }
    return false;
}

VTTRegion::VTTRegion()
    : m_id(emptyString())
    , m_width(defaultWidth)
    , m_heightInLines(defaultHeightInLines)
    , m_regionAnchor(FloatPoint(defaultAnchorPointX, defaultAnchorPointY))
    , m_viewportAnchor(FloatPoint(defaultAnchorPointX, defaultAnchorPointY))
    , m_scroll(defaultScroll)
    , m_track(nullptr)
    , m_currentTop(0)
    , m_scrollTimer(this, &VTTRegion::scrollTimerFired)
{
}

VTTRegion::~VTTRegion()
{
}

void VTTRegion::setTrack(TextTrack* track)
{
    m_track = track;
}

void VTTRegion::setId(const String& id)
{
    m_id = id;
}

void VTTRegion::setWidth(double value, ExceptionState& exceptionState)
{
    if (isNonPercentage(value, "width", exceptionState))
        return;

    m_width = value;
}

void VTTRegion::setHeight(long value, ExceptionState& exceptionState)
{
    if (value < 0) {
        exceptionState.throwDOMException(IndexSizeError, "The height provided (" + String::number(value) + ") is negative.");
        return;
    }

    m_heightInLines = value;
}

void VTTRegion::setRegionAnchorX(double value, ExceptionState& exceptionState)
{
    if (isNonPercentage(value, "regionAnchorX", exceptionState))
        return;

    m_regionAnchor.setX(value);
}

void VTTRegion::setRegionAnchorY(double value, ExceptionState& exceptionState)
{
    if (isNonPercentage(value, "regionAnchorY", exceptionState))
        return;

    m_regionAnchor.setY(value);
}

void VTTRegion::setViewportAnchorX(double value, ExceptionState& exceptionState)
{
    if (isNonPercentage(value, "viewportAnchorX", exceptionState))
        return;

    m_viewportAnchor.setX(value);
}

void VTTRegion::setViewportAnchorY(double value, ExceptionState& exceptionState)
{
    if (isNonPercentage(value, "viewportAnchorY", exceptionState))
        return;

    m_viewportAnchor.setY(value);
}

const AtomicString VTTRegion::scroll() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, upScrollValueKeyword, ("up", AtomicString::ConstructFromLiteral));

    if (m_scroll)
        return upScrollValueKeyword;

    return "";
}

void VTTRegion::setScroll(const AtomicString& value, ExceptionState& exceptionState)
{
    DEFINE_STATIC_LOCAL(const AtomicString, upScrollValueKeyword, ("up", AtomicString::ConstructFromLiteral));

    if (value != emptyString() && value != upScrollValueKeyword) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + value + "') is invalid. The 'scroll' property must be either the empty string, or 'up'.");
        return;
    }

    m_scroll = value == upScrollValueKeyword;
}

void VTTRegion::updateParametersFromRegion(VTTRegion* region)
{
    m_heightInLines = region->height();
    m_width = region->width();

    m_regionAnchor = FloatPoint(region->regionAnchorX(), region->regionAnchorY());
    m_viewportAnchor = FloatPoint(region->viewportAnchorX(), region->viewportAnchorY());

    setScroll(region->scroll(), ASSERT_NO_EXCEPTION);
}

void VTTRegion::setRegionSettings(const String& inputString)
{
    m_settings = inputString;

    VTTScanner input(inputString);

    while (!input.isAtEnd()) {
        input.skipWhile<VTTParser::isValidSettingDelimiter>();

        if (input.isAtEnd())
            break;

        // Scan the name part.
        RegionSetting name = scanSettingName(input);

        // Verify that we're looking at a '='.
        if (name == None || !input.scan('=')) {
            input.skipUntil<VTTParser::isASpace>();
            continue;
        }

        // Scan the value part.
        parseSettingValue(name, input);
    }
}

VTTRegion::RegionSetting VTTRegion::scanSettingName(VTTScanner& input)
{
    if (input.scan("id"))
        return Id;
    if (input.scan("height"))
        return Height;
    if (input.scan("width"))
        return Width;
    if (input.scan("viewportanchor"))
        return ViewportAnchor;
    if (input.scan("regionanchor"))
        return RegionAnchor;
    if (input.scan("scroll"))
        return Scroll;

    return None;
}

static inline bool parsedEntireRun(const VTTScanner& input, const VTTScanner::Run& run)
{
    return input.isAt(run.end());
}

void VTTRegion::parseSettingValue(RegionSetting setting, VTTScanner& input)
{
    DEFINE_STATIC_LOCAL(const AtomicString, scrollUpValueKeyword, ("up", AtomicString::ConstructFromLiteral));

    VTTScanner::Run valueRun = input.collectUntil<VTTParser::isASpace>();

    switch (setting) {
    case Id: {
        String stringValue = input.extractString(valueRun);
        if (stringValue.find("-->") == kNotFound)
            m_id = stringValue;
        break;
    }
    case Width: {
        float floatWidth;
        if (VTTParser::parseFloatPercentageValue(input, floatWidth) && parsedEntireRun(input, valueRun))
            m_width = floatWidth;
        else
            WTF_LOG(Media, "VTTRegion::parseSettingValue, invalid Width");
        break;
    }
    case Height: {
        int number;
        if (input.scanDigits(number) && parsedEntireRun(input, valueRun))
            m_heightInLines = number;
        else
            WTF_LOG(Media, "VTTRegion::parseSettingValue, invalid Height");
        break;
    }
    case RegionAnchor: {
        FloatPoint anchor;
        if (VTTParser::parseFloatPercentageValuePair(input, ',', anchor) && parsedEntireRun(input, valueRun))
            m_regionAnchor = anchor;
        else
            WTF_LOG(Media, "VTTRegion::parseSettingValue, invalid RegionAnchor");
        break;
    }
    case ViewportAnchor: {
        FloatPoint anchor;
        if (VTTParser::parseFloatPercentageValuePair(input, ',', anchor) && parsedEntireRun(input, valueRun))
            m_viewportAnchor = anchor;
        else
            WTF_LOG(Media, "VTTRegion::parseSettingValue, invalid ViewportAnchor");
        break;
    }
    case Scroll:
        if (input.scanRun(valueRun, scrollUpValueKeyword))
            m_scroll = true;
        else
            WTF_LOG(Media, "VTTRegion::parseSettingValue, invalid Scroll");
        break;
    case None:
        break;
    }

    input.skipRun(valueRun);
}

const AtomicString& VTTRegion::textTrackCueContainerShadowPseudoId()
{
    DEFINE_STATIC_LOCAL(const AtomicString, trackRegionCueContainerPseudoId,
        ("-webkit-media-text-track-region-container", AtomicString::ConstructFromLiteral));

    return trackRegionCueContainerPseudoId;
}

const AtomicString& VTTRegion::textTrackCueContainerScrollingClass()
{
    DEFINE_STATIC_LOCAL(const AtomicString, trackRegionCueContainerScrollingClass,
        ("scrolling", AtomicString::ConstructFromLiteral));

    return trackRegionCueContainerScrollingClass;
}

const AtomicString& VTTRegion::textTrackRegionShadowPseudoId()
{
    DEFINE_STATIC_LOCAL(const AtomicString, trackRegionShadowPseudoId,
        ("-webkit-media-text-track-region", AtomicString::ConstructFromLiteral));

    return trackRegionShadowPseudoId;
}

PassRefPtrWillBeRawPtr<HTMLDivElement> VTTRegion::getDisplayTree(Document& document)
{
    if (!m_regionDisplayTree) {
        m_regionDisplayTree = HTMLDivElement::create(document);
        prepareRegionDisplayTree();
    }

    return m_regionDisplayTree;
}

void VTTRegion::willRemoveVTTCueBox(VTTCueBox* box)
{
    WTF_LOG(Media, "VTTRegion::willRemoveVTTCueBox");
    ASSERT(m_cueContainer->contains(box));

    double boxHeight = box->getBoundingClientRect()->bottom() - box->getBoundingClientRect()->top();

    m_cueContainer->classList().remove(textTrackCueContainerScrollingClass(), ASSERT_NO_EXCEPTION);

    m_currentTop += boxHeight;
    m_cueContainer->setInlineStyleProperty(CSSPropertyTop, m_currentTop, CSSPrimitiveValue::CSS_PX);
}

void VTTRegion::appendVTTCueBox(PassRefPtrWillBeRawPtr<VTTCueBox> displayBox)
{
    ASSERT(m_cueContainer);

    if (m_cueContainer->contains(displayBox.get()))
        return;

    m_cueContainer->appendChild(displayBox);
    displayLastVTTCueBox();
}

void VTTRegion::displayLastVTTCueBox()
{
    WTF_LOG(Media, "VTTRegion::displayLastVTTCueBox");
    ASSERT(m_cueContainer);

    // FIXME: This should not be causing recalc styles in a loop to set the "top" css
    // property to move elements. We should just scroll the text track cues on the
    // compositor with an animation.

    if (m_scrollTimer.isActive())
        return;

    // If it's a scrolling region, add the scrolling class.
    if (isScrollingRegion())
        m_cueContainer->classList().add(textTrackCueContainerScrollingClass(), ASSERT_NO_EXCEPTION);

    float regionBottom = m_regionDisplayTree->getBoundingClientRect()->bottom();

    // Find first cue that is not entirely displayed and scroll it upwards.
    for (Element* child = ElementTraversal::firstChild(*m_cueContainer); child && !m_scrollTimer.isActive(); child = ElementTraversal::nextSibling(*child)) {
        ClientRect* clientRect = child->getBoundingClientRect();
        float childTop = clientRect->top();
        float childBottom = clientRect->bottom();

        if (regionBottom >= childBottom)
            continue;

        float height = childBottom - childTop;

        m_currentTop -= std::min(height, childBottom - regionBottom);
        m_cueContainer->setInlineStyleProperty(CSSPropertyTop, m_currentTop, CSSPrimitiveValue::CSS_PX);

        startTimer();
    }
}

void VTTRegion::prepareRegionDisplayTree()
{
    ASSERT(m_regionDisplayTree);

    // 7.2 Prepare region CSS boxes

    // FIXME: Change the code below to use viewport units when
    // http://crbug/244618 is fixed.

    // Let regionWidth be the text track region width.
    // Let width be 'regionWidth vw' ('vw' is a CSS unit)
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyWidth,
        m_width, CSSPrimitiveValue::CSS_PERCENTAGE);

    // Let lineHeight be '0.0533vh' ('vh' is a CSS unit) and regionHeight be
    // the text track region height. Let height be 'lineHeight' multiplied
    // by regionHeight.
    double height = lineHeight * m_heightInLines;
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyHeight,
        height, CSSPrimitiveValue::CSS_VH);

    // Let viewportAnchorX be the x dimension of the text track region viewport
    // anchor and regionAnchorX be the x dimension of the text track region
    // anchor. Let leftOffset be regionAnchorX multiplied by width divided by
    // 100.0. Let left be leftOffset subtracted from 'viewportAnchorX vw'.
    double leftOffset = m_regionAnchor.x() * m_width / 100;
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyLeft,
        m_viewportAnchor.x() - leftOffset,
        CSSPrimitiveValue::CSS_PERCENTAGE);

    // Let viewportAnchorY be the y dimension of the text track region viewport
    // anchor and regionAnchorY be the y dimension of the text track region
    // anchor. Let topOffset be regionAnchorY multiplied by height divided by
    // 100.0. Let top be topOffset subtracted from 'viewportAnchorY vh'.
    double topOffset = m_regionAnchor.y() * height / 100;
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyTop,
        m_viewportAnchor.y() - topOffset,
        CSSPrimitiveValue::CSS_PERCENTAGE);

    // The cue container is used to wrap the cues and it is the object which is
    // gradually scrolled out as multiple cues are appended to the region.
    m_cueContainer = HTMLDivElement::create(m_regionDisplayTree->document());
    m_cueContainer->setInlineStyleProperty(CSSPropertyTop,
        0.0,
        CSSPrimitiveValue::CSS_PX);

    m_cueContainer->setShadowPseudoId(textTrackCueContainerShadowPseudoId());
    m_regionDisplayTree->appendChild(m_cueContainer);

    // 7.5 Every WebVTT region object is initialised with the following CSS
    m_regionDisplayTree->setShadowPseudoId(textTrackRegionShadowPseudoId());
}

void VTTRegion::startTimer()
{
    WTF_LOG(Media, "VTTRegion::startTimer");

    if (m_scrollTimer.isActive())
        return;

    double duration = isScrollingRegion() ? scrollTime : 0;
    m_scrollTimer.startOneShot(duration, FROM_HERE);
}

void VTTRegion::stopTimer()
{
    WTF_LOG(Media, "VTTRegion::stopTimer");

    if (m_scrollTimer.isActive())
        m_scrollTimer.stop();
}

void VTTRegion::scrollTimerFired(Timer<VTTRegion>*)
{
    WTF_LOG(Media, "VTTRegion::scrollTimerFired");

    stopTimer();
    displayLastVTTCueBox();
}

DEFINE_TRACE(VTTRegion)
{
    visitor->trace(m_cueContainer);
    visitor->trace(m_regionDisplayTree);
    visitor->trace(m_track);
}

} // namespace blink
