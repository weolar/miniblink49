// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/platform_font_win.h"

#include <dwrite.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <wchar.h>
#include <windows.h>

#include <algorithm>

#include "base/debug/alias.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/scoped_comptr.h"
#include "base/win/scoped_gdi_object.h"
#include "base/win/scoped_hdc.h"
#include "base/win/scoped_select_object.h"
#include "base/win/win_util.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_render_params.h"
#include "ui/gfx/win/scoped_set_map_mode.h"

namespace {

// If the tmWeight field of a TEXTMETRIC structure has a value >= this, the
// font is bold.
const int kTextMetricWeightBold = 700;

// Returns the minimum font size, using the minimum size callback, if set.
int GetMinimumFontSize()
{
    int min_font_size = 0;
    if (gfx::PlatformFontWin::get_minimum_font_size_callback)
        min_font_size = gfx::PlatformFontWin::get_minimum_font_size_callback();
    return min_font_size;
}

// Returns either minimum font allowed for a current locale or
// lf_height + size_delta value.
int AdjustFontSize(int lf_height, int size_delta)
{
    if (lf_height < 0) {
        lf_height -= size_delta;
    } else {
        lf_height += size_delta;
    }
    const int min_font_size = GetMinimumFontSize();
    // Make sure lf_height is not smaller than allowed min font size for current
    // locale.
    if (abs(lf_height) < min_font_size) {
        return lf_height < 0 ? -min_font_size : min_font_size;
    } else {
        return lf_height;
    }
}

// Sets style properties on |font_info| based on |font_style|.
void SetLogFontStyle(int font_style, LOGFONT* font_info)
{
    font_info->lfUnderline = (font_style & gfx::Font::UNDERLINE) != 0;
    font_info->lfItalic = (font_style & gfx::Font::ITALIC) != 0;
    font_info->lfWeight = (font_style & gfx::Font::BOLD) ? FW_BOLD : FW_NORMAL;
}

// Returns the family name for the |IDWriteFont| interface passed in.
// The family name is returned in the |family_name_ret| parameter.
// Returns S_OK on success.
// TODO(ananta)
// Remove the CHECKs in this function once this stabilizes on the field.
HRESULT GetFamilyNameFromDirectWriteFont(IDWriteFont* dwrite_font,
    base::string16* family_name_ret)
{
    base::win::ScopedComPtr<IDWriteFontFamily> font_family;
    HRESULT hr = dwrite_font->GetFontFamily(font_family.Receive());
    if (FAILED(hr))
        CHECK(false);

    base::win::ScopedComPtr<IDWriteLocalizedStrings> family_name;
    hr = font_family->GetFamilyNames(family_name.Receive());
    if (FAILED(hr))
        CHECK(false);

    // TODO(ananta)
    // Add support for retrieving the family for the current locale.
    wchar_t family_name_for_locale[MAX_PATH] = { 0 };
    hr = family_name->GetString(0,
        family_name_for_locale,
        arraysize(family_name_for_locale));
    if (FAILED(hr))
        CHECK(false);

    *family_name_ret = family_name_for_locale;
    return hr;
}

// Uses the GDI interop functionality exposed by DirectWrite to find a
// matching DirectWrite font for the LOGFONT passed in. If we fail to
// find a direct match then we try the DirectWrite font substitution
// route to find a match.
// The contents of the LOGFONT pointer |font_info| may be modified on
// return.
HRESULT FindDirectWriteFontForLOGFONT(IDWriteFactory* factory,
    LOGFONT* font_info,
    IDWriteFont** dwrite_font)
{
    base::win::ScopedComPtr<IDWriteGdiInterop> gdi_interop;
    HRESULT hr = factory->GetGdiInterop(gdi_interop.Receive());
    if (FAILED(hr)) {
        CHECK(false);
        return hr;
    }

    hr = gdi_interop->CreateFontFromLOGFONT(font_info, dwrite_font);
    if (SUCCEEDED(hr))
        return hr;

    base::win::ScopedComPtr<IDWriteFontCollection> font_collection;
    hr = factory->GetSystemFontCollection(font_collection.Receive());
    if (FAILED(hr)) {
        CHECK(false);
        return hr;
    }

    // We try to find a matching font by triggering DirectWrite to substitute the
    // font passed in with a matching font (FontSubstitutes registry key)
    // If this succeeds we return the matched font.
    base::win::ScopedGDIObject<HFONT> font(::CreateFontIndirect(font_info));
    base::win::ScopedGetDC screen_dc(NULL);
    base::win::ScopedSelectObject scoped_font(screen_dc, font.get());

    base::win::ScopedComPtr<IDWriteFontFace> font_face;
    hr = gdi_interop->CreateFontFaceFromHdc(screen_dc, font_face.Receive());
    if (FAILED(hr))
        return hr;

    LOGFONT converted_font = { 0 };
    hr = gdi_interop->ConvertFontFaceToLOGFONT(font_face.get(), &converted_font);
    if (SUCCEEDED(hr)) {
        hr = font_collection->GetFontFromFontFace(font_face.get(), dwrite_font);
        if (SUCCEEDED(hr)) {
            wcscpy_s(font_info->lfFaceName, arraysize(font_info->lfFaceName),
                converted_font.lfFaceName);
        }
    }
    return hr;
}

// Returns a matching IDWriteFont for the |font_info| passed in. If we fail
// to find a matching font, then we return the IDWriteFont corresponding to
// the default font on the system.
// Returns S_OK on success.
// The contents of the LOGFONT pointer |font_info| may be modified on
// return.
HRESULT GetMatchingDirectWriteFont(LOGFONT* font_info,
    int font_style,
    IDWriteFactory* factory,
    IDWriteFont** dwrite_font)
{
    // First try the GDI compat route to get a matching DirectWrite font.
    // If that succeeds then we are good. If that fails then try and find a
    // match from the DirectWrite font collection.
    HRESULT hr = FindDirectWriteFontForLOGFONT(factory, font_info, dwrite_font);
    if (SUCCEEDED(hr))
        return hr;

    // Get a matching font from the system font collection exposed by
    // DirectWrite.
    base::win::ScopedComPtr<IDWriteFontCollection> font_collection;
    hr = factory->GetSystemFontCollection(font_collection.Receive());
    if (FAILED(hr)) {
        CHECK(false);
        return hr;
    }

    // Steps as below:-
    // This mirrors skia.
    // 1. Attempt to find a DirectWrite font family based on the face name in the
    //    font. That may not work at all times, as the face name could be random
    //    GDI has its own font system where in it creates a font matching the
    //    characteristics in the LOGFONT structure passed into
    //    CreateFontIndirect. DirectWrite does not do that. If this succeeds then
    //    return the matching IDWriteFont from the family.
    // 2. If step 1 fails then repeat with the default system font. This has the
    //    same limitations with the face name as mentioned above.
    // 3. If step 2 fails then return the first family from the collection and
    //    use that.
    base::win::ScopedComPtr<IDWriteFontFamily> font_family;
    BOOL exists = FALSE;
    uint32_t index = 0;
    hr = font_collection->FindFamilyName(font_info->lfFaceName, &index, &exists);
    // If we fail to find a match then try fallback to the default font on the
    // system. This is what skia does as well.
    if (FAILED(hr) || (index == UINT_MAX) || !exists) {
        NONCLIENTMETRICS metrics = { 0 };
        metrics.cbSize = sizeof(metrics);
        if (!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,
                sizeof(metrics),
                &metrics,
                0)) {
            CHECK(false);
            return E_FAIL;
        }

        if (wcsncmp(font_info->lfFaceName, metrics.lfMessageFont.lfFaceName,
                arraysize(font_info->lfFaceName))) {
            // First try the GDI compat route to get a matching DirectWrite font. If
            // that succeeds we are good. If not find a matching font from the font
            // collection.
            wcscpy_s(font_info->lfFaceName, arraysize(font_info->lfFaceName),
                metrics.lfMessageFont.lfFaceName);
            hr = FindDirectWriteFontForLOGFONT(factory, font_info, dwrite_font);
            if (SUCCEEDED(hr))
                return hr;

            // Best effort to find a matching font from the system font collection.
            hr = font_collection->FindFamilyName(metrics.lfMessageFont.lfFaceName,
                &index,
                &exists);
        }
    }

    if (index != UINT_MAX && exists) {
        hr = font_collection->GetFontFamily(index, font_family.Receive());
    } else {
        // If we fail to find a matching font, then fallback to the first font in
        // the list. This is what skia does as well.
        hr = font_collection->GetFontFamily(0, font_family.Receive());
    }

    if (FAILED(hr)) {
        CHECK(false);
        return hr;
    }

    DWRITE_FONT_WEIGHT weight = (font_style & SkTypeface::kBold)
        ? DWRITE_FONT_WEIGHT_BOLD
        : DWRITE_FONT_WEIGHT_NORMAL;
    DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL;
    DWRITE_FONT_STYLE italic = (font_style & SkTypeface::kItalic)
        ? DWRITE_FONT_STYLE_ITALIC
        : DWRITE_FONT_STYLE_NORMAL;

    // The IDWriteFontFamily::GetFirstMatchingFont call fails on certain machines
    // for fonts like MS UI Gothic, Segoe UI, etc. It is not clear why these
    // fonts could be accessible to GDI and not to DirectWrite.
    // The code below adds some debug fields to help track down these failures.
    // 1. We get the matching font list for the font attributes passed in.
    // 2. We get the font count in the family with a debug alias variable.
    // 3. If GetFirstMatchingFont fails then we CHECK as before.
    // Next step would be to remove the CHECKs in this function and fallback to
    // GDI.
    // http://crbug.com/434425
    // TODO(ananta)
    // Remove the GetMatchingFonts and related code here once we get to a stable
    // state in canary.
    base::win::ScopedComPtr<IDWriteFontList> matching_font_list;
    hr = font_family->GetMatchingFonts(weight, stretch, italic,
        matching_font_list.Receive());
    uint32_t matching_font_count = 0;
    if (SUCCEEDED(hr))
        matching_font_count = matching_font_list->GetFontCount();

    hr = font_family->GetFirstMatchingFont(weight, stretch, italic,
        dwrite_font);
    if (FAILED(hr)) {
        base::debug::Alias(&matching_font_count);
        CHECK(false);
    }

    base::string16 font_name;
    GetFamilyNameFromDirectWriteFont(*dwrite_font, &font_name);
    wcscpy_s(font_info->lfFaceName, arraysize(font_info->lfFaceName),
        font_name.c_str());
    return hr;
}

} // namespace

namespace gfx {

// static
PlatformFontWin::HFontRef* PlatformFontWin::base_font_ref_;

// static
PlatformFontWin::AdjustFontCallback
    PlatformFontWin::adjust_font_callback
    = nullptr;
PlatformFontWin::GetMinimumFontSizeCallback
    PlatformFontWin::get_minimum_font_size_callback
    = NULL;

IDWriteFactory* PlatformFontWin::direct_write_factory_ = nullptr;

////////////////////////////////////////////////////////////////////////////////
// PlatformFontWin, public

PlatformFontWin::PlatformFontWin()
    : font_ref_(GetBaseFontRef())
{
}

PlatformFontWin::PlatformFontWin(NativeFont native_font)
{
    InitWithCopyOfHFONT(native_font);
}

PlatformFontWin::PlatformFontWin(const std::string& font_name,
    int font_size)
{
    InitWithFontNameAndSize(font_name, font_size);
}

Font PlatformFontWin::DeriveFontWithHeight(int height, int style)
{
    DCHECK_GE(height, 0);

    // Create a font with a height near that of the target height.
    LOGFONT font_info;
    GetObject(GetNativeFont(), sizeof(LOGFONT), &font_info);
    font_info.lfHeight = height;
    SetLogFontStyle(style, &font_info);

    HFONT hfont = CreateFontIndirect(&font_info);
    Font font(new PlatformFontWin(CreateHFontRef(hfont)));

    // Respect the minimum font size constraint.
    const int min_font_size = GetMinimumFontSize();

    // Used to avoid shrinking the font and expanding it.
    bool ran_shrink_loop = false;

    // Iterate to find the largest font with a height <= |height|.
    while ((font.GetHeight() > height) && (font.GetFontSize() >= min_font_size)) {
        ran_shrink_loop = true;
        Font derived_font = font.Derive(-1, style);
        // Break the loop if the derived font is too small or hasn't shrunk at all.
        if ((derived_font.GetFontSize() < min_font_size) || ((derived_font.GetFontSize() == font.GetFontSize()) && (derived_font.GetHeight() == font.GetHeight())))
            break;
        font = derived_font;
    }

    while ((!ran_shrink_loop && font.GetHeight() <= height) || (font.GetFontSize() < min_font_size)) {
        Font derived_font = font.Derive(1, style);
        // Break the loop if the derived font is too large or hasn't grown at all.
        if (((derived_font.GetHeight() > height) && (font.GetFontSize() >= min_font_size)) || ((derived_font.GetFontSize() == font.GetFontSize()) && (derived_font.GetHeight() == font.GetHeight())))
            break;
        font = derived_font;
    }
    return font;
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFontWin, PlatformFont implementation:

Font PlatformFontWin::DeriveFont(int size_delta, int style) const
{
    LOGFONT font_info;
    GetObject(GetNativeFont(), sizeof(LOGFONT), &font_info);
    const int requested_font_size = font_ref_->requested_font_size();
    font_info.lfHeight = AdjustFontSize(-requested_font_size, size_delta);
    SetLogFontStyle(style, &font_info);

    HFONT hfont = CreateFontIndirect(&font_info);
    return Font(new PlatformFontWin(CreateHFontRef(hfont)));
}

int PlatformFontWin::GetHeight()
{
    return font_ref_->height();
}

int PlatformFontWin::GetBaseline()
{
    return font_ref_->baseline();
}

int PlatformFontWin::GetCapHeight()
{
    return font_ref_->cap_height();
}

int PlatformFontWin::GetExpectedTextWidth(int length)
{
    return length * std::min(font_ref_->GetDluBaseX(), font_ref_->ave_char_width());
}

int PlatformFontWin::GetStyle() const
{
    return font_ref_->style();
}

const std::string& PlatformFontWin::GetFontName() const
{
    return font_ref_->font_name();
}

std::string PlatformFontWin::GetActualFontNameForTesting() const
{
    // With the current implementation on Windows, HFontRef::font_name() returns
    // the font name taken from the HFONT handle, but it's not the name that comes
    // from the font's metadata.  See http://crbug.com/327287
    return font_ref_->font_name();
}

std::string PlatformFontWin::GetLocalizedFontName() const
{
    base::win::ScopedCreateDC memory_dc(CreateCompatibleDC(NULL));
    if (!memory_dc.Get())
        return GetFontName();

    // When a font has a localized name for a language matching the system
    // locale, GetTextFace() returns the localized name.
    base::win::ScopedSelectObject font(memory_dc.Get(), font_ref_->hfont());
    wchar_t localized_font_name[LF_FACESIZE];
    int length = GetTextFace(memory_dc.Get(), arraysize(localized_font_name),
        &localized_font_name[0]);
    if (length <= 0)
        return GetFontName();
    return base::SysWideToUTF8(localized_font_name);
}

int PlatformFontWin::GetFontSize() const
{
    return font_ref_->font_size();
}

const FontRenderParams& PlatformFontWin::GetFontRenderParams()
{
    CR_DEFINE_STATIC_LOCAL(const gfx::FontRenderParams, params,
        (gfx::GetFontRenderParams(gfx::FontRenderParamsQuery(), NULL)));
    return params;
}

NativeFont PlatformFontWin::GetNativeFont() const
{
    return font_ref_->hfont();
}

// static
void PlatformFontWin::SetDirectWriteFactory(IDWriteFactory* factory)
{
    // We grab a reference on the DirectWrite factory. This reference is
    // leaked, which is ok because skia leaks it as well.
    factory->AddRef();
    direct_write_factory_ = factory;
}

// static
void PlatformFontWin::GetTextMetricsForFont(HDC hdc,
    HFONT font,
    TEXTMETRIC* text_metrics)
{
    base::win::ScopedSelectObject scoped_font(hdc, font);
    GetTextMetrics(hdc, text_metrics);
}

// static
int PlatformFontWin::GetFontSize(const LOGFONT& font_info)
{
    if (font_info.lfHeight < 0)
        return -font_info.lfHeight;

    base::win::ScopedGetDC screen_dc(NULL);
    base::win::ScopedGDIObject<HFONT> font(CreateFontIndirect(&font_info));

    TEXTMETRIC font_metrics = { 0 };
    PlatformFontWin::GetTextMetricsForFont(screen_dc, font.get(), &font_metrics);
    return font_metrics.tmAscent;
}

////////////////////////////////////////////////////////////////////////////////
// Font, private:

void PlatformFontWin::InitWithCopyOfHFONT(HFONT hfont)
{
    DCHECK(hfont);
    LOGFONT font_info;
    GetObject(hfont, sizeof(LOGFONT), &font_info);
    font_ref_ = CreateHFontRef(CreateFontIndirect(&font_info));
}

void PlatformFontWin::InitWithFontNameAndSize(const std::string& font_name,
    int font_size)
{
    HFONT hf = ::CreateFont(-font_size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        base::UTF8ToUTF16(font_name).c_str());
    font_ref_ = CreateHFontRef(hf);
}

// static
PlatformFontWin::HFontRef* PlatformFontWin::GetBaseFontRef()
{
    if (base_font_ref_ == NULL) {
        NONCLIENTMETRICS_XP metrics;
        base::win::GetNonClientMetrics(&metrics);

        if (adjust_font_callback)
            adjust_font_callback(&metrics.lfMessageFont);
        metrics.lfMessageFont.lfHeight = AdjustFontSize(metrics.lfMessageFont.lfHeight, 0);
        HFONT font = CreateFontIndirect(&metrics.lfMessageFont);
        DLOG_ASSERT(font);
        base_font_ref_ = PlatformFontWin::CreateHFontRef(font);
        // base_font_ref_ is global, up the ref count so it's never deleted.
        base_font_ref_->AddRef();
    }
    return base_font_ref_;
}

PlatformFontWin::HFontRef* PlatformFontWin::CreateHFontRef(HFONT font)
{
    TEXTMETRIC font_metrics;

    {
        base::win::ScopedGetDC screen_dc(NULL);
        gfx::ScopedSetMapMode mode(screen_dc, MM_TEXT);
        GetTextMetricsForFont(screen_dc, font, &font_metrics);
    }

    if (direct_write_factory_)
        return CreateHFontRefFromSkia(font, font_metrics);

    return CreateHFontRefFromGDI(font, font_metrics);
}

PlatformFontWin::HFontRef* PlatformFontWin::CreateHFontRefFromGDI(
    HFONT font,
    const TEXTMETRIC& font_metrics)
{
    const int height = std::max<int>(1, font_metrics.tmHeight);
    const int baseline = std::max<int>(1, font_metrics.tmAscent);
    const int cap_height = std::max<int>(1, font_metrics.tmAscent - font_metrics.tmInternalLeading);
    const int ave_char_width = std::max<int>(1, font_metrics.tmAveCharWidth);
    const int font_size = std::max<int>(1, font_metrics.tmHeight - font_metrics.tmInternalLeading);
    int style = 0;
    if (font_metrics.tmItalic)
        style |= Font::ITALIC;
    if (font_metrics.tmUnderlined)
        style |= Font::UNDERLINE;
    if (font_metrics.tmWeight >= kTextMetricWeightBold)
        style |= Font::BOLD;

    return new HFontRef(font, font_size, height, baseline, cap_height,
        ave_char_width, style);
}

// static
PlatformFontWin::HFontRef* PlatformFontWin::CreateHFontRefFromSkia(
    HFONT gdi_font,
    const TEXTMETRIC& font_metrics)
{
    LOGFONT font_info = { 0 };
    GetObject(gdi_font, sizeof(LOGFONT), &font_info);

    // If the font height is passed in as 0, assume the height to be -1 to ensure
    // that we return the metrics for a 1 point font.
    // If the font height is positive it represents the rasterized font's cell
    // height. Calculate the actual height accordingly.
    if (font_info.lfHeight > 0) {
        font_info.lfHeight = font_metrics.tmInternalLeading - font_metrics.tmHeight;
    } else if (font_info.lfHeight == 0) {
        font_info.lfHeight = -1;
    }

    int skia_style = SkTypeface::kNormal;
    if (font_info.lfWeight >= FW_SEMIBOLD && font_info.lfWeight <= FW_ULTRABOLD) {
        skia_style |= SkTypeface::kBold;
    }
    if (font_info.lfItalic)
        skia_style |= SkTypeface::kItalic;

    // Skia does not return all values we need for font metrics. For e.g.
    // the cap height which indicates the height of capital letters is not
    // returned even though it is returned by DirectWrite.
    // TODO(ananta)
    // Fix SkScalerContext_win_dw.cpp to return all metrics we need from
    // DirectWrite and remove the code here which retrieves metrics from
    // DirectWrite to calculate the cap height.
    base::win::ScopedComPtr<IDWriteFont> dwrite_font;
    HRESULT hr = GetMatchingDirectWriteFont(&font_info,
        skia_style,
        direct_write_factory_,
        dwrite_font.Receive());
    if (FAILED(hr)) {
        CHECK(false);
        return nullptr;
    }

    DWRITE_FONT_METRICS dwrite_font_metrics = { 0 };
    dwrite_font->GetMetrics(&dwrite_font_metrics);

    skia::RefPtr<SkTypeface> skia_face = skia::AdoptRef(
        SkTypeface::CreateFromName(
            base::SysWideToUTF8(font_info.lfFaceName).c_str(),
            static_cast<SkTypeface::Style>(skia_style)));

    gfx::FontRenderParams font_params = gfx::GetFontRenderParams(gfx::FontRenderParamsQuery(), nullptr);
    SkFontHost::SetSubpixelOrder(
        gfx::FontRenderParams::SubpixelRenderingToSkiaLCDOrder(
            font_params.subpixel_rendering));
    SkFontHost::SetSubpixelOrientation(
        gfx::FontRenderParams::SubpixelRenderingToSkiaLCDOrientation(
            font_params.subpixel_rendering));

    SkPaint paint;
    paint.setAntiAlias(font_params.antialiasing);
    paint.setTypeface(skia_face.get());
    paint.setTextSize(-font_info.lfHeight);
    SkPaint::FontMetrics skia_metrics;
    paint.getFontMetrics(&skia_metrics);

    // The calculations below are similar to those in the CreateHFontRef
    // function. The height, baseline and cap height are rounded up to ensure
    // that they match up closely with GDI.
    const int height = std::ceil(skia_metrics.fDescent - skia_metrics.fAscent);
    const int baseline = std::max<int>(1, std::ceil(-skia_metrics.fAscent));
    const int cap_height = std::ceil(paint.getTextSize() * static_cast<double>(dwrite_font_metrics.capHeight) / dwrite_font_metrics.designUnitsPerEm);

    // The metrics retrieved from skia don't have the average character width. In
    // any case if we get the average character width from skia then use that or
    // the average character width in the TEXTMETRIC structure.
    // TODO(ananta): Investigate whether it is possible to retrieve this value
    // from DirectWrite.
    const int ave_char_width = skia_metrics.fAvgCharWidth == 0 ? font_metrics.tmAveCharWidth
                                                               : skia_metrics.fAvgCharWidth;

    int style = 0;
    if (skia_style & SkTypeface::kItalic)
        style |= Font::ITALIC;
    if (font_info.lfUnderline)
        style |= Font::UNDERLINE;
    if (font_info.lfWeight >= kTextMetricWeightBold)
        style |= Font::BOLD;

    // DirectWrite may have substituted the GDI font name with a fallback
    // font. Ensure that it is updated here.
    DeleteObject(gdi_font);
    gdi_font = ::CreateFontIndirect(&font_info);
    return new HFontRef(gdi_font, -font_info.lfHeight, height, baseline,
        cap_height, ave_char_width, style);
}

PlatformFontWin::PlatformFontWin(HFontRef* hfont_ref)
    : font_ref_(hfont_ref)
{
}

PlatformFontWin::~PlatformFontWin()
{
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFontWin::HFontRef:

PlatformFontWin::HFontRef::HFontRef(HFONT hfont,
    int font_size,
    int height,
    int baseline,
    int cap_height,
    int ave_char_width,
    int style)
    : hfont_(hfont)
    , font_size_(font_size)
    , height_(height)
    , baseline_(baseline)
    , cap_height_(cap_height)
    , ave_char_width_(ave_char_width)
    , style_(style)
    , dlu_base_x_(-1)
    , requested_font_size_(font_size)
{
    DLOG_ASSERT(hfont);

    LOGFONT font_info;
    GetObject(hfont_, sizeof(LOGFONT), &font_info);
    font_name_ = base::UTF16ToUTF8(base::string16(font_info.lfFaceName));

    // Retrieve the font size from the GetTextMetrics API instead of referencing
    // it from the LOGFONT structure. This is because the height as reported by
    // the LOGFONT structure is not always correct. For small fonts with size 1
    // the LOGFONT structure reports the height as -1, while the actual font size
    // is different. (2 on my XP machine).
    base::win::ScopedGetDC screen_dc(NULL);
    TEXTMETRIC font_metrics = { 0 };
    PlatformFontWin::GetTextMetricsForFont(screen_dc, hfont_, &font_metrics);
    requested_font_size_ = font_metrics.tmHeight - font_metrics.tmInternalLeading;
}

int PlatformFontWin::HFontRef::GetDluBaseX()
{
    if (dlu_base_x_ != -1)
        return dlu_base_x_;

    dlu_base_x_ = GetAverageCharWidthInDialogUnits(hfont_);
    return dlu_base_x_;
}

// static
int PlatformFontWin::HFontRef::GetAverageCharWidthInDialogUnits(
    HFONT gdi_font)
{
    base::win::ScopedGetDC screen_dc(NULL);
    base::win::ScopedSelectObject font(screen_dc, gdi_font);
    gfx::ScopedSetMapMode mode(screen_dc, MM_TEXT);

    // Yes, this is how Microsoft recommends calculating the dialog unit
    // conversions. See: http://support.microsoft.com/kb/125681
    SIZE ave_text_size;
    GetTextExtentPoint32(screen_dc,
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
        52, &ave_text_size);
    int dlu_base_x = (ave_text_size.cx / 26 + 1) / 2;

    DCHECK_NE(dlu_base_x, -1);
    return dlu_base_x;
}

PlatformFontWin::HFontRef::~HFontRef()
{
    DeleteObject(hfont_);
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFont, public:

// static
PlatformFont* PlatformFont::CreateDefault()
{
    return new PlatformFontWin;
}

// static
PlatformFont* PlatformFont::CreateFromNativeFont(NativeFont native_font)
{
    return new PlatformFontWin(native_font);
}

// static
PlatformFont* PlatformFont::CreateFromNameAndSize(const std::string& font_name,
    int font_size)
{
    return new PlatformFontWin(font_name, font_size);
}

} // namespace gfx
