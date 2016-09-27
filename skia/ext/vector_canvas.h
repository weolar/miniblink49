// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKIA_EXT_VECTOR_CANVAS_H_
#define SKIA_EXT_VECTOR_CANVAS_H_

#include "base/compiler_specific.h"
#include "skia/ext/platform_canvas.h"

// This was a specialization of PlatformCanvas, but all necessary functionality
// has been subsumed by just SkCanvas and a specialized device (PDF or EMF).
// Future evolution goal is to replace this notion (canvas+device) with
// an updated version of SkDocument, which will have explicit APIs for margins.
// At that point, this class (and header) will be removed entirely.

namespace skia {
    typedef PlatformCanvas VectorCanvas;
}  // namespace skia

#endif  // SKIA_EXT_VECTOR_CANVAS_H_

