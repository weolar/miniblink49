// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/trace_event.h"
#include "base/logging.h"
#include "skia/ext/analysis_canvas.h"
#include "third_party/skia/include/core/SkDraw.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "third_party/skia/include/core/SkShader.h"
#include "third_party/skia/src/core/SkRasterClip.h"
#include "ui/gfx/rect_conversions.h"

namespace {

const int kNoLayer = -1;

bool IsSolidColorPaint(const SkPaint& paint) {
  SkXfermode::Mode xfermode;

  // getXfermode can return a NULL, but that is handled
  // gracefully by AsMode (NULL turns into kSrcOver mode).
  SkXfermode::AsMode(paint.getXfermode(), &xfermode);

  // Paint is solid color if the following holds:
  // - Alpha is 1.0, style is fill, and there are no special effects
  // - Xfer mode is either kSrc or kSrcOver (kSrcOver is equivalent
  //   to kSrc if source alpha is 1.0, which is already checked).
  return (paint.getAlpha() == 255 &&
          !paint.getShader() &&
          !paint.getLooper() &&
          !paint.getMaskFilter() &&
          !paint.getColorFilter() &&
          !paint.getImageFilter() &&
          paint.getStyle() == SkPaint::kFill_Style &&
          (xfermode == SkXfermode::kSrc_Mode ||
           xfermode == SkXfermode::kSrcOver_Mode));
}

// Returns true if the specified drawn_rect will cover the entire canvas, and
// that the canvas is not clipped (i.e. it covers ALL of the canvas).
bool IsFullQuad(SkCanvas* canvas, const SkRect& drawn_rect) {
  if (!canvas->isClipRect())
    return false;

  SkIRect clip_irect;
  canvas->getClipDeviceBounds(&clip_irect);
  // if the clip is smaller than the canvas, we're partly clipped, so abort.
  if (!clip_irect.contains(SkIRect::MakeSize(canvas->getDeviceSize())))
    return false;

  const SkMatrix& matrix = canvas->getTotalMatrix();
  // If the transform results in a non-axis aligned
  // rect, then be conservative and return false.
  if (!matrix.rectStaysRect())
    return false;

  SkRect device_rect;
  matrix.mapRect(&device_rect, drawn_rect);
  SkRect clip_rect;
  clip_rect.set(clip_irect);
  return device_rect.contains(clip_rect);
}

} // namespace

namespace skia {

void AnalysisCanvas::SetForceNotSolid(bool flag) {
  is_forced_not_solid_ = flag;
  if (is_forced_not_solid_)
    is_solid_color_ = false;
}

void AnalysisCanvas::SetForceNotTransparent(bool flag) {
  is_forced_not_transparent_ = flag;
  if (is_forced_not_transparent_)
    is_transparent_ = false;
}

void AnalysisCanvas::clear(SkColor color) {
  is_transparent_ = (!is_forced_not_transparent_ && SkColorGetA(color) == 0);
  has_text_ = false;

  if (!is_forced_not_solid_ && SkColorGetA(color) == 255) {
    is_solid_color_ = true;
    color_ = color;
  } else {
    is_solid_color_ = false;
  }
}

void AnalysisCanvas::drawPaint(const SkPaint& paint) {
  // This check is in SkCanvas::drawPaint(), and some of our unittests rely on
  // on this, so we reproduce it here.
  if (isClipEmpty())
    return;

  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawPoints(SkCanvas::PointMode mode,
                                size_t count,
                                const SkPoint points[],
                                const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawRect(const SkRect& rect, const SkPaint& paint) {
  // This recreates the early-exit logic in SkCanvas.cpp, which aborts early
  // if the paint will "draw nothing".
  if (paint.nothingToDraw())
    return;

  bool does_cover_canvas = IsFullQuad(this, rect);

  SkXfermode::Mode xfermode;
  SkXfermode::AsMode(paint.getXfermode(), &xfermode);

  // This canvas will become transparent if the following holds:
  // - The quad is a full tile quad
  // - We're not in "forced not transparent" mode
  // - Transfer mode is clear (0 color, 0 alpha)
  //
  // If the paint alpha is not 0, or if the transfrer mode is
  // not src, then this canvas will not be transparent.
  //
  // In all other cases, we keep the current transparent value
  if (does_cover_canvas &&
      !is_forced_not_transparent_ &&
      xfermode == SkXfermode::kClear_Mode) {
    is_transparent_ = true;
    has_text_ = false;
  } else if (paint.getAlpha() != 0 || xfermode != SkXfermode::kSrc_Mode) {
    is_transparent_ = false;
  }

  // This bitmap is solid if and only if the following holds.
  // Note that this might be overly conservative:
  // - We're not in "forced not solid" mode
  // - Paint is solid color
  // - The quad is a full tile quad
  if (!is_forced_not_solid_ && IsSolidColorPaint(paint) && does_cover_canvas) {
    is_solid_color_ = true;
    color_ = paint.getColor();
    has_text_ = false;
  } else {
    is_solid_color_ = false;
  }
}

void AnalysisCanvas::drawOval(const SkRect& oval, const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawRRect(const SkRRect& rr, const SkPaint& paint) {
  // This should add the SkRRect to an SkPath, and call
  // drawPath, but since drawPath ignores the SkPath, just
  // do the same work here.
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawPath(const SkPath& path, const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawBitmap(const SkBitmap& bitmap,
                                SkScalar left,
                                SkScalar top,
                                const SkPaint*) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawBitmapRectToRect(const SkBitmap&,
                                          const SkRect* src,
                                          const SkRect& dst,
                                          const SkPaint* paint,
                                          DrawBitmapRectFlags flags) {
  // Call drawRect to determine transparency,
  // but reset solid color to false.
  SkPaint tmpPaint;
  if (!paint)
    paint = &tmpPaint;
  drawRect(dst, *paint);
  is_solid_color_ = false;
}

void AnalysisCanvas::drawBitmapMatrix(const SkBitmap& bitmap,
                                      const SkMatrix& matrix,
                                      const SkPaint* paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawBitmapNine(const SkBitmap& bitmap,
                                    const SkIRect& center,
                                    const SkRect& dst,
                                    const SkPaint* paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawSprite(const SkBitmap& bitmap,
                                int left,
                                int top,
                                const SkPaint* paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::onDrawText(const void* text,
                                size_t len,
                                SkScalar x,
                                SkScalar y,
                                const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
  has_text_ = true;
}

void AnalysisCanvas::onDrawPosText(const void* text,
                                   size_t byteLength,
                                   const SkPoint pos[],
                                   const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
  has_text_ = true;
}

void AnalysisCanvas::onDrawPosTextH(const void* text,
                                    size_t byteLength,
                                    const SkScalar xpos[],
                                    SkScalar constY,
                                    const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
  has_text_ = true;
}

void AnalysisCanvas::onDrawTextOnPath(const void* text,
                                      size_t len,
                                      const SkPath& path,
                                      const SkMatrix* matrix,
                                      const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
  has_text_ = true;
}

void AnalysisCanvas::onDrawDRRect(const SkRRect& outer,
                                  const SkRRect& inner,
                                  const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

void AnalysisCanvas::drawVertices(SkCanvas::VertexMode,
                                  int vertex_count,
                                  const SkPoint verts[],
                                  const SkPoint texs[],
                                  const SkColor colors[],
                                  SkXfermode* xmode,
                                  const uint16_t indices[],
                                  int index_count,
                                  const SkPaint& paint) {
  is_solid_color_ = false;
  is_transparent_ = false;
}

// Needed for now, since SkCanvas requires a bitmap, even if it is not backed
// by any pixels
static SkBitmap MakeEmptyBitmap(int width, int height) {
  SkBitmap bitmap;
  bitmap.setInfo(SkImageInfo::MakeUnknown(width, height));
  return bitmap;
}

AnalysisCanvas::AnalysisCanvas(int width, int height)
    : INHERITED(MakeEmptyBitmap(width, height)),
      saved_stack_size_(0),
      force_not_solid_stack_level_(kNoLayer),
      force_not_transparent_stack_level_(kNoLayer),
      is_forced_not_solid_(false),
      is_forced_not_transparent_(false),
      is_solid_color_(true),
      is_transparent_(true),
      has_text_(false) {}

AnalysisCanvas::~AnalysisCanvas() {}

bool AnalysisCanvas::GetColorIfSolid(SkColor* color) const {
  if (is_transparent_) {
    *color = SK_ColorTRANSPARENT;
    return true;
  }
  if (is_solid_color_) {
    *color = color_;
    return true;
  }
  return false;
}

bool AnalysisCanvas::HasText() const { return has_text_; }

bool AnalysisCanvas::abortDrawing() {
  // Early out as soon as we have detected that the tile has text.
  return HasText();
}

void AnalysisCanvas::onClipRect(const SkRect& rect, SkRegion::Op op, 
                                ClipEdgeStyle edge_style) {

  INHERITED::onClipRect(rect, op, edge_style);
}

void AnalysisCanvas::onClipPath(const SkPath& path, SkRegion::Op op,
                                ClipEdgeStyle edge_style) {
  // clipPaths can make our calls to IsFullQuad invalid (ie have false
  // positives). As a precaution, force the setting to be non-solid
  // and non-transparent until we pop this
  if (force_not_solid_stack_level_ == kNoLayer) {
    force_not_solid_stack_level_ = saved_stack_size_;
    SetForceNotSolid(true);
  }
  if (force_not_transparent_stack_level_ == kNoLayer) {
    force_not_transparent_stack_level_ = saved_stack_size_;
    SetForceNotTransparent(true);
  }

  INHERITED::onClipRect(path.getBounds(), op, edge_style);
}

void AnalysisCanvas::onClipRRect(const SkRRect& rrect,
                                 SkRegion::Op op,
                                 ClipEdgeStyle edge_style) {
  // clipRRect can make our calls to IsFullQuad invalid (ie have false
  // positives). As a precaution, force the setting to be non-solid
  // and non-transparent until we pop this
  if (force_not_solid_stack_level_ == kNoLayer) {
    force_not_solid_stack_level_ = saved_stack_size_;
    SetForceNotSolid(true);
  }
  if (force_not_transparent_stack_level_ == kNoLayer) {
    force_not_transparent_stack_level_ = saved_stack_size_;
    SetForceNotTransparent(true);
  }

  INHERITED::onClipRect(rrect.getBounds(), op, edge_style);
}

void AnalysisCanvas::willSave() {
  ++saved_stack_size_;
  INHERITED::willSave();
}

SkCanvas::SaveLayerStrategy AnalysisCanvas::willSaveLayer(
    const SkRect* bounds,
    const SkPaint* paint,
    SkCanvas::SaveFlags flags) {

  ++saved_stack_size_;

  SkIRect canvas_ibounds = SkIRect::MakeSize(this->getDeviceSize());
  SkRect canvas_bounds;
  canvas_bounds.set(canvas_ibounds);

  // If after we draw to the saved layer, we have to blend with the current
  // layer, then we can conservatively say that the canvas will not be of
  // solid color.
  if ((paint && !IsSolidColorPaint(*paint)) ||
      (bounds && !bounds->contains(canvas_bounds))) {
    if (force_not_solid_stack_level_ == kNoLayer) {
      force_not_solid_stack_level_ = saved_stack_size_;
      SetForceNotSolid(true);
    }
  }

  // If after we draw to the save layer, we have to blend with the current
  // layer using any part of the current layer's alpha, then we can
  // conservatively say that the canvas will not be transparent.
  SkXfermode::Mode xfermode = SkXfermode::kSrc_Mode;
  if (paint)
    SkXfermode::AsMode(paint->getXfermode(), &xfermode);
  if (xfermode != SkXfermode::kSrc_Mode) {
    if (force_not_transparent_stack_level_ == kNoLayer) {
      force_not_transparent_stack_level_ = saved_stack_size_;
      SetForceNotTransparent(true);
    }
  }

  INHERITED::willSaveLayer(bounds, paint, flags);
  // Actually saving a layer here could cause a new bitmap to be created
  // and real rendering to occur.
  return kNoLayer_SaveLayerStrategy;
}

void AnalysisCanvas::willRestore() {
  DCHECK(saved_stack_size_);
  if (saved_stack_size_) {
    --saved_stack_size_;
    if (saved_stack_size_ < force_not_solid_stack_level_) {
      SetForceNotSolid(false);
      force_not_solid_stack_level_ = kNoLayer;
    }
    if (saved_stack_size_ < force_not_transparent_stack_level_) {
      SetForceNotTransparent(false);
      force_not_transparent_stack_level_ = kNoLayer;
    }
  }

  INHERITED::willRestore();
}

}  // namespace skia


