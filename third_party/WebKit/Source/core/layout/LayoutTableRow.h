/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2009, 2013 Apple Inc. All rights reserved.
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
 */

#ifndef LayoutTableRow_h
#define LayoutTableRow_h

#include "core/CoreExport.h"
#include "core/layout/LayoutTableSection.h"

namespace blink {

static const unsigned unsetRowIndex = 0x7FFFFFFF;
static const unsigned maxRowIndex = 0x7FFFFFFE; // 2,147,483,646

class CORE_EXPORT LayoutTableRow final : public LayoutBox {
public:
    explicit LayoutTableRow(Element*);

    LayoutTableCell* firstCell() const;
    LayoutTableCell* lastCell() const;

    LayoutTableRow* previousRow() const;
    LayoutTableRow* nextRow() const;

    const LayoutObjectChildList* children() const { return &m_children; }
    LayoutObjectChildList* children() { return &m_children; }

    LayoutTableSection* section() const { return toLayoutTableSection(parent()); }
    LayoutTable* table() const { return toLayoutTable(parent()->parent()); }

    static LayoutTableRow* createAnonymous(Document*);
    static LayoutTableRow* createAnonymousWithParent(const LayoutObject*);
    virtual LayoutBox* createAnonymousBoxWithSameTypeAs(const LayoutObject* parent) const override
    {
        return createAnonymousWithParent(parent);
    }

    void setRowIndex(unsigned rowIndex)
    {
        if (UNLIKELY(rowIndex > maxRowIndex))
            CRASH();

        m_rowIndex = rowIndex;
    }

    bool rowIndexWasSet() const { return m_rowIndex != unsetRowIndex; }
    unsigned rowIndex() const
    {
        ASSERT(rowIndexWasSet());
        ASSERT(!section() || !section()->needsCellRecalc()); // index may be bogus if cells need recalc.
        return m_rowIndex;
    }

    const BorderValue& borderAdjoiningTableStart() const
    {
        if (section()->hasSameDirectionAs(table()))
            return style()->borderStart();

        return style()->borderEnd();
    }

    const BorderValue& borderAdjoiningTableEnd() const
    {
        if (section()->hasSameDirectionAs(table()))
            return style()->borderEnd();

        return style()->borderStart();
    }

    const BorderValue& borderAdjoiningStartCell(const LayoutTableCell*) const;
    const BorderValue& borderAdjoiningEndCell(const LayoutTableCell*) const;

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;

    void addOverflowFromCell(const LayoutTableCell*);

    virtual const char* name() const override { return "LayoutTableRow"; }

private:
    virtual LayoutObjectChildList* virtualChildren() override { return children(); }
    virtual const LayoutObjectChildList* virtualChildren() const override { return children(); }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectTableRow || LayoutBox::isOfType(type); }

    virtual void willBeRemovedFromTree() override;

    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;
    virtual void layout() override;

    virtual DeprecatedPaintLayerType layerTypeRequired() const override
    {
        if (hasTransformRelatedProperty() || hasHiddenBackface() || hasClipPath() || createsGroup() || style()->shouldCompositeForCurrentAnimations() || style()->hasCompositorProxy())
            return NormalDeprecatedPaintLayer;

        if (hasOverflowClip())
            return OverflowClipDeprecatedPaintLayer;

        return NoDeprecatedPaintLayer;
    }

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;

    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) override;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    void nextSibling() const = delete;
    void previousSibling() const = delete;

    LayoutObjectChildList m_children;
    unsigned m_rowIndex : 31;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutTableRow, isTableRow());

inline LayoutTableRow* LayoutTableRow::previousRow() const
{
    return toLayoutTableRow(LayoutObject::previousSibling());
}

inline LayoutTableRow* LayoutTableRow::nextRow() const
{
    return toLayoutTableRow(LayoutObject::nextSibling());
}

inline LayoutTableRow* LayoutTableSection::firstRow() const
{
    ASSERT(children() == virtualChildren());
    return toLayoutTableRow(children()->firstChild());
}

inline LayoutTableRow* LayoutTableSection::lastRow() const
{
    ASSERT(children() == virtualChildren());
    return toLayoutTableRow(children()->lastChild());
}

} // namespace blink

#endif // LayoutTableRow_h
