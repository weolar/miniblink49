// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "skia/ext/paint_simplifier.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkShader.h"

namespace skia {
namespace {

bool PaintHasBitmap(const SkPaint &paint) {
  SkShader* shader = paint.getShader();
  if (!shader)
    return false;

  if (shader->asAGradient(NULL) == SkShader::kNone_GradientType)
    return false;

  return shader->asABitmap(NULL, NULL, NULL) != SkShader::kNone_BitmapType;
}

}  // namespace

PaintSimplifier::PaintSimplifier()
  : INHERITED() {

}

PaintSimplifier::~PaintSimplifier() {

}

bool PaintSimplifier::filter(SkPaint* paint, Type type) {
  // Bitmaps are expensive. Skip draw if type has a bitmap.
  if (type == kBitmap_Type || PaintHasBitmap(*paint))
    return false;

  // Preserve a modicum of text quality; black & white text is
  // just too blocky, even during a fling.
  if (type != kText_Type) {
    paint->setAntiAlias(false);
  }
  paint->setSubpixelText(false);
  paint->setLCDRenderText(false);

  paint->setMaskFilter(NULL);

  // Uncomment this line to shade simplified tiles pink during debugging.
  //paint->setColor(SkColorSetRGB(255, 127, 127));

  return true;
}


}  // namespace skia


