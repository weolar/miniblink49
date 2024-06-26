// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_RANGE_RANGE_F_H_
#define UI_GFX_RANGE_RANGE_F_H_

#include <ostream>
#include <string>

#include "ui/gfx/gfx_export.h"
#include "ui/gfx/range/range.h"

namespace gfx {

// A float version of Range. RangeF is made of a start and end position; when
// they are the same, the range is empty. Note that |start_| can be greater
// than |end_| to respect the directionality of the range.
class GFX_EXPORT RangeF {
public:
    // Creates an empty range {0,0}.
    RangeF();

    // Initializes the range with a start and end.
    RangeF(float start, float end);

    // Initializes the range with the same start and end positions.
    explicit RangeF(float position);

    // Returns a range that is invalid, which is {float_max,float_max}.
    static const RangeF InvalidRange();

    // Checks if the range is valid through comparison to InvalidRange().
    bool IsValid() const;

    // Getters and setters.
    float start() const { return start_; }
    void set_start(float start) { start_ = start; }

    float end() const { return end_; }
    void set_end(float end) { end_ = end; }

    // Returns the absolute value of the length.
    float length() const
    {
        const float length = end() - start();
        return length >= 0 ? length : -length;
    }

    bool is_reversed() const { return start() > end(); }
    bool is_empty() const { return start() == end(); }

    // Returns the minimum and maximum values.
    float GetMin() const;
    float GetMax() const;

    bool operator==(const RangeF& other) const;
    bool operator!=(const RangeF& other) const;
    bool EqualsIgnoringDirection(const RangeF& other) const;

    // Returns true if this range intersects the specified |range|.
    bool Intersects(const RangeF& range) const;

    // Returns true if this range contains the specified |range|.
    bool Contains(const RangeF& range) const;

    // Computes the intersection of this range with the given |range|.
    // If they don't intersect, it returns an InvalidRange().
    // The returned range is always empty or forward (never reversed).
    RangeF Intersect(const RangeF& range) const;
    RangeF Intersect(const Range& range) const;

    // Floor/Ceil/Round the start and end values of the given RangeF.
    Range Floor() const;
    Range Ceil() const;
    Range Round() const;

    std::string ToString() const;

private:
    float start_;
    float end_;
};

GFX_EXPORT std::ostream& operator<<(std::ostream& os, const RangeF& range);

} // namespace gfx

#endif // UI_GFX_RANGE_RANGE_F_H_
