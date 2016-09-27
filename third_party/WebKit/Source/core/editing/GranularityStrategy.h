// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GranularityStrategy_h
#define GranularityStrategy_h

#include "core/editing/SelectionStrategy.h"
#include "core/editing/VisibleSelection.h"

namespace blink {

class GranularityStrategy {
public:
    virtual ~GranularityStrategy();
    virtual SelectionStrategy GetType() const = 0;
    virtual void Clear() = 0;

    // Calculates and returns the new selection based on the updated extent
    // location in absolute coordinates.
    virtual VisibleSelection updateExtent(const IntPoint&, LocalFrame*) = 0;

protected:
    GranularityStrategy();
};

// Always uses character granularity.
class CharacterGranularityStrategy final : public GranularityStrategy {
public:
    CharacterGranularityStrategy();
    ~CharacterGranularityStrategy() final;

    // GranularityStrategy:
    SelectionStrategy GetType() const final;
    void Clear() final;
    VisibleSelection updateExtent(const IntPoint&, LocalFrame*) final;
};

// "Expand by word, shrink by character" selection strategy.
// Uses character granularity when selection is shrinking. If the selection is
// expanding, granularity doesn't change until a word boundary is passed, after
// which the granularity switches to "word".
// In word granularity, the word is not selected until the extent point passes
// the middle of the word.
//
// The "offset" feature:
// The offset is the horizontal distance between the extent point (passed in
// updateExtent) and the end of the selection. In character granularity the
// offset is typically zero or near zero, however in word granularity it can be
// significant. When the offset is positive and the extent point moved to the
// left, the offset is preserved, i.e. the selection tracks the extent point
// with the constant offset. When the offset is positive and the extent point
// is moved to the right, the offset gets reduced. Selection will not grow
// until the offset is reduced all the way to zero.
//
// This behavior is best illustrated by an example:
//
// ^ marks base, | marks extent point, > marks selection end:
// Lorem ip^sum|> dolor sit amet, consectetur
//
// Move extent over the middle of "dolor". Granularity should change to word
// granularity and the selection end should jump to the end of the word.
// Lorem ip^sum dolo|r> sit amet, consectetur
//
// Move extent back one character. Granularity changes to "character". The
// selection end should move back one character as well. Note an offset between
// the extent and the selection end.
// Lorem ip^sum dol|o>r sit amet, consectetur
//
// Move extent forward one character. The offset is reduced to 0. Selection end
// doesn't change.
// Lorem ip^sum dolo|>r sit amet, consectetur
//
// Move forward one character. End moves with extent in character granularity.
// Lorem ip^sum dolor|> sit amet, consectetur
class DirectionGranularityStrategy final : public GranularityStrategy {
public:
    DirectionGranularityStrategy();
    ~DirectionGranularityStrategy() final;

    // GranularityStrategy:
    SelectionStrategy GetType() const final;
    void Clear() final;
    VisibleSelection updateExtent(const IntPoint&, LocalFrame*) final;

private:
    enum class StrategyState {
        // Starting state.
        // Selection was cleared and there were no extent updates since then.
        // One an update is performed, the strategy goes into the Expanding
        // state unless the update shrinks the selection without changing
        // relative base/extent order, in which case the strategy goes into the
        // Shrinking state.
        Cleared,
        // Last time the selection was changed by updateExtent - it was expanded
        // or the relative base/extent order was changed.
        Expanding,
        // Last time the selection was changed by updateExtent - it was shrunk
        // (without changing relative base/extent order).
        Shrinking
    };

    StrategyState m_state;

    // Current selection granularity being used.
    TextGranularity m_granularity;

    // Horizontal offset in pixels in absolute coordinates applied to the extent point.
    int m_offset;

    // This defines location of the offset-adjusted extent point (from the
    // latest updateExtent call) relative to the location of extent's
    // VisiblePosition. It is used to detect sub-position extent movement.
    IntSize m_diffExtentPointFromExtentPosition;
};

} // namespace blink

#endif // GranularityStrategy_h
