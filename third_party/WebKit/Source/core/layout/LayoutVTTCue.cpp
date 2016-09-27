/*
 * Copyright (C) 2012 Victor Carbune (victor@rosedu.org)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
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
#include "core/layout/LayoutVTTCue.h"

#include "core/html/shadow/MediaControls.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutState.h"

namespace blink {

LayoutVTTCue::LayoutVTTCue(ContainerNode* node, float snapToLinesPosition)
    : LayoutBlockFlow(node)
    , m_snapToLinesPosition(snapToLinesPosition)
{
}

class SnapToLinesLayouter {
    STACK_ALLOCATED();
public:
    SnapToLinesLayouter(LayoutVTTCue& cueBox, const IntRect& controlsRect)
        : m_cueBox(cueBox)
        , m_controlsRect(controlsRect)
    {
    }

    void layout();

private:
    bool isOutside() const;
    bool isOverlapping() const;
    LayoutUnit computeInitialPositionAdjustment(LayoutUnit&) const;
    bool shouldSwitchDirection(InlineFlowBox*, LayoutUnit) const;

    void moveBoxesBy(LayoutUnit distance)
    {
        m_cueBox.setLogicalTop(m_cueBox.logicalTop() + distance);
    }

    InlineFlowBox* findFirstLineBox() const;

    LayoutPoint m_specifiedPosition;
    LayoutVTTCue& m_cueBox;
    IntRect m_controlsRect;
};

InlineFlowBox* SnapToLinesLayouter::findFirstLineBox() const
{
    if (!m_cueBox.firstChild()->isLayoutInline())
        return nullptr;
    return toLayoutInline(m_cueBox.firstChild())->firstLineBox();
}

LayoutUnit SnapToLinesLayouter::computeInitialPositionAdjustment(LayoutUnit& step) const
{
    ASSERT(std::isfinite(m_cueBox.snapToLinesPosition()));

    // 6. Let line position be the text track cue computed line position.
    // 7. Round line position to an integer by adding 0.5 and then flooring it.
    LayoutUnit linePosition = floorf(m_cueBox.snapToLinesPosition() + 0.5f);

    WritingMode writingMode = m_cueBox.style()->writingMode();
    // 8. Vertical Growing Left: Add one to line position then negate it.
    if (writingMode == RightToLeftWritingMode)
        linePosition = -(linePosition + 1);

    // 9. Let position be the result of multiplying step and line position.
    LayoutUnit position = step * linePosition;

    // 10. Vertical Growing Left: Decrease position by the width of the
    // bounding box of the boxes in boxes, then increase position by step.
    if (writingMode == RightToLeftWritingMode) {
        position -= m_cueBox.size().width();
        position += step;
    }

    // 11. If line position is less than zero...
    if (linePosition < 0) {
        LayoutBlock* parentBlock = m_cueBox.containingBlock();

        // Horizontal / Vertical: ... then increase position by the
        // height / width of the video's rendering area ...
        position += blink::isHorizontalWritingMode(writingMode) ? parentBlock->size().height() : parentBlock->size().width();

        // ... and negate step.
        step = -step;
    }
    return position;
}

bool SnapToLinesLayouter::isOutside() const
{
    return !m_cueBox.containingBlock()->absoluteBoundingBoxRect().contains(m_cueBox.absoluteContentBox());
}

bool SnapToLinesLayouter::isOverlapping() const
{
    IntRect cueBoxRect = m_cueBox.absoluteBoundingBoxRect();
    for (LayoutObject* box = m_cueBox.previousSibling(); box; box = box->previousSibling()) {
        IntRect boxRect = box->absoluteBoundingBoxRect();

        if (cueBoxRect.intersects(boxRect))
            return true;
    }

    if (cueBoxRect.intersects(m_controlsRect))
        return true;

    return false;
}

bool SnapToLinesLayouter::shouldSwitchDirection(InlineFlowBox* firstLineBox, LayoutUnit step) const
{
    // 21. Horizontal: If step is negative and the top of the first line box in
    // boxes is now above the top of the title area, or if step is positive and
    // the bottom of the first line box in boxes is now below the bottom of the
    // title area, jump to the step labeled switch direction.
    // Vertical: If step is negative and the left edge of the first line
    // box in boxes is now to the left of the left edge of the title area, or
    // if step is positive and the right edge of the first line box in boxes is
    // now to the right of the right edge of the title area, jump to the step
    // labeled switch direction.
    LayoutUnit logicalTop = m_cueBox.logicalTop();
    if (step < 0 && logicalTop < 0)
        return true;
    if (step > 0 && logicalTop + firstLineBox->logicalHeight() > m_cueBox.containingBlock()->logicalHeight())
        return true;
    return false;
}

void SnapToLinesLayouter::layout()
{
    // http://dev.w3.org/html5/webvtt/#dfn-apply-webvtt-cue-settings
    // Step 13, "If cue's text track cue snap-to-lines flag is set".

    InlineFlowBox* firstLineBox = findFirstLineBox();
    if (!firstLineBox)
        return;

    // Steps 1-3 skipped.
    // 4. Horizontal: Let step be the height of the first line box in boxes.
    //    Vertical: Let step be the width of the first line box in boxes.
    LayoutUnit step = firstLineBox->logicalHeight();

    // 5. If step is zero, then jump to the step labeled done positioning below.
    if (!step)
        return;

    // Steps 6-11.
    LayoutUnit positionAdjustment = computeInitialPositionAdjustment(step);

    // 12. Move all boxes in boxes ...
    // Horizontal: ... down by the distance given by position
    // Vertical: ... right by the distance given by position
    moveBoxesBy(positionAdjustment);

    // 13. Remember the position of all the boxes in boxes as their specified
    // position.
    m_specifiedPosition = m_cueBox.location();

    // 14. Let best position be null. It will hold a position for boxes, much
    // like specified position in the previous step.
    // 15. Let best position score be null.

    // 16. Let switched be false.
    bool switched = false;

    // Step 17 skipped. (margin == 0; title area == video area)

    // 18. Step loop: If none of the boxes in boxes would overlap any of the
    // boxes in output, and all of the boxes in output are entirely within the
    // title area box, then jump to the step labeled done positioning below.
    while (isOutside() || isOverlapping()) {
        // 19. Let current position score be the percentage of the area of the
        // bounding box of the boxes in boxes that is outside the title area
        // box.
        // 20. If best position is null (i.e. this is the first run through
        // this loop, switched is still false, the boxes in boxes are at their
        // specified position, and best position score is still null), or if
        // current position score is a lower percentage than that in best
        // position score, then remember the position of all the boxes in boxes
        // as their best position, and set best position score to current
        // position score.
        if (!shouldSwitchDirection(firstLineBox, step)) {
            // 22. Horizontal: Move all the boxes in boxes down by the distance
            // given by step. (If step is negative, then this will actually
            // result in an upwards movement of the boxes in absolute terms.)
            // Vertical: Move all the boxes in boxes right by the distance
            // given by step. (If step is negative, then this will actually
            // result in a leftwards movement of the boxes in absolute terms.)
            moveBoxesBy(step);

            // 23. Jump back to the step labeled step loop.
            continue;
        }

        // 24. Switch direction: If switched is true, then move all the boxes in
        // boxes back to their best position, and jump to the step labeled done
        // positioning below.

        // 25. Otherwise, move all the boxes in boxes back to their specified
        // position as determined in the earlier step.
        m_cueBox.setLocation(m_specifiedPosition);

        // XX. If switched is true, jump to the step labeled done
        // positioning below.
        if (switched)
            break;

        // 26. Negate step.
        step = -step;

        // 27. Set switched to true.
        switched = true;

        // 28. Jump back to the step labeled step loop.
    }
}

void LayoutVTTCue::repositionCueSnapToLinesNotSet()
{
    // FIXME: Implement overlapping detection when snap-to-lines is not set. http://wkb.ug/84296

    // http://dev.w3.org/html5/webvtt/#dfn-apply-webvtt-cue-settings
    // Step 13, "If cue's text track cue snap-to-lines flag is not set".

    // 1. Let bounding box be the bounding box of the boxes in boxes.

    // 2. Run the appropriate steps from the following list:
    //    If the text track cue writing direction is horizontal
    //       If the text track cue line alignment is middle alignment
    //          Move all the boxes in boxes up by half of the height of
    //          bounding box.
    //       If the text track cue line alignment is end alignment
    //          Move all the boxes in boxes up by the height of bounding box.
    //
    //    If the text track cue writing direction is vertical growing left or
    //    vertical growing right
    //       If the text track cue line alignment is middle alignment
    //          Move all the boxes in boxes left by half of the width of
    //          bounding box.
    //       If the text track cue line alignment is end alignment
    //          Move all the boxes in boxes left by the width of bounding box.

    // 3. If none of the boxes in boxes would overlap any of the boxes in
    // output, and all the boxes in output are within the video's rendering
    // area, then jump to the step labeled done positioning below.

    // 4. If there is a position to which the boxes in boxes can be moved while
    // maintaining the relative positions of the boxes in boxes to each other
    // such that none of the boxes in boxes would overlap any of the boxes in
    // output, and all the boxes in output would be within the video's
    // rendering area, then move the boxes in boxes to the closest such
    // position to their current position, and then jump to the step labeled
    // done positioning below. If there are multiple such positions that are
    // equidistant from their current position, use the highest one amongst
    // them; if there are several at that height, then use the leftmost one
    // amongst them.

    // 5. Otherwise, jump to the step labeled done positioning below. (The
    // boxes will unfortunately overlap.)
}

void LayoutVTTCue::adjustForTopAndBottomMarginBorderAndPadding()
{
    // Accommodate extra top and bottom padding, border or margin.
    // Note: this is supported only for internal UA styling, not through the cue selector.
    if (!hasInlineDirectionBordersPaddingOrMargin())
        return;
    IntRect containerRect = containingBlock()->absoluteBoundingBoxRect();
    IntRect cueRect = absoluteBoundingBoxRect();

    int topOverflow = cueRect.y() - containerRect.y();
    int bottomOverflow = containerRect.y() + containerRect.height() - cueRect.y() - cueRect.height();

    int adjustment = 0;
    if (topOverflow < 0)
        adjustment = -topOverflow;
    else if (bottomOverflow < 0)
        adjustment = bottomOverflow;

    if (!adjustment)
        return;

    setY(location().y() + adjustment);
}

void LayoutVTTCue::layout()
{
    LayoutBlockFlow::layout();

    ASSERT(firstChild());

    LayoutState state(*this, locationOffset());

    // Determine the area covered by the media controls, if any. If the controls
    // are present, they are the next sibling of the text track container, which
    // is our parent. (LayoutMedia ensures that the media controls are laid out
    // before text tracks, so that the layout is up-to-date here.)
    ASSERT(parent()->node()->isTextTrackContainer());
    IntRect controlsRect;
    if (LayoutObject* parentSibling = parent()->nextSibling()) {
        // Only a part of the media controls is used for overlap avoidance.
        MediaControls* controls = toMediaControls(parentSibling->node());
        if (LayoutObject* controlsLayout = controls->layoutObjectForTextTrackLayout())
            controlsRect = controlsLayout->absoluteBoundingBoxRect();
    }

    // http://dev.w3.org/html5/webvtt/#dfn-apply-webvtt-cue-settings - step 13.
    if (!std::isnan(m_snapToLinesPosition)) {
        SnapToLinesLayouter(*this, controlsRect).layout();

        adjustForTopAndBottomMarginBorderAndPadding();
    } else {
        repositionCueSnapToLinesNotSet();
    }
}

} // namespace blink
