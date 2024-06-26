// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/render_text.h"

#include <limits.h>

#include <algorithm>
#include <climits>

#include "base/command_line.h"
#include "base/i18n/break_iterator.h"
#include "base/logging.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "third_party/icu/source/common/unicode/rbbi.h"
#include "third_party/icu/source/common/unicode/utf16.h"
#include "third_party/skia/include/core/SkDrawLooper.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/safe_integer_conversions.h"
#include "ui/gfx/platform_font.h"
#include "ui/gfx/render_text_harfbuzz.h"
#include "ui/gfx/scoped_canvas.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/switches.h"
#include "ui/gfx/text_elider.h"
#include "ui/gfx/text_utils.h"
#include "ui/gfx/utf16_indexing.h"

#if defined(OS_MACOSX)
#include "third_party/skia/include/ports/SkTypeface_mac.h"
#include "ui/gfx/render_text_mac.h"
#endif // defined(OS_MACOSX)

namespace gfx {

namespace {

    // All chars are replaced by this char when the password style is set.
    // TODO(benrg): GTK uses the first of U+25CF, U+2022, U+2731, U+273A, '*'
    // that's available in the font (find_invisible_char() in gtkentry.c).
    const base::char16 kPasswordReplacementChar = '*';

    // Default color used for the text and cursor.
    const SkColor kDefaultColor = SK_ColorBLACK;

    // Default color used for drawing selection background.
    const SkColor kDefaultSelectionBackgroundColor = SK_ColorGRAY;

    // Fraction of the text size to lower a strike through below the baseline.
    const SkScalar kStrikeThroughOffset = (-SK_Scalar1 * 6 / 21);
    // Fraction of the text size to lower an underline below the baseline.
    const SkScalar kUnderlineOffset = (SK_Scalar1 / 9);
    // Fraction of the text size to use for a strike through or under-line.
    const SkScalar kLineThickness = (SK_Scalar1 / 18);
    // Fraction of the text size to use for a top margin of a diagonal strike.
    const SkScalar kDiagonalStrikeMarginOffset = (SK_Scalar1 / 4);

    // Invalid value of baseline.  Assigning this value to |baseline_| causes
    // re-calculation of baseline.
    const int kInvalidBaseline = INT_MAX;

    // Returns the baseline, with which the text best appears vertically centered.
    int DetermineBaselineCenteringText(const Rect& display_rect,
        const FontList& font_list)
    {
        const int display_height = display_rect.height();
        const int font_height = font_list.GetHeight();
        // Lower and upper bound of baseline shift as we try to show as much area of
        // text as possible.  In particular case of |display_height| == |font_height|,
        // we do not want to shift the baseline.
        const int min_shift = std::min(0, display_height - font_height);
        const int max_shift = std::abs(display_height - font_height);
        const int baseline = font_list.GetBaseline();
        const int cap_height = font_list.GetCapHeight();
        const int internal_leading = baseline - cap_height;
        // Some platforms don't support getting the cap height, and simply return
        // the entire font ascent from GetCapHeight().  Centering the ascent makes
        // the font look too low, so if GetCapHeight() returns the ascent, center
        // the entire font height instead.
        const int space = display_height - ((internal_leading != 0) ? cap_height : font_height);
        const int baseline_shift = space / 2 - internal_leading;
        return baseline + std::max(min_shift, std::min(max_shift, baseline_shift));
    }

#if !defined(OS_MACOSX)
    // Converts |Font::FontStyle| flags to |SkTypeface::Style| flags.
    SkTypeface::Style ConvertFontStyleToSkiaTypefaceStyle(int font_style)
    {
        int skia_style = SkTypeface::kNormal;
        skia_style |= (font_style & Font::BOLD) ? SkTypeface::kBold : 0;
        skia_style |= (font_style & Font::ITALIC) ? SkTypeface::kItalic : 0;
        return static_cast<SkTypeface::Style>(skia_style);
    }
#endif

    int round(float value)
    {
        return static_cast<int>(floor(value + 0.5f));
    }

    // Given |font| and |display_width|, returns the width of the fade gradient.
    int CalculateFadeGradientWidth(const FontList& font_list, int display_width)
    {
        // Fade in/out about 3 characters of the beginning/end of the string.
        // Use a 1/3 of the display width if the display width is very short.
        const int narrow_width = font_list.GetExpectedTextWidth(3);
        const int gradient_width = std::min(narrow_width, round(display_width / 3.f));
        DCHECK_GE(gradient_width, 0);
        return gradient_width;
    }

    // Appends to |positions| and |colors| values corresponding to the fade over
    // |fade_rect| from color |c0| to color |c1|.
    void AddFadeEffect(const Rect& text_rect,
        const Rect& fade_rect,
        SkColor c0,
        SkColor c1,
        std::vector<SkScalar>* positions,
        std::vector<SkColor>* colors)
    {
        const SkScalar left = static_cast<SkScalar>(fade_rect.x() - text_rect.x());
        const SkScalar width = static_cast<SkScalar>(fade_rect.width());
        const SkScalar p0 = left / text_rect.width();
        const SkScalar p1 = (left + width) / text_rect.width();
        // Prepend 0.0 to |positions|, as required by Skia.
        if (positions->empty() && p0 != 0.0) {
            positions->push_back(0.0);
            colors->push_back(c0);
        }
        positions->push_back(p0);
        colors->push_back(c0);
        positions->push_back(p1);
        colors->push_back(c1);
    }

    // Creates a SkShader to fade the text, with |left_part| specifying the left
    // fade effect, if any, and |right_part| specifying the right fade effect.
    skia::RefPtr<SkShader> CreateFadeShader(const FontList& font_list,
        const Rect& text_rect,
        const Rect& left_part,
        const Rect& right_part,
        SkColor color)
    {
        // In general, fade down to 0 alpha.  But when the available width is less
        // than four characters, linearly ramp up the fade target alpha to as high as
        // 20% at zero width.  This allows the user to see the last faded characters a
        // little better when there are only a few characters shown.
        const float width_fraction = text_rect.width() / static_cast<float>(font_list.GetExpectedTextWidth(4));
        const SkAlpha kAlphaAtZeroWidth = 51;
        const SkAlpha alpha = (width_fraction < 1) ? static_cast<SkAlpha>(round((1 - width_fraction) * kAlphaAtZeroWidth)) : 0;
        const SkColor fade_color = SkColorSetA(color, alpha);

        std::vector<SkScalar> positions;
        std::vector<SkColor> colors;

        if (!left_part.IsEmpty())
            AddFadeEffect(text_rect, left_part, fade_color, color,
                &positions, &colors);
        if (!right_part.IsEmpty())
            AddFadeEffect(text_rect, right_part, color, fade_color,
                &positions, &colors);
        DCHECK(!positions.empty());

        // Terminate |positions| with 1.0, as required by Skia.
        if (positions.back() != 1.0) {
            positions.push_back(1.0);
            colors.push_back(colors.back());
        }

        const SkPoint points[2] = { PointToSkPoint(text_rect.origin()),
            PointToSkPoint(text_rect.top_right()) };
        return skia::AdoptRef(
            SkGradientShader::CreateLinear(&points[0], &colors[0], &positions[0],
                colors.size(), SkShader::kClamp_TileMode));
    }

    // Converts a FontRenderParams::Hinting value to the corresponding
    // SkPaint::Hinting value.
    SkPaint::Hinting FontRenderParamsHintingToSkPaintHinting(
        FontRenderParams::Hinting params_hinting)
    {
        switch (params_hinting) {
        case FontRenderParams::HINTING_NONE:
            return SkPaint::kNo_Hinting;
        case FontRenderParams::HINTING_SLIGHT:
            return SkPaint::kSlight_Hinting;
        case FontRenderParams::HINTING_MEDIUM:
            return SkPaint::kNormal_Hinting;
        case FontRenderParams::HINTING_FULL:
            return SkPaint::kFull_Hinting;
        }
        return SkPaint::kNo_Hinting;
    }

    // Make sure ranges don't break text graphemes.  If a range in |break_list|
    // does break a grapheme in |render_text|, the range will be slightly
    // extended to encompass the grapheme.
    template <typename T>
    void RestoreBreakList(RenderText* render_text, BreakList<T>* break_list)
    {
        break_list->SetMax(render_text->text().length());
        Range range;
        while (range.end() < break_list->max()) {
            const auto& current_break = break_list->GetBreak(range.end());
            range = break_list->GetRange(current_break);
            if (range.end() < break_list->max() && !render_text->IsValidCursorIndex(range.end())) {
                range.set_end(
                    render_text->IndexOfAdjacentGrapheme(range.end(), CURSOR_FORWARD));
                break_list->ApplyValue(current_break->second, range);
            }
        }
    }

} // namespace

namespace internal {

    // Value of |underline_thickness_| that indicates that underline metrics have
    // not been set explicitly.
    const SkScalar kUnderlineMetricsNotSet = -1.0f;

    SkiaTextRenderer::SkiaTextRenderer(Canvas* canvas)
        : canvas_(canvas)
        , canvas_skia_(canvas->sk_canvas())
        , underline_thickness_(kUnderlineMetricsNotSet)
        , underline_position_(0.0f)
    {
        DCHECK(canvas_skia_);
        paint_.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
        paint_.setStyle(SkPaint::kFill_Style);
        paint_.setAntiAlias(true);
        paint_.setSubpixelText(true);
        paint_.setLCDRenderText(true);
        paint_.setHinting(SkPaint::kNormal_Hinting);
    }

    SkiaTextRenderer::~SkiaTextRenderer()
    {
    }

    void SkiaTextRenderer::SetDrawLooper(SkDrawLooper* draw_looper)
    {
        paint_.setLooper(draw_looper);
    }

    void SkiaTextRenderer::SetFontRenderParams(const FontRenderParams& params,
        bool subpixel_rendering_suppressed)
    {
        ApplyRenderParams(params, subpixel_rendering_suppressed, &paint_);
    }

    void SkiaTextRenderer::SetTypeface(SkTypeface* typeface)
    {
        paint_.setTypeface(typeface);
    }

    void SkiaTextRenderer::SetTextSize(SkScalar size)
    {
        paint_.setTextSize(size);
    }

    void SkiaTextRenderer::SetFontWithStyle(const Font& font, int style)
    {
        skia::RefPtr<SkTypeface> typeface = CreateSkiaTypeface(font, style);
        if (typeface) {
            // |paint_| adds its own ref. So don't |release()| it from the ref ptr here.
            SetTypeface(typeface.get());

            // Enable fake bold text if bold style is needed but new typeface does not
            // have it.
            paint_.setFakeBoldText((style & Font::BOLD) && !typeface->isBold());
        }
    }

    void SkiaTextRenderer::SetForegroundColor(SkColor foreground)
    {
        paint_.setColor(foreground);
    }

    void SkiaTextRenderer::SetShader(SkShader* shader)
    {
        paint_.setShader(shader);
    }

    void SkiaTextRenderer::SetUnderlineMetrics(SkScalar thickness,
        SkScalar position)
    {
        underline_thickness_ = thickness;
        underline_position_ = position;
    }

    void SkiaTextRenderer::DrawPosText(const SkPoint* pos,
        const uint16_t* glyphs,
        size_t glyph_count)
    {
        const size_t byte_length = glyph_count * sizeof(glyphs[0]);
        canvas_skia_->drawPosText(&glyphs[0], byte_length, &pos[0], paint_);
    }

    void SkiaTextRenderer::DrawDecorations(int x, int y, int width, bool underline,
        bool strike, bool diagonal_strike)
    {
        if (underline)
            DrawUnderline(x, y, width);
        if (strike)
            DrawStrike(x, y, width);
        if (diagonal_strike) {
            if (!diagonal_)
                diagonal_.reset(new DiagonalStrike(canvas_, Point(x, y), paint_));
            diagonal_->AddPiece(width, paint_.getColor());
        } else if (diagonal_) {
            EndDiagonalStrike();
        }
    }

    void SkiaTextRenderer::EndDiagonalStrike()
    {
        if (diagonal_) {
            diagonal_->Draw();
            diagonal_.reset();
        }
    }

    void SkiaTextRenderer::DrawUnderline(int x, int y, int width)
    {
        SkScalar x_scalar = SkIntToScalar(x);
        SkRect r = SkRect::MakeLTRB(
            x_scalar, y + underline_position_, x_scalar + width,
            y + underline_position_ + underline_thickness_);
        if (underline_thickness_ == kUnderlineMetricsNotSet) {
            const SkScalar text_size = paint_.getTextSize();
            r.fTop = SkScalarMulAdd(text_size, kUnderlineOffset, y);
            r.fBottom = r.fTop + SkScalarMul(text_size, kLineThickness);
        }
        canvas_skia_->drawRect(r, paint_);
    }

    void SkiaTextRenderer::DrawStrike(int x, int y, int width) const
    {
        const SkScalar text_size = paint_.getTextSize();
        const SkScalar height = SkScalarMul(text_size, kLineThickness);
        const SkScalar offset = SkScalarMulAdd(text_size, kStrikeThroughOffset, y);
        SkScalar x_scalar = SkIntToScalar(x);
        const SkRect r = SkRect::MakeLTRB(x_scalar, offset, x_scalar + width, offset + height);
        canvas_skia_->drawRect(r, paint_);
    }

    SkiaTextRenderer::DiagonalStrike::DiagonalStrike(Canvas* canvas,
        Point start,
        const SkPaint& paint)
        : canvas_(canvas)
        , start_(start)
        , paint_(paint)
        , total_length_(0)
    {
    }

    SkiaTextRenderer::DiagonalStrike::~DiagonalStrike()
    {
    }

    void SkiaTextRenderer::DiagonalStrike::AddPiece(int length, SkColor color)
    {
        pieces_.push_back(Piece(length, color));
        total_length_ += length;
    }

    void SkiaTextRenderer::DiagonalStrike::Draw()
    {
        const SkScalar text_size = paint_.getTextSize();
        const SkScalar offset = SkScalarMul(text_size, kDiagonalStrikeMarginOffset);
        const int thickness = SkScalarCeilToInt(SkScalarMul(text_size, kLineThickness) * 2);
        const int height = SkScalarCeilToInt(text_size - offset);
        const Point end = start_ + Vector2d(total_length_, -height);
        const int clip_height = height + 2 * thickness;

        paint_.setAntiAlias(true);
        paint_.setStrokeWidth(SkIntToScalar(thickness));

        const bool clipped = pieces_.size() > 1;
        SkCanvas* sk_canvas = canvas_->sk_canvas();
        int x = start_.x();

        for (size_t i = 0; i < pieces_.size(); ++i) {
            paint_.setColor(pieces_[i].second);

            if (clipped) {
                canvas_->Save();
                sk_canvas->clipRect(RectToSkRect(
                    Rect(x, end.y() - thickness, pieces_[i].first, clip_height)));
            }

            canvas_->DrawLine(start_, end, paint_);

            if (clipped)
                canvas_->Restore();

            x += pieces_[i].first;
        }
    }

    StyleIterator::StyleIterator(const BreakList<SkColor>& colors,
        const BreakList<BaselineStyle>& baselines,
        const std::vector<BreakList<bool>>& styles)
        : colors_(colors)
        , baselines_(baselines)
        , styles_(styles)
    {
        color_ = colors_.breaks().begin();
        baseline_ = baselines_.breaks().begin();
        for (size_t i = 0; i < styles_.size(); ++i)
            style_.push_back(styles_[i].breaks().begin());
    }

    StyleIterator::~StyleIterator() { }

    Range StyleIterator::GetRange() const
    {
        Range range(colors_.GetRange(color_));
        range = range.Intersect(baselines_.GetRange(baseline_));
        for (size_t i = 0; i < NUM_TEXT_STYLES; ++i)
            range = range.Intersect(styles_[i].GetRange(style_[i]));
        return range;
    }

    void StyleIterator::UpdatePosition(size_t position)
    {
        color_ = colors_.GetBreak(position);
        baseline_ = baselines_.GetBreak(position);
        for (size_t i = 0; i < NUM_TEXT_STYLES; ++i)
            style_[i] = styles_[i].GetBreak(position);
    }

    LineSegment::LineSegment()
        : run(0)
    {
    }

    LineSegment::~LineSegment() { }

    Line::Line()
        : preceding_heights(0)
        , baseline(0)
    {
    }

    Line::~Line() { }

#if !defined(OS_MACOSX)
    skia::RefPtr<SkTypeface> CreateSkiaTypeface(const gfx::Font& font, int style)
    {
        SkTypeface::Style skia_style = ConvertFontStyleToSkiaTypefaceStyle(style);
        return skia::AdoptRef(
            SkTypeface::CreateFromName(font.GetFontName().c_str(), skia_style));
    }
#endif

    void ApplyRenderParams(const FontRenderParams& params,
        bool subpixel_rendering_suppressed,
        SkPaint* paint)
    {
        paint->setAntiAlias(params.antialiasing);
        paint->setLCDRenderText(!subpixel_rendering_suppressed && params.subpixel_rendering != FontRenderParams::SUBPIXEL_RENDERING_NONE);
        paint->setSubpixelText(params.subpixel_positioning);
        paint->setAutohinted(params.autohinter);
        paint->setHinting(FontRenderParamsHintingToSkPaintHinting(params.hinting));
    }

} // namespace internal

RenderText::~RenderText()
{
}

// static
RenderText* RenderText::CreateInstance()
{
#if defined(OS_MACOSX)
    static const bool use_native = !base::CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kEnableHarfBuzzRenderText);
    if (use_native)
        return new RenderTextMac;
#endif // defined(OS_MACOSX)
    return new RenderTextHarfBuzz;
}

// static
RenderText* RenderText::CreateInstanceForEditing()
{
    return new RenderTextHarfBuzz;
}

void RenderText::SetText(const base::string16& text)
{
    DCHECK(!composition_range_.IsValid());
    if (text_ == text)
        return;
    text_ = text;
    UpdateStyleLengths();

    // Clear style ranges as they might break new text graphemes and apply
    // the first style to the whole text instead.
    colors_.SetValue(colors_.breaks().begin()->second);
    baselines_.SetValue(baselines_.breaks().begin()->second);
    for (size_t style = 0; style < NUM_TEXT_STYLES; ++style)
        styles_[style].SetValue(styles_[style].breaks().begin()->second);
    cached_bounds_and_offset_valid_ = false;

    // Reset selection model. SetText should always followed by SetSelectionModel
    // or SetCursorPosition in upper layer.
    SetSelectionModel(SelectionModel());

    // Invalidate the cached text direction if it depends on the text contents.
    if (directionality_mode_ == DIRECTIONALITY_FROM_TEXT)
        text_direction_ = base::i18n::UNKNOWN_DIRECTION;

    obscured_reveal_index_ = -1;
    OnTextAttributeChanged();
}

void RenderText::AppendText(const base::string16& text)
{
    text_ += text;
    UpdateStyleLengths();
    cached_bounds_and_offset_valid_ = false;
    obscured_reveal_index_ = -1;
    OnTextAttributeChanged();
}

void RenderText::SetHorizontalAlignment(HorizontalAlignment alignment)
{
    if (horizontal_alignment_ != alignment) {
        horizontal_alignment_ = alignment;
        display_offset_ = Vector2d();
        cached_bounds_and_offset_valid_ = false;
    }
}

void RenderText::SetFontList(const FontList& font_list)
{
    font_list_ = font_list;
    const int font_style = font_list.GetFontStyle();
    SetStyle(BOLD, (font_style & gfx::Font::BOLD) != 0);
    SetStyle(ITALIC, (font_style & gfx::Font::ITALIC) != 0);
    SetStyle(UNDERLINE, (font_style & gfx::Font::UNDERLINE) != 0);
    baseline_ = kInvalidBaseline;
    cached_bounds_and_offset_valid_ = false;
    OnLayoutTextAttributeChanged(false);
}

void RenderText::SetCursorEnabled(bool cursor_enabled)
{
    cursor_enabled_ = cursor_enabled;
    cached_bounds_and_offset_valid_ = false;
}

void RenderText::ToggleInsertMode()
{
    insert_mode_ = !insert_mode_;
    cached_bounds_and_offset_valid_ = false;
}

void RenderText::SetObscured(bool obscured)
{
    if (obscured != obscured_) {
        obscured_ = obscured;
        obscured_reveal_index_ = -1;
        cached_bounds_and_offset_valid_ = false;
        OnTextAttributeChanged();
    }
}

void RenderText::SetObscuredRevealIndex(int index)
{
    if (obscured_reveal_index_ == index)
        return;

    obscured_reveal_index_ = index;
    cached_bounds_and_offset_valid_ = false;
    OnTextAttributeChanged();
}

void RenderText::SetMultiline(bool multiline)
{
    if (multiline != multiline_) {
        multiline_ = multiline;
        cached_bounds_and_offset_valid_ = false;
        lines_.clear();
        OnTextAttributeChanged();
    }
}

void RenderText::SetWordWrapBehavior(WordWrapBehavior behavior)
{
    if (word_wrap_behavior_ == behavior)
        return;
    word_wrap_behavior_ = behavior;
    if (multiline_) {
        cached_bounds_and_offset_valid_ = false;
        lines_.clear();
        OnTextAttributeChanged();
    }
}

void RenderText::SetReplaceNewlineCharsWithSymbols(bool replace)
{
    if (replace_newline_chars_with_symbols_ == replace)
        return;
    replace_newline_chars_with_symbols_ = replace;
    cached_bounds_and_offset_valid_ = false;
    OnTextAttributeChanged();
}

void RenderText::SetMinLineHeight(int line_height)
{
    if (min_line_height_ == line_height)
        return;
    min_line_height_ = line_height;
    cached_bounds_and_offset_valid_ = false;
    lines_.clear();
    OnDisplayTextAttributeChanged();
}

void RenderText::SetElideBehavior(ElideBehavior elide_behavior)
{
    // TODO(skanuj) : Add a test for triggering layout change.
    if (elide_behavior_ != elide_behavior) {
        elide_behavior_ = elide_behavior;
        OnDisplayTextAttributeChanged();
    }
}

void RenderText::SetDisplayRect(const Rect& r)
{
    if (r != display_rect_) {
        display_rect_ = r;
        baseline_ = kInvalidBaseline;
        cached_bounds_and_offset_valid_ = false;
        lines_.clear();
        if (elide_behavior_ != NO_ELIDE && elide_behavior_ != FADE_TAIL) {
            OnDisplayTextAttributeChanged();
        }
    }
}

void RenderText::SetCursorPosition(size_t position)
{
    MoveCursorTo(position, false);
}

void RenderText::MoveCursor(BreakType break_type,
    VisualCursorDirection direction,
    bool select)
{
    SelectionModel cursor(cursor_position(), selection_model_.caret_affinity());
    // Cancelling a selection moves to the edge of the selection.
    if (break_type != LINE_BREAK && !selection().is_empty() && !select) {
        SelectionModel selection_start = GetSelectionModelForSelectionStart();
        int start_x = GetCursorBounds(selection_start, true).x();
        int cursor_x = GetCursorBounds(cursor, true).x();
        // Use the selection start if it is left (when |direction| is CURSOR_LEFT)
        // or right (when |direction| is CURSOR_RIGHT) of the selection end.
        if (direction == CURSOR_RIGHT ? start_x > cursor_x : start_x < cursor_x)
            cursor = selection_start;
        // Use the nearest word boundary in the proper |direction| for word breaks.
        if (break_type == WORD_BREAK)
            cursor = GetAdjacentSelectionModel(cursor, break_type, direction);
        // Use an adjacent selection model if the cursor is not at a valid position.
        if (!IsValidCursorIndex(cursor.caret_pos()))
            cursor = GetAdjacentSelectionModel(cursor, CHARACTER_BREAK, direction);
    } else {
        cursor = GetAdjacentSelectionModel(cursor, break_type, direction);
    }
    if (select)
        cursor.set_selection_start(selection().start());
    MoveCursorTo(cursor);
}

bool RenderText::MoveCursorTo(const SelectionModel& model)
{
    // Enforce valid selection model components.
    size_t text_length = text().length();
    Range range(std::min(model.selection().start(),
                    static_cast<uint32_t>(text_length)),
        std::min(model.caret_pos(), text_length));
    // The current model only supports caret positions at valid cursor indices.
    if (!IsValidCursorIndex(range.start()) || !IsValidCursorIndex(range.end()))
        return false;
    SelectionModel sel(range, model.caret_affinity());
    bool changed = sel != selection_model_;
    SetSelectionModel(sel);
    return changed;
}

bool RenderText::SelectRange(const Range& range)
{
    uint32_t text_length = static_cast<uint32_t>(text().length());
    Range sel(std::min(range.start(), text_length),
        std::min(range.end(), text_length));
    // Allow selection bounds at valid indicies amid multi-character graphemes.
    if (!IsValidLogicalIndex(sel.start()) || !IsValidLogicalIndex(sel.end()))
        return false;
    LogicalCursorDirection affinity = (sel.is_reversed() || sel.is_empty()) ? CURSOR_FORWARD : CURSOR_BACKWARD;
    SetSelectionModel(SelectionModel(sel, affinity));
    return true;
}

bool RenderText::IsPointInSelection(const Point& point)
{
    if (selection().is_empty())
        return false;
    SelectionModel cursor = FindCursorPosition(point);
    return RangeContainsCaret(
        selection(), cursor.caret_pos(), cursor.caret_affinity());
}

void RenderText::ClearSelection()
{
    SetSelectionModel(SelectionModel(cursor_position(),
        selection_model_.caret_affinity()));
}

void RenderText::SelectAll(bool reversed)
{
    const size_t length = text().length();
    const Range all = reversed ? Range(length, 0) : Range(0, length);
    const bool success = SelectRange(all);
    DCHECK(success);
}

void RenderText::SelectWord()
{
    if (obscured_) {
        SelectAll(false);
        return;
    }

    size_t selection_max = selection().GetMax();

    base::i18n::BreakIterator iter(text(), base::i18n::BreakIterator::BREAK_WORD);
    bool success = iter.Init();
    DCHECK(success);
    if (!success)
        return;

    size_t selection_min = selection().GetMin();
    if (selection_min == text().length() && selection_min != 0)
        --selection_min;

    for (; selection_min != 0; --selection_min) {
        if (iter.IsStartOfWord(selection_min) || iter.IsEndOfWord(selection_min))
            break;
    }

    if (selection_min == selection_max && selection_max != text().length())
        ++selection_max;

    for (; selection_max < text().length(); ++selection_max)
        if (iter.IsEndOfWord(selection_max) || iter.IsStartOfWord(selection_max))
            break;

    const bool reversed = selection().is_reversed();
    MoveCursorTo(reversed ? selection_max : selection_min, false);
    MoveCursorTo(reversed ? selection_min : selection_max, true);
}

void RenderText::SetCompositionRange(const Range& composition_range)
{
    CHECK(!composition_range.IsValid() || Range(0, text_.length()).Contains(composition_range));
    composition_range_.set_end(composition_range.end());
    composition_range_.set_start(composition_range.start());
    // TODO(oshima|msw): Altering composition underlines shouldn't
    // require layout changes. It's currently necessary because
    // RenderTextHarfBuzz paints text decorations by run, and
    // RenderTextMac applies all styles during layout.
    OnLayoutTextAttributeChanged(false);
}

void RenderText::SetColor(SkColor value)
{
    colors_.SetValue(value);
    OnTextColorChanged();
}

void RenderText::ApplyColor(SkColor value, const Range& range)
{
    colors_.ApplyValue(value, range);
    OnTextColorChanged();
}

void RenderText::SetBaselineStyle(BaselineStyle value)
{
    baselines_.SetValue(value);
}

void RenderText::ApplyBaselineStyle(BaselineStyle value, const Range& range)
{
    baselines_.ApplyValue(value, range);
}

void RenderText::SetStyle(TextStyle style, bool value)
{
    styles_[style].SetValue(value);

    cached_bounds_and_offset_valid_ = false;
    // TODO(oshima|msw): Not all style change requires layout changes.
    // Consider optimizing based on the type of change.
    OnLayoutTextAttributeChanged(false);
}

void RenderText::ApplyStyle(TextStyle style, bool value, const Range& range)
{
    // Do not change styles mid-grapheme to avoid breaking ligatures.
    const size_t start = IsValidCursorIndex(range.start()) ? range.start() : IndexOfAdjacentGrapheme(range.start(), CURSOR_BACKWARD);
    const size_t end = IsValidCursorIndex(range.end()) ? range.end() : IndexOfAdjacentGrapheme(range.end(), CURSOR_FORWARD);
    styles_[style].ApplyValue(value, Range(start, end));

    cached_bounds_and_offset_valid_ = false;
    // TODO(oshima|msw): Not all style change requires layout changes.
    // Consider optimizing based on the type of change.
    OnLayoutTextAttributeChanged(false);
}

bool RenderText::GetStyle(TextStyle style) const
{
    return (styles_[style].breaks().size() == 1) && styles_[style].breaks().front().second;
}

void RenderText::SetDirectionalityMode(DirectionalityMode mode)
{
    if (mode == directionality_mode_)
        return;

    directionality_mode_ = mode;
    text_direction_ = base::i18n::UNKNOWN_DIRECTION;
    cached_bounds_and_offset_valid_ = false;
    OnLayoutTextAttributeChanged(false);
}

base::i18n::TextDirection RenderText::GetDisplayTextDirection()
{
    return GetTextDirection(GetDisplayText());
}

VisualCursorDirection RenderText::GetVisualDirectionOfLogicalEnd()
{
    return GetDisplayTextDirection() == base::i18n::LEFT_TO_RIGHT ? CURSOR_RIGHT : CURSOR_LEFT;
}

SizeF RenderText::GetStringSizeF()
{
    return gfx::SizeF(GetStringSize());
}

float RenderText::GetContentWidthF()
{
    const float string_size = GetStringSizeF().width();
    // The cursor is drawn one pixel beyond the int-enclosed text bounds.
    return cursor_enabled_ ? std::ceil(string_size) + 1 : string_size;
}

int RenderText::GetContentWidth()
{
    return ToCeiledInt(GetContentWidthF());
}

int RenderText::GetBaseline()
{
    if (baseline_ == kInvalidBaseline)
        baseline_ = DetermineBaselineCenteringText(display_rect(), font_list());
    DCHECK_NE(kInvalidBaseline, baseline_);
    return baseline_;
}

void RenderText::Draw(Canvas* canvas)
{
    EnsureLayout();

    if (clip_to_display_rect()) {
        Rect clip_rect(display_rect());
        clip_rect.Inset(ShadowValue::GetMargin(shadows_));

        canvas->Save();
        canvas->ClipRect(clip_rect);
    }

    if (!text().empty() && focused())
        DrawSelection(canvas);

    if (cursor_enabled() && cursor_visible() && focused())
        DrawCursor(canvas, selection_model_);

    if (!text().empty()) {
        internal::SkiaTextRenderer renderer(canvas);
        DrawVisualText(&renderer);
    }

    if (clip_to_display_rect())
        canvas->Restore();
}

void RenderText::DrawCursor(Canvas* canvas, const SelectionModel& position)
{
    // Paint cursor. Replace cursor is drawn as rectangle for now.
    // TODO(msw): Draw a better cursor with a better indication of association.
    canvas->FillRect(GetCursorBounds(position, true), cursor_color_);
}

bool RenderText::IsValidLogicalIndex(size_t index) const
{
    // Check that the index is at a valid code point (not mid-surrgate-pair) and
    // that it's not truncated from the display text (its glyph may be shown).
    //
    // Indices within truncated text are disallowed so users can easily interact
    // with the underlying truncated text using the ellipsis as a proxy. This lets
    // users select all text, select the truncated text, and transition from the
    // last rendered glyph to the end of the text without getting invisible cursor
    // positions nor needing unbounded arrow key presses to traverse the ellipsis.
    return index == 0 || index == text().length() || (index < text().length() && (truncate_length_ == 0 || index < truncate_length_) && IsValidCodePointIndex(text(), index));
}

Rect RenderText::GetCursorBounds(const SelectionModel& caret,
    bool insert_mode)
{
    // TODO(ckocagil): Support multiline. This function should return the height
    //                 of the line the cursor is on. |GetStringSize()| now returns
    //                 the multiline size, eliminate its use here.

    EnsureLayout();
    size_t caret_pos = caret.caret_pos();
    DCHECK(IsValidLogicalIndex(caret_pos));
    // In overtype mode, ignore the affinity and always indicate that we will
    // overtype the next character.
    LogicalCursorDirection caret_affinity = insert_mode ? caret.caret_affinity() : CURSOR_FORWARD;
    int x = 0, width = 1;
    Size size = GetStringSize();
    if (caret_pos == (caret_affinity == CURSOR_BACKWARD ? 0 : text().length())) {
        // The caret is attached to the boundary. Always return a 1-dip width caret,
        // since there is nothing to overtype.
        if ((GetDisplayTextDirection() == base::i18n::RIGHT_TO_LEFT)
            == (caret_pos == 0)) {
            x = size.width();
        }
    } else {
        size_t grapheme_start = (caret_affinity == CURSOR_FORWARD) ? caret_pos : IndexOfAdjacentGrapheme(caret_pos, CURSOR_BACKWARD);
        Range xspan(GetGlyphBounds(grapheme_start));
        if (insert_mode) {
            x = (caret_affinity == CURSOR_BACKWARD) ? xspan.end() : xspan.start();
        } else { // overtype mode
            x = xspan.GetMin();
            width = xspan.length();
        }
    }
    return Rect(ToViewPoint(Point(x, 0)), Size(width, size.height()));
}

const Rect& RenderText::GetUpdatedCursorBounds()
{
    UpdateCachedBoundsAndOffset();
    return cursor_bounds_;
}

size_t RenderText::IndexOfAdjacentGrapheme(size_t index,
    LogicalCursorDirection direction)
{
    if (index > text().length())
        return text().length();

    EnsureLayout();

    if (direction == CURSOR_FORWARD) {
        while (index < text().length()) {
            index++;
            if (IsValidCursorIndex(index))
                return index;
        }
        return text().length();
    }

    while (index > 0) {
        index--;
        if (IsValidCursorIndex(index))
            return index;
    }
    return 0;
}

SelectionModel RenderText::GetSelectionModelForSelectionStart() const
{
    const Range& sel = selection();
    if (sel.is_empty())
        return selection_model_;
    return SelectionModel(sel.start(),
        sel.is_reversed() ? CURSOR_BACKWARD : CURSOR_FORWARD);
}

const Vector2d& RenderText::GetUpdatedDisplayOffset()
{
    UpdateCachedBoundsAndOffset();
    return display_offset_;
}

void RenderText::SetDisplayOffset(int horizontal_offset)
{
    const int extra_content = GetContentWidth() - display_rect_.width();
    const int cursor_width = cursor_enabled_ ? 1 : 0;

    int min_offset = 0;
    int max_offset = 0;
    if (extra_content > 0) {
        switch (GetCurrentHorizontalAlignment()) {
        case ALIGN_LEFT:
            min_offset = -extra_content;
            break;
        case ALIGN_RIGHT:
            max_offset = extra_content;
            break;
        case ALIGN_CENTER:
            // The extra space reserved for cursor at the end of the text is ignored
            // when centering text. So, to calculate the valid range for offset, we
            // exclude that extra space, calculate the range, and add it back to the
            // range (if cursor is enabled).
            min_offset = -(extra_content - cursor_width + 1) / 2 - cursor_width;
            max_offset = (extra_content - cursor_width) / 2;
            break;
        default:
            break;
        }
    }
    if (horizontal_offset < min_offset)
        horizontal_offset = min_offset;
    else if (horizontal_offset > max_offset)
        horizontal_offset = max_offset;

    cached_bounds_and_offset_valid_ = true;
    display_offset_.set_x(horizontal_offset);
    cursor_bounds_ = GetCursorBounds(selection_model_, insert_mode_);
}

Vector2d RenderText::GetLineOffset(size_t line_number)
{
    Vector2d offset = display_rect().OffsetFromOrigin();
    // TODO(ckocagil): Apply the display offset for multiline scrolling.
    if (!multiline())
        offset.Add(GetUpdatedDisplayOffset());
    else
        offset.Add(Vector2d(0, lines_[line_number].preceding_heights));
    offset.Add(GetAlignmentOffset(line_number));
    return offset;
}

RenderText::RenderText()
    : horizontal_alignment_(base::i18n::IsRTL() ? ALIGN_RIGHT : ALIGN_LEFT)
    , directionality_mode_(DIRECTIONALITY_FROM_TEXT)
    , text_direction_(base::i18n::UNKNOWN_DIRECTION)
    , cursor_enabled_(true)
    , cursor_visible_(false)
    , insert_mode_(true)
    , cursor_color_(kDefaultColor)
    , selection_color_(kDefaultColor)
    , selection_background_focused_color_(kDefaultSelectionBackgroundColor)
    , focused_(false)
    , composition_range_(Range::InvalidRange())
    , colors_(kDefaultColor)
    , baselines_(NORMAL_BASELINE)
    , styles_(NUM_TEXT_STYLES)
    , composition_and_selection_styles_applied_(false)
    , obscured_(false)
    , obscured_reveal_index_(-1)
    , truncate_length_(0)
    , elide_behavior_(NO_ELIDE)
    , text_elided_(false)
    , min_line_height_(0)
    , multiline_(false)
    , word_wrap_behavior_(IGNORE_LONG_WORDS)
    , replace_newline_chars_with_symbols_(true)
    , subpixel_rendering_suppressed_(false)
    , clip_to_display_rect_(true)
    , baseline_(kInvalidBaseline)
    , cached_bounds_and_offset_valid_(false)
{
}

SelectionModel RenderText::GetAdjacentSelectionModel(
    const SelectionModel& current,
    BreakType break_type,
    VisualCursorDirection direction)
{
    EnsureLayout();

    if (break_type == LINE_BREAK || text().empty())
        return EdgeSelectionModel(direction);
    if (break_type == CHARACTER_BREAK)
        return AdjacentCharSelectionModel(current, direction);
    DCHECK(break_type == WORD_BREAK);
    return AdjacentWordSelectionModel(current, direction);
}

SelectionModel RenderText::EdgeSelectionModel(
    VisualCursorDirection direction)
{
    if (direction == GetVisualDirectionOfLogicalEnd())
        return SelectionModel(text().length(), CURSOR_FORWARD);
    return SelectionModel(0, CURSOR_BACKWARD);
}

void RenderText::SetSelectionModel(const SelectionModel& model)
{
    DCHECK_LE(model.selection().GetMax(), text().length());
    selection_model_ = model;
    cached_bounds_and_offset_valid_ = false;
}

void RenderText::OnTextColorChanged()
{
}

void RenderText::UpdateDisplayText(float text_width)
{
    // TODO(oshima): Consider support eliding for multi-line text.
    // This requires max_line support first.
    if (multiline_ || elide_behavior() == NO_ELIDE || elide_behavior() == FADE_TAIL || text_width < display_rect_.width() || layout_text_.empty()) {
        text_elided_ = false;
        display_text_.clear();
        return;
    }

    // This doesn't trim styles so ellipsis may get rendered as a different
    // style than the preceding text. See crbug.com/327850.
    display_text_.assign(Elide(layout_text_,
        text_width,
        static_cast<float>(display_rect_.width()),
        elide_behavior_));

    text_elided_ = display_text_ != layout_text_;
    if (!text_elided_)
        display_text_.clear();
}

const BreakList<size_t>& RenderText::GetLineBreaks()
{
    if (line_breaks_.max() != 0)
        return line_breaks_;

    const base::string16& layout_text = GetDisplayText();
    const size_t text_length = layout_text.length();
    line_breaks_.SetValue(0);
    line_breaks_.SetMax(text_length);
    base::i18n::BreakIterator iter(layout_text,
        base::i18n::BreakIterator::BREAK_LINE);
    const bool success = iter.Init();
    DCHECK(success);
    if (success) {
        do {
            line_breaks_.ApplyValue(iter.pos(), Range(iter.pos(), text_length));
        } while (iter.Advance());
    }
    return line_breaks_;
}

void RenderText::ApplyCompositionAndSelectionStyles()
{
    // Save the underline and color breaks to undo the temporary styles later.
    DCHECK(!composition_and_selection_styles_applied_);
    saved_colors_ = colors_;
    saved_underlines_ = styles_[UNDERLINE];

    // Apply an underline to the composition range in |underlines|.
    if (composition_range_.IsValid() && !composition_range_.is_empty())
        styles_[UNDERLINE].ApplyValue(true, composition_range_);

    // Apply the selected text color to the [un-reversed] selection range.
    if (!selection().is_empty() && focused()) {
        const Range range(selection().GetMin(), selection().GetMax());
        colors_.ApplyValue(selection_color_, range);
    }
    composition_and_selection_styles_applied_ = true;
}

void RenderText::UndoCompositionAndSelectionStyles()
{
    // Restore the underline and color breaks to undo the temporary styles.
    DCHECK(composition_and_selection_styles_applied_);
    colors_ = saved_colors_;
    styles_[UNDERLINE] = saved_underlines_;
    composition_and_selection_styles_applied_ = false;
}

Point RenderText::ToTextPoint(const Point& point)
{
    return point - GetLineOffset(0);
    // TODO(ckocagil): Convert multiline view space points to text space.
}

Point RenderText::ToViewPoint(const Point& point)
{
    if (!multiline())
        return point + GetLineOffset(0);

    // TODO(ckocagil): Traverse individual line segments for RTL support.
    DCHECK(!lines_.empty());
    int x = point.x();
    size_t line = 0;
    for (; line < lines_.size() && x > lines_[line].size.width(); ++line)
        x -= lines_[line].size.width();
    return Point(x, point.y()) + GetLineOffset(line);
}

std::vector<Rect> RenderText::TextBoundsToViewBounds(const Range& x)
{
    std::vector<Rect> rects;

    if (!multiline()) {
        rects.push_back(Rect(ToViewPoint(Point(x.GetMin(), 0)),
            Size(x.length(), GetStringSize().height())));
        return rects;
    }

    EnsureLayout();

    // Each line segment keeps its position in text coordinates. Traverse all line
    // segments and if the segment intersects with the given range, add the view
    // rect corresponding to the intersection to |rects|.
    for (size_t line = 0; line < lines_.size(); ++line) {
        int line_x = 0;
        const Vector2d offset = GetLineOffset(line);
        for (size_t i = 0; i < lines_[line].segments.size(); ++i) {
            const internal::LineSegment* segment = &lines_[line].segments[i];
            const Range intersection = segment->x_range.Intersect(x).Ceil();
            if (!intersection.is_empty()) {
                Rect rect(line_x + intersection.start() - segment->x_range.start(),
                    0, intersection.length(), lines_[line].size.height());
                rects.push_back(rect + offset);
            }
            line_x += segment->x_range.length();
        }
    }

    return rects;
}

HorizontalAlignment RenderText::GetCurrentHorizontalAlignment()
{
    if (horizontal_alignment_ != ALIGN_TO_HEAD)
        return horizontal_alignment_;
    return GetDisplayTextDirection() == base::i18n::RIGHT_TO_LEFT ? ALIGN_RIGHT : ALIGN_LEFT;
}

Vector2d RenderText::GetAlignmentOffset(size_t line_number)
{
    // TODO(ckocagil): Enable |lines_| usage on RenderTextMac.
    if (MultilineSupported() && multiline_)
        DCHECK_LT(line_number, lines_.size());
    Vector2d offset;
    HorizontalAlignment horizontal_alignment = GetCurrentHorizontalAlignment();
    if (horizontal_alignment != ALIGN_LEFT) {
        const int width = multiline_ ? std::ceil(lines_[line_number].size.width()) + (cursor_enabled_ ? 1 : 0) : GetContentWidth();
        offset.set_x(display_rect().width() - width);
        // Put any extra margin pixel on the left to match legacy behavior.
        if (horizontal_alignment == ALIGN_CENTER)
            offset.set_x((offset.x() + 1) / 2);
    }

    // Vertically center the text.
    if (multiline_) {
        const int text_height = lines_.back().preceding_heights + lines_.back().size.height();
        offset.set_y((display_rect_.height() - text_height) / 2);
    } else {
        offset.set_y(GetBaseline() - GetDisplayTextBaseline());
    }

    return offset;
}

void RenderText::ApplyFadeEffects(internal::SkiaTextRenderer* renderer)
{
    const int width = display_rect().width();
    if (multiline() || elide_behavior_ != FADE_TAIL || GetContentWidth() <= width)
        return;

    const int gradient_width = CalculateFadeGradientWidth(font_list(), width);
    if (gradient_width == 0)
        return;

    HorizontalAlignment horizontal_alignment = GetCurrentHorizontalAlignment();
    Rect solid_part = display_rect();
    Rect left_part;
    Rect right_part;
    if (horizontal_alignment != ALIGN_LEFT) {
        left_part = solid_part;
        left_part.Inset(0, 0, solid_part.width() - gradient_width, 0);
        solid_part.Inset(gradient_width, 0, 0, 0);
    }
    if (horizontal_alignment != ALIGN_RIGHT) {
        right_part = solid_part;
        right_part.Inset(solid_part.width() - gradient_width, 0, 0, 0);
        solid_part.Inset(0, 0, gradient_width, 0);
    }

    Rect text_rect = display_rect();
    text_rect.Inset(GetAlignmentOffset(0).x(), 0, 0, 0);

    // TODO(msw): Use the actual text colors corresponding to each faded part.
    skia::RefPtr<SkShader> shader = CreateFadeShader(font_list(), text_rect, left_part, right_part,
        colors_.breaks().front().second);
    if (shader)
        renderer->SetShader(shader.get());
}

void RenderText::ApplyTextShadows(internal::SkiaTextRenderer* renderer)
{
    skia::RefPtr<SkDrawLooper> looper = CreateShadowDrawLooper(shadows_);
    renderer->SetDrawLooper(looper.get());
}

base::i18n::TextDirection RenderText::GetTextDirection(
    const base::string16& text)
{
    if (text_direction_ == base::i18n::UNKNOWN_DIRECTION) {
        switch (directionality_mode_) {
        case DIRECTIONALITY_FROM_TEXT:
            // Derive the direction from the display text, which differs from text()
            // in the case of obscured (password) textfields.
            text_direction_ = base::i18n::GetFirstStrongCharacterDirection(text);
            break;
        case DIRECTIONALITY_FROM_UI:
            text_direction_ = base::i18n::IsRTL() ? base::i18n::RIGHT_TO_LEFT : base::i18n::LEFT_TO_RIGHT;
            break;
        case DIRECTIONALITY_FORCE_LTR:
            text_direction_ = base::i18n::LEFT_TO_RIGHT;
            break;
        case DIRECTIONALITY_FORCE_RTL:
            text_direction_ = base::i18n::RIGHT_TO_LEFT;
            break;
        default:
            NOTREACHED();
        }
    }

    return text_direction_;
}

size_t RenderText::TextIndexToGivenTextIndex(const base::string16& given_text,
    size_t index) const
{
    DCHECK(given_text == layout_text() || given_text == display_text());
    DCHECK_LE(index, text().length());
    ptrdiff_t i = obscured() ? UTF16IndexToOffset(text(), 0, index) : index;
    CHECK_GE(i, 0);
    // Clamp indices to the length of the given layout or display text.
    return std::min<size_t>(given_text.length(), i);
}

void RenderText::UpdateStyleLengths()
{
    const size_t text_length = text_.length();
    colors_.SetMax(text_length);
    baselines_.SetMax(text_length);
    for (size_t style = 0; style < NUM_TEXT_STYLES; ++style)
        styles_[style].SetMax(text_length);
}

// static
bool RenderText::RangeContainsCaret(const Range& range,
    size_t caret_pos,
    LogicalCursorDirection caret_affinity)
{
    // NB: exploits unsigned wraparound (WG14/N1124 section 6.2.5 paragraph 9).
    size_t adjacent = (caret_affinity == CURSOR_BACKWARD) ? caret_pos - 1 : caret_pos + 1;
    return range.Contains(Range(caret_pos, adjacent));
}

void RenderText::MoveCursorTo(size_t position, bool select)
{
    size_t cursor = std::min(position, text().length());
    if (IsValidCursorIndex(cursor))
        SetSelectionModel(SelectionModel(
            Range(select ? selection().start() : cursor, cursor),
            (cursor == 0) ? CURSOR_FORWARD : CURSOR_BACKWARD));
}

void RenderText::OnTextAttributeChanged()
{
    layout_text_.clear();
    display_text_.clear();
    text_elided_ = false;
    line_breaks_.SetMax(0);

    if (obscured_) {
        size_t obscured_text_length = static_cast<size_t>(UTF16IndexToOffset(text_, 0, text_.length()));
        layout_text_.assign(obscured_text_length, kPasswordReplacementChar);

        if (obscured_reveal_index_ >= 0 && obscured_reveal_index_ < static_cast<int>(text_.length())) {
            // Gets the index range in |text_| to be revealed.
            size_t start = obscured_reveal_index_;
            U16_SET_CP_START(text_.data(), 0, start);
            size_t end = start;
            UChar32 unused_char;
            U16_NEXT(text_.data(), end, text_.length(), unused_char);

            // Gets the index in |layout_text_| to be replaced.
            const size_t cp_start = static_cast<size_t>(UTF16IndexToOffset(text_, 0, start));
            if (layout_text_.length() > cp_start)
                layout_text_.replace(cp_start, 1, text_.substr(start, end - start));
        }
    } else {
        layout_text_ = text_;
    }

    const base::string16& text = layout_text_;
    if (truncate_length_ > 0 && truncate_length_ < text.length()) {
        // Truncate the text at a valid character break and append an ellipsis.
        icu::StringCharacterIterator iter(text.c_str());
        // Respect ELIDE_HEAD and ELIDE_MIDDLE preferences during truncation.
        if (elide_behavior_ == ELIDE_HEAD) {
            iter.setIndex32(text.length() - truncate_length_ + 1);
            layout_text_.assign(kEllipsisUTF16 + text.substr(iter.getIndex()));
        } else if (elide_behavior_ == ELIDE_MIDDLE) {
            iter.setIndex32(truncate_length_ / 2);
            const size_t ellipsis_start = iter.getIndex();
            iter.setIndex32(text.length() - (truncate_length_ / 2));
            const size_t ellipsis_end = iter.getIndex();
            DCHECK_LE(ellipsis_start, ellipsis_end);
            layout_text_.assign(text.substr(0, ellipsis_start) + kEllipsisUTF16 + text.substr(ellipsis_end));
        } else {
            iter.setIndex32(truncate_length_ - 1);
            layout_text_.assign(text.substr(0, iter.getIndex()) + kEllipsisUTF16);
        }
    }
    static const base::char16 kNewline[] = { '\n', 0 };
    static const base::char16 kNewlineSymbol[] = { 0x2424, 0 };
    if (!multiline_ && replace_newline_chars_with_symbols_)
        base::ReplaceChars(layout_text_, kNewline, kNewlineSymbol, &layout_text_);

    OnLayoutTextAttributeChanged(true);
}

base::string16 RenderText::Elide(const base::string16& text,
    float text_width,
    float available_width,
    ElideBehavior behavior)
{
    if (available_width <= 0 || text.empty())
        return base::string16();
    if (behavior == ELIDE_EMAIL)
        return ElideEmail(text, available_width);
    if (text_width > 0 && text_width < available_width)
        return text;

    TRACE_EVENT0("ui", "RenderText::Elide");

    // Create a RenderText copy with attributes that affect the rendering width.
    scoped_ptr<RenderText> render_text = CreateInstanceOfSameType();
    render_text->SetFontList(font_list_);
    render_text->SetDirectionalityMode(directionality_mode_);
    render_text->SetCursorEnabled(cursor_enabled_);
    render_text->set_truncate_length(truncate_length_);
    render_text->styles_ = styles_;
    render_text->baselines_ = baselines_;
    render_text->colors_ = colors_;
    if (text_width == 0) {
        render_text->SetText(text);
        text_width = render_text->GetContentWidthF();
    }
    if (text_width <= available_width)
        return text;

    const base::string16 ellipsis = base::string16(kEllipsisUTF16);
    const bool insert_ellipsis = (behavior != TRUNCATE);
    const bool elide_in_middle = (behavior == ELIDE_MIDDLE);
    const bool elide_at_beginning = (behavior == ELIDE_HEAD);

    if (insert_ellipsis) {
        render_text->SetText(ellipsis);
        const float ellipsis_width = render_text->GetContentWidthF();
        if (ellipsis_width > available_width)
            return base::string16();
    }

    StringSlicer slicer(text, ellipsis, elide_in_middle, elide_at_beginning);

    // Use binary search to compute the elided text.
    size_t lo = 0;
    size_t hi = text.length() - 1;
    const base::i18n::TextDirection text_direction = GetTextDirection(text);
    for (size_t guess = (lo + hi) / 2; lo <= hi; guess = (lo + hi) / 2) {
        // Restore colors. They will be truncated to size by SetText.
        render_text->colors_ = colors_;
        base::string16 new_text = slicer.CutString(guess, insert_ellipsis && behavior != ELIDE_TAIL);
        render_text->SetText(new_text);

        // This has to be an additional step so that the ellipsis is rendered with
        // same style as trailing part of the text.
        if (insert_ellipsis && behavior == ELIDE_TAIL) {
            // When ellipsis follows text whose directionality is not the same as that
            // of the whole text, it will be rendered with the directionality of the
            // whole text. Since we want ellipsis to indicate continuation of the
            // preceding text, we force the directionality of ellipsis to be same as
            // the preceding text using LTR or RTL markers.
            base::i18n::TextDirection trailing_text_direction = base::i18n::GetLastStrongCharacterDirection(new_text);
            new_text.append(ellipsis);
            if (trailing_text_direction != text_direction) {
                if (trailing_text_direction == base::i18n::LEFT_TO_RIGHT)
                    new_text += base::i18n::kLeftToRightMark;
                else
                    new_text += base::i18n::kRightToLeftMark;
            }
            render_text->SetText(new_text);
        }

        // Restore styles and baselines without breaking multi-character graphemes.
        render_text->styles_ = styles_;
        for (size_t style = 0; style < NUM_TEXT_STYLES; ++style)
            RestoreBreakList(render_text.get(), &render_text->styles_[style]);
        RestoreBreakList(render_text.get(), &render_text->baselines_);

        // We check the width of the whole desired string at once to ensure we
        // handle kerning/ligatures/etc. correctly.
        const float guess_width = render_text->GetContentWidthF();
        if (guess_width == available_width)
            break;
        if (guess_width > available_width) {
            hi = guess - 1;
            // Move back on the loop terminating condition when the guess is too wide.
            if (hi < lo)
                lo = hi;
        } else {
            lo = guess + 1;
        }
    }

    return render_text->text();
}

base::string16 RenderText::ElideEmail(const base::string16& email,
    float available_width)
{
    // The returned string will have at least one character besides the ellipsis
    // on either side of '@'; if that's impossible, a single ellipsis is returned.
    // If possible, only the username is elided. Otherwise, the domain is elided
    // in the middle, splitting available width equally with the elided username.
    // If the username is short enough that it doesn't need half the available
    // width, the elided domain will occupy that extra width.

    // Split the email into its local-part (username) and domain-part. The email
    // spec allows for @ symbols in the username under some special requirements,
    // but not in the domain part, so splitting at the last @ symbol is safe.
    const size_t split_index = email.find_last_of('@');
    DCHECK_NE(split_index, base::string16::npos);
    base::string16 username = email.substr(0, split_index);
    base::string16 domain = email.substr(split_index + 1);
    DCHECK(!username.empty());
    DCHECK(!domain.empty());

    // Subtract the @ symbol from the available width as it is mandatory.
    const base::string16 kAtSignUTF16 = base::ASCIIToUTF16("@");
    available_width -= GetStringWidthF(kAtSignUTF16, font_list());

    // Check whether eliding the domain is necessary: if eliding the username
    // is sufficient, the domain will not be elided.
    const float full_username_width = GetStringWidthF(username, font_list());
    const float available_domain_width = available_width - std::min(full_username_width, GetStringWidthF(username.substr(0, 1) + kEllipsisUTF16, font_list()));
    if (GetStringWidthF(domain, font_list()) > available_domain_width) {
        // Elide the domain so that it only takes half of the available width.
        // Should the username not need all the width available in its half, the
        // domain will occupy the leftover width.
        // If |desired_domain_width| is greater than |available_domain_width|: the
        // minimal username elision allowed by the specifications will not fit; thus
        // |desired_domain_width| must be <= |available_domain_width| at all cost.
        const float desired_domain_width = std::min<float>(available_domain_width,
            std::max<float>(available_width - full_username_width,
                available_width / 2));
        domain = Elide(domain, 0, desired_domain_width, ELIDE_MIDDLE);
        // Failing to elide the domain such that at least one character remains
        // (other than the ellipsis itself) remains: return a single ellipsis.
        if (domain.length() <= 1U)
            return base::string16(kEllipsisUTF16);
    }

    // Fit the username in the remaining width (at this point the elided username
    // is guaranteed to fit with at least one character remaining given all the
    // precautions taken earlier).
    available_width -= GetStringWidthF(domain, font_list());
    username = Elide(username, 0, available_width, ELIDE_TAIL);
    return username + kAtSignUTF16 + domain;
}

void RenderText::UpdateCachedBoundsAndOffset()
{
    if (cached_bounds_and_offset_valid_)
        return;

    // TODO(ckocagil): Add support for scrolling multiline text.

    int delta_x = 0;

    if (cursor_enabled()) {
        // When cursor is enabled, ensure it is visible. For this, set the valid
        // flag true and calculate the current cursor bounds using the stale
        // |display_offset_|. Then calculate the change in offset needed to move the
        // cursor into the visible area.
        cached_bounds_and_offset_valid_ = true;
        cursor_bounds_ = GetCursorBounds(selection_model_, insert_mode_);

        // TODO(bidi): Show RTL glyphs at the cursor position for ALIGN_LEFT, etc.
        if (cursor_bounds_.right() > display_rect_.right())
            delta_x = display_rect_.right() - cursor_bounds_.right();
        else if (cursor_bounds_.x() < display_rect_.x())
            delta_x = display_rect_.x() - cursor_bounds_.x();
    }

    SetDisplayOffset(display_offset_.x() + delta_x);
}

void RenderText::DrawSelection(Canvas* canvas)
{
    for (const Rect& s : GetSubstringBounds(selection()))
        canvas->FillRect(s, selection_background_focused_color_);
}

} // namespace gfx
