/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef LayoutFlexibleBox_h
#define LayoutFlexibleBox_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/OrderIterator.h"

namespace blink {

class CORE_EXPORT LayoutFlexibleBox : public LayoutBlock {
public:
    LayoutFlexibleBox(Element*);
    ~LayoutFlexibleBox() override;

    static LayoutFlexibleBox* createAnonymous(Document*);

    const char* name() const override { return "LayoutFlexibleBox"; }

    bool isFlexibleBox() const final { return true; }
    void layoutBlock(bool relayoutChildren) final;

    int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override;
    int firstLineBoxBaseline() const override;
    int inlineBlockBaseline(LineDirectionMode) const override;
    IntSize originAdjustmentForScrollbars() const /*override*/;
    bool hasTopOverflow() const /*override*/;
    bool hasLeftOverflow() const /*override*/;

    void paintChildren(const PaintInfo&, const LayoutPoint&) final override;

    bool isHorizontalFlow() const;

    /*const*/ OrderIterator& orderIterator() const { return m_orderIterator; }

    LayoutUnit crossSizeForPercentageResolution(const LayoutBox& child);
    LayoutUnit mainSizeForPercentageResolution(const LayoutBox& child);
    LayoutUnit childLogicalHeightForPercentageResolution(const LayoutBox& child);

    void clearCachedMainSizeForChild(const LayoutBox& child);

    LayoutUnit staticMainAxisPositionForPositionedChild(const LayoutBox& child);
    LayoutUnit staticCrossAxisPositionForPositionedChild(const LayoutBox& child);

    LayoutUnit staticInlinePositionForPositionedChild(const LayoutBox& child);
    LayoutUnit staticBlockPositionForPositionedChild(const LayoutBox& child);

    // Returns true if the position changed. In that case, the child will have to be
    // laid out again.
    bool setStaticPositionForPositionedLayout(LayoutBox& child);
protected:
    void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override;
    void computeChildPreferredLogicalWidths(LayoutObject& child, LayoutUnit& minPreferredLogicalWidth, LayoutUnit& maxPreferredLogicalWidth) const;

    void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    void removeChild(LayoutObject*) override;

private:
    enum FlexSign {
        PositiveFlexibility,
        NegativeFlexibility,
    };

    enum ChildLayoutType {
        LayoutIfNeeded,
        ForceLayout,
        NeverLayout
    };

    enum class TransformedWritingMode {
        TopToBottomWritingMode, RightToLeftWritingMode, LeftToRightWritingMode, BottomToTopWritingMode
    };

    enum class SizeDefiniteness {
        Definite, Indefinite, Unknown
    };

    struct FlexItem;
    struct LineContext;

    typedef Vector<FlexItem> OrderedFlexItemList;
    // Use an inline capacity of 8, since flexbox containers usually have less than 8 children.
    typedef Vector<LayoutRect, 8> ChildFrameRects;

    bool hasOrthogonalFlow(const LayoutBox& child) const;
    bool isColumnFlow() const;
    bool isLeftToRightFlow() const;
    bool isMultiline() const;
    Length flexBasisForChild(const LayoutBox& child) const;
    LayoutUnit crossAxisExtentForChild(const LayoutBox& child) const;
    LayoutUnit crossAxisIntrinsicExtentForChild(const LayoutBox& child) const;
    LayoutUnit childIntrinsicHeight(const LayoutBox& child) const;
    LayoutUnit childIntrinsicWidth(const LayoutBox& child) const;
    LayoutUnit mainAxisExtentForChild(const LayoutBox& child) const;
    LayoutUnit crossAxisExtent() const;
    LayoutUnit mainAxisExtent() const;
    LayoutUnit crossAxisContentExtent() const;
    LayoutUnit mainAxisContentExtent(LayoutUnit contentLogicalHeight);
    LayoutUnit computeMainAxisExtentForChild(const LayoutBox& child, SizeType, const Length& size);
    TransformedWritingMode getTransformedWritingMode() const;
    LayoutUnit flowAwareBorderStart() const;
    LayoutUnit flowAwareBorderEnd() const;
    LayoutUnit flowAwareBorderBefore() const;
    LayoutUnit flowAwareBorderAfter() const;
    LayoutUnit flowAwarePaddingStart() const;
    LayoutUnit flowAwarePaddingEnd() const;
    LayoutUnit flowAwarePaddingBefore() const;
    LayoutUnit flowAwarePaddingAfter() const;
    LayoutUnit flowAwareMarginStartForChild(const LayoutBox& child) const;
    LayoutUnit flowAwareMarginEndForChild(const LayoutBox& child) const;
    LayoutUnit flowAwareMarginBeforeForChild(const LayoutBox& child) const;
    LayoutUnit crossAxisMarginExtentForChild(const LayoutBox& child) const;
    LayoutUnit crossAxisScrollbarExtent() const;
    LayoutUnit crossAxisScrollbarExtentForChild(const LayoutBox& child) const;
    LayoutPoint flowAwareLocationForChild(const LayoutBox& child) const;
    bool useChildAspectRatio(const LayoutBox& child) const;
    LayoutUnit computeMainSizeFromAspectRatioUsing(const LayoutBox& child, Length crossSizeLength) const;
    void setFlowAwareLocationForChild(LayoutBox& child, const LayoutPoint&);
    void adjustAlignmentForChild(LayoutBox& child, LayoutUnit);
    ItemPosition alignmentForChild(const LayoutBox& child) const;
    LayoutUnit mainAxisBorderAndPaddingExtentForChild(const LayoutBox& child) const;
    LayoutUnit computeInnerFlexBaseSizeForChild(LayoutBox& child, ChildLayoutType = LayoutIfNeeded);
    bool mainAxisLengthIsDefinite(const LayoutBox& child, const Length& flexBasis) const;
    bool crossAxisLengthIsDefinite(const LayoutBox& child, const Length& flexBasis) const;
    bool childFlexBaseSizeRequiresLayout(const LayoutBox& child) const;
    bool needToStretchChildLogicalHeight(const LayoutBox& child) const;
    bool childHasIntrinsicMainAxisSize(const LayoutBox& child) const;
    EOverflow mainAxisOverflowForChild(const LayoutBox& child) const;
    EOverflow crossAxisOverflowForChild(const LayoutBox& child) const;
    void cacheChildMainSize(const LayoutBox& child);

    void layoutFlexItems(bool relayoutChildren, SubtreeLayoutScope&);
    LayoutUnit autoMarginOffsetInMainAxis(const OrderedFlexItemList&, LayoutUnit& availableFreeSpace);
    void updateAutoMarginsInMainAxis(LayoutBox& child, LayoutUnit autoMarginOffset);
    bool hasAutoMarginsInCrossAxis(const LayoutBox& child) const;
    bool updateAutoMarginsInCrossAxis(LayoutBox& child, LayoutUnit availableAlignmentSpace);
    void repositionLogicalHeightDependentFlexItems(Vector<LineContext>&);
    LayoutUnit clientLogicalBottomAfterRepositioning();

    LayoutUnit availableAlignmentSpaceForChild(LayoutUnit lineCrossAxisExtent, const LayoutBox& child);
    LayoutUnit availableAlignmentSpaceForChildBeforeStretching(LayoutUnit lineCrossAxisExtent, const LayoutBox& child);
    LayoutUnit marginBoxAscentForChild(const LayoutBox& child);

    LayoutUnit computeChildMarginValue(Length margin);
    void prepareOrderIteratorAndMargins();
    LayoutUnit adjustChildSizeForMinAndMax(const LayoutBox& child, LayoutUnit childSize);
    LayoutUnit adjustChildSizeForAspectRatioCrossAxisMinAndMax(const LayoutBox& child, LayoutUnit childSize);
    // The hypothetical main size of an item is the flex base size clamped according to its min and max main size properties
    bool computeNextFlexLine(OrderedFlexItemList& orderedChildren, LayoutUnit& sumFlexBaseSize, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink, LayoutUnit& sumHypotheticalMainSize, bool relayoutChildren);

    void freezeInflexibleItems(FlexSign, OrderedFlexItemList& children, LayoutUnit& remainingFreeSpace, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink);
    bool resolveFlexibleLengths(FlexSign, OrderedFlexItemList&, LayoutUnit initialFreeSpace, LayoutUnit& remainingFreeSpace, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink);
    void freezeViolations(Vector<FlexItem*>&, LayoutUnit& availableFreeSpace, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink);

    void resetAutoMarginsAndLogicalTopInCrossAxis(LayoutBox& child);
    void setOverrideMainAxisSizeForChild(LayoutBox& child, LayoutUnit childPreferredSize);
    void prepareChildForPositionedLayout(LayoutBox& child);
    size_t numberOfInFlowPositionedChildren(const OrderedFlexItemList&) const;
    void layoutAndPlaceChildren(LayoutUnit& crossAxisOffset, const OrderedFlexItemList&, LayoutUnit availableFreeSpace, bool relayoutChildren, SubtreeLayoutScope&, Vector<LineContext>&);
    void layoutColumnReverse(const OrderedFlexItemList&, LayoutUnit crossAxisOffset, LayoutUnit availableFreeSpace);
    void alignFlexLines(Vector<LineContext>&);
    void alignChildren(const Vector<LineContext>&);
    void applyStretchAlignmentToChild(LayoutBox& child, LayoutUnit lineCrossAxisExtent);
    void flipForRightToLeftColumn();
    void flipForWrapReverse(const Vector<LineContext>&, LayoutUnit crossAxisStartEdge);

    float countIntrinsicSizeForAlgorithmChange(LayoutUnit maxPreferredWidth, LayoutBox* child, float previousMaxContentFlexFraction) const;

    // This is used to cache the preferred size for orthogonal flow children so we don't have to relayout to get it
    HashMap<const LayoutObject*, LayoutUnit> m_intrinsicSizeAlongMainAxis;

    // This set is used to keep track of which children we laid out in this current layout iteration.
    // We need it because the ones in this set may need an additional layout pass for correct stretch alignment
    // handling, as the first layout likely did not use the correct value for percentage sizing of children.
    HashSet<const LayoutObject*> m_relaidOutChildren;

    mutable OrderIterator m_orderIterator;
    int m_numberOfInFlowChildrenOnFirstLine;

    // This is SizeIsUnknown outside of layoutBlock()
    mutable SizeDefiniteness m_hasDefiniteHeight;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutFlexibleBox, isFlexibleBox());

} // namespace blink

#endif // LayoutFlexibleBox_h
