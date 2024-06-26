// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_SELECTION_H_
#define CC_INPUT_SELECTION_H_

#include "cc/base/cc_export.h"

namespace cc {

template <typename BoundType>
struct CC_EXPORT Selection {
    Selection()
        : is_editable(false)
        , is_empty_text_form_control(false)
    {
    }
    ~Selection() { }

    BoundType start, end;
    bool is_editable;
    bool is_empty_text_form_control;
};

template <typename BoundType>
inline bool operator==(const Selection<BoundType>& lhs,
    const Selection<BoundType>& rhs)
{
    return lhs.start == rhs.start && lhs.end == rhs.end && lhs.is_editable == rhs.is_editable && lhs.is_empty_text_form_control == rhs.is_empty_text_form_control;
}

template <typename BoundType>
inline bool operator!=(const Selection<BoundType>& lhs,
    const Selection<BoundType>& rhs)
{
    return !(lhs == rhs);
}

} // namespace cc

#endif // CC_INPUT_SELECTION_H_
