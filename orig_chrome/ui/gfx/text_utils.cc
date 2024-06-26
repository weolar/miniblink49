// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/text_utils.h"

#include <stdint.h>

#include "base/i18n/char_iterator.h"
#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "third_party/icu/source/common/unicode/uchar.h"
#include "third_party/icu/source/common/unicode/utf16.h"

namespace gfx {

namespace {

    // Returns true if the code point |c| is a combining mark character in Unicode.
    bool CharIsMark(UChar32 c)
    {
        int8_t char_type = u_charType(c);
        return char_type == U_NON_SPACING_MARK || char_type == U_ENCLOSING_MARK || char_type == U_COMBINING_SPACING_MARK;
    }

    // Gets the code point of |str| at the given code unit position |index|. If
    // |index| is a surrogate code unit, returns the whole code point (unless the
    // code unit is unpaired, in which case it just returns the surrogate value).
    UChar32 GetCodePointAt(const base::string16& str, size_t index)
    {
        UChar32 c;
        U16_GET(str.data(), 0, index, str.size(), c);
        return c;
    }

} // namespace

base::string16 RemoveAcceleratorChar(const base::string16& s,
    base::char16 accelerator_char,
    int* accelerated_char_pos,
    int* accelerated_char_span)
{
    bool escaped = false;
    ptrdiff_t last_char_pos = -1;
    int last_char_span = 0;
    base::i18n::UTF16CharIterator chars(&s);
    base::string16 accelerator_removed;

    accelerator_removed.reserve(s.size());
    while (!chars.end()) {
        int32_t c = chars.get();
        int array_pos = chars.array_pos();
        chars.Advance();

        if (c != accelerator_char || escaped) {
            int span = chars.array_pos() - array_pos;
            if (escaped && c != accelerator_char) {
                last_char_pos = accelerator_removed.size();
                last_char_span = span;
            }
            for (int i = 0; i < span; i++)
                accelerator_removed.push_back(s[array_pos + i]);
            escaped = false;
        } else {
            escaped = true;
        }
    }

    if (accelerated_char_pos)
        *accelerated_char_pos = last_char_pos;
    if (accelerated_char_span)
        *accelerated_char_span = last_char_span;

    return accelerator_removed;
}

size_t FindValidBoundaryBefore(const base::string16& text, size_t index)
{
    size_t length = text.length();
    DCHECK_LE(index, length);
    if (index == length)
        return index;

    // If |index| straddles a combining character sequence, go back until we find
    // a base character.
    while (index > 0 && CharIsMark(GetCodePointAt(text, index)))
        --index;

    // If |index| straddles a UTF-16 surrogate pair, go back.
    U16_SET_CP_START(text.data(), 0, index);
    return index;
}

size_t FindValidBoundaryAfter(const base::string16& text, size_t index)
{
    DCHECK_LE(index, text.length());
    if (index == text.length())
        return index;

    int32_t text_index = base::checked_cast<int32_t>(index);
    int32_t text_length = base::checked_cast<int32_t>(text.length());

    // If |index| straddles a combining character sequence, go forward until we
    // find a base character.
    while (text_index < text_length && CharIsMark(GetCodePointAt(text, text_index))) {
        ++text_index;
    }

    // If |index| straddles a UTF-16 surrogate pair, go forward.
    U16_SET_CP_LIMIT(text.data(), 0, text_index, text_length);
    return static_cast<size_t>(text_index);
}

} // namespace gfx
