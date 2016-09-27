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

#ifndef AXTable_h
#define AXTable_h

#include "modules/ModulesExport.h"
#include "modules/accessibility/AXLayoutObject.h"
#include "wtf/Forward.h"

namespace blink {

class AXObjectCacheImpl;
class AXTableCell;

class MODULES_EXPORT AXTable : public AXLayoutObject {

protected:
    AXTable(LayoutObject*, AXObjectCacheImpl&);

public:
    static PassRefPtrWillBeRawPtr<AXTable> create(LayoutObject*, AXObjectCacheImpl&);
    ~AXTable() override;
    DECLARE_VIRTUAL_TRACE();

    void init() final;

    bool isAXTable() const final;
    bool isDataTable() const final;

    AccessibilityRole roleValue() const final;

    void addChildren() override;
    void clearChildren() final;

    // To be overridden by AXARIAGrid.
    virtual bool isAriaTable() const { return false; }
    virtual bool supportsSelectedRows() { return false; }

    const AccessibilityChildrenVector& columns();
    const AccessibilityChildrenVector& rows();

    unsigned columnCount();
    unsigned rowCount();

    String deprecatedTitle(TextUnderElementMode) const final;

    // all the cells in the table
    void cells(AccessibilityChildrenVector&);
    AXTableCell* cellForColumnAndRow(unsigned column, unsigned row);

    void columnHeaders(AccessibilityChildrenVector&);
    void rowHeaders(AccessibilityChildrenVector&);

    // an object that contains, as children, all the objects that act as headers
    AXObject* headerContainer();

protected:
    AccessibilityChildrenVector m_rows;
    AccessibilityChildrenVector m_columns;

    RefPtrWillBeMember<AXObject> m_headerContainer;
    bool m_isAXTable;

    bool hasARIARole() const;
    virtual bool isTableExposableThroughAccessibility() const;
    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const final;
};

DEFINE_AX_OBJECT_TYPE_CASTS(AXTable, isAXTable());

} // namespace blink

#endif // AXTable_h
