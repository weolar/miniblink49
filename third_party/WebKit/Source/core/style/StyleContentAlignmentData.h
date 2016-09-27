// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleContentAlignmentData_h
#define StyleContentAlignmentData_h

#include "core/style/ComputedStyleConstants.h"

namespace blink {

class StyleContentAlignmentData {
public:
    // Style data for Content-Distribution properties: align-content, justify-content.
    // <content-distribution> || [ <overflow-position>? && <content-position> ]
    StyleContentAlignmentData(ContentPosition position, ContentDistributionType distribution, OverflowAlignment overflow = OverflowAlignmentDefault)
        : m_position(position)
        , m_distribution(distribution)
        , m_overflow(overflow)
    {
    }

    void setPosition(ContentPosition position) { m_position = position; }
    void setDistribution(ContentDistributionType distribution) { m_distribution = distribution; }
    void setOverflow(OverflowAlignment overflow) { m_overflow = overflow; }

    ContentPosition position() const { return static_cast<ContentPosition>(m_position); }
    ContentDistributionType distribution() const { return static_cast<ContentDistributionType>(m_distribution); }
    OverflowAlignment overflow() const { return static_cast<OverflowAlignment>(m_overflow); }

    bool operator==(const StyleContentAlignmentData& o) const
    {
        return m_position == o.m_position && m_distribution == o.m_distribution && m_overflow == o.m_overflow;
    }

    bool operator!=(const StyleContentAlignmentData& o) const
    {
        return !(*this == o);
    }

private:
    unsigned m_position : 4; // ContentPosition
    unsigned m_distribution : 3; // ContentDistributionType
    unsigned m_overflow : 2; // OverflowAlignment
};

} // namespace blink

#endif // StyleContentAlignmentData_h
