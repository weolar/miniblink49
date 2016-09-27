// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutThemeWin_h
#define LayoutThemeWin_h

#include "core/layout/LayoutThemeDefault.h"

namespace blink {

class LayoutThemeWin final : public LayoutThemeDefault {
public:
    static PassRefPtr<LayoutTheme> create();
};

} // namespace blink

#endif // LayoutThemeWin_h
