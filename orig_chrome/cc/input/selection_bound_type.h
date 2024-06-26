// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_SELECTION_BOUND_TYPE_H_
#define CC_INPUT_SELECTION_BOUND_TYPE_H_

namespace cc {

enum SelectionBoundType {
    SELECTION_BOUND_LEFT,
    SELECTION_BOUND_RIGHT,
    SELECTION_BOUND_CENTER,
    SELECTION_BOUND_EMPTY,
    SELECTION_BOUND_TYPE_LAST = SELECTION_BOUND_EMPTY
};

} // namespace cc

#endif // CC_INPUT_SELECTION_BOUND_TYPE_H_
