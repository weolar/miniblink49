// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/text_utils.h"

#include "ui/gfx/canvas.h"

namespace gfx {

int GetStringWidth(const base::string16& text, const FontList& font_list)
{
    return Canvas::GetStringWidth(text, font_list);
}

float GetStringWidthF(const base::string16& text, const FontList& font_list)
{
    return Canvas::GetStringWidthF(text, font_list);
}

} // namespace gfx
