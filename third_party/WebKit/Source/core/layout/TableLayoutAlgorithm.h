/*
 * Copyright (C) 2002 Lars Knoll (knoll@kde.org)
 *           (C) 2002 Dirk Mueller (mueller@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License.
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

#ifndef TableLayoutAlgorithm_h
#define TableLayoutAlgorithm_h

#include "wtf/FastAllocBase.h"
#include "wtf/Noncopyable.h"

namespace blink {

class LayoutUnit;
class LayoutTable;

class TableLayoutAlgorithm {
    WTF_MAKE_NONCOPYABLE(TableLayoutAlgorithm); WTF_MAKE_FAST_ALLOCATED(TableLayoutAlgorithm);
public:
    explicit TableLayoutAlgorithm(LayoutTable* table)
        : m_table(table)
    {
    }

    virtual ~TableLayoutAlgorithm() { }

    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minWidth, LayoutUnit& maxWidth) = 0;
    virtual void applyPreferredLogicalWidthQuirks(LayoutUnit& minWidth, LayoutUnit& maxWidth) const = 0;
    virtual void layout() = 0;
    virtual void willChangeTableLayout() = 0;

protected:
    // FIXME: Once we enable SATURATED_LAYOUT_ARITHMETHIC, this should just be LayoutUnit::nearlyMax().
    // Until then though, using nearlyMax causes overflow in some tests, so we just pick a large number.
    const static int tableMaxWidth = 1000000;

    LayoutTable* m_table;
};

} // namespace blink

#endif // TableLayoutAlgorithm_h
