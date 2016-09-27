// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/editing/GranularityStrategy.h"

#include "core/editing/FrameSelection.h"
#include "core/editing/htmlediting.h"

namespace blink {

enum class BoundAdjust {CurrentPosIfOnBound, NextBoundIfOnBound};
enum class SearchDirection {SearchBackwards, SearchForward};

// We use the bottom-left corner of the caret rect to represent the
// location of a VisiblePosition. This way locations corresponding to
// VisiblePositions on the same line will all have the same y coordinate
// unless the text is transformed.
static IntPoint positionLocation(const VisiblePosition& vp)
{
    return vp.absoluteCaretBounds().minXMaxYCorner();
}

// Order is specified using the same contract as comparePositions.
static bool arePositionsInSpecifiedOrder(
    const VisiblePosition& vp1,
    const VisiblePosition& vp2,
    int specifiedOrder)
{
    int positionOrder = comparePositions(vp1, vp2);
    if (specifiedOrder == 0)
        return positionOrder == 0;
    return specifiedOrder > 0 ? positionOrder > 0 : positionOrder < 0;
}

// Returns the next word boundary starting from |pos|. |direction| specifies
// the direction in which to search for the next bound. nextIfOnBound
// controls whether |pos| or the next boundary is returned when |pos| is
// located exactly on word boundary.
static VisiblePosition nextWordBound(
    const VisiblePosition& pos,
    SearchDirection direction,
    BoundAdjust wordBoundAdjust)
{
    bool nextBoundIfOnBound = wordBoundAdjust ==  BoundAdjust::NextBoundIfOnBound;
    if (direction == SearchDirection::SearchForward) {
        EWordSide wordSide = nextBoundIfOnBound ? RightWordIfOnBoundary : LeftWordIfOnBoundary;
        return endOfWord(pos, wordSide);
    }
    EWordSide wordSide = nextBoundIfOnBound ? LeftWordIfOnBoundary : RightWordIfOnBoundary;
    return startOfWord(pos, wordSide);
}

GranularityStrategy::GranularityStrategy() { }

GranularityStrategy::~GranularityStrategy() { }

CharacterGranularityStrategy::CharacterGranularityStrategy() { }

CharacterGranularityStrategy::~CharacterGranularityStrategy() { }

SelectionStrategy CharacterGranularityStrategy::GetType() const
{
    return SelectionStrategy::Character;
}

void CharacterGranularityStrategy::Clear() { }

VisibleSelection CharacterGranularityStrategy::updateExtent(const IntPoint& extentPoint, LocalFrame* frame)
{
    const VisiblePosition& extentPosition = visiblePositionForContentsPoint(extentPoint, frame);
    const VisibleSelection& selection = frame->selection().selection();
    if (selection.visibleBase() == extentPosition)
        return selection;
    return VisibleSelection(selection.visibleBase(), extentPosition);
}

DirectionGranularityStrategy::DirectionGranularityStrategy()
    : m_state(StrategyState::Cleared)
    , m_granularity(CharacterGranularity)
    , m_offset(0) { }

DirectionGranularityStrategy::~DirectionGranularityStrategy() { }

SelectionStrategy DirectionGranularityStrategy::GetType() const
{
    return SelectionStrategy::Direction;
}

void DirectionGranularityStrategy::Clear()
{
    m_state = StrategyState::Cleared;
    m_granularity = CharacterGranularity;
    m_offset = 0;
    m_diffExtentPointFromExtentPosition = IntSize();
}

VisibleSelection DirectionGranularityStrategy::updateExtent(const IntPoint& extentPoint, LocalFrame* frame)
{
    const VisibleSelection& selection = frame->selection().selection();

    if (m_state == StrategyState::Cleared)
        m_state = StrategyState::Expanding;

    VisiblePosition oldOffsetExtentPosition = selection.visibleExtent();
    IntPoint oldExtentLocation = positionLocation(oldOffsetExtentPosition);

    IntPoint oldOffsetExtentPoint = oldExtentLocation + m_diffExtentPointFromExtentPosition;
    IntPoint oldExtentPoint = IntPoint(oldOffsetExtentPoint.x() - m_offset, oldOffsetExtentPoint.y());

    // Apply the offset.
    IntPoint newOffsetExtentPoint = extentPoint;
    int dx = extentPoint.x() - oldExtentPoint.x();
    if (m_offset != 0) {
        if (m_offset > 0 && dx > 0)
            m_offset = std::max(0, m_offset - dx);
        else if (m_offset < 0 && dx < 0)
            m_offset = std::min(0, m_offset - dx);
        newOffsetExtentPoint.move(m_offset, 0);
    }

    VisiblePosition newOffsetExtentPosition = visiblePositionForContentsPoint(newOffsetExtentPoint, frame);
    IntPoint newOffsetLocation = positionLocation(newOffsetExtentPosition);

    // Reset the offset in case of a vertical change in the location (could be
    // due to a line change or due to an unusual layout, e.g. rotated text).
    bool verticalChange = newOffsetLocation.y() != oldExtentLocation.y();
    if (verticalChange) {
        m_offset = 0;
        m_granularity = CharacterGranularity;
        newOffsetExtentPoint = extentPoint;
        newOffsetExtentPosition = visiblePositionForContentsPoint(extentPoint, frame);
    }

    const VisiblePosition base = selection.visibleBase();

    // Do not allow empty selection.
    if (newOffsetExtentPosition == base)
        return selection;

    // The direction granularity strategy, particularly the "offset" feature
    // doesn't work with non-horizontal text (e.g. when the text is rotated).
    // So revert to the behavior equivalent to the character granularity
    // strategy if we detect that the text's baseline coordinate changed
    // without a line change.
    if (verticalChange && inSameLine(newOffsetExtentPosition, oldOffsetExtentPosition))
        return VisibleSelection(selection.visibleBase(), newOffsetExtentPosition);

    int oldExtentBaseOrder = selection.isBaseFirst() ? 1 : -1;

    int newExtentBaseOrder;
    bool thisMoveShrunkSelection;
    if (newOffsetExtentPosition == oldOffsetExtentPosition) {
        if (m_granularity == CharacterGranularity)
            return selection;

        // If we are in Word granularity, we cannot exit here, since we may pass
        // the middle of the word without changing the position (in which case
        // the selection needs to expand).
        thisMoveShrunkSelection = false;
        newExtentBaseOrder = oldExtentBaseOrder;
    } else {
        bool selectionExpanded = arePositionsInSpecifiedOrder(newOffsetExtentPosition, oldOffsetExtentPosition, oldExtentBaseOrder);
        bool extentBaseOrderSwitched = selectionExpanded ? false : !arePositionsInSpecifiedOrder(newOffsetExtentPosition, base, oldExtentBaseOrder);
        newExtentBaseOrder = extentBaseOrderSwitched ? -oldExtentBaseOrder : oldExtentBaseOrder;

        // Determine the word boundary, i.e. the boundary extending beyond which
        // should change the granularity to WordGranularity.
        VisiblePosition wordBoundary;
        if (extentBaseOrderSwitched) {
            // Special case.
            // If the extent-base order was switched, then the selection is now
            // expanding in a different direction than before. Therefore we
            // calculate the word boundary in this new direction and based on
            // the |base| position.
            wordBoundary = nextWordBound(
                base,
                newExtentBaseOrder > 0 ? SearchDirection::SearchForward : SearchDirection::SearchBackwards,
                BoundAdjust::NextBoundIfOnBound);
            m_granularity = CharacterGranularity;
        } else {
            // Calculate the word boundary based on |oldExtentWithGranularity|.
            // If selection was shrunk in the last update and the extent is now
            // exactly on the word boundary - we need to take the next bound as
            // the bound of the current word.
            wordBoundary = nextWordBound(
                oldOffsetExtentPosition,
                oldExtentBaseOrder > 0 ? SearchDirection::SearchForward : SearchDirection::SearchBackwards,
                m_state == StrategyState::Shrinking ? BoundAdjust::NextBoundIfOnBound : BoundAdjust::CurrentPosIfOnBound);
        }

        bool expandedBeyondWordBoundary;
        if (selectionExpanded)
            expandedBeyondWordBoundary = arePositionsInSpecifiedOrder(newOffsetExtentPosition, wordBoundary, newExtentBaseOrder);
        else if (extentBaseOrderSwitched)
            expandedBeyondWordBoundary = arePositionsInSpecifiedOrder(newOffsetExtentPosition, wordBoundary, newExtentBaseOrder);
        else
            expandedBeyondWordBoundary = false;

        // The selection is shrunk if the extent changes position to be closer to
        // the base, and the extent/base order wasn't switched.
        thisMoveShrunkSelection = !extentBaseOrderSwitched && !selectionExpanded;

        if (expandedBeyondWordBoundary)
            m_granularity = WordGranularity;
        else if (thisMoveShrunkSelection)
            m_granularity = CharacterGranularity;
    }

    VisiblePosition newSelectionExtent = newOffsetExtentPosition;
    if (m_granularity == WordGranularity) {
        // Determine the bounds of the word where the extent is located.
        // Set the selection extent to one of the two bounds depending on
        // whether the extent is passed the middle of the word.
        VisiblePosition boundBeforeExtent = nextWordBound(newOffsetExtentPosition, SearchDirection::SearchBackwards, BoundAdjust::CurrentPosIfOnBound);
        VisiblePosition boundAfterExtent = nextWordBound(newOffsetExtentPosition, SearchDirection::SearchForward, BoundAdjust::CurrentPosIfOnBound);
        int xMiddleBetweenBounds = (positionLocation(boundAfterExtent).x() + positionLocation(boundBeforeExtent).x()) / 2;
        bool offsetExtentBeforeMiddle = newOffsetExtentPoint.x() < xMiddleBetweenBounds;
        newSelectionExtent = offsetExtentBeforeMiddle ? boundBeforeExtent : boundAfterExtent;
        // Update the offset if selection expanded in word granularity.
        if (newSelectionExtent != selection.visibleExtent()
            && ((newExtentBaseOrder > 0 && !offsetExtentBeforeMiddle) || (newExtentBaseOrder < 0 && offsetExtentBeforeMiddle))) {
            m_offset = positionLocation(newSelectionExtent).x() - extentPoint.x();
        }
    }

    // Only update the state if the selection actually changed as a result of
    // this move.
    if (newSelectionExtent != selection.visibleExtent())
        m_state = thisMoveShrunkSelection ? StrategyState::Shrinking : StrategyState::Expanding;

    m_diffExtentPointFromExtentPosition = extentPoint + IntSize(m_offset, 0) - positionLocation(newSelectionExtent);
    VisibleSelection newSelection = selection;
    newSelection.setExtent(newSelectionExtent);
    return newSelection;
}

} // namespace blink
