// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/containers/hash_tables.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "skia/ext/benchmarking_canvas.h"
#include "third_party/skia/include/utils/SkProxyCanvas.h"

namespace skia {

class AutoStamper {
public:
  AutoStamper(TimingCanvas* timing_canvas);
  ~AutoStamper();

private:
  TimingCanvas* timing_canvas_;
  base::TimeTicks start_ticks_;
};

class TimingCanvas : public SkProxyCanvas {
public:
  TimingCanvas(int width, int height, const BenchmarkingCanvas* track_canvas)
      : tracking_canvas_(track_canvas) {
    canvas_ = skia::AdoptRef(SkCanvas::NewRasterN32(width, height));

    setProxy(canvas_.get());
  }

  virtual ~TimingCanvas() {
  }

  double GetTime(size_t index) {
    TimingsMap::const_iterator timing_info = timings_map_.find(index);
    return timing_info != timings_map_.end()
        ? timing_info->second.InMillisecondsF()
        : 0.0;
  }

  // SkCanvas overrides.
  virtual void willSave() OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::willSave();
  }

  virtual SaveLayerStrategy willSaveLayer(const SkRect* bounds,
                                          const SkPaint* paint,
                                          SaveFlags flags) OVERRIDE {
    AutoStamper stamper(this);
    return SkProxyCanvas::willSaveLayer(bounds, paint, flags);
  }

  virtual void willRestore() OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::willRestore();
  }

  virtual void drawPaint(const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawPaint(paint);
  }

  virtual void drawPoints(PointMode mode, size_t count, const SkPoint pts[],
                          const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawPoints(mode, count, pts, paint);
  }

  virtual void drawOval(const SkRect& rect, const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawOval(rect, paint);
  }

  virtual void drawRect(const SkRect& rect, const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawRect(rect, paint);
  }

  virtual void drawRRect(const SkRRect& rrect, const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawRRect(rrect, paint);
  }

  virtual void drawPath(const SkPath& path, const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawPath(path, paint);
  }

  virtual void drawBitmap(const SkBitmap& bitmap, SkScalar left, SkScalar top,
                          const SkPaint* paint = NULL) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawBitmap(bitmap, left, top, paint);
  }

  virtual void drawBitmapRectToRect(const SkBitmap& bitmap, const SkRect* src,
                                    const SkRect& dst,
                                    const SkPaint* paint,
                                    DrawBitmapRectFlags flags) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawBitmapRectToRect(bitmap, src, dst, paint, flags);
  }

  virtual void drawBitmapMatrix(const SkBitmap& bitmap, const SkMatrix& m,
                                const SkPaint* paint = NULL) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawBitmapMatrix(bitmap, m, paint);
  }

  virtual void drawSprite(const SkBitmap& bitmap, int left, int top,
                          const SkPaint* paint = NULL) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawSprite(bitmap, left, top, paint);
  }

  virtual void drawVertices(VertexMode vmode, int vertexCount,
                            const SkPoint vertices[], const SkPoint texs[],
                            const SkColor colors[], SkXfermode* xmode,
                            const uint16_t indices[], int indexCount,
                            const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawVertices(vmode, vertexCount, vertices, texs, colors,
                                xmode, indices, indexCount, paint);
  }

  virtual void drawData(const void* data, size_t length) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::drawData(data, length);
  }

protected:
  virtual void onDrawText(const void* text, size_t byteLength, SkScalar x,
                          SkScalar y, const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onDrawText(text, byteLength, x, y, paint);
  }

  virtual void onDrawPosText(const void* text, size_t byteLength,
                             const SkPoint pos[],
                             const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onDrawPosText(text, byteLength, pos, paint);
  }

  virtual void onDrawPosTextH(const void* text, size_t byteLength,
                              const SkScalar xpos[], SkScalar constY,
                              const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onDrawPosTextH(text, byteLength, xpos, constY, paint);
  }

  virtual void onDrawTextOnPath(const void* text, size_t byteLength,
                                const SkPath& path, const SkMatrix* matrix,
                                const SkPaint& paint) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onDrawTextOnPath(text, byteLength, path, matrix, paint);
  }

  virtual void onClipRect(const SkRect& rect, SkRegion::Op op,
                          ClipEdgeStyle edge_style) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onClipRect(rect, op, edge_style);
  }

  virtual void onClipRRect(const SkRRect& rrect, SkRegion::Op op,
                          ClipEdgeStyle edge_style) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onClipRRect(rrect, op, edge_style);
  }

  virtual void onClipPath(const SkPath& path, SkRegion::Op op,
                          ClipEdgeStyle edge_style) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onClipPath(path, op, edge_style);
  }

  virtual void onClipRegion(const SkRegion& region,
                            SkRegion::Op op) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onClipRegion(region, op);
  }

  virtual void onDrawPicture(const SkPicture* picture) OVERRIDE {
    AutoStamper stamper(this);
    SkProxyCanvas::onDrawPicture(picture);
  }

private:
  typedef base::hash_map<size_t, base::TimeDelta> TimingsMap;
  TimingsMap timings_map_;

  skia::RefPtr<SkCanvas> canvas_;

  friend class AutoStamper;
  const BenchmarkingCanvas* tracking_canvas_;
};

AutoStamper::AutoStamper(TimingCanvas *timing_canvas)
    : timing_canvas_(timing_canvas) {
  start_ticks_ = base::TimeTicks::HighResNow();
}

AutoStamper::~AutoStamper() {
  base::TimeDelta delta = base::TimeTicks::HighResNow() - start_ticks_;
  int command_index = timing_canvas_->tracking_canvas_->CommandCount() - 1;
  DCHECK_GE(command_index, 0);
  timing_canvas_->timings_map_[command_index] = delta;
}

BenchmarkingCanvas::BenchmarkingCanvas(int width, int height)
    : SkNWayCanvas(width, height) {
  debug_canvas_ = skia::AdoptRef(SkNEW_ARGS(SkDebugCanvas, (width, height)));
  timing_canvas_ = skia::AdoptRef(SkNEW_ARGS(TimingCanvas, (width, height, this)));

  addCanvas(debug_canvas_.get());
  addCanvas(timing_canvas_.get());
}

BenchmarkingCanvas::~BenchmarkingCanvas() {
  removeAll();
}

size_t BenchmarkingCanvas::CommandCount() const {
  return debug_canvas_->getSize();
}

SkDrawCommand* BenchmarkingCanvas::GetCommand(size_t index) {
  DCHECK_LT(index, static_cast<size_t>(debug_canvas_->getSize()));
  return debug_canvas_->getDrawCommandAt(index);
}

double BenchmarkingCanvas::GetTime(size_t index) {
  DCHECK_LT(index,  static_cast<size_t>(debug_canvas_->getSize()));
  return timing_canvas_->GetTime(index);
}

} // namespace skia
