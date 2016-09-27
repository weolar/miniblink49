// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKIA_EXT_VECTOR_PLATFORM_DEVICE_EMF_WIN_H_
#define SKIA_EXT_VECTOR_PLATFORM_DEVICE_EMF_WIN_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "skia/ext/platform_device.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkRegion.h"

namespace skia {

// A device is basically a wrapper around SkBitmap that provides a surface for
// SkCanvas to draw into. This specific device is not not backed by a surface
// and is thus unreadable. This is because the backend is completely vectorial.
// This device is a simple wrapper over a Windows device context (HDC) handle.
// TODO(robertphillips): Once Skia's SkBaseDevice is refactored to remove
// the bitmap-specific entry points, this class should derive from it.
class VectorPlatformDeviceEmf : public SkBitmapDevice, public PlatformDevice {
 public:
  SK_API static SkBaseDevice* CreateDevice(int width, int height, bool isOpaque,
                                           HANDLE shared_section);

  // Factory function. The DC is kept as the output context.
  static SkBaseDevice* create(HDC dc, int width, int height);

  VectorPlatformDeviceEmf(HDC dc, const SkBitmap& bitmap);
  virtual ~VectorPlatformDeviceEmf();

  // PlatformDevice methods
  virtual PlatformSurface BeginPlatformPaint() OVERRIDE;
  virtual void DrawToNativeContext(HDC dc, int x, int y,
                                   const RECT* src_rect) OVERRIDE;
  // SkBaseDevice methods.
  virtual void drawPaint(const SkDraw& draw, const SkPaint& paint) OVERRIDE;
  virtual void drawPoints(const SkDraw& draw, SkCanvas::PointMode mode,
                          size_t count, const SkPoint[],
                          const SkPaint& paint) OVERRIDE;
  virtual void drawRect(const SkDraw& draw, const SkRect& r,
                        const SkPaint& paint) OVERRIDE;
  virtual void drawRRect(const SkDraw&, const SkRRect& rr,
                         const SkPaint& paint) OVERRIDE;
  virtual void drawPath(const SkDraw& draw, const SkPath& path,
                        const SkPaint& paint,
                        const SkMatrix* prePathMatrix = NULL,
                        bool pathIsMutable = false) OVERRIDE;
  virtual void drawBitmapRect(const SkDraw& draw, const SkBitmap& bitmap,
                              const SkRect* src, const SkRect& dst,
                              const SkPaint& paint,
                              SkCanvas::DrawBitmapRectFlags flags) SK_OVERRIDE;
  virtual void drawBitmap(const SkDraw& draw, const SkBitmap& bitmap,
                          const SkMatrix& matrix,
                          const SkPaint& paint) OVERRIDE;
  virtual void drawSprite(const SkDraw& draw, const SkBitmap& bitmap,
                          int x, int y, const SkPaint& paint) OVERRIDE;
  virtual void drawText(const SkDraw& draw, const void* text, size_t len,
                        SkScalar x, SkScalar y, const SkPaint& paint) OVERRIDE;
  virtual void drawPosText(const SkDraw& draw, const void* text, size_t len,
                           const SkScalar pos[], SkScalar constY,
                           int scalarsPerPos, const SkPaint& paint) OVERRIDE;
  virtual void drawTextOnPath(const SkDraw& draw, const void* text, size_t len,
                              const SkPath& path, const SkMatrix* matrix,
                              const SkPaint& paint) OVERRIDE;
  virtual void drawVertices(const SkDraw& draw, SkCanvas::VertexMode,
                            int vertexCount,
                            const SkPoint verts[], const SkPoint texs[],
                            const SkColor colors[], SkXfermode* xmode,
                            const uint16_t indices[], int indexCount,
                            const SkPaint& paint) OVERRIDE;
  virtual void drawDevice(const SkDraw& draw, SkBaseDevice*, int x, int y,
                          const SkPaint&) OVERRIDE;

  virtual void setMatrixClip(const SkMatrix& transform, const SkRegion& region,
                             const SkClipStack&) OVERRIDE;

  void LoadClipRegion();

 protected:
#ifdef SK_SUPPORT_LEGACY_COMPATIBLEDEVICE_CONFIG
  virtual SkBaseDevice* onCreateCompatibleDevice(SkBitmap::Config, int width,
                                                 int height, bool isOpaque,
                                                 Usage usage) OVERRIDE;
#endif
  virtual SkBaseDevice* onCreateDevice(const SkImageInfo& info,
                                       Usage usage) OVERRIDE;

 private:
  // Applies the SkPaint's painting properties in the current GDI context, if
  // possible. If GDI can't support all paint's properties, returns false. It
  // doesn't execute the "commands" in SkPaint.
  bool ApplyPaint(const SkPaint& paint);

  // Selects a new object in the device context. It can be a pen, a brush, a
  // clipping region, a bitmap or a font. Returns the old selected object.
  HGDIOBJ SelectObject(HGDIOBJ object);

  // Creates a brush according to SkPaint's properties.
  bool CreateBrush(bool use_brush, const SkPaint& paint);

  // Creates a pen according to SkPaint's properties.
  bool CreatePen(bool use_pen, const SkPaint& paint);

  // Restores back the previous objects (pen, brush, etc) after a paint command.
  void Cleanup();

  // Creates a brush according to SkPaint's properties.
  bool CreateBrush(bool use_brush, COLORREF color);

  // Creates a pen according to SkPaint's properties.
  bool CreatePen(bool use_pen, COLORREF color, int stroke_width,
                 float stroke_miter, DWORD pen_style);

  // Draws a bitmap in the the device, using the currently loaded matrix.
  void InternalDrawBitmap(const SkBitmap& bitmap, int x, int y,
                          const SkPaint& paint);

  // The Windows Device Context handle. It is the backend used with GDI drawing.
  // This backend is write-only and vectorial.
  HDC hdc_;

  // Translation assigned to the DC: we need to keep track of this separately
  // so it can be updated even if the DC isn't created yet.
  SkMatrix transform_;

  // The current clipping
  SkRegion clip_region_;

  // Previously selected brush before the current drawing.
  HGDIOBJ previous_brush_;

  // Previously selected pen before the current drawing.
  HGDIOBJ previous_pen_;

  DISALLOW_COPY_AND_ASSIGN(VectorPlatformDeviceEmf);
};

typedef void (*SkiaEnsureTypefaceCharactersAccessible)
    (const LOGFONT& font, const wchar_t* text, unsigned int text_length);

SK_API void SetSkiaEnsureTypefaceCharactersAccessible(
    SkiaEnsureTypefaceCharactersAccessible func);

}  // namespace skia

#endif  // SKIA_EXT_VECTOR_PLATFORM_DEVICE_EMF_WIN_H_
