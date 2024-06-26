// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/render_text_mac.h"

#import <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreText/CoreText.h>

#include <algorithm>
#include <cmath>
#include <utility>

#include "base/mac/foundation_util.h"
#include "base/mac/mac_util.h"
#include "base/mac/scoped_cftyperef.h"
#include "base/macros.h"
#include "base/strings/sys_string_conversions.h"
#include "skia/ext/skia_utils_mac.h"
#include "third_party/skia/include/ports/SkTypeface_mac.h"

namespace {

// This function makes a copy of |font| with the given symbolic traits. On OSX
// 10.11, CTFontCreateCopyWithSymbolicTraits has the right behavior but
// CTFontCreateWithFontDescriptor does not. The opposite holds true for OSX
// 10.10.
base::ScopedCFTypeRef<CTFontRef> CopyFontWithSymbolicTraits(CTFontRef font,
                                                            int sym_traits) {
  if (base::mac::IsOSElCapitanOrLater()) {
    return base::ScopedCFTypeRef<CTFontRef>(CTFontCreateCopyWithSymbolicTraits(
        font, 0, nullptr, sym_traits, sym_traits));
  }

  base::ScopedCFTypeRef<CTFontDescriptorRef> orig_desc(
      CTFontCopyFontDescriptor(font));
  base::ScopedCFTypeRef<CFDictionaryRef> orig_attributes(
      CTFontDescriptorCopyAttributes(orig_desc));
  // Make a mutable copy of orig_attributes.
  base::ScopedCFTypeRef<CFMutableDictionaryRef> attributes(
      CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, orig_attributes));

  base::ScopedCFTypeRef<CFMutableDictionaryRef> traits(
      CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                &kCFTypeDictionaryKeyCallBacks,
                                &kCFTypeDictionaryValueCallBacks));
  base::ScopedCFTypeRef<CFNumberRef> n(
      CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &sym_traits));
  CFDictionarySetValue(traits, kCTFontSymbolicTrait, n.release());
  CFDictionarySetValue(attributes, kCTFontTraitsAttribute, traits.release());

  base::ScopedCFTypeRef<CFStringRef> family_name(CTFontCopyFamilyName(font));
  CFDictionarySetValue(attributes, kCTFontNameAttribute, family_name.release());

  base::ScopedCFTypeRef<CTFontDescriptorRef> desc(
      CTFontDescriptorCreateWithAttributes(attributes));
  return base::ScopedCFTypeRef<CTFontRef>(
      CTFontCreateWithFontDescriptor(desc, 0.0, nullptr));
}

}  // namespace

namespace gfx {

namespace internal {

skia::RefPtr<SkTypeface> CreateSkiaTypeface(const gfx::Font& font, int style) {
  gfx::Font font_with_style = font.Derive(0, style);
  if (!font_with_style.GetNativeFont())
    return nullptr;
  return skia::AdoptRef(SkCreateTypefaceFromCTFont(
      static_cast<CTFontRef>(font_with_style.GetNativeFont())));
}

}  // namespace internal

RenderTextMac::RenderTextMac() : common_baseline_(0), runs_valid_(false) {}

RenderTextMac::~RenderTextMac() {}

scoped_ptr<RenderText> RenderTextMac::CreateInstanceOfSameType() const {
  return make_scoped_ptr(new RenderTextMac);
}

bool RenderTextMac::MultilineSupported() const {
  return false;
}

const base::string16& RenderTextMac::GetDisplayText() {
  return text_elided() ? display_text() : layout_text();
}

Size RenderTextMac::GetStringSize() {
  SizeF size_f = GetStringSizeF();
  return Size(std::ceil(size_f.width()), size_f.height());
}

SizeF RenderTextMac::GetStringSizeF() {
  EnsureLayout();
  return string_size_;
}

SelectionModel RenderTextMac::FindCursorPosition(const Point& point) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return SelectionModel();
}

std::vector<RenderText::FontSpan> RenderTextMac::GetFontSpansForTesting() {
  EnsureLayout();
  if (!runs_valid_)
    ComputeRuns();

  std::vector<RenderText::FontSpan> spans;
  for (size_t i = 0; i < runs_.size(); ++i) {
    const CFRange cf_range = CTRunGetStringRange(runs_[i].ct_run);
    const Range range(cf_range.location, cf_range.location + cf_range.length);
    spans.push_back(RenderText::FontSpan(runs_[i].font, range));
  }

  return spans;
}

int RenderTextMac::GetDisplayTextBaseline() {
  EnsureLayout();
  return common_baseline_;
}

SelectionModel RenderTextMac::AdjacentCharSelectionModel(
    const SelectionModel& selection,
    VisualCursorDirection direction) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return SelectionModel();
}

SelectionModel RenderTextMac::AdjacentWordSelectionModel(
    const SelectionModel& selection,
    VisualCursorDirection direction) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return SelectionModel();
}

Range RenderTextMac::GetGlyphBounds(size_t index) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return Range();
}

std::vector<Rect> RenderTextMac::GetSubstringBounds(const Range& range) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return std::vector<Rect>();
}

size_t RenderTextMac::TextIndexToDisplayIndex(size_t index) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return index;
}

size_t RenderTextMac::DisplayIndexToTextIndex(size_t index) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return index;
}

bool RenderTextMac::IsValidCursorIndex(size_t index) {
  // TODO(asvitkine): Implement this. http://crbug.com/131618
  return IsValidLogicalIndex(index);
}

void RenderTextMac::OnLayoutTextAttributeChanged(bool text_changed) {
  DCHECK(!multiline()) << "RenderTextMac does not support multi line";
  if (text_changed) {
    if (elide_behavior() != NO_ELIDE && elide_behavior() != FADE_TAIL &&
        !layout_text().empty()) {
      UpdateDisplayText(std::ceil(GetLayoutTextWidth()));
    } else {
      UpdateDisplayText(0);
    }
  }
  InvalidateStyle();
}

void RenderTextMac::OnDisplayTextAttributeChanged() {
  OnLayoutTextAttributeChanged(true);
}

void RenderTextMac::OnTextColorChanged() {
  InvalidateStyle();
}

void RenderTextMac::EnsureLayout() {
  if (line_.get())
    return;
  runs_.clear();
  runs_valid_ = false;

  line_ = EnsureLayoutInternal(GetDisplayText(), &attributes_);
  string_size_ = GetCTLineSize(line_.get(), &common_baseline_);
}

void RenderTextMac::DrawVisualText(internal::SkiaTextRenderer* renderer) {
  DCHECK(line_);
  if (!runs_valid_)
    ComputeRuns();

  ApplyFadeEffects(renderer);
  ApplyTextShadows(renderer);

  for (size_t i = 0; i < runs_.size(); ++i) {
    const TextRun& run = runs_[i];
    renderer->SetForegroundColor(run.foreground);

    CTFontRef ct_font = static_cast<CTFontRef>(run.font.GetNativeFont());
    renderer->SetTextSize(CTFontGetSize(ct_font));

    int font_style = Font::NORMAL;
    CTFontSymbolicTraits traits = CTFontGetSymbolicTraits(ct_font);
    if (traits & kCTFontBoldTrait)
      font_style |= Font::BOLD;
    if (traits & kCTFontItalicTrait)
      font_style |= Font::ITALIC;
    renderer->SetFontWithStyle(run.font, font_style);

    renderer->DrawPosText(&run.glyph_positions[0], &run.glyphs[0],
                          run.glyphs.size());
    renderer->DrawDecorations(run.origin.x(), run.origin.y(), run.width,
                              run.underline, run.strike, run.diagonal_strike);
  }

  renderer->EndDiagonalStrike();
}

RenderTextMac::TextRun::TextRun()
    : ct_run(NULL),
      origin(SkPoint::Make(0, 0)),
      width(0),
      foreground(SK_ColorBLACK),
      underline(false),
      strike(false),
      diagonal_strike(false) {}

RenderTextMac::TextRun::~TextRun() {}

float RenderTextMac::GetLayoutTextWidth() {
  base::ScopedCFTypeRef<CFMutableArrayRef> attributes_owner;
  base::ScopedCFTypeRef<CTLineRef> line(
      EnsureLayoutInternal(layout_text(), &attributes_owner));
  SkScalar baseline;
  return GetCTLineSize(line.get(), &baseline).width();
}

gfx::SizeF RenderTextMac::GetCTLineSize(CTLineRef line, SkScalar* baseline) {
  CGFloat ascent = 0;
  CGFloat descent = 0;
  CGFloat leading = 0;
  // TODO(asvitkine): Consider using CTLineGetBoundsWithOptions() on 10.8+.
  double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
  // Ensure ascent and descent are not smaller than ones of the font list.
  // Keep them tall enough to draw often-used characters.
  // For example, if a text field contains a Japanese character, which is
  // smaller than Latin ones, and then later a Latin one is inserted, this
  // ensures that the text baseline does not shift.
  CGFloat font_list_height = font_list().GetHeight();
  CGFloat font_list_baseline = font_list().GetBaseline();
  ascent = std::max(ascent, font_list_baseline);
  descent = std::max(descent, font_list_height - font_list_baseline);
  *baseline = ascent;
  return SizeF(width, std::max(ascent + descent + leading,
                               static_cast<CGFloat>(min_line_height())));
}

base::ScopedCFTypeRef<CTLineRef> RenderTextMac::EnsureLayoutInternal(
    const base::string16& text,
    base::ScopedCFTypeRef<CFMutableArrayRef>* attributes_owner) {
  CTFontRef ct_font =
      base::mac::NSToCFCast(font_list().GetPrimaryFont().GetNativeFont());

  const void* keys[] = {kCTFontAttributeName};
  const void* values[] = {ct_font};
  base::ScopedCFTypeRef<CFDictionaryRef> attributes(
      CFDictionaryCreate(NULL, keys, values, arraysize(keys), NULL,
                         &kCFTypeDictionaryValueCallBacks));

  base::ScopedCFTypeRef<CFStringRef> cf_text(base::SysUTF16ToCFStringRef(text));
  base::ScopedCFTypeRef<CFAttributedStringRef> attr_text(
      CFAttributedStringCreate(NULL, cf_text, attributes));
  base::ScopedCFTypeRef<CFMutableAttributedStringRef> attr_text_mutable(
      CFAttributedStringCreateMutableCopy(NULL, 0, attr_text));

  // TODO(asvitkine|msw): Respect GetTextDirection(), which may not match the
  // natural text direction. See kCTTypesetterOptionForcedEmbeddingLevel, etc.

  *attributes_owner = ApplyStyles(text, attr_text_mutable, ct_font);
  return base::ScopedCFTypeRef<CTLineRef>(
      CTLineCreateWithAttributedString(attr_text_mutable));
}

base::ScopedCFTypeRef<CFMutableArrayRef> RenderTextMac::ApplyStyles(
    const base::string16& text,
    CFMutableAttributedStringRef attr_string,
    CTFontRef font) {
  // Temporarily apply composition underlines and selection colors.
  ApplyCompositionAndSelectionStyles();

  // Note: CFAttributedStringSetAttribute() does not appear to retain the values
  // passed in, as can be verified via CFGetRetainCount(). To ensure the
  // attribute objects do not leak, they are saved to |attributes_|.
  // Clear the attributes storage.
  base::ScopedCFTypeRef<CFMutableArrayRef> attributes(
      CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks));

  // https://developer.apple.com/library/mac/#documentation/Carbon/Reference/CoreText_StringAttributes_Ref/Reference/reference.html
  internal::StyleIterator style(colors(), baselines(), styles());
  const size_t layout_text_length = CFAttributedStringGetLength(attr_string);
  for (size_t i = 0, end = 0; i < layout_text_length; i = end) {
    end = TextIndexToGivenTextIndex(text, style.GetRange().end());
    const CFRange range = CFRangeMake(i, end - i);
    base::ScopedCFTypeRef<CGColorRef> foreground(
        skia::CGColorCreateFromSkColor(style.color()));
    CFAttributedStringSetAttribute(attr_string, range,
                                   kCTForegroundColorAttributeName, foreground);
    CFArrayAppendValue(attributes, foreground);

    if (style.style(UNDERLINE)) {
      CTUnderlineStyle value = kCTUnderlineStyleSingle;
      base::ScopedCFTypeRef<CFNumberRef> underline_value(
          CFNumberCreate(NULL, kCFNumberSInt32Type, &value));
      CFAttributedStringSetAttribute(
          attr_string, range, kCTUnderlineStyleAttributeName, underline_value);
      CFArrayAppendValue(attributes, underline_value);
    }

    const int traits = (style.style(BOLD) ? kCTFontBoldTrait : 0) |
                       (style.style(ITALIC) ? kCTFontItalicTrait : 0);
    if (traits != 0) {
      base::ScopedCFTypeRef<CTFontRef> styled_font =
          CopyFontWithSymbolicTraits(font, traits);
      // TODO(asvitkine): Handle |styled_font| == NULL case better.
      if (styled_font) {
        CFAttributedStringSetAttribute(attr_string, range, kCTFontAttributeName,
                                       styled_font);
        CFArrayAppendValue(attributes, styled_font);
      }
    }

    style.UpdatePosition(DisplayIndexToTextIndex(end));
  }

  // Undo the temporarily applied composition underlines and selection colors.
  UndoCompositionAndSelectionStyles();

  return attributes;
}

void RenderTextMac::ComputeRuns() {
  DCHECK(line_);

  CFArrayRef ct_runs = CTLineGetGlyphRuns(line_);
  const CFIndex ct_runs_count = CFArrayGetCount(ct_runs);

  // TODO(asvitkine): Don't use GetLineOffset() until draw time, since it may be
  // updated based on alignment changes without resetting the layout.
  Vector2d text_offset = GetLineOffset(0);
  // Skia will draw glyphs with respect to the baseline.
  text_offset += Vector2d(0, common_baseline_);

  const SkScalar x = SkIntToScalar(text_offset.x());
  const SkScalar y = SkIntToScalar(text_offset.y());
  SkPoint run_origin = SkPoint::Make(x, y);

  const CFRange empty_cf_range = CFRangeMake(0, 0);
  for (CFIndex i = 0; i < ct_runs_count; ++i) {
    CTRunRef ct_run =
        base::mac::CFCast<CTRunRef>(CFArrayGetValueAtIndex(ct_runs, i));
    const size_t glyph_count = CTRunGetGlyphCount(ct_run);
    const double run_width =
        CTRunGetTypographicBounds(ct_run, empty_cf_range, NULL, NULL, NULL);
    if (glyph_count == 0) {
      run_origin.offset(run_width, 0);
      continue;
    }

    runs_.push_back(TextRun());
    TextRun* run = &runs_.back();
    run->ct_run = ct_run;
    run->origin = run_origin;
    run->width = run_width;
    run->glyphs.resize(glyph_count);
    CTRunGetGlyphs(ct_run, empty_cf_range, &run->glyphs[0]);
    // CTRunGetGlyphs() sometimes returns glyphs with value 65535 and zero
    // width (this has been observed at the beginning of a string containing
    // Arabic content). Passing these to Skia will trigger an assertion;
    // instead set their values to 0.
    for (size_t glyph = 0; glyph < glyph_count; glyph++) {
      if (run->glyphs[glyph] == 65535)
        run->glyphs[glyph] = 0;
    }

    run->glyph_positions.resize(glyph_count);
    const CGPoint* positions_ptr = CTRunGetPositionsPtr(ct_run);
    std::vector<CGPoint> positions;
    if (positions_ptr == NULL) {
      positions.resize(glyph_count);
      CTRunGetPositions(ct_run, empty_cf_range, &positions[0]);
      positions_ptr = &positions[0];
    }
    for (size_t glyph = 0; glyph < glyph_count; glyph++) {
      SkPoint* point = &run->glyph_positions[glyph];
      point->set(x + SkDoubleToScalar(positions_ptr[glyph].x),
                 y + SkDoubleToScalar(positions_ptr[glyph].y));
    }

    // TODO(asvitkine): Style boundaries are not necessarily per-run. Handle
    //                  this better. Also, support strike and diagonal_strike.
    CFDictionaryRef attributes = CTRunGetAttributes(ct_run);
    CTFontRef ct_font = base::mac::GetValueFromDictionary<CTFontRef>(
        attributes, kCTFontAttributeName);
    run->font = Font(static_cast<NSFont*>(ct_font));

    const CGColorRef foreground = base::mac::GetValueFromDictionary<CGColorRef>(
        attributes, kCTForegroundColorAttributeName);
    if (foreground)
      run->foreground = skia::CGColorRefToSkColor(foreground);

    const CFNumberRef underline =
        base::mac::GetValueFromDictionary<CFNumberRef>(
            attributes, kCTUnderlineStyleAttributeName);
    CTUnderlineStyle value = kCTUnderlineStyleNone;
    if (underline && CFNumberGetValue(underline, kCFNumberSInt32Type, &value))
      run->underline = (value == kCTUnderlineStyleSingle);

    run_origin.offset(run_width, 0);
  }
  runs_valid_ = true;
}

void RenderTextMac::InvalidateStyle() {
  line_.reset();
  attributes_.reset();
  runs_.clear();
  runs_valid_ = false;
}

}  // namespace gfx
