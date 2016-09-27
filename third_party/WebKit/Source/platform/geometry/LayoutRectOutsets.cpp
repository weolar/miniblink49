/*
 * Copyright (c) 2012, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/geometry/LayoutRectOutsets.h"

#include "wtf/Assertions.h"
#include <algorithm>

namespace blink {

LayoutUnit LayoutRectOutsets::logicalTop(WritingMode writingMode) const
{
    return isHorizontalWritingMode(writingMode) ? m_top : m_left;
}

LayoutUnit LayoutRectOutsets::logicalBottom(WritingMode writingMode) const
{
    return isHorizontalWritingMode(writingMode) ? m_bottom : m_right;
}

LayoutUnit LayoutRectOutsets::logicalLeft(WritingMode writingMode) const
{
    return isHorizontalWritingMode(writingMode) ? m_left : m_top;
}

LayoutUnit LayoutRectOutsets::logicalRight(WritingMode writingMode) const
{
    return isHorizontalWritingMode(writingMode) ? m_right : m_bottom;
}

LayoutRectOutsets LayoutRectOutsets::logicalOutsets(WritingMode writingMode) const
{
    if (!isHorizontalWritingMode(writingMode))
        return LayoutRectOutsets(m_left, m_bottom, m_right, m_top);
    return *this;
}

LayoutRectOutsets LayoutRectOutsets::logicalOutsetsWithFlippedLines(WritingMode writingMode) const
{
    LayoutRectOutsets outsets = logicalOutsets(writingMode);
    if (isFlippedLinesWritingMode(writingMode))
        std::swap(outsets.m_top, outsets.m_bottom);
    return outsets;
}

LayoutUnit LayoutRectOutsets::before(WritingMode writingMode) const
{
    switch (writingMode) {
    case TopToBottomWritingMode:
        return m_top;
    case BottomToTopWritingMode:
        return m_bottom;
    case LeftToRightWritingMode:
        return m_left;
    case RightToLeftWritingMode:
        return m_right;
    }
    ASSERT_NOT_REACHED();
    return m_top;
}

LayoutUnit LayoutRectOutsets::after(WritingMode writingMode) const
{
    switch (writingMode) {
    case TopToBottomWritingMode:
        return m_bottom;
    case BottomToTopWritingMode:
        return m_top;
    case LeftToRightWritingMode:
        return m_right;
    case RightToLeftWritingMode:
        return m_left;
    }
    ASSERT_NOT_REACHED();
    return m_bottom;
}

LayoutUnit LayoutRectOutsets::start(WritingMode writingMode, TextDirection direction) const
{
    if (isHorizontalWritingMode(writingMode))
        return isLeftToRightDirection(direction) ? m_left : m_right;
    return isLeftToRightDirection(direction) ? m_top : m_bottom;
}

LayoutUnit LayoutRectOutsets::end(WritingMode writingMode, TextDirection direction) const
{
    if (isHorizontalWritingMode(writingMode))
        return isLeftToRightDirection(direction) ? m_right : m_left;
    return isLeftToRightDirection(direction) ? m_bottom : m_top;
}

void LayoutRectOutsets::setBefore(WritingMode writingMode, LayoutUnit value)
{
    switch (writingMode) {
    case TopToBottomWritingMode:
        m_top = value;
        break;
    case BottomToTopWritingMode:
        m_bottom = value;
        break;
    case LeftToRightWritingMode:
        m_left = value;
        break;
    case RightToLeftWritingMode:
        m_right = value;
        break;
    default:
        ASSERT_NOT_REACHED();
        m_top = value;
    }
}

void LayoutRectOutsets::setAfter(WritingMode writingMode, LayoutUnit value)
{
    switch (writingMode) {
    case TopToBottomWritingMode:
        m_bottom = value;
        break;
    case BottomToTopWritingMode:
        m_top = value;
        break;
    case LeftToRightWritingMode:
        m_right = value;
        break;
    case RightToLeftWritingMode:
        m_left = value;
        break;
    default:
        ASSERT_NOT_REACHED();
        m_bottom = value;
    }
}

void LayoutRectOutsets::setStart(WritingMode writingMode, TextDirection direction, LayoutUnit value)
{
    if (isHorizontalWritingMode(writingMode)) {
        if (isLeftToRightDirection(direction))
            m_left = value;
        else
            m_right = value;
    } else {
        if (isLeftToRightDirection(direction))
            m_top = value;
        else
            m_bottom = value;
    }
}

void LayoutRectOutsets::setEnd(WritingMode writingMode, TextDirection direction, LayoutUnit value)
{
    if (isHorizontalWritingMode(writingMode)) {
        if (isLeftToRightDirection(direction))
            m_right = value;
        else
            m_left = value;
    } else {
        if (isLeftToRightDirection(direction))
            m_bottom = value;
        else
            m_top = value;
    }
}

} // namespace blink
