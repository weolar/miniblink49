// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKIA_EXT_PAINT_SIMPLIFIER_H
#define SKIA_EXT_PAINT_SIMPLIFIER_H

//#include "base/values.h"
#include "base/compiler_specific.h"
#include "third_party/skia/include/core/SkDrawFilter.h"

class SkPaint;

namespace skia {

/*
  When installed on a SkCanvas, reduces the quality of all draws
  to that canvas. This improves rasterization speed during flings.
  We turn off blurs, filters, and antialiasing *except for* text.
*/
class SK_API PaintSimplifier : public SkDrawFilter {
 public:
  PaintSimplifier();
  virtual ~PaintSimplifier();
  virtual bool filter(SkPaint*, Type) OVERRIDE;

 private:
  typedef SkDrawFilter INHERITED;
};

}  // namespace skia

#endif  // SKIA_EXT_PAINT_SIMPLIFIER_H

