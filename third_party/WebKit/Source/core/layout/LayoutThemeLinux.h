// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutThemeLinux_h
#define LayoutThemeLinux_h

#include "core/layout/LayoutThemeDefault.h"

namespace blink {

class LayoutThemeLinux final : public LayoutThemeDefault {
public:
    static PassRefPtr<LayoutTheme> create();
    virtual String extraDefaultStyleSheet() override;
};

} // namespace blink

#endif // LayoutThemeLinux_h
