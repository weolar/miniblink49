// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SCHEDULER_DRAW_RESULT_H_
#define CC_SCHEDULER_DRAW_RESULT_H_

namespace cc {

enum DrawResult {
    INVALID_RESULT,
    DRAW_SUCCESS,
    DRAW_ABORTED_CHECKERBOARD_ANIMATIONS,
    DRAW_ABORTED_MISSING_HIGH_RES_CONTENT,
    DRAW_ABORTED_CONTEXT_LOST,
    DRAW_ABORTED_CANT_DRAW,
};

} // namespace cc

#endif // CC_SCHEDULER_DRAW_RESULT_H_
