// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PointerProperties_h
#define PointerProperties_h

namespace blink {

// Bit field values indicating available pointer types.
enum PointerType {
    PointerTypeNone = 1 << 0,
    PointerTypeCoarse = 1 << 1,
    PointerTypeFine = 1 << 2
};

// Bit field values indicating available hover types.
enum HoverType {
    HoverTypeNone = 1 << 0,
    // Indicates that the primary pointing system can hover, but it requires
    // a significant action on the user's part. e.g. hover on "long press".
    HoverTypeOnDemand = 1 << 1,
    HoverTypeHover = 1 << 2
};

}

#endif
