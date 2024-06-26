// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/range/range.h"

#include <limits>

#include "base/logging.h"

namespace gfx {

Range::Range(const CHARRANGE& range, LONG total_length)
{
    // Check if this is an invalid range.
    if (range.cpMin == -1 && range.cpMax == -1) {
        *this = InvalidRange();
    } else {
        DCHECK_GE(range.cpMin, 0);
        set_start(range.cpMin);
        // {0,-1} is the "whole range" but that doesn't mean much out of context,
        // so use the |total_length| parameter.
        if (range.cpMax == -1) {
            DCHECK_EQ(0, range.cpMin);
            DCHECK_NE(-1, total_length);
            set_end(total_length);
        } else {
            set_end(range.cpMax);
        }
    }
}

CHARRANGE Range::ToCHARRANGE() const
{
    CHARRANGE r = { -1, -1 };
    if (!IsValid())
        return r;

    const LONG kLONGMax = std::numeric_limits<LONG>::max();
    CHECK_LE(static_cast<LONG>(start()), kLONGMax);
    CHECK_LE(static_cast<LONG>(end()), kLONGMax);
    r.cpMin = static_cast<LONG>(start());
    r.cpMax = static_cast<LONG>(end());
    return r;
}

} // namespace gfx
