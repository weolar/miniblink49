// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <Cocoa/Cocoa.h>
#include <stddef.h>

#include "base/macros.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/font.h"

TEST(PlatformFontMacTest, DeriveFont) {
  // Use a base font that support all traits.
  gfx::Font base_font("Helvetica", 13);

  // Bold
  gfx::Font bold_font(base_font.Derive(0, gfx::Font::BOLD));
  NSFontTraitMask traits = [[NSFontManager sharedFontManager]
      traitsOfFont:bold_font.GetNativeFont()];
  EXPECT_EQ(NSBoldFontMask, traits);

  // Italic
  gfx::Font italic_font(base_font.Derive(0, gfx::Font::ITALIC));
  traits = [[NSFontManager sharedFontManager]
      traitsOfFont:italic_font.GetNativeFont()];
  EXPECT_EQ(NSItalicFontMask, traits);

  // Bold italic
  gfx::Font bold_italic_font(base_font.Derive(
      0, gfx::Font::BOLD | gfx::Font::ITALIC));
  traits = [[NSFontManager sharedFontManager]
      traitsOfFont:bold_italic_font.GetNativeFont()];
  EXPECT_EQ(static_cast<NSFontTraitMask>(NSBoldFontMask | NSItalicFontMask),
            traits);
}

TEST(PlatformFontMacTest, ConstructFromNativeFont) {
  gfx::Font normal_font([NSFont fontWithName:@"Helvetica" size:12]);
  EXPECT_EQ(12, normal_font.GetFontSize());
  EXPECT_EQ("Helvetica", normal_font.GetFontName());
  EXPECT_EQ(gfx::Font::NORMAL, normal_font.GetStyle());

  gfx::Font bold_font([NSFont fontWithName:@"Helvetica-Bold" size:14]);
  EXPECT_EQ(14, bold_font.GetFontSize());
  EXPECT_EQ("Helvetica", bold_font.GetFontName());
  EXPECT_EQ(gfx::Font::BOLD, bold_font.GetStyle());

  gfx::Font italic_font([NSFont fontWithName:@"Helvetica-Oblique" size:14]);
  EXPECT_EQ(14, italic_font.GetFontSize());
  EXPECT_EQ("Helvetica", italic_font.GetFontName());
  EXPECT_EQ(gfx::Font::ITALIC, italic_font.GetStyle());

  gfx::Font bold_italic_font(
      [NSFont fontWithName:@"Helvetica-BoldOblique" size:14]);
  EXPECT_EQ(14, bold_italic_font.GetFontSize());
  EXPECT_EQ("Helvetica", bold_italic_font.GetFontName());
  EXPECT_EQ(gfx::Font::BOLD | gfx::Font::ITALIC, bold_italic_font.GetStyle());
}

// Ensures that the Font's reported height is consistent with the native font's
// ascender and descender metrics.
TEST(PlatformFontMacTest, ValidateFontHeight) {
  // Use the default ResourceBundle system font. E.g. Helvetica Neue in 10.10,
  // Lucida Grande before that, and San Francisco after.
  gfx::Font default_font;
  gfx::Font::FontStyle styles[] = {
    gfx::Font::NORMAL, gfx::Font::BOLD, gfx::Font::ITALIC, gfx::Font::UNDERLINE
  };

  for (size_t i = 0; i < arraysize(styles); ++i) {
    SCOPED_TRACE(testing::Message() << "Font::FontStyle: " << styles[i]);
    // Include the range of sizes used by ResourceBundle::FontStyle (-1 to +8).
    for (int delta = -1; delta <= 8; ++delta) {
      gfx::Font font = default_font.Derive(delta, styles[i]);
      SCOPED_TRACE(testing::Message() << "FontSize(): " << font.GetFontSize());
      NSFont* native_font = font.GetNativeFont();

      // Font height (an integer) should be the sum of these.
      CGFloat ascender = [native_font ascender];
      CGFloat descender = [native_font descender];
      CGFloat leading = [native_font leading];

      // NSFont always gives a negative value for descender. Others positive.
      EXPECT_GE(0, descender);
      EXPECT_LE(0, ascender);
      EXPECT_LE(0, leading);

      int sum = ceil(ascender - descender + leading);

      // Text layout is performed using an integral baseline offset derived from
      // the ascender. The height needs to be enough to fit the full descender
      // (plus baseline). So the height depends on the rounding of the ascender,
      // and can be as much as 1 greater than the simple sum of floats.
      EXPECT_LE(sum, font.GetHeight());
      EXPECT_GE(sum + 1, font.GetHeight());

      // Recreate the rounding performed for GetBaseLine().
      EXPECT_EQ(ceil(ceil(ascender) - descender + leading), font.GetHeight());
    }
  }
}
