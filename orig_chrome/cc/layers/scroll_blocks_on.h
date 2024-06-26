// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_SCROLL_BLOCKS_ON_H_
#define CC_LAYERS_SCROLL_BLOCKS_ON_H_

enum ScrollBlocksOn {
    SCROLL_BLOCKS_ON_NONE = 0x0,
    SCROLL_BLOCKS_ON_START_TOUCH = 0x1,
    SCROLL_BLOCKS_ON_WHEEL_EVENT = 0x2,
    SCROLL_BLOCKS_ON_SCROLL_EVENT = 0x4,
    SCROLL_BLOCKS_ON_MAX = SCROLL_BLOCKS_ON_START_TOUCH | SCROLL_BLOCKS_ON_WHEEL_EVENT | SCROLL_BLOCKS_ON_SCROLL_EVENT
};

inline ScrollBlocksOn operator|(ScrollBlocksOn a, ScrollBlocksOn b)
{
    return ScrollBlocksOn(static_cast<int>(a) | static_cast<int>(b));
}

inline ScrollBlocksOn& operator|=(ScrollBlocksOn& a, ScrollBlocksOn b)
{
    return a = a | b;
}

#endif // CC_LAYERS_SCROLL_BLOCKS_ON_H_
