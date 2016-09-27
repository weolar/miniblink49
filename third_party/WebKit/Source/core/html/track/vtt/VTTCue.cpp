/*
 * Copyright (c) 2013, Opera Software ASA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Opera Software ASA nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/track/vtt/VTTCue.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/NodeTraversal.h"
#include "core/events/Event.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/track/TextTrack.h"
#include "core/html/track/TextTrackCueList.h"
#include "core/html/track/vtt/VTTElement.h"
#include "core/html/track/vtt/VTTParser.h"
#include "core/html/track/vtt/VTTRegionList.h"
#include "core/html/track/vtt/VTTScanner.h"
#include "core/layout/LayoutVTTCue.h"
#include "platform/FloatConversion.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/text/BidiResolver.h"
#include "platform/text/TextRunIterator.h"
#include "wtf/MathExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

static const CSSValueID displayWritingModeMap[] = {
    CSSValueHorizontalTb, CSSValueVerticalRl, CSSValueVerticalLr
};
static_assert(WTF_ARRAY_LENGTH(displayWritingModeMap) == VTTCue::NumberOfWritingDirections,
    "displayWritingModeMap should have the same number of elements as VTTCue::NumberOfWritingDirections");

static const CSSValueID displayAlignmentMap[] = {
    CSSValueStart, CSSValueCenter, CSSValueEnd, CSSValueLeft, CSSValueRight
};
static_assert(WTF_ARRAY_LENGTH(displayAlignmentMap) == VTTCue::NumberOfAlignments,
    "displayAlignmentMap should have the same number of elements as VTTCue::NumberOfAlignments");

static const String& autoKeyword()
{
    DEFINE_STATIC_LOCAL(const String, autoString, ("auto"));
    return autoString;
}

static const String& startKeyword()
{
    DEFINE_STATIC_LOCAL(const String, start, ("start"));
    return start;
}

static const String& middleKeyword()
{
    DEFINE_STATIC_LOCAL(const String, middle, ("middle"));
    return middle;
}

static const String& endKeyword()
{
    DEFINE_STATIC_LOCAL(const String, end, ("end"));
    return end;
}

static const String& leftKeyword()
{
    DEFINE_STATIC_LOCAL(const String, left, ("left"));
    return left;
}

static const String& rightKeyword()
{
    DEFINE_STATIC_LOCAL(const String, right, ("right"));
    return right;
}

static const String& horizontalKeyword()
{
    return emptyString();
}

static const String& verticalGrowingLeftKeyword()
{
    DEFINE_STATIC_LOCAL(const String, verticalrl, ("rl"));
    return verticalrl;
}

static const String& verticalGrowingRightKeyword()
{
    DEFINE_STATIC_LOCAL(const String, verticallr, ("lr"));
    return verticallr;
}

static bool isInvalidPercentage(double value)
{
    ASSERT(std::isfinite(value));
    return value < 0 || value > 100;
}

static bool isInvalidPercentage(double value, ExceptionState& exceptionState)
{
    if (isInvalidPercentage(value)) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexOutsideRange<double>("value", value, 0, ExceptionMessages::InclusiveBound, 100, ExceptionMessages::InclusiveBound));
        return true;
    }
    return false;
}

// Sets inline CSS properties on passed in element if value is not an empty string
static void setInlineStylePropertyIfNotEmpty(Element& element,
    CSSPropertyID propertyID, const String& value)
{
    if (!value.isEmpty())
        element.setInlineStyleProperty(propertyID, value);
}

VTTCueBox::VTTCueBox(Document& document)
    : HTMLDivElement(document)
    , m_snapToLinesPosition(std::numeric_limits<float>::quiet_NaN())
{
    setShadowPseudoId(AtomicString("-webkit-media-text-track-display", AtomicString::ConstructFromLiteral));
}

void VTTCueBox::applyCSSProperties(const VTTDisplayParameters& displayParameters)
{
    // http://dev.w3.org/html5/webvtt/#applying-css-properties-to-webvtt-node-objects

    // Initialize the (root) list of WebVTT Node Objects with the following CSS settings:

    // the 'position' property must be set to 'absolute'
    setInlineStyleProperty(CSSPropertyPosition, CSSValueAbsolute);

    //  the 'unicode-bidi' property must be set to 'plaintext'
    setInlineStyleProperty(CSSPropertyUnicodeBidi, CSSValueWebkitPlaintext);

    // the 'direction' property must be set to direction
    setInlineStyleProperty(CSSPropertyDirection, displayParameters.direction);

    // the 'writing-mode' property must be set to writing-mode
    setInlineStyleProperty(CSSPropertyWebkitWritingMode, displayParameters.writingMode);

    const FloatPoint& position = displayParameters.position;

    // the 'top' property must be set to top,
    setInlineStyleProperty(CSSPropertyTop, position.y(), CSSPrimitiveValue::CSS_PERCENTAGE);

    // the 'left' property must be set to left
    setInlineStyleProperty(CSSPropertyLeft, position.x(), CSSPrimitiveValue::CSS_PERCENTAGE);

    // the 'width' property must be set to width, and the 'height' property  must be set to height
    if (displayParameters.writingMode == CSSValueHorizontalTb) {
        setInlineStyleProperty(CSSPropertyWidth, displayParameters.size, CSSPrimitiveValue::CSS_PERCENTAGE);
        setInlineStyleProperty(CSSPropertyHeight, CSSValueAuto);
    } else {
        setInlineStyleProperty(CSSPropertyWidth, CSSValueAuto);
        setInlineStyleProperty(CSSPropertyHeight, displayParameters.size,  CSSPrimitiveValue::CSS_PERCENTAGE);
    }

    // The 'text-align' property on the (root) List of WebVTT Node Objects must
    // be set to the value in the second cell of the row of the table below
    // whose first cell is the value of the corresponding cue's text track cue
    // alignment:
    setInlineStyleProperty(CSSPropertyTextAlign, displayParameters.textAlign);

    // TODO(philipj): The position adjustment for non-snap-to-lines cues has
    // been removed from the spec:
    // https://www.w3.org/Bugs/Public/show_bug.cgi?id=19178
    if (std::isnan(displayParameters.snapToLinesPosition)) {
        // 10.13.1 Set up x and y:
        // Note: x and y are set through the CSS left and top above.

        // 10.13.2 Position the boxes in boxes such that the point x% along the
        // width of the bounding box of the boxes in boxes is x% of the way
        // across the width of the video's rendering area, and the point y%
        // along the height of the bounding box of the boxes in boxes is y%
        // of the way across the height of the video's rendering area, while
        // maintaining the relative positions of the boxes in boxes to each
        // other.
        setInlineStyleProperty(CSSPropertyTransform,
            String::format("translate(-%.2f%%, -%.2f%%)", position.x(), position.y()));

        setInlineStyleProperty(CSSPropertyWhiteSpace, CSSValuePre);
    }

    // The snap-to-lines position is propagated to LayoutVTTCue.
    m_snapToLinesPosition = displayParameters.snapToLinesPosition;
}

LayoutObject* VTTCueBox::createLayoutObject(const ComputedStyle& style)
{
    // If WebVTT Regions are used, the regular WebVTT layout algorithm is no
    // longer necessary, since cues having the region parameter set do not have
    // any positioning parameters. Also, in this case, the regions themselves
    // have positioning information.
    if (style.position() == RelativePosition)
        return HTMLDivElement::createLayoutObject(style);

    return new LayoutVTTCue(this, m_snapToLinesPosition);
}

VTTCue::VTTCue(Document& document, double startTime, double endTime, const String& text)
    : TextTrackCue(startTime, endTime)
    , m_text(text)
    , m_linePosition(std::numeric_limits<float>::quiet_NaN())
    , m_textPosition(std::numeric_limits<float>::quiet_NaN())
    , m_cueSize(100)
    , m_writingDirection(Horizontal)
    , m_cueAlignment(Middle)
    , m_vttNodeTree(nullptr)
    , m_cueBackgroundBox(HTMLDivElement::create(document))
    , m_snapToLines(true)
    , m_displayTreeShouldChange(true)
{
    UseCounter::count(document, UseCounter::VTTCue);
    m_cueBackgroundBox->setShadowPseudoId(cueShadowPseudoId());
}

VTTCue::~VTTCue()
{
}

#ifndef NDEBUG
String VTTCue::toString() const
{
    return String::format("%p id=%s interval=%f-->%f cue=%s)", this, id().utf8().data(), startTime(), endTime(), text().utf8().data());
}
#endif

void VTTCue::cueDidChange()
{
    TextTrackCue::cueDidChange();
    m_displayTreeShouldChange = true;
}

const String& VTTCue::vertical() const
{
    switch (m_writingDirection) {
    case Horizontal:
        return horizontalKeyword();
    case VerticalGrowingLeft:
        return verticalGrowingLeftKeyword();
    case VerticalGrowingRight:
        return verticalGrowingRightKeyword();
    default:
        ASSERT_NOT_REACHED();
        return emptyString();
    }
}

void VTTCue::setVertical(const String& value)
{
    WritingDirection direction = m_writingDirection;
    if (value == horizontalKeyword())
        direction = Horizontal;
    else if (value == verticalGrowingLeftKeyword())
        direction = VerticalGrowingLeft;
    else if (value == verticalGrowingRightKeyword())
        direction = VerticalGrowingRight;
    else
        ASSERT_NOT_REACHED();

    if (direction == m_writingDirection)
        return;

    cueWillChange();
    m_writingDirection = direction;
    cueDidChange();
}

void VTTCue::setSnapToLines(bool value)
{
    if (m_snapToLines == value)
        return;

    cueWillChange();
    m_snapToLines = value;
    cueDidChange();
}

bool VTTCue::lineIsAuto() const
{
    return std::isnan(m_linePosition);
}

void VTTCue::line(DoubleOrAutoKeyword& result) const
{
    if (lineIsAuto())
        result.setAutoKeyword(autoKeyword());
    else
        result.setDouble(m_linePosition);
}

void VTTCue::setLine(const DoubleOrAutoKeyword& position)
{
    // http://dev.w3.org/html5/webvtt/#dfn-vttcue-line
    // On setting, the text track cue line position must be set to the new
    // value; if the new value is the string "auto", then it must be
    // interpreted as the special value auto.
    // ("auto" is translated to NaN.)
    float floatPosition;
    if (position.isAutoKeyword()) {
        if (lineIsAuto())
            return;
        floatPosition = std::numeric_limits<float>::quiet_NaN();
    } else {
        ASSERT(position.isDouble());
        floatPosition = narrowPrecisionToFloat(position.getAsDouble());
        if (m_linePosition == floatPosition)
            return;
    }

    cueWillChange();
    m_linePosition = floatPosition;
    cueDidChange();
}

bool VTTCue::textPositionIsAuto() const
{
    return std::isnan(m_textPosition);
}

void VTTCue::position(DoubleOrAutoKeyword& result) const
{
    if (textPositionIsAuto())
        result.setAutoKeyword(autoKeyword());
    else
        result.setDouble(m_textPosition);
}

void VTTCue::setPosition(const DoubleOrAutoKeyword& position, ExceptionState& exceptionState)
{
    // http://dev.w3.org/html5/webvtt/#dfn-vttcue-position
    // On setting, if the new value is negative or greater than 100, then an
    // IndexSizeError exception must be thrown. Otherwise, the text track cue
    // text position must be set to the new value; if the new value is the
    // string "auto", then it must be interpreted as the special value auto.
    float floatPosition;
    if (position.isAutoKeyword()) {
        if (textPositionIsAuto())
            return;
        floatPosition = std::numeric_limits<float>::quiet_NaN();
    } else {
        ASSERT(position.isDouble());
        if (isInvalidPercentage(position.getAsDouble(), exceptionState))
            return;
        floatPosition = narrowPrecisionToFloat(position.getAsDouble());
        if (m_textPosition == floatPosition)
            return;
    }

    cueWillChange();
    m_textPosition = floatPosition;
    cueDidChange();
}

void VTTCue::setSize(double size, ExceptionState& exceptionState)
{
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/the-video-element.html#dom-texttrackcue-size
    // On setting, if the new value is negative or greater than 100, then throw an IndexSizeError
    // exception. Otherwise, set the text track cue size to the new value.
    if (isInvalidPercentage(size, exceptionState))
        return;

    // Otherwise, set the text track cue line position to the new value.
    float floatSize = narrowPrecisionToFloat(size);
    if (m_cueSize == floatSize)
        return;

    cueWillChange();
    m_cueSize = floatSize;
    cueDidChange();
}

const String& VTTCue::align() const
{
    switch (m_cueAlignment) {
    case Start:
        return startKeyword();
    case Middle:
        return middleKeyword();
    case End:
        return endKeyword();
    case Left:
        return leftKeyword();
    case Right:
        return rightKeyword();
    default:
        ASSERT_NOT_REACHED();
        return emptyString();
    }
}

void VTTCue::setAlign(const String& value)
{
    CueAlignment alignment = m_cueAlignment;
    if (value == startKeyword())
        alignment = Start;
    else if (value == middleKeyword())
        alignment = Middle;
    else if (value == endKeyword())
        alignment = End;
    else if (value == leftKeyword())
        alignment = Left;
    else if (value == rightKeyword())
        alignment = Right;
    else
        ASSERT_NOT_REACHED();

    if (alignment == m_cueAlignment)
        return;

    cueWillChange();
    m_cueAlignment = alignment;
    cueDidChange();
}

void VTTCue::setText(const String& text)
{
    if (m_text == text)
        return;

    cueWillChange();
    // Clear the document fragment but don't bother to create it again just yet as we can do that
    // when it is requested.
    m_vttNodeTree = nullptr;
    m_text = text;
    cueDidChange();
}

void VTTCue::createVTTNodeTree()
{
    if (!m_vttNodeTree)
        m_vttNodeTree = VTTParser::createDocumentFragmentFromCueText(document(), m_text);
}

void VTTCue::copyVTTNodeToDOMTree(ContainerNode* vttNode, ContainerNode* parent)
{
    for (Node* node = vttNode->firstChild(); node; node = node->nextSibling()) {
        RefPtrWillBeRawPtr<Node> clonedNode;
        if (node->isVTTElement())
            clonedNode = toVTTElement(node)->createEquivalentHTMLElement(document());
        else
            clonedNode = node->cloneNode(false);
        parent->appendChild(clonedNode);
        if (node->isContainerNode())
            copyVTTNodeToDOMTree(toContainerNode(node), toContainerNode(clonedNode));
    }
}

PassRefPtrWillBeRawPtr<DocumentFragment> VTTCue::getCueAsHTML()
{
    createVTTNodeTree();
    RefPtrWillBeRawPtr<DocumentFragment> clonedFragment = DocumentFragment::create(document());
    copyVTTNodeToDOMTree(m_vttNodeTree.get(), clonedFragment.get());
    return clonedFragment.release();
}

void VTTCue::setRegionId(const String& regionId)
{
    if (m_regionId == regionId)
        return;

    cueWillChange();
    m_regionId = regionId;
    cueDidChange();
}

float VTTCue::calculateComputedLinePosition() const
{
    // http://dev.w3.org/html5/webvtt/#dfn-text-track-cue-computed-line-position
    // A text track cue has a text track cue computed line position whose value
    // is that returned by the following algorithm, which is defined in terms
    // of the other aspects of the cue:

    // 1. If the text track cue line position is numeric, the text track cue
    //    snap-to-lines flag of the text track cue is not set, and the text
    //    track cue line position is negative or greater than 100, then return
    //    100 and abort these steps.
    if (!lineIsAuto() && !m_snapToLines && isInvalidPercentage(m_linePosition))
        return 100;

    // 2. If the text track cue line position is numeric, return the value of
    //    the text track cue line position and abort these steps. (Either the
    //    text track cue snap-to-lines flag is set, so any value, not just
    //    those in the range 0..100, is valid, or the value is in the range
    //    0..100 and is thus valid regardless of the value of that flag.)
    if (!lineIsAuto())
        return m_linePosition;

    // 3. If the text track cue snap-to-lines flag of the text track cue is not
    //    set, return the value 100 and abort these steps. (The text track cue
    //    line position is the special value auto.)
    if (!m_snapToLines)
        return 100;

    // 4. Let cue be the text track cue.
    // 5. If cue is not in a list of cues of a text track, or if that text
    //    track is not in the list of text tracks of a media element, return -1
    //    and abort these steps.
    if (!track())
        return -1;

    // 6. Let track be the text track whose list of cues the cue is in.
    // 7. Let n be the number of text tracks whose text track mode is showing
    //    and that are in the media element's list of text tracks before track.
    int n = track()->trackIndexRelativeToRenderedTracks();

    // 8. Increment n by one. / 9. Negate n. / 10. Return n.
    n++;
    n = -n;
    return n;
}

class VTTTextRunIterator : public TextRunIterator {
public:
    VTTTextRunIterator() { }
    VTTTextRunIterator(const TextRun* textRun, unsigned offset) : TextRunIterator(textRun, offset) { }

    bool atParagraphSeparator() const
    {
        // Within a cue, paragraph boundaries are only denoted by Type B characters,
        // such as U+000A LINE FEED (LF), U+0085 NEXT LINE (NEL), and U+2029 PARAGRAPH SEPARATOR.
        return WTF::Unicode::category(current()) & WTF::Unicode::Separator_Paragraph;
    }
};

// Almost the same as determineDirectionality in core/html/HTMLElement.cpp, but
// that one uses a "plain" TextRunIterator (which only checks for '\n').
static TextDirection determineDirectionality(const String& value, bool& hasStrongDirectionality)
{
    TextRun run(value);
    BidiResolver<VTTTextRunIterator, BidiCharacterRun> bidiResolver;
    bidiResolver.setStatus(BidiStatus(LTR, false));
    bidiResolver.setPositionIgnoringNestedIsolates(VTTTextRunIterator(&run, 0));
    return bidiResolver.determineDirectionality(&hasStrongDirectionality);
}

static CSSValueID determineTextDirection(DocumentFragment* vttRoot)
{
    ASSERT(vttRoot);

    // Apply the Unicode Bidirectional Algorithm's Paragraph Level steps to the
    // concatenation of the values of each WebVTT Text Object in nodes, in a
    // pre-order, depth-first traversal, excluding WebVTT Ruby Text Objects and
    // their descendants.
    TextDirection textDirection = LTR;
    Node* node = NodeTraversal::next(*vttRoot);
    while (node) {
        ASSERT(node->isDescendantOf(vttRoot));

        if (node->isTextNode()) {
            bool hasStrongDirectionality;
            textDirection = determineDirectionality(node->nodeValue(), hasStrongDirectionality);
            if (hasStrongDirectionality)
                break;
        } else if (node->isVTTElement()) {
            if (toVTTElement(node)->webVTTNodeType() == VTTNodeTypeRubyText) {
                node = NodeTraversal::nextSkippingChildren(*node);
                continue;
            }
        }

        node = NodeTraversal::next(*node);
    }
    return isLeftToRightDirection(textDirection) ? CSSValueLtr : CSSValueRtl;
}

float VTTCue::calculateComputedTextPosition() const
{
    // http://dev.w3.org/html5/webvtt/#dfn-text-track-cue-computed-text-position

    // 1. If the text track cue text position is numeric, then return the value
    // of the text track cue text position and abort these steps. (Otherwise,
    // the text track cue text position is the special value auto.)
    if (!textPositionIsAuto())
        return m_textPosition;

    switch (m_cueAlignment) {
    // 2. If the text track cue text alignment is start or left, return 0 and abort these steps.
    case Start:
    case Left:
        return 0;
    // 3. If the text track cue text alignment is end or right, return 100 and abort these steps.
    case End:
    case Right:
        return 100;
    // 4. If the text track cue text alignment is middle, return 50 and abort these steps.
    case Middle:
        return 50;
    default:
        ASSERT_NOT_REACHED();
        return 0;
    }
}

VTTCue::CueAlignment VTTCue::calculateComputedCueAlignment() const
{
    switch (m_cueAlignment) {
    case VTTCue::Left:
        return VTTCue::Start;
    case VTTCue::Right:
        return VTTCue::End;
    default:
        return m_cueAlignment;
    }
}

VTTDisplayParameters::VTTDisplayParameters()
    : size(std::numeric_limits<float>::quiet_NaN())
    , direction(CSSValueNone)
    , textAlign(CSSValueNone)
    , writingMode(CSSValueNone)
    , snapToLinesPosition(std::numeric_limits<float>::quiet_NaN()) { }

VTTDisplayParameters VTTCue::calculateDisplayParameters() const
{
    // http://dev.w3.org/html5/webvtt/#dfn-apply-webvtt-cue-settings

    VTTDisplayParameters displayParameters;

    // Steps 1 and 2.
    displayParameters.direction = determineTextDirection(m_vttNodeTree.get());

    if (displayParameters.direction == CSSValueRtl)
        UseCounter::count(document(), UseCounter::VTTCueRenderRtl);

    // Note: The 'text-align' property is also determined here so that
    // VTTCueBox::applyCSSProperties need not have access to a VTTCue.
    displayParameters.textAlign = displayAlignmentMap[cueAlignment()];

    // 3. If the text track cue writing direction is horizontal, then let
    // block-flow be 'tb'. Otherwise, if the text track cue writing direction is
    // vertical growing left, then let block-flow be 'lr'. Otherwise, the text
    // track cue writing direction is vertical growing right; let block-flow be
    // 'rl'.
    displayParameters.writingMode = displayWritingModeMap[m_writingDirection];

    // Resolve the cue alignment to one of the values {start, end, middle}.
    CueAlignment computedCueAlignment = calculateComputedCueAlignment();

    // 4. Determine the value of maximum size for cue as per the appropriate
    // rules from the following list:
    float computedTextPosition = calculateComputedTextPosition();
    float maximumSize = computedTextPosition;
    if (computedCueAlignment == Start) {
        maximumSize = 100 - computedTextPosition;
    } else if (computedCueAlignment == End) {
        maximumSize = computedTextPosition;
    } else if (computedCueAlignment == Middle) {
        maximumSize = computedTextPosition <= 50 ? computedTextPosition : (100 - computedTextPosition);
        maximumSize = maximumSize * 2;
    } else {
        ASSERT_NOT_REACHED();
    }

    // 5. If the text track cue size is less than maximum size, then let size
    // be text track cue size. Otherwise, let size be maximum size.
    displayParameters.size = std::min(m_cueSize, maximumSize);

    // 6. If the text track cue writing direction is horizontal, then let width
    // be 'size vw' and height be 'auto'. Otherwise, let width be 'auto' and
    // height be 'size vh'. (These are CSS values used by the next section to
    // set CSS properties for the rendering; 'vw' and 'vh' are CSS units.)
    // (Emulated in VTTCueBox::applyCSSProperties.)

    // 7. Determine the value of x-position or y-position for cue as per the
    // appropriate rules from the following list:
    if (m_writingDirection == Horizontal) {
        switch (computedCueAlignment) {
        case Start:
            displayParameters.position.setX(computedTextPosition);
            break;
        case End:
            displayParameters.position.setX(computedTextPosition - displayParameters.size);
            break;
        case Middle:
            displayParameters.position.setX(computedTextPosition - displayParameters.size / 2);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
    } else {
        // Cases for m_writingDirection being VerticalGrowing{Left|Right}
        switch (computedCueAlignment) {
        case Start:
            displayParameters.position.setY(computedTextPosition);
            break;
        case End:
            displayParameters.position.setY(computedTextPosition - displayParameters.size);
            break;
        case Middle:
            displayParameters.position.setY(computedTextPosition - displayParameters.size / 2);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
    }

    // A text track cue has a text track cue computed line position whose value
    // is defined in terms of the other aspects of the cue.
    float computedLinePosition = calculateComputedLinePosition();

    // 8. Determine the value of whichever of x-position or y-position is not
    // yet calculated for cue as per the appropriate rules from the following
    // list:
    if (!m_snapToLines) {
        if (m_writingDirection == Horizontal)
            displayParameters.position.setY(computedLinePosition);
        else
            displayParameters.position.setX(computedLinePosition);
    } else {
        if (m_writingDirection == Horizontal)
            displayParameters.position.setY(0);
        else
            displayParameters.position.setX(0);
    }

    // Step 9 not implemented (margin == 0).

    // The snap-to-lines position is propagated to LayoutVTTCue.
    displayParameters.snapToLinesPosition = m_snapToLines
        ? computedLinePosition
        : std::numeric_limits<float>::quiet_NaN();

    ASSERT(std::isfinite(displayParameters.size));
    ASSERT(displayParameters.direction != CSSValueNone);
    ASSERT(displayParameters.writingMode != CSSValueNone);
    return displayParameters;
}

void VTTCue::updatePastAndFutureNodes(double movieTime)
{
    DEFINE_STATIC_LOCAL(const String, timestampTag, ("timestamp"));

    ASSERT(isActive());

    // An active cue may still not have a display tree, e.g. if its track is
    // hidden or if the track belongs to an audio element.
    if (!m_displayTree)
        return;

    // FIXME: Per spec it's possible for neither :past nor :future to match, but
    // as implemented here and in SelectorChecker they are simply each others
    // negations. For a cue with no internal timestamps, :past will match but
    // should not per spec. :future is correct, however. See the spec bug to
    // determine what the correct behavior should be:
    // https://www.w3.org/Bugs/Public/show_bug.cgi?id=28237

    bool isPastNode = true;
    double currentTimestamp = startTime();
    if (currentTimestamp > movieTime)
        isPastNode = false;

    for (Node& child : NodeTraversal::descendantsOf(*m_displayTree)) {
        if (child.nodeName() == timestampTag) {
            double currentTimestamp;
            bool check = VTTParser::collectTimeStamp(child.nodeValue(), currentTimestamp);
            ASSERT_UNUSED(check, check);

            if (currentTimestamp > movieTime)
                isPastNode = false;
        }

        if (child.isVTTElement()) {
            toVTTElement(child).setIsPastNode(isPastNode);
            // Make an elemenet id match a cue id for style matching purposes.
            if (!id().isEmpty())
                toElement(child).setIdAttribute(id());
        }
    }
}

PassRefPtrWillBeRawPtr<VTTCueBox> VTTCue::getDisplayTree()
{
    ASSERT(track() && track()->isRendered() && isActive());

    if (!m_displayTree) {
        m_displayTree = VTTCueBox::create(document());
        m_displayTree->appendChild(m_cueBackgroundBox);
    }

    ASSERT(m_displayTree->firstChild() == m_cueBackgroundBox);

    if (!m_displayTreeShouldChange) {
        // Apply updated user style overrides for text tracks when display tree doesn't change.
        // This ensures that the track settings are refreshed when the video is
        // replayed or when the user slides back to an already rendered track.
        applyUserOverrideCSSProperties();
        return m_displayTree;
    }

    createVTTNodeTree();

    m_cueBackgroundBox->removeChildren();
    m_vttNodeTree->cloneChildNodes(m_cueBackgroundBox.get());

    // TODO(philipj): The region identifier may be non-empty without there being
    // a corresponding region, in which case this VTTCueBox will be added
    // directly to the text track container in updateDisplay().
    if (regionId().isEmpty()) {
        VTTDisplayParameters displayParameters = calculateDisplayParameters();
        m_displayTree->applyCSSProperties(displayParameters);
    } else {
        m_displayTree->setInlineStyleProperty(CSSPropertyPosition, CSSValueRelative);
    }

    // Apply user override settings for text tracks
    applyUserOverrideCSSProperties();

    m_displayTreeShouldChange = false;

    return m_displayTree;
}

void VTTCue::removeDisplayTree(RemovalNotification removalNotification)
{
    if (removalNotification == NotifyRegion && track()->regions()) {
        // The region needs to be informed about the cue removal.
        VTTRegion* region = track()->regions()->getRegionById(m_regionId);
        if (region)
            region->willRemoveVTTCueBox(m_displayTree.get());
    }

    if (m_displayTree)
        m_displayTree->remove(ASSERT_NO_EXCEPTION);
}

void VTTCue::updateDisplay(HTMLDivElement& container)
{
    ASSERT(track() && track()->isRendered() && isActive());

    UseCounter::count(document(), UseCounter::VTTCueRender);

    if (m_writingDirection != Horizontal)
        UseCounter::count(document(), UseCounter::VTTCueRenderVertical);

    if (!m_snapToLines)
        UseCounter::count(document(), UseCounter::VTTCueRenderSnapToLinesFalse);

    if (!lineIsAuto())
        UseCounter::count(document(), UseCounter::VTTCueRenderLineNotAuto);

    if (textPositionIsAuto())
        UseCounter::count(document(), UseCounter::VTTCueRenderPositionNot50);

    if (m_cueSize != 100)
        UseCounter::count(document(), UseCounter::VTTCueRenderSizeNot100);

    if (m_cueAlignment != Middle)
        UseCounter::count(document(), UseCounter::VTTCueRenderAlignNotMiddle);

    RefPtrWillBeRawPtr<VTTCueBox> displayBox = getDisplayTree();
    VTTRegion* region = 0;
    if (track()->regions())
        region = track()->regions()->getRegionById(regionId());

    if (!region) {
        // If cue has an empty text track cue region identifier or there is no
        // WebVTT region whose region identifier is identical to cue's text
        // track cue region identifier, run the following substeps:
        if (displayBox->hasChildren() && !container.contains(displayBox.get())) {
            // Note: the display tree of a cue is removed when the active flag of the cue is unset.
            container.appendChild(displayBox);
        }
    } else {
        // Let region be the WebVTT region whose region identifier
        // matches the text track cue region identifier of cue.
        RefPtrWillBeRawPtr<HTMLDivElement> regionNode = region->getDisplayTree(document());

        // Append the region to the viewport, if it was not already.
        if (!container.contains(regionNode.get()))
            container.appendChild(regionNode);

        region->appendVTTCueBox(displayBox);
    }
}

VTTCue::CueSetting VTTCue::settingName(VTTScanner& input) const
{
    CueSetting parsedSetting = None;
    if (input.scan("vertical"))
        parsedSetting = Vertical;
    else if (input.scan("line"))
        parsedSetting = Line;
    else if (input.scan("position"))
        parsedSetting = Position;
    else if (input.scan("size"))
        parsedSetting = Size;
    else if (input.scan("align"))
        parsedSetting = Align;
    else if (RuntimeEnabledFeatures::webVTTRegionsEnabled() && input.scan("region"))
        parsedSetting = RegionId;
    // Verify that a ':' follows.
    if (parsedSetting != None && input.scan(':'))
        return parsedSetting;
    return None;
}

static bool scanPercentage(VTTScanner& input, float& number)
{
    // http://dev.w3.org/html5/webvtt/#dfn-parse-a-percentage-string

    // 1. Let input be the string being parsed.
    // 2. If input contains any characters other than U+0025 PERCENT SIGN
    //    characters (%), U+002E DOT characters (.) and ASCII digits, then
    //    fail.
    // 3. If input does not contain at least one ASCII digit, then fail.
    // 4. If input contains more than one U+002E DOT character (.), then fail.
    // 5. If any character in input other than the last character is a U+0025
    //    PERCENT SIGN character (%), then fail.
    // 6. If the last character in input is not a U+0025 PERCENT SIGN character
    //    (%), then fail.
    // 7. Ignoring the trailing percent sign, interpret input as a real
    //    number. Let that number be the percentage.
    // 8. If percentage is outside the range 0..100, then fail.
    // 9. Return percentage.
    return input.scanPercentage(number) && !isInvalidPercentage(number);
}

void VTTCue::parseSettings(const String& inputString)
{
    VTTScanner input(inputString);

    while (!input.isAtEnd()) {

        // The WebVTT cue settings part of a WebVTT cue consists of zero or more of the following components, in any order,
        // separated from each other by one or more U+0020 SPACE characters or U+0009 CHARACTER TABULATION (tab) characters.
        input.skipWhile<VTTParser::isValidSettingDelimiter>();

        if (input.isAtEnd())
            break;

        // When the user agent is to parse the WebVTT settings given by a string input for a text track cue cue,
        // the user agent must run the following steps:
        // 1. Let settings be the result of splitting input on spaces.
        // 2. For each token setting in the list settings, run the following substeps:
        //    1. If setting does not contain a U+003A COLON character (:), or if the first U+003A COLON character (:)
        //       in setting is either the first or last character of setting, then jump to the step labeled next setting.
        //    2. Let name be the leading substring of setting up to and excluding the first U+003A COLON character (:) in that string.
        CueSetting name = settingName(input);

        // 3. Let value be the trailing substring of setting starting from the character immediately after the first U+003A COLON character (:) in that string.
        VTTScanner::Run valueRun = input.collectUntil<VTTParser::isValidSettingDelimiter>();

        // 4. Run the appropriate substeps that apply for the value of name, as follows:
        switch (name) {
        case Vertical: {
            // If name is a case-sensitive match for "vertical"
            // 1. If value is a case-sensitive match for the string "rl", then
            //    let cue's text track cue writing direction be vertical
            //    growing left.
            if (input.scanRun(valueRun, verticalGrowingLeftKeyword()))
                m_writingDirection = VerticalGrowingLeft;

            // 2. Otherwise, if value is a case-sensitive match for the string
            //    "lr", then let cue's text track cue writing direction be
            //    vertical growing right.
            else if (input.scanRun(valueRun, verticalGrowingRightKeyword()))
                m_writingDirection = VerticalGrowingRight;
            break;
        }
        case Line: {
            // If name is a case-sensitive match for "line"
            // Steps 1 - 2 skipped.
            float number;
            // 3. If linepos does not contain at least one ASCII digit, then
            //    jump to the step labeled next setting.
            // 4. If the last character in linepos is a U+0025 PERCENT SIGN character (%)
            //
            //    If parse a percentage string from linepos doesn't fail, let
            //    number be the returned percentage, otherwise jump to the step
            //    labeled next setting.
            bool isPercentage = scanPercentage(input, number);
            if (!isPercentage) {
                // Otherwise
                //
                // 1. If linepos contains any characters other than U+002D
                //    HYPHEN-MINUS characters (-) and ASCII digits, then jump to
                //    the step labeled next setting.
                // 2. If any character in linepos other than the first character is
                //    a U+002D HYPHEN-MINUS character (-), then jump to the step
                //    labeled next setting.
                bool isNegative = input.scan('-');
                int intLinePosition;
                if (!input.scanDigits(intLinePosition))
                    break;
                // 3. Interpret linepos as a (potentially signed) integer, and let
                //    number be that number.
                number = isNegative ? -intLinePosition : intLinePosition;
            }
            if (!input.isAt(valueRun.end()))
                break;
            // 5. Let cue's text track cue line position be number.
            m_linePosition = number;
            // 6. If the last character in linepos is a U+0025 PERCENT SIGN
            //    character (%), then let cue's text track cue snap-to-lines
            //    flag be false. Otherwise, let it be true.
            m_snapToLines = !isPercentage;
            // Steps 7 - 9 skipped.
            break;
        }
        case Position: {
            // If name is a case-sensitive match for "position".
            float number;
            // Steps 1 - 2 skipped.
            // 3. If parse a percentage string from colpos doesn't fail, let
            //    number be the returned percentage, otherwise jump to the step
            //    labeled next setting (text track cue text position's value
            //    remains the special value auto).
            if (!scanPercentage(input, number))
                break;
            if (!input.isAt(valueRun.end()))
                break;
            // 4. Let cue's text track cue text position be number.
            m_textPosition = number;
            // Steps 5 - 7 skipped.
            break;
        }
        case Size: {
            // If name is a case-sensitive match for "size"
            float number;
            // 1. If parse a percentage string from value doesn't fail, let
            //    number be the returned percentage, otherwise jump to the step
            //    labeled next setting.
            if (!scanPercentage(input, number))
                break;
            if (!input.isAt(valueRun.end()))
                break;
            // 2. Let cue's text track cue size be number.
            m_cueSize = number;
            break;
        }
        case Align: {
            // If name is a case-sensitive match for "align"
            // 1. If value is a case-sensitive match for the string "start",
            //    then let cue's text track cue alignment be start alignment.
            if (input.scanRun(valueRun, startKeyword()))
                m_cueAlignment = Start;

            // 2. If value is a case-sensitive match for the string "middle",
            //    then let cue's text track cue alignment be middle alignment.
            else if (input.scanRun(valueRun, middleKeyword()))
                m_cueAlignment = Middle;

            // 3. If value is a case-sensitive match for the string "end", then
            //    let cue's text track cue alignment be end alignment.
            else if (input.scanRun(valueRun, endKeyword()))
                m_cueAlignment = End;

            // 4. If value is a case-sensitive match for the string "left",
            //    then let cue's text track cue alignment be left alignment.
            else if (input.scanRun(valueRun, leftKeyword()))
                m_cueAlignment = Left;

            // 5. If value is a case-sensitive match for the string "right",
            //    then let cue's text track cue alignment be right alignment.
            else if (input.scanRun(valueRun, rightKeyword()))
                m_cueAlignment = Right;
            break;
        }
        case RegionId:
            m_regionId = input.extractString(valueRun);
            break;
        case None:
            break;
        }

        // Make sure the entire run is consumed.
        input.skipRun(valueRun);
    }

    // If cue's line position is not auto or cue's size is not 100 or cue's
    // writing direction is not horizontal, but cue's region identifier is not
    // the empty string, let cue's region identifier be the empty string.
    if (m_regionId.isEmpty())
        return;

    if (!lineIsAuto() || m_cueSize != 100 || m_writingDirection != Horizontal)
        m_regionId = emptyString();
}

void VTTCue::applyUserOverrideCSSProperties()
{
    Settings* settings = document().settings();
    if (!settings)
        return;

    setInlineStylePropertyIfNotEmpty(*m_cueBackgroundBox,
        CSSPropertyBackgroundColor, settings->textTrackBackgroundColor());
    setInlineStylePropertyIfNotEmpty(*m_cueBackgroundBox,
        CSSPropertyFontFamily, settings->textTrackFontFamily());
    setInlineStylePropertyIfNotEmpty(*m_cueBackgroundBox,
        CSSPropertyFontStyle, settings->textTrackFontStyle());
    setInlineStylePropertyIfNotEmpty(*m_cueBackgroundBox,
        CSSPropertyFontVariant, settings->textTrackFontVariant());
    setInlineStylePropertyIfNotEmpty(*m_cueBackgroundBox,
        CSSPropertyColor, settings->textTrackTextColor());
    setInlineStylePropertyIfNotEmpty(*m_cueBackgroundBox,
        CSSPropertyTextShadow, settings->textTrackTextShadow());
    setInlineStylePropertyIfNotEmpty(*m_cueBackgroundBox,
        CSSPropertyFontSize, settings->textTrackTextSize());
}

ExecutionContext* VTTCue::executionContext() const
{
    ASSERT(m_cueBackgroundBox);
    return m_cueBackgroundBox->executionContext();
}

Document& VTTCue::document() const
{
    ASSERT(m_cueBackgroundBox);
    return m_cueBackgroundBox->document();
}

DEFINE_TRACE(VTTCue)
{
    visitor->trace(m_vttNodeTree);
    visitor->trace(m_cueBackgroundBox);
    visitor->trace(m_displayTree);
    TextTrackCue::trace(visitor);
}

} // namespace blink
