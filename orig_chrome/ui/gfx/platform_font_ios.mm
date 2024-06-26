// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/platform_font_ios.h"

#import <UIKit/UIKit.h>

#include <cmath>

#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_render_params.h"
#include "ui/gfx/ios/NSString+CrStringDrawing.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// PlatformFontIOS, public:

PlatformFontIOS::PlatformFontIOS() {
  font_size_ = [UIFont systemFontSize];
  style_ = gfx::Font::NORMAL;
  UIFont* system_font = [UIFont systemFontOfSize:font_size_];
  font_name_ = base::SysNSStringToUTF8([system_font fontName]);
  CalculateMetrics();
}

PlatformFontIOS::PlatformFontIOS(NativeFont native_font) {
  std::string font_name = base::SysNSStringToUTF8([native_font fontName]);
  InitWithNameSizeAndStyle(font_name,
                           [native_font pointSize],
                           gfx::Font::NORMAL);
}

PlatformFontIOS::PlatformFontIOS(const std::string& font_name,
                                 int font_size) {
  InitWithNameSizeAndStyle(font_name, font_size, gfx::Font::NORMAL);
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFontIOS, PlatformFont implementation:

Font PlatformFontIOS::DeriveFont(int size_delta, int style) const {
  return Font(new PlatformFontIOS(font_name_, font_size_ + size_delta, style));
}

int PlatformFontIOS::GetHeight() {
  return height_;
}

int PlatformFontIOS::GetBaseline() {
  return ascent_;
}

int PlatformFontIOS::GetCapHeight() {
  return cap_height_;
}

int PlatformFontIOS::GetExpectedTextWidth(int length) {
  return length * average_width_;
}

int PlatformFontIOS::GetStyle() const {
  return style_;
}

const std::string& PlatformFontIOS::GetFontName() const {
  return font_name_;
}

std::string PlatformFontIOS::GetActualFontNameForTesting() const {
  return base::SysNSStringToUTF8([GetNativeFont() familyName]);
}

int PlatformFontIOS::GetFontSize() const {
  return font_size_;
}

const FontRenderParams& PlatformFontIOS::GetFontRenderParams() {
  NOTIMPLEMENTED();
  static FontRenderParams params;
  return params;
}

NativeFont PlatformFontIOS::GetNativeFont() const {
  return [UIFont fontWithName:base::SysUTF8ToNSString(font_name_)
                         size:font_size_];
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFontIOS, private:

PlatformFontIOS::PlatformFontIOS(const std::string& font_name,
                                 int font_size,
                                 int style) {
  InitWithNameSizeAndStyle(font_name, font_size, style);
}

void PlatformFontIOS::InitWithNameSizeAndStyle(const std::string& font_name,
                                               int font_size,
                                               int style) {
  font_name_ = font_name;
  font_size_ = font_size;
  style_ = style;
  CalculateMetrics();
}

void PlatformFontIOS::CalculateMetrics() {
  UIFont* font = GetNativeFont();
  height_ = font.lineHeight;
  ascent_ = font.ascender;
  cap_height_ = font.capHeight;
  average_width_ = [@"x" cr_sizeWithFont:font].width;
}

////////////////////////////////////////////////////////////////////////////////
// PlatformFont, public:

// static
PlatformFont* PlatformFont::CreateDefault() {
  return new PlatformFontIOS;
}

// static
PlatformFont* PlatformFont::CreateFromNativeFont(NativeFont native_font) {
  return new PlatformFontIOS(native_font);
}

// static
PlatformFont* PlatformFont::CreateFromNameAndSize(const std::string& font_name,
                                                  int font_size) {
  return new PlatformFontIOS(font_name, font_size);
}

}  // namespace gfx
