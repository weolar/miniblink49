// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebScrollBlocksOn_h
#define WebScrollBlocksOn_h

#include "WebCommon.h"

namespace blink {

enum WebScrollBlocksOn {
    WebScrollBlocksOnNone = 0x0,
    WebScrollBlocksOnStartTouch = 0x1,
    WebScrollBlocksOnWheelEvent = 0x2,
    WebScrollBlocksOnScrollEvent = 0x4,
};
inline WebScrollBlocksOn operator| (WebScrollBlocksOn a, WebScrollBlocksOn b) { return WebScrollBlocksOn(int(a) | int(b)); }
inline WebScrollBlocksOn& operator|= (WebScrollBlocksOn& a, WebScrollBlocksOn b) { return a = a | b; }
inline WebScrollBlocksOn operator& (WebScrollBlocksOn a, WebScrollBlocksOn b) { return WebScrollBlocksOn(int(a) & int(b)); }
inline WebScrollBlocksOn& operator&= (WebScrollBlocksOn& a, WebScrollBlocksOn b) { return a = a & b; }

} // namespace blink

#endif
