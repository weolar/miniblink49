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

#include "config.h"
#include "core/layout/LayoutTableCol.h"

#include "core/HTMLNames.h"
#include "core/html/HTMLTableColElement.h"
#include "core/layout/LayoutTable.h"
#include "core/layout/LayoutTableCell.h"

namespace blink {

using namespace HTMLNames;

LayoutTableCol::LayoutTableCol(Element* element)
    : LayoutBox(element)
    , m_span(1)
{
    // init LayoutObject attributes
    setInline(true); // our object is not Inline
    updateFromElement();
}

void LayoutTableCol::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBox::styleDidChange(diff, oldStyle);

    // If border was changed, notify table.
    if (parent()) {
        LayoutTable* table = this->table();
        if (table && !table->selfNeedsLayout() && !table->normalChildNeedsLayout() && oldStyle && oldStyle->border() != style()->border()) {
            table->invalidateCollapsedBorders();
        } else if (oldStyle && oldStyle->logicalWidth() != style()->logicalWidth()) {
            // FIXME : setPreferredLogicalWidthsDirty is done for all cells as of now.
            // Need to find a better way so that only the cells which are changed by
            // the col width should have preferred logical widths recomputed.
            for (LayoutObject* child = table->children()->firstChild(); child; child = child->nextSibling()) {
                if (!child->isTableSection())
                    continue;
                LayoutTableSection* section = toLayoutTableSection(child);
                for (LayoutTableRow* row = section->firstRow(); row; row = row->nextRow()) {
                    for (LayoutTableCell* cell = row->firstCell(); cell; cell = cell->nextCell())
                        cell->setPreferredLogicalWidthsDirty();
                }
            }
        }
    }
}

void LayoutTableCol::updateFromElement()
{
    unsigned oldSpan = m_span;
    Node* n = node();
    if (isHTMLTableColElement(n)) {
        HTMLTableColElement& tc = toHTMLTableColElement(*n);
        m_span = tc.span();
    } else {
        m_span = 1;
    }
    if (m_span != oldSpan && style() && parent())
        setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AttributeChanged);
}

void LayoutTableCol::insertedIntoTree()
{
    LayoutBox::insertedIntoTree();
    table()->addColumn(this);
}

void LayoutTableCol::willBeRemovedFromTree()
{
    LayoutBox::willBeRemovedFromTree();
    table()->removeColumn(this);
}

bool LayoutTableCol::isChildAllowed(LayoutObject* child, const ComputedStyle& style) const
{
    // We cannot use isTableColumn here as style() may return 0.
    return child->isLayoutTableCol() && style.display() == TABLE_COLUMN;
}

bool LayoutTableCol::canHaveChildren() const
{
    // Cols cannot have children. This is actually necessary to fix a bug
    // with libraries.uc.edu, which makes a <p> be a table-column.
    return isTableColumnGroup();
}

LayoutRect LayoutTableCol::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    // For now, just paint invalidate the whole table.
    // FIXME: Find a better way to do this, e.g., need to paint invalidate all the cells that we
    // might have propagated a background color or borders into.
    // FIXME: check for paintInvalidationContainer each time here?

    LayoutTable* parentTable = table();
    if (!parentTable)
        return LayoutRect();
    return parentTable->clippedOverflowRectForPaintInvalidation(paintInvalidationContainer, paintInvalidationState);
}

void LayoutTableCol::imageChanged(WrappedImagePtr, const IntRect*)
{
    // FIXME: Issue paint invalidation of only the rect the image paints in.
    setShouldDoFullPaintInvalidation();
}

void LayoutTableCol::clearPreferredLogicalWidthsDirtyBits()
{
    clearPreferredLogicalWidthsDirty();

    for (LayoutObject* child = firstChild(); child; child = child->nextSibling())
        child->clearPreferredLogicalWidthsDirty();
}

LayoutTable* LayoutTableCol::table() const
{
    LayoutObject* table = parent();
    if (table && !table->isTable())
        table = table->parent();
    return table && table->isTable() ? toLayoutTable(table) : nullptr;
}

LayoutTableCol* LayoutTableCol::enclosingColumnGroup() const
{
    if (!parent()->isLayoutTableCol())
        return nullptr;

    LayoutTableCol* parentColumnGroup = toLayoutTableCol(parent());
    ASSERT(parentColumnGroup->isTableColumnGroup());
    ASSERT(isTableColumn());
    return parentColumnGroup;
}

LayoutTableCol* LayoutTableCol::nextColumn() const
{
    // If |this| is a column-group, the next column is the colgroup's first child column.
    if (LayoutObject* firstChild = this->firstChild())
        return toLayoutTableCol(firstChild);

    // Otherwise it's the next column along.
    LayoutObject* next = nextSibling();

    // Failing that, the child is the last column in a column-group, so the next column is the next column/column-group after its column-group.
    if (!next && parent()->isLayoutTableCol())
        next = parent()->nextSibling();

    for (; next && !next->isLayoutTableCol(); next = next->nextSibling()) { }

    return toLayoutTableCol(next);
}

const BorderValue& LayoutTableCol::borderAdjoiningCellStartBorder(const LayoutTableCell*) const
{
    return style()->borderStart();
}

const BorderValue& LayoutTableCol::borderAdjoiningCellEndBorder(const LayoutTableCell*) const
{
    return style()->borderEnd();
}

const BorderValue& LayoutTableCol::borderAdjoiningCellBefore(const LayoutTableCell* cell) const
{
    ASSERT_UNUSED(cell, table()->colElement(cell->col() + cell->colSpan()) == this);
    return style()->borderStart();
}

const BorderValue& LayoutTableCol::borderAdjoiningCellAfter(const LayoutTableCell* cell) const
{
    ASSERT_UNUSED(cell, table()->colElement(cell->col() - 1) == this);
    return style()->borderEnd();
}

}
