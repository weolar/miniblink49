// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/render_text.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include <algorithm>

#include "base/format_macros.h"
#include "base/i18n/break_iterator.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/break_list.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/color_utils.h"
#include "ui/gfx/font.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/range/range.h"
#include "ui/gfx/range/range_f.h"
#include "ui/gfx/render_text_harfbuzz.h"
#include "ui/gfx/text_utils.h"

#if defined(OS_WIN)
#include "base/win/windows_version.h"
#include "ui/gfx/platform_font_win.h"
#endif

#if defined(OS_MACOSX)
#include <ApplicationServices/ApplicationServices.h>

#include "ui/gfx/render_text_mac.h"
#endif

using base::ASCIIToUTF16;
using base::UTF8ToUTF16;
using base::WideToUTF16;
using base::WideToUTF8;

namespace gfx {
namespace test {

    class RenderTextTestApi {
    public:
        RenderTextTestApi(RenderText* render_text)
            : render_text_(render_text)
        {
        }

        static SkPaint& GetRendererPaint(internal::SkiaTextRenderer* renderer)
        {
            return renderer->paint_;
        }

        void DrawVisualText(internal::SkiaTextRenderer* renderer)
        {
            render_text_->EnsureLayout();
            render_text_->DrawVisualText(renderer);
        }

    private:
        RenderText* render_text_;

        DISALLOW_COPY_AND_ASSIGN(RenderTextTestApi);
    };

} // namespace test

namespace {

    // Various weak, LTR, RTL, and Bidi string cases with three characters each.
    const wchar_t kWeak[] = L" . ";
    const wchar_t kLtr[] = L"abc";
    const wchar_t kRtl[] = L"\x5d0\x5d1\x5d2";
    const wchar_t kLtrRtl[] = L"a"
                              L"\x5d0\x5d1";
    const wchar_t kLtrRtlLtr[] = L"a"
                                 L"\x5d1"
                                 L"b";
    const wchar_t kRtlLtr[] = L"\x5d0\x5d1"
                              L"a";
    const wchar_t kRtlLtrRtl[] = L"\x5d0"
                                 L"a"
                                 L"\x5d1";

    // Checks whether |range| contains |index|. This is not the same as calling
    // range.Contains(Range(index)), which returns true if |index| == |range.end()|.
    bool IndexInRange(const Range& range, size_t index)
    {
        return index >= range.start() && index < range.end();
    }

    base::string16 GetSelectedText(RenderText* render_text)
    {
        return render_text->text().substr(render_text->selection().GetMin(),
            render_text->selection().length());
    }

    // A test utility function to set the application default text direction.
    void SetRTL(bool rtl)
    {
        // Override the current locale/direction.
        base::i18n::SetICUDefaultLocale(rtl ? "he" : "en");
        EXPECT_EQ(rtl, base::i18n::IsRTL());
    }

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
    // Ensure cursor movement in the specified |direction| yields |expected| values.
    void RunMoveCursorLeftRightTest(RenderText* render_text,
        const std::vector<SelectionModel>& expected,
        VisualCursorDirection direction)
    {
        for (size_t i = 0; i < expected.size(); ++i) {
            SCOPED_TRACE(base::StringPrintf("Going %s; expected value index %d.",
                direction == CURSOR_LEFT ? "left" : "right", static_cast<int>(i)));
            EXPECT_EQ(expected[i], render_text->selection_model());
            render_text->MoveCursor(CHARACTER_BREAK, direction, false);
        }
        // Check that cursoring is clamped at the line edge.
        EXPECT_EQ(expected.back(), render_text->selection_model());
        // Check that it is the line edge.
        render_text->MoveCursor(LINE_BREAK, direction, false);
        EXPECT_EQ(expected.back(), render_text->selection_model());
    }
#endif // !defined(OS_MACOSX)

    // The class which records the drawing operations so that the test case can
    // verify where exactly the glyphs are drawn.
    class TestSkiaTextRenderer : public internal::SkiaTextRenderer {
    public:
        struct TextLog {
            TextLog()
                : glyph_count(0u)
                , color(SK_ColorTRANSPARENT)
            {
            }
            PointF origin;
            size_t glyph_count;
            SkColor color;
        };

        struct DecorationLog {
            DecorationLog(int x, int y, int width, bool underline, bool strike,
                bool diagonal_strike)
                : x(x)
                , y(y)
                , width(width)
                , underline(underline)
                , strike(strike)
                , diagonal_strike(diagonal_strike)
            {
            }
            int x;
            int y;
            int width;
            bool underline;
            bool strike;
            bool diagonal_strike;
        };

        explicit TestSkiaTextRenderer(Canvas* canvas)
            : internal::SkiaTextRenderer(canvas)
        {
        }
        ~TestSkiaTextRenderer() override { }

        void GetTextLogAndReset(std::vector<TextLog>* text_log)
        {
            text_log_.swap(*text_log);
            text_log_.clear();
        }

        void GetDecorationLogAndReset(std::vector<DecorationLog>* decoration_log)
        {
            decoration_log_.swap(*decoration_log);
            decoration_log_.clear();
        }

    private:
        // internal::SkiaTextRenderer:
        void DrawPosText(const SkPoint* pos,
            const uint16_t* glyphs,
            size_t glyph_count) override
        {
            TextLog log_entry;
            log_entry.glyph_count = glyph_count;
            if (glyph_count > 0) {
                log_entry.origin = PointF(SkScalarToFloat(pos[0].x()), SkScalarToFloat(pos[0].y()));
                for (size_t i = 1U; i < glyph_count; ++i) {
                    log_entry.origin.SetToMin(
                        PointF(SkScalarToFloat(pos[i].x()), SkScalarToFloat(pos[i].y())));
                }
            }
            log_entry.color = test::RenderTextTestApi::GetRendererPaint(this).getColor();
            text_log_.push_back(log_entry);
            internal::SkiaTextRenderer::DrawPosText(pos, glyphs, glyph_count);
        }

        void DrawDecorations(int x, int y, int width, bool underline, bool strike,
            bool diagonal_strike) override
        {
            decoration_log_.push_back(
                DecorationLog(x, y, width, underline, strike, diagonal_strike));
            internal::SkiaTextRenderer::DrawDecorations(
                x, y, width, underline, strike, diagonal_strike);
        }

        std::vector<TextLog> text_log_;
        std::vector<DecorationLog> decoration_log_;

        DISALLOW_COPY_AND_ASSIGN(TestSkiaTextRenderer);
    };

    // Given a buffer to test against, this can be used to test various areas of the
    // rectangular buffer against a specific color value.
    class TestRectangleBuffer {
    public:
        TestRectangleBuffer(const wchar_t* string,
            const SkColor* buffer,
            uint32_t stride,
            uint32_t row_count)
            : string_(string)
            , buffer_(buffer)
            , stride_(stride)
            , row_count_(row_count)
        {
        }

        // Test if any values in the rectangular area are anything other than |color|.
        void EnsureSolidRect(SkColor color,
            int left,
            int top,
            int width,
            int height) const
        {
            ASSERT_LT(top, row_count_) << string_;
            ASSERT_LE(top + height, row_count_) << string_;
            ASSERT_LT(left, stride_) << string_;
            ASSERT_LE(left + width, stride_) << string_ << ", left " << left
                                             << ", width " << width << ", stride_ "
                                             << stride_;
            for (int y = top; y < top + height; ++y) {
                for (int x = left; x < left + width; ++x) {
                    SkColor buffer_color = buffer_[x + y * stride_];
                    EXPECT_EQ(color, buffer_color) << string_ << " at " << x << ", " << y;
                }
            }
        }

    private:
        const wchar_t* string_;
        const SkColor* buffer_;
        int stride_;
        int row_count_;

        DISALLOW_COPY_AND_ASSIGN(TestRectangleBuffer);
    };

} // namespace

using RenderTextTest = testing::Test;

TEST_F(RenderTextTest, DefaultStyles)
{
    // Check the default styles applied to new instances and adjusted text.
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    EXPECT_TRUE(render_text->text().empty());
    const wchar_t* const cases[] = { kWeak, kLtr, L"Hello", kRtl, L"", L"" };
    for (size_t i = 0; i < arraysize(cases); ++i) {
        EXPECT_TRUE(render_text->colors().EqualsValueForTesting(SK_ColorBLACK));
        EXPECT_TRUE(
            render_text->baselines().EqualsValueForTesting(NORMAL_BASELINE));
        for (size_t style = 0; style < NUM_TEXT_STYLES; ++style)
            EXPECT_TRUE(render_text->styles()[style].EqualsValueForTesting(false));
        render_text->SetText(WideToUTF16(cases[i]));
    }
}

TEST_F(RenderTextTest, SetStyles)
{
    // Ensure custom default styles persist across setting and clearing text.
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    const SkColor color = SK_ColorRED;
    render_text->SetColor(color);
    render_text->SetBaselineStyle(SUPERSCRIPT);
    render_text->SetStyle(BOLD, true);
    render_text->SetStyle(UNDERLINE, false);
    const wchar_t* const cases[] = { kWeak, kLtr, L"Hello", kRtl, L"", L"" };
    for (size_t i = 0; i < arraysize(cases); ++i) {
        EXPECT_TRUE(render_text->colors().EqualsValueForTesting(color));
        EXPECT_TRUE(render_text->baselines().EqualsValueForTesting(SUPERSCRIPT));
        EXPECT_TRUE(render_text->styles()[BOLD].EqualsValueForTesting(true));
        EXPECT_TRUE(render_text->styles()[UNDERLINE].EqualsValueForTesting(false));
        render_text->SetText(WideToUTF16(cases[i]));

        // Ensure custom default styles can be applied after text has been set.
        if (i == 1)
            render_text->SetStyle(STRIKE, true);
        if (i >= 1)
            EXPECT_TRUE(render_text->styles()[STRIKE].EqualsValueForTesting(true));
    }
}

TEST_F(RenderTextTest, ApplyStyles)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("012345678"));

    // Apply a ranged color and style and check the resulting breaks.
    render_text->ApplyColor(SK_ColorRED, Range(1, 4));
    render_text->ApplyBaselineStyle(SUPERIOR, Range(2, 4));
    render_text->ApplyStyle(BOLD, true, Range(2, 5));
    std::vector<std::pair<size_t, SkColor>> expected_color;
    expected_color.push_back(std::pair<size_t, SkColor>(0, SK_ColorBLACK));
    expected_color.push_back(std::pair<size_t, SkColor>(1, SK_ColorRED));
    expected_color.push_back(std::pair<size_t, SkColor>(4, SK_ColorBLACK));
    EXPECT_TRUE(render_text->colors().EqualsForTesting(expected_color));
    std::vector<std::pair<size_t, BaselineStyle>> expected_baseline_style;
    expected_baseline_style.push_back(
        std::pair<size_t, BaselineStyle>(0, NORMAL_BASELINE));
    expected_baseline_style.push_back(
        std::pair<size_t, BaselineStyle>(2, SUPERIOR));
    expected_baseline_style.push_back(
        std::pair<size_t, BaselineStyle>(4, NORMAL_BASELINE));
    EXPECT_TRUE(
        render_text->baselines().EqualsForTesting(expected_baseline_style));
    std::vector<std::pair<size_t, bool>> expected_style;
    expected_style.push_back(std::pair<size_t, bool>(0, false));
    expected_style.push_back(std::pair<size_t, bool>(2, true));
    expected_style.push_back(std::pair<size_t, bool>(5, false));
    EXPECT_TRUE(render_text->styles()[BOLD].EqualsForTesting(expected_style));

    // Ensure that setting a value overrides the ranged values.
    render_text->SetColor(SK_ColorBLUE);
    EXPECT_TRUE(render_text->colors().EqualsValueForTesting(SK_ColorBLUE));
    render_text->SetBaselineStyle(SUBSCRIPT);
    EXPECT_TRUE(render_text->baselines().EqualsValueForTesting(SUBSCRIPT));
    render_text->SetStyle(BOLD, false);
    EXPECT_TRUE(render_text->styles()[BOLD].EqualsValueForTesting(false));

    // Apply a value over the text end and check the resulting breaks (INT_MAX
    // should be used instead of the text length for the range end)
    const size_t text_length = render_text->text().length();
    render_text->ApplyColor(SK_ColorRED, Range(0, text_length));
    render_text->ApplyBaselineStyle(SUPERIOR, Range(0, text_length));
    render_text->ApplyStyle(BOLD, true, Range(2, text_length));
    std::vector<std::pair<size_t, SkColor>> expected_color_end;
    expected_color_end.push_back(std::pair<size_t, SkColor>(0, SK_ColorRED));
    EXPECT_TRUE(render_text->colors().EqualsForTesting(expected_color_end));
    std::vector<std::pair<size_t, BaselineStyle>> expected_baseline_end;
    expected_baseline_end.push_back(
        std::pair<size_t, BaselineStyle>(0, SUPERIOR));
    EXPECT_TRUE(render_text->baselines().EqualsForTesting(expected_baseline_end));
    std::vector<std::pair<size_t, bool>> expected_style_end;
    expected_style_end.push_back(std::pair<size_t, bool>(0, false));
    expected_style_end.push_back(std::pair<size_t, bool>(2, true));
    EXPECT_TRUE(render_text->styles()[BOLD].EqualsForTesting(expected_style_end));

    // Ensure ranged values adjust to accommodate text length changes.
    render_text->ApplyStyle(ITALIC, true, Range(0, 2));
    render_text->ApplyStyle(ITALIC, true, Range(3, 6));
    render_text->ApplyStyle(ITALIC, true, Range(7, text_length));
    std::vector<std::pair<size_t, bool>> expected_italic;
    expected_italic.push_back(std::pair<size_t, bool>(0, true));
    expected_italic.push_back(std::pair<size_t, bool>(2, false));
    expected_italic.push_back(std::pair<size_t, bool>(3, true));
    expected_italic.push_back(std::pair<size_t, bool>(6, false));
    expected_italic.push_back(std::pair<size_t, bool>(7, true));
    EXPECT_TRUE(render_text->styles()[ITALIC].EqualsForTesting(expected_italic));

    // Changing the text should clear any breaks except for the first one.
    render_text->SetText(ASCIIToUTF16("0123456"));
    expected_italic.resize(1);
    EXPECT_TRUE(render_text->styles()[ITALIC].EqualsForTesting(expected_italic));
    render_text->ApplyStyle(ITALIC, false, Range(2, 4));
    render_text->SetText(ASCIIToUTF16("012345678"));
    EXPECT_TRUE(render_text->styles()[ITALIC].EqualsForTesting(expected_italic));
    render_text->ApplyStyle(ITALIC, false, Range(0, 1));
    render_text->SetText(ASCIIToUTF16("0123456"));
    expected_italic.begin()->second = false;
    EXPECT_TRUE(render_text->styles()[ITALIC].EqualsForTesting(expected_italic));
    render_text->ApplyStyle(ITALIC, true, Range(2, 4));
    render_text->SetText(ASCIIToUTF16("012345678"));
    EXPECT_TRUE(render_text->styles()[ITALIC].EqualsForTesting(expected_italic));

    // TODO(tmoniuszko): Enable when RenderTextMac::IsValidCursorIndex is
    //                   implemented.
#if !defined(OS_MACOSX)
    // Styles shouldn't be changed mid-grapheme.
    render_text->SetText(WideToUTF16(
        L"0"
        L"\x0915\x093f"
        L"1"
        L"\x0915\x093f"
        L"2"));
    render_text->ApplyStyle(UNDERLINE, true, Range(2, 5));
    std::vector<std::pair<size_t, bool>> expected_underline;
    expected_underline.push_back(std::pair<size_t, bool>(0, false));
    expected_underline.push_back(std::pair<size_t, bool>(1, true));
    expected_underline.push_back(std::pair<size_t, bool>(6, false));
    EXPECT_TRUE(render_text->styles()[UNDERLINE].EqualsForTesting(
        expected_underline));
#endif // !defined(OS_MACOSX)
}

TEST_F(RenderTextTest, AppendTextKeepsStyles)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    // Setup basic functionality.
    render_text->SetText(ASCIIToUTF16("abc"));
    render_text->ApplyColor(SK_ColorRED, Range(0, 1));
    render_text->ApplyBaselineStyle(SUPERSCRIPT, Range(1, 2));
    render_text->ApplyStyle(UNDERLINE, true, Range(2, 3));
    // Verify basic functionality.
    std::vector<std::pair<size_t, SkColor>> expected_color;
    expected_color.push_back(std::pair<size_t, SkColor>(0, SK_ColorRED));
    expected_color.push_back(std::pair<size_t, SkColor>(1, SK_ColorBLACK));
    EXPECT_TRUE(render_text->colors().EqualsForTesting(expected_color));
    std::vector<std::pair<size_t, BaselineStyle>> expected_baseline;
    expected_baseline.push_back(
        std::pair<size_t, BaselineStyle>(0, NORMAL_BASELINE));
    expected_baseline.push_back(std::pair<size_t, BaselineStyle>(1, SUPERSCRIPT));
    expected_baseline.push_back(
        std::pair<size_t, BaselineStyle>(2, NORMAL_BASELINE));
    EXPECT_TRUE(render_text->baselines().EqualsForTesting(expected_baseline));
    std::vector<std::pair<size_t, bool>> expected_style;
    expected_style.push_back(std::pair<size_t, bool>(0, false));
    expected_style.push_back(std::pair<size_t, bool>(2, true));
    EXPECT_TRUE(
        render_text->styles()[UNDERLINE].EqualsForTesting(expected_style));
    // Ensure AppendText maintains current text styles.
    render_text->AppendText(ASCIIToUTF16("def"));
    EXPECT_EQ(render_text->GetDisplayText(), ASCIIToUTF16("abcdef"));
    EXPECT_TRUE(render_text->colors().EqualsForTesting(expected_color));
    EXPECT_TRUE(render_text->baselines().EqualsForTesting(expected_baseline));
    EXPECT_TRUE(
        render_text->styles()[UNDERLINE].EqualsForTesting(expected_style));
}

void TestVisualCursorMotionInObscuredField(RenderText* render_text,
    const base::string16& text,
    bool select)
{
    ASSERT_TRUE(render_text->obscured());
    render_text->SetText(text);
    int len = text.length();
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, select);
    EXPECT_EQ(SelectionModel(Range(select ? 0 : len, len), CURSOR_FORWARD),
        render_text->selection_model());
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, select);
    EXPECT_EQ(SelectionModel(0, CURSOR_BACKWARD), render_text->selection_model());
    for (int j = 1; j <= len; ++j) {
        render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, select);
        EXPECT_EQ(SelectionModel(Range(select ? 0 : j, j), CURSOR_BACKWARD),
            render_text->selection_model());
    }
    for (int j = len - 1; j >= 0; --j) {
        render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, select);
        EXPECT_EQ(SelectionModel(Range(select ? 0 : j, j), CURSOR_FORWARD),
            render_text->selection_model());
    }
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, select);
    EXPECT_EQ(SelectionModel(Range(select ? 0 : len, len), CURSOR_FORWARD),
        render_text->selection_model());
    render_text->MoveCursor(WORD_BREAK, CURSOR_LEFT, select);
    EXPECT_EQ(SelectionModel(0, CURSOR_BACKWARD), render_text->selection_model());
}

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
TEST_F(RenderTextTest, ObscuredText)
{
    const base::string16 seuss = ASCIIToUTF16("hop on pop");
    const base::string16 no_seuss = ASCIIToUTF16("**********");
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    // GetLayoutText() returns asterisks when the obscured bit is set.
    render_text->SetText(seuss);
    render_text->SetObscured(true);
    EXPECT_EQ(seuss, render_text->text());
    EXPECT_EQ(no_seuss, render_text->GetDisplayText());
    render_text->SetObscured(false);
    EXPECT_EQ(seuss, render_text->text());
    EXPECT_EQ(seuss, render_text->GetDisplayText());

    render_text->SetObscured(true);

    // Surrogate pairs are counted as one code point.
    const base::char16 invalid_surrogates[] = { 0xDC00, 0xD800, 0 };
    render_text->SetText(invalid_surrogates);
    EXPECT_EQ(ASCIIToUTF16("**"), render_text->GetDisplayText());
    const base::char16 valid_surrogates[] = { 0xD800, 0xDC00, 0 };
    render_text->SetText(valid_surrogates);
    EXPECT_EQ(ASCIIToUTF16("*"), render_text->GetDisplayText());
    EXPECT_EQ(0U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(2U, render_text->cursor_position());

    // Test index conversion and cursor validity with a valid surrogate pair.
    EXPECT_EQ(0U, render_text->TextIndexToDisplayIndex(0U));
    EXPECT_EQ(1U, render_text->TextIndexToDisplayIndex(1U));
    EXPECT_EQ(1U, render_text->TextIndexToDisplayIndex(2U));
    EXPECT_EQ(0U, render_text->DisplayIndexToTextIndex(0U));
    EXPECT_EQ(2U, render_text->DisplayIndexToTextIndex(1U));
    EXPECT_TRUE(render_text->IsValidCursorIndex(0U));
    EXPECT_FALSE(render_text->IsValidCursorIndex(1U));
    EXPECT_TRUE(render_text->IsValidCursorIndex(2U));

    // FindCursorPosition() should not return positions between a surrogate pair.
    render_text->SetDisplayRect(Rect(0, 0, 20, 20));
    EXPECT_EQ(render_text->FindCursorPosition(Point(0, 0)).caret_pos(), 0U);
    EXPECT_EQ(render_text->FindCursorPosition(Point(20, 0)).caret_pos(), 2U);
    for (int x = -1; x <= 20; ++x) {
        SelectionModel selection = render_text->FindCursorPosition(Point(x, 0));
        EXPECT_TRUE(selection.caret_pos() == 0U || selection.caret_pos() == 2U);
    }

    // GetGlyphBounds() should yield the entire string bounds for text index 0.
    EXPECT_EQ(render_text->GetStringSize().width(),
        static_cast<int>(render_text->GetGlyphBounds(0U).length()));

    // Cursoring is independent of underlying characters when text is obscured.
    const wchar_t* const texts[] = {
        kWeak, kLtr, kLtrRtl, kLtrRtlLtr, kRtl, kRtlLtr, kRtlLtrRtl,
        L"hop on pop", // Check LTR word boundaries.
        L"\x05d0\x05d1 \x05d0\x05d2 \x05d1\x05d2", // Check RTL word boundaries.
    };
    for (size_t i = 0; i < arraysize(texts); ++i) {
        base::string16 text = WideToUTF16(texts[i]);
        TestVisualCursorMotionInObscuredField(render_text.get(), text, false);
        TestVisualCursorMotionInObscuredField(render_text.get(), text, true);
    }
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, RevealObscuredText)
{
    const base::string16 seuss = ASCIIToUTF16("hop on pop");
    const base::string16 no_seuss = ASCIIToUTF16("**********");
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    render_text->SetText(seuss);
    render_text->SetObscured(true);
    EXPECT_EQ(seuss, render_text->text());
    EXPECT_EQ(no_seuss, render_text->GetDisplayText());

    // Valid reveal index and new revealed index clears previous one.
    render_text->RenderText::SetObscuredRevealIndex(0);
    EXPECT_EQ(ASCIIToUTF16("h*********"), render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(1);
    EXPECT_EQ(ASCIIToUTF16("*o********"), render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(2);
    EXPECT_EQ(ASCIIToUTF16("**p*******"), render_text->GetDisplayText());

    // Invalid reveal index.
    render_text->RenderText::SetObscuredRevealIndex(-1);
    EXPECT_EQ(no_seuss, render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(seuss.length() + 1);
    EXPECT_EQ(no_seuss, render_text->GetDisplayText());

    // SetObscured clears the revealed index.
    render_text->RenderText::SetObscuredRevealIndex(0);
    EXPECT_EQ(ASCIIToUTF16("h*********"), render_text->GetDisplayText());
    render_text->SetObscured(false);
    EXPECT_EQ(seuss, render_text->GetDisplayText());
    render_text->SetObscured(true);
    EXPECT_EQ(no_seuss, render_text->GetDisplayText());

    // SetText clears the revealed index.
    render_text->SetText(ASCIIToUTF16("new"));
    EXPECT_EQ(ASCIIToUTF16("***"), render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(2);
    EXPECT_EQ(ASCIIToUTF16("**w"), render_text->GetDisplayText());
    render_text->SetText(ASCIIToUTF16("new longer"));
    EXPECT_EQ(ASCIIToUTF16("**********"), render_text->GetDisplayText());

    // Text with invalid surrogates.
    const base::char16 invalid_surrogates[] = { 0xDC00, 0xD800, 'h', 'o', 'p', 0 };
    render_text->SetText(invalid_surrogates);
    EXPECT_EQ(ASCIIToUTF16("*****"), render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(0);
    const base::char16 invalid_expect_0[] = { 0xDC00, '*', '*', '*', '*', 0 };
    EXPECT_EQ(invalid_expect_0, render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(1);
    const base::char16 invalid_expect_1[] = { '*', 0xD800, '*', '*', '*', 0 };
    EXPECT_EQ(invalid_expect_1, render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(2);
    EXPECT_EQ(ASCIIToUTF16("**h**"), render_text->GetDisplayText());

    // Text with valid surrogates before and after the reveal index.
    const base::char16 valid_surrogates[] = { 0xD800, 0xDC00, 'h', 'o', 'p', 0xD800, 0xDC00, 0 };
    render_text->SetText(valid_surrogates);
    EXPECT_EQ(ASCIIToUTF16("*****"), render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(0);
    const base::char16 valid_expect_0_and_1[] = { 0xD800, 0xDC00, '*', '*', '*', '*', 0 };
    EXPECT_EQ(valid_expect_0_and_1, render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(1);
    EXPECT_EQ(valid_expect_0_and_1, render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(2);
    EXPECT_EQ(ASCIIToUTF16("*h***"), render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(5);
    const base::char16 valid_expect_5_and_6[] = { '*', '*', '*', '*', 0xD800, 0xDC00, 0 };
    EXPECT_EQ(valid_expect_5_and_6, render_text->GetDisplayText());
    render_text->RenderText::SetObscuredRevealIndex(6);
    EXPECT_EQ(valid_expect_5_and_6, render_text->GetDisplayText());
}

// TODO(PORT): Fails for RenderTextMac.
#if !defined(OS_MACOSX)
TEST_F(RenderTextTest, ElidedText)
{
    // TODO(skanuj) : Add more test cases for following
    // - RenderText styles.
    // - Cross interaction of truncate, elide and obscure.
    // - ElideText tests from text_elider.cc.
    struct {
        const wchar_t* text;
        const wchar_t* display_text;
        const bool elision_expected;
    } cases[] = {
        // Strings shorter than the elision width should be laid out in full.
        { L"", L"", false },
        { L"M", L"", false },
        { L" . ", L" . ", false },
        { kWeak, kWeak, false },
        { kLtr, kLtr, false },
        { kLtrRtl, kLtrRtl, false },
        { kLtrRtlLtr, kLtrRtlLtr, false },
        { kRtl, kRtl, false },
        { kRtlLtr, kRtlLtr, false },
        { kRtlLtrRtl, kRtlLtrRtl, false },
        // Strings as long as the elision width should be laid out in full.
        { L"012ab", L"012ab", false },
        // Long strings should be elided with an ellipsis appended at the end.
        { L"012abc", L"012a\x2026", true },
        { L"012ab"
          L"\x5d0\x5d1",
            L"012a\x2026", true },
        { L"012a"
          L"\x5d1"
          L"b",
            L"012a\x2026", true },
        // No RLM marker added as digits (012) have weak directionality.
        { L"01"
          L"\x5d0\x5d1\x5d2",
            L"01\x5d0\x5d1\x2026", true },
        // RLM marker added as "ab" have strong LTR directionality.
        { L"ab"
          L"\x5d0\x5d1\x5d2",
            L"ab\x5d0\x5d1\x2026\x200f", true },
        // Test surrogate pairs. \xd834\xdd1e forms a single code point U+1D11E;
        // \xd834\xdd22 forms a second code point U+1D122. The first should be kept;
        // the second removed (no surrogate pair should be partially elided).
        { L"0123\xd834\xdd1e\xd834\xdd22", L"0123\xd834\xdd1e\x2026", true },
        // Test combining character sequences. U+0915 U+093F forms a compound glyph;
        // U+0915 U+0942 forms a second compound glyph. The first should be kept;
        // the second removed (no combining sequence should be partially elided).
        { L"0123\x0915\x093f\x0915\x0942", L"0123\x0915\x093f\x2026", true },
        // U+05E9 U+05BC U+05C1 U+05B8 forms a four-character compound glyph. Again,
        // it should be either fully elided, or not elided at all. If completely
        // elided, an LTR Mark (U+200E) should be added.
        { L"0\x05e9\x05bc\x05c1\x05b8", L"0\x05e9\x05bc\x05c1\x05b8", false },
        { L"0\x05e9\x05bc\x05c1\x05b8", L"0\x2026\x200E", true },
        { L"01\x05e9\x05bc\x05c1\x05b8", L"01\x2026\x200E", true },
        { L"012\x05e9\x05bc\x05c1\x05b8", L"012\x2026\x200E", true },
        { L"012\xF0\x9D\x84\x9E", L"012\xF0\x2026", true },
    };

    scoped_ptr<RenderText> expected_render_text(RenderText::CreateInstance());
    expected_render_text->SetFontList(FontList("serif, Sans serif, 12px"));
    expected_render_text->SetDisplayRect(Rect(0, 0, 9999, 100));

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetFontList(FontList("serif, Sans serif, 12px"));
    render_text->SetElideBehavior(ELIDE_TAIL);

    for (size_t i = 0; i < arraysize(cases); i++) {
        SCOPED_TRACE(base::StringPrintf("Testing cases[%" PRIuS "] '%ls'", i,
            cases[i].text));

        // Compute expected width
        expected_render_text->SetText(WideToUTF16(cases[i].display_text));
        int expected_width = expected_render_text->GetContentWidth();

        base::string16 input = WideToUTF16(cases[i].text);
        // Extend the input text to ensure that it is wider than the display_text,
        // and so it will get elided.
        if (cases[i].elision_expected)
            input.append(WideToUTF16(L" MMMMMMMMMMM"));
        render_text->SetText(input);
        render_text->SetDisplayRect(Rect(0, 0, expected_width, 100));
        EXPECT_EQ(input, render_text->text());
        EXPECT_EQ(WideToUTF16(cases[i].display_text),
            render_text->GetDisplayText());
        expected_render_text->SetText(base::string16());
    }
}

TEST_F(RenderTextTest, ElidedObscuredText)
{
    scoped_ptr<RenderText> expected_render_text(RenderText::CreateInstance());
    expected_render_text->SetFontList(FontList("serif, Sans serif, 12px"));
    expected_render_text->SetDisplayRect(Rect(0, 0, 9999, 100));
    expected_render_text->SetText(WideToUTF16(L"**\x2026"));

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetFontList(FontList("serif, Sans serif, 12px"));
    render_text->SetElideBehavior(ELIDE_TAIL);
    render_text->SetDisplayRect(
        Rect(0, 0, expected_render_text->GetContentWidth(), 100));
    render_text->SetObscured(true);
    render_text->SetText(WideToUTF16(L"abcdef"));
    EXPECT_EQ(WideToUTF16(L"abcdef"), render_text->text());
    EXPECT_EQ(WideToUTF16(L"**\x2026"), render_text->GetDisplayText());
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, ElidedEmail)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("test@example.com"));
    const gfx::Size size = render_text->GetStringSize();

    const base::string16 long_email = ASCIIToUTF16("longemailaddresstest@example.com");
    render_text->SetText(long_email);
    render_text->SetElideBehavior(ELIDE_EMAIL);
    render_text->SetDisplayRect(gfx::Rect(size));
    EXPECT_GE(size.width(), render_text->GetStringSize().width());
    EXPECT_GT(long_email.size(), render_text->GetDisplayText().size());
}

TEST_F(RenderTextTest, TruncatedText)
{
    struct {
        const wchar_t* text;
        const wchar_t* display_text;
    } cases[] = {
        // Strings shorter than the truncation length should be laid out in full.
        { L"", L"" },
        { kWeak, kWeak },
        { kLtr, kLtr },
        { kLtrRtl, kLtrRtl },
        { kLtrRtlLtr, kLtrRtlLtr },
        { kRtl, kRtl },
        { kRtlLtr, kRtlLtr },
        { kRtlLtrRtl, kRtlLtrRtl },
        // Strings as long as the truncation length should be laid out in full.
        { L"01234", L"01234" },
        // Long strings should be truncated with an ellipsis appended at the end.
        { L"012345", L"0123\x2026" },
        { L"012"
          L" . ",
            L"012 \x2026" },
        { L"012"
          L"abc",
            L"012a\x2026" },
        { L"012"
          L"a"
          L"\x5d0\x5d1",
            L"012a\x2026" },
        { L"012"
          L"a"
          L"\x5d1"
          L"b",
            L"012a\x2026" },
        { L"012"
          L"\x5d0\x5d1\x5d2",
            L"012\x5d0\x2026" },
        { L"012"
          L"\x5d0\x5d1"
          L"a",
            L"012\x5d0\x2026" },
        { L"012"
          L"\x5d0"
          L"a"
          L"\x5d1",
            L"012\x5d0\x2026" },
        // Surrogate pairs should be truncated reasonably enough.
        { L"0123\x0915\x093f", L"0123\x2026" },
        { L"0\x05e9\x05bc\x05c1\x05b8", L"0\x05e9\x05bc\x05c1\x05b8" },
        { L"01\x05e9\x05bc\x05c1\x05b8", L"01\x05e9\x05bc\x2026" },
        { L"012\x05e9\x05bc\x05c1\x05b8", L"012\x05e9\x2026" },
        { L"0123\x05e9\x05bc\x05c1\x05b8", L"0123\x2026" },
        { L"01234\x05e9\x05bc\x05c1\x05b8", L"0123\x2026" },
        { L"012\xF0\x9D\x84\x9E", L"012\xF0\x2026" },
    };

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->set_truncate_length(5);
    for (size_t i = 0; i < arraysize(cases); i++) {
        render_text->SetText(WideToUTF16(cases[i].text));
        EXPECT_EQ(WideToUTF16(cases[i].text), render_text->text());
        EXPECT_EQ(WideToUTF16(cases[i].display_text), render_text->GetDisplayText())
            << "For case " << i << ": " << cases[i].text;
    }
}

TEST_F(RenderTextTest, TruncatedObscuredText)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->set_truncate_length(3);
    render_text->SetObscured(true);
    render_text->SetText(WideToUTF16(L"abcdef"));
    EXPECT_EQ(WideToUTF16(L"abcdef"), render_text->text());
    EXPECT_EQ(WideToUTF16(L"**\x2026"), render_text->GetDisplayText());
}

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
TEST_F(RenderTextTest, TruncatedCursorMovementLTR)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->set_truncate_length(2);
    render_text->SetText(WideToUTF16(L"abcd"));

    EXPECT_EQ(SelectionModel(0, CURSOR_BACKWARD), render_text->selection_model());
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(SelectionModel(4, CURSOR_FORWARD), render_text->selection_model());
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(SelectionModel(0, CURSOR_BACKWARD), render_text->selection_model());

    std::vector<SelectionModel> expected;
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    // The cursor hops over the ellipsis and elided text to the line end.
    expected.push_back(SelectionModel(4, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(4, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);

    expected.clear();
    expected.push_back(SelectionModel(4, CURSOR_FORWARD));
    // The cursor hops over the elided text to preceeding text.
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);
}

TEST_F(RenderTextTest, TruncatedCursorMovementRTL)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->set_truncate_length(2);
    render_text->SetText(WideToUTF16(L"\x5d0\x5d1\x5d2\x5d3"));

    EXPECT_EQ(SelectionModel(0, CURSOR_BACKWARD), render_text->selection_model());
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(SelectionModel(4, CURSOR_FORWARD), render_text->selection_model());
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(SelectionModel(0, CURSOR_BACKWARD), render_text->selection_model());

    std::vector<SelectionModel> expected;
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    // The cursor hops over the ellipsis and elided text to the line end.
    expected.push_back(SelectionModel(4, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(4, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);

    expected.clear();
    expected.push_back(SelectionModel(4, CURSOR_FORWARD));
    // The cursor hops over the elided text to preceeding text.
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, GetDisplayTextDirection)
{
    struct {
        const wchar_t* text;
        const base::i18n::TextDirection text_direction;
    } cases[] = {
        // Blank strings and those with no/weak directionality default to LTR.
        { L"", base::i18n::LEFT_TO_RIGHT },
        { kWeak, base::i18n::LEFT_TO_RIGHT },
        // Strings that begin with strong LTR characters.
        { kLtr, base::i18n::LEFT_TO_RIGHT },
        { kLtrRtl, base::i18n::LEFT_TO_RIGHT },
        { kLtrRtlLtr, base::i18n::LEFT_TO_RIGHT },
        // Strings that begin with strong RTL characters.
        { kRtl, base::i18n::RIGHT_TO_LEFT },
        { kRtlLtr, base::i18n::RIGHT_TO_LEFT },
        { kRtlLtrRtl, base::i18n::RIGHT_TO_LEFT },
    };

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    const bool was_rtl = base::i18n::IsRTL();

    for (size_t i = 0; i < 2; ++i) {
        // Toggle the application default text direction (to try each direction).
        SetRTL(!base::i18n::IsRTL());
        const base::i18n::TextDirection ui_direction = base::i18n::IsRTL() ? base::i18n::RIGHT_TO_LEFT : base::i18n::LEFT_TO_RIGHT;

        // Ensure that directionality modes yield the correct text directions.
        for (size_t j = 0; j < arraysize(cases); j++) {
            render_text->SetText(WideToUTF16(cases[j].text));
            render_text->SetDirectionalityMode(DIRECTIONALITY_FROM_TEXT);
            EXPECT_EQ(render_text->GetDisplayTextDirection(), cases[j].text_direction);
            render_text->SetDirectionalityMode(DIRECTIONALITY_FROM_UI);
            EXPECT_EQ(render_text->GetDisplayTextDirection(), ui_direction);
            render_text->SetDirectionalityMode(DIRECTIONALITY_FORCE_LTR);
            EXPECT_EQ(render_text->GetDisplayTextDirection(),
                base::i18n::LEFT_TO_RIGHT);
            render_text->SetDirectionalityMode(DIRECTIONALITY_FORCE_RTL);
            EXPECT_EQ(render_text->GetDisplayTextDirection(),
                base::i18n::RIGHT_TO_LEFT);
        }
    }

    EXPECT_EQ(was_rtl, base::i18n::IsRTL());

    // Ensure that text changes update the direction for DIRECTIONALITY_FROM_TEXT.
    render_text->SetDirectionalityMode(DIRECTIONALITY_FROM_TEXT);
    render_text->SetText(WideToUTF16(kLtr));
    EXPECT_EQ(render_text->GetDisplayTextDirection(), base::i18n::LEFT_TO_RIGHT);
    render_text->SetText(WideToUTF16(kRtl));
    EXPECT_EQ(render_text->GetDisplayTextDirection(), base::i18n::RIGHT_TO_LEFT);
}

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
TEST_F(RenderTextTest, MoveCursorLeftRightInLtr)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    // Pure LTR.
    render_text->SetText(ASCIIToUTF16("abc"));
    // |expected| saves the expected SelectionModel when moving cursor from left
    // to right.
    std::vector<SelectionModel> expected;
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(2, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);

    expected.clear();
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    expected.push_back(SelectionModel(2, CURSOR_FORWARD));
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);
}

TEST_F(RenderTextTest, MoveCursorLeftRightInLtrRtl)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    // LTR-RTL
    render_text->SetText(WideToUTF16(L"abc\x05d0\x05d1\x05d2"));
    // The last one is the expected END position.
    std::vector<SelectionModel> expected;
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(2, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(5, CURSOR_FORWARD));
    expected.push_back(SelectionModel(4, CURSOR_FORWARD));
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    expected.push_back(SelectionModel(6, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);

    expected.clear();
    expected.push_back(SelectionModel(6, CURSOR_FORWARD));
    expected.push_back(SelectionModel(4, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(5, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(6, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(2, CURSOR_FORWARD));
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);
}

TEST_F(RenderTextTest, MoveCursorLeftRightInLtrRtlLtr)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    // LTR-RTL-LTR.
    render_text->SetText(WideToUTF16(L"a"
                                     L"\x05d1"
                                     L"b"));
    std::vector<SelectionModel> expected;
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(3, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);

    expected.clear();
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    expected.push_back(SelectionModel(2, CURSOR_FORWARD));
    expected.push_back(SelectionModel(2, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);
}

TEST_F(RenderTextTest, MoveCursorLeftRightInRtl)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    // Pure RTL.
    render_text->SetText(WideToUTF16(L"\x05d0\x05d1\x05d2"));
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    std::vector<SelectionModel> expected;

    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(2, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);

    expected.clear();

    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    expected.push_back(SelectionModel(2, CURSOR_FORWARD));
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);
}

TEST_F(RenderTextTest, MoveCursorLeftRightInRtlLtr)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    // RTL-LTR
    render_text->SetText(WideToUTF16(L"\x05d0\x05d1\x05d2"
                                     L"abc"));
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    std::vector<SelectionModel> expected;
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(2, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(5, CURSOR_FORWARD));
    expected.push_back(SelectionModel(4, CURSOR_FORWARD));
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    expected.push_back(SelectionModel(6, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);

    expected.clear();
    expected.push_back(SelectionModel(6, CURSOR_FORWARD));
    expected.push_back(SelectionModel(4, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(5, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(6, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(2, CURSOR_FORWARD));
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);
}

TEST_F(RenderTextTest, MoveCursorLeftRightInRtlLtrRtl)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    // RTL-LTR-RTL.
    render_text->SetText(WideToUTF16(L"\x05d0"
                                     L"a"
                                     L"\x05d1"));
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    std::vector<SelectionModel> expected;
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(1, CURSOR_FORWARD));
    expected.push_back(SelectionModel(3, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_LEFT);

    expected.clear();
    expected.push_back(SelectionModel(3, CURSOR_FORWARD));
    expected.push_back(SelectionModel(2, CURSOR_FORWARD));
    expected.push_back(SelectionModel(2, CURSOR_BACKWARD));
    expected.push_back(SelectionModel(0, CURSOR_FORWARD));
    expected.push_back(SelectionModel(0, CURSOR_BACKWARD));
    RunMoveCursorLeftRightTest(render_text.get(), expected, CURSOR_RIGHT);
}

TEST_F(RenderTextTest, MoveCursorLeftRight_ComplexScript)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    render_text->SetText(WideToUTF16(L"\x0915\x093f\x0915\x094d\x0915"));
    EXPECT_EQ(0U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(2U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(4U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(5U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(5U, render_text->cursor_position());

    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(4U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(2U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(0U, render_text->cursor_position());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(0U, render_text->cursor_position());
}

TEST_F(RenderTextTest, MoveCursorLeftRight_MeiryoUILigatures)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    // Meiryo UI uses single-glyph ligatures for 'ff' and 'ffi', but each letter
    // (code point) has unique bounds, so mid-glyph cursoring should be possible.
    render_text->SetFontList(FontList("Meiryo UI, 12px"));
    render_text->SetText(WideToUTF16(L"ff ffi"));
    EXPECT_EQ(0U, render_text->cursor_position());
    for (size_t i = 0; i < render_text->text().length(); ++i) {
        render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
        EXPECT_EQ(i + 1, render_text->cursor_position());
    }
    EXPECT_EQ(6U, render_text->cursor_position());
}

TEST_F(RenderTextTest, GraphemePositions)
{
    // LTR 2-character grapheme, LTR abc, LTR 2-character grapheme.
    const base::string16 kText1 = WideToUTF16(L"\x0915\x093f"
                                              L"abc"
                                              L"\x0915\x093f");

    // LTR ab, LTR 2-character grapheme, LTR cd.
    const base::string16 kText2 = WideToUTF16(L"ab"
                                              L"\x0915\x093f"
                                              L"cd");

    // The below is 'MUSICAL SYMBOL G CLEF', which is represented in UTF-16 as
    // two characters forming the surrogate pair 0x0001D11E.
    const std::string kSurrogate = "\xF0\x9D\x84\x9E";

    // LTR ab, UTF16 surrogate pair, LTR cd.
    const base::string16 kText3 = UTF8ToUTF16("ab" + kSurrogate + "cd");

    struct {
        base::string16 text;
        size_t index;
        size_t expected_previous;
        size_t expected_next;
    } cases[] = {
        { base::string16(), 0, 0, 0 },
        { base::string16(), 1, 0, 0 },
        { base::string16(), 50, 0, 0 },
        { kText1, 0, 0, 2 },
        { kText1, 1, 0, 2 },
        { kText1, 2, 0, 3 },
        { kText1, 3, 2, 4 },
        { kText1, 4, 3, 5 },
        { kText1, 5, 4, 7 },
        { kText1, 6, 5, 7 },
        { kText1, 7, 5, 7 },
        { kText1, 8, 7, 7 },
        { kText1, 50, 7, 7 },
        { kText2, 0, 0, 1 },
        { kText2, 1, 0, 2 },
        { kText2, 2, 1, 4 },
        { kText2, 3, 2, 4 },
        { kText2, 4, 2, 5 },
        { kText2, 5, 4, 6 },
        { kText2, 6, 5, 6 },
        { kText2, 7, 6, 6 },
        { kText2, 50, 6, 6 },
        { kText3, 0, 0, 1 },
        { kText3, 1, 0, 2 },
        { kText3, 2, 1, 4 },
        { kText3, 3, 2, 4 },
        { kText3, 4, 2, 5 },
        { kText3, 5, 4, 6 },
        { kText3, 6, 5, 6 },
        { kText3, 7, 6, 6 },
        { kText3, 50, 6, 6 },
    };

#if defined(OS_WIN)
    // TODO(msw): XP fails due to lack of font support: http://crbug.com/106450
    if (base::win::GetVersion() < base::win::VERSION_VISTA)
        return;
#endif

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    for (size_t i = 0; i < arraysize(cases); i++) {
        SCOPED_TRACE(base::StringPrintf("Testing cases[%" PRIuS "]", i));
        render_text->SetText(cases[i].text);

        size_t next = render_text->IndexOfAdjacentGrapheme(cases[i].index,
            CURSOR_FORWARD);
        EXPECT_EQ(cases[i].expected_next, next);
        EXPECT_TRUE(render_text->IsValidCursorIndex(next));

        size_t previous = render_text->IndexOfAdjacentGrapheme(cases[i].index,
            CURSOR_BACKWARD);
        EXPECT_EQ(cases[i].expected_previous, previous);
        EXPECT_TRUE(render_text->IsValidCursorIndex(previous));
    }
}

TEST_F(RenderTextTest, MidGraphemeSelectionBounds)
{
#if defined(OS_WIN)
    // TODO(msw): XP fails due to lack of font support: http://crbug.com/106450
    if (base::win::GetVersion() < base::win::VERSION_VISTA)
        return;
#endif

    // Test that selection bounds may be set amid multi-character graphemes.
    const base::string16 kHindi = WideToUTF16(L"\x0915\x093f");
    const base::string16 kThai = WideToUTF16(L"\x0e08\x0e33");
    const base::string16 cases[] = { kHindi, kThai };

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    for (size_t i = 0; i < arraysize(cases); i++) {
        SCOPED_TRACE(base::StringPrintf("Testing cases[%" PRIuS "]", i));
        render_text->SetText(cases[i]);
        EXPECT_TRUE(render_text->IsValidLogicalIndex(1));
        EXPECT_FALSE(render_text->IsValidCursorIndex(1));
        EXPECT_TRUE(render_text->SelectRange(Range(2, 1)));
        EXPECT_EQ(Range(2, 1), render_text->selection());
        EXPECT_EQ(1U, render_text->cursor_position());
        // Although selection bounds may be set within a multi-character grapheme,
        // cursor movement (e.g. via arrow key) should avoid those indices.
        render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
        EXPECT_EQ(0U, render_text->cursor_position());
        render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
        EXPECT_EQ(2U, render_text->cursor_position());
    }
}

TEST_F(RenderTextTest, FindCursorPosition)
{
    const wchar_t* kTestStrings[] = { kLtrRtl, kLtrRtlLtr, kRtlLtr, kRtlLtrRtl };
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetDisplayRect(Rect(0, 0, 100, 20));
    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("Testing case[%" PRIuS "]", i));
        render_text->SetText(WideToUTF16(kTestStrings[i]));
        for (size_t j = 0; j < render_text->text().length(); ++j) {
            const Range range(render_text->GetGlyphBounds(j));
            // Test a point just inside the leading edge of the glyph bounds.
            int x = range.is_reversed() ? range.GetMax() - 1 : range.GetMin() + 1;
            EXPECT_EQ(j, render_text->FindCursorPosition(Point(x, 0)).caret_pos());
        }
    }
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, EdgeSelectionModels)
{
    // Simple Latin text.
    const base::string16 kLatin = WideToUTF16(L"abc");
    // LTR 2-character grapheme.
    const base::string16 kLTRGrapheme = WideToUTF16(L"\x0915\x093f");
    // LTR 2-character grapheme, LTR a, LTR 2-character grapheme.
    const base::string16 kHindiLatin = WideToUTF16(L"\x0915\x093f"
                                                   L"a"
                                                   L"\x0915\x093f");
    // RTL 2-character grapheme.
    const base::string16 kRTLGrapheme = WideToUTF16(L"\x05e0\x05b8");
    // RTL 2-character grapheme, LTR a, RTL 2-character grapheme.
    const base::string16 kHebrewLatin = WideToUTF16(L"\x05e0\x05b8"
                                                    L"a"
                                                    L"\x05e0\x05b8");

    struct {
        base::string16 text;
        base::i18n::TextDirection expected_text_direction;
    } cases[] = {
        { base::string16(), base::i18n::LEFT_TO_RIGHT },
        { kLatin, base::i18n::LEFT_TO_RIGHT },
        { kLTRGrapheme, base::i18n::LEFT_TO_RIGHT },
        { kHindiLatin, base::i18n::LEFT_TO_RIGHT },
        { kRTLGrapheme, base::i18n::RIGHT_TO_LEFT },
        { kHebrewLatin, base::i18n::RIGHT_TO_LEFT },
    };

#if defined(OS_WIN)
    // TODO(msw): XP fails due to lack of font support: http://crbug.com/106450
    if (base::win::GetVersion() < base::win::VERSION_VISTA)
        return;
#endif

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    for (size_t i = 0; i < arraysize(cases); i++) {
        render_text->SetText(cases[i].text);
        bool ltr = (cases[i].expected_text_direction == base::i18n::LEFT_TO_RIGHT);

        SelectionModel start_edge = render_text->EdgeSelectionModel(ltr ? CURSOR_LEFT : CURSOR_RIGHT);
        EXPECT_EQ(start_edge, SelectionModel(0, CURSOR_BACKWARD));

        SelectionModel end_edge = render_text->EdgeSelectionModel(ltr ? CURSOR_RIGHT : CURSOR_LEFT);
        EXPECT_EQ(end_edge, SelectionModel(cases[i].text.length(), CURSOR_FORWARD));
    }
}

TEST_F(RenderTextTest, SelectAll)
{
    const wchar_t* const cases[] = { kWeak, kLtr, kLtrRtl, kLtrRtlLtr, kRtl, kRtlLtr, kRtlLtrRtl };

    // Ensure that SelectAll respects the |reversed| argument regardless of
    // application locale and text content directionality.
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    const SelectionModel expected_reversed(Range(3, 0), CURSOR_FORWARD);
    const SelectionModel expected_forwards(Range(0, 3), CURSOR_BACKWARD);
    const bool was_rtl = base::i18n::IsRTL();

    for (size_t i = 0; i < 2; ++i) {
        SetRTL(!base::i18n::IsRTL());
        // Test that an empty string produces an empty selection model.
        render_text->SetText(base::string16());
        EXPECT_EQ(render_text->selection_model(), SelectionModel());

        // Test the weak, LTR, RTL, and Bidi string cases.
        for (size_t j = 0; j < arraysize(cases); j++) {
            render_text->SetText(WideToUTF16(cases[j]));
            render_text->SelectAll(false);
            EXPECT_EQ(render_text->selection_model(), expected_forwards);
            render_text->SelectAll(true);
            EXPECT_EQ(render_text->selection_model(), expected_reversed);
        }
    }

    EXPECT_EQ(was_rtl, base::i18n::IsRTL());
}

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
TEST_F(RenderTextTest, MoveCursorLeftRightWithSelection)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(WideToUTF16(L"abc\x05d0\x05d1\x05d2"));
    // Left arrow on select ranging (6, 4).
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(Range(6), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(Range(4), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(Range(5), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(Range(6), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, true);
    EXPECT_EQ(Range(6, 5), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, true);
    EXPECT_EQ(Range(6, 4), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(Range(6), render_text->selection());

    // Right arrow on select ranging (4, 6).
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(Range(0), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(Range(1), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(Range(2), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(Range(3), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(Range(5), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(Range(4), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, true);
    EXPECT_EQ(Range(4, 5), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, true);
    EXPECT_EQ(Range(4, 6), render_text->selection());
    render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(Range(4), render_text->selection());
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, CenteredDisplayOffset)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("abcdefghij"));
    render_text->SetHorizontalAlignment(ALIGN_CENTER);

    const int kEnlargement = 10;
    const int content_width = render_text->GetContentWidth();
    Rect display_rect(0, 0, content_width / 2,
        render_text->font_list().GetHeight());
    render_text->SetDisplayRect(display_rect);

    // Move the cursor to the beginning of the text and, by checking the cursor
    // bounds, make sure no empty space is to the left of the text.
    render_text->SetCursorPosition(0);
    EXPECT_EQ(display_rect.x(), render_text->GetUpdatedCursorBounds().x());

    // Widen the display rect and, by checking the cursor bounds, make sure no
    // empty space is introduced to the left of the text.
    display_rect.Inset(0, 0, -kEnlargement, 0);
    render_text->SetDisplayRect(display_rect);
    EXPECT_EQ(display_rect.x(), render_text->GetUpdatedCursorBounds().x());

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
    // Move the cursor to the end of the text and, by checking the cursor bounds,
    // make sure no empty space is to the right of the text.
    render_text->SetCursorPosition(render_text->text().length());
    EXPECT_EQ(display_rect.right(),
        render_text->GetUpdatedCursorBounds().right());

    // Widen the display rect and, by checking the cursor bounds, make sure no
    // empty space is introduced to the right of the text.
    display_rect.Inset(0, 0, -kEnlargement, 0);
    render_text->SetDisplayRect(display_rect);
    EXPECT_EQ(display_rect.right(),
        render_text->GetUpdatedCursorBounds().right());
#endif // !defined(OS_MACOSX)
}

void MoveLeftRightByWordVerifier(RenderText* render_text,
    const wchar_t* str)
{
    render_text->SetText(WideToUTF16(str));

    // Test moving by word from left ro right.
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, false);
    bool first_word = true;
    while (true) {
        // First, test moving by word from a word break position, such as from
        // "|abc def" to "abc| def".
        SelectionModel start = render_text->selection_model();
        render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
        SelectionModel end = render_text->selection_model();
        if (end == start) // reach the end.
            break;

        // For testing simplicity, each word is a 3-character word.
        int num_of_character_moves = first_word ? 3 : 4;
        first_word = false;
        render_text->MoveCursorTo(start);
        for (int j = 0; j < num_of_character_moves; ++j)
            render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
        EXPECT_EQ(end, render_text->selection_model());

        // Then, test moving by word from positions inside the word, such as from
        // "a|bc def" to "abc| def", and from "ab|c def" to "abc| def".
        for (int j = 1; j < num_of_character_moves; ++j) {
            render_text->MoveCursorTo(start);
            for (int k = 0; k < j; ++k)
                render_text->MoveCursor(CHARACTER_BREAK, CURSOR_RIGHT, false);
            render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
            EXPECT_EQ(end, render_text->selection_model());
        }
    }

    // Test moving by word from right to left.
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    first_word = true;
    while (true) {
        SelectionModel start = render_text->selection_model();
        render_text->MoveCursor(WORD_BREAK, CURSOR_LEFT, false);
        SelectionModel end = render_text->selection_model();
        if (end == start) // reach the end.
            break;

        int num_of_character_moves = first_word ? 3 : 4;
        first_word = false;
        render_text->MoveCursorTo(start);
        for (int j = 0; j < num_of_character_moves; ++j)
            render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
        EXPECT_EQ(end, render_text->selection_model());

        for (int j = 1; j < num_of_character_moves; ++j) {
            render_text->MoveCursorTo(start);
            for (int k = 0; k < j; ++k)
                render_text->MoveCursor(CHARACTER_BREAK, CURSOR_LEFT, false);
            render_text->MoveCursor(WORD_BREAK, CURSOR_LEFT, false);
            EXPECT_EQ(end, render_text->selection_model());
        }
    }
}

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
// TODO(msw): Make these work on Windows.
#if !defined(OS_WIN)
TEST_F(RenderTextTest, MoveLeftRightByWordInBidiText)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    // For testing simplicity, each word is a 3-character word.
    std::vector<const wchar_t*> test;
    test.push_back(L"abc");
    test.push_back(L"abc def");
    test.push_back(L"\x05E1\x05E2\x05E3");
    test.push_back(L"\x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6");
    test.push_back(L"abc \x05E1\x05E2\x05E3");
    test.push_back(L"abc def \x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6");
    test.push_back(L"abc def hij \x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6"
                   L" \x05E7\x05E8\x05E9");

    test.push_back(L"abc \x05E1\x05E2\x05E3 hij");
    test.push_back(L"abc def \x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6 hij opq");
    test.push_back(L"abc def hij \x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6"
                   L" \x05E7\x05E8\x05E9"
                   L" opq rst uvw");

    test.push_back(L"\x05E1\x05E2\x05E3 abc");
    test.push_back(L"\x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6 abc def");
    test.push_back(L"\x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6 \x05E7\x05E8\x05E9"
                   L" abc def hij");

    test.push_back(L"\x05D1\x05D2\x05D3 abc \x05E1\x05E2\x05E3");
    test.push_back(L"\x05D1\x05D2\x05D3 \x05D4\x05D5\x05D6 abc def"
                   L" \x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6");
    test.push_back(L"\x05D1\x05D2\x05D3 \x05D4\x05D5\x05D6 \x05D7\x05D8\x05D9"
                   L" abc def hij \x05E1\x05E2\x05E3 \x05E4\x05E5\x05E6"
                   L" \x05E7\x05E8\x05E9");

    for (size_t i = 0; i < test.size(); ++i)
        MoveLeftRightByWordVerifier(render_text.get(), test[i]);
}

TEST_F(RenderTextTest, MoveLeftRightByWordInBidiText_TestEndOfText)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    render_text->SetText(WideToUTF16(L"ab\x05E1"));
    // Moving the cursor by word from "abC|" to the left should return "|abC".
    // But since end of text is always treated as a word break, it returns
    // position "ab|C".
    // TODO(xji): Need to make it work as expected.
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    render_text->MoveCursor(WORD_BREAK, CURSOR_LEFT, false);
    // EXPECT_EQ(SelectionModel(), render_text->selection_model());

    // Moving the cursor by word from "|abC" to the right returns "abC|".
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, false);
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(SelectionModel(3, CURSOR_FORWARD), render_text->selection_model());

    render_text->SetText(WideToUTF16(L"\x05E1\x05E2"
                                     L"a"));
    // For logical text "BCa", moving the cursor by word from "aCB|" to the left
    // returns "|aCB".
    render_text->MoveCursor(LINE_BREAK, CURSOR_RIGHT, false);
    render_text->MoveCursor(WORD_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(SelectionModel(3, CURSOR_FORWARD), render_text->selection_model());

    // Moving the cursor by word from "|aCB" to the right should return "aCB|".
    // But since end of text is always treated as a word break, it returns
    // position "a|CB".
    // TODO(xji): Need to make it work as expected.
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, false);
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    // EXPECT_EQ(SelectionModel(), render_text->selection_model());
}

TEST_F(RenderTextTest, MoveLeftRightByWordInTextWithMultiSpaces)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(WideToUTF16(L"abc     def"));
    render_text->MoveCursorTo(SelectionModel(5, CURSOR_FORWARD));
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(11U, render_text->cursor_position());

    render_text->MoveCursorTo(SelectionModel(5, CURSOR_FORWARD));
    render_text->MoveCursor(WORD_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(0U, render_text->cursor_position());
}
#endif // !defined(OS_WIN)

TEST_F(RenderTextTest, MoveLeftRightByWordInChineseText)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(WideToUTF16(L"\x6211\x4EEC\x53BB\x516C\x56ED\x73A9"));
    render_text->MoveCursor(LINE_BREAK, CURSOR_LEFT, false);
    EXPECT_EQ(0U, render_text->cursor_position());
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(2U, render_text->cursor_position());
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(3U, render_text->cursor_position());
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(5U, render_text->cursor_position());
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(6U, render_text->cursor_position());
    render_text->MoveCursor(WORD_BREAK, CURSOR_RIGHT, false);
    EXPECT_EQ(6U, render_text->cursor_position());
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, StringSizeSanity)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(UTF8ToUTF16("Hello World"));
    const Size string_size = render_text->GetStringSize();
    EXPECT_GT(string_size.width(), 0);
    EXPECT_GT(string_size.height(), 0);
}

TEST_F(RenderTextTest, StringSizeLongStrings)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    Size previous_string_size;
    for (size_t length = 10; length < 1000000; length *= 10) {
        render_text->SetText(base::string16(length, 'a'));
        const Size string_size = render_text->GetStringSize();
        EXPECT_GT(string_size.width(), previous_string_size.width());
        EXPECT_GT(string_size.height(), 0);
        previous_string_size = string_size;
    }
}

// TODO(asvitkine): This test fails because PlatformFontMac uses point font
//                  sizes instead of pixel sizes like other implementations.
#if !defined(OS_MACOSX)
TEST_F(RenderTextTest, StringSizeEmptyString)
{
    // Ascent and descent of Arial and Symbol are different on most platforms.
    const FontList font_list("Arial,Symbol, 16px");
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetFontList(font_list);
    render_text->SetDisplayRect(Rect(0, 0, 0, font_list.GetHeight()));

    // The empty string respects FontList metrics for non-zero height
    // and baseline.
    render_text->SetText(base::string16());
    EXPECT_EQ(font_list.GetHeight(), render_text->GetStringSize().height());
    EXPECT_EQ(0, render_text->GetStringSize().width());
    EXPECT_EQ(font_list.GetBaseline(), render_text->GetBaseline());

    render_text->SetText(UTF8ToUTF16(" "));
    EXPECT_EQ(font_list.GetHeight(), render_text->GetStringSize().height());
    EXPECT_EQ(font_list.GetBaseline(), render_text->GetBaseline());
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, StringSizeRespectsFontListMetrics)
{
    // Check that Arial and Symbol have different font metrics.
    Font arial_font("Arial", 16);
    ASSERT_EQ("arial",
        base::ToLowerASCII(arial_font.GetActualFontNameForTesting()));
    Font symbol_font("Symbol", 16);
    ASSERT_EQ("symbol",
        base::ToLowerASCII(symbol_font.GetActualFontNameForTesting()));
    EXPECT_NE(arial_font.GetHeight(), symbol_font.GetHeight());
    EXPECT_NE(arial_font.GetBaseline(), symbol_font.GetBaseline());
    // "a" should be rendered with Arial, not with Symbol.
    const char* arial_font_text = "a";
    // "®" (registered trademark symbol) should be rendered with Symbol,
    // not with Arial.
    const char* symbol_font_text = "\xC2\xAE";

    Font smaller_font = arial_font;
    Font larger_font = symbol_font;
    const char* smaller_font_text = arial_font_text;
    const char* larger_font_text = symbol_font_text;
    if (symbol_font.GetHeight() < arial_font.GetHeight() && symbol_font.GetBaseline() < arial_font.GetBaseline()) {
        std::swap(smaller_font, larger_font);
        std::swap(smaller_font_text, larger_font_text);
    }
    ASSERT_LT(smaller_font.GetHeight(), larger_font.GetHeight());
    ASSERT_LT(smaller_font.GetBaseline(), larger_font.GetBaseline());

    // Check |smaller_font_text| is rendered with the smaller font.
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(UTF8ToUTF16(smaller_font_text));
    render_text->SetFontList(FontList(smaller_font));
    render_text->SetDisplayRect(Rect(0, 0, 0,
        render_text->font_list().GetHeight()));
    EXPECT_EQ(smaller_font.GetHeight(), render_text->GetStringSize().height());
    EXPECT_EQ(smaller_font.GetBaseline(), render_text->GetBaseline());

    // Layout the same text with mixed fonts.  The text should be rendered with
    // the smaller font, but the height and baseline are determined with the
    // metrics of the font list, which is equal to the larger font.
    std::vector<Font> fonts;
    fonts.push_back(smaller_font); // The primary font is the smaller font.
    fonts.push_back(larger_font);
    const FontList font_list(fonts);
    render_text->SetFontList(font_list);
    render_text->SetDisplayRect(Rect(0, 0, 0,
        render_text->font_list().GetHeight()));
    EXPECT_LT(smaller_font.GetHeight(), render_text->GetStringSize().height());
    EXPECT_LT(smaller_font.GetBaseline(), render_text->GetBaseline());
    EXPECT_EQ(font_list.GetHeight(), render_text->GetStringSize().height());
    EXPECT_EQ(font_list.GetBaseline(), render_text->GetBaseline());
}

TEST_F(RenderTextTest, MinLineHeight)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    render_text->SetText(ASCIIToUTF16("Hello!"));
    SizeF default_size = render_text->GetStringSizeF();
    ASSERT_NE(0, default_size.height());
    ASSERT_NE(0, default_size.width());

    render_text->SetMinLineHeight(default_size.height() / 2);
    EXPECT_EQ(default_size.ToString(), render_text->GetStringSizeF().ToString());

    render_text->SetMinLineHeight(default_size.height() * 2);
    SizeF taller_size = render_text->GetStringSizeF();
    EXPECT_EQ(default_size.height() * 2, taller_size.height());
    EXPECT_EQ(default_size.width(), taller_size.width());
}

TEST_F(RenderTextTest, SetFontList)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetFontList(FontList("Arial,Symbol, 13px"));
    const std::vector<Font>& fonts = render_text->font_list().GetFonts();
    ASSERT_EQ(2U, fonts.size());
    EXPECT_EQ("Arial", fonts[0].GetFontName());
    EXPECT_EQ("Symbol", fonts[1].GetFontName());
    EXPECT_EQ(13, render_text->font_list().GetFontSize());
}

TEST_F(RenderTextTest, StringSizeBoldWidth)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(UTF8ToUTF16("Hello World"));

    const int plain_width = render_text->GetStringSize().width();
    EXPECT_GT(plain_width, 0);

    // Apply a bold style and check that the new width is greater.
    render_text->SetStyle(BOLD, true);
    const int bold_width = render_text->GetStringSize().width();
    EXPECT_GT(bold_width, plain_width);

    // Now, apply a plain style over the first word only.
    render_text->ApplyStyle(BOLD, false, Range(0, 5));
    const int plain_bold_width = render_text->GetStringSize().width();
    EXPECT_GT(plain_bold_width, plain_width);
    EXPECT_LT(plain_bold_width, bold_width);
}

TEST_F(RenderTextTest, StringSizeHeight)
{
    base::string16 cases[] = {
        WideToUTF16(L"Hello World!"), // English
        WideToUTF16(L"\x6328\x62f6"), // Japanese
        WideToUTF16(L"\x0915\x093f"), // Hindi
        WideToUTF16(L"\x05e0\x05b8"), // Hebrew
    };

    const FontList default_font_list;
    const FontList& larger_font_list = default_font_list.DeriveWithSizeDelta(24);
    EXPECT_GT(larger_font_list.GetHeight(), default_font_list.GetHeight());

    for (size_t i = 0; i < arraysize(cases); i++) {
        scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
        render_text->SetFontList(default_font_list);
        render_text->SetText(cases[i]);

        const int height1 = render_text->GetStringSize().height();
        EXPECT_GT(height1, 0);

        // Check that setting the larger font increases the height.
        render_text->SetFontList(larger_font_list);
        const int height2 = render_text->GetStringSize().height();
        EXPECT_GT(height2, height1);
    }
}

TEST_F(RenderTextTest, GetBaselineSanity)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(UTF8ToUTF16("Hello World"));
    const int baseline = render_text->GetBaseline();
    EXPECT_GT(baseline, 0);
}

TEST_F(RenderTextTest, CursorBoundsInReplacementMode)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("abcdefg"));
    render_text->SetDisplayRect(Rect(100, 17));
    SelectionModel sel_b(1, CURSOR_FORWARD);
    SelectionModel sel_c(2, CURSOR_FORWARD);
    Rect cursor_around_b = render_text->GetCursorBounds(sel_b, false);
    Rect cursor_before_b = render_text->GetCursorBounds(sel_b, true);
    Rect cursor_before_c = render_text->GetCursorBounds(sel_c, true);
    EXPECT_EQ(cursor_around_b.x(), cursor_before_b.x());
    EXPECT_EQ(cursor_around_b.right(), cursor_before_c.x());
}

TEST_F(RenderTextTest, GetTextOffset)
{
    // The default horizontal text offset differs for LTR and RTL, and is only set
    // when the RenderText object is created.  This test will check the default in
    // LTR mode, and the next test will check the RTL default.
    const bool was_rtl = base::i18n::IsRTL();
    SetRTL(false);
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("abcdefg"));
    render_text->SetFontList(FontList("Arial, 13px"));

    // Set display area's size equal to the font size.
    const Size font_size(render_text->GetContentWidth(),
        render_text->font_list().GetHeight());
    Rect display_rect(font_size);
    render_text->SetDisplayRect(display_rect);

    Vector2d offset = render_text->GetLineOffset(0);
    EXPECT_TRUE(offset.IsZero());

    const int kEnlargementX = 2;
    display_rect.Inset(0, 0, -kEnlargementX, 0);
    render_text->SetDisplayRect(display_rect);

    // Check the default horizontal alignment.
    offset = render_text->GetLineOffset(0);
    EXPECT_EQ(0, offset.x());

    // Check explicitly setting the horizontal alignment.
    render_text->SetHorizontalAlignment(ALIGN_LEFT);
    offset = render_text->GetLineOffset(0);
    EXPECT_EQ(0, offset.x());
    render_text->SetHorizontalAlignment(ALIGN_CENTER);
    offset = render_text->GetLineOffset(0);
    EXPECT_EQ(kEnlargementX / 2, offset.x());
    render_text->SetHorizontalAlignment(ALIGN_RIGHT);
    offset = render_text->GetLineOffset(0);
    EXPECT_EQ(kEnlargementX, offset.x());

    // Check that text is vertically centered within taller display rects.
    const int kEnlargementY = display_rect.height();
    display_rect.Inset(0, 0, 0, -kEnlargementY);
    render_text->SetDisplayRect(display_rect);
    const Vector2d prev_offset = render_text->GetLineOffset(0);
    display_rect.Inset(0, 0, 0, -2 * kEnlargementY);
    render_text->SetDisplayRect(display_rect);
    offset = render_text->GetLineOffset(0);
    EXPECT_EQ(prev_offset.y() + kEnlargementY, offset.y());

    SetRTL(was_rtl);
}

TEST_F(RenderTextTest, GetTextOffsetHorizontalDefaultInRTL)
{
    // This only checks the default horizontal alignment in RTL mode; all other
    // GetLineOffset(0) attributes are checked by the test above.
    const bool was_rtl = base::i18n::IsRTL();
    SetRTL(true);
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("abcdefg"));
    render_text->SetFontList(FontList("Arial, 13px"));
    const int kEnlargement = 2;
    const Size font_size(render_text->GetContentWidth() + kEnlargement,
        render_text->GetStringSize().height());
    Rect display_rect(font_size);
    render_text->SetDisplayRect(display_rect);
    Vector2d offset = render_text->GetLineOffset(0);
    EXPECT_EQ(kEnlargement, offset.x());
    SetRTL(was_rtl);
}

TEST_F(RenderTextTest, SetDisplayOffset)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("abcdefg"));
    render_text->SetFontList(FontList("Arial, 13px"));

    const Size font_size(render_text->GetContentWidth(),
        render_text->font_list().GetHeight());
    const int kEnlargement = 10;

    // Set display width |kEnlargement| pixels greater than content width and test
    // different possible situations. In this case the only possible display
    // offset is zero.
    Rect display_rect(font_size);
    display_rect.Inset(0, 0, -kEnlargement, 0);
    render_text->SetDisplayRect(display_rect);

    struct {
        HorizontalAlignment alignment;
        int offset;
    } small_content_cases[] = {
        { ALIGN_LEFT, -kEnlargement },
        { ALIGN_LEFT, 0 },
        { ALIGN_LEFT, kEnlargement },
        { ALIGN_RIGHT, -kEnlargement },
        { ALIGN_RIGHT, 0 },
        { ALIGN_RIGHT, kEnlargement },
        { ALIGN_CENTER, -kEnlargement },
        { ALIGN_CENTER, 0 },
        { ALIGN_CENTER, kEnlargement },
    };

    for (size_t i = 0; i < arraysize(small_content_cases); i++) {
        render_text->SetHorizontalAlignment(small_content_cases[i].alignment);
        render_text->SetDisplayOffset(small_content_cases[i].offset);
        EXPECT_EQ(0, render_text->GetUpdatedDisplayOffset().x());
    }

    // Set display width |kEnlargement| pixels less than content width and test
    // different possible situations.
    display_rect = Rect(font_size);
    display_rect.Inset(0, 0, kEnlargement, 0);
    render_text->SetDisplayRect(display_rect);

    struct {
        HorizontalAlignment alignment;
        int offset;
        int expected_offset;
    } large_content_cases[] = {
        // When text is left-aligned, display offset can be in range
        // [-kEnlargement, 0].
        { ALIGN_LEFT, -2 * kEnlargement, -kEnlargement },
        { ALIGN_LEFT, -kEnlargement / 2, -kEnlargement / 2 },
        { ALIGN_LEFT, kEnlargement, 0 },
        // When text is right-aligned, display offset can be in range
        // [0, kEnlargement].
        { ALIGN_RIGHT, -kEnlargement, 0 },
        { ALIGN_RIGHT, kEnlargement / 2, kEnlargement / 2 },
        { ALIGN_RIGHT, 2 * kEnlargement, kEnlargement },
        // When text is center-aligned, display offset can be in range
        // [-kEnlargement / 2 - 1, (kEnlargement - 1) / 2].
        { ALIGN_CENTER, -kEnlargement, -kEnlargement / 2 - 1 },
        { ALIGN_CENTER, -kEnlargement / 4, -kEnlargement / 4 },
        { ALIGN_CENTER, kEnlargement / 4, kEnlargement / 4 },
        { ALIGN_CENTER, kEnlargement, (kEnlargement - 1) / 2 },
    };

    for (size_t i = 0; i < arraysize(large_content_cases); i++) {
        render_text->SetHorizontalAlignment(large_content_cases[i].alignment);
        render_text->SetDisplayOffset(large_content_cases[i].offset);
        EXPECT_EQ(large_content_cases[i].expected_offset,
            render_text->GetUpdatedDisplayOffset().x());
    }
}

TEST_F(RenderTextTest, SameFontForParentheses)
{
    struct {
        const base::char16 left_char;
        const base::char16 right_char;
    } punctuation_pairs[] = {
        { '(', ')' },
        { '{', '}' },
        { '<', '>' },
    };
    struct {
        base::string16 text;
    } cases[] = {
        // English(English)
        { WideToUTF16(L"Hello World(a)") },
        // English(English)English
        { WideToUTF16(L"Hello World(a)Hello World") },

        // Japanese(English)
        { WideToUTF16(L"\x6328\x62f6(a)") },
        // Japanese(English)Japanese
        { WideToUTF16(L"\x6328\x62f6(a)\x6328\x62f6") },
        // English(Japanese)English
        { WideToUTF16(L"Hello World(\x6328\x62f6)Hello World") },

        // Hindi(English)
        { WideToUTF16(L"\x0915\x093f(a)") },
        // Hindi(English)Hindi
        { WideToUTF16(L"\x0915\x093f(a)\x0915\x093f") },
        // English(Hindi)English
        { WideToUTF16(L"Hello World(\x0915\x093f)Hello World") },

        // Hebrew(English)
        { WideToUTF16(L"\x05e0\x05b8(a)") },
        // Hebrew(English)Hebrew
        { WideToUTF16(L"\x05e0\x05b8(a)\x05e0\x05b8") },
        // English(Hebrew)English
        { WideToUTF16(L"Hello World(\x05e0\x05b8)Hello World") },
    };

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    for (size_t i = 0; i < arraysize(cases); ++i) {
        base::string16 text = cases[i].text;
        const size_t start_paren_char_index = text.find('(');
        ASSERT_NE(base::string16::npos, start_paren_char_index);
        const size_t end_paren_char_index = text.find(')');
        ASSERT_NE(base::string16::npos, end_paren_char_index);

        for (size_t j = 0; j < arraysize(punctuation_pairs); ++j) {
            text[start_paren_char_index] = punctuation_pairs[j].left_char;
            text[end_paren_char_index] = punctuation_pairs[j].right_char;
            render_text->SetText(text);

            const std::vector<RenderText::FontSpan> spans = render_text->GetFontSpansForTesting();

            int start_paren_span_index = -1;
            int end_paren_span_index = -1;
            for (size_t k = 0; k < spans.size(); ++k) {
                if (IndexInRange(spans[k].second, start_paren_char_index))
                    start_paren_span_index = k;
                if (IndexInRange(spans[k].second, end_paren_char_index))
                    end_paren_span_index = k;
            }
            ASSERT_NE(-1, start_paren_span_index);
            ASSERT_NE(-1, end_paren_span_index);

            const Font& start_font = spans[start_paren_span_index].first;
            const Font& end_font = spans[end_paren_span_index].first;
            EXPECT_EQ(start_font.GetFontName(), end_font.GetFontName());
            EXPECT_EQ(start_font.GetFontSize(), end_font.GetFontSize());
            EXPECT_EQ(start_font.GetStyle(), end_font.GetStyle());
        }
    }
}

// Make sure the caret width is always >=1 so that the correct
// caret is drawn at high DPI. crbug.com/164100.
TEST_F(RenderTextTest, CaretWidth)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16("abcdefg"));
    EXPECT_GE(render_text->GetUpdatedCursorBounds().width(), 1);
}

TEST_F(RenderTextTest, SelectWord)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(ASCIIToUTF16(" foo  a.bc.d bar"));

    struct {
        size_t cursor;
        size_t selection_start;
        size_t selection_end;
    } cases[] = {
        { 0, 0, 1 },
        { 1, 1, 4 },
        { 2, 1, 4 },
        { 3, 1, 4 },
        { 4, 4, 6 },
        { 5, 4, 6 },
        { 6, 6, 7 },
        { 7, 7, 8 },
        { 8, 8, 10 },
        { 9, 8, 10 },
        { 10, 10, 11 },
        { 11, 11, 12 },
        { 12, 12, 13 },
        { 13, 13, 16 },
        { 14, 13, 16 },
        { 15, 13, 16 },
        { 16, 13, 16 },
    };

    for (size_t i = 0; i < arraysize(cases); ++i) {
        render_text->SetCursorPosition(cases[i].cursor);
        render_text->SelectWord();
        EXPECT_EQ(Range(cases[i].selection_start, cases[i].selection_end),
            render_text->selection());
    }
}

// Make sure the last word is selected when the cursor is at text.length().
TEST_F(RenderTextTest, LastWordSelected)
{
    const std::string kTestURL1 = "http://www.google.com";
    const std::string kTestURL2 = "http://www.google.com/something/";

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    render_text->SetText(ASCIIToUTF16(kTestURL1));
    render_text->SetCursorPosition(kTestURL1.length());
    render_text->SelectWord();
    EXPECT_EQ(ASCIIToUTF16("com"), GetSelectedText(render_text.get()));
    EXPECT_FALSE(render_text->selection().is_reversed());

    render_text->SetText(ASCIIToUTF16(kTestURL2));
    render_text->SetCursorPosition(kTestURL2.length());
    render_text->SelectWord();
    EXPECT_EQ(ASCIIToUTF16("/"), GetSelectedText(render_text.get()));
    EXPECT_FALSE(render_text->selection().is_reversed());
}

// When given a non-empty selection, SelectWord should expand the selection to
// nearest word boundaries.
TEST_F(RenderTextTest, SelectMultipleWords)
{
    const std::string kTestURL = "http://www.google.com";

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());

    render_text->SetText(ASCIIToUTF16(kTestURL));
    render_text->SelectRange(Range(16, 20));
    render_text->SelectWord();
    EXPECT_EQ(ASCIIToUTF16("google.com"), GetSelectedText(render_text.get()));
    EXPECT_FALSE(render_text->selection().is_reversed());

    // SelectWord should preserve the selection direction.
    render_text->SelectRange(Range(20, 16));
    render_text->SelectWord();
    EXPECT_EQ(ASCIIToUTF16("google.com"), GetSelectedText(render_text.get()));
    EXPECT_TRUE(render_text->selection().is_reversed());
}

// TODO(asvitkine): RenderTextMac cursor movements. http://crbug.com/131618
#if !defined(OS_MACOSX)
TEST_F(RenderTextTest, DisplayRectShowsCursorLTR)
{
    ASSERT_FALSE(base::i18n::IsRTL());
    ASSERT_FALSE(base::i18n::ICUIsRTL());

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(WideToUTF16(L"abcdefghijklmnopqrstuvwxzyabcdefg"));
    render_text->MoveCursorTo(SelectionModel(render_text->text().length(),
        CURSOR_FORWARD));
    int width = render_text->GetStringSize().width();
    ASSERT_GT(width, 10);

    // Ensure that the cursor is placed at the width of its preceding text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(width, render_text->GetUpdatedCursorBounds().x());

    // Ensure that shrinking the display rectangle keeps the cursor in view.
    render_text->SetDisplayRect(Rect(width - 10, 1));
    EXPECT_EQ(render_text->display_rect().width(),
        render_text->GetUpdatedCursorBounds().right());

    // Ensure that the text will pan to fill its expanding display rectangle.
    render_text->SetDisplayRect(Rect(width - 5, 1));
    EXPECT_EQ(render_text->display_rect().width(),
        render_text->GetUpdatedCursorBounds().right());

    // Ensure that a sufficiently large display rectangle shows all the text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(width, render_text->GetUpdatedCursorBounds().x());

    // Repeat the test with RTL text.
    render_text->SetText(WideToUTF16(L"\x5d0\x5d1\x5d2\x5d3\x5d4\x5d5\x5d6\x5d7"
                                     L"\x5d8\x5d9\x5da\x5db\x5dc\x5dd\x5de\x5df"));
    render_text->MoveCursorTo(SelectionModel(0, CURSOR_FORWARD));
    width = render_text->GetStringSize().width();
    ASSERT_GT(width, 10);

    // Ensure that the cursor is placed at the width of its preceding text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(width, render_text->GetUpdatedCursorBounds().x());

    // Ensure that shrinking the display rectangle keeps the cursor in view.
    render_text->SetDisplayRect(Rect(width - 10, 1));
    EXPECT_EQ(render_text->display_rect().width(),
        render_text->GetUpdatedCursorBounds().right());

    // Ensure that the text will pan to fill its expanding display rectangle.
    render_text->SetDisplayRect(Rect(width - 5, 1));
    EXPECT_EQ(render_text->display_rect().width(),
        render_text->GetUpdatedCursorBounds().right());

    // Ensure that a sufficiently large display rectangle shows all the text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(width, render_text->GetUpdatedCursorBounds().x());
}
#endif // !defined(OS_MACOSX)

TEST_F(RenderTextTest, DisplayRectShowsCursorRTL)
{
    // Set the application default text direction to RTL.
    const bool was_rtl = base::i18n::IsRTL();
    SetRTL(true);

    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetText(WideToUTF16(L"abcdefghijklmnopqrstuvwxzyabcdefg"));
    render_text->MoveCursorTo(SelectionModel(0, CURSOR_FORWARD));
    int width = render_text->GetStringSize().width();
    ASSERT_GT(width, 10);

    // Ensure that the cursor is placed at the width of its preceding text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(render_text->display_rect().width() - width - 1,
        render_text->GetUpdatedCursorBounds().x());

    // Ensure that shrinking the display rectangle keeps the cursor in view.
    render_text->SetDisplayRect(Rect(width - 10, 1));
    EXPECT_EQ(0, render_text->GetUpdatedCursorBounds().x());

    // Ensure that the text will pan to fill its expanding display rectangle.
    render_text->SetDisplayRect(Rect(width - 5, 1));
    EXPECT_EQ(0, render_text->GetUpdatedCursorBounds().x());

    // Ensure that a sufficiently large display rectangle shows all the text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(render_text->display_rect().width() - width - 1,
        render_text->GetUpdatedCursorBounds().x());

    // Repeat the test with RTL text.
    render_text->SetText(WideToUTF16(L"\x5d0\x5d1\x5d2\x5d3\x5d4\x5d5\x5d6\x5d7"
                                     L"\x5d8\x5d9\x5da\x5db\x5dc\x5dd\x5de\x5df"));
    render_text->MoveCursorTo(SelectionModel(render_text->text().length(),
        CURSOR_FORWARD));
    width = render_text->GetStringSize().width();
    ASSERT_GT(width, 10);

    // Ensure that the cursor is placed at the width of its preceding text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(render_text->display_rect().width() - width - 1,
        render_text->GetUpdatedCursorBounds().x());

    // Ensure that shrinking the display rectangle keeps the cursor in view.
    render_text->SetDisplayRect(Rect(width - 10, 1));
    EXPECT_EQ(0, render_text->GetUpdatedCursorBounds().x());

    // Ensure that the text will pan to fill its expanding display rectangle.
    render_text->SetDisplayRect(Rect(width - 5, 1));
    EXPECT_EQ(0, render_text->GetUpdatedCursorBounds().x());

    // Ensure that a sufficiently large display rectangle shows all the text.
    render_text->SetDisplayRect(Rect(width + 10, 1));
    EXPECT_EQ(render_text->display_rect().width() - width - 1,
        render_text->GetUpdatedCursorBounds().x());

    // Reset the application default text direction to LTR.
    SetRTL(was_rtl);
    EXPECT_EQ(was_rtl, base::i18n::IsRTL());
}

// Changing colors between or inside ligated glyphs should not break shaping.
TEST_F(RenderTextTest, SelectionKeepsLigatures)
{
    const wchar_t* kTestStrings[] = { L"\x644\x623", L"\x633\x627" };
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->set_selection_color(SK_ColorRED);
    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        render_text->SetText(WideToUTF16(kTestStrings[i]));
        const int expected_width = render_text->GetStringSize().width();
        render_text->MoveCursorTo(SelectionModel(Range(0, 1), CURSOR_FORWARD));
        EXPECT_EQ(expected_width, render_text->GetStringSize().width());
        // Drawing the text should not DCHECK or crash; see http://crbug.com/262119
        render_text->Draw(&canvas);
        render_text->MoveCursorTo(SelectionModel(0, CURSOR_FORWARD));
    }
}

// Ensure strings wrap onto multiple lines for a small available width.
TEST_F(RenderTextTest, Multiline_MinWidth)
{
    const wchar_t* kTestStrings[] = { kWeak, kLtr, kLtrRtl, kLtrRtlLtr, kRtl,
        kRtlLtr, kRtlLtrRtl };

    RenderTextHarfBuzz render_text;
    render_text.SetDisplayRect(Rect(1, 1000));
    render_text.SetMultiline(true);
    render_text.SetWordWrapBehavior(WRAP_LONG_WORDS);
    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(WideToUTF16(kTestStrings[i]));
        render_text.Draw(&canvas);
        EXPECT_GT(render_text.lines_.size(), 1U);
    }
}

// Ensure strings wrap onto multiple lines for a normal available width.
TEST_F(RenderTextTest, Multiline_NormalWidth)
{
    const struct {
        const wchar_t* const text;
        const Range first_line_char_range;
        const Range second_line_char_range;
        bool is_ltr;
    } kTestStrings[] = {
        { L"abc defg hijkl", Range(0, 9), Range(9, 14), true },
        { L"qwertyzxcvbn", Range(0, 10), Range(10, 12), true },
        { L"\x0627\x0644\x0644\x063A\x0629 "
          L"\x0627\x0644\x0639\x0631\x0628\x064A\x0629",
            Range(0, 6), Range(6, 13), false },
        { L"\x062A\x0641\x0627\x062D \x05EA\x05E4\x05D5\x05D6\x05D9"
          L"\x05DA\x05DB\x05DD",
            Range(0, 5), Range(5, 13), false }
    };

    RenderTextHarfBuzz render_text;
    // Specify the fixed width for characters to suppress the possible variations
    // of linebreak results.
    render_text.set_glyph_width_for_test(5);
    render_text.SetDisplayRect(Rect(50, 1000));
    render_text.SetMultiline(true);
    render_text.SetWordWrapBehavior(WRAP_LONG_WORDS);
    render_text.SetHorizontalAlignment(ALIGN_TO_HEAD);

    Canvas canvas;
    TestSkiaTextRenderer renderer(&canvas);

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(WideToUTF16(kTestStrings[i].text));
        render_text.EnsureLayout();
        render_text.DrawVisualText(&renderer);

        ASSERT_EQ(2U, render_text.lines_.size());
        ASSERT_EQ(1U, render_text.lines_[0].segments.size());
        EXPECT_EQ(kTestStrings[i].first_line_char_range,
            render_text.lines_[0].segments[0].char_range);
        ASSERT_EQ(1U, render_text.lines_[1].segments.size());
        EXPECT_EQ(kTestStrings[i].second_line_char_range,
            render_text.lines_[1].segments[0].char_range);

        std::vector<TestSkiaTextRenderer::TextLog> text_log;
        renderer.GetTextLogAndReset(&text_log);
        ASSERT_EQ(2U, text_log.size());
        // NOTE: this expectation compares the character length and glyph counts,
        // which isn't always equal. This is okay only because all the test
        // strings are simple (like, no compound characters nor UTF16-surrogate
        // pairs). Be careful in case more complicated test strings are added.
        EXPECT_EQ(kTestStrings[i].first_line_char_range.length(),
            text_log[0].glyph_count);
        EXPECT_EQ(kTestStrings[i].second_line_char_range.length(),
            text_log[1].glyph_count);
        EXPECT_LT(text_log[0].origin.y(), text_log[1].origin.y());
        if (kTestStrings[i].is_ltr) {
            EXPECT_EQ(0, text_log[0].origin.x());
            EXPECT_EQ(0, text_log[1].origin.x());
        } else {
            EXPECT_LT(0, text_log[0].origin.x());
            EXPECT_LT(0, text_log[1].origin.x());
        }
    }
}

// Ensure strings don't wrap onto multiple lines for a sufficient available
// width.
TEST_F(RenderTextTest, Multiline_SufficientWidth)
{
    const wchar_t* kTestStrings[] = { L"", L" ", L".", L" . ", L"abc", L"a b c",
        L"\x62E\x628\x632", L"\x62E \x628 \x632" };

    RenderTextHarfBuzz render_text;
    render_text.SetDisplayRect(Rect(1000, 1000));
    render_text.SetMultiline(true);
    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(WideToUTF16(kTestStrings[i]));
        render_text.Draw(&canvas);
        EXPECT_EQ(1U, render_text.lines_.size());
    }
}

TEST_F(RenderTextTest, Multiline_Newline)
{
    const struct {
        const wchar_t* const text;
        const size_t lines_count;
        // Ranges of the characters on each line preceding the newline.
        const Range line_char_ranges[3];
    } kTestStrings[] = {
        { L"abc\ndef", 2ul, { Range(0, 3), Range(4, 7), Range::InvalidRange() } },
        { L"a \n b ", 2ul, { Range(0, 2), Range(3, 6), Range::InvalidRange() } },
        { L"ab\n", 2ul, { Range(0, 2), Range(), Range::InvalidRange() } },
        { L"a\n\nb", 3ul, { Range(0, 1), Range(), Range(3, 4) } },
        { L"\nab", 2ul, { Range(), Range(1, 3), Range::InvalidRange() } },
        { L"\n", 2ul, { Range(), Range(), Range::InvalidRange() } },
    };

    RenderTextHarfBuzz render_text;
    render_text.SetDisplayRect(Rect(200, 1000));
    render_text.SetMultiline(true);
    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(WideToUTF16(kTestStrings[i].text));
        render_text.Draw(&canvas);
        EXPECT_EQ(kTestStrings[i].lines_count, render_text.lines_.size());
        if (kTestStrings[i].lines_count != render_text.lines_.size())
            continue;

        for (size_t j = 0; j < kTestStrings[i].lines_count; ++j) {
            SCOPED_TRACE(base::StringPrintf("Line %" PRIuS "", j));
            // There might be multiple segments in one line. Merge all the segments
            // ranges in the same line.
            const size_t segment_size = render_text.lines()[j].segments.size();
            Range line_range;
            if (segment_size > 0)
                line_range = Range(
                    render_text.lines()[j].segments[0].char_range.start(),
                    render_text.lines()[j].segments[segment_size - 1].char_range.end());
            EXPECT_EQ(kTestStrings[i].line_char_ranges[j], line_range);
        }
    }
}

// Make sure that multiline mode ignores elide behavior.
TEST_F(RenderTextTest, Multiline_IgnoreElide)
{
    const wchar_t kTestString[] = L"very very very long string xxxxxxxxxxxxxxxxxxxxxxxxxx";
    const wchar_t kEllipsis[] = L"\x2026";

    RenderTextHarfBuzz render_text;
    render_text.SetElideBehavior(ELIDE_TAIL);
    render_text.SetDisplayRect(Rect(20, 1000));
    render_text.SetText(base::WideToUTF16(kTestString));
    EXPECT_NE(base::string16::npos,
        render_text.GetDisplayText().find(base::WideToUTF16(kEllipsis)));

    render_text.SetMultiline(true);
    EXPECT_EQ(base::string16::npos,
        render_text.GetDisplayText().find(base::WideToUTF16(kEllipsis)));
}

TEST_F(RenderTextTest, Multiline_NewlineCharacterReplacement)
{
    const wchar_t* kTestStrings[] = {
        L"abc\ndef",
        L"a \n b ",
        L"ab\n",
        L"a\n\nb",
        L"\nab",
        L"\n",
    };

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        RenderTextHarfBuzz render_text;
        render_text.SetDisplayRect(Rect(200, 1000));
        render_text.SetText(WideToUTF16(kTestStrings[i]));

        base::string16 display_text = render_text.GetDisplayText();
        // If RenderText is not multiline, the newline characters are replaced
        // by symbols, therefore the character should be changed.
        EXPECT_NE(WideToUTF16(kTestStrings[i]), render_text.GetDisplayText());

        // Setting multiline will fix this, the newline characters will be back
        // to the original text.
        render_text.SetMultiline(true);
        EXPECT_EQ(WideToUTF16(kTestStrings[i]), render_text.GetDisplayText());
    }
}

// Ensure horizontal alignment works in multiline mode.
TEST_F(RenderTextTest, Multiline_HorizontalAlignment)
{
    const struct {
        const wchar_t* const text;
        const gfx::HorizontalAlignment alignment;
    } kTestStrings[] = {
        { L"abcdefghij\nhijkl", gfx::ALIGN_LEFT },
        { L"nhijkl\nabcdefghij", gfx::ALIGN_LEFT },
        // hebrew, 2nd line shorter
        { L"\x5d0\x5d1\x5d2\x5d3\x5d4\x5d5\x5d6\x5d7\n\x5d0\x5d1\x5d2\x5d3",
            gfx::ALIGN_RIGHT },
        // hebrew, 2nd line longer
        { L"\x5d0\x5d1\x5d2\x5d3\n\x5d0\x5d1\x5d2\x5d3\x5d4\x5d5\x5d6\x5d7",
            gfx::ALIGN_RIGHT },
        // arabic, 2nd line shorter
        { L"\x62a\x62b\x62c\x62d\x62e\x62f\x630\n\x660\x661\x662\x663\x664",
            gfx::ALIGN_RIGHT },
        // arabic, 2nd line longer
        { L"\x660\x661\x662\x663\x664\n\x62a\x62b\x62c\x62d\x62e\x62f\x630",
            gfx::ALIGN_RIGHT },
    };
    const int kGlyphSize = 5;
    RenderTextHarfBuzz render_text;
    render_text.SetHorizontalAlignment(gfx::ALIGN_TO_HEAD);
    render_text.set_glyph_width_for_test(kGlyphSize);
    render_text.SetDisplayRect(Rect(100, 1000));
    render_text.SetMultiline(true);

    Canvas canvas;
    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "] %ls", i,
            kTestStrings[i].text));
        render_text.SetText(WideToUTF16(kTestStrings[i].text));
        render_text.Draw(&canvas);
        ASSERT_LE(2u, render_text.lines().size());
        if (kTestStrings[i].alignment == gfx::ALIGN_LEFT) {
            EXPECT_EQ(0, render_text.GetAlignmentOffset(0).x());
            EXPECT_EQ(0, render_text.GetAlignmentOffset(1).x());
        } else {
            std::vector<base::string16> lines = base::SplitString(
                base::WideToUTF16(kTestStrings[i].text),
                base::string16(1, '\n'), base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
            ASSERT_EQ(2u, lines.size());
            int difference = (lines[0].length() - lines[1].length()) * kGlyphSize;
            EXPECT_EQ(render_text.GetAlignmentOffset(0).x() + difference,
                render_text.GetAlignmentOffset(1).x());
        }
    }
}

TEST_F(RenderTextTest, Multiline_WordWrapBehavior)
{
    const int kGlyphSize = 5;
    const struct {
        const WordWrapBehavior behavior;
        const size_t num_lines;
        const Range char_ranges[4];
    } kTestScenarios[] = {
        { IGNORE_LONG_WORDS, 3u,
            { Range(0, 4), Range(4, 11), Range(11, 14), Range::InvalidRange() } },
        { TRUNCATE_LONG_WORDS, 3u,
            { Range(0, 4), Range(4, 8), Range(11, 14), Range::InvalidRange() } },
        { WRAP_LONG_WORDS, 4u,
            { Range(0, 4), Range(4, 8), Range(8, 11), Range(11, 14) } },
        // TODO(mukai): implement ELIDE_LONG_WORDS. It's not used right now.
    };
    RenderTextHarfBuzz render_text;
    render_text.SetMultiline(true);
    render_text.SetText(ASCIIToUTF16("foo fooooo foo"));
    render_text.set_glyph_width_for_test(kGlyphSize);
    render_text.SetDisplayRect(Rect(0, 0, kGlyphSize * 4, 0));

    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestScenarios); ++i) {
        SCOPED_TRACE(base::StringPrintf(
            "kTestScenarios[%" PRIuS "] %d", i, kTestScenarios[i].behavior));
        render_text.SetWordWrapBehavior(kTestScenarios[i].behavior);
        render_text.Draw(&canvas);

        ASSERT_EQ(kTestScenarios[i].num_lines, render_text.lines().size());
        for (size_t j = 0; j < render_text.lines().size(); ++j) {
            SCOPED_TRACE(base::StringPrintf("%" PRIuS "-th line", j));
            EXPECT_EQ(kTestScenarios[i].char_ranges[j],
                render_text.lines()[j].segments[0].char_range);
            EXPECT_EQ(kTestScenarios[i].char_ranges[j].length() * kGlyphSize,
                render_text.lines()[j].size.width());
        }
    }
}

TEST_F(RenderTextTest, Multiline_LineBreakerBehavior)
{
    const int kGlyphSize = 5;
    const struct {
        const wchar_t* const text;
        const WordWrapBehavior behavior;
        const Range char_ranges[3];
    } kTestScenarios[] = {
        { L"a single run", IGNORE_LONG_WORDS,
            { Range(0, 2), Range(2, 9), Range(9, 12) } },
        // 3 words: "That's ", ""good". ", "aaa" and 7 runs: "That", "'", "s ",
        // """, "good", "". ", "aaa". They all mixed together.
        { L"That's \"good\". aaa", IGNORE_LONG_WORDS,
            { Range(0, 7), Range(7, 15), Range(15, 18) } },
        // Test "\"" should be put into a new line correctly.
        { L"a \"good\" one.", IGNORE_LONG_WORDS,
            { Range(0, 2), Range(2, 9), Range(9, 13) } },
        // Test for full-width space.
        { L"That's\x3000good.\x3000yyy", IGNORE_LONG_WORDS,
            { Range(0, 7), Range(7, 13), Range(13, 16) } },
        { L"a single run", TRUNCATE_LONG_WORDS,
            { Range(0, 2), Range(2, 6), Range(9, 12) } },
        { L"That's \"good\". aaa", TRUNCATE_LONG_WORDS,
            { Range(0, 4), Range(7, 11), Range(15, 18) } },
        { L"That's good. aaa", TRUNCATE_LONG_WORDS,
            { Range(0, 4), Range(7, 11), Range(13, 16) } },
        { L"a \"good\" one.", TRUNCATE_LONG_WORDS,
            { Range(0, 2), Range(2, 6), Range(9, 13) } },
        { L"asingleword", WRAP_LONG_WORDS,
            { Range(0, 4), Range(4, 8), Range(8, 11) } },
        { L"That's good", WRAP_LONG_WORDS,
            { Range(0, 4), Range(4, 7), Range(7, 11) } },
        { L"That's \"g\".", WRAP_LONG_WORDS,
            { Range(0, 4), Range(4, 7), Range(7, 11) } },
    };

    RenderTextHarfBuzz render_text;
    render_text.SetMultiline(true);
    render_text.set_glyph_width_for_test(kGlyphSize);
    render_text.SetDisplayRect(Rect(0, 0, kGlyphSize * 4, 0));

    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestScenarios); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(WideToUTF16(kTestScenarios[i].text));
        render_text.SetWordWrapBehavior(kTestScenarios[i].behavior);
        render_text.Draw(&canvas);

        ASSERT_EQ(3u, render_text.lines().size());
        for (size_t j = 0; j < render_text.lines().size(); ++j) {
            SCOPED_TRACE(base::StringPrintf("%" PRIuS "-th line", j));
            // Merge all the segments ranges in the same line.
            size_t segment_size = render_text.lines()[j].segments.size();
            Range line_range;
            if (segment_size > 0)
                line_range = Range(
                    render_text.lines()[j].segments[0].char_range.start(),
                    render_text.lines()[j].segments[segment_size - 1].char_range.end());
            EXPECT_EQ(kTestScenarios[i].char_ranges[j], line_range);
            EXPECT_EQ(kTestScenarios[i].char_ranges[j].length() * kGlyphSize,
                render_text.lines()[j].size.width());
        }
    }
}

// Test that Surrogate pairs or combining character sequences do not get
// separated by line breaking.
TEST_F(RenderTextTest, Multiline_SurrogatePairsOrCombiningChars)
{
    RenderTextHarfBuzz render_text;
    render_text.SetMultiline(true);
    render_text.SetWordWrapBehavior(WRAP_LONG_WORDS);

    // Below is 'MUSICAL SYMBOL G CLEF' (U+1D11E), which is represented in UTF-16
    // as two code units forming a surrogate pair: 0xD834 0xDD1E.
    const base::char16 kSurrogate[] = { 0xD834, 0xDD1E, 0 };
    const base::string16 text_surrogate(kSurrogate);
    const int kSurrogateWidth = GetStringWidth(kSurrogate, render_text.font_list());

    // Below is a Devanagari two-character combining sequence U+0921 U+093F. The
    // sequence forms a single display character and should not be separated.
    const base::char16 kCombiningChars[] = { 0x921, 0x93F, 0 };
    const base::string16 text_combining(kCombiningChars);
    const int kCombiningCharsWidth = GetStringWidth(kCombiningChars, render_text.font_list());

    const struct {
        const base::string16 text;
        const int display_width;
        const Range char_ranges[3];
    } kTestScenarios[] = {
        { text_surrogate + text_surrogate + text_surrogate,
            kSurrogateWidth / 2 * 3,
            { Range(0, 2), Range(2, 4), Range(4, 6) } },
        { text_surrogate + UTF8ToUTF16(" ") + kCombiningChars,
            std::min(kSurrogateWidth, kCombiningCharsWidth) / 2,
            { Range(0, 2), Range(2, 3), Range(3, 5) } },
    };

    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestScenarios); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(kTestScenarios[i].text);
        render_text.SetDisplayRect(Rect(0, 0, kTestScenarios[i].display_width, 0));
        render_text.Draw(&canvas);

        ASSERT_EQ(3u, render_text.lines().size());
        for (size_t j = 0; j < render_text.lines().size(); ++j) {
            SCOPED_TRACE(base::StringPrintf("%" PRIuS "-th line", j));
            // There is only one segment in each line.
            EXPECT_EQ(kTestScenarios[i].char_ranges[j],
                render_text.lines()[j].segments[0].char_range);
        }
    }
}

// Test that Zero width characters have the correct line breaking behavior.
TEST_F(RenderTextTest, Multiline_ZeroWidthChars)
{
    RenderTextHarfBuzz render_text;
    render_text.SetMultiline(true);
    render_text.SetWordWrapBehavior(WRAP_LONG_WORDS);

    const base::char16 kZeroWidthSpace = { 0x200B };
    const base::string16 text(UTF8ToUTF16("test") + kZeroWidthSpace + UTF8ToUTF16("\n") + kZeroWidthSpace + UTF8ToUTF16("test."));
    const int kTestWidth = GetStringWidth(UTF8ToUTF16("test"), render_text.font_list());
    const Range char_ranges[3] = { Range(0, 5), Range(6, 11), Range(11, 12) };

    Canvas canvas;
    render_text.SetText(text);
    render_text.SetDisplayRect(Rect(0, 0, kTestWidth, 0));
    render_text.Draw(&canvas);

    ASSERT_EQ(3u, render_text.lines().size());
    for (size_t j = 0; j < render_text.lines().size(); ++j) {
        SCOPED_TRACE(base::StringPrintf("%" PRIuS "-th line", j));
        int segment_size = render_text.lines()[j].segments.size();
        ASSERT_GT(segment_size, 0);
        Range line_range(
            render_text.lines()[j].segments[0].char_range.start(),
            render_text.lines()[j].segments[segment_size - 1].char_range.end());
        EXPECT_EQ(char_ranges[j], line_range);
    }
}

TEST_F(RenderTextTest, NewlineWithoutMultilineFlag)
{
    const wchar_t* kTestStrings[] = {
        L"abc\ndef",
        L"a \n b ",
        L"ab\n",
        L"a\n\nb",
        L"\nab",
        L"\n",
    };

    RenderTextHarfBuzz render_text;
    render_text.SetDisplayRect(Rect(200, 1000));
    Canvas canvas;

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(WideToUTF16(kTestStrings[i]));
        render_text.Draw(&canvas);

        EXPECT_EQ(1U, render_text.lines_.size());
    }
}

// Make sure the horizontal positions of runs in a line (left-to-right for
// LTR languages and right-to-left for RTL languages).
TEST_F(RenderTextTest, HarfBuzz_HorizontalPositions)
{
    const struct {
        const wchar_t* const text;
        const Range first_run_char_range;
        const Range second_run_char_range;
        bool is_rtl;
    } kTestStrings[] = {
        { L"abc\x3042\x3044\x3046\x3048\x304A", Range(0, 3), Range(3, 8), false },
        { L"\x062A\x0641\x0627\x062D"
          L"\x05EA\x05E4\x05D5\x05D6\x05D9\x05DA\x05DB\x05DD",
            Range(0, 4), Range(4, 12), true },
    };

    RenderTextHarfBuzz render_text;
    Canvas canvas;
    TestSkiaTextRenderer renderer(&canvas);

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        SCOPED_TRACE(base::StringPrintf("kTestStrings[%" PRIuS "]", i));
        render_text.SetText(WideToUTF16(kTestStrings[i].text));

        render_text.EnsureLayout();
        const internal::TextRunList* run_list = render_text.GetRunList();
        ASSERT_EQ(2U, run_list->runs().size());
        EXPECT_EQ(kTestStrings[i].first_run_char_range, run_list->runs()[0]->range);
        EXPECT_EQ(kTestStrings[i].second_run_char_range,
            run_list->runs()[1]->range);
        // If it's RTL, the visual order is reversed.
        if (kTestStrings[i].is_rtl) {
            EXPECT_EQ(1U, run_list->logical_to_visual(0));
            EXPECT_EQ(0U, run_list->logical_to_visual(1));
        } else {
            EXPECT_EQ(0U, run_list->logical_to_visual(0));
            EXPECT_EQ(1U, run_list->logical_to_visual(1));
        }

        render_text.DrawVisualText(&renderer);

        std::vector<TestSkiaTextRenderer::TextLog> text_log;
        renderer.GetTextLogAndReset(&text_log);

        EXPECT_EQ(2U, text_log.size());

        // Verifies the DrawText happens in the visual order and left-to-right.
        // If the text is RTL, the logically first run should be drawn at last.
        EXPECT_EQ(run_list->runs()[run_list->logical_to_visual(0)]->glyph_count,
            text_log[0].glyph_count);
        EXPECT_EQ(run_list->runs()[run_list->logical_to_visual(1)]->glyph_count,
            text_log[1].glyph_count);
        EXPECT_LT(text_log[0].origin.x(), text_log[1].origin.x());
    }
}

// Test TextRunHarfBuzz's cluster finding logic.
TEST_F(RenderTextTest, HarfBuzz_Clusters)
{
    struct {
        uint32_t glyph_to_char[4];
        Range chars[4];
        Range glyphs[4];
        bool is_rtl;
    } cases[] = {
        { // From string "A B C D" to glyphs "a b c d".
            { 0, 1, 2, 3 },
            { Range(0, 1), Range(1, 2), Range(2, 3), Range(3, 4) },
            { Range(0, 1), Range(1, 2), Range(2, 3), Range(3, 4) },
            false },
        { // From string "A B C D" to glyphs "d c b a".
            { 3, 2, 1, 0 },
            { Range(0, 1), Range(1, 2), Range(2, 3), Range(3, 4) },
            { Range(3, 4), Range(2, 3), Range(1, 2), Range(0, 1) },
            true },
        { // From string "A B C D" to glyphs "ab c c d".
            { 0, 2, 2, 3 },
            { Range(0, 2), Range(0, 2), Range(2, 3), Range(3, 4) },
            { Range(0, 1), Range(0, 1), Range(1, 3), Range(3, 4) },
            false },
        { // From string "A B C D" to glyphs "d c c ba".
            { 3, 2, 2, 0 },
            { Range(0, 2), Range(0, 2), Range(2, 3), Range(3, 4) },
            { Range(3, 4), Range(3, 4), Range(1, 3), Range(0, 1) },
            true },
    };

    internal::TextRunHarfBuzz run;
    run.range = Range(0, 4);
    run.glyph_count = 4;
    run.glyph_to_char.resize(4);

    for (size_t i = 0; i < arraysize(cases); ++i) {
        std::copy(cases[i].glyph_to_char, cases[i].glyph_to_char + 4,
            run.glyph_to_char.begin());
        run.is_rtl = cases[i].is_rtl;

        for (size_t j = 0; j < 4; ++j) {
            SCOPED_TRACE(base::StringPrintf("Case %" PRIuS ", char %" PRIuS, i, j));
            Range chars;
            Range glyphs;
            run.GetClusterAt(j, &chars, &glyphs);
            EXPECT_EQ(cases[i].chars[j], chars);
            EXPECT_EQ(cases[i].glyphs[j], glyphs);
            EXPECT_EQ(cases[i].glyphs[j], run.CharRangeToGlyphRange(chars));
        }
    }
}

// Ensure that graphemes with multiple code points do not get split.
TEST_F(RenderTextTest, HarfBuzz_SubglyphGraphemeCases)
{
    const wchar_t* cases[] = {
        // "A" with a combining umlaut, followed by a "B".
        L"A\x0308"
        L"B",
        // Devanagari biconsonantal conjunct "ki", followed by an "a".
        L"\x0915\x093f\x0905",
        // Thai consonant and vowel pair "cho chan" + "sara am", followed by Thai
        // digit 0.
        L"\x0e08\x0e33\x0E50",
    };

    RenderTextHarfBuzz render_text;

    for (size_t i = 0; i < arraysize(cases); ++i) {
        SCOPED_TRACE(base::StringPrintf("Case %" PRIuS, i));

        base::string16 text = WideToUTF16(cases[i]);
        render_text.SetText(text);
        render_text.EnsureLayout();
        internal::TextRunList* run_list = render_text.GetRunList();
        ASSERT_EQ(1U, run_list->size());
        internal::TextRunHarfBuzz* run = run_list->runs()[0];

        base::i18n::BreakIterator* iter = render_text.grapheme_iterator_.get();
        auto first_grapheme_bounds = run->GetGraphemeBounds(iter, 0);
        EXPECT_EQ(first_grapheme_bounds, run->GetGraphemeBounds(iter, 1));
        auto second_grapheme_bounds = run->GetGraphemeBounds(iter, 2);
        EXPECT_EQ(first_grapheme_bounds.end(), second_grapheme_bounds.start());
    }
}

// Test the partition of a multi-grapheme cluster into grapheme ranges.
TEST_F(RenderTextTest, HarfBuzz_SubglyphGraphemePartition)
{
    struct {
        uint32_t glyph_to_char[2];
        Range bounds[4];
        bool is_rtl;
    } cases[] = {
        { // From string "A B C D" to glyphs "a bcd".
            { 0, 1 },
            { Range(0, 10), Range(10, 13), Range(13, 17), Range(17, 20) },
            false },
        { // From string "A B C D" to glyphs "ab cd".
            { 0, 2 },
            { Range(0, 5), Range(5, 10), Range(10, 15), Range(15, 20) },
            false },
        { // From string "A B C D" to glyphs "dcb a".
            { 1, 0 },
            { Range(10, 20), Range(7, 10), Range(3, 7), Range(0, 3) },
            true },
        { // From string "A B C D" to glyphs "dc ba".
            { 2, 0 },
            { Range(15, 20), Range(10, 15), Range(5, 10), Range(0, 5) },
            true },
    };

    internal::TextRunHarfBuzz run;
    run.range = Range(0, 4);
    run.glyph_count = 2;
    run.glyph_to_char.resize(2);
    run.positions.reset(new SkPoint[4]);
    run.width = 20;

    const base::string16 kString = ASCIIToUTF16("abcd");
    scoped_ptr<base::i18n::BreakIterator> iter(new base::i18n::BreakIterator(
        kString, base::i18n::BreakIterator::BREAK_CHARACTER));
    ASSERT_TRUE(iter->Init());

    for (size_t i = 0; i < arraysize(cases); ++i) {
        std::copy(cases[i].glyph_to_char, cases[i].glyph_to_char + 2,
            run.glyph_to_char.begin());
        run.is_rtl = cases[i].is_rtl;
        for (int j = 0; j < 2; ++j)
            run.positions[j].set(j * 10, 0);

        for (size_t j = 0; j < 4; ++j) {
            SCOPED_TRACE(base::StringPrintf("Case %" PRIuS ", char %" PRIuS, i, j));
            EXPECT_EQ(cases[i].bounds[j],
                run.GetGraphemeBounds(iter.get(), j).Round());
        }
    }
}

TEST_F(RenderTextTest, HarfBuzz_RunDirection)
{
    RenderTextHarfBuzz render_text;
    const base::string16 mixed = WideToUTF16(
        L"\x05D0\x05D1"
        L"1234"
        L"\x05D2\x05D3"
        L"abc");
    render_text.SetText(mixed);

    // Get the run list for both display directions.
    render_text.SetDirectionalityMode(DIRECTIONALITY_FORCE_LTR);
    render_text.EnsureLayout();
    internal::TextRunList* run_list = render_text.GetRunList();
    ASSERT_EQ(4U, run_list->size());
    EXPECT_TRUE(run_list->runs()[0]->is_rtl);
    EXPECT_FALSE(run_list->runs()[1]->is_rtl);
    EXPECT_TRUE(run_list->runs()[2]->is_rtl);
    EXPECT_FALSE(run_list->runs()[3]->is_rtl);

    // The Latin letters should appear to the right of the other runs.
    EXPECT_EQ(2U, run_list->logical_to_visual(0));
    EXPECT_EQ(1U, run_list->logical_to_visual(1));
    EXPECT_EQ(0U, run_list->logical_to_visual(2));
    EXPECT_EQ(3U, run_list->logical_to_visual(3));

    render_text.SetDirectionalityMode(DIRECTIONALITY_FORCE_RTL);
    render_text.EnsureLayout();
    run_list = render_text.GetRunList();
    ASSERT_EQ(4U, run_list->size());
    EXPECT_TRUE(run_list->runs()[0]->is_rtl);
    EXPECT_FALSE(run_list->runs()[1]->is_rtl);
    EXPECT_TRUE(run_list->runs()[2]->is_rtl);
    EXPECT_FALSE(run_list->runs()[3]->is_rtl);

    // The Latin letters should appear to the left of the other runs.
    EXPECT_EQ(3U, run_list->logical_to_visual(0));
    EXPECT_EQ(2U, run_list->logical_to_visual(1));
    EXPECT_EQ(1U, run_list->logical_to_visual(2));
    EXPECT_EQ(0U, run_list->logical_to_visual(3));
}

TEST_F(RenderTextTest, HarfBuzz_BreakRunsByUnicodeBlocks)
{
    RenderTextHarfBuzz render_text;

    // The '\x25B6' "play character" should break runs. http://crbug.com/278913
    render_text.SetText(WideToUTF16(L"x\x25B6y"));
    render_text.EnsureLayout();
    internal::TextRunList* run_list = render_text.GetRunList();
    ASSERT_EQ(3U, run_list->size());
    EXPECT_EQ(Range(0, 1), run_list->runs()[0]->range);
    EXPECT_EQ(Range(1, 2), run_list->runs()[1]->range);
    EXPECT_EQ(Range(2, 3), run_list->runs()[2]->range);

    render_text.SetText(WideToUTF16(L"x \x25B6 y"));
    render_text.EnsureLayout();
    run_list = render_text.GetRunList();
    ASSERT_EQ(4U, run_list->size());
    EXPECT_EQ(Range(0, 2), run_list->runs()[0]->range);
    EXPECT_EQ(Range(2, 3), run_list->runs()[1]->range);
    EXPECT_EQ(Range(3, 4), run_list->runs()[2]->range);
    EXPECT_EQ(Range(4, 5), run_list->runs()[3]->range);
}

TEST_F(RenderTextTest, HarfBuzz_BreakRunsByEmoji)
{
    RenderTextHarfBuzz render_text;

    // \xF0\x9F\x98\x81 (U+1F601) is smile icon emoji. \xE2\x9C\xA8 (U+2728) is
    // a sparkle icon. Both can be drawn with color emoji fonts, so runs should be
    // separated. See crbug.com/448909
    render_text.SetText(UTF8ToUTF16("x\xF0\x9F\x98\x81y\xE2\x9C\xA8"));
    render_text.EnsureLayout();
    internal::TextRunList* run_list = render_text.GetRunList();
    ASSERT_EQ(4U, run_list->size());
    EXPECT_EQ(Range(0, 1), run_list->runs()[0]->range);
    // The length is 2 since U+1F601 is represented as a surrogate pair in UTF16.
    EXPECT_EQ(Range(1, 3), run_list->runs()[1]->range);
    EXPECT_EQ(Range(3, 4), run_list->runs()[2]->range);
    EXPECT_EQ(Range(4, 5), run_list->runs()[3]->range);
}

TEST_F(RenderTextTest, HarfBuzz_BreakRunsByAscii)
{
    RenderTextHarfBuzz render_text;

    // \xF0\x9F\x90\xB1 (U+1F431) is a cat face. It should be put into a separate
    // run from the ASCII period character.
    render_text.SetText(UTF8ToUTF16("\xF0\x9F\x90\xB1."));
    render_text.EnsureLayout();
    internal::TextRunList* run_list = render_text.GetRunList();
    ASSERT_EQ(2U, run_list->size());
    // U+1F431 is represented as a surrogate pair in UTF16.
    EXPECT_EQ(Range(0, 2), run_list->runs()[0]->range);
    EXPECT_EQ(Range(2, 3), run_list->runs()[1]->range);
}

TEST_F(RenderTextTest, GlyphBounds)
{
    const wchar_t* kTestStrings[] = {
        L"asdf 1234 qwer", L"\x0647\x0654", L"\x0645\x0631\x062D\x0628\x0627"
    };
    scoped_ptr<RenderText> render_text(new RenderTextHarfBuzz);

    for (size_t i = 0; i < arraysize(kTestStrings); ++i) {
        render_text->SetText(WideToUTF16(kTestStrings[i]));
        render_text->EnsureLayout();

        for (size_t j = 0; j < render_text->text().length(); ++j)
            EXPECT_FALSE(render_text->GetGlyphBounds(j).is_empty());
    }
}

// Ensure that shaping with a non-existent font does not cause a crash.
TEST_F(RenderTextTest, HarfBuzz_NonExistentFont)
{
    RenderTextHarfBuzz render_text;
    render_text.SetText(ASCIIToUTF16("test"));
    render_text.EnsureLayout();
    internal::TextRunList* run_list = render_text.GetRunList();
    ASSERT_EQ(1U, run_list->size());
    internal::TextRunHarfBuzz* run = run_list->runs()[0];
    render_text.ShapeRunWithFont(render_text.text(),
        Font("TheFontThatDoesntExist", 13),
        FontRenderParams(), run);
}

// Ensure an empty run returns sane values to queries.
TEST_F(RenderTextTest, HarfBuzz_EmptyRun)
{
    internal::TextRunHarfBuzz run;
    const base::string16 kString = ASCIIToUTF16("abcdefgh");
    scoped_ptr<base::i18n::BreakIterator> iter(new base::i18n::BreakIterator(
        kString, base::i18n::BreakIterator::BREAK_CHARACTER));
    ASSERT_TRUE(iter->Init());

    run.range = Range(3, 8);
    run.glyph_count = 0;
    EXPECT_EQ(Range(0, 0), run.CharRangeToGlyphRange(Range(4, 5)));
    EXPECT_EQ(Range(0, 0), run.GetGraphemeBounds(iter.get(), 4).Round());
    Range chars;
    Range glyphs;
    run.GetClusterAt(4, &chars, &glyphs);
    EXPECT_EQ(Range(3, 8), chars);
    EXPECT_EQ(Range(0, 0), glyphs);
}

// Ensure the line breaker doesn't compute the word's width bigger than the
// actual size. See http://crbug.com/470073
TEST_F(RenderTextTest, HarfBuzz_WordWidthWithDiacritics)
{
    RenderTextHarfBuzz render_text;
    const base::string16 kWord = WideToUTF16(L"\u0906\u092A\u0915\u0947 ");
    render_text.SetText(kWord);
    const gfx::SizeF text_size = render_text.GetStringSizeF();

    render_text.SetText(kWord + kWord);
    render_text.SetMultiline(true);
    EXPECT_EQ(text_size.width() * 2, render_text.GetStringSizeF().width());
    EXPECT_EQ(text_size.height(), render_text.GetStringSizeF().height());
    render_text.SetDisplayRect(gfx::Rect(0, 0, std::ceil(text_size.width()), 0));
    EXPECT_NEAR(text_size.width(), render_text.GetStringSizeF().width(), 1.0f);
    EXPECT_EQ(text_size.height() * 2, render_text.GetStringSizeF().height());
}

// Ensure a string fits in a display rect with a width equal to the string's.
TEST_F(RenderTextTest, StringFitsOwnWidth)
{
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    const base::string16 kString = ASCIIToUTF16("www.example.com");

    render_text->SetText(kString);
    render_text->ApplyStyle(BOLD, true, Range(0, 3));
    render_text->SetElideBehavior(ELIDE_TAIL);

    render_text->SetDisplayRect(Rect(0, 0, 500, 100));
    EXPECT_EQ(kString, render_text->GetDisplayText());
    render_text->SetDisplayRect(Rect(0, 0, render_text->GetContentWidth(), 100));
    EXPECT_EQ(kString, render_text->GetDisplayText());
}

// TODO(derat): Figure out why this fails on Windows: http://crbug.com/427184
#if !defined(OS_WIN)
// Ensure that RenderText examines all of the fonts in its FontList before
// falling back to other fonts.
TEST_F(RenderTextTest, HarfBuzz_FontListFallback)
{
    // Double-check that the requested fonts are present.
    FontList font_list("Arial, Symbol, 12px");
    const std::vector<Font>& fonts = font_list.GetFonts();
    ASSERT_EQ(2u, fonts.size());
    ASSERT_EQ("arial",
        base::ToLowerASCII(fonts[0].GetActualFontNameForTesting()));
    ASSERT_EQ("symbol",
        base::ToLowerASCII(fonts[1].GetActualFontNameForTesting()));

    // "⊕" (CIRCLED PLUS) should be rendered with Symbol rather than falling back
    // to some other font that's present on the system.
    RenderTextHarfBuzz render_text;
    render_text.SetFontList(font_list);
    render_text.SetText(UTF8ToUTF16("\xE2\x8A\x95"));
    const std::vector<RenderText::FontSpan> spans = render_text.GetFontSpansForTesting();
    ASSERT_EQ(static_cast<size_t>(1), spans.size());
    EXPECT_EQ("Symbol", spans[0].first.GetFontName());
}
#endif // !defined(OS_WIN)

// Ensure that the fallback fonts of the Uniscribe font are tried for shaping.
#if defined(OS_WIN)
TEST_F(RenderTextTest, HarfBuzz_UniscribeFallback)
{
    RenderTextHarfBuzz render_text;
    PlatformFontWin* font_win = new PlatformFontWin("Meiryo", 12);
    // Japanese name for Meiryo. This name won't be found in the system's linked
    // fonts, forcing RTHB to try the Uniscribe font and its fallbacks.
    font_win->font_ref_->font_name_ = WideToUTF8(L"\x30e1\x30a4\x30ea\x30aa");
    FontList font_list((Font(font_win)));

    render_text.SetFontList(font_list);
    // Korean character "han".
    render_text.SetText(WideToUTF16(L"\xd55c"));
    render_text.EnsureLayout();
    internal::TextRunList* run_list = render_text.GetRunList();
    ASSERT_EQ(1U, run_list->size());
    EXPECT_EQ(0U, run_list->runs()[0]->CountMissingGlyphs());
}
#endif // defined(OS_WIN)

// Ensure that the fallback fonts offered by gfx::GetFallbackFonts() are
// tried. Note this test assumes the font "Arial" doesn't provide a unicode
// glyph for a particular character, and that there exists a system fallback
// font which does.
// TODO(msw): Fallback doesn't find a glyph on Linux.
#if !defined(OS_LINUX)
TEST_F(RenderTextTest, HarfBuzz_UnicodeFallback)
{
    RenderTextHarfBuzz render_text;
    render_text.SetFontList(FontList("Arial, 12px"));

    // Korean character "han".
    render_text.SetText(WideToUTF16(L"\xd55c"));
    render_text.EnsureLayout();
    internal::TextRunList* run_list = render_text.GetRunList();
    ASSERT_EQ(1U, run_list->size());
    EXPECT_EQ(0U, run_list->runs()[0]->CountMissingGlyphs());
}
#endif // !defined(OS_LINUX)

// Ensure that the width reported by RenderText is sufficient for drawing. Draws
// to a canvas and checks if any pixel beyond the bounding rectangle is colored.
TEST_F(RenderTextTest, TextDoesntClip)
{
    const wchar_t* kTestStrings[] = {
        L"            ",
        // TODO(dschuyler): Underscores draw outside GetStringSize;
        // crbug.com/459812.  This appears to be a preexisting issue that wasn't
        // revealed by the prior unit tests.
        // L"TEST_______",
        L"TEST some stuff",
        L"WWWWWWWWWW",
        L"gAXAXAXAXAXAXA",
        // TODO(dschuyler): A-Ring draws outside GetStringSize; crbug.com/459812.
        // L"g\x00C5X\x00C5X\x00C5X\x00C5X\x00C5X\x00C5X\x00C5",
        L"\x0647\x0654\x0647\x0654\x0647\x0654\x0647\x0654\x0645\x0631\x062D"
        L"\x0628\x0627"
    };
    const Size kCanvasSize(300, 50);
    const int kTestSize = 10;

    skia::RefPtr<SkSurface> surface = skia::AdoptRef(
        SkSurface::NewRasterN32Premul(kCanvasSize.width(), kCanvasSize.height()));
    Canvas canvas(skia::SharePtr(surface->getCanvas()), 1.0f);
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetHorizontalAlignment(ALIGN_LEFT);
    render_text->SetColor(SK_ColorBLACK);

    for (auto string : kTestStrings) {
        surface->getCanvas()->clear(SK_ColorWHITE);
        render_text->SetText(WideToUTF16(string));
        const Size string_size = render_text->GetStringSize();
        render_text->ApplyBaselineStyle(SUPERSCRIPT, Range(1, 2));
        render_text->ApplyBaselineStyle(SUPERIOR, Range(3, 4));
        render_text->ApplyBaselineStyle(INFERIOR, Range(5, 6));
        render_text->ApplyBaselineStyle(SUBSCRIPT, Range(7, 8));
        render_text->SetStyle(BOLD, true);
        render_text->SetDisplayRect(
            Rect(kTestSize, kTestSize, string_size.width(), string_size.height()));
        // Allow the RenderText to paint outside of its display rect.
        render_text->set_clip_to_display_rect(false);
        ASSERT_LE(string_size.width() + kTestSize * 2, kCanvasSize.width());

        render_text->Draw(&canvas);
        ASSERT_LT(string_size.width() + kTestSize, kCanvasSize.width());
        const uint32_t* buffer = static_cast<const uint32_t*>(surface->peekPixels(nullptr, nullptr));
        ASSERT_NE(nullptr, buffer);
        TestRectangleBuffer rect_buffer(string, buffer, kCanvasSize.width(),
            kCanvasSize.height());
        {
#if !defined(OS_CHROMEOS)
            // TODO(dschuyler): On ChromeOS text draws above the GetStringSize rect.
            SCOPED_TRACE("TextDoesntClip Top Side");
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, 0, 0, kCanvasSize.width(),
                kTestSize);
#endif
        }
        {
            SCOPED_TRACE("TextDoesntClip Bottom Side");
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, 0,
                kTestSize + string_size.height(),
                kCanvasSize.width(), kTestSize);
        }
        {
            SCOPED_TRACE("TextDoesntClip Left Side");
#if defined(OS_WIN)
            // TODO(dschuyler): On Windows XP the Unicode test draws to the left edge
            // as if it is ignoring the SetDisplayRect shift by kTestSize.  This
            // appears to be a preexisting issue that wasn't revealed by the prior
            // unit tests.
#elif defined(OS_MACOSX) || defined(OS_CHROMEOS)
            // TODO(dschuyler): On Windows (non-XP), Chrome OS and Mac smoothing draws
            // left of text.  This appears to be a preexisting issue that wasn't
            // revealed by the prior unit tests.
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, 0, kTestSize, kTestSize - 1,
                string_size.height());
#else
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, 0, kTestSize, kTestSize,
                string_size.height());
#endif
        }
        {
            SCOPED_TRACE("TextDoesntClip Right Side");
#if !defined(OS_MACOSX)
            // TODO(dschuyler): On Mac text draws to right of GetStringSize.  This
            // appears to be a preexisting issue that wasn't revealed by the prior
            // unit tests.
            rect_buffer.EnsureSolidRect(SK_ColorWHITE,
                kTestSize + string_size.width(), kTestSize,
                kTestSize, string_size.height());
#endif
        }
    }
}

// Ensure that the text will clip to the display rect. Draws to a canvas and
// checks whether any pixel beyond the bounding rectangle is colored.
TEST_F(RenderTextTest, TextDoesClip)
{
    const wchar_t* kTestStrings[] = { L"TEST", L"W", L"WWWW", L"gAXAXWWWW" };
    const Size kCanvasSize(300, 50);
    const int kTestSize = 10;

    skia::RefPtr<SkSurface> surface = skia::AdoptRef(
        SkSurface::NewRasterN32Premul(kCanvasSize.width(), kCanvasSize.height()));
    Canvas canvas(skia::SharePtr(surface->getCanvas()), 1.0f);
    scoped_ptr<RenderText> render_text(RenderText::CreateInstance());
    render_text->SetHorizontalAlignment(ALIGN_LEFT);
    render_text->SetColor(SK_ColorBLACK);

    for (auto string : kTestStrings) {
        surface->getCanvas()->clear(SK_ColorWHITE);
        render_text->SetText(WideToUTF16(string));
        const Size string_size = render_text->GetStringSize();
        int fake_width = string_size.width() / 2;
        int fake_height = string_size.height() / 2;
        render_text->SetDisplayRect(
            Rect(kTestSize, kTestSize, fake_width, fake_height));
        render_text->set_clip_to_display_rect(true);
        render_text->Draw(&canvas);
        ASSERT_LT(string_size.width() + kTestSize, kCanvasSize.width());
        const uint32_t* buffer = static_cast<const uint32_t*>(surface->peekPixels(nullptr, nullptr));
        ASSERT_NE(nullptr, buffer);
        TestRectangleBuffer rect_buffer(string, buffer, kCanvasSize.width(),
            kCanvasSize.height());
        {
            SCOPED_TRACE("TextDoesClip Top Side");
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, 0, 0, kCanvasSize.width(),
                kTestSize);
        }

        {
            SCOPED_TRACE("TextDoesClip Bottom Side");
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, 0, kTestSize + fake_height,
                kCanvasSize.width(), kTestSize);
        }
        {
            SCOPED_TRACE("TextDoesClip Left Side");
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, 0, kTestSize, kTestSize,
                fake_height);
        }
        {
            SCOPED_TRACE("TextDoesClip Right Side");
            rect_buffer.EnsureSolidRect(SK_ColorWHITE, kTestSize + fake_width,
                kTestSize, kTestSize, fake_height);
        }
    }
}

#if defined(OS_MACOSX)
TEST_F(RenderTextTest, Mac_ElidedText)
{
    RenderTextMac render_text;
    base::string16 text(ASCIIToUTF16("This is an example."));
    render_text.SetText(text);
    gfx::Size string_size = render_text.GetStringSize();
    render_text.SetDisplayRect(gfx::Rect(string_size));
    render_text.EnsureLayout();
    // NOTE: Character and glyph counts are only comparable for simple text.
    EXPECT_EQ(text.size(),
        static_cast<size_t>(CTLineGetGlyphCount(render_text.line_)));

    render_text.SetElideBehavior(ELIDE_TAIL);
    string_size.set_width(string_size.width() / 2);
    render_text.SetDisplayRect(gfx::Rect(string_size));
    render_text.EnsureLayout();
    CFIndex glyph_count = CTLineGetGlyphCount(render_text.line_);
    EXPECT_GT(text.size(), static_cast<size_t>(glyph_count));
    EXPECT_NE(0, glyph_count);
}
#endif

// Ensure color changes are picked up by the RenderText implementation.
TEST_F(RenderTextTest, ColorChange)
{
    RenderTextHarfBuzz render_text_harfbuzz;
#if defined(OS_MACOSX)
    RenderTextMac render_text_mac;
#endif

    RenderText* backend[] = {
        &render_text_harfbuzz,
#if defined(OS_MACOSX)
        &render_text_mac,
#endif
    };

    Canvas canvas;
    TestSkiaTextRenderer renderer(&canvas);

    for (size_t i = 0; i < arraysize(backend); ++i) {
        SCOPED_TRACE(testing::Message() << "backend: " << i);
        test::RenderTextTestApi test_api(backend[i]);
        backend[i]->SetText(ASCIIToUTF16("x"));
        test_api.DrawVisualText(&renderer);

        std::vector<TestSkiaTextRenderer::TextLog> text_log;

        renderer.GetTextLogAndReset(&text_log);
        EXPECT_EQ(1u, text_log.size());
        EXPECT_EQ(SK_ColorBLACK, text_log[0].color);

        backend[i]->SetColor(SK_ColorRED);
        test_api.DrawVisualText(&renderer);

        renderer.GetTextLogAndReset(&text_log);
        EXPECT_EQ(1u, text_log.size());
        EXPECT_EQ(SK_ColorRED, text_log[0].color);
    }
}

} // namespace gfx
