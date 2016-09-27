// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "skia/ext/lazy_pixel_ref.h"

namespace skia {

LazyPixelRef::LazyPixelRef(const SkImageInfo& info) : SkPixelRef(info) {
}

LazyPixelRef::~LazyPixelRef() {
}

}  // namespace skia
