// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutThemeWin.h"

namespace blink {

PassRefPtr<LayoutTheme> LayoutThemeWin::create()
{
    return adoptRef(new LayoutThemeWin());
}

LayoutTheme& LayoutTheme::theme()
{
    DEFINE_STATIC_REF(LayoutTheme, layoutTheme, (LayoutThemeWin::create()));
    return *layoutTheme;
}

} // namespace blink
