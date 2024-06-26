// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_FONT_FALLBACK_WIN_H_
#define UI_GFX_FONT_FALLBACK_WIN_H_

#include <stddef.h>

#include <string>
#include <vector>

#include "base/macros.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_fallback.h"

namespace gfx {

// Internals of font_fallback_win.cc exposed for testing.
namespace internal {

    // Parses comma separated SystemLink |entry|, per the format described here:
    // http://msdn.microsoft.com/en-us/goglobal/bb688134.aspx
    //
    // Sets |filename| and |font_name| respectively. If a field is not present
    // or could not be parsed, the corresponding parameter will be cleared.
    void GFX_EXPORT ParseFontLinkEntry(const std::string& entry,
        std::string* filename,
        std::string* font_name);

    // Parses a font |family| in the format "FamilyFoo & FamilyBar (TrueType)".
    // Splits by '&' and strips off the trailing parenthesized expression.
    void GFX_EXPORT ParseFontFamilyString(const std::string& family,
        std::vector<std::string>* font_names);

    // Iterator over linked fallback fonts for a given font. The linked font chain
    // comes from the Windows registry, but gets cached between uses.
    class GFX_EXPORT LinkedFontsIterator {
    public:
        // Instantiates the iterator over the linked font chain for |font|. The first
        // item will be |font| itself.
        explicit LinkedFontsIterator(Font font);
        virtual ~LinkedFontsIterator();

        // Sets the font that would be returned by the next call to |NextFont()|,
        // useful for inserting one-time entries into the iterator chain.
        void SetNextFont(Font font);

        // Gets the next font in the link chain, if available, and increments the
        // iterator. Returns |true| on success or |false| if the iterator is past
        // last item (in that case, the value of |font| should not be used). If
        // |SetNextFont()| was called, returns the font set that way and clears it.
        bool NextFont(Font* font);

    protected:
        // Retrieves the list of linked fonts. Protected and virtual so that it may
        // be overridden by tests.
        virtual const std::vector<Font>* GetLinkedFonts() const;

    private:
        // Original font whose linked fonts are being iterated over.
        Font original_font_;

        // Font that was set via |SetNextFont()|.
        Font next_font_;

        // Indicates whether |SetNextFont()| was called.
        bool next_font_set_;

        // The font most recently returned by |NextFont()|.
        Font current_font_;

        // List of linked fonts; weak pointer.
        const std::vector<Font>* linked_fonts_;

        // Index of the current entry in the |linked_fonts_| list.
        size_t linked_font_index_;

        DISALLOW_COPY_AND_ASSIGN(LinkedFontsIterator);
    };

} // namespace internal

// Finds a fallback font to render the specified |text| with respect to an
// initial |font|. Returns the resulting font via out param |result|. Returns
// |true| if a fallback font was found.
bool GetUniscribeFallbackFont(const Font& font,
    const wchar_t* text,
    int text_length,
    Font* result);

} // namespace gfx

#endif // UI_GFX_FONT_FALLBACK_WIN_H_
