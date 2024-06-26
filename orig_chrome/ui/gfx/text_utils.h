// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_TEXT_UTILS_H_
#define UI_GFX_TEXT_UTILS_H_

#include <stddef.h>

#include "base/strings/string16.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {

class FontList;

// Strip the accelerator char (typically '&') from a menu string.  A double
// accelerator char ('&&') will be converted to a single char.  The out params
// |accelerated_char_pos| and |accelerated_char_span| will be set to the index
// and span of the last accelerated character, respectively, or -1 and 0 if
// there was none.
GFX_EXPORT base::string16 RemoveAcceleratorChar(const base::string16& s,
    base::char16 accelerator_char,
    int* accelerated_char_pos,
    int* accelerated_char_span);

// Returns the number of horizontal pixels needed to display the specified
// |text| with |font_list|.
GFX_EXPORT int GetStringWidth(const base::string16& text,
    const FontList& font_list);

// This is same as GetStringWidth except that fractional width is returned.
GFX_EXPORT float GetStringWidthF(const base::string16& text,
    const FontList& font_list);

// Returns a valid cut boundary at or before |index|. The surrogate pair and
// combining characters should not be separated.
GFX_EXPORT size_t
FindValidBoundaryBefore(const base::string16& text, size_t index);

// Returns a valid cut boundary at or after |index|. The surrogate pair and
// combining characters should not be separated.
GFX_EXPORT size_t
FindValidBoundaryAfter(const base::string16& text, size_t index);

} // namespace gfx

#endif // UI_GFX_TEXT_UTILS_H_
