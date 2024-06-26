// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_BSP_COMPARE_RESULT_H_
#define CC_OUTPUT_BSP_COMPARE_RESULT_H_

namespace cc {

enum BspCompareResult {
    BSP_FRONT,
    BSP_BACK,
    BSP_SPLIT,
    BSP_COPLANAR_FRONT,
    BSP_COPLANAR_BACK,
    BSP_COPLANAR,
};

} // namespace cc

#endif // CC_OUTPUT_BSP_COMPARE_RESULT_H_
