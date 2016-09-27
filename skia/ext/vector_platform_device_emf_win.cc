// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "skia/ext/vector_platform_device_emf_win.h"

#include <windows.h>

#include "base/logging.h"
#include "base/strings/string16.h"
#include "skia/ext/bitmap_platform_device.h"
#include "skia/ext/skia_utils_win.h"
#include "third_party/skia/include/core/SkFontHost.h"
#include "third_party/skia/include/core/SkPathEffect.h"
#include "third_party/skia/include/core/SkTemplates.h"
#include "third_party/skia/include/core/SkUtils.h"
#include "third_party/skia/include/ports/SkTypeface_win.h"

namespace skia {

#define CHECK_FOR_NODRAW_ANNOTATION(paint) \
    do { if (paint.isNoDrawAnnotation()) { return; } } while (0)

// static
SkBaseDevice* VectorPlatformDeviceEmf::CreateDevice(
    int width, int height, bool is_opaque, HANDLE shared_section) {
  if (!is_opaque) {
    // TODO(maruel):  http://crbug.com/18382 When restoring a semi-transparent
    // layer, i.e. merging it, we need to rasterize it because GDI doesn't
    // support transparency except for AlphaBlend(). Right now, a
    // BitmapPlatformDevice is created when VectorCanvas think a saveLayers()
    // call is being done. The way to save a layer would be to create an
    // EMF-based VectorDevice and have this device registers the drawing. When
    // playing back the device into a bitmap, do it at the printer's dpi instead
    // of the layout's dpi (which is much lower).
    return BitmapPlatformDevice::Create(width, height, is_opaque,
                                        shared_section);
  }

  // TODO(maruel):  http://crbug.com/18383 Look if it would be worth to
  // increase the resolution by ~10x (any worthy factor) to increase the
  // rendering precision (think about printing) while using a relatively
  // low dpi. This happens because we receive float as input but the GDI
  // functions works with integers. The idea is to premultiply the matrix
  // with this factor and multiply each SkScalar that are passed to
  // SkScalarRound(value) as SkScalarRound(value * 10). Safari is already
  // doing the same for text rendering.
  SkASSERT(shared_section);
  SkBaseDevice* device = VectorPlatformDeviceEmf::create(
      reinterpret_cast<HDC>(shared_section), width, height);
  return device;
}

static void FillBitmapInfoHeader(int width, int height, BITMAPINFOHEADER* hdr) {
  hdr->biSize = sizeof(BITMAPINFOHEADER);
  hdr->biWidth = width;
  hdr->biHeight = -height;  // Minus means top-down bitmap.
  hdr->biPlanes = 1;
  hdr->biBitCount = 32;
  hdr->biCompression = BI_RGB;  // no compression
  hdr->biSizeImage = 0;
  hdr->biXPelsPerMeter = 1;
  hdr->biYPelsPerMeter = 1;
  hdr->biClrUsed = 0;
  hdr->biClrImportant = 0;
}

SkBaseDevice* VectorPlatformDeviceEmf::create(HDC dc, int width, int height) {
  InitializeDC(dc);

  // Link the SkBitmap to the current selected bitmap in the device context.
  SkBitmap bitmap;
  HGDIOBJ selected_bitmap = GetCurrentObject(dc, OBJ_BITMAP);
  bool succeeded = false;
  if (selected_bitmap != NULL) {
    BITMAP bitmap_data = {0};
    if (GetObject(selected_bitmap, sizeof(BITMAP), &bitmap_data) ==
        sizeof(BITMAP)) {
      // The context has a bitmap attached. Attach our SkBitmap to it.
      // Warning: If the bitmap gets unselected from the HDC,
      // VectorPlatformDeviceEmf has no way to detect this, so the HBITMAP
      // could be released while SkBitmap still has a reference to it. Be
      // cautious.
      if (width == bitmap_data.bmWidth && height == bitmap_data.bmHeight) {
        SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
        succeeded = bitmap.installPixels(info, bitmap_data.bmBits,
                                         bitmap_data.bmWidthBytes);
      }
    }
  }

  if (!succeeded)
    bitmap.setInfo(SkImageInfo::MakeUnknown(width, height));

  return new VectorPlatformDeviceEmf(dc, bitmap);
}

VectorPlatformDeviceEmf::VectorPlatformDeviceEmf(HDC dc, const SkBitmap& bitmap)
    : SkBitmapDevice(bitmap),
      hdc_(dc),
      previous_brush_(NULL),
      previous_pen_(NULL) {
  transform_.reset();
  SetPlatformDevice(this, this);
}

VectorPlatformDeviceEmf::~VectorPlatformDeviceEmf() {
  SkASSERT(previous_brush_ == NULL);
  SkASSERT(previous_pen_ == NULL);
}

HDC VectorPlatformDeviceEmf::BeginPlatformPaint() {
  return hdc_;
}

void VectorPlatformDeviceEmf::drawPaint(const SkDraw& draw,
                                        const SkPaint& paint) {
  // TODO(maruel):  Bypass the current transformation matrix.
  SkRect rect;
  rect.fLeft = 0;
  rect.fTop = 0;
  rect.fRight = SkIntToScalar(width() + 1);
  rect.fBottom = SkIntToScalar(height() + 1);
  drawRect(draw, rect, paint);
}

void VectorPlatformDeviceEmf::drawPoints(const SkDraw& draw,
                                         SkCanvas::PointMode mode,
                                         size_t count,
                                         const SkPoint pts[],
                                         const SkPaint& paint) {
  if (!count)
    return;

  if (mode == SkCanvas::kPoints_PointMode) {
    SkASSERT(false);
    return;
  }

  SkPaint tmp_paint(paint);
  tmp_paint.setStyle(SkPaint::kStroke_Style);

  // Draw a path instead.
  SkPath path;
  switch (mode) {
    case SkCanvas::kLines_PointMode:
      if (count % 2) {
        SkASSERT(false);
        return;
      }
      for (size_t i = 0; i < count / 2; ++i) {
        path.moveTo(pts[2 * i]);
        path.lineTo(pts[2 * i + 1]);
      }
      break;
    case SkCanvas::kPolygon_PointMode:
      path.moveTo(pts[0]);
      for (size_t i = 1; i < count; ++i) {
        path.lineTo(pts[i]);
      }
      break;
    default:
      SkASSERT(false);
      return;
  }
  // Draw the calculated path.
  drawPath(draw, path, tmp_paint);
}

void VectorPlatformDeviceEmf::drawRect(const SkDraw& draw,
                                       const SkRect& rect,
                                       const SkPaint& paint) {
  CHECK_FOR_NODRAW_ANNOTATION(paint);
  if (paint.getPathEffect()) {
    // Draw a path instead.
    SkPath path_orginal;
    path_orginal.addRect(rect);

    // Apply the path effect to the rect.
    SkPath path_modified;
    paint.getFillPath(path_orginal, &path_modified);

    // Removes the path effect from the temporary SkPaint object.
    SkPaint paint_no_effet(paint);
    paint_no_effet.setPathEffect(NULL);

    // Draw the calculated path.
    drawPath(draw, path_modified, paint_no_effet);
    return;
  }

  if (!ApplyPaint(paint)) {
    return;
  }
  HDC dc = BeginPlatformPaint();
  if (!Rectangle(dc, SkScalarRoundToInt(rect.fLeft),
                 SkScalarRoundToInt(rect.fTop),
                 SkScalarRoundToInt(rect.fRight),
                 SkScalarRoundToInt(rect.fBottom))) {
    SkASSERT(false);
  }
  EndPlatformPaint();
  Cleanup();
}

void VectorPlatformDeviceEmf::drawRRect(const SkDraw& draw, const SkRRect& rr,
                                        const SkPaint& paint) {
  SkPath path;
  path.addRRect(rr);
  this->drawPath(draw, path, paint, NULL, true);
}

void VectorPlatformDeviceEmf::drawPath(const SkDraw& draw,
                                       const SkPath& path,
                                       const SkPaint& paint,
                                       const SkMatrix* prePathMatrix,
                                       bool pathIsMutable) {
  CHECK_FOR_NODRAW_ANNOTATION(paint);
  if (paint.getPathEffect()) {
    // Apply the path effect forehand.
    SkPath path_modified;
    paint.getFillPath(path, &path_modified);

    // Removes the path effect from the temporary SkPaint object.
    SkPaint paint_no_effet(paint);
    paint_no_effet.setPathEffect(NULL);

    // Draw the calculated path.
    drawPath(draw, path_modified, paint_no_effet);
    return;
  }

  if (!ApplyPaint(paint)) {
    return;
  }
  HDC dc = BeginPlatformPaint();
  if (PlatformDevice::LoadPathToDC(dc, path)) {
    switch (paint.getStyle()) {
      case SkPaint::kFill_Style: {
        BOOL res = StrokeAndFillPath(dc);
        SkASSERT(res != 0);
        break;
      }
      case SkPaint::kStroke_Style: {
        BOOL res = StrokePath(dc);
        SkASSERT(res != 0);
        break;
      }
      case SkPaint::kStrokeAndFill_Style: {
        BOOL res = StrokeAndFillPath(dc);
        SkASSERT(res != 0);
        break;
      }
      default:
        SkASSERT(false);
        break;
    }
  }
  EndPlatformPaint();
  Cleanup();
}

void VectorPlatformDeviceEmf::drawBitmapRect(const SkDraw& draw,
                                             const SkBitmap& bitmap,
                                             const SkRect* src,
                                             const SkRect& dst,
                                             const SkPaint& paint,
                                             SkCanvas::DrawBitmapRectFlags flags) {
    SkMatrix    matrix;
    SkRect      bitmapBounds, tmpSrc, tmpDst;
    SkBitmap    tmpBitmap;

    bitmapBounds.isetWH(bitmap.width(), bitmap.height());

    // Compute matrix from the two rectangles
    if (src) {
        tmpSrc = *src;
    } else {
        tmpSrc = bitmapBounds;
    }
    matrix.setRectToRect(tmpSrc, dst, SkMatrix::kFill_ScaleToFit);

    const SkBitmap* bitmapPtr = &bitmap;

    // clip the tmpSrc to the bounds of the bitmap, and recompute dstRect if
    // needed (if the src was clipped). No check needed if src==null.
    if (src) {
        if (!bitmapBounds.contains(*src)) {
            if (!tmpSrc.intersect(bitmapBounds)) {
                return; // nothing to draw
            }
            // recompute dst, based on the smaller tmpSrc
            matrix.mapRect(&tmpDst, tmpSrc);
        }

        // since we may need to clamp to the borders of the src rect within
        // the bitmap, we extract a subset.
        // TODO: make sure this is handled in drawrect and remove it from here.
        SkIRect srcIR;
        tmpSrc.roundOut(&srcIR);
        if (!bitmap.extractSubset(&tmpBitmap, srcIR)) {
            return;
        }
        bitmapPtr = &tmpBitmap;

        // Since we did an extract, we need to adjust the matrix accordingly
        SkScalar dx = 0, dy = 0;
        if (srcIR.fLeft > 0) {
            dx = SkIntToScalar(srcIR.fLeft);
        }
        if (srcIR.fTop > 0) {
            dy = SkIntToScalar(srcIR.fTop);
        }
        if (dx || dy) {
            matrix.preTranslate(dx, dy);
        }
    }
    this->drawBitmap(draw, *bitmapPtr, matrix, paint);
}

void VectorPlatformDeviceEmf::drawBitmap(const SkDraw& draw,
                                         const SkBitmap& bitmap,
                                         const SkMatrix& matrix,
                                         const SkPaint& paint) {
  // Load the temporary matrix. This is what will translate, rotate and resize
  // the bitmap.
  SkMatrix actual_transform(transform_);
  actual_transform.preConcat(matrix);
  LoadTransformToDC(hdc_, actual_transform);

  InternalDrawBitmap(bitmap, 0, 0, paint);

  // Restore the original matrix.
  LoadTransformToDC(hdc_, transform_);
}

void VectorPlatformDeviceEmf::drawSprite(const SkDraw& draw,
                                         const SkBitmap& bitmap,
                                         int x, int y,
                                         const SkPaint& paint) {
  SkMatrix identity;
  identity.reset();
  LoadTransformToDC(hdc_, identity);

  InternalDrawBitmap(bitmap, x, y, paint);

  // Restore the original matrix.
  LoadTransformToDC(hdc_, transform_);
}

/////////////////////////////////////////////////////////////////////////

static bool gdiCanHandleText(const SkPaint& paint) {
  return !paint.getShader() &&
         !paint.getPathEffect() &&
         (SkPaint::kFill_Style == paint.getStyle()) &&
         (255 == paint.getAlpha());
}

class SkGDIFontSetup {
 public:
  SkGDIFontSetup() :
      fHDC(NULL),
      fNewFont(NULL),
      fSavedFont(NULL),
      fSavedTextColor(0),
      fUseGDI(false) {
    SkDEBUGCODE(fUseGDIHasBeenCalled = false;)
  }
  ~SkGDIFontSetup();

  // can only be called once
  bool useGDI(HDC hdc, const SkPaint&);

 private:
  HDC      fHDC;
  HFONT    fNewFont;
  HFONT    fSavedFont;
  COLORREF fSavedTextColor;
  bool     fUseGDI;
  SkDEBUGCODE(bool fUseGDIHasBeenCalled;)
};

bool SkGDIFontSetup::useGDI(HDC hdc, const SkPaint& paint) {
  SkASSERT(!fUseGDIHasBeenCalled);
  SkDEBUGCODE(fUseGDIHasBeenCalled = true;)

  fUseGDI = gdiCanHandleText(paint);
  if (fUseGDI) {
    fSavedTextColor = GetTextColor(hdc);
    SetTextColor(hdc, skia::SkColorToCOLORREF(paint.getColor()));

    LOGFONT lf = {0};
    SkLOGFONTFromTypeface(paint.getTypeface(), &lf);
    lf.lfHeight = -SkScalarRoundToInt(paint.getTextSize());
    fNewFont = CreateFontIndirect(&lf);
    fSavedFont = (HFONT)::SelectObject(hdc, fNewFont);
    fHDC = hdc;
  }
  return fUseGDI;
}

SkGDIFontSetup::~SkGDIFontSetup() {
  if (fUseGDI) {
    ::SelectObject(fHDC, fSavedFont);
    ::DeleteObject(fNewFont);
    SetTextColor(fHDC, fSavedTextColor);
  }
}

static SkScalar getAscent(const SkPaint& paint) {
  SkPaint::FontMetrics fm;
  paint.getFontMetrics(&fm);
  return fm.fAscent;
}

// return the options int for ExtTextOut. Only valid if the paint's text
// encoding is not UTF8 (in which case ExtTextOut can't be used).
static UINT getTextOutOptions(const SkPaint& paint) {
  if (SkPaint::kGlyphID_TextEncoding == paint.getTextEncoding()) {
    return ETO_GLYPH_INDEX;
  } else {
    SkASSERT(SkPaint::kUTF16_TextEncoding == paint.getTextEncoding());
    return 0;
  }
}

static SkiaEnsureTypefaceCharactersAccessible
    g_skia_ensure_typeface_characters_accessible = NULL;

SK_API void SetSkiaEnsureTypefaceCharactersAccessible(
    SkiaEnsureTypefaceCharactersAccessible func) {
  // This function is supposed to be called once in process life time.
  SkASSERT(g_skia_ensure_typeface_characters_accessible == NULL);
  g_skia_ensure_typeface_characters_accessible = func;
}

void EnsureTypefaceCharactersAccessible(
    const SkTypeface& typeface, const wchar_t* text, unsigned int text_length) {
  LOGFONT lf = {0};
  SkLOGFONTFromTypeface(&typeface, &lf);
  g_skia_ensure_typeface_characters_accessible(lf, text, text_length);
}

bool EnsureExtTextOut(HDC hdc, int x, int y, UINT options, const RECT * lprect,
                      LPCWSTR text, unsigned int characters, const int * lpDx,
                      SkTypeface* const typeface) {
  bool success = ExtTextOut(hdc, x, y, options, lprect, text, characters, lpDx);
  if (!success) {
    if (typeface) {
      EnsureTypefaceCharactersAccessible(*typeface,
                                         text,
                                         characters);
      success = ExtTextOut(hdc, x, y, options, lprect, text, characters, lpDx);
      if (!success) {
        LOGFONT lf = {0};
        SkLOGFONTFromTypeface(typeface, &lf);
        VLOG(1) << "SkFontHost::EnsureTypefaceCharactersAccessible FAILED for "
                << " FaceName = " << lf.lfFaceName
                << " and characters: " << base::string16(text, characters);
      }
    } else {
      VLOG(1) << "ExtTextOut FAILED for default FaceName "
              << " and characters: " << base::string16(text, characters);
    }
  }
  return success;
}

void VectorPlatformDeviceEmf::drawText(const SkDraw& draw,
                                       const void* text,
                                       size_t byteLength,
                                       SkScalar x,
                                       SkScalar y,
                                       const SkPaint& paint) {
  SkGDIFontSetup setup;
  bool useDrawPath = true;

  if (SkPaint::kUTF8_TextEncoding != paint.getTextEncoding()
      && setup.useGDI(hdc_, paint)) {
    UINT options = getTextOutOptions(paint);
    UINT count = byteLength >> 1;
    useDrawPath = !EnsureExtTextOut(hdc_, SkScalarRoundToInt(x),
        SkScalarRoundToInt(y + getAscent(paint)), options, 0,
        reinterpret_cast<const wchar_t*>(text), count, NULL,
        paint.getTypeface());
  }

  if (useDrawPath) {
    SkPath path;
    paint.getTextPath(text, byteLength, x, y, &path);
    drawPath(draw, path, paint);
  }
}

static size_t size_utf8(const char* text) {
  return SkUTF8_CountUTF8Bytes(text);
}

static size_t size_utf16(const char* text) {
  uint16_t c = *reinterpret_cast<const uint16_t*>(text);
  return SkUTF16_IsHighSurrogate(c) ? 4 : 2;
}

static size_t size_glyphid(const char* text) {
  return 2;
}

void VectorPlatformDeviceEmf::drawPosText(const SkDraw& draw,
                                          const void* text,
                                          size_t len,
                                          const SkScalar pos[],
                                          SkScalar constY,
                                          int scalarsPerPos,
                                          const SkPaint& paint) {
  SkGDIFontSetup setup;
  bool useDrawText = true;

  if (scalarsPerPos == 2 && len >= 2 &&
      SkPaint::kUTF8_TextEncoding != paint.getTextEncoding() &&
      setup.useGDI(hdc_, paint)) {
    int startX = SkScalarRoundToInt(pos[0]);
    int startY = SkScalarRoundToInt(pos[1] + getAscent(paint));
    const int count = len >> 1;
    SkAutoSTMalloc<64, INT> storage(count);
    INT* advances = storage.get();
    for (int i = 0; i < count - 1; ++i) {
      advances[i] = SkScalarRoundToInt(pos[2] - pos[0]);
      pos += 2;
    }
    advances[count - 1] = 0;
    useDrawText = !EnsureExtTextOut(hdc_, startX, startY,
        getTextOutOptions(paint), 0, reinterpret_cast<const wchar_t*>(text),
        count, advances, paint.getTypeface());
  }

  if (useDrawText) {
    size_t (*bytesPerCodePoint)(const char*);
    switch (paint.getTextEncoding()) {
    case SkPaint::kUTF8_TextEncoding:
      bytesPerCodePoint = size_utf8;
      break;
    case SkPaint::kUTF16_TextEncoding:
      bytesPerCodePoint = size_utf16;
      break;
    default:
      SkASSERT(SkPaint::kGlyphID_TextEncoding == paint.getTextEncoding());
      bytesPerCodePoint = size_glyphid;
      break;
    }

    const char* curr = reinterpret_cast<const char*>(text);
    const char* stop = curr + len;
    while (curr < stop) {
      SkScalar y = (1 == scalarsPerPos) ? constY : pos[1];
      size_t bytes = bytesPerCodePoint(curr);
      drawText(draw, curr, bytes, pos[0], y, paint);
      curr += bytes;
      pos += scalarsPerPos;
    }
  }
}

void VectorPlatformDeviceEmf::drawTextOnPath(const SkDraw& draw,
                                             const void* text,
                                             size_t len,
                                             const SkPath& path,
                                             const SkMatrix* matrix,
                                             const SkPaint& paint) {
  // This function isn't used in the code. Verify this assumption.
  SkASSERT(false);
}

void VectorPlatformDeviceEmf::drawVertices(const SkDraw& draw,
                                           SkCanvas::VertexMode vmode,
                                           int vertexCount,
                                           const SkPoint vertices[],
                                           const SkPoint texs[],
                                           const SkColor colors[],
                                           SkXfermode* xmode,
                                           const uint16_t indices[],
                                           int indexCount,
                                           const SkPaint& paint) {
  // This function isn't used in the code. Verify this assumption.
  SkASSERT(false);
}

void VectorPlatformDeviceEmf::drawDevice(const SkDraw& draw,
                                         SkBaseDevice* device,
                                         int x,
                                         int y,
                                         const SkPaint& paint) {
  // TODO(maruel):  http://b/1183870 Playback the EMF buffer at printer's dpi if
  // it is a vectorial device.
  drawSprite(draw, device->accessBitmap(false), x, y, paint);
}

bool VectorPlatformDeviceEmf::ApplyPaint(const SkPaint& paint) {
  // Note: The goal here is to transfert the SkPaint's state to the HDC's state.
  // This function does not execute the SkPaint drawing commands. These should
  // be executed in drawPaint().

  SkPaint::Style style = paint.getStyle();
  if (!paint.getAlpha())
      style = (SkPaint::Style) SkPaint::kStyleCount;

  switch (style) {
    case SkPaint::kFill_Style:
      if (!CreateBrush(true, paint) ||
          !CreatePen(false, paint))
        return false;
      break;
    case SkPaint::kStroke_Style:
      if (!CreateBrush(false, paint) ||
          !CreatePen(true, paint))
        return false;
      break;
    case SkPaint::kStrokeAndFill_Style:
      if (!CreateBrush(true, paint) ||
          !CreatePen(true, paint))
        return false;
      break;
    default:
      if (!CreateBrush(false, paint) ||
          !CreatePen(false, paint))
        return false;
      break;
  }

  /*
  getFlags();
    isAntiAlias();
    isDither()
    isLinearText()
    isSubpixelText()
    isUnderlineText()
    isStrikeThruText()
    isFakeBoldText()
    isDevKernText()
    isFilterBitmap()

  // Skia's text is not used. This should be fixed.
  getTextAlign()
  getTextScaleX()
  getTextSkewX()
  getTextEncoding()
  getFontMetrics()
  getFontSpacing()
  */

  // BUG 1094907: Implement shaders. Shaders currently in use:
  //  SkShader::CreateBitmapShader
  //  SkGradientShader::CreateRadial
  //  SkGradientShader::CreateLinear
  // SkASSERT(!paint.getShader());

  // http://b/1106647 Implement loopers and mask filter. Looper currently in
  // use:
  //   SkBlurDrawLooper is used for shadows.
  // SkASSERT(!paint.getLooper());
  // SkASSERT(!paint.getMaskFilter());

  // http://b/1165900 Implement xfermode.
  // SkASSERT(!paint.getXfermode());

  // The path effect should be processed before arriving here.
  SkASSERT(!paint.getPathEffect());

  // This isn't used in the code. Verify this assumption.
  SkASSERT(!paint.getRasterizer());
  // Reuse code to load Win32 Fonts.
  return true;
}

void VectorPlatformDeviceEmf::setMatrixClip(const SkMatrix& transform,
                                            const SkRegion& region,
                                            const SkClipStack&) {
  transform_ = transform;
  LoadTransformToDC(hdc_, transform_);
  clip_region_ = region;
  if (!clip_region_.isEmpty())
    LoadClipRegion();
}

void VectorPlatformDeviceEmf::DrawToNativeContext(HDC dc, int x, int y,
                                                  const RECT* src_rect) {
  SkASSERT(false);
}

void VectorPlatformDeviceEmf::LoadClipRegion() {
  SkMatrix t;
  t.reset();
  LoadClippingRegionToDC(hdc_, clip_region_, t);
}

#ifdef SK_SUPPORT_LEGACY_COMPATIBLEDEVICE_CONFIG
SkBaseDevice* VectorPlatformDeviceEmf::onCreateCompatibleDevice(
    SkBitmap::Config config, int width, int height, bool isOpaque,
    Usage /*usage*/) {
  SkASSERT(config == SkBitmap::kARGB_8888_Config);
  return VectorPlatformDeviceEmf::CreateDevice(width, height, isOpaque, NULL);
}
#endif

SkBaseDevice* VectorPlatformDeviceEmf::onCreateDevice(const SkImageInfo& info,
                                                      Usage /*usage*/) {
  SkASSERT(info.colorType() == kPMColor_SkColorType);
  return VectorPlatformDeviceEmf::CreateDevice(
      info.width(), info.height(), info.isOpaque(), NULL);
}

bool VectorPlatformDeviceEmf::CreateBrush(bool use_brush, COLORREF color) {
  SkASSERT(previous_brush_ == NULL);
  // We can't use SetDCBrushColor() or DC_BRUSH when drawing to a EMF buffer.
  // SetDCBrushColor() calls are not recorded at all and DC_BRUSH will use
  // WHITE_BRUSH instead.

  if (!use_brush) {
    // Set the transparency.
    if (0 == SetBkMode(hdc_, TRANSPARENT)) {
      SkASSERT(false);
      return false;
    }

    // Select the NULL brush.
    previous_brush_ = SelectObject(GetStockObject(NULL_BRUSH));
    return previous_brush_ != NULL;
  }

  // Set the opacity.
  if (0 == SetBkMode(hdc_, OPAQUE)) {
    SkASSERT(false);
    return false;
  }

  // Create and select the brush.
  previous_brush_ = SelectObject(CreateSolidBrush(color));
  return previous_brush_ != NULL;
}

bool VectorPlatformDeviceEmf::CreatePen(bool use_pen,
                                        COLORREF color,
                                        int stroke_width,
                                        float stroke_miter,
                                        DWORD pen_style) {
  SkASSERT(previous_pen_ == NULL);
  // We can't use SetDCPenColor() or DC_PEN when drawing to a EMF buffer.
  // SetDCPenColor() calls are not recorded at all and DC_PEN will use BLACK_PEN
  // instead.

  // No pen case
  if (!use_pen) {
    previous_pen_ = SelectObject(GetStockObject(NULL_PEN));
    return previous_pen_ != NULL;
  }

  // Use the stock pen if the stroke width is 0.
  if (stroke_width == 0) {
    // Create a pen with the right color.
    previous_pen_ = SelectObject(::CreatePen(PS_SOLID, 0, color));
    return previous_pen_ != NULL;
  }

  // Load a custom pen.
  LOGBRUSH brush = {0};
  brush.lbStyle = BS_SOLID;
  brush.lbColor = color;
  brush.lbHatch = 0;
  HPEN pen = ExtCreatePen(pen_style, stroke_width, &brush, 0, NULL);
  SkASSERT(pen != NULL);
  previous_pen_ = SelectObject(pen);
  if (previous_pen_ == NULL)
    return false;

  if (!SetMiterLimit(hdc_, stroke_miter, NULL)) {
    SkASSERT(false);
    return false;
  }
  return true;
}

void VectorPlatformDeviceEmf::Cleanup() {
  if (previous_brush_) {
    HGDIOBJ result = SelectObject(previous_brush_);
    previous_brush_ = NULL;
    if (result) {
      BOOL res = DeleteObject(result);
      SkASSERT(res != 0);
    }
  }
  if (previous_pen_) {
    HGDIOBJ result = SelectObject(previous_pen_);
    previous_pen_ = NULL;
    if (result) {
      BOOL res = DeleteObject(result);
      SkASSERT(res != 0);
    }
  }
  // Remove any loaded path from the context.
  AbortPath(hdc_);
}

HGDIOBJ VectorPlatformDeviceEmf::SelectObject(HGDIOBJ object) {
  HGDIOBJ result = ::SelectObject(hdc_, object);
  SkASSERT(result != HGDI_ERROR);
  if (result == HGDI_ERROR)
    return NULL;
  return result;
}

bool VectorPlatformDeviceEmf::CreateBrush(bool use_brush,
                                          const SkPaint& paint) {
  // Make sure that for transparent color, no brush is used.
  if (paint.getAlpha() == 0) {
    use_brush = false;
  }

  return CreateBrush(use_brush, SkColorToCOLORREF(paint.getColor()));
}

bool VectorPlatformDeviceEmf::CreatePen(bool use_pen, const SkPaint& paint) {
  // Make sure that for transparent color, no pen is used.
  if (paint.getAlpha() == 0) {
    use_pen = false;
  }

  DWORD pen_style = PS_GEOMETRIC | PS_SOLID;
  switch (paint.getStrokeJoin()) {
    case SkPaint::kMiter_Join:
      // Connects path segments with a sharp join.
      pen_style |= PS_JOIN_MITER;
      break;
    case SkPaint::kRound_Join:
      // Connects path segments with a round join.
      pen_style |= PS_JOIN_ROUND;
      break;
    case SkPaint::kBevel_Join:
      // Connects path segments with a flat bevel join.
      pen_style |= PS_JOIN_BEVEL;
      break;
    default:
      SkASSERT(false);
      break;
  }
  switch (paint.getStrokeCap()) {
    case SkPaint::kButt_Cap:
      // Begin/end contours with no extension.
      pen_style |= PS_ENDCAP_FLAT;
      break;
    case SkPaint::kRound_Cap:
      // Begin/end contours with a semi-circle extension.
      pen_style |= PS_ENDCAP_ROUND;
      break;
    case SkPaint::kSquare_Cap:
      // Begin/end contours with a half square extension.
      pen_style |= PS_ENDCAP_SQUARE;
      break;
    default:
      SkASSERT(false);
      break;
  }

  return CreatePen(use_pen,
                   SkColorToCOLORREF(paint.getColor()),
                   SkScalarRoundToInt(paint.getStrokeWidth()),
                   paint.getStrokeMiter(),
                   pen_style);
}

void VectorPlatformDeviceEmf::InternalDrawBitmap(const SkBitmap& bitmap,
                                                 int x, int y,
                                                 const SkPaint& paint) {
  unsigned char alpha = paint.getAlpha();
  if (alpha == 0)
    return;

  bool is_translucent;
  if (alpha != 255) {
    // ApplyPaint expect an opaque color.
    SkPaint tmp_paint(paint);
    tmp_paint.setAlpha(255);
    if (!ApplyPaint(tmp_paint))
      return;
    is_translucent = true;
  } else {
    if (!ApplyPaint(paint))
      return;
    is_translucent = false;
  }
  int src_size_x = bitmap.width();
  int src_size_y = bitmap.height();
  if (!src_size_x || !src_size_y)
    return;

  // Create a BMP v4 header that we can serialize. We use the shared "V3"
  // fillter to fill the stardard items, then add in the "V4" stuff we want.
  BITMAPV4HEADER bitmap_header = {0};
  FillBitmapInfoHeader(src_size_x, src_size_y,
                       reinterpret_cast<BITMAPINFOHEADER*>(&bitmap_header));
  bitmap_header.bV4Size = sizeof(BITMAPV4HEADER);
  bitmap_header.bV4RedMask   = 0x00ff0000;
  bitmap_header.bV4GreenMask = 0x0000ff00;
  bitmap_header.bV4BlueMask  = 0x000000ff;
  bitmap_header.bV4AlphaMask = 0xff000000;

  SkAutoLockPixels lock(bitmap);
  SkASSERT(bitmap.config() == SkBitmap::kARGB_8888_Config);
  const uint32_t* pixels = static_cast<const uint32_t*>(bitmap.getPixels());
  if (pixels == NULL) {
    SkASSERT(false);
    return;
  }

  if (!is_translucent) {
    int row_length = bitmap.rowBytesAsPixels();
    // There is no quick way to determine if an image is opaque.
    for (int y2 = 0; y2 < src_size_y; ++y2) {
      for (int x2 = 0; x2 < src_size_x; ++x2) {
        if (SkColorGetA(pixels[(y2 * row_length) + x2]) != 255) {
          is_translucent = true;
          y2 = src_size_y;
          break;
        }
      }
    }
  }

  HDC dc = BeginPlatformPaint();
  BITMAPINFOHEADER hdr = {0};
  FillBitmapInfoHeader(src_size_x, src_size_y, &hdr);
  if (is_translucent) {
    // The image must be loaded as a bitmap inside a device context.
    HDC bitmap_dc = ::CreateCompatibleDC(dc);
    void* bits = NULL;
    HBITMAP hbitmap = ::CreateDIBSection(
        bitmap_dc, reinterpret_cast<const BITMAPINFO*>(&hdr),
        DIB_RGB_COLORS, &bits, NULL, 0);

    // static cast to a char so we can do byte ptr arithmatic to
    // get the offset.
    unsigned char* dest_buffer = static_cast<unsigned char *>(bits);

    // We will copy row by row to avoid having to worry about
    // the row strides being different.
    const int dest_row_size = hdr.biBitCount / 8 * hdr.biWidth;
    for (int row = 0; row < bitmap.height(); ++row) {
      int dest_offset = row * dest_row_size;
      // pixels_offset in terms of pixel count.
      int src_offset = row * bitmap.rowBytesAsPixels();
      memcpy(dest_buffer + dest_offset, pixels + src_offset, dest_row_size);
    }
    SkASSERT(hbitmap);
    HGDIOBJ old_bitmap = ::SelectObject(bitmap_dc, hbitmap);

    // After some analysis of IE7's behavior, this is the thing to do. I was
    // sure IE7 was doing so kind of bitmasking due to the way translucent image
    // where renderered but after some windbg tracing, it is being done by the
    // printer driver after all (mostly HP printers). IE7 always use AlphaBlend
    // for bitmasked images. The trick seems to switch the stretching mode in
    // what the driver expects.
    DWORD previous_mode = GetStretchBltMode(dc);
    BOOL result = SetStretchBltMode(dc, COLORONCOLOR);
    SkASSERT(result);
    // Note that this function expect premultiplied colors (!)
    BLENDFUNCTION blend_function = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    result = GdiAlphaBlend(dc,
                           x, y,  // Destination origin.
                           src_size_x, src_size_y,  // Destination size.
                           bitmap_dc,
                           0, 0,  // Source origin.
                           src_size_x, src_size_y,  // Source size.
                           blend_function);
    SkASSERT(result);
    result = SetStretchBltMode(dc, previous_mode);
    SkASSERT(result);

    ::SelectObject(bitmap_dc, static_cast<HBITMAP>(old_bitmap));
    DeleteObject(hbitmap);
    DeleteDC(bitmap_dc);
  } else {
    int nCopied = StretchDIBits(dc,
                                x, y,  // Destination origin.
                                src_size_x, src_size_y,
                                0, 0,  // Source origin.
                                src_size_x, src_size_y,  // Source size.
                                pixels,
                                reinterpret_cast<const BITMAPINFO*>(&hdr),
                                DIB_RGB_COLORS,
                                SRCCOPY);
  }
  EndPlatformPaint();
  Cleanup();
}

}  // namespace skia
