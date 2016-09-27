// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutThemeLinux.h"

#include "platform/PlatformResourceLoader.h"

namespace blink {

PassRefPtr<LayoutTheme> LayoutThemeLinux::create()
{
    return adoptRef(new LayoutThemeLinux());
}

LayoutTheme& LayoutTheme::theme()
{
    DEFINE_STATIC_REF(LayoutTheme, layoutTheme, (LayoutThemeLinux::create()));
    return *layoutTheme;
}

String LayoutThemeLinux::extraDefaultStyleSheet()
{
    return LayoutThemeDefault::extraDefaultStyleSheet() +
        loadResourceAsASCIIString("themeChromiumLinux.css");
}

} // namespace blink
