// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/range/range_f.h"

#include <stddef.h>

#include <algorithm>
#include <cmath>
#include <limits>

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"

namespace gfx {

RangeF::RangeF()
    : start_(0.0f)
    , end_(0.0f)
{
}

RangeF::RangeF(float start, float end)
    : start_(start)
    , end_(end)
{
}

RangeF::RangeF(float position)
    : start_(position)
    , end_(position)
{
}

// static
const RangeF RangeF::InvalidRange()
{
    return RangeF(std::numeric_limits<float>::max());
}

bool RangeF::IsValid() const
{
    return *this != InvalidRange();
}

float RangeF::GetMin() const
{
    return std::min(start(), end());
}

float RangeF::GetMax() const
{
    return std::max(start(), end());
}

bool RangeF::operator==(const RangeF& other) const
{
    return start() == other.start() && end() == other.end();
}

bool RangeF::operator!=(const RangeF& other) const
{
    return !(*this == other);
}

bool RangeF::EqualsIgnoringDirection(const RangeF& other) const
{
    return GetMin() == other.GetMin() && GetMax() == other.GetMax();
}

bool RangeF::Intersects(const RangeF& range) const
{
    return IsValid() && range.IsValid() && !(range.GetMax() < GetMin() || range.GetMin() >= GetMax());
}

bool RangeF::Contains(const RangeF& range) const
{
    return IsValid() && range.IsValid() && GetMin() <= range.GetMin() && range.GetMax() <= GetMax();
}

RangeF RangeF::Intersect(const RangeF& range) const
{
    float min = std::max(GetMin(), range.GetMin());
    float max = std::min(GetMax(), range.GetMax());

    if (min >= max) // No intersection.
        return InvalidRange();

    return RangeF(min, max);
}

RangeF RangeF::Intersect(const Range& range) const
{
    RangeF range_f(range.start(), range.end());
    return Intersect(range_f);
}

Range RangeF::Floor() const
{
    size_t start = start_ > 0.0f ? static_cast<size_t>(std::floor(start_)) : 0;
    size_t end = end_ > 0.0f ? static_cast<size_t>(std::floor(end_)) : 0;
    return Range(start, end);
}

Range RangeF::Ceil() const
{
    size_t start = start_ > 0.0f ? static_cast<size_t>(std::ceil(start_)) : 0;
    size_t end = end_ > 0.0f ? static_cast<size_t>(std::ceil(end_)) : 0;
    return Range(start, end);
}

Range RangeF::Round() const
{
    size_t start = start_ > 0.0f ? static_cast<size_t>(std::floor(start_ + 0.5f)) : 0;
    size_t end = end_ > 0.0f ? static_cast<size_t>(std::floor(end_ + 0.5f)) : 0;
    return Range(start, end);
}

std::string RangeF::ToString() const
{
    return base::StringPrintf("{%f,%f}", start(), end());
}

std::ostream& operator<<(std::ostream& os, const RangeF& range)
{
    return os << range.ToString();
}

} // namespace gfx
