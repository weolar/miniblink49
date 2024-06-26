// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/djb2.h"

uint32 DJB2Hash(const void* buf, size_t len, uint32 seed)
{
    const uint8* src = reinterpret_cast<const uint8*>(buf);
    uint32 hash = seed;
    for (size_t i = 0; i < len; ++i) {
        hash = hash * 33 + src[i];
    }
    return hash;
}
