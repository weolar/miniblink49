/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/accessibility/AXTableCell.h"

#include "core/layout/LayoutTableCell.h"
#include "modules/accessibility/AXObjectCacheImpl.h"


namespace blink {

using namespace HTMLNames;

AXTableCell::AXTableCell(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
    : AXLayoutObject(layoutObject, axObjectCache)
{
}

AXTableCell::~AXTableCell()
{
}

PassRefPtrWillBeRawPtr<AXTableCell> AXTableCell::create(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXTableCell(layoutObject, axObjectCache));
}

bool AXTableCell::isTableHeaderCell() const
{
    return node() && node()->hasTagName(thTag);
}

bool AXTableCell::isRowHeaderCell() const
{
    const AtomicString& scope = getAttribute(scopeAttr);
    return equalIgnoringCase(scope, "row") || equalIgnoringCase(scope, "rowgroup");
}

bool AXTableCell::isColumnHeaderCell() const
{
    const AtomicString& scope = getAttribute(scopeAttr);
    return equalIgnoringCase(scope, "col") || equalIgnoringCase(scope, "colgroup");
}

bool AXTableCell::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    AXObjectInclusion decision = defaultObjectInclusion(ignoredReasons);
    if (decision == IncludeObject)
        return false;
    if (decision == IgnoreObject)
        return true;

    if (!isTableCell())
        return AXLayoutObject::computeAccessibilityIsIgnored(ignoredReasons);

    return false;
}

AXObject* AXTableCell::parentTable() const
{
    if (!m_layoutObject || !m_layoutObject->isTableCell())
        return 0;

    // If the document no longer exists, we might not have an axObjectCache.
    if (isDetached())
        return 0;

    // Do not use getOrCreate. parentTable() can be called while the layout tree is being modified
    // by javascript, and creating a table element may try to access the layout tree while in a bad state.
    // By using only get() implies that the AXTable must be created before AXTableCells. This should
    // always be the case when AT clients access a table.
    // https://bugs.webkit.org/show_bug.cgi?id=42652
    return axObjectCache().get(toLayoutTableCell(m_layoutObject)->table());
}

bool AXTableCell::isTableCell() const
{
    AXObject* parent = parentObjectUnignored();
    if (!parent || !parent->isTableRow())
        return false;

    return true;
}

static AccessibilityRole decideRoleFromSibling(LayoutTableCell* siblingCell)
{
    if (!siblingCell)
        return CellRole;

    if (Node* siblingNode = siblingCell->node()) {
        if (siblingNode->hasTagName(thTag))
            return ColumnHeaderRole;
        if (siblingNode->hasTagName(tdTag))
            return RowHeaderRole;
    }

    return CellRole;
}

AccessibilityRole AXTableCell::scanToDecideHeaderRole()
{
    if (!isTableHeaderCell())
        return CellRole;

    // Check scope attribute first.
    if (isRowHeaderCell())
        return RowHeaderRole;

    if (isColumnHeaderCell())
        return ColumnHeaderRole;

    // Check the previous cell and the next cell on the same row.
    LayoutTableCell* layoutCell = toLayoutTableCell(m_layoutObject);
    AccessibilityRole headerRole = CellRole;

    // if header is preceded by header cells on the same row, then it is a
    // column header. If it is preceded by other cells then it's a row header.
    if ((headerRole = decideRoleFromSibling(layoutCell->previousCell())) != CellRole)
        return headerRole;

    // if header is followed by header cells on the same row, then it is a
    // column header. If it is followed by other cells then it's a row header.
    if ((headerRole = decideRoleFromSibling(layoutCell->nextCell())) != CellRole)
        return headerRole;

    // If there are no other cells on that row, then it is a column header.
    return ColumnHeaderRole;
}

AccessibilityRole AXTableCell::determineAccessibilityRole()
{
    if (!isTableCell())
        return AXLayoutObject::determineAccessibilityRole();

    return scanToDecideHeaderRole();
}

void AXTableCell::rowIndexRange(pair<unsigned, unsigned>& rowRange)
{
    if (!m_layoutObject || !m_layoutObject->isTableCell())
        return;

    LayoutTableCell* layoutCell = toLayoutTableCell(m_layoutObject);
    rowRange.first = layoutCell->rowIndex();
    rowRange.second = layoutCell->rowSpan();

    // since our table might have multiple sections, we have to offset our row appropriately
    LayoutTableSection* section = layoutCell->section();
    LayoutTable* table = layoutCell->table();
    if (!table || !section)
        return;

    LayoutTableSection* tableSection = table->topSection();
    unsigned rowOffset = 0;
    while (tableSection) {
        if (tableSection == section)
            break;
        rowOffset += tableSection->numRows();
        tableSection = table->sectionBelow(tableSection, SkipEmptySections);
    }

    rowRange.first += rowOffset;
}

void AXTableCell::columnIndexRange(pair<unsigned, unsigned>& columnRange)
{
    if (!m_layoutObject || !m_layoutObject->isTableCell())
        return;

    LayoutTableCell* cell = toLayoutTableCell(m_layoutObject);
    columnRange.first = cell->table()->colToEffCol(cell->col());
    columnRange.second = cell->table()->colToEffCol(cell->col() + cell->colSpan()) - columnRange.first;
}

SortDirection AXTableCell::sortDirection() const
{
    if (roleValue() != RowHeaderRole
        && roleValue() != ColumnHeaderRole)
        return SortDirectionUndefined;

    const AtomicString& ariaSort = getAttribute(aria_sortAttr);
    if (ariaSort.isEmpty())
        return SortDirectionUndefined;
    if (equalIgnoringCase(ariaSort, "none"))
        return SortDirectionNone;
    if (equalIgnoringCase(ariaSort, "ascending"))
        return SortDirectionAscending;
    if (equalIgnoringCase(ariaSort, "descending"))
        return SortDirectionDescending;
    if (equalIgnoringCase(ariaSort, "other"))
        return SortDirectionOther;
    return SortDirectionUndefined;
}

AXObject* AXTableCell::deprecatedTitleUIElement() const
{
    // Try to find if the first cell in this row is a <th>. If it is,
    // then it can act as the title ui element. (This is only in the
    // case when the table is not appearing as an AXTable.)
    if (isTableCell() || !m_layoutObject || !m_layoutObject->isTableCell())
        return 0;

    // Table cells that are th cannot have title ui elements, since by definition
    // they are title ui elements
    if (isTableHeaderCell())
        return 0;

    LayoutTableCell* layoutCell = toLayoutTableCell(m_layoutObject);

    // If this cell is in the first column, there is no need to continue.
    int col = layoutCell->col();
    if (!col)
        return 0;

    int row = layoutCell->rowIndex();

    LayoutTableSection* section = layoutCell->section();
    if (!section)
        return 0;

    LayoutTableCell* headerCell = section->primaryCellAt(row, 0);
    if (!headerCell || headerCell == layoutCell)
        return 0;

    Node* cellElement = headerCell->node();
    if (!cellElement || !cellElement->hasTagName(thTag))
        return 0;

    return axObjectCache().getOrCreate(headerCell);
}

} // namespace blink
