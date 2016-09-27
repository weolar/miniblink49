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
#include "modules/accessibility/AXTableColumn.h"

#include "core/layout/LayoutTableCell.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "modules/accessibility/AXTableCell.h"


namespace blink {

using namespace HTMLNames;

AXTableColumn::AXTableColumn(AXObjectCacheImpl& axObjectCache)
    : AXMockObject(axObjectCache)
{
}

AXTableColumn::~AXTableColumn()
{
}

PassRefPtrWillBeRawPtr<AXTableColumn> AXTableColumn::create(AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXTableColumn(axObjectCache));
}


void AXTableColumn::setParent(AXObject* parent)
{
    AXMockObject::setParent(parent);

    clearChildren();
}

LayoutRect AXTableColumn::elementRect() const
{
    // this will be filled in when addChildren is called
    return m_columnRect;
}

void AXTableColumn::headerObjectsForColumn(AccessibilityChildrenVector& headers)
{
    if (!m_parent)
        return;

    LayoutObject* layoutObject = m_parent->layoutObject();
    if (!layoutObject)
        return;

    if (!m_parent->isAXTable())
        return;

    if (toAXTable(m_parent)->isAriaTable()) {
        for (const auto& cell : children()) {
            if (cell->roleValue() == ColumnHeaderRole)
                headers.append(cell);
        }
        return;
    }

    if (!layoutObject->isTable())
        return;

    LayoutTable* table = toLayoutTable(layoutObject);
    LayoutTableSection* tableSection = table->topSection();
    for (; tableSection; tableSection = table->sectionBelow(tableSection, SkipEmptySections)) {
        unsigned numCols = tableSection->numColumns();
        if (m_columnIndex >= numCols)
            continue;
        unsigned numRows = tableSection->numRows();
        for (unsigned r = 0; r < numRows; r++) {
            LayoutTableCell* layoutCell = tableSection->primaryCellAt(r, m_columnIndex);
            if (!layoutCell)
                continue;

            AXObject* cell = axObjectCache().getOrCreate(layoutCell->node());
            if (!cell || !cell->isTableCell() || headers.contains(cell))
                continue;

            if (toAXTableCell(cell)->scanToDecideHeaderRole() == ColumnHeaderRole)
                headers.append(cell);
        }
    }
}

AXObject* AXTableColumn::headerObject()
{
    AccessibilityChildrenVector headers;
    headerObjectsForColumn(headers);
    if (!headers.size())
        return 0;

    return headers[0].get();
}

bool AXTableColumn::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    if (!m_parent)
        return true;

    if (!m_parent->accessibilityIsIgnored())
        return false;

    if (ignoredReasons)
        m_parent->computeAccessibilityIsIgnored(ignoredReasons);

    return true;
}

void AXTableColumn::addChildren()
{
    ASSERT(!m_haveChildren);

    m_haveChildren = true;
    if (!m_parent || !m_parent->isAXTable())
        return;

    AXTable* parentTable = toAXTable(m_parent);
    int numRows = parentTable->rowCount();

    for (int i = 0; i < numRows; i++) {
        AXTableCell* cell = parentTable->cellForColumnAndRow(m_columnIndex, i);
        if (!cell)
            continue;

        // make sure the last one isn't the same as this one (rowspan cells)
        if (m_children.size() > 0 && m_children.last() == cell)
            continue;

        m_children.append(cell);
        m_columnRect.unite(cell->elementRect());
    }
}

} // namespace blink
