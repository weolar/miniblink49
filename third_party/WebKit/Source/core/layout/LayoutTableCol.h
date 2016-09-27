/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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

#ifndef LayoutTableCol_h
#define LayoutTableCol_h

#include "core/layout/LayoutBox.h"

namespace blink {

class LayoutTable;
class LayoutTableCell;

class LayoutTableCol final : public LayoutBox {
public:
    explicit LayoutTableCol(Element*);

    LayoutObject* firstChild() const { ASSERT(children() == virtualChildren()); return children()->firstChild(); }

    // If you have a LayoutTableCol, use firstChild or lastChild instead.
    void slowFirstChild() const = delete;
    void slowLastChild() const = delete;

    const LayoutObjectChildList* children() const { return &m_children; }
    LayoutObjectChildList* children() { return &m_children; }

    void clearPreferredLogicalWidthsDirtyBits();

    unsigned span() const { return m_span; }

    bool isTableColumnGroupWithColumnChildren() { return firstChild(); }
    bool isTableColumn() const { return style()->display() == TABLE_COLUMN; }
    bool isTableColumnGroup() const { return style()->display() == TABLE_COLUMN_GROUP; }

    LayoutTableCol* enclosingColumnGroup() const;
    LayoutTableCol* enclosingColumnGroupIfAdjacentBefore() const
    {
        if (previousSibling())
            return nullptr;
        return enclosingColumnGroup();
    }

    LayoutTableCol* enclosingColumnGroupIfAdjacentAfter() const
    {
        if (nextSibling())
            return nullptr;
        return enclosingColumnGroup();
    }


    // Returns the next column or column-group.
    LayoutTableCol* nextColumn() const;

    const BorderValue& borderAdjoiningCellStartBorder(const LayoutTableCell*) const;
    const BorderValue& borderAdjoiningCellEndBorder(const LayoutTableCell*) const;
    const BorderValue& borderAdjoiningCellBefore(const LayoutTableCell*) const;
    const BorderValue& borderAdjoiningCellAfter(const LayoutTableCell*) const;

    virtual const char* name() const override { return "LayoutTableCol"; }

private:
    virtual LayoutObjectChildList* virtualChildren() override { return children(); }
    virtual const LayoutObjectChildList* virtualChildren() const override { return children(); }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectLayoutTableCol || LayoutBox::isOfType(type); }
    virtual void updateFromElement() override;
    virtual void computePreferredLogicalWidths() override { ASSERT_NOT_REACHED(); }

    virtual void insertedIntoTree() override;
    virtual void willBeRemovedFromTree() override;

    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;
    virtual bool canHaveChildren() const override;
    virtual DeprecatedPaintLayerType layerTypeRequired() const override { return NoDeprecatedPaintLayer; }

    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override;
    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) override;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    LayoutTable* table() const;

    LayoutObjectChildList m_children;
    unsigned m_span;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutTableCol, isLayoutTableCol());

}

#endif
