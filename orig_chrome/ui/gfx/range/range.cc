// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/range/range.h"

#include <algorithm>
#include <limits>

#include "base/logging.h"
#include "base/strings/stringprintf.h"

namespace gfx {

Range::Range()
    : start_(0)
    , end_(0)
{
}

Range::Range(uint32_t start, uint32_t end)
    : start_(start)
    , end_(end)
{
}

Range::Range(uint32_t position)
    : start_(position)
    , end_(position)
{
}

// static
const Range Range::InvalidRange()
{
    return Range(std::numeric_limits<uint32_t>::max());
}

bool Range::IsValid() const
{
    return *this != InvalidRange();
}

uint32_t Range::GetMin() const
{
    return std::min(start(), end());
}

uint32_t Range::GetMax() const
{
    return std::max(start(), end());
}

bool Range::operator==(const Range& other) const
{
    return start() == other.start() && end() == other.end();
}

bool Range::operator!=(const Range& other) const
{
    return !(*this == other);
}

bool Range::EqualsIgnoringDirection(const Range& other) const
{
    return GetMin() == other.GetMin() && GetMax() == other.GetMax();
}

bool Range::Intersects(const Range& range) const
{
    return IsValid() && range.IsValid() && !(range.GetMax() < GetMin() || range.GetMin() >= GetMax());
}

bool Range::Contains(const Range& range) const
{
    return IsValid() && range.IsValid() && GetMin() <= range.GetMin() && range.GetMax() <= GetMax();
}

Range Range::Intersect(const Range& range) const
{
    uint32_t min = std::max(GetMin(), range.GetMin());
    uint32_t max = std::min(GetMax(), range.GetMax());

    if (min >= max) // No intersection.
        return InvalidRange();

    return Range(min, max);
}

std::string Range::ToString() const
{
    return base::StringPrintf("{%d,%d}", start(), end());
}

std::ostream& operator<<(std::ostream& os, const Range& range)
{
    return os << range.ToString();
}

} // namespace gfx
