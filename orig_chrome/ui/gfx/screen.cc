// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/screen.h"

#include "ui/gfx/geometry/rect.h"

namespace gfx {

namespace {

    Screen* g_screen;

} // namespace

Screen::Screen()
{
}

Screen::~Screen()
{
}

// static
Screen* Screen::GetScreen()
{
#if defined(OS_MACOSX) || defined(OS_ANDROID)
    // TODO(scottmg): https://crbug.com/558054
    if (!g_screen)
        g_screen = CreateNativeScreen();
#endif
    return g_screen;
}

// static
void Screen::SetScreenInstance(Screen* instance)
{
    g_screen = instance;
}

gfx::Rect Screen::ScreenToDIPRectInWindow(NativeView view,
    const gfx::Rect& screen_rect) const
{
    float scale = GetDisplayNearestWindow(view).device_scale_factor();
    return ScaleToEnclosingRect(screen_rect, 1.0f / scale);
}

gfx::Rect Screen::DIPToScreenRectInWindow(NativeView view,
    const gfx::Rect& dip_rect) const
{
    float scale = GetDisplayNearestWindow(view).device_scale_factor();
    return ScaleToEnclosingRect(dip_rect, scale);
}

} // namespace gfx
