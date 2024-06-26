// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_CANVAS_H_
#define UI_GFX_CANVAS_H_

#include <stdint.h>

#include <vector>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "skia/ext/platform_canvas.h"
#include "skia/ext/refptr.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/shadow_value.h"
#include "ui/gfx/text_constants.h"

namespace gfx {

class Rect;
class RectF;
class FontList;
class Point;
class Size;
class Transform;

// Canvas is a SkCanvas wrapper that provides a number of methods for
// common operations used throughout an application built using ui/gfx.
//
// All methods that take integer arguments (as is used throughout views)
// end with Int. If you need to use methods provided by SkCanvas, you'll
// need to do a conversion. In particular you'll need to use |SkIntToScalar()|,
// or if converting from a scalar to an integer |SkScalarRound()|.
//
// A handful of methods in this class are overloaded providing an additional
// argument of type SkXfermode::Mode. SkXfermode::Mode specifies how the
// source and destination colors are combined. Unless otherwise specified,
// the variant that does not take a SkXfermode::Mode uses a transfer mode
// of kSrcOver_Mode.
class GFX_EXPORT Canvas {
public:
    enum {
        // Specifies the alignment for text rendered with the DrawStringRect method.
        TEXT_ALIGN_LEFT = 1 << 0,
        TEXT_ALIGN_CENTER = 1 << 1,
        TEXT_ALIGN_RIGHT = 1 << 2,
        TEXT_ALIGN_TO_HEAD = 1 << 3,

        // Specifies the text consists of multiple lines.
        MULTI_LINE = 1 << 4,

        // By default DrawStringRect does not process the prefix ('&') character
        // specially. That is, the string "&foo" is rendered as "&foo". When
        // rendering text from a resource that uses the prefix character for
        // mnemonics, the prefix should be processed and can be rendered as an
        // underline (SHOW_PREFIX), or not rendered at all (HIDE_PREFIX).
        SHOW_PREFIX = 1 << 5,
        HIDE_PREFIX = 1 << 6,

        // Prevent ellipsizing
        NO_ELLIPSIS = 1 << 7,

        // Specifies if words can be split by new lines.
        // This only works with MULTI_LINE.
        CHARACTER_BREAK = 1 << 8,

        // Instructs DrawStringRect() to not use subpixel rendering.  This is useful
        // when rendering text onto a fully- or partially-transparent background
        // that will later be blended with another image.
        NO_SUBPIXEL_RENDERING = 1 << 9,
    };

    // Creates an empty canvas with image_scale of 1x.
    Canvas();

    // Creates canvas with provided DIP |size| and |image_scale|.
    // If this canvas is not opaque, it's explicitly cleared to transparent before
    // being returned.
    Canvas(const Size& size, float image_scale, bool is_opaque);

    // Constructs a canvas with the size and the image_scale of the provided
    // |image_rep|, and draws the |image_rep| into it.
    Canvas(const ImageSkiaRep& image_rep, bool is_opaque);

    // Creates a Canvas backed by an |sk_canvas| with |image_scale_|.
    // |sk_canvas| is assumed to be already scaled based on |image_scale|
    // so no additional scaling is applied.
    Canvas(const skia::RefPtr<SkCanvas>& sk_canvas, float image_scale);

    virtual ~Canvas();

    // Recreates the backing platform canvas with DIP |size| and |image_scale_|.
    // If the canvas is not opaque, it is explicitly cleared.
    // This method is public so that canvas_skia_paint can recreate the platform
    // canvas after having initialized the canvas.
    // TODO(pkotwicz): Push the image_scale into skia::PlatformCanvas such that
    // this method can be private.
    void RecreateBackingCanvas(const Size& size,
        float image_scale,
        bool is_opaque);

    // Compute the size required to draw some text with the provided fonts.
    // Attempts to fit the text with the provided width and height. Increases
    // height and then width as needed to make the text fit. This method
    // supports multiple lines. On Skia only a line_height can be specified and
    // specifying a 0 value for it will cause the default height to be used.
    static void SizeStringInt(const base::string16& text,
        const FontList& font_list,
        int* width,
        int* height,
        int line_height,
        int flags);

    // This is same as SizeStringInt except that fractional size is returned.
    // See comment in GetStringWidthF for its usage.
    static void SizeStringFloat(const base::string16& text,
        const FontList& font_list,
        float* width,
        float* height,
        int line_height,
        int flags);

    // Returns the number of horizontal pixels needed to display the specified
    // |text| with |font_list|.
    static int GetStringWidth(const base::string16& text,
        const FontList& font_list);

    // This is same as GetStringWidth except that fractional width is returned.
    // Use this method for the scenario that multiple string widths need to be
    // summed up. This is because GetStringWidth returns the ceiled width and
    // adding multiple ceiled widths could cause more precision loss for certain
    // platform like Mac where the fractioal width is used.
    static float GetStringWidthF(const base::string16& text,
        const FontList& font_list);

    // Returns the default text alignment to be used when drawing text on a
    // Canvas based on the directionality of the system locale language.
    // This function is used by Canvas::DrawStringRect when the text alignment
    // is not specified.
    //
    // This function returns either Canvas::TEXT_ALIGN_LEFT or
    // Canvas::TEXT_ALIGN_RIGHT.
    static int DefaultCanvasTextAlignment();

    // Draws text with a 1-pixel halo around it of the given color.
    // On Windows, it allows ClearType to be drawn to an otherwise transparent
    //   bitmap for drag images. Drag images have only 1-bit of transparency, so
    //   we don't do any fancy blurring.
    // On Linux, text with halo is created by stroking it with 2px |halo_color|
    //   then filling it with |text_color|.
    // On Mac, NOTIMPLEMENTED.
    //   TODO(dhollowa): Skia-native implementation is underway.  Cut over to
    //   that when ready.  http::/crbug.com/109946
    void DrawStringRectWithHalo(const base::string16& text,
        const FontList& font_list,
        SkColor text_color,
        SkColor halo_color,
        const Rect& display_rect,
        int flags);

    // Extracts an ImageSkiaRep from the contents of this canvas.
    ImageSkiaRep ExtractImageRep() const;

    // Draws a dashed rectangle of the specified color.
    void DrawDashedRect(const Rect& rect, SkColor color);

    // Unscales by the image scale factor (aka device scale factor), and returns
    // that factor.  This is useful when callers want to draw directly in the
    // native scale.
    float UndoDeviceScaleFactor();

    // Saves a copy of the drawing state onto a stack, operating on this copy
    // until a balanced call to Restore() is made.
    void Save();

    // As with Save(), except draws to a layer that is blended with the canvas
    // at the specified alpha once Restore() is called.
    // |layer_bounds| are the bounds of the layer relative to the current
    // transform.
    void SaveLayerAlpha(uint8_t alpha);
    void SaveLayerAlpha(uint8_t alpha, const Rect& layer_bounds);

    // Restores the drawing state after a call to Save*(). It is an error to
    // call Restore() more times than Save*().
    void Restore();

    // Adds |rect| to the current clip.
    void ClipRect(const Rect& rect);

    // Adds |path| to the current clip. |do_anti_alias| is true if the clip
    // should be antialiased.
    void ClipPath(const SkPath& path, bool do_anti_alias);

    // Returns true if the current clip is empty.
    bool IsClipEmpty() const;

    // Returns the bounds of the current clip (in local coordinates) in the
    // |bounds| parameter, and returns true if it is non empty.
    bool GetClipBounds(Rect* bounds);

    void Translate(const Vector2d& offset);

    void Scale(int x_scale, int y_scale);

    // Fills the entire canvas' bitmap (restricted to current clip) with
    // specified |color| using a transfer mode of SkXfermode::kSrcOver_Mode.
    void DrawColor(SkColor color);

    // Fills the entire canvas' bitmap (restricted to current clip) with
    // specified |color| and |mode|.
    void DrawColor(SkColor color, SkXfermode::Mode mode);

    // Fills |rect| with |color| using a transfer mode of
    // SkXfermode::kSrcOver_Mode.
    void FillRect(const Rect& rect, SkColor color);

    // Fills |rect| with the specified |color| and |mode|.
    void FillRect(const Rect& rect, SkColor color, SkXfermode::Mode mode);

    // Draws a single pixel rect in the specified region with the specified
    // color, using a transfer mode of SkXfermode::kSrcOver_Mode.
    //
    // NOTE: if you need a single pixel line, use DrawLine.
    void DrawRect(const Rect& rect, SkColor color);

    // Draws a single pixel rect in the specified region with the specified
    // color and transfer mode.
    //
    // NOTE: if you need a single pixel line, use DrawLine.
    void DrawRect(const Rect& rect, SkColor color, SkXfermode::Mode mode);

    // Draws the given rectangle with the given |paint| parameters.
    void DrawRect(const Rect& rect, const SkPaint& paint);

    // Draw the given point with the given |paint| parameters.
    void DrawPoint(const Point& p, const SkPaint& paint);

    // Draws a single pixel line with the specified color.
    void DrawLine(const Point& p1, const Point& p2, SkColor color);

    // Draws a line with the given |paint| parameters.
    void DrawLine(const Point& p1, const Point& p2, const SkPaint& paint);

    // Draws a circle with the given |paint| parameters.
    void DrawCircle(const Point& center_point,
        int radius,
        const SkPaint& paint);

    // Draws the given rectangle with rounded corners of |radius| using the
    // given |paint| parameters. DEPRECATED in favor of the RectF version below.
    // TODO(mgiuca): Remove this (http://crbug.com/553726).
    void DrawRoundRect(const Rect& rect, int radius, const SkPaint& paint);

    // Draws the given rectangle with rounded corners of |radius| using the
    // given |paint| parameters.
    void DrawRoundRect(const RectF& rect, float radius, const SkPaint& paint);

    // Draws the given path using the given |paint| parameters.
    void DrawPath(const SkPath& path, const SkPaint& paint);

    // Draws an image with the origin at the specified location. The upper left
    // corner of the bitmap is rendered at the specified location.
    // Parameters are specified relative to current canvas scale not in pixels.
    // Thus, x is 2 pixels if canvas scale = 2 & |x| = 1.
    void DrawImageInt(const ImageSkia&, int x, int y);

    // Helper for DrawImageInt(..., paint) that constructs a temporary paint and
    // calls paint.setAlpha(alpha).
    void DrawImageInt(const ImageSkia&, int x, int y, uint8_t alpha);

    // Draws an image with the origin at the specified location, using the
    // specified paint. The upper left corner of the bitmap is rendered at the
    // specified location.
    // Parameters are specified relative to current canvas scale not in pixels.
    // Thus, |x| is 2 pixels if canvas scale = 2 & |x| = 1.
    void DrawImageInt(const ImageSkia& image,
        int x,
        int y,
        const SkPaint& paint);

    // Draws a portion of an image in the specified location. The src parameters
    // correspond to the region of the bitmap to draw in the region defined
    // by the dest coordinates.
    //
    // If the width or height of the source differs from that of the destination,
    // the image will be scaled. When scaling down, a mipmap will be generated.
    // Set |filter| to use filtering for images, otherwise the nearest-neighbor
    // algorithm is used for resampling.
    //
    // An optional custom SkPaint can be provided.
    // Parameters are specified relative to current canvas scale not in pixels.
    // Thus, |x| is 2 pixels if canvas scale = 2 & |x| = 1.
    void DrawImageInt(const ImageSkia& image,
        int src_x,
        int src_y,
        int src_w,
        int src_h,
        int dest_x,
        int dest_y,
        int dest_w,
        int dest_h,
        bool filter);
    void DrawImageInt(const ImageSkia& image,
        int src_x,
        int src_y,
        int src_w,
        int src_h,
        int dest_x,
        int dest_y,
        int dest_w,
        int dest_h,
        bool filter,
        const SkPaint& paint);

    // Same as the DrawImageInt functions above. Difference being this does not
    // do any scaling, i.e. it does not scale the output by the device scale
    // factor (the internal image_scale_). It takes an ImageSkiaRep instead of
    // an ImageSkia as the caller chooses the exact scale/pixel representation to
    // use, which will not be scaled while drawing it into the canvas.
    void DrawImageIntInPixel(const ImageSkiaRep& image_rep,
        int dest_x,
        int dest_y,
        int dest_w,
        int dest_h,
        bool filter,
        const SkPaint& paint);

    // Draws an |image| with the top left corner at |x| and |y|, clipped to
    // |path|.
    // Parameters are specified relative to current canvas scale not in pixels.
    // Thus, x is 2 pixels if canvas scale = 2 & |x| = 1.
    void DrawImageInPath(const ImageSkia& image,
        int x,
        int y,
        const SkPath& path,
        const SkPaint& paint);

    // Draws text with the specified color, fonts and location. The text is
    // aligned to the left, vertically centered, clipped to the region. If the
    // text is too big, it is truncated and '...' is added to the end.
    void DrawStringRect(const base::string16& text,
        const FontList& font_list,
        SkColor color,
        const Rect& display_rect);

    // Draws text with the specified color, fonts and location. The last argument
    // specifies flags for how the text should be rendered. It can be one of
    // TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT or TEXT_ALIGN_LEFT.
    void DrawStringRectWithFlags(const base::string16& text,
        const FontList& font_list,
        SkColor color,
        const Rect& display_rect,
        int flags);

    // Similar to above DrawStringRect method but with text shadows support.
    // Currently it's only implemented for canvas skia. Specifying a 0 line_height
    // will cause the default height to be used.
    void DrawStringRectWithShadows(const base::string16& text,
        const FontList& font_list,
        SkColor color,
        const Rect& text_bounds,
        int line_height,
        int flags,
        const ShadowValues& shadows);

    // Draws a dotted gray rectangle used for focus purposes.
    void DrawFocusRect(const Rect& rect);

    // Draws a |rect| in the specified region with the specified |color| with a
    // with of one logical pixel which might be more device pixels.
    void DrawSolidFocusRect(const Rect& rect, SkColor color);

    // Tiles the image in the specified region.
    // Parameters are specified relative to current canvas scale not in pixels.
    // Thus, |x| is 2 pixels if canvas scale = 2 & |x| = 1.
    void TileImageInt(const ImageSkia& image,
        int x,
        int y,
        int w,
        int h);
    void TileImageInt(const ImageSkia& image,
        int src_x,
        int src_y,
        int dest_x,
        int dest_y,
        int w,
        int h);
    void TileImageInt(const ImageSkia& image,
        int src_x,
        int src_y,
        float tile_scale_x,
        float tile_scale_y,
        int dest_x,
        int dest_y,
        int w,
        int h);

    // Apply transformation on the canvas.
    void Transform(const Transform& transform);

    // Draws the given string with a fade gradient at the end.
    void DrawFadedString(const base::string16& text,
        const FontList& font_list,
        SkColor color,
        const Rect& display_rect,
        int flags);

    SkCanvas* sk_canvas() { return canvas_.get(); }
    float image_scale() const { return image_scale_; }

private:
    // Test whether the provided rectangle intersects the current clip rect.
    bool IntersectsClipRectInt(int x, int y, int w, int h);
    bool IntersectsClipRect(const Rect& rect);

    // Helper for the DrawImageInt functions declared above. The
    // |remove_image_scale| parameter indicates if the scale of the |image_rep|
    // should be removed when drawing the image, to avoid double-scaling it.
    void DrawImageIntHelper(const ImageSkiaRep& image_rep,
        int src_x,
        int src_y,
        int src_w,
        int src_h,
        int dest_x,
        int dest_y,
        int dest_w,
        int dest_h,
        bool filter,
        const SkPaint& paint,
        bool remove_image_scale);

    // The device scale factor at which drawing on this canvas occurs.
    // An additional scale can be applied via Canvas::Scale(). However,
    // Canvas::Scale() does not affect |image_scale_|.
    float image_scale_;

    skia::RefPtr<SkCanvas> canvas_;

    DISALLOW_COPY_AND_ASSIGN(Canvas);
};

} // namespace gfx

#endif // UI_GFX_CANVAS_H_
