// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/color_profile.h"

#import <Cocoa/Cocoa.h>
#include <stddef.h>

#include "base/mac/mac_util.h"
#include "base/mac/scoped_cftyperef.h"
#include "ui/gfx/mac/coordinate_conversion.h"

namespace {

NSScreen* GetNSScreenFromBounds(const gfx::Rect& bounds) {
  NSScreen* screen = nil;
  int overlap = 0;

  for (NSScreen* monitor in [NSScreen screens]) {
    gfx::Rect monitor_rect = gfx::ScreenRectFromNSRect([monitor frame]);
    gfx::Rect overlap_rect = gfx::IntersectRects(monitor_rect, bounds);
    int overlap_size = overlap_rect.width() * overlap_rect.height();
    if (overlap_size > overlap) {
      overlap = overlap_size;
      screen = monitor;
    }
  }

  return screen;
}

}  // namespace

namespace gfx {

bool GetDisplayColorProfile(const gfx::Rect& bounds,
                            std::vector<char>* profile) {
  DCHECK(profile->empty());

  NSScreen* screen = GetNSScreenFromBounds(bounds);
  if (!screen || bounds.IsEmpty())
    return false;
  NSColorSpace* color_space = [screen colorSpace];
  if (!color_space)
    return false;

  if ([color_space isEqual:[NSColorSpace sRGBColorSpace]])
    return true;
  NSData* profile_data = [color_space ICCProfileData];
  const char* data = static_cast<const char*>([profile_data bytes]);
  size_t length = [profile_data length];
  if (data && !gfx::InvalidColorProfileLength(length))
    profile->assign(data, data + length);
  return true;
}

GFX_EXPORT bool GetDisplayColorProfile(gfx::NativeWindow window,
                                       std::vector<char>* profile) {
  DCHECK(profile->empty());

  NSColorSpace* color_space = [window colorSpace];
  if (!color_space || NSIsEmptyRect([window frame]))
    return false;

  if ([color_space isEqual:[NSColorSpace sRGBColorSpace]])
    return true;
  NSData* profile_data = [color_space ICCProfileData];
  const char* data = static_cast<const char*>([profile_data bytes]);
  size_t length = [profile_data length];
  if (data && !gfx::InvalidColorProfileLength(length))
    profile->assign(data, data + length);
  return true;
}

void ReadColorProfile(std::vector<char>* profile) {
  CGColorSpaceRef monitor_color_space(base::mac::GetSystemColorSpace());
  base::ScopedCFTypeRef<CFDataRef> icc_profile(
      CGColorSpaceCopyICCProfile(monitor_color_space));
  if (!icc_profile)
    return;
  size_t length = CFDataGetLength(icc_profile);
  if (gfx::InvalidColorProfileLength(length))
    return;
  const unsigned char* data = CFDataGetBytePtr(icc_profile);
  profile->assign(data, data + length);
}

}  // namespace gfx
