// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/platform_font_linux.h"

#include <string>

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_render_params.h"
#include "ui/gfx/linux_font_delegate.h"
#include "ui/gfx/test/fontconfig_util_linux.h"

namespace gfx {

// Implementation of LinuxFontDelegate used to control the default font
// description on Linux.
class TestFontDelegate : public LinuxFontDelegate {
public:
    TestFontDelegate()
        : size_pixels_(0)
        , style_(gfx::Font::NORMAL)
    {
    }
    ~TestFontDelegate() override { }

    void set_family(const std::string& family) { family_ = family; }
    void set_size_pixels(int size_pixels) { size_pixels_ = size_pixels; }
    void set_style(int style) { style_ = style; }
    void set_params(const FontRenderParams& params) { params_ = params; }

    FontRenderParams GetDefaultFontRenderParams() const override
    {
        NOTIMPLEMENTED();
        return FontRenderParams();
    }
    void GetDefaultFontDescription(
        std::string* family_out,
        int* size_pixels_out,
        int* style_out,
        FontRenderParams* params_out) const override
    {
        *family_out = family_;
        *size_pixels_out = size_pixels_;
        *style_out = style_;
        *params_out = params_;
    }

private:
    // Default values to be returned.
    std::string family_;
    int size_pixels_;
    int style_;
    FontRenderParams params_;

    DISALLOW_COPY_AND_ASSIGN(TestFontDelegate);
};

class PlatformFontLinuxTest : public testing::Test {
public:
    PlatformFontLinuxTest()
    {
        SetUpFontconfig();
        original_font_delegate_ = LinuxFontDelegate::instance();
        LinuxFontDelegate::SetInstance(&test_font_delegate_);
    }

    ~PlatformFontLinuxTest() override
    {
        LinuxFontDelegate::SetInstance(
            const_cast<LinuxFontDelegate*>(original_font_delegate_));
        TearDownFontconfig();
    }

protected:
    TestFontDelegate test_font_delegate_;

private:
    // Originally-registered delegate.
    const LinuxFontDelegate* original_font_delegate_;

    DISALLOW_COPY_AND_ASSIGN(PlatformFontLinuxTest);
};

// Test that PlatformFontLinux's default constructor initializes the instance
// with the correct parameters.
TEST_F(PlatformFontLinuxTest, DefaultFont)
{
    ASSERT_TRUE(LoadSystemFontIntoFontconfig("arial.ttf"));
    ASSERT_TRUE(LoadSystemFontIntoFontconfig("times_new_roman.ttf"));

#if defined(OS_CHROMEOS)
    PlatformFontLinux::SetDefaultFontDescription("Arial,Times New Roman,13px");
#else
    test_font_delegate_.set_family("Arial");
    test_font_delegate_.set_size_pixels(13);
    test_font_delegate_.set_style(gfx::Font::NORMAL);
    FontRenderParams params;
    params.antialiasing = false;
    params.hinting = FontRenderParams::HINTING_FULL;
    test_font_delegate_.set_params(params);
#endif
    scoped_refptr<gfx::PlatformFontLinux> font(new gfx::PlatformFontLinux());
    EXPECT_EQ("Arial", font->GetFontName());
    EXPECT_EQ(13, font->GetFontSize());
    EXPECT_EQ(gfx::Font::NORMAL, font->GetStyle());
#if !defined(OS_CHROMEOS)
    // On Linux, the FontRenderParams returned by the the delegate should be used.
    EXPECT_EQ(params.antialiasing, font->GetFontRenderParams().antialiasing);
    EXPECT_EQ(params.hinting, font->GetFontRenderParams().hinting);
#endif

    // Drop the old default font and check that new settings are loaded.
#if defined(OS_CHROMEOS)
    PlatformFontLinux::SetDefaultFontDescription(
        "Times New Roman,Arial,Bold 15px");
#else
    test_font_delegate_.set_family("Times New Roman");
    test_font_delegate_.set_size_pixels(15);
    test_font_delegate_.set_style(gfx::Font::BOLD);
#endif
    PlatformFontLinux::ReloadDefaultFont();
    scoped_refptr<gfx::PlatformFontLinux> font2(new gfx::PlatformFontLinux());
    EXPECT_EQ("Times New Roman", font2->GetFontName());
    EXPECT_EQ(15, font2->GetFontSize());
    EXPECT_EQ(gfx::Font::BOLD, font2->GetStyle());
}

} // namespace gfx
