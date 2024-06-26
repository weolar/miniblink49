// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/selection_model.h"

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"

namespace gfx {

SelectionModel::SelectionModel()
    : selection_(0)
    , caret_affinity_(CURSOR_BACKWARD)
{
}

SelectionModel::SelectionModel(size_t position, LogicalCursorDirection affinity)
    : selection_(position)
    , caret_affinity_(affinity)
{
}

SelectionModel::SelectionModel(const Range& selection,
    LogicalCursorDirection affinity)
    : selection_(selection)
    , caret_affinity_(affinity)
{
}

bool SelectionModel::operator==(const SelectionModel& sel) const
{
    return selection_ == sel.selection() && caret_affinity_ == sel.caret_affinity();
}

std::string SelectionModel::ToString() const
{
    std::string str = "{";
    if (selection().is_empty())
        base::StringAppendF(&str, "%" PRIuS, caret_pos());
    else
        str += selection().ToString();
    const bool backward = caret_affinity() == CURSOR_BACKWARD;
    return str + (backward ? ",BACKWARD}" : ",FORWARD}");
}

} // namespace gfx
