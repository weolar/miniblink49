/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "StyleScrollSnapData.h"

#include "core/style/ComputedStyle.h"

namespace blink {

ScrollSnapPoints::ScrollSnapPoints()
    : repeatOffset(100, Percent)
    , hasRepeat(false)
    , usesElements(false)
{
}

bool operator==(const ScrollSnapPoints& a, const ScrollSnapPoints& b)
{
    return a.repeatOffset == b.repeatOffset
        && a.hasRepeat == b.hasRepeat
        && a.usesElements == b.usesElements;
}

StyleScrollSnapData::StyleScrollSnapData()
    : m_xPoints(ComputedStyle::initialScrollSnapPointsX())
    , m_yPoints(ComputedStyle::initialScrollSnapPointsY())
    , m_destination(ComputedStyle::initialScrollSnapDestination())
    , m_coordinates(ComputedStyle::initialScrollSnapCoordinate())
{
}

StyleScrollSnapData::StyleScrollSnapData(const StyleScrollSnapData& other)
    : m_xPoints(other.m_xPoints)
    , m_yPoints(other.m_yPoints)
    , m_destination(other.m_destination)
    , m_coordinates(other.m_coordinates)
{
}

bool operator==(const StyleScrollSnapData& a, const StyleScrollSnapData& b)
{
    return a.m_xPoints == b.m_xPoints
        && a.m_yPoints == b.m_yPoints
        && a.m_destination == b.m_destination
        && a.m_coordinates == b.m_coordinates;
}

} // namespace blink
