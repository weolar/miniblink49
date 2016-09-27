// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleSelfAlignmentData_h
#define StyleSelfAlignmentData_h

#include "core/style/ComputedStyleConstants.h"

namespace blink {

class StyleSelfAlignmentData {
public:
    // Style data for Self-Aligment and Default-Alignment properties: align-{self, items}, justify-{self, items}.
    // [ <self-position> && <overflow-position>? ] | [ legacy && [ left | right | center ] ]
    StyleSelfAlignmentData(ItemPosition position, OverflowAlignment overflow, ItemPositionType positionType = NonLegacyPosition)
        : m_position(position)
        , m_positionType(positionType)
        , m_overflow(overflow)
    {
    }

    void setPosition(ItemPosition position) { m_position = position; }
    void setPositionType(ItemPositionType positionType) { m_positionType = positionType; }
    void setOverflow(OverflowAlignment overflow) { m_overflow = overflow; }

    ItemPosition position() const { return static_cast<ItemPosition>(m_position); }
    ItemPositionType positionType() const { return static_cast<ItemPositionType>(m_positionType); }
    OverflowAlignment overflow() const { return static_cast<OverflowAlignment>(m_overflow); }

    bool operator==(const StyleSelfAlignmentData& o) const
    {
        return m_position == o.m_position && m_positionType == o.m_positionType && m_overflow == o.m_overflow;
    }

    bool operator!=(const StyleSelfAlignmentData& o) const
    {
        return !(*this == o);
    }

private:
    unsigned m_position : 4; // ItemPosition
    unsigned m_positionType: 1; // Whether or not alignment uses the 'legacy' keyword.
    unsigned m_overflow : 2; // OverflowAlignment
};

} // namespace blink

#endif // StyleSelfAlignmentData_h
