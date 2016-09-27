/*
 * Copyright (C) 2006, 2007, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LayoutTextControlSingleLine_h
#define LayoutTextControlSingleLine_h

#include "core/html/HTMLInputElement.h"
#include "core/layout/LayoutTextControl.h"

namespace blink {

class HTMLInputElement;

class LayoutTextControlSingleLine : public LayoutTextControl {
public:
    LayoutTextControlSingleLine(HTMLInputElement*);
    virtual ~LayoutTextControlSingleLine();
    // FIXME: Move createInnerEditorStyle() to TextControlInnerEditorElement.
    virtual PassRefPtr<ComputedStyle> createInnerEditorStyle(const ComputedStyle& startStyle) const override final;

    void capsLockStateMayHaveChanged();

protected:
    virtual void centerContainerIfNeeded(LayoutBox*) const { }
    virtual LayoutUnit computeLogicalHeightLimit() const;
    Element* containerElement() const;
    Element* editingViewPortElement() const;
    HTMLInputElement* inputElement() const;

private:
    virtual bool hasControlClip() const override final;
    virtual LayoutRect controlClipRect(const LayoutPoint&) const override final;
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectTextField || LayoutTextControl::isOfType(type); }

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;
    virtual void layout() override;

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override final;

    virtual void autoscroll(const IntPoint&) override final;

    // Subclassed to forward to our inner div.
    virtual LayoutUnit scrollLeft() const override final;
    virtual LayoutUnit scrollTop() const override final;
    virtual LayoutUnit scrollWidth() const override final;
    virtual LayoutUnit scrollHeight() const override final;
    virtual void setScrollLeft(LayoutUnit) override final;
    virtual void setScrollTop(LayoutUnit) override final;

    int textBlockWidth() const;
    virtual float getAvgCharWidth(AtomicString family) override final;
    virtual LayoutUnit preferredContentLogicalWidth(float charWidth) const override final;
    virtual LayoutUnit computeControlLogicalHeight(LayoutUnit lineHeight, LayoutUnit nonContentHeight) const override;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override final;

    bool textShouldBeTruncated() const;

    HTMLElement* innerSpinButtonElement() const;

    bool m_shouldDrawCapsLockIndicator;
    LayoutUnit m_desiredInnerEditorLogicalHeight;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutTextControlSingleLine, isTextField());

// ----------------------------

class LayoutTextControlInnerBlock : public LayoutBlockFlow {
public:
    LayoutTextControlInnerBlock(Element* element) : LayoutBlockFlow(element) { }
    virtual int inlineBlockBaseline(LineDirectionMode direction) const override { return lastLineBoxBaseline(direction); }

private:
    virtual bool isIntrinsicallyScrollable(ScrollbarOrientation orientation) const override
    {
        return orientation == HorizontalScrollbar;
    }
    virtual bool scrollsOverflowX() const override { return hasOverflowClip(); }
    virtual bool scrollsOverflowY() const override { return false; }
    virtual bool hasLineIfEmpty() const override { return true; }
};

}

#endif
