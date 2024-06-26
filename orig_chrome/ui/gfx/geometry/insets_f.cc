// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/geometry/insets_f.h"

#include "base/strings/stringprintf.h"

namespace gfx {

InsetsF::InsetsF()
    : InsetsF(0.f, 0.f, 0.f, 0.f)
{
}

InsetsF::InsetsF(float top, float left, float bottom, float right)
    : top_(top)
    , left_(left)
    , bottom_(bottom)
    , right_(right)
{
}

InsetsF::~InsetsF() { }

std::string InsetsF::ToString() const
{
    // Print members in the same order of the constructor parameters.
    return base::StringPrintf("%f,%f,%f,%f", top(), left(), bottom(), right());
}

} // namespace gfx
