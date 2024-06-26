// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/linux_font_delegate.h"

namespace {

gfx::LinuxFontDelegate* g_linux_font_delegate = 0;

} // namespace

namespace gfx {

void LinuxFontDelegate::SetInstance(LinuxFontDelegate* instance)
{
    g_linux_font_delegate = instance;
}

const LinuxFontDelegate* LinuxFontDelegate::instance()
{
    return g_linux_font_delegate;
}

} // namespace gfx
