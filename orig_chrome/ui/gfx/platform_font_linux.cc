// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/platform_font_linux.h"

#include <algorithm>
#include <string>

#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkString.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/linux_font_delegate.h"
#include "ui/gfx/text_utils.h"

namespace gfx {
namespace {

// The font family name which is used when a user's application font for
// GNOME/KDE is a non-scalable one. The name should be listed in the
// IsFallbackFontAllowed function in skia/ext/SkFontHost_fontconfig_direct.cpp.
#if defined(OS_ANDROID)
    const char* kFallbackFontFamilyName = "serif";
#else
    const char* kFallbackFontFamilyName = "sans";
#endif

    // The default font, used for the default constructor.
    base::LazyInstance<scoped_refptr<PlatformFontLinux>>::Leaky g_default_font = LAZY_INSTANCE_INITIALIZER;

    // Creates a SkTypeface for the passed-in Font::FontStyle and family. If a
    // fallback typeface is used instead of the requested family, |family| will be
    // updated to contain the fallback's family name.
    skia::RefPtr<SkTypeface> CreateSkTypeface(int style, std::string* family)
    {
        DCHECK(family);

        int skia_style = SkTypeface::kNormal;
        if (Font::BOLD & style)
            skia_style |= SkTypeface::kBold;
        if (Font::ITALIC & style)
            skia_style |= SkTypeface::kItalic;

        skia::RefPtr<SkTypeface> typeface = skia::AdoptRef(SkTypeface::CreateFromName(
            family->c_str(), static_cast<SkTypeface::Style>(skia_style)));
        if (!typeface) {
            // A non-scalable font such as .pcf is specified. Fall back to a default
            // scalable font.
            typeface = skia::AdoptRef(SkTypeface::CreateFromName(
                kFallbackFontFamilyName, static_cast<SkTypeface::Style>(skia_style)));
            CHECK(typeface) << "Could not find any font: " << family << ", "
                            << kFallbackFontFamilyName;
            *family = kFallbackFontFamilyName;
        }
        return typeface;
    }

} // namespace

#if defined(OS_CHROMEOS)
std::string* PlatformFontLinux::default_font_description_ = NULL;
#endif

////////////////////////////////////////////////////////////////////////////////
// PlatformFontLinux, public:

PlatformFontLinux::PlatformFontLinux()
{
    if (!g_default_font.Get()) {
        std::string family = kFallbackFontFamilyName;
        int size_pixels = 12;
        int style = Font::NORMAL;
        FontRenderParams params;

#if defined(OS_CHROMEOS)
        // On Chrome OS, a FontList font description string is stored as a
        // translatable resource and passed in via SetDefaultFontDescription().
        if (default_font_description_) {
            FontRenderParamsQuery query;
            CHECK(FontList::ParseDescription(*default_font_description_,
                &query.families, &query.style,
                &query.pixel_size))
                << "Failed to parse font description " << *default_font_description_;
            params = gfx::GetFontRenderParams(query, &family);
            size_pixels = query.pixel_size;
            style = query.style;
        }
#else
        // On Linux, LinuxFontDelegate is used to query the native toolkit (e.g.
        // GTK+) for the default UI font.
        const LinuxFontDelegate* delegate = LinuxFontDelegate::instance();
        if (delegate) {
            delegate->GetDefaultFontDescription(
                &family, &size_pixels, &style, &params);
        }
#endif

        g_default_font.Get() = new PlatformFontLinux(
            CreateSkTypeface(style, &family), family, size_pixels, style, params);
    }

    InitFromPlatformFont(g_default_font.Get().get());
}

PlatformFontLinux::PlatformFontLinux(const std::string& font_name,
    int font_size_pixels)
{
    FontRenderParamsQuery query;
    query.families.push_back(font_name);
    query.pixel_size = font_size_pixels;
    query.style = Font::NORMAL;
    InitFromDetails(skia::RefPtr<SkTypeface>(), font_name, font_size_pixels,
        query.style, gfx::GetFontRenderParams(query, NULL));
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFontLinux, PlatformFont implementation:

// static
void PlatformFontLinux::ReloadDefaultFont()
{
    // Reset the scoped_refptr.
    g_default_font.Get() = nullptr;
}

#if defined(OS_CHROMEOS)
// static
void PlatformFontLinux::SetDefaultFontDescription(
    const std::string& font_description)
{
    delete default_font_description_;
    default_font_description_ = new std::string(font_description);
}

#endif

Font PlatformFontLinux::DeriveFont(int size_delta, int style) const
{
    const int new_size = font_size_pixels_ + size_delta;
    DCHECK_GT(new_size, 0);

    // If the style changed, we may need to load a new face.
    std::string new_family = font_family_;
    skia::RefPtr<SkTypeface> typeface = (style == style_) ? typeface_ : CreateSkTypeface(style, &new_family);

    FontRenderParamsQuery query;
    query.families.push_back(new_family);
    query.pixel_size = new_size;
    query.style = style;

    return Font(new PlatformFontLinux(typeface, new_family, new_size, style,
        gfx::GetFontRenderParams(query, NULL)));
}

int PlatformFontLinux::GetHeight()
{
    ComputeMetricsIfNecessary();
    return height_pixels_;
}

int PlatformFontLinux::GetBaseline()
{
    ComputeMetricsIfNecessary();
    return ascent_pixels_;
}

int PlatformFontLinux::GetCapHeight()
{
    ComputeMetricsIfNecessary();
    return cap_height_pixels_;
}

int PlatformFontLinux::GetExpectedTextWidth(int length)
{
    ComputeMetricsIfNecessary();
    return round(static_cast<float>(length) * average_width_pixels_);
}

int PlatformFontLinux::GetStyle() const
{
    return style_;
}

const std::string& PlatformFontLinux::GetFontName() const
{
    return font_family_;
}

std::string PlatformFontLinux::GetActualFontNameForTesting() const
{
    SkString family_name;
    typeface_->getFamilyName(&family_name);
    return family_name.c_str();
}

int PlatformFontLinux::GetFontSize() const
{
    return font_size_pixels_;
}

const FontRenderParams& PlatformFontLinux::GetFontRenderParams()
{
#if defined(OS_CHROMEOS)
    float current_scale_factor = GetFontRenderParamsDeviceScaleFactor();
    if (current_scale_factor != device_scale_factor_) {
        FontRenderParamsQuery query;
        query.families.push_back(font_family_);
        query.pixel_size = font_size_pixels_;
        query.style = style_;
        query.device_scale_factor = current_scale_factor;
        font_render_params_ = gfx::GetFontRenderParams(query, nullptr);
        device_scale_factor_ = current_scale_factor;
    }
#endif
    return font_render_params_;
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFontLinux, private:

PlatformFontLinux::PlatformFontLinux(const skia::RefPtr<SkTypeface>& typeface,
    const std::string& family,
    int size_pixels,
    int style,
    const FontRenderParams& render_params)
{
    InitFromDetails(typeface, family, size_pixels, style, render_params);
}

PlatformFontLinux::~PlatformFontLinux() { }

void PlatformFontLinux::InitFromDetails(
    const skia::RefPtr<SkTypeface>& typeface,
    const std::string& font_family,
    int font_size_pixels,
    int style,
    const FontRenderParams& render_params)
{
    DCHECK_GT(font_size_pixels, 0);

    font_family_ = font_family;
    typeface_ = typeface ? typeface : CreateSkTypeface(style, &font_family_);

    font_size_pixels_ = font_size_pixels;
    style_ = style;
#if defined(OS_CHROMEOS)
    device_scale_factor_ = GetFontRenderParamsDeviceScaleFactor();
#endif
    font_render_params_ = render_params;
}

void PlatformFontLinux::InitFromPlatformFont(const PlatformFontLinux* other)
{
    typeface_ = other->typeface_;
    font_family_ = other->font_family_;
    font_size_pixels_ = other->font_size_pixels_;
    style_ = other->style_;
#if defined(OS_CHROMEOS)
    device_scale_factor_ = other->device_scale_factor_;
#endif
    font_render_params_ = other->font_render_params_;

    if (!other->metrics_need_computation_) {
        metrics_need_computation_ = false;
        ascent_pixels_ = other->ascent_pixels_;
        height_pixels_ = other->height_pixels_;
        cap_height_pixels_ = other->cap_height_pixels_;
        average_width_pixels_ = other->average_width_pixels_;
    }
}

void PlatformFontLinux::ComputeMetricsIfNecessary()
{
    if (metrics_need_computation_) {
        metrics_need_computation_ = false;

        SkPaint paint;
        paint.setAntiAlias(false);
        paint.setSubpixelText(false);
        paint.setTextSize(font_size_pixels_);
        paint.setTypeface(typeface_.get());
        paint.setFakeBoldText((Font::BOLD & style_) && !typeface_->isBold());
        paint.setTextSkewX((Font::ITALIC & style_) && !typeface_->isItalic() ? -SK_Scalar1 / 4 : 0);
        SkPaint::FontMetrics metrics;
        paint.getFontMetrics(&metrics);
        ascent_pixels_ = SkScalarCeilToInt(-metrics.fAscent);
        height_pixels_ = ascent_pixels_ + SkScalarCeilToInt(metrics.fDescent);
        cap_height_pixels_ = SkScalarCeilToInt(metrics.fCapHeight);
        average_width_pixels_ = SkScalarToDouble(metrics.fAvgCharWidth);
    }
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFont, public:

// static
PlatformFont* PlatformFont::CreateDefault()
{
    return new PlatformFontLinux;
}

// static
PlatformFont* PlatformFont::CreateFromNameAndSize(const std::string& font_name,
    int font_size)
{
    return new PlatformFontLinux(font_name, font_size);
}

} // namespace gfx
