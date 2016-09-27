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
#include "modules/accessibility/AXTableRow.h"

#include "core/layout/LayoutTableRow.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "modules/accessibility/AXTableCell.h"


namespace blink {

using namespace HTMLNames;

AXTableRow::AXTableRow(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
    : AXLayoutObject(layoutObject, axObjectCache)
{
}

AXTableRow::~AXTableRow()
{
}

PassRefPtrWillBeRawPtr<AXTableRow> AXTableRow::create(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXTableRow(layoutObject, axObjectCache));
}

AccessibilityRole AXTableRow::determineAccessibilityRole()
{
    if (!isTableRow())
        return AXLayoutObject::determineAccessibilityRole();

    if ((m_ariaRole = determineAriaRoleAttribute()) != UnknownRole)
        return m_ariaRole;

    return RowRole;
}

bool AXTableRow::isTableRow() const
{
    AXObject* table = parentTable();
    if (!table || !table->isAXTable())
        return false;

    return true;
}

bool AXTableRow::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    AXObjectInclusion decision = defaultObjectInclusion(ignoredReasons);
    if (decision == IncludeObject)
        return false;
    if (decision == IgnoreObject)
        return true;

    if (!isTableRow())
        return AXLayoutObject::computeAccessibilityIsIgnored(ignoredReasons);

    return false;
}

AXObject* AXTableRow::parentTable() const
{
    AXObject* parent = parentObjectUnignored();
    if (!parent || !parent->isAXTable())
        return 0;

    return parent;
}

AXObject* AXTableRow::headerObject()
{
    AccessibilityChildrenVector headers;
    headerObjectsForRow(headers);
    if (!headers.size())
        return 0;

    return headers[0].get();
}

void AXTableRow::headerObjectsForRow(AccessibilityChildrenVector& headers)
{
    if (!m_layoutObject || !m_layoutObject->isTableRow())
        return;

    for (const auto& cell : children()) {
        if (!cell->isTableCell())
            continue;

        if (toAXTableCell(cell.get())->scanToDecideHeaderRole() == RowHeaderRole)
            headers.append(cell);
    }
}

} // namespace blink
