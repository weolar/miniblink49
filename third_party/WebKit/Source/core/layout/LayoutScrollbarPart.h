/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutScrollbarPart_h
#define LayoutScrollbarPart_h

#include "core/layout/LayoutBlock.h"
#include "platform/scroll/ScrollTypes.h"

namespace blink {

class LayoutScrollbar;

class LayoutScrollbarPart final : public LayoutBlock {
public:
    static LayoutScrollbarPart* createAnonymous(Document*, LayoutScrollbar* = nullptr, ScrollbarPart = NoPart);

    virtual ~LayoutScrollbarPart();

    virtual const char* name() const override { return "LayoutScrollbarPart"; }

    virtual DeprecatedPaintLayerType layerTypeRequired() const override { return NoDeprecatedPaintLayer; }

    virtual void layout() override;

    // Scrollbar parts needs to be rendered at device pixel boundaries.
    virtual LayoutRectOutsets marginBoxOutsets() const override
    {
        ASSERT(isIntegerValue(LayoutBlock::marginBoxOutsets().top()));
        return LayoutBlock::marginBoxOutsets();
    }
    virtual LayoutUnit marginTop() const override
    {
        ASSERT(isIntegerValue(LayoutBlock::marginTop()));
        return LayoutBlock::marginTop();
    }
    virtual LayoutUnit marginBottom() const override
    {
        ASSERT(isIntegerValue(LayoutBlock::marginBottom()));
        return LayoutBlock::marginBottom();
    }
    virtual LayoutUnit marginLeft() const override
    {
        ASSERT(isIntegerValue(LayoutBlock::marginLeft()));
        return LayoutBlock::marginLeft();
    }
    virtual LayoutUnit marginRight() const override
    {
        ASSERT(isIntegerValue(LayoutBlock::marginRight()));
        return LayoutBlock::marginRight();
    }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectLayoutScrollbarPart || LayoutBlock::isOfType(type); }
    LayoutObject* layoutObjectOwningScrollbar() const;

protected:
    virtual void styleWillChange(StyleDifference, const ComputedStyle& newStyle) override;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) override;

private:
    LayoutScrollbarPart(LayoutScrollbar*, ScrollbarPart);

    virtual void computePreferredLogicalWidths() override;

    // Have all padding getters return 0. The important point here is to avoid resolving percents
    // against the containing block, since scroll bar corners don't always have one (so it would
    // crash). Scroll bar corners are not actually laid out, and they don't have child content, so
    // what we return here doesn't really matter.
    virtual LayoutUnit paddingTop() const override { return LayoutUnit(); }
    virtual LayoutUnit paddingBottom() const override { return LayoutUnit(); }
    virtual LayoutUnit paddingLeft() const override { return LayoutUnit(); }
    virtual LayoutUnit paddingRight() const override { return LayoutUnit(); }
    virtual LayoutUnit paddingBefore() const override { return LayoutUnit(); }
    virtual LayoutUnit paddingAfter() const override { return LayoutUnit(); }
    virtual LayoutUnit paddingStart() const override { return LayoutUnit(); }
    virtual LayoutUnit paddingEnd() const override { return LayoutUnit(); }

    void layoutHorizontalPart();
    void layoutVerticalPart();

    void computeScrollbarWidth();
    void computeScrollbarHeight();

    LayoutScrollbar* m_scrollbar;
    ScrollbarPart m_part;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutScrollbarPart, isLayoutScrollbarPart());

} // namespace blink

#endif // LayoutScrollbarPart_h
