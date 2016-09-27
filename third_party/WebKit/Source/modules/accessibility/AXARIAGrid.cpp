/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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
#include "modules/accessibility/AXARIAGrid.h"

#include "core/layout/LayoutObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "modules/accessibility/AXTableColumn.h"
#include "modules/accessibility/AXTableRow.h"


namespace blink {

AXARIAGrid::AXARIAGrid(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
    : AXTable(layoutObject, axObjectCache)
{
}

AXARIAGrid::~AXARIAGrid()
{
}

PassRefPtrWillBeRawPtr<AXARIAGrid> AXARIAGrid::create(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXARIAGrid(layoutObject, axObjectCache));
}

bool AXARIAGrid::addTableCellChild(AXObject* child, HashSet<AXObject*>& appendedRows, unsigned& columnCount)
{
    if (!child || !child->isTableRow() || child->ariaRoleAttribute() != RowRole)
        return false;

    AXTableRow* row = toAXTableRow(child);
    if (appendedRows.contains(row))
        return false;

    // store the maximum number of columns
    unsigned rowCellCount = row->children().size();
    if (rowCellCount > columnCount)
        columnCount = rowCellCount;

    row->setRowIndex((int)m_rows.size());
    m_rows.append(row);

    // Try adding the row if it's not ignoring accessibility,
    // otherwise add its children (the cells) as the grid's children.
    if (!row->accessibilityIsIgnored())
        m_children.append(row);
    else
        m_children.appendVector(row->children());

    appendedRows.add(row);
    return true;
}

void AXARIAGrid::addChildren()
{
    ASSERT(!m_haveChildren);

    if (!isAXTable()) {
        AXLayoutObject::addChildren();
        return;
    }

    m_haveChildren = true;
    if (!m_layoutObject)
        return;

    Vector<AXObject*> children;
    for (AXObject* child = firstChild(); child; child = child->nextSibling())
        children.append(child);
    computeAriaOwnsChildren(children);

    AXObjectCacheImpl& axCache = axObjectCache();

    // add only rows that are labeled as aria rows
    HashSet<AXObject*> appendedRows;
    unsigned columnCount = 0;
    for (const auto& child : children) {
        if (!addTableCellChild(child, appendedRows, columnCount)) {

            // in case the layout tree doesn't match the expected ARIA hierarchy, look at the children
            if (!child->hasChildren())
                child->addChildren();

            // The children of this non-row will contain all non-ignored elements (recursing to find them).
            // This allows the table to dive arbitrarily deep to find the rows.
            for (const auto& childObject : child->children())
                addTableCellChild(childObject.get(), appendedRows, columnCount);
        }
    }

    // make the columns based on the number of columns in the first body
    for (unsigned i = 0; i < columnCount; ++i) {
        AXTableColumn* column = toAXTableColumn(axCache.getOrCreate(ColumnRole));
        column->setColumnIndex((int)i);
        column->setParent(this);
        m_columns.append(column);
        if (!column->accessibilityIsIgnored())
            m_children.append(column);
    }

    AXObject* headerContainerObject = headerContainer();
    if (headerContainerObject && !headerContainerObject->accessibilityIsIgnored())
        m_children.append(headerContainerObject);
}

} // namespace blink
