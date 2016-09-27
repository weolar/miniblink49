// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKIA_EXT_BENCHMARKING_CANVAS_H_
#define SKIA_EXT_BENCHMARKING_CANVAS_H_

#include "base/compiler_specific.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/utils/SkNWayCanvas.h"
#include "third_party/skia/src/utils/debugger/SkDebugCanvas.h"

namespace skia {

class TimingCanvas;

class SK_API BenchmarkingCanvas : public SkNWayCanvas {
public:
  BenchmarkingCanvas(int width, int height);
  virtual ~BenchmarkingCanvas();

  // Returns the number of draw commands executed on this canvas.
  size_t CommandCount() const;

  // Get draw command info for a given index.
  SkDrawCommand* GetCommand(size_t index);

  // Return the recorded render time (milliseconds) for a draw command index.
  double GetTime(size_t index);

private:
  // In order to avoid introducing a Skia version dependency, this
  // implementation dispatches draw commands in lock-step to two distinct
  // canvases:
  //   * a SkDebugCanvas used for gathering command info and tracking
  //     the current command index
  //   * a SkiaTimingCanvas used for measuring raster paint times (and relying
  //     on the former for tracking the current command index).
  //
  // This way, if the SkCanvas API is extended, we don't need to worry about
  // updating content::SkiaTimingCanvas to accurately override all new methods
  // (to avoid timing info indices from getting out of sync), as SkDebugCanvas
  // already does that for us.

  skia::RefPtr<SkDebugCanvas> debug_canvas_;
  skia::RefPtr<TimingCanvas> timing_canvas_;
};

}
#endif // SKIA_EXT_BENCHMARKING_CANVAS_H
