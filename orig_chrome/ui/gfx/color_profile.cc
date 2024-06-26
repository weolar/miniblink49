// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/color_profile.h"
#include "build/build_config.h"

namespace gfx {

#if defined(OS_WIN) || (defined(OS_MACOSX) && !defined(OS_IOS))
void ReadColorProfile(std::vector<char>* profile);
#else
void ReadColorProfile(std::vector<char>* profile)
{
}
GFX_EXPORT bool GetDisplayColorProfile(const gfx::Rect& bounds,
    std::vector<char>* profile)
{
    // TODO(port): consider screen color profile support.
    return false;
}
#endif

ColorProfile::ColorProfile()
{
    // TODO: support multiple monitors.
    ReadColorProfile(&profile_);
}

ColorProfile::~ColorProfile()
{
}

} // namespace gfx
