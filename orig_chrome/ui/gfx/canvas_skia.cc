// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/canvas.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "base/i18n/rtl.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/numerics/safe_conversions.h"
#include "build/build_config.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkPixmap.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/range/range.h"
#include "ui/gfx/render_text.h"
#include "ui/gfx/shadow_value.h"
#include "ui/gfx/text_elider.h"
#include "ui/gfx/text_utils.h"

namespace gfx {

namespace {

    // Checks each pixel immediately adjacent to the given pixel in the bitmap. If
    // any of them are not the halo color, returns true. This defines the halo of
    // pixels that will appear around the text. Note that we have to check each
    // pixel against both the halo color and transparent since
    // |DrawStringRectWithHalo| will modify the bitmap as it goes, and cleared
    // pixels shouldn't count as changed.
    bool PixelShouldGetHalo(const SkPixmap& pixmap,
        int x, int y,
        SkColor halo_color)
    {
        if (x > 0 && *pixmap.addr32(x - 1, y) != halo_color && *pixmap.addr32(x - 1, y) != 0)
            return true; // Touched pixel to the left.
        if (x < pixmap.width() - 1 && *pixmap.addr32(x + 1, y) != halo_color && *pixmap.addr32(x + 1, y) != 0)
            return true; // Touched pixel to the right.
        if (y > 0 && *pixmap.addr32(x, y - 1) != halo_color && *pixmap.addr32(x, y - 1) != 0)
            return true; // Touched pixel above.
        if (y < pixmap.height() - 1 && *pixmap.addr32(x, y + 1) != halo_color && *pixmap.addr32(x, y + 1) != 0)
            return true; // Touched pixel below.
        return false;
    }

    // Strips accelerator character prefixes in |text| if needed, based on |flags|.
    // Returns a range in |text| to underline or Range::InvalidRange() if
    // underlining is not needed.
    Range StripAcceleratorChars(int flags, base::string16* text)
    {
        if (flags & (Canvas::SHOW_PREFIX | Canvas::HIDE_PREFIX)) {
            int char_pos = -1;
            int char_span = 0;
            *text = RemoveAcceleratorChar(*text, '&', &char_pos, &char_span);
            if ((flags & Canvas::SHOW_PREFIX) && char_pos != -1)
                return Range(char_pos, char_pos + char_span);
        }
        return Range::InvalidRange();
    }

    // Elides |text| and adjusts |range| appropriately. If eliding causes |range|
    // to no longer point to the same character in |text|, |range| is made invalid.
    void ElideTextAndAdjustRange(const FontList& font_list,
        float width,
        base::string16* text,
        Range* range)
    {
        const base::char16 start_char = (range->IsValid() ? text->at(range->start()) : 0);
        *text = ElideText(*text, font_list, width, ELIDE_TAIL);
        if (!range->IsValid())
            return;
        if (range->start() >= text->length() || text->at(range->start()) != start_char) {
            *range = Range::InvalidRange();
        }
    }

    // Updates |render_text| from the specified parameters.
    void UpdateRenderText(const Rect& rect,
        const base::string16& text,
        const FontList& font_list,
        int flags,
        SkColor color,
        RenderText* render_text)
    {
        render_text->SetFontList(font_list);
        render_text->SetText(text);
        render_text->SetCursorEnabled(false);
        render_text->SetDisplayRect(rect);

        // Set the text alignment explicitly based on the directionality of the UI,
        // if not specified.
        if (!(flags & (Canvas::TEXT_ALIGN_CENTER | Canvas::TEXT_ALIGN_RIGHT | Canvas::TEXT_ALIGN_LEFT | Canvas::TEXT_ALIGN_TO_HEAD))) {
            flags |= Canvas::DefaultCanvasTextAlignment();
        }

        if (flags & Canvas::TEXT_ALIGN_TO_HEAD)
            render_text->SetHorizontalAlignment(ALIGN_TO_HEAD);
        else if (flags & Canvas::TEXT_ALIGN_RIGHT)
            render_text->SetHorizontalAlignment(ALIGN_RIGHT);
        else if (flags & Canvas::TEXT_ALIGN_CENTER)
            render_text->SetHorizontalAlignment(ALIGN_CENTER);
        else
            render_text->SetHorizontalAlignment(ALIGN_LEFT);

        render_text->set_subpixel_rendering_suppressed(
            (flags & Canvas::NO_SUBPIXEL_RENDERING) != 0);

        render_text->SetColor(color);
        const int font_style = font_list.GetFontStyle();
        render_text->SetStyle(BOLD, (font_style & Font::BOLD) != 0);
        render_text->SetStyle(ITALIC, (font_style & Font::ITALIC) != 0);
        render_text->SetStyle(UNDERLINE, (font_style & Font::UNDERLINE) != 0);
    }

} // namespace

// static
void Canvas::SizeStringFloat(const base::string16& text,
    const FontList& font_list,
    float* width, float* height,
    int line_height,
    int flags)
{
    DCHECK_GE(*width, 0);
    DCHECK_GE(*height, 0);

    if ((flags & MULTI_LINE) && *width != 0) {
        WordWrapBehavior wrap_behavior = TRUNCATE_LONG_WORDS;
        if (flags & CHARACTER_BREAK)
            wrap_behavior = WRAP_LONG_WORDS;
        else if (!(flags & NO_ELLIPSIS))
            wrap_behavior = ELIDE_LONG_WORDS;

        std::vector<base::string16> strings;
        ElideRectangleText(text, font_list, *width, INT_MAX, wrap_behavior,
            &strings);
        Rect rect(base::saturated_cast<int>(*width), INT_MAX);
        scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
        UpdateRenderText(rect, base::string16(), font_list, flags, 0,
            render_text.get());

        float h = 0;
        float w = 0;
        for (size_t i = 0; i < strings.size(); ++i) {
            StripAcceleratorChars(flags, &strings[i]);
            render_text->SetText(strings[i]);
            const SizeF& string_size = render_text->GetStringSizeF();
            w = std::max(w, string_size.width());
            h += (i > 0 && line_height > 0) ? std::max(static_cast<float>(line_height), string_size.height())
                                            : string_size.height();
        }
        *width = w;
        *height = h;
    } else {
        scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
        Rect rect(base::saturated_cast<int>(*width),
            base::saturated_cast<int>(*height));
        base::string16 adjusted_text = text;
        StripAcceleratorChars(flags, &adjusted_text);
        UpdateRenderText(rect, adjusted_text, font_list, flags, 0,
            render_text.get());
        const SizeF& string_size = render_text->GetStringSizeF();
        *width = string_size.width();
        *height = string_size.height();
    }
}

void Canvas::DrawStringRectWithShadows(const base::string16& text,
    const FontList& font_list,
    SkColor color,
    const Rect& text_bounds,
    int line_height,
    int flags,
    const ShadowValues& shadows)
{
    if (!IntersectsClipRect(text_bounds))
        return;

    Rect clip_rect(text_bounds);
    clip_rect.Inset(ShadowValue::GetMargin(shadows));

    canvas_->save();
    ClipRect(clip_rect);

    Rect rect(text_bounds);

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->set_shadows(shadows);

    if (flags & MULTI_LINE) {
        WordWrapBehavior wrap_behavior = IGNORE_LONG_WORDS;
        if (flags & CHARACTER_BREAK)
            wrap_behavior = WRAP_LONG_WORDS;
        else if (!(flags & NO_ELLIPSIS))
            wrap_behavior = ELIDE_LONG_WORDS;

        std::vector<base::string16> strings;
        ElideRectangleText(text, font_list,
            static_cast<float>(text_bounds.width()),
            text_bounds.height(), wrap_behavior, &strings);

        for (size_t i = 0; i < strings.size(); i++) {
            Range range = StripAcceleratorChars(flags, &strings[i]);
            UpdateRenderText(rect, strings[i], font_list, flags, color,
                render_text.get());
            int line_padding = 0;
            if (line_height > 0)
                line_padding = line_height - render_text->GetStringSize().height();
            else
                line_height = render_text->GetStringSize().height();

                // TODO(msw|asvitkine): Center Windows multi-line text: crbug.com/107357
#if !defined(OS_WIN)
            if (i == 0) {
                // TODO(msw|asvitkine): Support multi-line text with varied heights.
                const int text_height = strings.size() * line_height - line_padding;
                rect += Vector2d(0, (text_bounds.height() - text_height) / 2);
            }
#endif

            rect.set_height(line_height - line_padding);

            if (range.IsValid())
                render_text->ApplyStyle(UNDERLINE, true, range);
            render_text->SetDisplayRect(rect);
            render_text->Draw(this);
            rect += Vector2d(0, line_height);
        }
    } else {
        base::string16 adjusted_text = text;
        Range range = StripAcceleratorChars(flags, &adjusted_text);
        bool elide_text = ((flags & NO_ELLIPSIS) == 0);

#if defined(OS_LINUX)
        // On Linux, eliding really means fading the end of the string. But only
        // for LTR text. RTL text is still elided (on the left) with "...".
        if (elide_text) {
            render_text->SetText(adjusted_text);
            if (render_text->GetDisplayTextDirection() == base::i18n::LEFT_TO_RIGHT) {
                render_text->SetElideBehavior(FADE_TAIL);
                elide_text = false;
            }
        }
#endif

        if (elide_text) {
            ElideTextAndAdjustRange(font_list,
                static_cast<float>(text_bounds.width()),
                &adjusted_text, &range);
        }

        UpdateRenderText(rect, adjusted_text, font_list, flags, color,
            render_text.get());
        if (range.IsValid())
            render_text->ApplyStyle(UNDERLINE, true, range);
        render_text->Draw(this);
    }

    canvas_->restore();
}

void Canvas::DrawStringRectWithHalo(const base::string16& text,
    const FontList& font_list,
    SkColor text_color,
    SkColor halo_color_in,
    const Rect& display_rect,
    int flags)
{
    // Some callers will have semitransparent halo colors, which we don't handle
    // (since the resulting image can have 1-bit transparency only).
    SkColor halo_color = SkColorSetA(halo_color_in, 0xFF);

    // Create a temporary buffer filled with the halo color. It must leave room
    // for the 1-pixel border around the text.
    Size size(display_rect.width() + 2, display_rect.height() + 2);
    Canvas text_canvas(size, image_scale(), false);
    SkPaint bkgnd_paint;
    bkgnd_paint.setColor(halo_color);
    text_canvas.DrawRect(Rect(size), bkgnd_paint);

    // Draw the text into the temporary buffer. This will have correct
    // ClearType since the background color is the same as the halo color.
    text_canvas.DrawStringRectWithFlags(
        text, font_list, text_color,
        Rect(1, 1, display_rect.width(), display_rect.height()), flags);

    uint32_t halo_premul = SkPreMultiplyColor(halo_color);
    SkPixmap pixmap;
    skia::GetWritablePixels(text_canvas.sk_canvas(), &pixmap);

    for (int cur_y = 0; cur_y < pixmap.height(); cur_y++) {
        uint32_t* text_row = pixmap.writable_addr32(0, cur_y);
        for (int cur_x = 0; cur_x < pixmap.width(); cur_x++) {
            if (text_row[cur_x] == halo_premul) {
                // This pixel was not touched by the text routines. See if it borders
                // a touched pixel in any of the 4 directions (not diagonally).
                if (!PixelShouldGetHalo(pixmap, cur_x, cur_y, halo_premul))
                    text_row[cur_x] = 0; // Make transparent.
            } else {
                text_row[cur_x] |= 0xff << SK_A32_SHIFT; // Make opaque.
            }
        }
    }

    // Draw the halo bitmap with blur.
    SkBitmap bitmap;
    bitmap.installPixels(pixmap.info(), pixmap.writable_addr(),
        pixmap.rowBytes());
    ImageSkia text_image = ImageSkia(ImageSkiaRep(bitmap,
        text_canvas.image_scale()));
    DrawImageInt(text_image, display_rect.x() - 1, display_rect.y() - 1);
}

void Canvas::DrawFadedString(const base::string16& text,
    const FontList& font_list,
    SkColor color,
    const Rect& display_rect,
    int flags)
{
    // If the whole string fits in the destination then just draw it directly.
    if (GetStringWidth(text, font_list) <= display_rect.width()) {
        DrawStringRectWithFlags(text, font_list, color, display_rect, flags);
        return;
    }
    // Align with content directionality instead of fading both ends.
    flags &= ~TEXT_ALIGN_CENTER;
    if (!(flags & (TEXT_ALIGN_LEFT | TEXT_ALIGN_RIGHT)))
        flags |= TEXT_ALIGN_TO_HEAD;
    flags |= NO_ELLIPSIS;

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    Rect rect = display_rect;
    UpdateRenderText(rect, text, font_list, flags, color, render_text.get());
    render_text->SetElideBehavior(FADE_TAIL);

    canvas_->save();
    ClipRect(display_rect);
    render_text->Draw(this);
    canvas_->restore();
}

} // namespace gfx
