// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutAnalyzer_h
#define LayoutAnalyzer_h

#include "platform/LayoutUnit.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class LayoutBlock;
class LayoutObject;
class TracedValue;

// Observes the performance of layout and reports statistics via a TracedValue.
// Usage:
// LayoutAnalyzer::Scope analyzer(*this);
class LayoutAnalyzer {
public:
    enum Counter {
        LayoutBlockWidthChanged,
        LayoutBlockHeightChanged,
        LayoutBlockSizeChanged,
        LayoutBlockSizeDidNotChange,
        LayoutObjectsThatSpecifyColumns,
        LayoutAnalyzerStackMaximumDepth,
        LayoutObjectsThatAreFloating,
        LayoutObjectsThatHaveALayer,
        LayoutInlineObjectsThatAlwaysCreateLineBoxes,
        LayoutObjectsThatHadNeverHadLayout,
        LayoutObjectsThatAreOutOfFlowPositioned,
        LayoutObjectsThatNeedPositionedMovementLayout,
        PerformLayoutRootLayoutObjects,
        LayoutObjectsThatNeedLayoutForThemselves,
        LayoutObjectsThatNeedSimplifiedLayout,
        LayoutObjectsThatAreTableCells,
        LayoutObjectsThatAreTextAndCanNotUseTheSimpleFontCodePath,
        CharactersInLayoutObjectsThatAreTextAndCanNotUseTheSimpleFontCodePath,
        LayoutObjectsThatAreTextAndCanUseTheSimpleFontCodePath,
        CharactersInLayoutObjectsThatAreTextAndCanUseTheSimpleFontCodePath,
        TotalLayoutObjectsThatWereLaidOut,
    };
    static const size_t NumCounters = 21;

    class Scope {
    public:
        explicit Scope(const LayoutObject&);
        ~Scope();

    private:
        const LayoutObject& m_layoutObject;
        LayoutAnalyzer* m_analyzer;
    };

    class BlockScope {
    public:
        explicit BlockScope(const LayoutBlock&);
        ~BlockScope();

    private:
        const LayoutBlock& m_block;
        LayoutUnit m_width;
        LayoutUnit m_height;
    };

    LayoutAnalyzer() { }

    void reset();
    void push(const LayoutObject&);
    void pop(const LayoutObject&);

    void increment(Counter counter, unsigned delta = 1)
    {
        m_counters[counter] += delta;
    }

    PassRefPtr<TracedValue> toTracedValue();

private:
    const char* nameForCounter(Counter) const;

    double m_startMs;
    unsigned m_depth;
    unsigned m_counters[NumCounters];
};

} // namespace blink

#endif // LayoutAnalyzer_h
