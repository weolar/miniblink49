/*
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *           (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef LayoutTextControl_h
#define LayoutTextControl_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutFlexibleBox.h"

namespace blink {

class HTMLTextFormControlElement;

class CORE_EXPORT LayoutTextControl : public LayoutBlockFlow {
public:
    virtual ~LayoutTextControl();

    HTMLTextFormControlElement* textFormControlElement() const;
    virtual PassRefPtr<ComputedStyle> createInnerEditorStyle(const ComputedStyle& startStyle) const = 0;

    virtual const char* name() const override { return "LayoutTextControl"; }

protected:
    LayoutTextControl(HTMLTextFormControlElement*);

    // This convenience function should not be made public because
    // innerEditorElement may outlive the layout tree.
    HTMLElement* innerEditorElement() const;

    int scrollbarThickness() const;
    void adjustInnerEditorStyle(ComputedStyle& textBlockStyle) const;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    void hitInnerEditorElement(HitTestResult&, const LayoutPoint& pointInContainer, const LayoutPoint& accumulatedOffset);

    int textBlockLogicalWidth() const;
    int textBlockLogicalHeight() const;

    float scaleEmToUnits(int x) const;

    static bool hasValidAvgCharWidth(AtomicString family);
    virtual float getAvgCharWidth(AtomicString family);
    virtual LayoutUnit preferredContentLogicalWidth(float charWidth) const = 0;
    virtual LayoutUnit computeControlLogicalHeight(LayoutUnit lineHeight, LayoutUnit nonContentHeight) const = 0;

    virtual void updateFromElement() override;
    virtual void computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues&) const override;
    virtual LayoutObject* layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope&) override;

    // We need to override this function because we don't want overflow:hidden on an <input>
    // to affect the baseline calculation. This is necessary because we are an inline-block
    // element as an implementation detail which would normally be affected by this.
    virtual int inlineBlockBaseline(LineDirectionMode direction) const override { return lastLineBoxBaseline(direction); }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectTextControl || LayoutBlockFlow::isOfType(type); }

private:
    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override final;
    virtual void computePreferredLogicalWidths() override final;
    virtual void removeLeftoverAnonymousBlock(LayoutBlock*) override final { }
    virtual bool avoidsFloats() const override final { return true; }

    virtual void addChild(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override final;

    virtual void addFocusRingRects(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const override final;

    virtual bool canBeProgramaticallyScrolled() const override final { return true; }
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutTextControl, isTextControl());

// LayoutObject for our inner container, for <search> and others.
// We can't use LayoutFlexibleBox directly, because flexboxes have a different
// baseline definition, and then inputs of different types wouldn't line up
// anymore.
class LayoutTextControlInnerContainer final : public LayoutFlexibleBox {
public:
    explicit LayoutTextControlInnerContainer(Element* element)
        : LayoutFlexibleBox(element)
    { }
    virtual ~LayoutTextControlInnerContainer() { }

    virtual int baselinePosition(FontBaseline baseline, bool firstLine, LineDirectionMode direction, LinePositionMode position) const override
    {
        return LayoutBlock::baselinePosition(baseline, firstLine, direction, position);
    }
    virtual int firstLineBoxBaseline() const override { return LayoutBlock::firstLineBoxBaseline(); }
    virtual int inlineBlockBaseline(LineDirectionMode direction) const override { return lastLineBoxBaseline(direction); }
};


} // namespace blink

#endif // LayoutTextControl_h
